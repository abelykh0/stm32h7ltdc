#ifndef __CONFIG_H
#define __CONFIG_H

// Visible screen area (without borders)
#define H_SIZE 720 //512
#define V_SIZE 400 //256
#define TEXT_COLUMNS (H_SIZE / 8)
#define TEXT_ROWS (V_SIZE / 8)
#define BORDER_COLOR 0x30 // ..BBGGRR
extern uint8_t VideoRam[H_SIZE * V_SIZE];

// Wide modes
//#define VIDEO_MODE_640x400 // 250 K
#define VIDEO_MODE_720x400 // 281.3 K
//#define VIDEO_MODE_848x480 // 397.5 K
//#define VIDEO_MODE_1280x720
//#define VIDEO_MODE_1600x900
//#define VIDEO_MODE_1920x1080

// Classic modes (4:3)
//#define VIDEO_MODE_640x480 // 300 K
//#define VIDEO_MODE_800x600 // 468.8 K
//#define VIDEO_MODE_1024x768
//#define VIDEO_MODE_1280x960

#ifdef VIDEO_MODE_1920x1080

// Pixel clock 148.5 MHz
#define VIDEO_MODE_PLL3M 10
#define VIDEO_MODE_PLL3N 297
#define VIDEO_MODE_PLL3R 5
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_1

// Horizontal 1920 pixels
#define VIDEO_MODE_H_SYNC 44
#define VIDEO_MODE_H_BACKPORCH 148
#define VIDEO_MODE_H_WIDTH 1920
#define VIDEO_MODE_H_FRONTPORCH 88
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AH

// Vertical 1080 lines @ 60 Hz
#define VIDEO_MODE_V_SYNC 5
#define VIDEO_MODE_V_BACKPORCH 36
#define VIDEO_MODE_V_HEIGHT 1080
#define VIDEO_MODE_V_FRONTPORCH 4
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AH

#endif


#ifdef VIDEO_MODE_640x400

// Pixel clock 25.175 MHz
#define VIDEO_MODE_PLL3M 13
#define VIDEO_MODE_PLL3N 144
#define VIDEO_MODE_PLL3R 11
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_0

// Horizontal 640 pixels
#define VIDEO_MODE_H_SYNC 96
#define VIDEO_MODE_H_BACKPORCH 48
#define VIDEO_MODE_H_WIDTH 640
#define VIDEO_MODE_H_FRONTPORCH 16
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AL

// Vertical 400 lines @ 70 Hz
#define VIDEO_MODE_V_SYNC 2
#define VIDEO_MODE_V_BACKPORCH 35
#define VIDEO_MODE_V_HEIGHT 400
#define VIDEO_MODE_V_FRONTPORCH 12
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AH

#endif

#ifdef VIDEO_MODE_1280x960

// Pixel clock 108.0 MHz
#define VIDEO_MODE_PLL3M 5
#define VIDEO_MODE_PLL3N 108
#define VIDEO_MODE_PLL3R 5
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_2

// Horizontal 1280 pixels
#define VIDEO_MODE_H_SYNC 112
#define VIDEO_MODE_H_BACKPORCH 312
#define VIDEO_MODE_H_WIDTH 1280
#define VIDEO_MODE_H_FRONTPORCH 96
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AH

// Vertical 960 lines @ 60 Hz
#define VIDEO_MODE_V_SYNC 3
#define VIDEO_MODE_V_BACKPORCH 36
#define VIDEO_MODE_V_HEIGHT 960
#define VIDEO_MODE_V_FRONTPORCH 1
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AH

#endif

#ifdef VIDEO_MODE_1024x768

// Pixel clock 65.0 MHz
#define VIDEO_MODE_PLL3M 5
#define VIDEO_MODE_PLL3N 65
#define VIDEO_MODE_PLL3R 5
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_2

// Horizontal 1024 pixels
#define VIDEO_MODE_H_SYNC 136
#define VIDEO_MODE_H_BACKPORCH 160
#define VIDEO_MODE_H_WIDTH 1024
#define VIDEO_MODE_H_FRONTPORCH 24
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AL

// Vertical 768 lines @ 60 Hz
#define VIDEO_MODE_V_SYNC 6
#define VIDEO_MODE_V_BACKPORCH 29
#define VIDEO_MODE_V_HEIGHT 768
#define VIDEO_MODE_V_FRONTPORCH 3
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AL

#endif

#ifdef VIDEO_MODE_800x600

// Pixel clock 40.0 MHz
#define VIDEO_MODE_PLL3M 5
#define VIDEO_MODE_PLL3N 40
#define VIDEO_MODE_PLL3R 5
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_2

// Horizontal 800 pixels
#define VIDEO_MODE_H_SYNC 128
#define VIDEO_MODE_H_BACKPORCH 88
#define VIDEO_MODE_H_WIDTH 800
#define VIDEO_MODE_H_FRONTPORCH 40
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AH

// Vertical 600 lines @ 60 Hz
#define VIDEO_MODE_V_SYNC 4
#define VIDEO_MODE_V_BACKPORCH 23
#define VIDEO_MODE_V_HEIGHT 600
#define VIDEO_MODE_V_FRONTPORCH 1
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AH

#endif

#ifdef VIDEO_MODE_1280x720

// Pixel clock 74.25 MHz
#define VIDEO_MODE_PLL3M 10
#define VIDEO_MODE_PLL3N 297
#define VIDEO_MODE_PLL3R 10
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_1

// Horizontal 1280 pixels
#define VIDEO_MODE_H_SYNC 40
#define VIDEO_MODE_H_BACKPORCH 220
#define VIDEO_MODE_H_WIDTH 1280
#define VIDEO_MODE_H_FRONTPORCH 110
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AH

// Vertical 720 lines
#define VIDEO_MODE_V_SYNC 5
#define VIDEO_MODE_V_BACKPORCH 20
#define VIDEO_MODE_V_HEIGHT 720
#define VIDEO_MODE_V_FRONTPORCH 5
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AH

#endif

#ifdef VIDEO_MODE_1600x900

// Pixel clock 106.47 MHz
#define VIDEO_MODE_PLL3M 10
#define VIDEO_MODE_PLL3N 213
#define VIDEO_MODE_PLL3R 5
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_1

// Horizontal 1440 pixels
#define VIDEO_MODE_H_SYNC 152
#define VIDEO_MODE_H_BACKPORCH 232
#define VIDEO_MODE_H_WIDTH 1440
#define VIDEO_MODE_H_FRONTPORCH 80
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AL

// Vertical 900 lines @ 60 Hz
#define VIDEO_MODE_V_SYNC 3
#define VIDEO_MODE_V_BACKPORCH 28
#define VIDEO_MODE_V_HEIGHT 900
#define VIDEO_MODE_V_FRONTPORCH 1
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AH

#endif

#ifdef VIDEO_MODE_848x480

// Pixel clock 33.75 MHz
#define VIDEO_MODE_PLL3M 5
#define VIDEO_MODE_PLL3N 54
#define VIDEO_MODE_PLL3R 8
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_2

// Horizontal 848 pixels
#define VIDEO_MODE_H_SYNC 112
#define VIDEO_MODE_H_BACKPORCH 112
#define VIDEO_MODE_H_WIDTH 848
#define VIDEO_MODE_H_FRONTPORCH 16
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AH

// Vertical 480 lines @ 60 Hz
#define VIDEO_MODE_V_SYNC 8
#define VIDEO_MODE_V_BACKPORCH 23
#define VIDEO_MODE_V_HEIGHT 480
#define VIDEO_MODE_V_FRONTPORCH 6
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AH

#endif

#ifdef VIDEO_MODE_720x400

// Pixel clock 28.322 MHz
#define VIDEO_MODE_PLL3M 13
#define VIDEO_MODE_PLL3N 162
#define VIDEO_MODE_PLL3R 11
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_0

// Horizontal 720 pixels
#define VIDEO_MODE_H_SYNC 108
#define VIDEO_MODE_H_BACKPORCH 51
#define VIDEO_MODE_H_WIDTH 720
#define VIDEO_MODE_H_FRONTPORCH 15
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AL

// Vertical 400 lines @ 70 Hz
#define VIDEO_MODE_V_SYNC 2
#define VIDEO_MODE_V_BACKPORCH 32
#define VIDEO_MODE_V_HEIGHT 400
#define VIDEO_MODE_V_FRONTPORCH 11
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AL

#endif

#ifdef VIDEO_MODE_640x480

// Pixel clock 25.175 MHz
#define VIDEO_MODE_PLL3M 13
#define VIDEO_MODE_PLL3N 144
#define VIDEO_MODE_PLL3R 11
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_0

// Horizontal 640 pixels
#define VIDEO_MODE_H_SYNC 96
#define VIDEO_MODE_H_BACKPORCH 48
#define VIDEO_MODE_H_WIDTH 640
#define VIDEO_MODE_H_FRONTPORCH 16
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AL

// Vertical 480 lines @ 60 Hz
#define VIDEO_MODE_V_SYNC 2
#define VIDEO_MODE_V_BACKPORCH 33
#define VIDEO_MODE_V_HEIGHT 480
#define VIDEO_MODE_V_FRONTPORCH 33
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AL

#endif


#endif
