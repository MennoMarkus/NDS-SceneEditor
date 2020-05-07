#pragma once
/*
#include "Service.h"

#include <vector>

#include "glm/glm.hpp"

class Program;

struct DebugLine {
	glm::vec3 startPos;
	glm::vec3 startCol;
	glm::vec3 endPos;
	glm::vec3 endCol;
};

class DebugRenderer : public Service {
public:
	DebugRenderer(const char* fileVert, const char* fileFrag);
	~DebugRenderer();
	
	void set_view_projection(glm::mat4* mat);
	void add_line(const glm::vec3& startPos, const glm::vec3& startCol, const glm::vec3& endPos, const glm::vec3& endCol);
	void render(bool keepData = false);
private:
	std::vector<DebugLine> m_lines;
	
	Program* m_program;
	glm::mat4* m_viewProjection;

	unsigned int m_VAO = 0;
	unsigned int m_VBO = 0;
};
*/