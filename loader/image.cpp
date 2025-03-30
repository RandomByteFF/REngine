#include "image.hpp"
#include <stdexcept>

namespace REngine::Loader {
	Image::Image(std::string path) {
		if (!path.empty()) {
			Load(path);
		}
	}

	void Image::Load(std::string &path) {
		int texChannels;
		pixels = stbi_load(path.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);
		size = width * height * 4;
		if (!pixels) {
			throw std::runtime_error("Failed to load image");
		}
	}

	int Image::Width() const {
		return width;
	}

	int Image::Height() const {
		return height;
	}

	int Image::Size() const {
		return size;
	}

	unsigned char *Image::Pixels() const {
		return pixels;
	}

	void Image::Destroy() {
		stbi_image_free(pixels);
		pixels = nullptr;
		width = 0;
		height = 0;
		size = 0;
	}

	Image::~Image() {
		if (pixels) Destroy();
	}
}