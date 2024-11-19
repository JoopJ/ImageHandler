#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "src/Shader.h"
#include "src/ImageRenderer.h"

#include <vector>

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {

	const char* filename = "images/sample_640x426.ppm";
	//const char* filename = "images/sample_1920x1080.ppm";

	auto result = extract_image_dimensions(filename);
	int window_width = std::get<0>(result);
	int window_height = std::get<1>(result);
     
	// Intializations
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Image Viewer", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialise GLAD" << std::endl;
        return -1;
    }

	// Build and compile shader program
	Shader shader("src/shaders/basic.vert", "src/shaders/basic.frag");

	ImageRenderer renderer;
	renderer.setup_rendering();
	renderer.load_image_from_rgb(filename);

	glViewport(0, 0, window_width, window_height);

    // Loop
	while (window != NULL && !glfwWindowShouldClose(window)) 
    {
		process_input(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render
		shader.use();
		shader.setInt("texture1", 0);
		renderer.render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}