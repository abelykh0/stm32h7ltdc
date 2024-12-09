#include "startup.h"

#include "stm32h7xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_video_if.h"
#include "ltdc.h"
#include "quadspi.h"
#include "w25qxx_qspi.h"
#include "user_diskio.h"
#include "fatfs.h"

#include "screen/screen.h"
#include "keyboard/ps2_keyboard.h"
#include "emulator/bkEmu.h"

extern JPEG_HandleTypeDef hjpeg;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern USBD_HandleTypeDef hUsbDeviceFS;

static uint32_t L8Clut[256];
uint8_t VideoRam[200 * 200];// __attribute__(( section(".sram2") ));

static Display::Screen screen;

static void USB_DEVICE_Init();
static void MapFlash();
static void PrepareClut();

extern "C" void initialize()
{
	PrepareClut();
}

extern "C" void setup()
{
	USB_DEVICE_Init();
	MapFlash();

	HAL_LTDC_ConfigCLUT(&hltdc, L8Clut, 256, LTDC_LAYER_1);
	HAL_LTDC_EnableCLUT(&hltdc, LTDC_LAYER_1);

	memset(VideoRam               , 0x10, 200 * 40);
	memset(VideoRam + 200 * 40 * 1, 0x03, 200 * 40); // red
	memset(VideoRam + 200 * 40 * 2, 0x0C, 200 * 40); // green
	memset(VideoRam + 200 * 40 * 3, 0x30, 200 * 40); // blue
	memset(VideoRam + 200 * 40 * 4, 0xFF, 200 * 40);


/*
	// Read ROMs from external flash
	// (the built-in flash is only 128K)
	if (f_mount(&USERFatFS, (TCHAR*)u"1:/", 1) == FR_OK)
	{
		FIL file;
		if (f_open(&file, (const TCHAR*)u"1:/BASIC10.ROM", FA_READ) == FR_OK)
		{
			UINT bytesRead = sizeof(basic);
			f_read(&file, basic, bytesRead, &bytesRead);
			f_close(&file);
		}

		if (f_open(&file, (const TCHAR*)u"1:/MONIT10.ROM", FA_READ) == FR_OK)
		{
			UINT bytesRead = sizeof(monitor);
			f_read(&file, monitor, bytesRead, &bytesRead);
			f_close(&file);
		}

		f_mount(nullptr, nullptr, 1);
	}
*/


}

extern "C" void loop()
{
	//bk_loop();
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
	HAL_Delay(500);
}

static void USB_DEVICE_Init()
{
	  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
	  {
	    Error_Handler();
	  }

	  // Defaults are 128, 64, 128 (320 total)
	  HAL_PCDEx_SetRxFiFo(&hpcd_USB_OTG_FS,    48);
	  HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_FS, 0, 16);
	  HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_FS, 1, 256);

	  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_VIDEO) != USBD_OK)
	  {
	    Error_Handler();
	  }
	  if (USBD_VIDEO_RegisterInterface(&hUsbDeviceFS, &USBD_VIDEO_fops_FS) != USBD_OK)
	  {
	    Error_Handler();
	  }
	  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
	  {
	    Error_Handler();
	  }
}

static void MapFlash()
{
	w25qxx_Init();
	w25qxx_EnterQPI();
	w25qxx_Startup(w25qxx_NormalMode); // w25qxx_DTRMode
}

static void PrepareClut()
{
	for (uint32_t i = 0; i < 256; i++)
	{
		// xxBBGGRR > ARBG
		uint32_t a = 0xff000000;

		uint8_t paletteR = i & 0x0003;
		uint32_t r = 85 * paletteR;
		r <<= 16;

		uint8_t paletteG = i & 0x000C;
		paletteG >>= 2;
		uint32_t g = 85 * paletteG;
		g <<= 8;

		uint8_t paletteB = i & 0x0030;
		paletteB >>= 4;
		uint32_t b = 85 * paletteB;

		L8Clut[i] = a | r | g | b;
	}
}
