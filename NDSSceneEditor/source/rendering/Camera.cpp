#include "rendering/Camera.h"

#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "screen/window/Window.h"
#include "services/Logger.h"

namespace nds_se
{
	Camera::Camera()
	{
		initializeCamera();
	}

	Camera::Camera(glm::vec3 position, glm::vec3 lookAtTarget, glm::vec3 up) :
		m_position(position), 
		m_lookAtTarget(lookAtTarget),
		m_up(up)
	{
		initializeCamera();
	}

	void Camera::updateCameraMovement(Window& window, float deltaTime)
	{
		WindowInput& input = window.getInput();

		// Toggle scene movement
		if (input.isPressed(MOUSE_RIGHT_BUTTON))
		{
			m_hasMovementFocus = true;
			input.setMouseCursorVisibility(false);
			input.setMousePosition(window.getSize() / 2);
		}
		else if (input.isReleased(MOUSE_RIGHT_BUTTON))
		{
			m_hasMovementFocus = false;
			input.setMouseCursorVisibility(true);
		}

		if (m_hasMovementFocus)
		{
			// Camera movement speed calculation
			m_movementSpeed = glm::clamp(m_movementSpeed - input.getMouseWheelOffset() * deltaTime, 1.0f, 40.0f);

			// Camera movement calculation
			m_position.x += (sinf(m_angle.x) * input.isDown(KEY_S) -
							 sinf(m_angle.x) * input.isDown(KEY_W) -
							 cosf(m_angle.x) * input.isDown(KEY_A) +
							 cosf(m_angle.x) * input.isDown(KEY_D)) * m_movementSpeed * deltaTime;

			m_position.z += (cosf(m_angle.x) * input.isDown(KEY_S) -
							 cosf(m_angle.x) * input.isDown(KEY_W) +
							 sinf(m_angle.x) * input.isDown(KEY_A) -
							 sinf(m_angle.x) * input.isDown(KEY_D)) * m_movementSpeed * deltaTime;

			m_position.y += (sinf(m_angle.y) * input.isDown(KEY_W) -
							 sinf(m_angle.y) * input.isDown(KEY_S) +
							 1.0f * input.isDown(KEY_E) - 
							 1.0f * input.isDown(KEY_Q)) * m_movementSpeed * deltaTime;

			// Camera orientation calculation
			glm::vec2 mousePosition = input.getMousePosition();
			m_angle += ((glm::vec2)window.getSize() / 2.0f - mousePosition) * m_rotationSpeed * deltaTime;

			input.setMousePosition(window.getSize() / 2);

			// Angle clamp
			if		(m_angle.y >  glm::radians(m_angleClamp)) m_angle.y =  glm::radians(m_angleClamp);
			else if (m_angle.y < -glm::radians(m_angleClamp)) m_angle.y = -glm::radians(m_angleClamp);

			// Recalculate camera target considering translation and rotation
			glm::mat4 translation = glm::translate(glm::mat4(1), glm::vec3(0, 0, m_targetDistance / m_panningDivider));
			glm::mat4 rotation = glm::eulerAngleXYZ(2.0f * glm::pi<float>() - m_angle.y, 2.0f * glm::pi<float>() - m_angle.x, 0.0f);
			glm::mat4 transform = glm::transpose(translation) * rotation;
			transform = glm::transpose(transform);

			m_lookAtTarget.x = m_position.x - transform[3][0];
			m_lookAtTarget.y = m_position.y - transform[3][1];
			m_lookAtTarget.z = m_position.z - transform[3][2];
		}
	}

	glm::mat4 Camera::getViewMatrix() const
	{
		return glm::lookAt(m_position, m_lookAtTarget, m_up);
	}

	glm::mat4 Camera::getProjectionMatrix(glm::vec2 screenSize) const
	{
		return glm::perspective(glm::radians(M_FOV_Y), screenSize.x / screenSize.y, M_NEAR_Z, M_FAR_Z);
	}

	glm::mat4 Camera::getViewProjectionMatrix(glm::vec2 screenSize) const
	{
		return getProjectionMatrix(screenSize) * getViewMatrix();
	}

	glm::vec2 Camera::getWorldToScreenPosition(glm::vec3 worldPosition, glm::vec2 screenSize, bool* o_isOnScreen)
	{
		glm::mat4 viewProjMat = getViewProjectionMatrix(screenSize);
		glm::quat worldPositionQuat{ worldPosition.x, worldPosition.y, worldPosition.z, 1.0f };
		glm::quat temp;

		// transform position to clip space
		temp.x = viewProjMat[0][0] * worldPositionQuat.x + viewProjMat[1][0] * worldPositionQuat.y + viewProjMat[2][0] * worldPositionQuat.z + viewProjMat[3][0] * worldPositionQuat.w;
		temp.y = viewProjMat[0][1] * worldPositionQuat.x + viewProjMat[1][1] * worldPositionQuat.y + viewProjMat[2][1] * worldPositionQuat.z + viewProjMat[3][1] * worldPositionQuat.w;
		temp.z = viewProjMat[0][2] * worldPositionQuat.x + viewProjMat[1][2] * worldPositionQuat.y + viewProjMat[2][2] * worldPositionQuat.z + viewProjMat[3][2] * worldPositionQuat.w;
		temp.w = viewProjMat[0][3] * worldPositionQuat.x + viewProjMat[1][3] * worldPositionQuat.y + viewProjMat[2][3] * worldPositionQuat.z + viewProjMat[3][3] * worldPositionQuat.w;

		glm::vec3 normalizedDeviceCoordinates = { temp.x / temp.w, -temp.y / temp.w, temp.z / temp.w };

		// Calculate 2d screen position 
		glm::vec2 screenPosition = { (normalizedDeviceCoordinates.x + 1.0f) / 2.0f * (float)screenSize.x,
									 (normalizedDeviceCoordinates.y + 1.0f) / 2.0f * (float)screenSize.y };

		// Check screen bounds
		if (o_isOnScreen != nullptr)
		{
			*o_isOnScreen = screenPosition.x > 0 && screenPosition.x < screenSize.x &&
							screenPosition.y > 0 && screenPosition.y < screenSize.y;
		}

		return screenPosition;
	}

	void Camera::initializeCamera()
	{
		glm::vec3 v1 = m_position;
		glm::vec3 v2 = m_lookAtTarget;

		float dx = v2.x - v1.x;
		float dy = v2.y - v1.y;
		float dz = v2.z - v1.z;

		m_targetDistance = sqrtf(dx * dx + dy * dy + dz * dz);
		m_angle.x = atan2f(dx, dz) - glm::pi<float>();
		m_angle.y = atan2f(dy, sqrtf(dx * dx + dz * dz));
	}
}