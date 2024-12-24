#ifndef __STARTUP_H__
#define __STARTUP_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "stdint.h"

typedef enum
{
    DEMO_COLORS = 0,
    XONIX = 1,
} demo_mode;

typedef struct tagBmpHeader {
	int16_t type;
	int32_t size;
	int16_t reserved1;
	int16_t reserved2;
	int32_t offsetPixels;
}  __attribute__((packed)) bmpHeader;

void initialize();
void setup();
void loop();

#ifdef __cplusplus
}
#endif

#endif /* __STARTUP_H__ */
