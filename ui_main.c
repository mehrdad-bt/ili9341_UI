#include "ui_main.h"
#include "ili9341.h"
#include "buttons.h"
#include "ui_core.h"
#include "ui_settings.h"
#include "main.h"
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"  
#define DEBOUNCE_DELAY_MS 50  



static MainOption current_option = MAIN_OPTION_OCS;
static MainOption last_highlighted = 10;  // Tracks last highlighted item
static bool needs_full_redraw = true;
static uint32_t last_button_time = 0;
static ButtonEventType last_button = BUTTON_EVENT_NONE;


//BUTTON HANDLERS
void MainPage_ButtonHandler(ButtonEventType event) {
    uint32_t now = HAL_GetTick();
    
    // Debounce logic
    if(event == BUTTON_EVENT_NONE || 
       (now - last_button_time < DEBOUNCE_DELAY_MS && event == last_button)) {
        return;
    }
    
    last_button_time = now;
    last_button = event;

    // Original menu logic
    switch(event) {
        case BUTTON_EVENT_UP:
            current_option = (current_option + 1) % 4;
            break;
            
        case BUTTON_EVENT_DOWN:
            current_option = (current_option - 1 + 4) % 4;
            break;
            
        case BUTTON_EVENT_SELECT:
            if(current_option == MAIN_OPTION_SETTINGS) {
                UI_NavigateTo(PAGE_SETTING);
            }
            else if(current_option == MAIN_OPTION_FAN) {
                UI_NavigateTo(PAGE_FAN);
            }
            else if(current_option == MAIN_OPTION_OCS) {
                UI_NavigateTo(PAGE_OCS);
            }
            else if(current_option == MAIN_OPTION_SENSOR) {
                UI_NavigateTo(PAGE_TEMP);
            }
            break;
            
        default:
            break;
    }
    
    UI_UpdateDisplay();
}



    


void MainPage_Init(void)
{
    current_option = MAIN_OPTION_OCS;
    last_highlighted = 10;
    needs_full_redraw = true;
    Buttons_SetHandler(MainPage_ButtonHandler);
}


void MainPage_ForceRedraw(void) {
    needs_full_redraw = true;
}

void MainPage_Draw(void) {
    const char* options[] = {"oscilloscope", "fan", "sensor", "Settings"};
    
    // Full redraw when needed
    if (needs_full_redraw) {
        ILI9341_FillScreen(current_bg_Color);
        
        // Static elements
        ILI9341_DrawString(100, 20, "MAIN MENU", COLOR_WHITE, current_bg_Color, 2);
        ILI9341_DrawString(10, 10, "Ver.04", COLOR_RED, current_bg_Color, 1);
        ILI9341_DrawString(30, 200, "Use Up/Down...", COLOR_GREEN, current_bg_Color, 1);
        ILI9341_DrawString(30, 220, "Press Select...", COLOR_GREEN, current_bg_Color, 1);

        // Draw all options unhighlighted
        for (uint8_t i = 0; i < 4; i++) {
            uint16_t y_pos = 50 + i * 40;
            ILI9341_DrawString(20, y_pos, options[i], COLOR_WHITE, current_bg_Color, 2);
        }
        
        needs_full_redraw = false;
    }

    // Handle option highlighting
    if (last_highlighted != current_option) {
        // Clear previous highlight if exists
        if (last_highlighted != 10) {
            uint16_t y_prev = 50 + last_highlighted * 40;
            ILI9341_DrawString(20, y_prev, options[last_highlighted], 
                             COLOR_WHITE, current_bg_Color, 2);
        }
        
        // Apply new highlight
        uint16_t y_new = 50 + current_option * 40;
        ILI9341_DrawString(20, y_new, options[current_option],
                         COLOR_BLACK, COLOR_YELLOW, 2);
        
        last_highlighted = current_option;
    }
}
//void MainPage_Draw(void)
//{
//	ILI9341_FillScreen(current_bg_Color);
//	ILI9341_DrawString(100,20,"MAIN MENU", COLOR_WHITE, current_bg_Color, 2);
//	ILI9341_DrawString(10,10,"Ver.03", COLOR_RED, current_bg_Color, 1);
//	//draw options with selection highlight
//	
//	
//	const char* options[] = {"oscilloscope", "fan","sensor", "Settings"};
//	for(uint8_t i=0; i < 4 ; i++){
//		uint16_t y_pos=50 + i*40;
//		
//		//highlight selected option

//		ILI9341_DrawString(20, y_pos, options[i], COLOR_WHITE, current_bg_Color, 2);

//		if(i == current_option)
//		{

//		ILI9341_DrawString(20, y_pos, options[i], COLOR_BLACK, COLOR_YELLOW, 2);
//		} 
//	}
	
	
	//Draw Instructions
	
//	ILI9341_DrawString(30,200, "Use Up/Down to select", COLOR_GREEN, current_bg_Color, 1);
//	ILI9341_DrawString(30,220, "Press Select To Confirm", COLOR_GREEN, current_bg_Color, 1);
//}



