#pragma once
#include <string>
#include <memory>

#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "screen/window/WindowInput.h"

namespace nds_se
{
	class Window
	{
		friend class InputManager;

	private:
		GLFWwindow* m_window = nullptr;
		std::unique_ptr<WindowInput> m_input = nullptr;

	public:
		Window(int width, int height, bool fullscreen, const std::string& title);
		Window& operator=(const Window&) = delete; // Prevent pointers being deleted more than once.
		Window(const Window&) = delete;
		~Window();

		void beginRender();
		void endRender();

		bool shouldClose() const;
		glm::ivec2 getSize() const;
		WindowInput& getInput() { return *m_input; }

	private:
		static void resizeCallback(GLFWwindow* window, int width, int height);
		static void errorCallback(int error, const char* description);
	};
}