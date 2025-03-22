#pragma once

#include "stb_image.h"
#include <string>

namespace REngine::Loader {
	class Image {
		int width = 0;
		int height = 0;
		int size = 0;
		stbi_uc *pixels = nullptr;
	public:
		Image(std::string path = "");
		void Load(std::string &path);
		int Width() const;
		int Height() const;
		int Size() const;
		unsigned char *Pixels() const;
		void Destroy();
		~Image();
	};
}