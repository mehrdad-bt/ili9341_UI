#include "ui_settings.h"
#include "ui_core.h"
#include "ili9341.h"
#include "buttons.h"
#include "ui_background.h"

typedef enum {
	
	SETTING_COLOR,
	SETTING_BACK,
	SETTING_COUNT,
}settingsOption;

static settingsOption current_option = SETTING_COLOR;






static void SettingsPage_ButtonHandler(ButtonEventType event) {
	switch(event) {
		case BUTTON_EVENT_UP:
			current_option = (current_option +1 ) %SETTING_COUNT;
			break;
		
		  case BUTTON_EVENT_DOWN:
			current_option = (current_option - 1  + SETTING_COUNT) %SETTING_COUNT;
			break;
		
			case BUTTON_EVENT_SELECT:
				if(current_option == SETTING_COLOR) {
					UI_NavigateTo(PAGE_COLOR_SELECT);
					return;
				}else {
					UI_NavigateTo(PAGE_MAIN);
					return;
					}
				}
					
			UI_UpdateDisplay();
				
}

void SettingsPage_Init(void) {  // Must match declaration
    current_option = SETTING_COLOR;
    Buttons_SetHandler(SettingsPage_ButtonHandler);
}



/* Draw settings page */
void SettingsPage_Draw(void) {
    // Clear screen with current background
    ILI9341_FillScreen(current_bg_Color);
    
    // Draw title
    ILI9341_DrawString(100, 10, "SETTINGS", COLOR_WHITE, current_bg_Color, 2);
    
    // Draw all options
    const char* options[SETTING_COUNT] = {"Change Color", "Back to Main"};
    
    for(uint8_t i = 0; i < SETTING_COUNT; i++) {
        uint16_t y_pos = 50 + i * 40;
        ILI9341_DrawString(20, y_pos, options[i], COLOR_WHITE, current_bg_Color, 2);
        // Highlight selected option
        if(i == current_option) {
 //           ILI9341_FillRectangle(20, y_pos - 5, 240, 30, COLOR_YELLOW);
            ILI9341_DrawString(20, y_pos, options[i], COLOR_BLACK, COLOR_YELLOW, 2);
        }
    }
    
    // Current color preview
//    ILI9341_FillRectangle(180, 150, 80, 40, current_bg_Color);
//    ILI9341_DrawRectangle(179, 149, 82, 42, COLOR_WHITE); // Border
//    ILI9341_DrawString(150, 200, "Current Color", COLOR_BLUE, current_bg_Color, 1);
    
    // Instructions
//    ILI9341_DrawString(30, 220, "Press OPTION to select", COLOR_BLUE, current_bg_Color, 1);
}

