#pragma once
#include "glm/glm.hpp"

#include "services/Service.h"

namespace nds_se
{
	class Window;

	class Camera : public Service
	{
	private:
		glm::vec3 m_position = { 0, 0, 0 };
		glm::vec3 m_lookAtTarget = { 1, 0, 0 };
		glm::vec3 m_up = { 0, 1, 0 };

		const float M_FOV_Y = 70.0f; // Same fov as the NDS
		const float M_NEAR_Z = 0.1f; // Same near z as the NDS
		const float M_FAR_Z = 1000.0f; // Same far z as the NDS

		float m_targetDistance;
		glm::vec2 m_angle;

		// Movement controls
		bool m_hasMovementFocus = false;
		float m_movementSpeed = 20.0f;
		float m_rotationSpeed = 0.12f;
		float m_angleClamp = 89.0f;
		float m_panningDivider = 5.1f;

	public:
		Camera();
		Camera(glm::vec3 position, glm::vec3 lookAtTarget, glm::vec3 up);

		void updateCameraMovement(Window& window, float deltaTime);

		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix(glm::vec2 screenSize) const;
		glm::mat4 getViewProjectionMatrix(glm::vec2 screenSize) const;

		glm::vec2 getWorldToScreenPosition(glm::vec3 worldPosition, glm::vec2 screenSize, bool* o_isOnScreen);

	private:
		void initializeCamera();
	};
}