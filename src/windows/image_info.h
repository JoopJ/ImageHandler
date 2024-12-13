#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <imgui/imgui.h>

class ImageInfo {
public:
	static ImageInfo& getInstance() {
		static ImageInfo instance;
		return instance;
	}

	void AddInfo(const char* key, const char* value);
	void AddTab(const char* tabName, const int tabSize);
	void Clear();
	void Display();

private:
	ImageInfo() {};
	std::vector<std::pair<std::string, std::string>> info;
	std::vector<std::pair<std::string, int>> tabs;
	int infoIndex = 0;

	void Tabs();
	void Table(int size);
};