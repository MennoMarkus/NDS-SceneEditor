#include "Application.h"

#include "glm/gtc/matrix_transform.hpp"
#include "services/ServiceLocator.h"

#include "ecs/ComponentDataArray.h"
#include "ecs/SystemBase.h"

namespace nds_se
{
	Application::Application() : 
		m_window(1600, 900, false, "NDS Scene Editor"),
		m_camera(glm::vec3(4, 3, 3), glm::vec3(0), glm::vec3(0, 1, 0))
	{
		// Setup logger UI
		m_logViewUICallbackID = LOGGER->registerCallback(std::bind(&LogView::log, &m_logViewUI, std::placeholders::_1));

		// Setup resource managers
		ServiceLocator::get().provideService<ResourceAllocator<Texture>>(&m_textureManager);
		ServiceLocator::get().provideService<ResourceAllocator<ShaderProgram>>(&m_shaderManager);
		ServiceLocator::get().provideService<ResourceAllocator<Model>>(&m_modelManager);

		// Setup rendering
		m_renderer.setViewProjectionMatrix(m_camera.getViewProjectionMatrix(m_window.getSize()));

		ResourceID<ShaderProgram> shader = m_shaderManager.loadResource("resources/shaders/SimpleShd.glsl.vs", "resources/shaders/SimpleShd.glsl.fs");
		m_renderer.setShaderProgram(shader);

		// Load default texture
		m_textureManager.loadResource("resources/textures/WhiteTex.png");

		// Setup entities
		ServiceLocator::get().provideService<EntityManager>(&m_entityManager);
	}

	Application::~Application()
	{
		// Remove logger UI
		LOGGER->unregisterCallback(m_logViewUICallbackID);
	}

	void Application::run()
	{
		ResourceID<Model> model = m_modelManager.loadResource("resources/models/boxroom/Boxroom.obj");
		glm::vec3 modelTranslation = { 0.0f, 0.0f, 0.0f };

		float currentTime = (float)glfwGetTime();
		float lastTime = currentTime;

		// ECS TEST CODE
		std::unique_ptr<ISystemBase> systemPtr(new SystemBase<int, float>("System",
			[](Entity entity, int& t1, float& t2) 
			{
				LOG(LOG_DEBUG, "System reports entity " << entity << " with values " << t1 << " and " << t2);
			}
		));
		m_entityManager.registerSystem(std::move(systemPtr));

		std::unique_ptr<ISystemBase> system2Ptr(new SystemBase<>("System2", 
			[](Entity entity)
			{
				LOG(LOG_DEBUG, "System2 reports entity " << entity);
			}
		));
		m_entityManager.registerSystem(std::move(system2Ptr));
		
		LOG(LOG_DEBUG, "Created entity " << m_entityManager.createEntity());
		LOG(LOG_DEBUG, "Created entity " << m_entityManager.createEntity());
		LOG(LOG_DEBUG, "Created entity " << m_entityManager.createEntity());
		m_entityManager.destroyEntity(1);
		LOG(LOG_DEBUG, "Destroyed entity 1");
		LOG(LOG_DEBUG, "Created entity " << m_entityManager.createEntity());
		LOG(LOG_DEBUG, "Created entity " << m_entityManager.createEntity());

		m_entityManager.setComponent<int>(1, 1);
		LOG(LOG_DEBUG, "Entity 1, set component<int> to { 1 }");
		m_entityManager.setComponent<float>(1, 1.0f);
		LOG(LOG_DEBUG, "Entity 1, set component<float> to { 1.0f }");
		m_entityManager.setComponent<int>(2, 2);
		LOG(LOG_DEBUG, "Entity 2, set component<int> to { 2 }");
		m_entityManager.setComponent<int>(3, 3);
		LOG(LOG_DEBUG, "Entity 3, set component<int> to { 3 }");
		m_entityManager.removeComponent<int>(2);
		LOG(LOG_DEBUG, "Entity 2, removed component<int>");
		LOG(LOG_DEBUG, "Entity 3, returns component<int> data " << m_entityManager.getComponent<int>(3));

		m_entityManager.update();
		//

		while (!m_window.shouldClose())
		{
			// Update
			lastTime = currentTime;
			currentTime = (float)glfwGetTime();
			m_camera.updateCameraMovement(m_window, currentTime - lastTime);

			// Render
			m_window.beginRender();

			// Render models
			m_renderer.setViewProjectionMatrix(m_camera.getViewProjectionMatrix(m_window.getSize()));
			m_renderer.drawModel(model, glm::translate(glm::mat4(1), modelTranslation));

			// Render editor
			m_viewportUI.render();
			m_logViewUI.render();
			m_sceneViewUI.render();

			m_window.endRender();
		}
	}
}