#include "rendering/TextureData.h"

namespace nds_se
{
	TextureData::TextureData(FIBITMAP* bitmap) :
		m_bitmap(bitmap)
	{}

	TextureData::TextureData(const TextureData& texture)
	{
		m_bitmap = FreeImage_Clone(m_bitmap);
	}

	TextureData::TextureData(glm::uvec2 size, unsigned int BPP)
	{
		m_bitmap = FreeImage_Allocate(size.x, size.y, BPP);
	}

	TextureData::~TextureData()
	{
		if (m_bitmap != nullptr)
			FreeImage_Unload(m_bitmap);
	}

	void TextureData::create(glm::uvec2 size, unsigned int BPP)
	{
		if (m_bitmap != nullptr)
			FreeImage_Unload(m_bitmap);

		m_bitmap = FreeImage_Allocate(size.x, size.y, BPP);
	}

	void TextureData::paste(TextureData& texture, glm::uvec2 topLeft, unsigned int alpha)
	{
		assert(m_bitmap && FreeImage_GetColorType(m_bitmap) == FIC_RGB);
		//assert(texture.m_bitmap && FreeImage_GetColorType(texture.m_bitmap) == FIC_RGB);
		FreeImage_Paste(m_bitmap, texture.m_bitmap, topLeft.x, topLeft.y, alpha);
	}

	void* TextureData::getBits()
	{
		assert(m_bitmap);
		return FreeImage_GetBits(m_bitmap);
	}

	void* const TextureData::getBits() const
	{
		assert(m_bitmap);
		return FreeImage_GetBits(m_bitmap);
	}

	glm::uvec2 TextureData::getSize() const
	{
		assert(m_bitmap);
		return glm::uvec2(FreeImage_GetWidth(m_bitmap), FreeImage_GetHeight(m_bitmap));
	}

	int TextureData::getPitch() const
	{
		assert(m_bitmap);
		return FreeImage_GetPitch(m_bitmap);
	}

	inline int TextureData::getBPP() const
	{
		assert(m_bitmap);
		return FreeImage_GetBPP(m_bitmap);
	}

	inline RGBQUAD TextureData::getPixelColor(glm::uvec2 pixel) const
	{
		assert(m_bitmap && FreeImage_GetColorType(m_bitmap) == FIC_RGB);

		RGBQUAD result;
		FreeImage_GetPixelColor(m_bitmap, pixel.x, pixel.y, &result);
		return result;
	}

	inline bool TextureData::setPixelColor(glm::uvec2 pixel, RGBQUAD color)
	{
		assert(m_bitmap && FreeImage_GetColorType(m_bitmap) == FIC_RGB);
		return FreeImage_SetPixelColor(m_bitmap, pixel.x, pixel.y, &color) == TRUE ? true : false;
	}

	BYTE TextureData::getPixelIndex(glm::uvec2 pixel) const
	{
		assert(m_bitmap);

		BYTE result;
		FreeImage_GetPixelIndex(m_bitmap, pixel.x, pixel.y, &result);
		return result;
	}

	inline bool TextureData::setPixelIndex(glm::uvec2 pixel, BYTE color)
	{
		assert(m_bitmap && FreeImage_GetColorType(m_bitmap) == FIC_PALETTE);
		return FreeImage_SetPixelIndex(m_bitmap, pixel.x, pixel.y, &color) == TRUE ? true : false;
	}

	inline const RGBQUAD* const TextureData::getPalette() const
	{
		assert(m_bitmap && FreeImage_GetColorType(m_bitmap) == FIC_PALETTE);
		return FreeImage_GetPalette(m_bitmap);
	}

	inline void TextureData::setPalette(const RGBQUAD* const palette)
	{
		assert(m_bitmap && FreeImage_GetColorType(m_bitmap) == FIC_PALETTE);

		RGBQUAD* dstPalette = FreeImage_GetPalette(m_bitmap);
		memcpy(dstPalette, palette, sizeof(RGBQUAD) * 256);
	}

	inline unsigned TextureData::getColorsUsed() const
	{
		assert(m_bitmap && FreeImage_GetColorType(m_bitmap) == FIC_PALETTE);
		return FreeImage_GetColorsUsed(m_bitmap);
	}
}