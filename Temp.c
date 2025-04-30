#include "Temp.h"
#include "ili9341.h"
#include "buttons.h"
#include "ui_core.h"
#include "ui_settings.h"
#include <stdio.h>
#include "main.h"
#include <stdbool.h>
#include "stm32f4xx_hal.h"  
#include "wallpapers.h"


static uint8_t first_run = 1;




//Temp and Hum 
uint8_t Tx_Soft[1]={AHT10_RESET};
uint8_t Tx_INIT[3]={AHT10_INIT,0x08,0x00};
uint8_t Tx_Trig[3]={AHT10_TRIG,0x33,0x00};	
uint8_t Rx_Data[6];
uint32_t Temperature, Humidity;
float tem,hum;
I2C_HandleTypeDef hi2c1;

//BUTTON HANDLERS
void TempPage_ButtonHandler(ButtonEventType event)
{
	switch(event)
	{
		
			
		case BUTTON_EVENT_BACK:
				 UI_NavigateTo(PAGE_MAIN);
			break;
	}
	

}




void TempPage_Init(void)
{

	  Buttons_SetHandler(TempPage_ButtonHandler);
	  first_run = 1;
	
}





void TempPage_Draw(void) {
    static uint32_t last_update = 0;
    // Clear screen only on first run
    if(first_run==1) {
			  ILI9341_DrawImage(0, 0, 320, 240, (uint16_t*)TempWallpaper);

        ILI9341_DrawString(20, 20, "Temperature and Humidity", COLOR_BLACK, COLOR_WHITE, 2);
        first_run = 0;
    }




    // Update sensor data every 2 seconds
    if(HAL_GetTick() - last_update >= 2000) {
        // Read sensor
        HAL_I2C_Master_Transmit(&hi2c1, AHT10_ADD<<1, Tx_Trig, 3, 200);
        osDelay(100); // Use RTOS delay instead of HAL_Delay
        
        HAL_I2C_Master_Receive(&hi2c1, (AHT10_ADD<<1 | 0x01), Rx_Data, 6, 300);
        
        // Process data
        Temperature = ((Rx_Data[3]&0x0f)<<16) + (Rx_Data[4]<<8) + Rx_Data[5];
        tem = ((float)Temperature/1048576)*200-50;
        
        Humidity = (Rx_Data[1]<<12) + (Rx_Data[2]<<4) + (Rx_Data[3]>>4);
        hum = (float)Humidity/1048576*100;
        
        // Display values
        char temp[10], humid[10];
        sprintf(temp, "%02.2f C", tem);
        sprintf(humid, "%02.2f %%", hum);
        
        ILI9341_DrawString(30, 70, temp, COLOR_RED,COLOR_WHITE, 3);
        ILI9341_DrawString(30, 150, humid, COLOR_BLUE, COLOR_WHITE, 3);
        
        last_update = HAL_GetTick();
    }
    
    // Small delay to prevent CPU overload
    osDelay(50);
}



