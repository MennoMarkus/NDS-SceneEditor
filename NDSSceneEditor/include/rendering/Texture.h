#pragma once
#include <string>
#include "rendering/TextureData.h"

namespace nds_se
{
	class Texture
	{
	private:
		unsigned int m_renderID = 0;

		std::string m_filePath;
		TextureData m_textureData;

	public:
		Texture(const std::string& filePath);
		~Texture();

		void load();
		bool equalsResource(const Texture& other) const;

		void bind() const;
		void unBind() const;

		inline const std::string& getFilePath() const;

		const TextureData& getData() const;
		TextureData& getData();

		const FIBITMAP* getBitmap() const;
		FIBITMAP* getBitmap();

	private:
		// Prevent resource copies as this will delete OpenGL IDs and pointers multiple times
		Texture& operator=(const Texture&) = delete;
		Texture(const Texture&) = delete;
	};
}