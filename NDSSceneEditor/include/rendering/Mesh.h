#pragma once
#include <vector>
#include "rendering/Vertex.h"
#include "rendering/Texture.h"
#include "services/resourceAllocator/ResourceID.h"

namespace nds_se
{
	enum MeshType
	{
		TRIANGLES = 0x0004, // OpenGL value
		TRIANGLE_STRIP = 0x0005
	};

	using Vertices = std::vector<Vertex>;
	using Indices = std::vector<unsigned int>;

	class Mesh
	{
	private:
		MeshType m_meshType;
		unsigned int m_vertexArrayID = 0;

		unsigned int m_vertexBufferID = 0;
		Vertices m_vertices;

		unsigned int m_indexBufferID = 0;
		Indices m_indices;

		ResourceID<Texture> m_textureID;

	public:
		Mesh(MeshType type, const Vertices& vertices, const Indices& indices, ResourceID<Texture> textureID = ResourceID<Texture>::INVALID);
		void destroy();

		void render();

		const Vertices& getVertices() const { return m_vertices; }
		const Indices& getIndices() const { return m_indices; }
		ResourceID<Texture> getTextureID() const { return m_textureID; }
	};
}