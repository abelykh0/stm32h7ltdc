#include "startup.h"

#include "stm32h7xx_hal.h"
#include "ltdc.h"
#include "quadspi.h"
#include "w25qxx_qspi.h"
#include "user_diskio.h"
#include "fatfs.h"

#include "screen/screen.h"
#include "keyboard/ps2_keyboard.h"
#include "emulator/bkEmu.h"
#include "demo_colors/demo_colors.h"

extern JPEG_HandleTypeDef hjpeg;

static uint32_t L8Clut[256];
uint8_t VideoRam[H_SIZE * V_SIZE];// __attribute__(( section(".sram2") ));

static Display::Screen screen;

static void MapFlash();
static void PrepareClut();
static void LtdcInit();

extern "C" void initialize()
{
	PrepareClut();
}

extern "C" void setup()
{
	// Read ROMs from external flash
	// (the built-in flash is only 128K)
	MapFlash();
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

	LtdcInit();
	init_demo_colors();
}

extern "C" void loop()
{
	//bk_loop();
	loop_demo_colors();
	//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
	//HAL_Delay(500);
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
	LTDC_LayerCfgTypeDef pLayerCfg = {0};
	hltdc.Instance = LTDC;

	LTDC_InitTypeDef* init = &hltdc.Init;
	init->DEPolarity = LTDC_DEPOLARITY_AL;
	init->PCPolarity = LTDC_PCPOLARITY_IIPC;
	init->Backcolor.Blue = 0;
	init->Backcolor.Green = 0;
	init->Backcolor.Red = 0;

	// Horizontal
	init->HSPolarity = VIDEO_MODE_H_POLARITY;
	init->HorizontalSync = VIDEO_MODE_H_SYNC - 1;
	init->AccumulatedHBP = init->HorizontalSync + VIDEO_MODE_H_BACKPORCH;
	init->AccumulatedActiveW = init->AccumulatedHBP + VIDEO_MODE_H_WIDTH;
	init->TotalWidth = init->AccumulatedActiveW + VIDEO_MODE_H_FRONTPORCH;

	// Vertical
	init->VSPolarity = VIDEO_MODE_V_POLARITY;
	init->VerticalSync = VIDEO_MODE_V_SYNC - 1;
	init->AccumulatedVBP = init->VerticalSync + VIDEO_MODE_V_BACKPORCH;
	init->AccumulatedActiveH = init->AccumulatedVBP + VIDEO_MODE_V_HEIGHT;
	init->TotalHeigh = init->AccumulatedActiveH + VIDEO_MODE_V_FRONTPORCH;

	if (HAL_LTDC_Init(&hltdc) != HAL_OK)
	{
		Error_Handler();
	}

	pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_L8;
	pLayerCfg.Alpha = 0xff;
	pLayerCfg.Alpha0 = 0xff;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;

	uint32_t argb = L8Clut[BORDER_COLOR];
	pLayerCfg.Backcolor.Blue = argb & 0xFF;
	pLayerCfg.Backcolor.Green = (argb >> 8) & 0xFF;
	pLayerCfg.Backcolor.Red = (argb >> 16) & 0xFF;

	pLayerCfg.WindowX0 = (VIDEO_MODE_H_WIDTH - H_SIZE) / 2;
	pLayerCfg.WindowX1 = pLayerCfg.WindowX0 + H_SIZE - 1;
	pLayerCfg.WindowY0 = (VIDEO_MODE_V_HEIGHT - V_SIZE) / 2;
	pLayerCfg.WindowY1 = pLayerCfg.WindowY0 + V_SIZE - 1;
	pLayerCfg.ImageWidth = H_SIZE;
	pLayerCfg.ImageHeight = V_SIZE;
	pLayerCfg.FBStartAdress = (uint32_t)VideoRam;

	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_LTDC_MspInit(&hltdc);

	// Pixel clock
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLL3.PLL3P = 2;
	PeriphClkInitStruct.PLL3.PLL3Q = 2;
	PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
	PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;

	PeriphClkInitStruct.PLL3.PLL3M = VIDEO_MODE_PLL3M;
	PeriphClkInitStruct.PLL3.PLL3N = VIDEO_MODE_PLL3N;
	PeriphClkInitStruct.PLL3.PLL3R = VIDEO_MODE_PLL3R;
	PeriphClkInitStruct.PLL3.PLL3RGE = VIDEO_MODE_PLL3RGE;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	// Unfortunately, pin A1 is an LED, take it back :)
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_LTDC_ConfigCLUT(&hltdc, L8Clut, 256, LTDC_LAYER_1);
	HAL_LTDC_EnableCLUT(&hltdc, LTDC_LAYER_1);
}
