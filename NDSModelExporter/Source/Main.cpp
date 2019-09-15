#include <iostream>
#include "ObjLoader.h"
#include "Stripify/Stripify.h"

int main(int argc, char* argv[])
{
	// Get the input file
	std::string inputFile;
	if (argc > 1)
	{
		inputFile = std::string(argv[1]);
	} else
		inputFile = "B:/CppProjects/NDSModelExporter/Monkey.obj";

	// Load model
	std::vector<Model> models;
	if (!ObjLoader::load(inputFile, models))
		exit(1);

	for (auto& model : models)
	{
		std::vector<Primative> primatives = Stripify::Strip(model.vertexBuffer);

		int totalVertexCount = 0;
		std::cout << model.vertexBuffer.size() / 3 << " initial triangles contaning " << model.vertexBuffer.size() << " vertices.\n";
		std::cout << primatives.size() << " amount of primatives:\n";
		for (auto& primative : primatives)
		{
			std::cout << "Type: \t" << (primative.type == PrimativeType::TRIANGLES ? "Triangles" : "Strip    ");
			std::cout << " \tTriangle Count: " << (primative.type == PrimativeType::TRIANGLES ? primative.vertices.size() / 3 : (primative.vertices.size() - 3) + 1);
			std::cout << " \tVertex Count: " << primative.vertices.size() << "\n";
			totalVertexCount += primative.vertices.size();
		}
		std::cout << "New total vertices: " << totalVertexCount << std::endl;
	}

	std::cout << "ola" << std::endl;
	return 0;
}