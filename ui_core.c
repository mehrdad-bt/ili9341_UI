#include "ui_core.h"
#include "ui_main.h"
#include "ui_settings.h"
#include "ui_background.h"  // Changed from ui_background.h
#include "ili9341.h"
#include "stm32f4xx_hal.h"
#include "buttons.h"

uint16_t current_bg_Color = COLOR_BLACK;  // Fixed variable name (consistent capitalization)
static PageID current_page = PAGE_MAIN;



void UI_NavigateTo(PageID page)
{
    /* Remove the if-condition to force initialization */
    current_page = page;  // Always update the current page
    
    // Clear previous button callbacks
    Buttons_SetHandler(NULL);
    
    // Initialize new page (will always execute)
    switch(page)
    {
        case PAGE_MAIN: 
            MainPage_Init(); 
            break;
        case PAGE_SETTING:
            SettingsPage_Init(); 
            break;
        case PAGE_COLOR_SELECT: 
            ColorsPage_Init(); 
            break;
        default:
            break;
    }
    
    UI_UpdateDisplay();
}

void UI_Init(void) 
{
//    Buttons_Init();
    UI_NavigateTo(PAGE_MAIN);
}


// Changed from static to global scope
void UI_UpdateDisplay(void) 
{
    /* Clear screen with current background color */
    ILI9341_FillScreen(current_bg_Color);
    
    /* Draw the current active page */
    switch(current_page) {
        case PAGE_MAIN:
            MainPage_Draw();
            break;
            
        case PAGE_SETTING:  // Fixed enum name
            SettingsPage_Draw();
            break;
            
        case PAGE_COLOR_SELECT:
            ColorsPage_Draw();
            break;
            
        default:
            /* Error state - draw warning */
            ILI9341_DrawString(10, 10, "PAGE ERROR", COLOR_RED, current_bg_Color, 2);
            break;
    }
}
