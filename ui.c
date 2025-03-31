#include "ui.h"
#include "ili9341.h"
#include "buttons.h"
#include <math.h>
#include <string.h>

/* Private variables */
static uint32_t blinkTimer = 0;
static uint8_t blinkState = 0;
static uint8_t back_btn_should_blink = 0;
static uint8_t needs_full_redraw = 1;
static uint8_t colorSelection = 0;
static uint8_t animationFrame = 0;

/* Button coordinates */
static const uint16_t textX1 = 70, textY1 = 50;
static const uint16_t textX2 = 70, textY2 = 70;
static const uint16_t textX3 = 70, textY3 = 90;
static const uint16_t textWidth = 180;

/* Global variables */

Page_t currentPage = PAGE_MAIN;
SelectedOption_t currentOption = OPTION_PLAY;
PlayerState playerState = PLAYER_STOPPED;
uint16_t bgColor = COLOR_BLACK;

	ImageButton_t back_text_button = {
    .x = 10, .y = 200, .width = 80, .height = 30,
    .normal_img = NULL, .pressed_img = NULL, .is_pressed = 0
	};

/* Private function prototypes */
static void DrawMainPage(void);
static void DrawSecondPage(void);
static void DrawColorSelectPage(void);
static void DrawBackButton(void);
static void DrawPlayIcon(uint16_t x, uint16_t y, uint16_t size, uint16_t color);
static void DrawPauseIcon(uint16_t x, uint16_t y, uint16_t size, uint16_t color);
static void HandlePageNavigation(void);
static uint8_t IsBottonPressed(uint16_t bottonPin);
/* Public functions */

void UI_Init(void) {
    bgColor = COLOR_BLACK;
    currentPage = PAGE_MAIN;
    currentOption = OPTION_PLAY;
    playerState = PLAYER_STOPPED;
}

void UI_UpdateDisplay(void) {
    static uint32_t last_blink_time = 0;
    
    /* Handle blinking timer (500ms interval) */
    if (HAL_GetTick() - last_blink_time > 500) {
        last_blink_time = HAL_GetTick();
        blinkState = !blinkState;
        
        if (currentPage == PAGE_MAIN) {
            /* Update only the selected option */
            uint16_t y = textY1 + currentOption*20;
            const char* text = (currentOption == 0) ? "Play" : 
                             (currentOption == 1) ? "Pause" : "Setting";
            
            ILI9341_FillRectangle(textX1, y-2, textWidth, 24, bgColor);
            ILI9341_DrawString(textX1, y, text, 
                             blinkState ? COLOR_BLACK : COLOR_WHITE,
                             blinkState ? COLOR_WHITE : bgColor, 2);
        }
        else if (currentPage == PAGE_SECOND) {
            /* Blink selected option only */
            if (currentOption == OPTION_BG_COLOR) {
                ILI9341_FillRectangle(70, 100, 150, 20, bgColor);
                ILI9341_DrawString(70, 100, "Background Color", 
                                 blinkState ? COLOR_BLACK : COLOR_WHITE,
                                 blinkState ? COLOR_WHITE : bgColor, 2);
            }
            
            /* Blink back button only if selected */
            if (back_btn_should_blink) {
                uint16_t btn_bg = blinkState ? COLOR_WHITE : COLOR_RED;
                uint16_t btn_text = blinkState ? COLOR_BLACK : COLOR_WHITE;
                ILI9341_FillRectangle(10, 200, 80, 30, btn_bg);
                ILI9341_DrawString(30, 205, "BACK", btn_text, btn_bg, 2);
            }
        }
    }

    /* Full redraw when needed */
    if (needs_full_redraw) {
        ILI9341_FillScreen(bgColor);
        
        switch(currentPage) {
            case PAGE_MAIN: DrawMainPage(); break;
            case PAGE_SECOND: DrawSecondPage(); break;
            case PAGE_COLOR_SELECT: DrawColorSelectPage(); break;
        }
        
        needs_full_redraw = 0;
    }
}

void UI_HandleInput(void) {
    HandlePageNavigation();
}

void UI_HandlePlayerButton(void) {
    static uint32_t lastPressTime = 0;
    static uint8_t wasPressed = 0;
    
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET) {
        if(!wasPressed && (HAL_GetTick() - lastPressTime > 200)) {
            wasPressed = 1;
            lastPressTime = HAL_GetTick();
            
            /* STATE TRANSITION WITH EFFECT */
            animationFrame = 0; /* reset animation */
            
            switch(playerState) {
                case PLAYER_STOPPED:
                case PLAYER_PAUSED:
                    playerState = PLAYER_PLAYING;
                    break;
                case PLAYER_PLAYING:
                    playerState = PLAYER_PAUSED;
                    break;
            }
        }
    } else {
        wasPressed = 0;
    }
    
    if(animationFrame < 255) {
        animationFrame += 5;
    }
}

void UI_DrawPlayerControls(void) {
    uint16_t x = 150, y = 200, size = 30;
    
    /* Clear previous control area */
    ILI9341_FillRectangle(x, y, size+50, size+50, bgColor);
    
    switch(playerState) {
        case PLAYER_PLAYING:
            DrawPauseIcon(x, y, size, COLOR_RED);
            break;
        case PLAYER_PAUSED:
        case PLAYER_STOPPED:
            DrawPlayIcon(x, y, size, COLOR_GREEN);
            break;
    }
}


void DrawPage() {
    ILI9341_FillScreen(bgColor);  // Use the selected background color
    
    if(currentPage == PAGE_MAIN) {
        ILI9341_DrawString(100,10,"MAIN PAGE", COLOR_WHITE, bgColor, 2);
    } 
    else if(currentPage == PAGE_SECOND) {
        ILI9341_DrawString(100,10,"SECOND PAGE", COLOR_WHITE, bgColor, 2);
        
        // Add Background Color option
        uint16_t bg_opt_y = 100;
        if(currentOption == OPTION_BG_COLOR) {
            ILI9341_DrawString(70, bg_opt_y, "Background Color", 
                              blinkState ? COLOR_BLACK : COLOR_WHITE,
                              blinkState ? COLOR_WHITE : COLOR_BLACK, 2);
        } else {
            ILI9341_DrawString(70, bg_opt_y, "Background Color", COLOR_WHITE, bgColor, 2);
        }
    }
    else if(currentPage == PAGE_COLOR_SELECT) {
        ILI9341_DrawString(80,10,"SELECT COLOR", COLOR_WHITE, bgColor, 2);
        
        // Draw color options
        ILI9341_FillRectangle(50, 50, 60, 60, COLOR_OPTION_1);
        ILI9341_FillRectangle(130, 50, 60, 60, COLOR_OPTION_2);
        ILI9341_FillRectangle(50, 130, 60, 60, COLOR_OPTION_3);
        ILI9341_FillRectangle(130, 130, 60, 60, COLOR_OPTION_4);
        
        // Draw selection indicator
        uint16_t x = 50 + (colorSelection % 2) * 80;
        uint16_t y = 50 + (colorSelection / 2) * 80;
        ILI9341_DrawRectangle(x-2, y-2, 64, 64, COLOR_WHITE);
    }
    
    // Draw common elements
    ILI9341_DrawString(10,10,"Ver.00", COLOR_RED, bgColor, 1);
    
    if(currentPage == PAGE_SECOND) {
        DrawBackButton();
    }
}


/* Private functions */


void DrawBackButton(void)
{
	  uint8_t back_blink_state = 0;
    if(back_text_button.is_pressed) {
        // Pressed state (solid white)
        ILI9341_FillRectangle(back_text_button.x, back_text_button.y, 
                            back_text_button.width, back_text_button.height, COLOR_WHITE);
        ILI9341_DrawString(back_text_button.x+20, back_text_button.y+5, 
                         "BACK", COLOR_BLACK, COLOR_WHITE, 2);
    } 
    else {
        // Normal state with blinking
        uint16_t bg_color = back_blink_state ? COLOR_WHITE : COLOR_RED;
        uint16_t text_color = back_blink_state ? COLOR_BLACK : COLOR_WHITE;
        
        ILI9341_FillRectangle(back_text_button.x, back_text_button.y, 
                            back_text_button.width, back_text_button.height, bg_color);
        ILI9341_DrawString(back_text_button.x+20, back_text_button.y+5, 
                         "BACK", text_color, bg_color, 2);
    }
}



	uint8_t IsBottonPressed(uint16_t bottonPin)
	{
		static uint32_t lastPressTime = 0;
		if(HAL_GPIO_ReadPin(BTN_PORT, bottonPin) == GPIO_PIN_RESET)
		{
			if(HAL_GetTick() - lastPressTime > 200)
			{
				//200 ms debounce
				lastPressTime = HAL_GetTick();
				return 1;
			}
		}
		
		return 0;
		
	}

static void HandlePageNavigation(void) {
    if (IsBottonPressed(NEXT_BTN_PIN)) {
        if (currentPage == PAGE_MAIN) {
            currentOption = (currentOption + 1) % 3;
        }
        else if (currentPage == PAGE_SECOND) {
            currentOption = (currentOption == OPTION_BG_COLOR) ? OPTION_SETTING : OPTION_BG_COLOR;
            back_btn_should_blink = (currentOption == OPTION_SETTING);
        }
        else if (currentPage == PAGE_COLOR_SELECT) {
            colorSelection = (colorSelection + 1) % 4;
        }
        needs_full_redraw = 1;
        HAL_Delay(200);
    }
    else if (IsBottonPressed(BACK_BTN_PIN)) {
        if (currentPage == PAGE_MAIN) {
            currentOption = (currentOption - 1 + 3) % 3;
        }
        else if (currentPage == PAGE_SECOND) {
            currentOption = (currentOption == OPTION_SETTING) ? OPTION_BG_COLOR : OPTION_SETTING;
            back_btn_should_blink = (currentOption == OPTION_SETTING);
        }
        else if (currentPage == PAGE_COLOR_SELECT) {
            colorSelection = (colorSelection - 1 + 4) % 4;
        }
        needs_full_redraw = 1;
        HAL_Delay(200);
    }
    else if (IsBottonPressed(OPTION_BTN_PIN)) {
        if (currentPage == PAGE_MAIN && currentOption == OPTION_SETTING) {
            currentPage = PAGE_SECOND;
            currentOption = OPTION_SETTING;
            back_btn_should_blink = 0;
        }
        else if (currentPage == PAGE_SECOND) {
            if (currentOption == OPTION_BG_COLOR) {
                currentPage = PAGE_COLOR_SELECT;
                colorSelection = 0;
            }
            else if (currentOption == OPTION_SETTING) {
                currentPage = PAGE_MAIN;
            }
        }
        else if (currentPage == PAGE_COLOR_SELECT) {
            switch(colorSelection) {
                case 0: bgColor = COLOR_BLUE; break;
                case 1: bgColor = COLOR_GREEN; break;
                case 2: bgColor = COLOR_RED; break;
                case 3: bgColor = COLOR_WHITE; break;
            }
            currentPage = PAGE_SECOND;
        }
        needs_full_redraw = 1;
        HAL_Delay(200);
    }
}

static void DrawMainPage(void) {
    ILI9341_DrawString(100,10,"MAIN PAGE", COLOR_WHITE, bgColor, 2);
    
    const char* options[3] = {"Play", "Pause", "Setting"};
    for (int i = 0; i < 3; i++) {
        uint16_t y = textY1 + i*20;
        if (i == currentOption) {
            ILI9341_DrawString(textX1, y, options[i], 
                             blinkState ? COLOR_BLACK : COLOR_WHITE,
                             blinkState ? COLOR_WHITE : bgColor, 2);
        } else {
            ILI9341_DrawString(textX1, y, options[i], COLOR_WHITE, bgColor, 2);
        }
    }
}

static void DrawSecondPage(void) {
    ILI9341_DrawString(100,10,"SECOND PAGE", COLOR_WHITE, bgColor, 2);
    
    /* Background Color option */
    ILI9341_DrawString(70, 100, "Background Color", 
                     (currentOption == OPTION_BG_COLOR) ? 
                     (blinkState ? COLOR_BLACK : COLOR_WHITE) : COLOR_WHITE,
                     (currentOption == OPTION_BG_COLOR) ? 
                     (blinkState ? COLOR_WHITE : bgColor) : bgColor, 2);
    
    /* Back button */
    uint16_t btn_bg = back_btn_should_blink ? 
                    (blinkState ? COLOR_WHITE : COLOR_RED) : COLOR_RED;
    uint16_t btn_text = back_btn_should_blink ? 
                      (blinkState ? COLOR_BLACK : COLOR_WHITE) : COLOR_WHITE;
    ILI9341_FillRectangle(10, 200, 80, 30, btn_bg);
    ILI9341_DrawString(30, 205, "BACK", btn_text, btn_bg, 2);
}

static void DrawColorSelectPage(void) {
    ILI9341_DrawString(80,10,"SELECT COLOR", COLOR_WHITE, bgColor, 2);
    
    /* Draw color options */
    ILI9341_FillRectangle(50, 50, 60, 60, COLOR_BLUE);
    ILI9341_FillRectangle(130, 50, 60, 60, COLOR_GREEN);
    ILI9341_FillRectangle(50, 130, 60, 60, COLOR_RED);
    ILI9341_FillRectangle(130, 130, 60, 60, COLOR_WHITE);
    
    /* Draw selection indicator */
    uint16_t x = 50 + (colorSelection % 2) * 80;
    uint16_t y = 50 + (colorSelection / 2) * 80;
    ILI9341_DrawRectangle(x-2, y-2, 64, 64, COLOR_WHITE);
}

static void DrawPlayIcon(uint16_t x, uint16_t y, uint16_t size, uint16_t color) {
    for(uint8_t i = 0; i < size/2; i++) {
        ILI9341_DrawLine(x, y + i, x, y + size - i, color);
        ILI9341_DrawLine(x, y + i, x + size, y + size/2, color);
        ILI9341_DrawLine(x, y + size - i, x + size, y + size/2, color);
    }
}

static void DrawPauseIcon(uint16_t x, uint16_t y, uint16_t size, uint16_t color) {
    uint8_t barWidth = size/3;
    uint8_t spacing = size/6;
    
    /* Left bar */
    ILI9341_FillRectangle(x, y, barWidth, size, color);
    /* Right bar */
    ILI9341_FillRectangle(x + barWidth + spacing, y, barWidth, size, color);
}

