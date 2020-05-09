#include "textureCompressor/TextureCompressor.h"

#include <assert.h>
#include <algorithm>

#include "freeImage/FreeImage.h"

namespace nds_se
{
	TextureCompressor::TextureCompressor(TextureData& texture)
	{
		// Round image size to the nearest multiple of the tile size.
		glm::uvec2 size = texture.getSize();
		glm::uvec2 remainder = size % glm::uvec2(TILE_WIDTH, TILE_HEIGHT);
		if (remainder.x > 0) size.x += TILE_WIDTH - remainder.x;
		if (remainder.y > 0) size.y += TILE_HEIGHT - remainder.y;

		// Copy texture to the potentially larger texture.
		m_texture.create(size, 24);
		RGBQUAD fillColor = { 0, 0, 0 };
		FreeImage_FillBackground(m_texture.m_bitmap, &fillColor);
		m_texture.paste(texture, { 0, 0 }, 256);

		// Quantisize to 555 and convert back to 24 bit
		FIBITMAP* temp = FreeImage_ConvertTo16Bits555(m_texture.m_bitmap);
		FreeImage_Unload(m_texture.m_bitmap);

		m_texture.m_bitmap = FreeImage_ConvertTo24Bits(temp);
		FreeImage_Unload(temp);
	}

	void TextureCompressor::compress(TextureData& o_compressedTextureData)
	{
		o_compressedTextureData.create(m_texture.getSize(), 24);
		m_palette.clear();
		m_blocks.clear();
		m_headers.clear();

		RGBQUAD padColor;
		padColor.rgbRed = 255;
		padColor.rgbGreen = 0;
		padColor.rgbBlue = 0;

		for (unsigned int y = 0; y < m_texture.getSize().y; y += TILE_HEIGHT)
		{
			for (unsigned int x = 0; x < m_texture.getSize().x; x += TILE_WIDTH)
			{
				RGBQUAD lerpColors[2];
				int colorCount = 4;

				// Quantize (paletize) an tile in the image into a 4 bit palette color bitmap.
				TextureData tile = FreeImage_Copy(m_texture.m_bitmap, x, y, x + TILE_WIDTH, y + TILE_HEIGHT);
				TextureData tileQuant = FreeImage_ColorQuantizeEx(tile.m_bitmap, m_quantizeAlgorithm, colorCount, 0, NULL);
				assert(tileQuant.m_bitmap != nullptr);

				// Get color pallet size.
				colorCount = countUniqueColors(tileQuant);
				int colorPadCount = colorCount;

				RGBQUAD* palette = FreeImage_GetPalette(tileQuant.m_bitmap);

				// Try to compress colors in a smaller block size by lerping between colors and checking if the new color is within an acceptable margn of error.
				// Block modes are according to the nds 2 bit texel values specification.
				TexelBlockMode blockMode = MODE2; // 4 colors for if all else fails
				if (colorCount == 4)
				{
					if (compareColors(palette[1], getLerpedColor(palette[0], palette[3], 0.375), 2 * m_colorError) &&
						compareColors(palette[2], getLerpedColor(palette[0], palette[3], 0.625), 2 * m_colorError))
					{
						lerpColors[0] = palette[0];
						lerpColors[1] = palette[3];

						BYTE a = 1;
						BYTE b = 3;
						FreeImage_SwapColors(tileQuant.m_bitmap, &palette[a], &palette[b], true);
						FreeImage_SwapPaletteIndices(tileQuant.m_bitmap, &a, &b);

						palette = lerpColors;
						colorCount = 2;
						blockMode = MODE3;
					}
				}
				else if (colorCount == 3)
				{
					if (compareColors(palette[1], getLerpedColor(palette[0], palette[2], 0.5), 2 * m_colorError))
					{
						lerpColors[0] = palette[0];
						lerpColors[1] = palette[2];

						BYTE a = 1;
						BYTE b = 2;
						FreeImage_SwapColors(tileQuant.m_bitmap, &palette[a], &palette[b], true);
						FreeImage_SwapPaletteIndices(tileQuant.m_bitmap, &a, &b);

						palette = lerpColors;
						colorCount = 2;
						colorPadCount = 2;
						blockMode = MODE1;
					}
					else
					{
						colorPadCount = 4; // round up, colors are allocated two by two 
					}
				}
				else if (colorCount == 1)
				{
					colorPadCount = 2; // round up, colors are allocated two by two 
				}
				assert((colorPadCount & 1) == 0);


				// Find the color palette with the smallets color difference with our current pallet.
				int base;
				int minDifference = std::numeric_limits<int>::max();
				int minDifferenceBase = 0;

				for (base = 0; base < int(m_palette.size()) - (colorCount - 1); base += 2)
				{
					int currentMaxDifference = 0;
					for (int i = 0; i < colorCount; ++i)
					{
						int difference = getColorDifference(m_palette[(size_t)base + i], palette[i]);
						currentMaxDifference = std::max(currentMaxDifference, difference);
					}

					if (currentMaxDifference < minDifference)
					{
						minDifference = currentMaxDifference;
						minDifferenceBase = base;
					}
				}

				// Add the current palette to the global palette if the color difference is not within a margin of error with our current palette.
				if (minDifference < (int)m_colorError)
					base = minDifferenceBase;
				else
				{
					base = int(m_palette.size());

					for (int i = 0; i < colorCount; ++i)
						m_palette.push_back(palette[i]);

					for (int i = colorCount; i < colorPadCount; ++i)
						m_palette.push_back(padColor);
				}
				assert((base & 1) == 0);

				// Calculate and add the header
				unsigned short header = 0;
				header |= (base >> 1) & ((1 << 14) - 1);
				header |= blockMode;
				m_headers.push_back(header);

				// Calculate and add the tile block
				for (int iy = TILE_HEIGHT - 1; iy >= 0; --iy)
				{
					unsigned char row = 0;
					for (int ix = 0; ix < TILE_WIDTH; ++ix)
					{
						int val = tileQuant.getPixelIndex({ ix, iy });
						assert((val & 3) == val);
						row |= val << (ix * 2);
					}
					m_blocks.push_back(row);
				}

				// Add the decompressed tile block to the decompressed result
				o_compressedTextureData.paste(tileQuant, { x, y }, 255);
			}
		}	
	}

	int TextureCompressor::countUniqueColors(const TextureData& texture)
	{
		assert(texture.m_bitmap != nullptr);

		const unsigned char* bits = (const unsigned char*)texture.getBits();

		// Count colors by counting color indices.
		int max = 0;
		for (int y = 0; y < (int)texture.getSize().y; y++)
		{
			for (int x = 0; x < (int)texture.getSize().x; x++)
			{
				if (bits[x] > max) 
					max = bits[x];
			}
			bits += texture.getPitch();
		}

		return max + 1;
	}

	bool TextureCompressor::compareColors(const RGBQUAD& lhs, const RGBQUAD& rhs, int error)
	{
		if (abs(lhs.rgbRed	 - rhs.rgbRed)	 > error) return false;
		if (abs(lhs.rgbGreen - rhs.rgbGreen) > error) return false;
		if (abs(lhs.rgbBlue  - rhs.rgbBlue)  > error) return false;
		return true;
	}

	RGBQUAD TextureCompressor::getLerpedColor(const RGBQUAD& color1, const RGBQUAD& color2, float t)
	{
		RGBQUAD result;
		result.rgbRed	= int(color1.rgbRed   * t + color2.rgbRed	* (1 - t));
		result.rgbGreen = int(color1.rgbGreen * t + color2.rgbGreen * (1 - t));
		result.rgbBlue	= int(color1.rgbBlue  * t + color2.rgbBlue  * (1 - t));
		return result;
	}

	int TextureCompressor::getColorDifference(const RGBQUAD& color1, const RGBQUAD& color2)
	{
		int maxDiff = 0;
		if (abs(color1.rgbRed	- color2.rgbRed)   > maxDiff) maxDiff = abs(color1.rgbRed   - color2.rgbRed);
		if (abs(color1.rgbGreen - color2.rgbGreen) > maxDiff) maxDiff = abs(color1.rgbGreen - color2.rgbGreen);
		if (abs(color1.rgbBlue	- color2.rgbBlue)  > maxDiff) maxDiff = abs(color1.rgbBlue  - color2.rgbBlue);
		return maxDiff;
	}
}