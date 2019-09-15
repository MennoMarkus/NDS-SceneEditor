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
		return position == other.position &&
			   normal == other.normal &&
			   textureCoordinate == other.textureCoordinate &&
			   diffuseColor == other.diffuseColor &&
			   ambientColor == other.ambientColor &&
			   specularColor == other.specularColor &&
			   emissionColor == other.emissionColor &&
			   alpha == other.alpha;
	}
};

inline bool operator==(const Vertex& lhs, const Vertex& rhs)
{
	return position == other.position &&
		normal == other.normal &&
		textureCoordinate == other.textureCoordinate &&
		diffuseColor == other.diffuseColor &&
		ambientColor == other.ambientColor &&
		specularColor == other.specularColor &&
		emissionColor == other.emissionColor &&
		alpha == other.alpha;
}

struct Model
{
	float minBound[3];
	float maxBound[3];
	std::vector<Vertex> vertexBuffer;
};