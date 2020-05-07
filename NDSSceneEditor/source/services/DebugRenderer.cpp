#include "services/DebugRenderer.h"
/*
#include "glad/glad.h"

#include "rendering/Program.h"

DebugRenderer::DebugRenderer(const char* fileVert, const char* fileFrag){
	m_program = new Program(fileVert, fileFrag);

	//Generate buffers
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
}
DebugRenderer::~DebugRenderer(){
	delete m_program;

	glDeleteBuffers(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void DebugRenderer::set_view_projection(glm::mat4* mat) {
	m_viewProjection = mat;
}

void DebugRenderer::add_line(const glm::vec3& startPos, const glm::vec3& startCol, const glm::vec3& endPos, const glm::vec3& endCol){
	m_lines.push_back({startPos, startCol, endPos, endCol});
}

void DebugRenderer::render(bool keepData) {
	if (m_lines.size() > 0) {
		m_program->activate();
		m_program->set_uniform("mvp", (*m_viewProjection));

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DebugLine)*m_lines.size(), m_lines.data(), GL_STREAM_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugLine)/2, (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugLine)/2, (void*)sizeof(glm::vec3));
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_LINES, 0, m_lines.size()*2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if (!keepData) {
			m_lines.clear();
		}
	}
}
*/