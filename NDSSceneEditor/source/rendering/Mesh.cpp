#include "rendering/Mesh.h"

#include "glad/glad.h"

#include "services/ServiceLocator.h"
#include "services/resourceAllocator/ResourceAllocator.h"
#include "rendering/ShaderProgram.h"
#include "glm/gtc/random.hpp"

namespace nds_se
{
	Mesh::Mesh(PrimativeType type, const std::vector<Vertex>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, ResourceID<Texture> textureID) :
		m_vertices(vertexBuffer),
		m_indices(indexBuffer),
		m_textureID(textureID),
		m_primativeType(type)
	{
		// Generate vertex buffer array
		glGenVertexArrays(1, &m_vertexArrayID);
		glBindVertexArray(m_vertexArrayID);

		// Generate vertex buffer
		glGenBuffers(1, &m_vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

		// Set vertex buffer layout
		glEnableVertexAttribArray(0); // Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);

		glEnableVertexAttribArray(1); // Normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));

		glEnableVertexAttribArray(2); // Texture coordinate
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, textureCoordinate));

		glEnableVertexAttribArray(3); // Color
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

		// Generate index buffer
		if (m_indices.size() > 0)
		{
			glGenBuffers(1, &m_indexBufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
		}
		else
		{
			m_indexBufferID = 0;
		}

		// Unbind 
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Mesh::destroy()
	{
		if (m_indexBufferID != 0)
			glDeleteBuffers(1, &m_indexBufferID);

		if (m_vertexArrayID != 0)
		{
			glDeleteBuffers(1, &m_vertexBufferID);
			glDeleteVertexArrays(1, &m_vertexArrayID);
		}
	}

	void Mesh::render()
	{
		// Bind the models texture if the model has one.
		ResourceAllocator<Texture>* textureManager = ServiceLocator::get<ResourceAllocator<Texture>>();
		if (m_textureID != ResourceID<Texture>::INVALID)
		{
			Texture& texture = textureManager->getResource(m_textureID);
			texture.bind();
		}
		else
		{
			Texture& texture = textureManager->getOrLoadResource(nullptr, "resources/textures/WhiteTex.png", nullptr);
			texture.bind();
		}

		//DEBUG
		auto shaderManager = ServiceLocator::get<ResourceAllocator<ShaderProgram>>();
		srand(m_vertexArrayID);
		shaderManager->getResource(0).setUniform("meshCol", glm::vec4(glm::linearRand(0.0, 1.0), glm::linearRand(0.0, 1.0), glm::linearRand(0.0, 1.0), 1.0f));

		// Draw the mesh
		glBindVertexArray(m_vertexArrayID);
		if (m_indexBufferID == 0)
			glDrawArrays((GLenum)m_primativeType, 0, (GLsizei)m_vertices.size());
		else
			glDrawElements((GLenum)m_primativeType, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}