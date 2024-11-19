#include "tga_parser.h"

#pragma pack(push, 1)
struct TGAHeader {
	uint8_t idLength;			// Length of the Image Identification Field
	uint8_t colorMapType;		// 0 = No color map, 1 = Color map included
	uint8_t imageType;
	// Color map specification
	uint16_t colorMapStart;		// Index of first color map entry
	uint16_t colorMapLength;	// Number of color map entries
	uint8_t colorMapDepth;		// Number of bits per color map entry
	// Image specification
	uint16_t xOrigin;
	uint16_t yOrigin;
	uint16_t width;
	uint16_t height;
	uint8_t bitsPerPixel;
	// Image Descriptor
	uint8_t attribBits : 3;		// Number of attribute bits associated with each pixel
	uint8_t reserved : 1;		// Reserved. Must be 0
	uint8_t screenOrigin : 1;	// 0 = origin in bottom left corner. 1 = origin in top left corner.
	uint8_t interleaving : 1;	// Data storage interleaving flag
	// Image Identification Field (optional)
	// Color map data (optional)
	// Image data
};
#pragma pack(pop)

unsigned char* load_TGA(const char* filename, int& width, int& height) 
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return nullptr;
	}

	TGAHeader header;

	file.read(reinterpret_cast<char*>(&header), sizeof(header));

	width = header.width;
	height = header.height;

	// Find the size of the image data
	size_t dataSize = width * height * (header.bitsPerPixel / 8);
	unsigned char* pixelData = new unsigned char[dataSize];

	// Find start of image data
	int dataOffset = sizeof(header) + header.idLength + header.colorMapLength * (header.colorMapDepth / 8);
	file.seekg(dataOffset, std::ios::beg);
	file.read(reinterpret_cast<char*>(pixelData), dataSize);

	file.close();
	return pixelData;
}