#pragma once

#include <glad/glad.h>

#include "ppm_parser.h"

#include <string>
#include <vector>

class ImageRenderer
{
public:
	ImageRenderer();
	void load_image_from_file(const std::string& filename);
	void setup_rendering();
	void render();
	int get_width() { return width; }
	int get_height() { return height; }
	bool is_image_loaded() { return loaded; }
	~ImageRenderer();

private:
	unsigned int texture, VAO, VBO;
	int width, height;
	bool loaded;
};

