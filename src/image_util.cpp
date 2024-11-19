#include "image_util.h"

void swap_red_and_blue(unsigned char* pixelData,
	int width, int height, int channels) {
	for (int i = 0; i < width * height * channels; i += channels) {
		std::swap(pixelData[i], pixelData[i + 2]);
	}
}