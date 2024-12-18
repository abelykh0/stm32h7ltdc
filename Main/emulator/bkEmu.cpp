// BK-0010 page mapping
// ====================
// 0000..3FFF RAM (16K)
//   0020 Screen mode 0 - 512x256, FF - 256x256
//   0021 Screen inversion 0 - off, FF - on
// 4000..7FFF Video RAM (16K)
// 8000..9FFF ROM Monitor and drivers (8K)
// A000..FF7F ROM Basic (24448 bytes)
// FF80..FFFF I/O ports
//   FFB0 Keyboard status
//   FFB2 Keyboard key code
//   FFB3 Scroll register
//   FFC6 System Timer counter start value
//   FFC8 System Timer counter
//   FFCA System Timer control

#include <emulator/bkEmu.h>
#include <emulator/bkInput.h>
#include <emulator/bkScreen.h>
#include "keyboard/ps2_keyboard.h"

uint8_t RamBuffer[RAM_AVAILABLE] __attribute__(( section(".sram2") ));
uint8_t basic[24448] __attribute__(( section(".sram2") ));
uint8_t monitor[8192] __attribute__(( section(".sram2") ));

pdp_regs pdp;

flag_t bkmodel = 0;
flag_t io_stop_happened;
uint16_t port0177664;
unsigned short last_branch;
bk::bkScreen screen;

const int TICK_RATE = 3000000; /* CPU clock speed */

void bk_setup()
{
	bk_reset();
}

int32_t bk_loop()
{
	pdp_regs* p = &pdp;
	register int result; /* result of execution */
	int result2 = OK; /* result of error handling */
	int rtt = 0; /* rtt don't trap yet flag */
	int32_t returnValue = 0;

	//Uint32 last_screen_update = SDL_GetTicks();
	//double timing_delta = ticks - SDL_GetTicks() * (TICK_RATE/1000.0);

	/*
	 * Fetch and execute the instruction.
	 */

	result = ll_word(p, p->regs[PC], &p->ir);
	p->regs[PC] += 2;
	if (result == OK)
	{
		result = (itab[p->ir >> 6].func)(p);
		//timing(p);
	}

	/*
	 * Mop up the mess.
	 */

	if (result != OK)
	{
		switch (result)
		{
		case BUS_ERROR: /* vector 4 */
			ticks += 64;
			break;
		case ODD_ADDRESS:
			result2 = service((d_word) 04);
			break;
		case CPU_ILLEGAL: /* vector 10 */
			result2 = service((d_word) 010);
			break;
		case CPU_BPT: /* vector 14 */
			result2 = service((d_word) 014);
			break;
		case CPU_EMT: /* vector 30 */
			result2 = service((d_word) 030);
			break;
		case CPU_TRAP: /* vector 34 */
			result2 = service((d_word) 034);
			break;
		case CPU_IOT: /* vector 20 */
			result2 = service((d_word) 020);
			break;
		case CPU_WAIT:
			in_wait_instr = 1;
			result2 = OK;
			break;
		case CPU_RTT:
			rtt = 1;
			result2 = OK;
			break;
		case CPU_HALT:
			io_stop_happened = 4;
			result2 = service((d_word) 004);
			break;
		default:
			// Unexpected return
			//flag = 0;
			result2 = OK;
			break;
		}
		if (result2 != OK)
		{
			// Double trap
			ll_word(p, 0177716, &p->regs[PC]);
			p->regs[PC] &= 0177400;
		}
	}

	// Keyboard input
	int32_t scanCode = Ps2_GetScancode();
	if (scanCode > 0)
	{
		if ((scanCode & 0xFF00) == 0xF000)
		{
			// key up

			scanCode = ((scanCode & 0xFF0000) >> 8 | (scanCode & 0xFF));
			if (!OnKey(scanCode, true))
			{
				returnValue = scanCode;
			}
		}
		else
		{
			// key down
			OnKey(scanCode, false);
		}
	}

	if ((p->psw & 020) && (rtt == 0))
	{ /* trace bit */
		if (service((d_word) 014) != OK)
		{
			// Double trap
			ll_word(p, 0177716, &p->regs[PC]);
			p->regs[PC] &= 0177400;
			p->regs[SP] = 01000; /* whatever */
		}
	}
	rtt = 0;
	p->total++;

	//if (nflag)
	//	sound_flush();

//		if (bkmodel && ticks >= ticks_timer) {
//			scr_sync();
//			if (timer_intr_enabled) {
//				ev_register(TIMER_PRI, service, 0, 0100);
//			}
//			ticks_timer += half_frame_delay;
//		}

	int priority = (p->psw >> 5) & 7;
	if (pending_interrupts && priority != 7)
	{
		ev_fire(priority);
	}

	return returnValue;
}

void bk_reset()
{
	for (int x = 0; x < 8; ++x)
	{
		pdp.regs[x] = 0;
	}

	pdp.ir = 0;
	//pdp.psw = 0200;

	pdp.regs[PC] = 0x8000;
	//ll_word(&pdp, 0177716, &pdp.regs[PC]);
//	pdp.regs[PC] &= 0177400;

	for (uint32_t* ram = (uint32_t*) RamBuffer;
			ram < (uint32_t*) &RamBuffer[RAM_AVAILABLE]; ram++)
	{
		*ram = 0xFF00FF;
	}

	port0177664 = 01330;
}

/*
 * Load a byte from the given address.
 */
extern "C" int ll_byte(pdp_regs* p, c_addr addr, d_byte* byte)
{
	if (addr >= (uint16_t) 0xFF80)
	{
		// I/O Ports

		switch (addr)
		{
		case TTY_REG:
			*byte = port0177660;
			break;
		case TTY_REG + 2:
			*byte = port0177662;
			port0177660 &= ~0x80;
			break;
		case TTY_REG + 4:
			*byte = (uint8_t)port0177664;
			break;
		case TTY_REG + 5:
			*byte = (uint8_t)port0177664 >> 8;
			break;
		case IO_REG:
			*byte = (uint8_t)port0177716;
			break;
		case IO_REG + 1:
			*byte = (uint8_t)port0177716 >> 8;
			break;
		default:
			*byte = 0;
			break;
		}
	}
	else if (addr >= (uint16_t) 0xA000)
	{
		// ROM Basic
		*byte = basic[addr - (uint16_t) 0xA000];
	}
	else if (addr >= (uint16_t) 0x8000)
	{
		// ROM Monitor
		*byte = monitor[addr - (uint16_t) 0x8000];
	}
	else if (addr >= (uint16_t) 0x4000)
	{
		// Video RAM
		*byte = screen.getVideoRam(addr);
	}
	else
	{
		// RAM
		*byte = RamBuffer[addr];
	}

	return OK;
}

/*
 * Load a word from the given address.
 */
extern "C" int ll_word(pdp_regs* p, c_addr addr, d_word* word)
{
	if (addr & 0x1)
	{
		int result;
		uint8_t byte1;
		if ((result = ll_byte(p, addr, &byte1)) != OK)
		{
			return result;
		}

		uint8_t byte2;
		if ((result = ll_byte(p, addr + 1, &byte2)) != OK)
		{
			return result;
		}

		*word = (byte2 << 8) | byte1;

		return OK;
	}

	if (addr >= (uint16_t) 0xFF80)
	{
		// I/O port

		switch (addr)
		{
		case TTY_REG:
			*word = (uint16_t)port0177660;
			break;
		case TTY_REG + 2:
			*word = (uint16_t)port0177662;
			port0177660 &= ~0x80;
			break;
		case TTY_REG + 4:
			*word = port0177664;
			break;
		case IO_REG:
			*word = port0177716;
			break;
		default:
			*word = 0;
			break;
		}
	}
	else if (addr >= (uint16_t)0xA000)
	{
		// ROM Basic
		*word = ((uint16_t*) basic)[(addr - (uint16_t)0xA000) >> 1];
	}
	else if (addr >= (uint16_t)0x8000)
	{
		// ROM Monitor
		*word = ((uint16_t*) monitor)[(addr - (uint16_t)0x8000) >> 1];
	}
	else if (addr >= (uint16_t)0x4000)
	{
		// Video RAM
		uint16_t loByte = screen.getVideoRam(addr);
		uint16_t hiByte = screen.getVideoRam(addr + 1);
		*word = (hiByte << 8) & loByte;
	}
	else
	{
		// RAM
		*word = ((uint16_t*)RamBuffer)[addr >> 1];
	}

	return OK;
}

void updateScreenMode(pdp_regs* p)
{
	d_word screenMode;
	d_word inversion;
	ll_word(p, 0x10, &screenMode);
	ll_word(p, 0x12, &inversion);

	uint8_t mode;
	if (screenMode)
	{
		// 256x256
		mode = 0x80;
	}
	else
	{
		// 512x256
		mode = 0x00;
	}

	screen.setMode(mode);
}

void updateScreenOffset()
{
	screen.setOffset(port0177664 & 0xFF);
}

/*
 * Store a byte at the given address.
 */
extern "C" int sl_byte(pdp_regs* p, c_addr addr, d_byte byte)
{
	if (addr >= (uint16_t) 0xFF80)
	{
		// I/O port

		switch (addr)
		{
		case TTY_REG:
			/* only let them set IE */
			port0177660 = (port0177660 & ~0x40) | (byte & 0x40);
			break;
		case TTY_REG + 2:
			port0177662 = (port0177662 & 0xFF00) | byte;
			break;
		case TTY_REG + 3:
			port0177662 = (port0177662 & 0xFF) | (byte >> 8);
			break;
		case TTY_REG + 4:
			port0177664 = (port0177664 & 0xFF00) | byte;
			updateScreenOffset();
			break;
		case TTY_REG + 5:
			port0177664 = (port0177664 & 0xFF) | (byte >> 8);
			updateScreenOffset();
			break;
		default:
			break;
		}
	}
	else if (addr >= (uint16_t) 0xA000)
	{
		// Can't write to ROM
	}
	else if (addr >= (uint16_t) 0x4000)
	{
		// Video RAM
		screen.setVideoRam(addr, byte);
	}
	else
	{
		// RAM
		RamBuffer[addr] = byte;

		c_addr wordAddr = addr >> 1;
		if (wordAddr == 0x08 || wordAddr == 0x09)
		{
			updateScreenMode(p);
		}
	}

	return OK;
}

/*
 *  Store a word at the given address.
 */
extern "C" int sl_word(pdp_regs* p, c_addr addr, d_word word)
{
	if (addr & 0x1)
	{
		int result;
		if ((result = sl_byte(p, addr, (uint8_t)word)) != OK)
		{
			return result;
		}

		if ((result = sl_byte(p, addr + 1, (uint8_t)(word >> 8))) != OK)
		{
			return result;
		}

		return ODD_ADDRESS;
	}

	if (addr >= (uint16_t)0xFF80)
	{
		// I/O port

		switch (addr)
		{
		case TTY_REG:
			/* only let them set IE */
			port0177660 = (port0177660 & ~0x40) | (word & 0x40);
			break;
		case TTY_REG + 4:
			port0177664 = (word & 01377);
			updateScreenOffset();
			break;
		default:
			break;
		}
	}
	else if (addr >= (uint16_t)0xA000)
	{
		// Can't write to ROM
	}
	else if (addr >= (uint16_t)0x4000)
	{
		// Video RAM
		screen.setVideoRam(addr, word & 0xFF);
		screen.setVideoRam(addr + 1, word >> 8);
	}
	else
	{
		// RAM
		c_addr wordAddr = addr >> 1;
		((uint16_t*)RamBuffer)[wordAddr] = word;

		if (wordAddr == 0x08 || wordAddr == 0x09)
		{
			updateScreenMode(p);
		}
	}

	return OK;
}

/*
 * Reset the UNIBUS devices.
 */
extern "C" void q_reset()
{
}

