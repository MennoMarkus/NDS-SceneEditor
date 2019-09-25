#include "ObjLoader.h"
#include <iostream>
#include <algorithm>
#define TINYOBJLOADER_IMPLEMENTATION
#include "TinyObjLoader/tiny_obj_loader.h"

bool ObjLoader::load(const std::string& file, std::vector<Model>& o_models)
{
	// Get directory path
	std::string inputDir = "";
	size_t lastSlashPos = file.find_last_of("\\/");
	inputDir = (std::string::npos == lastSlashPos) ? "" : file.substr(0, lastSlashPos + 1);

	// Load obj file data
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string errorMessage;
	bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &errorMessage, file.c_str(), inputDir.c_str());

	// Check for errors
	if (!errorMessage.empty() || !success)
	{
		std::cerr << errorMessage << std::endl;
		return false;
	}

	// Add default material
	materials.push_back(tinyobj::material_t());

	// Convert shapes
	for (int s = 0; s < shapes.size(); s++)
	{
		Model model;

		for (int f = 0; f < shapes[s].mesh.indices.size() / 3; f++)
		{
			tinyobj::index_t index0 = shapes[s].mesh.indices[3 * f + 0];
			tinyobj::index_t index1 = shapes[s].mesh.indices[3 * f + 1];
			tinyobj::index_t index2 = shapes[s].mesh.indices[3 * f + 2];

			// Get material
			bool isDefaultMaterial = false;
			int materialId = shapes[s].mesh.material_ids[f];
			if (materialId < 0 || materialId >= materials.size())
			{
				materialId = materials.size() - 1;
				isDefaultMaterial = true;
			}

			// --- Get material
			float diffuseColor[3];
			float ambientColor[3];
			float specularColor[3];
			float shininess;
			float emissionColor[3];
			float alpha;
			if (!isDefaultMaterial)
			{
				diffuseColor[0] = materials[materialId].diffuse[0];
				diffuseColor[1] = materials[materialId].diffuse[1];
				diffuseColor[2] = materials[materialId].diffuse[2];

				ambientColor[0] = materials[materialId].ambient[0];
				ambientColor[1] = materials[materialId].ambient[1];
				ambientColor[2] = materials[materialId].ambient[2];

				specularColor[0] = materials[materialId].specular[0];
				specularColor[1] = materials[materialId].specular[1];
				specularColor[2] = materials[materialId].specular[2];

				shininess = materials[materialId].shininess;

				emissionColor[0] = materials[materialId].emission[0];
				emissionColor[1] = materials[materialId].emission[1];
				emissionColor[2] = materials[materialId].emission[2];

				alpha = materials[materialId].dissolve;
			}
			else
			{
				diffuseColor[0] = -1.f;
				diffuseColor[1] = -1.f;
				diffuseColor[2] = -1.f;

				ambientColor[0] = -1.f;
				ambientColor[1] = -1.f;
				ambientColor[2] = -1.f;

				specularColor[0] = -1.f;
				specularColor[1] = -1.f;
				specularColor[2] = -1.f;

				shininess = -1.f;

				emissionColor[0] = -1.f;
				emissionColor[1] = -1.f;
				emissionColor[2] = -1.f;

				alpha = -1.f;
			}

			// --- Get texture coordinates
			float textureCoordinates[3][2];
			if (attributes.texcoords.size() > 0 &&
				index0.texcoord_index >= 0 && index1.texcoord_index >= 0 && index2.texcoord_index >= 0)
			{
				assert(attributes.texcoords.size() > 2 * index0.texcoord_index + 1);
				assert(attributes.texcoords.size() > 2 * index1.texcoord_index + 1);
				assert(attributes.texcoords.size() > 2 * index2.texcoord_index + 1);

				// Valid uv's
				textureCoordinates[0][0] = attributes.texcoords[2 * index0.texcoord_index];
				textureCoordinates[0][1] = attributes.texcoords[2 * index0.texcoord_index + 1];
				textureCoordinates[1][0] = attributes.texcoords[2 * index1.texcoord_index];
				textureCoordinates[1][1] = attributes.texcoords[2 * index1.texcoord_index + 1];
				textureCoordinates[2][0] = attributes.texcoords[2 * index2.texcoord_index];
				textureCoordinates[2][1] = attributes.texcoords[2 * index2.texcoord_index + 1];
			}
			else
			{
				// Invalid uv's
				textureCoordinates[0][0] = -1.f;
				textureCoordinates[0][1] = -1.f;
				textureCoordinates[1][0] = -1.f;
				textureCoordinates[1][1] = -1.f;
				textureCoordinates[2][0] = -1.f;
				textureCoordinates[2][1] = -1.f;
			}

			// --- Get vertices
			float vertices[3][3];
			for (int v = 0; v < 3; v++)
			{
				int vertexIndex0 = index0.vertex_index;
				int vertexIndex1 = index1.vertex_index;
				int vertexIndex2 = index2.vertex_index;
				assert(vertexIndex0 >= 0);
				assert(vertexIndex1 >= 0);
				assert(vertexIndex2 >= 0);

				vertices[0][v] = attributes.vertices[3 * vertexIndex0 + v];
				vertices[1][v] = attributes.vertices[3 * vertexIndex1 + v];
				vertices[2][v] = attributes.vertices[3 * vertexIndex2 + v];

				// Keep track of the bounds
				model.minBound[v] = std::min(vertices[0][v], model.minBound[v]);
				model.minBound[v] = std::min(vertices[1][v], model.minBound[v]);
				model.minBound[v] = std::min(vertices[2][v], model.minBound[v]);

				model.maxBound[v] = std::max(vertices[0][v], model.maxBound[v]);
				model.maxBound[v] = std::max(vertices[1][v], model.maxBound[v]);
				model.maxBound[v] = std::max(vertices[2][v], model.maxBound[v]);
			}

			// --- Get normals
			float normals[3][3];
			if (attributes.normals.size() > 0 &&
				index0.normal_index >= 0 && index1.normal_index >= 0 && index2.normal_index > 0)
			{
				for (int n = 0; n < 3; n++)
				{
					assert(3 * index0.normal_index + n < attributes.normals.size());
					assert(3 * index1.normal_index + n < attributes.normals.size());
					assert(3 * index2.normal_index + n < attributes.normals.size());

					// Valid normals
					normals[0][n] = attributes.normals[3 * index0.normal_index + n];
					normals[1][n] = attributes.normals[3 * index1.normal_index + n];
					normals[2][n] = attributes.normals[3 * index2.normal_index + n];
				}
			}
			else
			{
				for (int n = 0; n < 3; n++)
				{
					// Invalid normals
					normals[0][n] = -1.f;
					normals[1][n] = -1.f;
					normals[2][n] = -1.f;
				}
			}

			// Create vertices
			for (int i = 0; i < 3; i++)
			{
				Vertex newVertex;
				std::copy(std::begin(vertices[i]), std::end(vertices[i]), std::begin(newVertex.position));
				std::copy(std::begin(normals[i]), std::end(normals[i]), std::begin(newVertex.normal));
				std::copy(std::begin(textureCoordinates[i]), std::end(textureCoordinates[i]), std::begin(newVertex.textureCoordinate));
				std::copy(std::begin(diffuseColor), std::end(diffuseColor), std::begin(newVertex.diffuseColor));
				std::copy(std::begin(ambientColor), std::end(ambientColor), std::begin(newVertex.ambientColor));
				std::copy(std::begin(specularColor), std::end(specularColor), std::begin(newVertex.specularColor));
				newVertex.shininess = shininess;
				std::copy(std::begin(emissionColor), std::end(emissionColor), std::begin(newVertex.emissionColor));
				newVertex.alpha = alpha;

				model.vertexBuffer.push_back(newVertex);
			}
		}

		o_models.push_back(model);
	}

	return true;
}
