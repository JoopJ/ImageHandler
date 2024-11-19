#pragma once

#include <glad/glad.h>

#include "ppm_parser.h"
#include "shader.h"

#include <string>
#include <vector>

class ImageRenderer
{
public:
	ImageRenderer(const char* vertexPath, const char* fragmentPath);
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

	Shader shader;

	const char* get_file_extension(const std::string& filename);
};

