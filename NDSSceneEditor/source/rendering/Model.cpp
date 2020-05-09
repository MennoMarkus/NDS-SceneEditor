#include "rendering/Model.h"

#include <filesystem>

#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"

#include "services/Logger.h"
#include "services/resourceAllocator/ResourceAllocator.h"
#include "rendering/Texture.h"
#include "triangleStripper/TriangleStripper.h"

namespace nds_se
{
	Model::Model(const std::string& filePath) :
		m_filePath(filePath)
	{}

	Model::~Model()
	{
		for (auto& mesh : m_meshes)
			mesh.destroy();
	}

	void Model::load()
	{
		// Open and process model file
		const unsigned int importerFlags = aiProcess_JoinIdenticalVertices |
											aiProcess_Triangulate |
											aiProcess_FlipUVs |
											aiProcess_LimitBoneWeights |
											aiProcess_ValidateDataStructure |
											aiProcess_SortByPType |
											aiProcess_OptimizeMeshes |
											aiProcess_OptimizeGraph |
											aiProcess_RemoveRedundantMaterials |
											aiProcess_FindInvalidData |
											aiProcess_Debone |
											aiProcess_RemoveComponent;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(m_filePath, importerFlags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG(LOG_ERROR, "[ASSIMP]" << importer.GetErrorString());
			return;
		}

		processAiNode(scene->mRootNode, scene);
	}

	bool Model::equalsResource(const Model & other) const
	{
		return m_filePath == other.m_filePath;
	}

	void Model::render()
	{
		for (auto& mesh : m_meshes)
			mesh.render();
	}

	void Model::processAiNode(const aiNode* node, const aiScene* scene)
	{
		// Process meshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processAiMesh(mesh, scene);
		}

		// Process node child meshes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processAiNode(node->mChildren[i], scene);
		}
	}

	void Model::processAiMesh(const aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		ResourceID<Texture> textureID;
		
		// Get vertices
		vertices.reserve(mesh->mNumVertices);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			// position
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;

			// normal
			if (mesh->HasNormals())
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			// texture coordinate
			if (mesh->HasTextureCoords(0))
			{
				vertex.textureCoordinate.x = mesh->mTextureCoords[0][i].x;
				vertex.textureCoordinate.y = mesh->mTextureCoords[0][i].y;
			}

			// color
			if (mesh->HasVertexColors(0))
			{
				vertex.color.r = mesh->mColors[0][i].r;
				vertex.color.g = mesh->mColors[0][i].g;
				vertex.color.b = mesh->mColors[0][i].b;
				vertex.color.a = mesh->mColors[0][i].a;
			}
			else
			{
				vertex.color = glm::vec4(1.0f);
			}

			vertices.push_back(vertex);
		}

		// Get indices
		indices.reserve((size_t)mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			for (int j = 0; j < 3; j++)
				indices.push_back(mesh->mFaces[i].mIndices[j]);
		}

		// Get texture
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString texturePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

			// If the texture file does not excist at the given location, search for it in the models parent folder.
			if (!std::filesystem::exists(texturePath.C_Str()))
			{
				std::string fileName = std::filesystem::path(texturePath.C_Str()).filename().generic_string();
				texturePath = std::filesystem::path(m_filePath).parent_path().generic_string();
				texturePath.Append("/");
				texturePath.Append(fileName.c_str());
			}

			ResourceAllocator<Texture>* textureManager = ServiceLocator::get<ResourceAllocator<Texture>>();
			Texture& texture = textureManager->getOrLoadResource(&textureID, texturePath.C_Str(), true);

			// Textures can be rescaled due to compression, so we need to adjust texture coordinates.
			for (auto& vertex : vertices)
			{
				vertex.textureCoordinate *= texture.getTextureCoordinateScaleFactor();
				vertex.textureCoordinate += glm::vec2(1.0f) - texture.getTextureCoordinateScaleFactor();
			}
		}

		// Post process indices into triangle strips
		std::vector<Primative> primatives;
		TriangleStripper triangleStripper(indices);
		triangleStripper.strip(primatives);
		
		for (auto& primative : primatives)
		{
			// TODO: separte out vertices
			m_meshes.emplace_back(primative.m_type, vertices, primative.m_indices, textureID);
		}
	}
}