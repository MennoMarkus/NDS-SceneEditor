#include "screen/window/WindowInput.h"

#include <algorithm>

namespace nds_se
{
	std::unordered_set<WindowInput*> WindowInput::m_inputListeners;

	WindowInput::WindowInput(GLFWwindow* window) :
		m_window(window)
	{
		// Add input listener
		m_inputListeners.insert(this);

		// Init mouse position
		glfwGetCursorPos(m_window, (double*)&m_mousePosition.x, (double*)&m_mousePosition.y);
	}

	WindowInput::~WindowInput()
	{
		// Remove this input listener
		for (auto it = std::begin(m_inputListeners); it != std::end(m_inputListeners); ++it)
		{
			if (*it == this)
			{
				m_inputListeners.erase(it);
				break;
			}
		}
	}

	void WindowInput::pollInput()
	{
		// Register previous input state
		std::copy(std::begin(m_currentKeyboardState), std::end(m_currentKeyboardState), std::begin(m_previousKeyboardState));

		std::copy(std::begin(m_currentMouseButtonState), std::end(m_currentMouseButtonState), std::begin(m_previousMouseButtonState));

		m_previousMouseWheelOffset = m_currentMouseWheelOffset;
		m_currentMouseWheelOffset = 0;

		// Poll new events
		glfwPollEvents();
	}

	bool WindowInput::isPressed(KeyboardKey key)
	{
		if (m_previousKeyboardState[key] == 0 && m_currentKeyboardState[key] == 1)
			return true;
		return false;
	}

	bool WindowInput::isDown(KeyboardKey key)
	{
		if (m_currentKeyboardState[key] == 1)
			return true;
		return false;
	}

	bool WindowInput::isReleased(KeyboardKey key)
	{
		if (m_previousKeyboardState[key] == 1 && m_currentKeyboardState[key] == 0)
			return true;
		return false;
	}

	bool WindowInput::isUp(KeyboardKey key)
	{
		if (m_currentKeyboardState[key] == 0)
			return true;
		return false;
	}

	bool WindowInput::isPressed(MouseButton button)
	{
		if (m_previousMouseButtonState[button] == 0 && m_currentMouseButtonState[button] == 1)
			return true;
		return false;
	}

	bool WindowInput::isDown(MouseButton button)
	{
		if (m_currentMouseButtonState[button] == 1)
			return true;
		return false;
	}

	bool WindowInput::isReleased(MouseButton button)
	{
		if (m_previousMouseButtonState[button] == 1 && m_currentMouseButtonState[button] == 0)
			return true;
		return false;
	}

	bool WindowInput::isUp(MouseButton button)
	{
		if (m_currentMouseButtonState[button] == 0)
			return true;
		return false;
	}

	void WindowInput::setMousePosition(glm::vec2 position)
	{
		glfwSetCursorPos(m_window, position.x, position.y);
		m_mousePosition = position;
	}

	void WindowInput::setMouseCursorVisibility(bool isVisible)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, isVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		m_isMouseCursorHidden = !isVisible;
	}

	void WindowInput::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		for (auto& inputListener : m_inputListeners)
		{
			if (inputListener->m_window == window)
			{
				inputListener->m_currentKeyboardState[key] = (char)(action != GLFW_RELEASE);
				break;
			}
		}
	}

	void WindowInput::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		for (auto& inputListener : m_inputListeners)
		{
			if (inputListener->m_window == window)
			{
				inputListener->m_currentMouseButtonState[button] = (char)action;
				break;
			}
		}
	}

	void WindowInput::mouseCursorPositionCallback(GLFWwindow* window, double x, double y)
	{
		for (auto& inputListener : m_inputListeners)
		{
			if (inputListener->m_window == window)
			{
				inputListener->m_mousePosition = glm::vec2((float)x, (float)y);
				break;
			}
		}
	}

	void WindowInput::mouseCursorEnterCallback(GLFWwindow * window, int enter)
	{
		for (auto& inputListener : m_inputListeners)
		{
			if (inputListener->m_window == window)
			{
				inputListener->m_isMouseCursorOnScreen = (bool)enter;
				break;
			}
		}
	}

	void WindowInput::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		for (auto& inputListener : m_inputListeners)
		{
			if (inputListener->m_window == window)
			{
				inputListener->m_currentMouseWheelOffset = (float)yoffset;
				break;
			}
		}
	}
}