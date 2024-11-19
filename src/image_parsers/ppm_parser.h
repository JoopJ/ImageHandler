#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

int find_ppm_header_end(const std::string& header);

std::tuple<std::string, int, int, int> extract_ppm_header
    (const std::string& header);

std::vector<unsigned char> extract_ppm_pixel_data(
    const std::string& filename, int& width, int& height);

unsigned char* load_PPM(const char* filename, int& width, int& height);

std::tuple<int, int> extract_image_dimensions(const std::string& filename);