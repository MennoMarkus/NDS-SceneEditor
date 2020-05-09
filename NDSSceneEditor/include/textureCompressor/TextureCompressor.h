#pragma once
#include <vector>
#include "rendering/TextureData.h"

namespace nds_se
{
	// Texel block mode accroding to the NDS documentation. Part of the header data.
	enum TexelBlockMode
	{						// Texel0 | Texel1 | Texel2							| Texel 3
							// --------------------------------------------------------------------------------
		MODE0 = (0 << 14),	// color0 | color1 | color2,						| transparent
		MODE1 = (1 << 14),	// color0 | color1 | (color0 + color1) / 2,			| transparent
		MODE2 = (2 << 14),	// color0 | color1 | color2,						| color3
		MODE3 = (3 << 14)	// color0 | color1 | (color0 * 5 + color1 * 3) / 8	| (color0 * 3 + Color1 * 5) / 8
	};

	class TextureCompressor
	{
	private:
		const int TILE_WIDTH = 4;
		const int TILE_HEIGHT = 4;

		TextureData m_texture;

		// Settings
		unsigned int m_colorError = 8;
		FREE_IMAGE_QUANTIZE m_quantizeAlgorithm = FIQ_WUQUANT;

		// Compressed data
		std::vector<RGBQUAD> m_palette;
		std::vector<unsigned char> m_blocks;
		std::vector<unsigned short> m_headers;

	public:
		// The textureCompressor will resize textures to the nearest multiple of 4 on each axis.
		TextureCompressor(TextureData& texture);
		void compress(TextureData& o_compressedTextureData);

		unsigned int getColorError() const { return m_colorError; }
		void setColorError(unsigned int error) { m_colorError = error <= 1 ? 1 : error; }

		FREE_IMAGE_QUANTIZE getQuantizeAlgorithm() const { return m_quantizeAlgorithm; }
		void setQuantizeAlgorithm(FREE_IMAGE_QUANTIZE algorithm) { m_quantizeAlgorithm = algorithm; }

	private:
		int countUniqueColors(const TextureData& texture);

		bool compareColors(const RGBQUAD& lhs, const RGBQUAD& rhs, int error);

		RGBQUAD getLerpedColor(const RGBQUAD& color1, const RGBQUAD& color2, float t);
		int getColorDifference(const RGBQUAD& color1, const RGBQUAD& color2);
	};
}