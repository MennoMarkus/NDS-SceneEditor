#include "rendering/ShaderProgram.h"

#include <fstream>
#include <sstream>

#include "services/Logger.h"

namespace nds_se
{
	unsigned int ShaderProgram::m_currentGeneratedShaderID = 0;

	ShaderProgram::ShaderProgram(std::string vertexShader, std::string fragmentShader, bool isFile) :
		m_isFile(isFile)
	{
		if (m_isFile)
		{
			m_vertexShaderFilePath = vertexShader;
			m_fragmentShaderFilePath = fragmentShader;
		}
		else
		{
			m_currentGeneratedShaderID++;
			m_vertexShaderFilePath = "GeneratedVertexShader#" + m_currentGeneratedShaderID;
			m_fragmentShaderFilePath = "GeneratedVertexShader#" + m_currentGeneratedShaderID;
			m_vertexShaderSrc = vertexShader;
			m_fragmentShaderSrc = fragmentShader;
		}
	}

	ShaderProgram::~ShaderProgram()
	{
		if (m_programID != 0)
			glDeleteProgram(m_programID);
	}

	void ShaderProgram::load()
	{
		// Parse file
		if (m_isFile)
		{
			m_vertexShaderSrc = parseShaderFile(m_vertexShaderFilePath);
			m_fragmentShaderSrc = parseShaderFile(m_fragmentShaderFilePath);
		}

		// Create shader
		int vertexShaderID = compileShaderCode(m_vertexShaderSrc, GL_VERTEX_SHADER);
		int fragmentShaderID = compileShaderCode(m_fragmentShaderSrc, GL_FRAGMENT_SHADER);
		m_vertexShaderSrc.clear();
		m_fragmentShaderSrc.clear();
		if (vertexShaderID < 0 || fragmentShaderID < 0)
			return;

		m_programID = glCreateProgram();
		glAttachShader(m_programID, vertexShaderID);
		glAttachShader(m_programID, fragmentShaderID);
		glLinkProgram(m_programID);
		glValidateProgram(m_programID);
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

		// Validate shader program
		int linkSuccess, validateSuccess;
		glGetProgramiv(m_programID, GL_LINK_STATUS, &linkSuccess);
		glGetProgramiv(m_programID, GL_VALIDATE_STATUS, &validateSuccess);
		if (linkSuccess == GL_FALSE || validateSuccess == GL_FALSE)
		{
			int length;
			glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &length);

			char* message = (char*)alloca(length * sizeof(char));
			glGetProgramInfoLog(m_programID, length, &length, message);
			LOG(LOG_ERROR, "Failed to compile shader program.\n>>\t" << message);

			glDeleteProgram(m_programID);
		}
	}

	bool ShaderProgram::equalsResource(const ShaderProgram& other) const
	{
		return  m_vertexShaderFilePath == other.m_vertexShaderFilePath && 
				m_fragmentShaderFilePath == other.m_fragmentShaderFilePath;
	}

	void ShaderProgram::bind() const
	{
		glUseProgram(m_programID);
	}

	void ShaderProgram::unBind() const
	{
		glUseProgram(0);
	}

	unsigned int ShaderProgram::getUniformLocation(const std::string& name)
	{
		// Try find cached shader uniform location
		auto it = m_uniformLocations.find(name);
		if (it != m_uniformLocations.end())
		{
			return it->second;
		}

		// Get shader uniform location
		int location = glGetUniformLocation(m_programID, name.c_str());
		if (location == -1)
			LOG(LOG_WARNING, "Shader uniform " << name << " doesn't excist.");

		m_uniformLocations[name] = location;

		return location;
	}

	std::string ShaderProgram::parseShaderFile(const std::string& filePath) const
	{
		// Open file
		std::ifstream stream;
		stream.open(filePath);
		if (!stream.is_open())
		{
			LOG(LOG_ERROR, "Failed to load shader file " << filePath);
			return "";
		}

		// Read file contents
		std::stringstream ss;
		ss << stream.rdbuf();

		stream.close();
		return ss.str();
	}

	int ShaderProgram::compileShaderCode(const std::string& sourceCode, GLenum type)
	{
		// Create shader from source
		unsigned int shaderID = glCreateShader(type);
		const char* source = sourceCode.c_str();
		glShaderSource(shaderID, 1, &source, NULL);
		glCompileShader(shaderID);

		// Validate shader creation
		int success;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			int length;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);

			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(shaderID, length, &length, message);
			LOG(LOG_ERROR, "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader.\n>>\t" << message);

			glDeleteShader(shaderID);
			return -1;
		}

		return shaderID;
	}
}