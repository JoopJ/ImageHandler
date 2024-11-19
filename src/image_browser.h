#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <imgui/imgui.h>

class ImageBrowser 
{
public:
	explicit ImageBrowser(const std::string& folderPath);
	bool Display();
	const std::string& GetSelectedFile() const;

private:
	std::string folderPath;
	std::vector<std::string> fileList;
	std::string selectedFile;

	void LoadFileList();
};