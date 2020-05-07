#pragma once
#include <string>
#include <vector>
#include "rendering/Mesh.h"

struct aiNode;
struct aiMesh;
struct aiScene;

namespace nds_se
{
	class Model
	{
	private:
		std::string m_filePath;
		std::vector<Mesh> m_meshes;

	public:
		Model(const std::string& filePath);
		~Model();

		void load();
		bool equalsResource(const Model& other) const;

		void render();

		const std::vector<Mesh>& getMeshes() const { return m_meshes; }

	private:
		void processAiNode(const aiNode* node, const aiScene* scene);
		void processAiMesh(const aiMesh* mesh, const aiScene* scene);
	};
}