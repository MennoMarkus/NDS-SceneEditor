#pragma once
#include "glm/glm.hpp"
#include "services/resourceAllocator/ResourceID.h"
#include "rendering/Camera.h"
#include "rendering/Model.h"
#include "rendering/ShaderProgram.h"
#include "services/Service.h"

namespace nds_se
{
	class Renderer : public Service
	{
	private:
		glm::mat4 m_viewProjection;
		ResourceID<ShaderProgram> m_currentShader;

	public:
		Renderer() = default;
		
		void setViewProjectionMatrix(const glm::mat4& value) { m_viewProjection = value; }
		void setShaderProgram(ResourceID<ShaderProgram> shaderProgram) { m_currentShader = shaderProgram; }

		void drawModel(const ResourceID<Model> modelID, glm::mat4 transform);
	};
}