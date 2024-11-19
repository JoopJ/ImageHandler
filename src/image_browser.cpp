#include "image_browser.h"
#include <iostream> // for std::cerr

ImageBrowser::ImageBrowser(const std::string& folderPath) 
	: folderPath(folderPath), selectedFile("") {
	LoadFileList();
}

void ImageBrowser::LoadFileList() {
	fileList.clear();
	try {
		for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
			if (entry.is_regular_file()) {
				fileList.push_back(entry.path().string());
				//std::cout << "Found file: " << entry.path().string() << std::endl;
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error loading file list: " << e.what() << std::endl;
	}
}

// Returns true if a new file is selected
bool ImageBrowser::Display() {
	bool newFileSelected = false;
	ImGui::Begin("Image Browser");
	for (const auto& file : fileList) {
		const bool is_selected = selectedFile == file;
		if (ImGui::Selectable(file.c_str(), is_selected)) {
			selectedFile = file;
			std::cout << "Selected file: " << selectedFile << std::endl;
			newFileSelected = true;
		}
	}
	ImGui::End();
	return newFileSelected;
}

const std::string& ImageBrowser::GetSelectedFile() const {
	return selectedFile;
}