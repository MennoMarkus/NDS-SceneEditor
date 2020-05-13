#include "rendering/Texture.h"

#include "glad/glad.h"

#include "services/Logger.h"
#include "textureCompressor/TextureCompressor.h"

namespace nds_se
{
	Texture::Texture(const std::string& filePath, bool compressOnLoad) :
		m_filePath(filePath),
		m_compressOnLoad(compressOnLoad)
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
		// Deduce texture file format.
		FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(m_filePath.c_str());
		if (imageFormat == FIF_UNKNOWN)
			imageFormat = FreeImage_GetFIFFromFilename(m_filePath.c_str());

		if (imageFormat == FIF_UNKNOWN)
		{
			LOG(LOG_ERROR, "[FREE_IMAGE] Failed to load image file " << m_filePath << " due to unknow file type.");
			return;
		}

		// Load texture.
		m_textureData.m_bitmap = FreeImage_Load(imageFormat, m_filePath.c_str());
		if (!m_textureData.m_bitmap)
		{
			LOG(LOG_ERROR, "[FREE_IMAGE] Failed to load image file " << m_filePath);
			return;
		}

		FreeImage_FlipVertical(m_textureData.m_bitmap);

		// Convert to 24 bit texture
		FIBITMAP* temp = m_textureData.m_bitmap;
		m_textureData.m_bitmap = FreeImage_ConvertTo24Bits(temp);
		FreeImage_Unload(temp);

		// Create a compressed copy of the texture.
		if (m_compressOnLoad)
			compress();

		//Create and bind texture.
		glGenTextures(1, &m_renderID);
		glBindTexture(GL_TEXTURE_2D, m_renderID);

		//Set the texture settings.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//Generate texture and mipmaps.
		glm::uvec2 size = m_textureData.getSize();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_BGR, GL_UNSIGNED_BYTE, m_textureData.getBits());
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	bool Texture::equalsResource(const Texture & other) const
	{
		return m_filePath == other.m_filePath;
	}

	void Texture::compress()
	{
		// Color reduction.
		FIBITMAP* temp = FreeImage_ColorQuantizeEx(m_textureData.m_bitmap, m_reductionAlgorithm, m_reductionColorCount);
		m_compressedTextureData = TextureData(FreeImage_ConvertTo24Bits(temp));
		FreeImage_Unload(temp);

		// Compress texture data.
		TextureCompressor textureCompressor(m_compressedTextureData);
		textureCompressor.setQuantizeAlgorithm(m_tileCompressionAlgorithm);
		textureCompressor.setColorError(m_colorCompressionError);
		textureCompressor.compress(m_compressedTextureData, m_compressedPalette, m_compressedBlocks, m_compressedHeaders);

		// Compressed texture can be upscaled. Report if this is the case.
		m_textureCoordinateScaleFactor = (glm::vec2)m_textureData.getSize() / (glm::vec2)m_compressedTextureData.getSize();
		if (m_textureCoordinateScaleFactor != glm::vec2(1))
			LOG(LOG_WARNING, "Texture " << m_filePath << " needed to be upscaled for compatibility with the Nintendo DS. Please provide an image with a size devidable by 4 to prevent problems with repeating textures!");

		// Compressed texture can be upscaled. Scale the orginal texture to the compressed texture size.
		TextureData temp2(m_compressedTextureData.getSize(), 24);
		RGBQUAD fillColor = { 0, 0, 0 };
		FreeImage_FillBackground(temp2.m_bitmap, &fillColor);
		temp2.paste(m_textureData, { 0, 0 }, 256);
		m_textureData = temp2;

		// Upload the new texture data.
		setDisplayCompressedTexture(m_displayCompressedTexture);
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

	void Texture::setDisplayCompressedTexture(bool showCompressed)
	{
		// Make sure there is an actual texture data to switch between.
		if (m_compressedTextureData.m_bitmap == nullptr || m_renderID == 0)
			return;

		// Switch the texture data bound to this render ID.
		bind();

		// Compressed textures always have the same size as the orginal.
		glm::uvec2 size = m_textureData.getSize(); 
		if (showCompressed)
		{
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_BGR, GL_UNSIGNED_BYTE, m_compressedTextureData.getBits());
		}
		else
		{
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_BGR, GL_UNSIGNED_BYTE, m_textureData.getBits());
		}
		m_displayCompressedTexture = showCompressed;
	}

	const std::string& Texture::getFilePath() const
	{
		return m_filePath;
	}

	glm::vec2 Texture::getTextureCoordinateScaleFactor() const
	{
		return m_textureCoordinateScaleFactor;
	}

}