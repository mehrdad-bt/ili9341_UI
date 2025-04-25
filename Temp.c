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

typedef enum {
	
	
	SETTING_BACK,
	SETTING_COUNT,
}TempOption;


//Temp and Hum 
uint8_t Tx_Soft[1]={AHT10_RESET};
uint8_t Tx_INIT[3]={AHT10_INIT,0x08,0x00};
uint8_t Tx_Trig[3]={AHT10_TRIG,0x33,0x00};	
uint8_t Rx_Data[6];
uint32_t Temperature, Humidity;
float tem,hum;

static TempOption current_option = SETTING_BACK;



I2C_HandleTypeDef hi2c1;


//BUTTON HANDLERS
void TempPage_ButtonHandler(ButtonEventType event)
{
	switch(event)
	{
		case BUTTON_EVENT_UP:
//		  pwm_duty = (pwm_duty < DUTY_STEP) ? 0 : pwm_duty - DUTY_STEP;
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);
//			if(FanSpeed>0) FanSpeed--;
//		  UI_UpdateDisplay();
				break;
//			
		case BUTTON_EVENT_DOWN:
//			pwm_duty = (pwm_duty + DUTY_STEP) > 200 ? 200 : pwm_duty + DUTY_STEP;
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);
//			if(FanSpeed < 7) FanSpeed++;
//			UI_UpdateDisplay();
				break;
			
		case BUTTON_EVENT_SELECT:
			 if(current_option == SETTING_BACK)
			{
				 UI_NavigateTo(PAGE_MAIN);
		     
			}
			break;
//			else if(current_option == MAIN_OPTION_PLAY)
//			{
//				 led_blink_enabled = 1;
//			}
						

	
	}
	

}




void TempPage_Init(void)
{
//	current_option = MAIN_OPTION_PLAY;
	  Buttons_SetHandler(TempPage_ButtonHandler);
	  first_run = 1;
	
}





void TempPage_Draw(void) {
    static uint32_t last_update = 0;
//    static uint8_t first_run = 1;
    // Clear screen only on first run
    if(first_run==1) {
			  ILI9341_DrawImage(0, 0, 320, 240, (uint16_t*)TempWallpaper);
//        ILI9341_FillScreen(TempWallpaper);
        ILI9341_DrawString(20, 20, "Temperature and Humidity", COLOR_BLACK, COLOR_WHITE, 2);
        first_run = 0;
//			    ILI9341_DrawString(50, 220, "Back To Main", 
//                      (current_option == SETTING_BACK) ? COLOR_BLACK : COLOR_WHITE,
//                      (current_option == SETTING_BACK) ? COLOR_YELLOW : current_bg_Color, 
//                      2);
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

//void TempPage_Draw(void)
//{
//    // Clear screen and draw titles
//    ILI9341_FillScreen(current_bg_Color);
//		ILI9341_DrawString(40, 20, "Temprature and Humidity", COLOR_WHITE, current_bg_Color, 2);
//		char temp[7],humid[7];
//		ILI9341_DrawString(50, 220, "Back To Main" , COLOR_WHITE, current_bg_Color, 2);
//		if(current_option == SETTING_BACK)ILI9341_DrawString(50, 220, "Back To Main", COLOR_BLACK, COLOR_YELLOW, 2);
//		while(1)
//		{
//		HAL_I2C_Master_Transmit(&hi2c1, AHT10_ADD<<1, Tx_Trig, 3, 200);
//		HAL_Delay(100);
//		HAL_I2C_Master_Receive(&hi2c1, (AHT10_ADD<<1 | 0x01), Rx_Data, 6, 300);
//		Temperature=((Rx_Data[3]&0x0f)<<16)+(Rx_Data[4]<<8)+(Rx_Data[5]);
//		tem=((float)Temperature/1048576)*200-50;
//		Humidity=(Rx_Data[1]<<12)+(Rx_Data[2]<<4)+(Rx_Data[3]<<4);
//		hum=(float)Humidity/1048576*100;	
//    sprintf(temp, "%02.2f C", tem);
//		sprintf(humid, "%02.2f %%", hum);	
//    ILI9341_DrawString(80, 70, temp, COLOR_YELLOW, current_bg_Color, 3);
//		ILI9341_DrawString(80, 150, humid, COLOR_CYAN, current_bg_Color, 3);	
//		HAL_Delay(2000);
////		if(BUTTON_EVENT_SELECT==1) return;	
//		}
//			
//	
// 
//		
//		
//		
//		
//		
//		
//    
//    // Draw instructions
////    ILI9341_DrawString(30, 200, "Use Up/Down to select", COLOR_GREEN, current_bg_Color, 1);
////    ILI9341_DrawString(30, 220, "Press Select To Go Back", COLOR_GREEN, current_bg_Color, 1);
//}


