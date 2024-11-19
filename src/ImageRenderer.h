#pragma once

#include <glad/glad.h>

#include "ppm_parser.h"

#include <string>
#include <vector>

class ImageRenderer
{
private:
	unsigned int texture, VAO, VBO;
	int width, height;

public:
	ImageRenderer();
	void load_image_from_rgb(const std::string& filename);
	void setup_rendering();
	void render();
	int get_width() { return width; }
	int get_height() { return height; }
	~ImageRenderer();
};

