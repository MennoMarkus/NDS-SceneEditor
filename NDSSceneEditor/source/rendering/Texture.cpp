#include "rendering/Texture.h"

#include "glad/glad.h"

#include "services/Logger.h"
#include "textureCompressor/TextureCompressor.h"

namespace nds_se
{
	Texture::Texture(const std::string& filePath) :
		m_filePath(filePath)
	{}

	Texture::~Texture() 
	{
		if (m_renderID != 0)
		{
			glDeleteTextures(1, &m_renderID);
		}
	}

	void Texture::load()
	{
		// Deduce texture file format
		FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(m_filePath.c_str());
		if (imageFormat == FIF_UNKNOWN)
			imageFormat = FreeImage_GetFIFFromFilename(m_filePath.c_str());

		if (imageFormat == FIF_UNKNOWN)
		{
			LOG(LOG_ERROR, "[FREE_IMAGE] Failed to load image file " << m_filePath << " due to unknow file type.");
			return;
		}

		// Load texture
		m_textureData.m_bitmap = FreeImage_Load(imageFormat, m_filePath.c_str());
		if (!m_textureData.m_bitmap)
		{
			LOG(LOG_ERROR, "[FREE_IMAGE] Failed to load image file " << m_filePath);
			return;
		}

		FreeImage_FlipVertical(m_textureData.m_bitmap);

		// Compress texture data
		TextureCompressor textureCompressor(m_textureData);
		textureCompressor.compress(m_textureData);

		//Create and bind texture
		glGenTextures(1, &m_renderID);
		glBindTexture(GL_TEXTURE_2D, m_renderID);

		//Set the texture settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//Generate texture and mipmaps
		glm::uvec2 size = m_textureData.getSize();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_BGR, GL_UNSIGNED_BYTE, m_textureData.getBits());
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	bool Texture::equalsResource(const Texture & other) const
	{
		return m_filePath == other.m_filePath;
	}

	void Texture::bind() const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_renderID);
	}

	inline void Texture::unBind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	inline const std::string& Texture::getFilePath() const
	{
		return m_filePath;
	}

	const TextureData& Texture::getData() const
	{
		return m_textureData;
	}

	TextureData& Texture::getData()
	{
		return m_textureData;
	}

	const FIBITMAP* Texture::getBitmap() const
	{
		return m_textureData.m_bitmap;
	}

	FIBITMAP* Texture::getBitmap()
	{
		return m_textureData.m_bitmap;
	}	
}