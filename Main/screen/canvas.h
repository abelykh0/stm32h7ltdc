#ifndef __MCU_H__
#define __MCU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "usbd_video_conf.h"

typedef struct
{
	uint8_t y[8][8];
	uint8_t cb[8][8];
	uint8_t cr[8][8];
} __attribute__((packed)) Mcu;

typedef struct
{
	uint8_t y;
	uint8_t cb;
	uint8_t cr;
} __attribute__((packed)) YCbCr;

#define MCU_WIDTH (UVC_WIDTH / 8)
#define MCU_HEIGHT (UVC_HEIGHT / 8)
#define TEXT_COLUMNS (UVC_WIDTH / 8)
#define TEXT_ROWS (UVC_HEIGHT / 8)
#define CHUNK_SIZE_IN_MCU (MCU_WIDTH * 4)
#define CHUNK_SIZE (CHUNK_SIZE_IN_MCU * sizeof(Mcu))

uint8_t getVerticalOffset();
void setVerticalOffset(uint8_t offset);

void Clear(uint8_t color);
void SetPixel(uint16_t x, uint16_t y, uint8_t color);
YCbCr GetPixel(uint16_t x, uint16_t y);
uint8_t* GetChunkPointer(uint8_t chunkIndex);

#ifdef __cplusplus
}
#endif

#endif
