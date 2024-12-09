#include <unordered_map>
#include "bkScreen.h"
#include "screen/canvas.h"

using namespace bk;
using namespace std;

// 00 - 512x256, no inversion
// 01 - 512x256, inversion
// 82 - 256x256, no inversion, colors
// 83 - 256x256, inversion, colors
// 84 - 256x256, no inversion, black and white
// 85 - 256x256, inversion, black and white
unordered_map<uint8_t, uint8_t> mode512x256 {
	{16, 0x00 },
	{235,0x01 }
};

unordered_map<uint8_t, uint8_t> mode256x256 {
	{16, 0x00 },
	{82, 0x01 },
	{145,0x02 },
	{41, 0x03 }
};

void bkScreen::setMode(uint8_t mode)
{
	this->mode = mode;
}

void bkScreen::setOffset(uint8_t offset)
{
	setVerticalOffset(offset);
}

uint8_t bkScreen::getVideoRam(uint16_t address)
{
	uint8_t result = 0;

	if ((this->mode & 0x80) == 0)
	{
		// 512x256 - 1 bit per pixel
		uint16_t offset = address - 0x4000;
		uint16_t charX = offset % 64;
		uint16_t y = offset / 64;
		for (uint16_t x = charX * 8; x < charX * 8 + 8; x++)
		{
			YCbCr pixel = GetPixel(x, y);
			uint8_t pixelBit = mode512x256[pixel.y];

			if ((this->mode & 0x01) != 0)
			{
				// inversion
				pixelBit ^= 1;
			}

			result |= pixelBit;
		    result <<= 1;
		}
	}
	else
	{
		// 256x256 - 2 bit per pixel
		uint16_t offset = address - 0x4000;
		uint16_t charX = offset % 64;
		uint16_t y = offset / 64;
		for (uint16_t x = charX * 8; x < charX * 8 + 8; x += 2)
		{
			YCbCr pixel = GetPixel(x, y);
			uint8_t pixelBits = mode256x256[pixel.y];

			if ((this->mode & 0x01) != 0)
			{
				// inversion
				pixelBits ^= 0x03;
			}

			result |= pixelBits;
		    result <<= 2;
		}
	}

	return result;
}

void bkScreen::setVideoRam(uint16_t address, uint8_t value)
{
	if ((this->mode & 0x80) == 0)
	{
		// 512x256 - 1 bit per pixel
		uint16_t offset = address - 0x4000;
		uint16_t charX = offset % 64;
		uint16_t y = offset / 64;
		for (uint16_t x = charX * 8; x < charX * 8 + 8; x++)
		{
			uint8_t pixelBits = (value & 0x01);
			if ((this->mode & 0x01) != 0)
			{
				// inversion
				pixelBits ^= 1;
			}

			uint8_t color;
			if (pixelBits)
			{
				color = 0x3f;
			}
			else
			{
				color = 0x00;
			}

			value >>= 1;
			SetPixel(x, y, color);
		}
	}
	else
	{
		// 256x256 - 2 bit per pixel
		uint16_t offset = address - 0x4000;
		uint16_t charX = offset % 64;
		uint16_t y = offset / 64;
		for (uint16_t x = charX * 8; x < charX * 8 + 8; x += 2)
		{
			uint8_t pixelBits = (value & 0x03);
			if ((this->mode & 0x01) != 0)
			{
				// inversion
				pixelBits ^= 0x03;
			}

			uint8_t color;
			switch (pixelBits)
			{
			case 0x00:
				color = 0x00;
				break;
			case 0x01:
				color = 0x30;
				break;
			case 0x02:
				color = 0x0C;
				break;
			case 0x03:
				color = 0x03;
				break;
			}

			value >>= 2;
			SetPixel(x, y, color);
			SetPixel(x + 1, y, color);
		}
	}
}
