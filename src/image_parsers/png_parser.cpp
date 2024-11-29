#include "png_parser.h"

void print_IHDR_chunk(IHDRChunk IHDR);
size_t Paeth(unsigned char a, unsigned char b, unsigned char c);

PNGReader::PNGReader(const char* filename, int& width, int& height) {
	load_PNG(filename);
	width = m_IHDR.width;
	height = m_IHDR.height;
}

PNGReader::~PNGReader() {
	if (m_pixelData) {
		delete[] m_pixelData;
	}
}

unsigned char* PNGReader::get_pixel_data() {
	return m_pixelData;
}

void PNGReader::load_PNG(const char* filename) {
	// Delete pixel_data if it already exists
	if (m_pixelData) {
		delete[] m_pixelData;
	}

	// Open file
	m_file.open(filename, std::ios::binary);
	if (!m_file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return;
	}

	// Check if the file is a PNG
	if (!verify_signature()) {
		m_file.close();
		std::cout << "Error: tried to read file: " << filename << " as PNG, which it is not." << std::endl;
		return;
	}

	// Read chunks
	uint32_t dataLength;
	char* chunkType = new char[5];	// 4 bytes + null terminator
	unsigned char* chunkData;
	uint32_t crc;

	m_current_chunk_start_pos = 8;	// Skip signature
	bool endReached = false;
	// Read chunks
	while (!endReached) {
		
		// Read chunk
		read_chunk_length(dataLength);
		m_current_chunk_length = 4 + 4 + dataLength + 4;

		read_chunk_type(chunkType);
		chunkData = new unsigned char[dataLength];

		if (dataLength != 0)   // Skip if chunk is empty
			read_chunk_data(dataLength, chunkData);

		read_chunk_crc(crc);
		verify_crc(dataLength, crc);

		// Interpret chunk
		if (strcmp(chunkType, "IDAT") == 0) {
			//std::cout << "IDAT chunk found at " << m_current_chunk_start_pos << std::endl;
			handle_IDAT_chunk(chunkData, dataLength);
		}
		else if (strcmp(chunkType, "IHDR") == 0) {
			m_IHDR = *reinterpret_cast<IHDRChunk*>(chunkData);

			// Convert to little-endian
			m_IHDR.width = _byteswap_ulong(m_IHDR.width);
			m_IHDR.height = _byteswap_ulong(m_IHDR.height);
		}
		else if (strcmp(chunkType, "IEND") == 0) {
			//std::cout << "IEND chunk found at " << m_current_chunk_start_pos << std::endl;
			endReached = true;
		}
		else if (strcmp(chunkType, "PLTE") == 0) {
			//std::cout << "PLTE chunk found at " << m_current_chunk_start_pos << std::endl;
			;
		}
		else if (strcmp(chunkType, "sRGB") == 0) {
			//std::cout << "sRGB chunk found at " << m_current_chunk_start_pos << std::endl;
			;
		}
		else if (strcmp(chunkType, "eXIf") == 0) {
			//std::cout << "eXIf chunk found at " << m_current_chunk_start_pos << std::endl;
			;
		}
		else if (strcmp(chunkType, "pHYs") == 0) {
			//std::cout << "pHYs chunk found at " << m_current_chunk_start_pos << std::endl;
			;
		}
		else if (strcmp(chunkType, "tIME") == 0) {
			//std::cout << "tIME chunk found at " << m_current_chunk_start_pos << std::endl;
			;
		}
		else if (strcmp(chunkType, "iTXt") == 0) {
			//std::cout << "iTXt chunk found at " << m_current_chunk_start_pos << std::endl;
			;
		}
		else {
			std::cerr << "Error: Unknown chunk type: " << chunkType << std::endl;
			endReached = true;
		}

		m_current_chunk_start_pos += m_current_chunk_length;

		delete[] chunkData;
	}
	decompress_image_data(m_IHDR.width, m_IHDR.height);
	delete[] chunkType;
	m_file.close();
	return;
}

bool PNGReader::verify_signature() {
	unsigned char signature[8];
	m_file.read(reinterpret_cast<char*>(signature), 8);
	if (m_file.gcount() != 8) {
		std::cerr << "Error reading file" << std::endl;
		return false;
	}
	unsigned char expectedSignature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	if (memcmp(signature, expectedSignature, 8) != 0) {
		std::cerr << "Error: File is not a PNG" << std::endl;
		return false;
	}
	return true;
}

void PNGReader::read_chunk_length(uint32_t& length) {
	// Seek to start of chunk
	m_file.seekg(m_current_chunk_start_pos);
	m_file.read(reinterpret_cast<char*>(&length), 4);
	if (m_file.gcount() != 4) {
		std::cerr << "Error reading chunk at 'length'" << std::endl;
		return;
	}
	// Convert to little-endian
	length = _byteswap_ulong(length);

	return;
}

void PNGReader::read_chunk_type(char* chunkType) {
	// Seek to chunk type position
	m_file.seekg(m_current_chunk_start_pos + 4);
	m_file.read(chunkType, 4);
	if (m_file.gcount() != 4) {
		std::cerr << "Error reading chunk at 'chunk type'" << std::endl;
		return;
	}
	// Null-terminate the string, to print safely
	chunkType[4] = '\0';

	return;
}

void PNGReader::read_chunk_data(uint32_t length, unsigned char* data) {
	// Seek to chunk data position
	m_file.seekg(m_current_chunk_start_pos + 8);
	m_file.read(reinterpret_cast<char*>(data), length);
	if (m_file.gcount() != length) {
		std::cerr << "Error reading chunk at 'data'" << std::endl;
		return;
	}
	// Print data (for debugging)
	//std::cout << "Data: "; for (int i = 0; i < length; i++)
	//	std::cout << static_cast<int>(data[i]) << " "; std::cout << std::endl;
	return;
}

void PNGReader::read_chunk_crc(uint32_t& crc) {
	// Seek to CRC position
	m_file.seekg(m_current_chunk_start_pos + m_current_chunk_length - 4);
	m_file.read(reinterpret_cast<char*>(&crc), 4);
	if (m_file.gcount() != 4) {
		std::cerr << "Error reading chunk at 'CRC'" << std::endl;
		return;
	}
	// Convert to little-endian
	crc = _byteswap_ulong(crc);
	//std::cout << "CRC: " << static_cast<int>(crc) << std::endl;
	return;
}

bool PNGReader::verify_crc(uint32_t length, uint32_t crc) {
	// Seek past length
	m_file.seekg(m_current_chunk_start_pos + 4);
	unsigned char* CRCData = new unsigned char[length + 4];
	m_file.read(reinterpret_cast<char*>(CRCData), length + 4);
	if (m_file.gcount() != (length + 4)) {
		std::cerr << "Error reading CRC data" << std::endl;
		delete[] CRCData;
		return false;
	}

	// Calculate CRC
	uint32_t calculatedCRC = crc32(0l, Z_NULL, 0);
	calculatedCRC = crc32(calculatedCRC, CRCData, length + 4);
	delete[] CRCData;

	if (calculatedCRC != crc) {
		std::cerr << "Error: CRC mismatch, Chunk is corrupted" << std::endl;
		return false;
	}
	else {
		//std::cout << "CRC verified" << std::endl;
		return true;
	}
}

void PNGReader::handle_IDAT_chunk(unsigned char* chunkData, uint32_t length) {
	m_IDAT_data.insert(m_IDAT_data.end(), chunkData, chunkData + length);
}

void PNGReader::decompress_image_data(const size_t width,const size_t height) {

	const size_t bytes_per_pixel = 4;	// RGBA
	const size_t row_size = width * bytes_per_pixel;
	const size_t row_stride = row_size + 1;			// Each row has a filter_type byte
	const size_t filtered_size = row_stride * height;

	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = m_IDAT_data.size();
	stream.next_in = m_IDAT_data.data();

	if (inflateInit(&stream, 15 + 32) != Z_OK) {
		std::cerr << "Error: inflateInit failed" << std::endl;
		return;
	}

	unsigned char* filteredData = new unsigned char[filtered_size];
	stream.avail_out = filtered_size;
	stream.next_out = filteredData;

	int ret = inflate(&stream, Z_FINISH);
	inflateEnd(&stream);
	if (ret != Z_STREAM_END) {
		delete[] filteredData;
		std::cerr << "Error: inflate failed" << std::endl;
		return;
	}

	const size_t final_size = width * height * bytes_per_pixel;
	m_pixelData = new unsigned char[final_size];

	unsigned char* prev_row = new unsigned char[row_size];
	unsigned char* curr_row = new unsigned char[row_size];

	// Process each row with filtering
	for (size_t y = 0; y < height; y++) {
		unsigned char* src = filteredData + (y * row_stride);
		unsigned char* dst = m_pixelData + (y * row_size);

		unsigned char filter_type = src[0];

		memcpy(curr_row, src + 1, row_size);

		// Apply filter
		switch (filter_type) {
			case 0: // None
				memcpy(dst, curr_row, row_size);
				break;
			case 1: // Sub
				for (size_t x = 0; x < row_size; x++) {
					unsigned char a = x >= bytes_per_pixel ? dst[x - bytes_per_pixel] : 0;
					dst[x] = curr_row[x] + a;
				}
				break;
			case 2: // Up
				for (size_t x = 0; x < row_size; x++) {
					dst[x] = curr_row[x] + prev_row[x];
				}
				break;
			case 3:	// Avg
				for (size_t x = 0; x < row_size; x++) {
					unsigned char a = x >= bytes_per_pixel ? dst[x - bytes_per_pixel] : 0;
					unsigned char b = prev_row[x];
					dst[x] = curr_row[x] + ((a + b) / 2);
				}
				break;
			case 4: // Paeth
				for (size_t x = 0; x < row_size; x++) {
					unsigned char a = x >= bytes_per_pixel ? dst[x - bytes_per_pixel] : 0;
					unsigned char b = prev_row[x];
					unsigned char c = x >= bytes_per_pixel ? prev_row[x - bytes_per_pixel] : 0;
					dst[x] = curr_row[x] + Paeth(a, b, c);
				}
				break;
		}

		memcpy(prev_row, dst, row_size);
	}

	delete[] prev_row;
	delete[] curr_row;
	delete[] filteredData;

	return;
}

int PNGReader::get_pixel_depth() {
	if (m_IHDR.colorType == 0) {
		if (m_IHDR.bitDepth == 1) {
			return 1;
		}
		else if (m_IHDR.bitDepth == 2) {
			return 2;
		}
		else if (m_IHDR.bitDepth == 4) {
			return 4;
		}
		else if (m_IHDR.bitDepth == 8) {
			return 8;
		}
		else if (m_IHDR.bitDepth == 16) {
			return 16;
		}
	}
	else if (m_IHDR.colorType == 2) {
		if (m_IHDR.bitDepth == 8) {
			return 24;
		}
		else if (m_IHDR.bitDepth == 16) {
			return 48;
		}
	}
	else if (m_IHDR.colorType == 3) {
		if (m_IHDR.bitDepth == 1) {
			return 1;
		}
		else if (m_IHDR.bitDepth == 2) {
			return 2;
		}
		else if (m_IHDR.bitDepth == 4) {
			return 4;
		}
		else if (m_IHDR.bitDepth == 8) {
			return 8;
		}
	}
	else if (m_IHDR.colorType == 4) {
		if (m_IHDR.bitDepth == 8) {
			return 16;
		}
		else if (m_IHDR.bitDepth == 16) {
			return 32;
		}
	}
	else if (m_IHDR.colorType == 6) {
		if (m_IHDR.bitDepth == 8) {
			return 32;
		}
		else if (m_IHDR.bitDepth == 16) {
			return 64;
		}
	}
}

size_t Paeth(unsigned char a, unsigned char b, unsigned char c) {
	int p = a + b - c; 
	int pa = abs(p - a);
	int pb = abs(p - b);
	int pc = abs(p - c);
	if (pa <= pb && pa <= pc) {
		return a;
	}
	else if (pb <= pc) {
		return b;
	}
	else {
		return c;
	}
}

// Output functions
void print_IHDR_chunk(IHDRChunk IHDR) {
	std::cout << "Width: " << IHDR.width << std::endl;
	std::cout << "Height: " << IHDR.height << std::endl;
	std::cout << "Bit depth: " << static_cast<int>(IHDR.bitDepth) << std::endl;
	std::cout << "Color type: " << static_cast<int>(IHDR.colorType) << std::endl;
	std::cout << "Compression method: " << static_cast<int>(IHDR.compressionMethod) << std::endl;
	std::cout << "Filter method: " << static_cast<int>(IHDR.filterMethod) << std::endl;
	std::cout << "Interlace method: " << static_cast<int>(IHDR.interlaceMethod) << std::endl;
}