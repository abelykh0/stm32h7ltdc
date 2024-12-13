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
uint8_t VideoRam[H_SIZE * V_SIZE];// __attribute__(( section(".sram2") ));

static Display::Screen screen;

static void USB_DEVICE_Init();
static void MapFlash();
static void PrepareClut();
static void LtdcInit();

extern "C" void initialize()
{
	PrepareClut();
}

extern "C" void setup()
{
	LtdcInit();
	USB_DEVICE_Init();
	MapFlash();

	memset(VideoRam               , 0x10, H_SIZE * V_SIZE);

	for (uint8_t color = 0; color < 4; color++)
	{
		memset(VideoRam + H_SIZE * 6 * color, color == 0 ? 0x3F : color, H_SIZE * 6);
	}
	for (uint8_t color = 0; color < 4; color++)
	{
		memset(VideoRam + H_SIZE * 6 * (color + 4), color == 0 ? 0x3F : color << 2, H_SIZE * 6);
	}
	for (uint8_t color = 0; color < 4; color++)
	{
		memset(VideoRam + H_SIZE * 6 * (color + 8), color == 0 ? 0x3F : color  << 4, H_SIZE * 6);
	}


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

		// R3,R4
		uint32_t paletteR = i & 0x0003;
		uint32_t r = paletteR << 3;
		r <<= 16;

		// G3,G4
		uint32_t paletteG = i & 0x000C;
		uint32_t g = paletteG << 1; // >> 2 << 3
		g <<= 8;

		// B2,B3
		uint32_t paletteB = i & 0x0030;
		uint32_t b = paletteB >> 2; // >> 4 << 2

		L8Clut[i] = a | r | g | b;
	}
}

static void LtdcInit()
{
	// Pixel clock
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLL3.PLL3M = 5;
	PeriphClkInitStruct.PLL3.PLL3P = 2;
	PeriphClkInitStruct.PLL3.PLL3Q = 2;
	PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
	PeriphClkInitStruct.PLL3.PLL3FRACN = 0;

	PeriphClkInitStruct.PLL3.PLL3N = VIDEO_MODE_PLL3N;
	PeriphClkInitStruct.PLL3.PLL3R = VIDEO_MODE_PLL3R;
	PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	LTDC_LayerCfgTypeDef pLayerCfg = {0};

	hltdc.Instance = LTDC;
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IIPC;
	hltdc.Init.Backcolor.Blue = 0;
	hltdc.Init.Backcolor.Green = 0;
	hltdc.Init.Backcolor.Red = 0;

	// Horizontal
	hltdc.Init.HSPolarity = VIDEO_MODE_H_POLARITY;
	hltdc.Init.HorizontalSync = VIDEO_MODE_H_SYNC - 1;
	hltdc.Init.AccumulatedHBP = VIDEO_MODE_H_SYNC + VIDEO_MODE_H_BACKPORCH - 1;
	hltdc.Init.AccumulatedActiveW = VIDEO_MODE_H_SYNC + VIDEO_MODE_H_BACKPORCH + VIDEO_MODE_H_WIDTH - 1;
	hltdc.Init.TotalWidth = VIDEO_MODE_H_SYNC + VIDEO_MODE_H_BACKPORCH + VIDEO_MODE_H_WIDTH + VIDEO_MODE_H_FRONTPORCH - 1;

	// Vertical
	hltdc.Init.VSPolarity = VIDEO_MODE_V_POLARITY;
	hltdc.Init.VerticalSync = VIDEO_MODE_V_SYNC - 1;
	hltdc.Init.AccumulatedVBP = VIDEO_MODE_V_SYNC + VIDEO_MODE_V_BACKPORCH - 1;
	hltdc.Init.AccumulatedActiveH = VIDEO_MODE_V_SYNC + VIDEO_MODE_V_BACKPORCH + VIDEO_MODE_V_HEIGHT - 1;
	hltdc.Init.TotalHeigh = VIDEO_MODE_V_SYNC + VIDEO_MODE_V_BACKPORCH + VIDEO_MODE_V_HEIGHT + VIDEO_MODE_V_FRONTPORCH - 1;

	if (HAL_LTDC_Init(&hltdc) != HAL_OK)
	{
		Error_Handler();
	}

	pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_L8;
	pLayerCfg.Alpha = 0xff;
	pLayerCfg.Alpha0 = 0xff;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	pLayerCfg.Backcolor.Blue = 0;
	pLayerCfg.Backcolor.Green = 0xff;
	pLayerCfg.Backcolor.Red = 0;

	pLayerCfg.WindowX0 = (VIDEO_MODE_H_WIDTH - H_SIZE) / 2;
	pLayerCfg.WindowX1 = H_SIZE - 1;
	pLayerCfg.WindowY0 = (VIDEO_MODE_V_HEIGHT - V_SIZE) / 2;
	pLayerCfg.WindowY1 = V_SIZE - 1;
	pLayerCfg.ImageWidth = H_SIZE;
	pLayerCfg.ImageHeight = V_SIZE;
	pLayerCfg.FBStartAdress = (uint32_t)VideoRam;

	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_LTDC_ConfigCLUT(&hltdc, L8Clut, 256, LTDC_LAYER_1);
	HAL_LTDC_EnableCLUT(&hltdc, LTDC_LAYER_1);
}
