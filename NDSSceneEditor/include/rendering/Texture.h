#pragma once
#include <string>
#include <vector>
#include "rendering/TextureData.h"

namespace nds_se
{
	class Texture
	{
	private:
		unsigned int m_renderID = 0;

		// Original texture data
		std::string m_filePath;
		bool m_compressOnLoad = true;
		TextureData m_textureData;

		// Compressed texture data
		TextureData m_compressedTextureData;
		std::vector<RGBQUAD> m_compressedPalette;
		std::vector<unsigned char> m_compressedBlocks;
		std::vector<unsigned short> m_compressedHeaders;

		// Compressed settings
		FREE_IMAGE_QUANTIZE m_reductionAlgorithm = FIQ_WUQUANT;
		unsigned int m_reductionColorCount = 128;
		FREE_IMAGE_QUANTIZE m_tileCompressionAlgorithm = FIQ_WUQUANT;
		unsigned int m_colorCompressionError = 8;

		// Dsplay settings
		bool m_displayCompressedTexture = false;
		glm::vec2 m_textureCoordinateScaleFactor = { 1, 1 };

	public:
		Texture(const std::string& filePath, bool compressOnLoad = true);
		~Texture();

		void load();
		bool equalsResource(const Texture& other) const;

		void compress();

		void bind() const;
		void unBind() const;

		// Reuploads texture data and should therefor not be called frequently.
		void setDisplayCompressedTexture(bool showCompressed);

		const std::string& getFilePath() const;
		glm::vec2 getTextureCoordinateScaleFactor() const;

	private:
		// Prevent resource copies as this will delete OpenGL IDs and pointers multiple times
		Texture& operator=(const Texture&) = delete;
		Texture(const Texture&) = delete;
	};
}