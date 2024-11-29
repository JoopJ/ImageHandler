#include "bmp_parser.h"

#pragma pack(push, 1) // Disable padding
struct BMPFileHeader {
	uint16_t fileType;			// 0x4D42 (BM)
	uint32_t fileSize;			// File size in bytes
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t dataOffset;		// Offset to start of image data
};

struct BMPInfoHeader {
	uint32_t size;				// Size of this header
	int32_t width;				// Image width
	int32_t height;				// Image height
	uint16_t planes;			// Number of color planes
	uint16_t bitsCount;			// Number of bits per pixel
	uint32_t compression;		// Compression method
	uint32_t sizeImage;			// Size of image data
	int32_t xPixelsPerMeter;	// Horizontal resolution
	int32_t yPixelsPerMeter;	// Vertical resolution
	uint32_t colorsUsed;		// Number of colors in the color palette
	uint32_t colorsImportant;	// Number of important colors
};
#pragma pack(pop)

unsigned char* load_BMP(const char* filename, int& width, int& height) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return nullptr;
	}

	BMPFileHeader fileHeader;
	BMPInfoHeader infoHeader;

	file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
	if (fileHeader.fileType != 0x4D42) {
		std::cerr << "Error: " << filename << " is not a BMP file" << std::endl;
		return nullptr;
	}

	file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

	width = infoHeader.width;
	height = infoHeader.height;

	if (infoHeader.bitsCount != 24 || infoHeader.compression != 0) {
		std::cerr << "Error: Only uncompressed 24-bit BMP files are supported" << std::endl;
		return nullptr;
	}

	// Allocate memory for pixel data
	size_t rowStride = (width * 3 + 3) & ~3; // Row size padded to multiple of 4 bytes
	size_t dataSize = rowStride * height;
	unsigned char* pixelData = new unsigned char[dataSize];

	// Seek to start of pixel data
	file.seekg(fileHeader.dataOffset, std::ios::beg);
	file.read(reinterpret_cast<char*>(pixelData), dataSize);

	file.close();
	return pixelData;
}