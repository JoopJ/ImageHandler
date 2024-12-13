#include "image_info.h"

void ImageInfo::AddInfo(const char* key, const char* value) {
	info.push_back({ key, value });
}
void ImageInfo::AddTab(const char* tabName, const int tabSize) {
	tabs.push_back({tabName, tabSize});
}

void ImageInfo::Clear() {
	info.clear();
	tabs.clear();
}

void ImageInfo::Display() {
	ImGui::Begin("Image Info");
	Tabs();
	ImGui::End();
}

// Private
void ImageInfo::Tabs() {
	// Don't display with no info
	if (tabs.size() == 0 || info.size() == 0) {
		return;
	}
	infoIndex = 0;
	
	if (ImGui::BeginTabBar("Image Info Tabs")) {
		for (auto tab = tabs.begin(); tab != tabs.end(); tab++) {
			if (ImGui::BeginTabItem(tab->first.c_str())) {
				// Display table
				if (ImGui::BeginTable(tab->first.c_str(), 2)) {
					Table(tab->second);
					ImGui::EndTable();
				}
				ImGui::EndTabItem();
			}
			infoIndex += tab->second;
		}
		ImGui::EndTabBar();
	}
}

// Populate Table
void ImageInfo::Table(int size) {
	int count = 0;
	auto i = info.begin() + infoIndex;

	while (count < size && i != info.end()) {
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("%s", i->first.c_str());
		ImGui::TableNextColumn();
		ImGui::Text("%s", i->second.c_str());
		++count;
		++i;
	}
}