#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <zlib/zlib.h>

#pragma pack(push, 1)
struct IHDRChunk {
	uint32_t width;
	uint32_t height;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;
};
#pragma pack(pop)

class PNGReader {
public:
	PNGReader(const char* filename, int& width, int& height);
	~PNGReader();
	unsigned char* get_pixel_data();
	int getWidth() const { return m_IHDR.width; }
	int getHeight() const { return m_IHDR.height; }

private:
	std::vector<unsigned char> m_IDAT_data;
	unsigned char* m_pixelData;

	// Header data
	IHDRChunk m_IHDR;
	// File data
	bool sRGB;

	std::ifstream m_file;
	int m_current_chunk_start_pos;
	int m_current_chunk_length;

	void load_PNG(const char* filename);
	bool verify_signature();

	void read_chunk_length(uint32_t& length);
	void read_chunk_type(char* chunkType);
	void read_chunk_data(uint32_t length, unsigned char* data);
	void read_chunk_crc(uint32_t& crc);
	bool verify_crc(uint32_t length, uint32_t crc);
	int get_pixel_depth();

	void handle_IDAT_chunk(unsigned char* chunkData, uint32_t length);
	void decompress_image_data(size_t width, size_t height);
};