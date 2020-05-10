#pragma once
#include "screen/window/Window.h"
#include "rendering/Renderer.h"
#include "rendering/Camera.h"

#include "services/resourceAllocator/ResourceAllocator.h"
#include "rendering/Texture.h"
#include "rendering/Model.h"
#include "rendering/ShaderProgram.h"

#include "screen/editor/Viewport.h"
#include "screen/editor/LogView.h"

#include "ecs/EntityManager.h"

namespace nds_se
{
	class Application
	{
	private:
		Window m_window;
		Renderer m_renderer;
		Camera m_camera;

		Viewport m_viewportUI;
		LogView m_logViewUI;
		LogCallbackID m_logViewUICallbackID;

		ResourceAllocator<Texture> m_textureManager;
		ResourceAllocator<Model> m_modelManager;
		ResourceAllocator<ShaderProgram> m_shaderManager;

		EntityManager m_entityManager;

	public:
		Application();
		~Application();

		void run();
	};
}