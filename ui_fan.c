#include "ui_fan.h"
#include "ili9341.h"
#include "buttons.h"
#include "ui_core.h"
#include "ui_settings.h"
#include <stdio.h>
#include "main.h"


#define DUTY_STEP 20

extern TIM_HandleTypeDef htim1;
static uint8_t FanSpeed=0;
volatile uint16_t pwm_duty=0;


//BUTTON HANDLERS
void FanPage_ButtonHandler(ButtonEventType event)
{
	switch(event)
	{
		case BUTTON_EVENT_UP:
		  pwm_duty = (pwm_duty < DUTY_STEP) ? 0 : pwm_duty - DUTY_STEP;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);
			if(FanSpeed>0) FanSpeed--;
		  UI_UpdateDisplay();
				break;
			
		case BUTTON_EVENT_DOWN:
			pwm_duty = (pwm_duty + DUTY_STEP) > 200 ? 200 : pwm_duty + DUTY_STEP;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);
			if(FanSpeed < 7) FanSpeed++;
			UI_UpdateDisplay();
				break;
			
		case BUTTON_EVENT_SELECT:
//			if(current_option == MAIN_OPTION_SETTINGS)
//			{
				 UI_NavigateTo(PAGE_MAIN);
		     break;
//			}
//			else if(current_option == MAIN_OPTION_PLAY)
//			{
//				 led_blink_enabled = 1;
//			}
						

	
	}
	

}




void FanPage_Init(void)
{
//	current_option = MAIN_OPTION_PLAY;
	Buttons_SetHandler(FanPage_ButtonHandler);
	
}

void FanPage_Draw(void)
{
    // Clear screen and draw titles
    ILI9341_FillScreen(current_bg_Color);
    ILI9341_DrawString(70, 20, "FAN SPEED CONTROL", COLOR_WHITE, current_bg_Color, 2);
//    ILI9341_DrawString(10, 10, "Ver.01", COLOR_RED, current_bg_Color, 1);

    // Ensure FanSpeed is within 0-7 range
//    if(FanSpeed > 7) FanSpeed = 7;
//    if(FanSpeed < 0) FanSpeed = 0;
    // Draw speed number (centered above the bar)
    char speedStr[4];
    sprintf(speedStr, "%d", FanSpeed);
    ILI9341_DrawString(140, 70, speedStr, COLOR_WHITE, current_bg_Color, 2);
    
    // Rectangle bar parameters
    const uint16_t barX = 50;
    const uint16_t barY = 100;
    const uint16_t barWidth = 220;
    const uint16_t barHeight = 25;
    const float segmentWidth = barWidth / 7.1;
    
    // Draw empty bar outline
    ILI9341_DrawRectangle(barX, barY, barWidth, barHeight, COLOR_WHITE);
    
    // Fill the bar proportional to FanSpeed
    if(FanSpeed > 0) {
        uint16_t fillWidth = FanSpeed * segmentWidth;
			 if(FanSpeed <= 2)
			 {
        ILI9341_FillRectangle(barX+2, barY+2, fillWidth-2, barHeight-4, COLOR_GREEN);
			 }
			 else if(FanSpeed > 2 & FanSpeed<=4)
			 {
				 ILI9341_FillRectangle(barX+2, barY+2, fillWidth-2, barHeight-4, COLOR_AMBER);
			 }
			 else if(FanSpeed > 4 )
			 {
				  ILI9341_FillRectangle(barX+2, barY+2, fillWidth-2, barHeight-4, COLOR_RED);
			 }
    }
		
		
		
		
		
		
    
    // Draw instructions
    ILI9341_DrawString(30, 200, "Use Up/Down to select", COLOR_GREEN, current_bg_Color, 1);
    ILI9341_DrawString(30, 220, "Press Select To Go Back", COLOR_GREEN, current_bg_Color, 1);
}

//void FanPage_Draw(void)
//{
//	ILI9341_FillScreen(current_bg_Color);
//	ILI9341_DrawString(100,20,"FAN SPEED CONTROLL", COLOR_WHITE, current_bg_Color, 2);
//	ILI9341_DrawString(10,10,"Ver.01", COLOR_RED, current_bg_Color, 1);
//	//draw options with selection highlight
//	
//	


//		
//		//highlight selected option

//	char speedStr[4]; // Buffer for string conversion
//	if(FanSpeed > 7) FanSpeed=7;
//	sprintf(speedStr, "%d", FanSpeed); // Convert integer to string
//	ILI9341_DrawString(100, 100, speedStr, COLOR_WHITE, current_bg_Color, 2);
//	


// 

//	
//	
//	//Draw Instructions
//	
//	ILI9341_DrawString(30,200, "Use Up/Down to select", COLOR_GREEN, current_bg_Color, 1);
//	ILI9341_DrawString(30,220, "Press Select To Confirm", COLOR_GREEN, current_bg_Color, 1);
//}
