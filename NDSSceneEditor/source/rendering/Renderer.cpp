#include "rendering/Renderer.h"

#include "glad/glad.h"

#include "services/resourceAllocator/ResourceAllocator.h"

namespace nds_se
{
	void Renderer::drawModel(const ResourceID<Model> modelID, glm::mat4 transform)
	{
		// Set model view projection matrix
		ResourceAllocator<ShaderProgram>* shaderManager = ServiceLocator::get<ResourceAllocator<ShaderProgram>>();
		ShaderProgram& shader = shaderManager->getResource(m_currentShader);
		shader.bind();
		shader.setUniform("mvp", m_viewProjection * transform);

		ResourceAllocator<Model>* modelManager = ServiceLocator::get<ResourceAllocator<Model>>();
		Model& model = modelManager->getResource(modelID);
		model.render();
	}
}