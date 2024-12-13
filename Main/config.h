#ifndef __CONFIG_H
#define __CONFIG_H

// Visible screen area (without borders)
#define H_SIZE 848
#define V_SIZE 480
extern uint8_t VideoRam[H_SIZE * V_SIZE];

//#define VIDEO_MODE_720x400
#define VIDEO_MODE_848x480

#ifdef VIDEO_MODE_848x480

// Pixel clock 33.75 MHz
#define VIDEO_MODE_PLL3N 54
#define VIDEO_MODE_PLL3R 8
#define VIDEO_MODE_PLL3VCOSEL RCC_PLL3VCOWIDE

// Horizontal 848 pixels
#define VIDEO_MODE_H_SYNC 112
#define VIDEO_MODE_H_BACKPORCH 112
#define VIDEO_MODE_H_WIDTH 848
#define VIDEO_MODE_H_FRONTPORCH 16
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AH

// Vertical 480 lines
#define VIDEO_MODE_V_SYNC 8
#define VIDEO_MODE_V_BACKPORCH 23
#define VIDEO_MODE_V_HEIGHT 480
#define VIDEO_MODE_V_FRONTPORCH 6
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AH

#endif

#ifdef VIDEO_MODE_720x400

// Pixel clock 33.75 MHz
#define PIXEL_CLOCK_PLL3N 34
#define PIXEL_CLOCK_PLL3R 6
#define PIXEL_CLOCK_PLL3VCOSEL RCC_PLL3VCOMEDIUM




#endif


#endif
