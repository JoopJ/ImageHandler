#include "ppm_parser.h"

int find_ppm_header_end(const std::string& header)
{   // P6_H_W_MVal_ : 4th whitespace is end of header
    int header_end = -1;
    int ws_count = 0;
    for (int i = 0; i < header.size(); i++) {
        if (isspace(header[i])) {
            ws_count++;
            if (ws_count == 4) {
                header_end = i;
                break;
            }
        }
    }
    if (header_end == -1) {
        std::cout << "Error: Unable to find end of header" << std::endl;
    }
    return header_end;
}

std::tuple<std::string, int, int, int> extract_ppm_header
(const std::string& header)
{
    std::istringstream iss(header);
    std::string magic_number;
    int width, height, maxval;

    iss >> magic_number >> width >> height >> maxval;

    //std::cout << "Magic number: " << magic_number << std::endl;
    //std::cout << "Width: " << width << std::endl;
    //std::cout << "Height: " << height << std::endl;
    //std::cout << "Maxval: " << maxval << std::endl;

    return { magic_number, width, height, maxval };
}

std::vector<unsigned char> extract_ppm_pixel_data(
    const std::string& filename, int& width, int& height) 
{
    std::streampos size;
    char* memblock;
    std::vector<unsigned char> pixelData;

    std::ifstream file(filename, std::ios::in |
        std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char[size];

        file.seekg(0, std::ios::beg);
        if (file.read(memblock, size)) {
            std::cout << "File read successfully" << std::endl;
			std::cout << "File size: " << size << std::endl;
        }
        else {
            std::cout << "Error reading file" << std::endl;
            return pixelData;
        }
        file.close();

        // Find end of header
        int header_end = find_ppm_header_end(std::string(memblock));

        // Extract header
		auto header = extract_ppm_header(std::string(memblock, header_end));
		width = std::get<1>(header);
		height = std::get<2>(header);

        // Access pizel data
        int pixel_data_offset = header_end + 1;

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int pixel_offset = pixel_data_offset +
                    (row * width + col) * 3;

                unsigned char r = memblock[pixel_offset];
                unsigned char g = memblock[pixel_offset + 1];
                unsigned char b = memblock[pixel_offset + 2];

                pixelData.push_back(r);
                pixelData.push_back(g);
                pixelData.push_back(b);
            }
        }

        delete[] memblock;
    }
    else std::cout << "Unable to open file";


    return pixelData;
}

unsigned char* load_PPM(const char* filename, int& width, int& height)
{
    std::streampos size;
	char* memblock;
	unsigned char* pixelData = nullptr;

	std::ifstream file(filename, std::ios::in | std::ios::binary | 
        std::ios::ate);

    if (file.is_open())
    {
        size = file.tellg();
		memblock = new char[size];

		file.seekg(0, std::ios::beg);
        if (!file.read(memblock, size)) {
            std::cout << "Error reading file" << std::endl;
            delete[] memblock;
            return nullptr;
        }
		file.close();
        
		int header_end = find_ppm_header_end(std::string(memblock));

		auto header = extract_ppm_header(std::string(memblock, header_end));
		width = std::get<1>(header);
		height = std::get<2>(header);

		// Allocate memory for pixel data
		int pixel_data_offset = header_end + 1;
		size_t pixelDataSize = width * height * 3;
		pixelData = new unsigned char[pixelDataSize];

        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int pixel_offset = pixel_data_offset +
                    (row * width + col) * 3;

                int data_index = (row * width + col) * 3;

                pixelData[data_index] = memblock[pixel_offset];         // Red
                pixelData[data_index + 1] = memblock[pixel_offset + 1]; // Green
                pixelData[data_index + 2] = memblock[pixel_offset + 2]; // Blue
            }
        }

		delete[] memblock;
    }
	else std::cout << "Unable to open file";

	return pixelData;
}

std::tuple<int, int> extract_image_dimensions(const std::string& filename) 
{
    std::streampos size;
    char* memblock;
    int width = -1, height = -1;

    std::ifstream file(filename, std::ios::in |
        std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char[size];

        file.seekg(0, std::ios::beg);
        if (file.read(memblock, size)) {
            std::cout << "File read successfully" << std::endl;
        }
        else {
            std::cout << "Error reading file" << std::endl;
            return { 0, 0 };
        }
        file.close();

        // Find end of header
        int header_end = find_ppm_header_end(std::string(memblock));

        // Extract header
        auto header = extract_ppm_header(std::string(memblock, header_end));
        width = std::get<1>(header);
        height = std::get<2>(header);

        delete[] memblock;
    }
    else std::cout << "Unable to open file";

	return { width, height };
}