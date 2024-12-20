#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "src/windows/image_renderer.h"
#include "src/windows/image_browser.h"
#include "src/windows/image_info.h"

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
     
	// Intializations
	// GLFW
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Image Handler", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialise GLAD" << std::endl;
        return -1;
    }
	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	//----------------------------------------------

	ImageRenderer image_renderer;
	image_renderer.setup_rendering();
	ImageBrowser image_browser("images/");

	//----------------------------------------------

    // Loop
	while (window != NULL && !glfwWindowShouldClose(window)) 
    {
		process_input(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (image_browser.Display()) {
			ImageInfo::getInstance().Clear(); // Clear previous image info
			// when a new file is selected, load the image and resize the window
			image_renderer.load_image_from_file(image_browser.GetSelectedFile());
			glfwSetWindowSize(window, image_renderer.get_width(), 
				image_renderer.get_height());
		}
		ImageInfo::getInstance().Display();

		// Rendering
		// ImGui
		ImGui::Render();
		glClearColor(0.1f, 0.5f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Image
		if (image_renderer.is_image_loaded()) {
			image_renderer.render();
		}

		// Render ImGui
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// Render Viewports
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(window);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
    glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}