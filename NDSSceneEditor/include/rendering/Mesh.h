#pragma once
#include <vector>
#include "rendering/Primative.h"
#include "rendering/Vertex.h"
#include "rendering/Texture.h"
#include "services/resourceAllocator/ResourceID.h"

namespace nds_se
{
	class Mesh
	{
	private:
		PrimativeType m_primativeType;
		unsigned int m_vertexArrayID = 0;

		unsigned int m_vertexBufferID = 0;
		std::vector<Vertex> m_vertices;

		unsigned int m_indexBufferID = 0;
		std::vector<unsigned int> m_indices;

		ResourceID<Texture> m_textureID;

	public:
		Mesh(PrimativeType type, const std::vector<Vertex>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, ResourceID<Texture> textureID = ResourceID<Texture>::INVALID);
		void destroy();

		void render();

		const std::vector<Vertex>& getVertices() const { return m_vertices; }
		const std::vector<unsigned int>& getIndices() const { return m_indices; }
		ResourceID<Texture> getTextureID() const { return m_textureID; }
	};
}