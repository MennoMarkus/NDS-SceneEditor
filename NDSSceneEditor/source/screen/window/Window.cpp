#include "screen/window/Window.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imguizmo/ImGuizmo.h"

#include "services/Logger.h"

namespace nds_se
{
	Window::Window(int width, int height, bool fullscreen, const std::string& title)
	{
		//Initialize GLFW
		glfwSetErrorCallback(errorCallback);
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//Create the window
		m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
		if (!m_window)
		{
			glfwTerminate();
			LOG(LOG_FATAL, "Failed to create GLFW window.");
		}
		glfwMakeContextCurrent(m_window);

		//Initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LOG(LOG_FATAL, "Failed to initialize GLAD.");
		}
		glfwSwapInterval(1);

		//Set the viewport
		glViewport(0, 0, width, height);

		//Set the callback function for the framebuffer resizing
		glfwSetFramebufferSizeCallback(m_window, resizeCallback);

		// Setup input
		m_input = std::unique_ptr<WindowInput>(new WindowInput(m_window));

		glfwSetKeyCallback(m_window, WindowInput::keyboardCallback);
		glfwSetMouseButtonCallback(m_window, WindowInput::mouseButtonCallback);
		glfwSetCursorPosCallback(m_window, WindowInput::mouseCursorPositionCallback);
		glfwSetCursorEnterCallback(m_window, WindowInput::mouseCursorEnterCallback);
		glfwSetScrollCallback(m_window, WindowInput::mouseScrollCallback);

		// OpenGL settings
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); // Same as NDS
		glCullFace(GL_BACK);

		//Set up imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = "resources/Imgui.ini";
		io.DisplaySize = ImVec2((float)width, (float)height);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init("#version 430 core");

		//Set up imguizmo
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}

	Window::~Window()
	{
		// Cleanup ImGui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		// Cleanup 
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Window::beginRender()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void Window::endRender()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_window);
		m_input->pollInput();
	}

	bool Window::shouldClose() const
	{
		return glfwWindowShouldClose(m_window);
	}

	glm::ivec2 Window::getSize() const
	{
		glm::ivec2 size;
		glfwGetFramebufferSize(m_window, &size.x, &size.y);
		return size;
	}

	void Window::resizeCallback(GLFWwindow* window, int width, int height)
	{
		//Resize viewport of default framebuffer
		glViewport(0, 0, width, height);

		//Resize imgui screen size data
		ImGuiIO& io = ImGui::GetIO();// (void)io;
		io.DisplaySize = ImVec2((float)width, (float)height);
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}

	void Window::errorCallback(int error, const char* description)
	{
		LOG(LOG_ERROR, "[GLFW Error " << error << "] " << description);
	}
}