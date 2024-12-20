#include "image_renderer.h"

ImageRenderer::ImageRenderer() : texture(0), VAO(0), 
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
	unsigned char* pixelData = nullptr;

	const char* file_extension = get_file_extension(filename);

	// Default values
	width = 800;
	height = 600;
	if (strcmp(file_extension, "ppm") == 0) {
		internalFormat = GL_RGB;
		pixelData = load_PPM(filename.c_str(), width, height);
		dataFormat = GL_RGB;
		flipped = false;
	}
	else if (strcmp(file_extension, "bmp") == 0) {
		internalFormat = GL_RGB;
		pixelData = load_BMP(filename.c_str(), width, height);
		// BMP files are stored in BGR format, so swap B and R	
		dataFormat = GL_BGR;
		flipped = true;
	}
	else if (strcmp(file_extension, "tga") == 0) {
		internalFormat = GL_RGB;
		pixelData = load_TGA(filename.c_str(), width, height);
		// TGA files are stored in BGR format, so swap B and R
		dataFormat = GL_BGR;
		flipped = true;
	} 
	else if (strcmp(file_extension, "png") == 0) {
		internalFormat = GL_RGBA;
		PNGReader pngReader(filename.c_str(), width, height);
		pixelData = new unsigned char[width * height * 4];
		memcpy(pixelData, pngReader.get_pixel_data(), width * height * 4);
		dataFormat = GL_RGBA;
		flipped = false;
	}
	else {
		// Check if unsupported file extension is critical or not
		if (isupper(file_extension[0])) {
			std::cerr << "Unsupported file extension: " << file_extension << std::endl;
			return;
		}
		else {
			std::cout << "Unsupported file extension: " << file_extension << std::endl;
		}
		return;
	}
	// Handle failed image loading
	if (pixelData == nullptr) {
		std::cerr << "Error loading image: " << filename << std::endl;
		return;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, 
		GL_UNSIGNED_BYTE, pixelData);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "Error loading texture: " << err << std::endl;
	}
	loaded = true;

	std::cout << "Loaded image: " << filename << "\n\n";
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
	if (flipped) {
		shader.setBool("flipped", true);
	}
	else {
		shader.setBool("flipped", false);
	}

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

const char* ImageRenderer::get_file_extension(const std::string& filename) {
	size_t dotIndex = filename.rfind(".");
	if (dotIndex == std::string::npos) {
		return "";
	}
	return filename.c_str() + dotIndex + 1;
}