#ifndef __BKSCREEN_INCLUDED__
#define __BKSCREEN_INCLUDED__

#include <stdint.h>

namespace bk
{
class bkScreen
{
private:
	uint8_t mode;
	uint8_t offset;

public:
	// 0 - 512x256, no inversion
	// 1 - 512x256, inversion
	// 2 - 256x256, no inversion, colors
	// 3 - 256x256, inversion, colors
	// 4 - 256x256, no inversion, black and white
	// 5 - 256x256, inversion, black and white
	void setMode(uint8_t mode);

	// port0177664, low byte
	void setOffset(uint8_t offset);

	// 4000..7FFF Video RAM (16K)
	uint8_t getVideoRam(uint16_t address);
	void setVideoRam(uint16_t address, uint8_t value);
};
}

#endif
