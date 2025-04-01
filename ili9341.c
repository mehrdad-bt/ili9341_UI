/*
 * ili9341.c
 *
 *  Created on: Mar 21, 2025
 *      Author: zbook
 */

#include "ili9341.h"
#include "font.h"
#include "string.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"
#include <stdlib.h>

static SPI_HandleTypeDef *hspi;
static GPIO_TypeDef* CS_port;
static uint16_t CS_pin;
static GPIO_TypeDef* DC_port;
static uint16_t DC_pin;
static GPIO_TypeDef* RESET_port;
static uint16_t RESET_pin;


//static SPI_HandleTypeDef *hspi_touch;
//static GPIO_TypeDef* touch_cs_port;
//static uint16_t touch_cs_pin;







//write a command

void ILI9341_WriteCommand(uint8_t cmd)

{
	HAL_GPIO_WritePin(DC_port,DC_pin, GPIO_PIN_RESET); //TELL SCREEN WE ARE SENDING A COMMAND
	HAL_GPIO_WritePin(CS_port,CS_pin, GPIO_PIN_RESET); //TELL SCREEN TO LISTEN
	HAL_SPI_Transmit(hspi, &cmd, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(CS_port,CS_pin, GPIO_PIN_SET); //TELL SCREEN STOP LISTENING
}


void ILI9341_WriteData(uint8_t data)
{
    HAL_GPIO_WritePin(DC_port, DC_pin, GPIO_PIN_SET); // DC high for data
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET); // CS low to select
    HAL_SPI_Transmit(hspi, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET); // CS high to deselect
}


void ILI9341_WriteData16(uint16_t data) //SENDING 16 BIT DATA
{
	HAL_GPIO_WritePin(DC_port,DC_pin, GPIO_PIN_SET); //TELL SCREEN WE ARE SENDING DATA
	HAL_GPIO_WritePin(CS_port,CS_pin, GPIO_PIN_RESET); //TELL SCREEN TO LISTEN
	uint8_t buffer[2];
	buffer[0] = data >> 8; //send high byte first
	buffer[1] = data & 0xff; //send low byte next
	HAL_SPI_Transmit(hspi, buffer, 2, HAL_MAX_DELAY); // send 16 bit data
	HAL_GPIO_WritePin(CS_port,CS_pin, GPIO_PIN_SET); //TELL SCREEN STOP LISTENING


}



//setting the address window

void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	ILI9341_WriteCommand(ILI9341_CASET); //COLUMN ADDRESS SET
	ILI9341_WriteData(x0 >> 8);
	ILI9341_WriteData(x0 & 0Xff);
	ILI9341_WriteData(x1 >> 8);
	ILI9341_WriteData(x1 & 0Xff);

	ILI9341_WriteCommand(ILI9341_PASET); //PAGE ADDRESS SET
	ILI9341_WriteData(y0 >> 8);
	ILI9341_WriteData(y0 & 0Xff);
	ILI9341_WriteData(y1 >> 8);
	ILI9341_WriteData(y1 & 0Xff);

	ILI9341_WriteCommand(ILI9341_RAMWR); //MEMORY WRITE


}

//FILL ENTIRE SCREEN WITH ONE COLOR

void ILI9341_FillScreen(uint16_t color)
{
	ILI9341_SetAddressWindow(0, 0, 319, 239);
	for(uint32_t i = 0; i < 240*320 ; i++)
	{
		ILI9341_WriteData16(color); //send 16bit color data

	}



}


//SETTING THE SCREEN UP

void ILI9341_Init(SPI_HandleTypeDef *hspi_instance, GPIO_TypeDef* CS_port_instance, uint16_t CS_pin_instance, GPIO_TypeDef* DC_port_instance, uint16_t DC_pin_instance, GPIO_TypeDef* RESET_port_instance, uint16_t RESET_pin_instance)
{
	hspi = hspi_instance;
	CS_port = CS_port_instance;
	CS_pin = CS_pin_instance;
	DC_port = DC_port_instance;
	DC_pin = DC_pin_instance;
	RESET_port = RESET_port_instance;
	RESET_pin = RESET_pin_instance;


	//Reset the display

	HAL_GPIO_WritePin(RESET_port, RESET_pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RESET_port, RESET_pin, GPIO_PIN_SET);
	HAL_Delay(120);

	//Initialization sequence
	ILI9341_WriteCommand(ILI9341_SWRESET); //SOFTWARE RESET
	HAL_Delay(150);

	ILI9341_WriteCommand(ILI9341_PWCTR1); //POWER CONTROL 1
	ILI9341_WriteData(0X23);

    ILI9341_WriteCommand(ILI9341_PWCTR2); // Power control 2
    ILI9341_WriteData(0x10);

    ILI9341_WriteCommand(ILI9341_VMCTR1); // VCOM control 1
    ILI9341_WriteData(0x3E);
    ILI9341_WriteData(0x28);

    ILI9341_WriteCommand(ILI9341_VMCTR2); // VCOM control 2
    ILI9341_WriteData(0x86);

    ILI9341_WriteCommand(ILI9341_MADCTL); // Memory access control
    ILI9341_WriteData(0x28);

    ILI9341_WriteCommand(ILI9341_PIXFMT); // Pixel format
    ILI9341_WriteData(0x55);

    ILI9341_WriteCommand(ILI9341_SLPOUT); // Sleep out
    HAL_Delay(120);

    ILI9341_WriteCommand(ILI9341_DISPON); // Display on


}


void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	if(x>= 240 || y>=320) return;
	ILI9341_SetAddressWindow(x, y, x+1, y+1);
	ILI9341_WriteData16(color);

}



void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    // Check bounds
    if (x >= 240 || y >= 320) return;
    if ((x + w - 1) >= 240) w = 240 - x;
    if ((y + h - 1) >= 320) h = 320 - y;

    // Set the address window to the rectangle area
    ILI9341_SetAddressWindow(x, y, x + w - 1, y + h - 1);

    // Fill the rectangle with the specified color
    for (uint32_t i = 0; i < w * h; i++)
    {
        ILI9341_WriteData(color >> 8);    // Send high byte
        ILI9341_WriteData(color & 0xFF); // Send low byte
    }
}



void ILI9341_ScreenTest(void)
{
	  for(uint8_t i=0;i<239;i++)
	  {
		  for(uint8_t j=0;j<106;j++)
	  {
		  ILI9341_DrawPixel(i,j,COLOR_RED);
	  }
	  }

	  for(uint8_t i=0;i<239;i++)
	  {
		  for(uint8_t j=106;j<212;j++)
	  {
		  ILI9341_DrawPixel(i,j,COLOR_GREEN);
	  }
	  }

	  for(uint8_t i=0;i<239;i++)
	  {
		  for(uint16_t j=212;j<320;j++)
	  {
		  ILI9341_DrawPixel(i,j,COLOR_BLUE);
	  }
	  }

}

void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *img)
{

	ILI9341_SetAddressWindow(x, y, x + w  - 1, y + h - 1); //set adress window to image area

	for(uint32_t i = 0; i < w * h ; i++) //send image data to display
	{
		ILI9341_WriteData16(img[i]);

//		for(uint16_t j=0;j<h;j++)
//		{
//		ILI9341_WriteData16(img[j]);
//		}

	}

}


void ILI9341_DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg_color, uint16_t bg_color, uint8_t size)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        uint8_t line = font[(c - 32) * 5 + i]; // Get the font data for the character
        for (uint8_t j = 0; j < 8; j++)
        {
            if (line & 0x1) // Check if the pixel is set
            {
                if (size == 1)
                {
                    ILI9341_DrawPixel(x + i, y + j, fg_color);
                }
                else
                {
                    ILI9341_FillRectangle(x + (i * size), y + (j * size), size, size, fg_color);
                }
            }
            else if (bg_color != fg_color) // Draw background if needed
            {
                if (size == 1)
                {
                    ILI9341_DrawPixel(x + i, y + j, bg_color);
                }
                else
                {
                    ILI9341_FillRectangle(x + (i * size), y + (j * size), size, size, bg_color);
                }
            }
            line >>= 1; // Move to the next pixel in the column
        }
    }
}






// Draw a string on the screen
void ILI9341_DrawString(uint16_t x, uint16_t y, const char* str, uint16_t fg_color, uint16_t bg_color, uint8_t size)
{
	//calculate the total width of string
	uint16_t total_width = 0;
	const char *temp_str = str;
	while(*temp_str)
	{
		total_width += 6 * size; //5 pixels for character + 1 pixel for spacing
		temp_str++;
	}

	ILI9341_FillRectangle(x -3, y -3, total_width +3 ,( 8 * size )+3, bg_color); //draw the bg as a single rectangle


    while (*str) //draw each character
    {
        ILI9341_DrawChar(x, y, *str, fg_color, bg_color, size);
        x += 6 * size;
        str++;
    }
}



void ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;	
	int16_t err = dx +dy, e2;
	while(1) {
		ILI9341_DrawPixel(x0, y0, color);
		if(x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if(e2 >= dy) { err += dy; x0 +=sx; }
		if(e2 <= dx) { err += dx; y0 +=sy; }
	}
}

void ILI9341_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	
	while(x <y) {
		if( f >= 0) {
			y--;
			ddF_y +=2;
			f += ddF_y;
		}
		x++;
		ddF_x +=2;
		f +=ddF_x;
		ILI9341_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		ILI9341_DrawLine(x0 - x, y0 - y, x0 + x, y0 - y, color);
		ILI9341_DrawLine(x0 - y, y0 + x, x0 + y, y0 + x, color);
		ILI9341_DrawLine(x0 - y, y0 - x, x0 + y, y0 - x, color);
	}
}


void ILI9341_DrawRectangle(uint16_t x, uint16_t y,  uint16_t w,  uint16_t h,  uint16_t color)
{
	
	if(x >= ILI9341_WIDTH || y>= ILI9341_HEIGHT)
		return;
	
	if ((x + w -1) >= ILI9341_WIDTH)
		w = ILI9341_WIDTH - x;

	if ((y + h -1) >= ILI9341_HEIGHT)
		h = ILI9341_HEIGHT - y;	
	
	ILI9341_SetAddressWindow(x, y, x + w -1, y + h -1);
	
	uint32_t pixels = w *h;
	HAL_GPIO_WritePin(DC_port, DC_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_RESET);
	
	uint8_t color_data[2] = { color >>8 , color & 0xff};
	while(pixels--)
	{
		HAL_SPI_Transmit(hspi, color_data, 2, HAL_MAX_DELAY);
		
	}
		HAL_GPIO_WritePin(CS_port, CS_pin, GPIO_PIN_SET);
	
}	





//void Touch_Init(SPI_HandleTypeDef *hspi_instance, GPIO_TypeDef* touch_cs_port_instance,uint16_t touch_cs_pin_instance)
//{
//	//initialize touch controller spi and gpio setting

//	hspi_touch = hspi_instance;
//	touch_cs_port = touch_cs_port_instance;
//	touch_cs_pin = touch_cs_pin_instance;

//	//set the touch chip select high (inactive)
//	HAL_GPIO_WritePin(touch_cs_port, touch_cs_pin, GPIO_PIN_SET);

//}


//uint16_t Touch_Read(uint8_t command)
//{
//	//read touch data from touch controller

//	uint8_t tx_data[3] = {command, 0x00, 0x00};
//	uint8_t rx_data[3] = {0};

//	HAL_GPIO_WritePin(touch_cs_port, touch_cs_pin, GPIO_PIN_RESET);
//	HAL_SPI_TransmitReceive(hspi_touch, tx_data, rx_data, 3, HAL_MAX_DELAY);
//	HAL_GPIO_WritePin(touch_cs_port, touch_cs_pin, GPIO_PIN_SET);

//	return ((rx_data[1] << 8)  | rx_data[2] ) >> 3;



//}

//void Touch_GetCoordinates(uint16_t *x, uint16_t *y, uint16_t *z)
//{
//	//read x,y,z (pressure) coordinates

//	*x = Touch_Read(0xD0);
//	*y = Touch_Read(0x90);
//	*z = Touch_Read(0xb0);


//}


//void Touch_Calibrate(uint16_t raw_x, uint16_t raw_y, uint16_t *display_x, uint16_t *display_y)
//{
//	//calibrate raw touch coordinates to display coordinates

//	uint16_t raw_x_min = 100;
//	uint16_t raw_x_max = 2000;
//	uint16_t raw_y_min = 100;
//	uint16_t raw_y_max = 2000;

////	*display_x = raw_x;
////	*display_y = raw_y;
//    *display_x = (raw_x - raw_x_min) * 239 / (raw_x_max -raw_x_min);
// 	*display_y = (raw_y - raw_y_min) * 319 / (raw_y_max -raw_y_min);

////	if(*display_x >= 240) *display_x = 239;
////	if(*display_y >= 320) *display_y = 319;

//}






