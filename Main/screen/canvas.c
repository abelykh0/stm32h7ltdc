#include "string.h"
#include "screen/canvas.h"

static Mcu canvas01[1]; // [24 * MCU_WIDTH] __attribute__(( section(".sram2") ));
static Mcu canvas02[1]; //[(MCU_HEIGHT - 24) * MCU_WIDTH];
static uint8_t verticalOffset = 0;

// Y-Cb-Cr 64 color palette (indexed by BBGGRR)
uint8_t Colors[][3] =
{
		/*000000*/ {16, 128, 128}, /* black */
		/*000001*/ {38, 115, 165},
		/*000010*/ {60, 103, 203},
		/*000011*/ {82, 90, 240},  /* red */
		/*000100*/ {59, 103, 97},
		/*000101*/ {81, 91, 134},
		/*000110*/ {103, 78, 171},
		/*000111*/ {124, 66, 209},
		/*001000*/ {102, 79, 65},
		/*001001*/ {124, 66, 103},
		/*001010*/ {145, 53, 140},
		/*001011*/ {167, 41, 177},
		/*001100*/ {145, 54, 34},  /* green */
		/*001101*/ {166, 41, 71},
		/*001110*/ {188, 29, 109},
		/*001111*/ {210, 16, 146},
		/*010000*/ {24, 165, 122},
		/*010001*/ {46, 153, 159},
		/*010010*/ {68, 140, 197},
		/*010011*/ {90, 128, 234},
		/*010100*/ {67, 141, 91},
		/*010101*/ {89, 128, 128}, /* light grey */
		/*010110*/ {111, 115, 165},
		/*010111*/ {133, 103, 203},
		/*011000*/ {110, 116, 59},
		/*011001*/ {132, 103, 97},
		/*011010*/ {154, 91, 134},
		/*011011*/ {176, 78, 171},
		/*011100*/ {153, 91, 28},
		/*011101*/ {175, 79, 65},
		/*011110*/ {197, 66, 103},
		/*011111*/ {218, 53, 140},
		/*100000*/ {33, 203, 116},
		/*100001*/ {55, 190, 153},
		/*100010*/ {76, 177, 191},
		/*100011*/ {98, 165, 228},
		/*100100*/ {76, 178, 85},
		/*100101*/ {97, 165, 122},
		/*100110*/ {119, 153, 159},
		/*100111*/ {141, 140, 197},
		/*101000*/ {118, 153, 53},
		/*101001*/ {140, 141, 91},
		/*101010*/ {162, 128, 128},/* grey */
		/*101011*/ {184, 115, 165},
		/*101100*/ {161, 128, 22},
		/*101101*/ {183, 116, 59},
		/*101110*/ {205, 103, 97},
		/*101111*/ {227, 91, 134},
		/*110000*/ {41, 240, 110}, /* blue */
		/*110001*/ {63, 227, 147},
		/*110010*/ {85, 215, 185},
		/*110011*/ {107, 202, 222},
		/*110100*/ {84, 215, 79},
		/*110101*/ {106, 203, 116},
		/*110110*/ {128, 190, 153},
		/*110111*/ {149, 177, 191},
		/*111000*/ {127, 190, 47},
		/*111001*/ {149, 178, 85},
		/*111010*/ {170, 165, 122},
		/*111011*/ {192, 153, 159},
		/*111100*/ {170, 166, 16},
		/*111101*/ {191, 153, 53},
		/*111110*/ {213, 141, 91},
		/*111111*/ {235, 128, 128} /* white */
};

uint8_t getVerticalOffset()
{
	return verticalOffset;
}
void setVerticalOffset(uint8_t offset)
{
	verticalOffset = offset;
}

Mcu* GetMcuPointer(uint16_t offset)
{
	//offset += verticalOffset * MCU_WIDTH;

	if (offset > MCU_WIDTH * MCU_HEIGHT)
	{
		offset -= MCU_WIDTH * MCU_HEIGHT;
	}

	if (offset < 24 * MCU_WIDTH)
	{
		return &canvas01[offset];
	}
	else
	{
		return &canvas02[offset - (24 * MCU_WIDTH)];
	}
}

uint8_t* GetChunkPointer(uint8_t chunkIndex)
{
	return (uint8_t*)GetMcuPointer(chunkIndex * CHUNK_SIZE_IN_MCU);
}

void Clear(uint8_t color)
{
	verticalOffset = 0;
	uint8_t colorIndex = color & 0x3F;
	uint8_t yValue =  Colors[colorIndex][0];
	uint8_t cbValue = Colors[colorIndex][1];
	uint8_t crValue = Colors[colorIndex][2];
	for (int i = 0; i < MCU_WIDTH * MCU_HEIGHT; i++)
	{
		Mcu* mcu = GetMcuPointer(i);
		memset(mcu->y,  yValue,  8 * 8);
		memset(mcu->cb, cbValue, 8 * 8);
		memset(mcu->cr, crValue, 8 * 8);
	}
}

// UVC_WIDTH x UVC_HEIGHT pixels
void SetPixel(uint16_t x, uint16_t y, uint8_t color)
{
	if (x >= UVC_WIDTH || y >= UVC_HEIGHT)
	{
		return;
	}

	uint8_t colorIndex = color & 0x3F;
	Mcu* mcu = GetMcuPointer(x / 8 + (y / 8 * MCU_WIDTH));
	uint8_t xOffset = x % 8;
	uint8_t yOffset = y % 8;
	mcu->y[yOffset][xOffset]  = Colors[colorIndex][0];
	mcu->cb[yOffset][xOffset] = Colors[colorIndex][1];
	mcu->cr[yOffset][xOffset] = Colors[colorIndex][2];
}

YCbCr GetPixel(uint16_t x, uint16_t y)
{
	YCbCr result;

	if (x >= UVC_WIDTH || y >= UVC_HEIGHT)
	{
		result.y = 0;
		result.cb = 0;
		result.cr = 0;
		return result;
	}

	Mcu* mcu = GetMcuPointer(x / 8 + (y / 8 * MCU_WIDTH));
	uint8_t xOffset = x % 8;
	uint8_t yOffset = y % 8;
	result.y = mcu->y[yOffset][xOffset];
	result.cb = mcu->cb[yOffset][xOffset];
	result.cr = mcu->cr[yOffset][xOffset];
	return result;
}
