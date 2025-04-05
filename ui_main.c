#include "ui_main.h"
#include "ili9341.h"
#include "buttons.h"
#include "ui_core.h"
#include "ui_settings.h"


static MainOption current_option = MAIN_OPTION_PLAY;


//BUTTON HANDLERS
void MainPage_ButtonHandler(ButtonEventType event)
{
	switch(event)
	{
		case BUTTON_EVENT_UP:
			current_option = (current_option +1) %3;
		  UI_UpdateDisplay();
				break;
			
		case BUTTON_EVENT_DOWN:
			current_option = (current_option -1  +3) %3;
			UI_UpdateDisplay();
				break;
			
		case BUTTON_EVENT_SELECT:
			if(current_option == MAIN_OPTION_SETTINGS)
			{
						UI_NavigateTo(PAGE_SETTING);
			}
			else if(current_option == MAIN_OPTION_PLAY)
			{
				 led_blink_enabled = 1;
			}
				break;		

	
	}
	
//	UI_UpdateDisplay();
}



    
    // Request UI update (non-blocking)

//static void MainPage_ButtonHandler(ButtonEventType event)
//{
//	switch(event)
//	{
//		case BUTTON_EVENT_UP:
//			current_option = (current_option +1) %3;
//		  UI_UpdateDisplay();
//				break;
//			
//		case BUTTON_EVENT_DOWN:
//			current_option = (current_option -1  +3) %3;
//			UI_UpdateDisplay();
//				break;
//			
//		case BUTTON_EVENT_SELECT:
//			if(current_option == MAIN_OPTION_SETTINGS)
//			{
//						UI_NavigateTo(PAGE_SETTING);
//			}
//				break;		
//	}
//	
////	UI_UpdateDisplay();
//}







void MainPage_Init(void)
{
	current_option = MAIN_OPTION_PLAY;
	Buttons_SetHandler(MainPage_ButtonHandler);
	
}

void MainPage_Draw(void)
{
	ILI9341_FillScreen(current_bg_Color);
	ILI9341_DrawString(100,20,"MAIN MENU", COLOR_WHITE, current_bg_Color, 2);
	ILI9341_DrawString(10,10,"Ver.01", COLOR_RED, current_bg_Color, 1);
	//draw options with selection highlight
	
	
	const char* options[] = {"Play", "Pause", "Settings"};
	for(uint8_t i=0; i < 3 ; i++){
		uint16_t y_pos=70 + i*40;
		
		//highlight selected option

		ILI9341_DrawString(20, y_pos, options[i], COLOR_WHITE, current_bg_Color, 2);

		if(i == current_option)
		{

		ILI9341_DrawString(20, y_pos, options[i], COLOR_BLACK, COLOR_YELLOW, 2);
		} 
	}
	
	
	//Draw Instructions
	
	ILI9341_DrawString(30,200, "Use Up/Down to select", COLOR_GREEN, current_bg_Color, 1);
	ILI9341_DrawString(30,220, "Press Select To Confirm", COLOR_GREEN, current_bg_Color, 1);
}
