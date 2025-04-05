#include "ui_core.h"
#include "ui_main.h"
#include "ui_settings.h"
#include "ui_background.h"  // Changed from ui_background.h
#include "ili9341.h"
#include "stm32f4xx_hal.h"
#include "buttons.h"

uint16_t current_bg_Color = COLOR_BLACK;  // Fixed variable name (consistent capitalization)
PageID current_page = PAGE_MAIN;



void UI_NavigateTo(PageID page) {
    if (current_page == page) return;  // Skip if already on the page
    
    current_page = page;
    Buttons_SetHandler(NULL);  // Clear old handlers
    
    // Initialize only if not done before (optional)
    switch(page) {
        case PAGE_MAIN: 
             MainPage_Init();  // Skip if already initialized in UI_Init()
            break;
        case PAGE_SETTING:
            SettingsPage_Init(); 
            break;
        case PAGE_COLOR_SELECT: 
            ColorsPage_Init(); 
            break;
    }
    
    UI_UpdateDisplay();  // Will only redraw dynamic parts
}

void UI_Init(void) 
{
	
	  current_page = PAGE_MAIN;  // Set default page
    current_bg_Color = COLOR_BLACK;  // Default background
    
    // Initialize buttons (if needed)
    // Buttons_Init();  
    
    // Draw the initial page (no mutex needed here if running in `main()`)
    MainPage_Init();  // Sets up callbacks

    MainPage_Draw();  // First render
		
	
////    Buttons_Init();
//	osMutexWait(RecursiveMutexHandle, osWaitForever);
//  UI_NavigateTo(PAGE_MAIN);
//	osMutexRelease(RecursiveMutexHandle);
}


// Changed from static to global scope
void UI_UpdateDisplay(void) 
{
    /* Clear screen with current background color */
    osMutexWait(RecursiveMutexHandle, osWaitForever);
	  static uint16_t last_bg_color = 0;
    if (current_bg_Color != last_bg_color) {
        ILI9341_FillScreen(current_bg_Color);
        last_bg_color = current_bg_Color;
    }
    
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
		osMutexRelease(RecursiveMutexHandle);
}



