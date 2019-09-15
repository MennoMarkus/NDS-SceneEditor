#pragma once
#include <vector>

struct Vertex
{
	float position[3];
	float normal[3];
	float textureCoordinate[2];
	float diffuseColor[3];
	float ambientColor[3];
	float specularColor[3];
	float emissionColor[3];
	float alpha;

	inline bool operator==(const Vertex& other) const
	{
		return position[0] == other.position[0] && position[1] == other.position[1] && position[2] == other.position[2] &&
			   normal[0] == other.normal[0] && normal[1] == other.normal[1] && normal[2] == other.normal[2] &&
			   textureCoordinate[0] == other.textureCoordinate[0] && textureCoordinate[1] == other.textureCoordinate[1] &&
			   diffuseColor[0] == other.diffuseColor[0] && diffuseColor[1] == other.diffuseColor[1] && diffuseColor[2] == other.diffuseColor[2] &&
			   ambientColor[0] == other.ambientColor[0] && ambientColor[1] == other.ambientColor[1] && ambientColor[2] == other.ambientColor[2] &&
			   specularColor[0] == other.specularColor[0] && specularColor[1] == other.specularColor[1] && specularColor[2] == other.specularColor[2] &&
			   emissionColor[0] == other.emissionColor[0] && emissionColor[1] == other.emissionColor[1] && emissionColor[2] == other.emissionColor[2] &&
			   alpha == other.alpha;
	}
};

struct Model
{
	float minBound[3];
	float maxBound[3];
	std::vector<Vertex> vertexBuffer;
};