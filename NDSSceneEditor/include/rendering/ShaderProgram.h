#pragma once
#include <string>
#include <unordered_map>
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace nds_se
{
	class ShaderProgram
	{
	private:
		unsigned int m_programID = 0;

		bool m_isFile = false;
		std::string m_vertexShaderFilePath;
		std::string m_fragmentShaderFilePath;

		// Will always be empty outside of when loading
		std::string m_vertexShaderSrc;
		std::string m_fragmentShaderSrc;
		static unsigned int m_currentGeneratedShaderID;

		std::unordered_map<std::string, int> m_uniformLocations;

	public:
		ShaderProgram(std::string vertexShader, std::string fragmentShader, bool isFile = true);
		~ShaderProgram();

		void load();
		bool equalsResource(const ShaderProgram& other) const;

		void bind() const;
		void unBind() const;

		inline const std::string& getVertexShaderFilePath() const { return m_vertexShaderFilePath; }
		inline const std::string& getFragmentShaderFilePath() const { return m_fragmentShaderFilePath; }

		template <typename T>
		inline void setUniform(const std::string& name, T value) { static_assert(false); }

	private:
		unsigned int getUniformLocation(const std::string& name);

		std::string parseShaderFile(const std::string& filePath) const;
		int compileShaderCode(const std::string& sourceCode, GLenum type);

		// Prevent resource copies as this will delete OpenGL IDs and pointers multiple times
		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram(const ShaderProgram&) = delete;
	};

	/*
	 * Set uniform types definition
	 */

	template<> inline void ShaderProgram::setUniform<float>(const std::string& name, float value) { glUniform1f(getUniformLocation(name), value); }
	template<> inline void ShaderProgram::setUniform<glm::vec2>(const std::string& name, glm::vec2 value) { glUniform2fv(getUniformLocation(name), 1, &value[0]); }
	template<> inline void ShaderProgram::setUniform<glm::vec3>(const std::string& name, glm::vec3 value) { glUniform3fv(getUniformLocation(name), 1, &value[0]); }
	template<> inline void ShaderProgram::setUniform<glm::vec4>(const std::string& name, glm::vec4 value) { glUniform4fv(getUniformLocation(name), 1, &value[0]); }
	template<> inline void ShaderProgram::setUniform<glm::mat3>(const std::string& name, glm::mat3 value) { glUniformMatrix3fv(getUniformLocation(name), 1, false, &value[0][0]); }
	template<> inline void ShaderProgram::setUniform<glm::mat4>(const std::string& name, glm::mat4 value) { glUniformMatrix4fv(getUniformLocation(name), 1, false, &value[0][0]); }
	template<> inline void ShaderProgram::setUniform<int>(const std::string& name, int value) { glUniform1i(getUniformLocation(name), value); }
	template<> inline void ShaderProgram::setUniform<glm::ivec2>(const std::string& name, glm::ivec2 value) { glUniform2iv(getUniformLocation(name), 1, &value[0]); }
	template<> inline void ShaderProgram::setUniform<glm::ivec3>(const std::string& name, glm::ivec3 value) { glUniform3iv(getUniformLocation(name), 1, &value[0]); }
	template<> inline void ShaderProgram::setUniform<glm::ivec4>(const std::string& name, glm::ivec4 value) { glUniform4iv(getUniformLocation(name), 1, &value[0]); }
	template<> inline void ShaderProgram::setUniform<bool>(const std::string& name, bool value) { glUniform1i(getUniformLocation(name), value); }
}