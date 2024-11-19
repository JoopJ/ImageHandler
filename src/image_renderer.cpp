#include "image_renderer.h"

ImageRenderer::ImageRenderer(const char* vertexPath, const char* fragmentPath) : texture(0), VAO(0), 
	VBO(0), width(0), height(0) {
	loaded = false;
	shader = Shader(vertexPath, fragmentPath);
}

ImageRenderer::~ImageRenderer() {
	if (texture) glDeleteTextures(1, &texture);
	if (VAO) glDeleteVertexArrays(1, &VAO);
	if (VBO) glDeleteBuffers(1, &VBO);
}

void ImageRenderer::load_image_from_file(const std::string& filename)
{
	// Check file is of type PPM
	if (filename.substr(filename.find_last_of(".") + 1) != "ppm") {
		std::cout << "Filename: " << filename << std::endl;
		std::cerr << "Error: File is not of type PPM" << std::endl;
		return;
	}
	auto tuple_result = extract_ppm_pixel_data(filename);
	std::vector<unsigned char> pixelData = std::get<0>(tuple_result);
	width = std::get<1>(tuple_result);
	height = std::get<2>(tuple_result);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelData.data());

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "Error loading texture: " << err << std::endl;
	}
	loaded = true;
}

void ImageRenderer::setup_rendering() {
	float vertices[] = {
		// positions		 // texture coords
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void ImageRenderer::render() {
	shader.use();
	shader.setInt("texture1", 0);

	GLint currentVAO, currentTexture;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);

	if (currentVAO != VAO) std::cerr << "Unexpected VAO" << std::endl;
	if (currentTexture != texture) std::cerr << "Unexpected texture" << std::endl;

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	if (glIsVertexArray(VAO) == GL_FALSE) std::cerr << "VAO is not valid" << std::endl;
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}