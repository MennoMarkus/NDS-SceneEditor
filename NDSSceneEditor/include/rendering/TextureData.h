#pragma once
#include "glm/glm.hpp"
#include "freeImage/FreeImage.h"

namespace nds_se
{
	/* 
	 * TextureData acts as a wrapper around FIBITMAP. It can be used to access the raw texture data bits.
	 */

	class TextureData
	{
	public:
		FIBITMAP* m_bitmap = nullptr;

	public:
		TextureData() = default;
		TextureData(FIBITMAP* bitmap);
		TextureData(glm::uvec2 size, unsigned int BPP);
		TextureData(const TextureData& texture);
		TextureData& operator=(const TextureData& texture);
		~TextureData();

		void create(glm::uvec2 size, unsigned int BPP);
		void paste(TextureData& texture, glm::uvec2 topLeft, unsigned int alpha);

		void* getBits();
		void* const getBits() const;

		glm::uvec2 getSize() const;
		int getPitch() const;
		int getBPP() const;

		RGBQUAD getPixelColor(glm::uvec2 pixel) const;
		bool setPixelColor(glm::uvec2 pixel, RGBQUAD color);

		BYTE getPixelIndex(glm::uvec2 pixel) const;
		bool setPixelIndex(glm::uvec2 pixel, BYTE color);

		const RGBQUAD* const getPalette() const;
		void setPalette(const RGBQUAD* const palette);

		unsigned getColorsUsed() const;
	};
}