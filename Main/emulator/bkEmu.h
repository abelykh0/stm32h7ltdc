#ifndef __BKEMU_H
#define __BKEMU_H

#include <emulator/pdp/defines.h>

#define RAM_AVAILABLE 0x01 //0x4000
extern uint8_t RamBuffer[RAM_AVAILABLE];
extern pdp_regs pdp;
extern uint8_t basic[1]; //[24448];
extern uint8_t monitor[1]; //[8192];

// 0..7 Shift
// 9 Extended memory 0 - on, 1 - off
extern uint16_t port0177664;

void bk_setup();
int32_t bk_loop();
void bk_reset();

#endif
