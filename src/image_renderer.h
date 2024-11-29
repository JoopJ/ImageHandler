#pragma once

#include <glad/glad.h>

#include "image_parsers/ppm_parser.h"
#include "image_parsers/bmp_parser.h"
#include "image_parsers/tga_parser.h"
#include "image_parsers/png_parser.h"
#include "shader.h"

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
	bool flipped = false;
	GLint internalFormat;
	GLenum dataFormat;

	Shader shader;
	const char* vertexPath = "src/shaders/basic.vert";
	const char* fragmentPath = "src/shaders/basic.frag";


	const char* get_file_extension(const std::string& filename);
};

