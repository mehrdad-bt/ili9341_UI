#include "ui.h"
#include "ili9341.h"
#include "buttons.h"
#include <math.h>
#include <string.h>

/* Private variables */
static uint8_t blinkState = 0;
static uint8_t needs_full_redraw = 1;
static uint8_t needs_option_redraw = 0;
static uint8_t colorSelection = 0;
static uint8_t animationFrame = 0;

/* Button coordinates */
#define TEXT_X1 70
#define TEXT_Y1 50
#define TEXT_Y2 70
#define TEXT_Y3 90
#define OPTION_WIDTH 180
#define OPTION_HEIGHT 24
#define OPTION_SPACING 30

/* Global variables */
Page_t currentPage = PAGE_MAIN;
MainOption_t currentMainOption = OPTION_PLAY;
SettingsOption_t currentSettingsOption = COLOR_SELECT;
PlayerState playerState = PLAYER_STOPPED;
uint16_t bgColor = COLOR_BLACK;

/* Private function prototypes */
static void DrawMainPage(void);
static void DrawSecondPage(void);
static void DrawColorSelectPage(void);
static void DrawPlayIcon(uint16_t x, uint16_t y, uint16_t size, uint16_t color);
static void DrawPauseIcon(uint16_t x, uint16_t y, uint16_t size, uint16_t color);
static void HandlePageNavigation(void);
static uint8_t IsButtonPressed(uint16_t buttonPin);
static void DrawBlinkingOption(uint16_t x, uint16_t y, const char* text, uint8_t is_selected);

void UI_Init(void) {
    bgColor = COLOR_BLACK;
    currentPage = PAGE_MAIN;
    currentMainOption = OPTION_PLAY;
    currentSettingsOption = COLOR_SELECT;
    playerState = PLAYER_STOPPED;
    needs_full_redraw = 1;
}

void UI_UpdateDisplay(void) {
    static uint32_t last_blink_time = 0;
    
    /* Handle blinking timer (500ms interval) */
    if (HAL_GetTick() - last_blink_time > 500) {
        last_blink_time = HAL_GetTick();
        blinkState = !blinkState;
        needs_option_redraw = 1;
    }

    /* Full redraw when needed */
    if (needs_full_redraw) {
        ILI9341_FillScreen(bgColor);
        
        switch(currentPage) {
            case PAGE_MAIN: 
                DrawMainPage(); 
                break;
            case PAGE_SECOND: 
                DrawSecondPage(); 
                break;
            case PAGE_COLOR_SELECT:
                DrawColorSelectPage();
                break;
        }
        needs_full_redraw = 0;
        needs_option_redraw = 0;
    }
    /* Partial redraw for blinking options */
    else if (needs_option_redraw) {
        switch(currentPage) {
            case PAGE_MAIN:
                DrawBlinkingOption(TEXT_X1, TEXT_Y1 + currentMainOption*OPTION_SPACING, 
                                 (currentMainOption == OPTION_PLAY) ? "Play" : 
                                 (currentMainOption == OPTION_PAUSE) ? "Pause" : "Settings", 1);
                break;
            case PAGE_SECOND: {
                const char* options[] = {"Select Color", "Setting 1", "Setting 2", "Back"};
                uint16_t y = 50 + currentSettingsOption*OPTION_SPACING;
                DrawBlinkingOption(70, y, options[currentSettingsOption], 1);
                break;
            }
            case PAGE_COLOR_SELECT: {
                const char* colorNames[] = {"Blue", "Green", "Red", "White"};
                uint16_t y = 50 + colorSelection*OPTION_SPACING;
                DrawBlinkingOption(70, y, colorNames[colorSelection], 1);
                break;
            }
        }
        needs_option_redraw = 0;
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
            
            animationFrame = 0;
            
            switch(playerState) {
                case PLAYER_STOPPED:
                case PLAYER_PAUSED:
                    playerState = PLAYER_PLAYING;
                    break;
                case PLAYER_PLAYING:
                    playerState = PLAYER_PAUSED;
                    break;
            }
            needs_full_redraw = 1;
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

static void DrawMainPage(void) {
    ILI9341_DrawString(100,10,"MAIN PAGE", COLOR_WHITE, bgColor, 2);
    
    const char* options[3] = {"Play", "Pause", "Settings"};
    for (uint8_t i = 0; i < 3; i++) {
        DrawBlinkingOption(TEXT_X1, TEXT_Y1 + i*OPTION_SPACING, options[i], (i == (uint8_t)currentMainOption));
    }
}

static void DrawSecondPage(void) {
    ILI9341_DrawString(100,10,"SETTINGS", COLOR_WHITE, bgColor, 2);
    
    const char* options[] = {"Select Color", "Setting 1", "Setting 2", "Back"};
    uint16_t y = 50;
    
    for (uint8_t i = 0; i < 4; i++) {
        DrawBlinkingOption(70, y, options[i], (i == (uint8_t)currentSettingsOption));
        y += OPTION_SPACING;
    }
}

static void DrawColorSelectPage(void) {
    ILI9341_DrawString(80, 10, "SELECT COLOR", COLOR_WHITE, bgColor, 2);
    
    const char* colorNames[] = {"Blue", "Green", "Red", "White"};
    uint16_t y = 50;
    
    for (uint8_t i = 0; i < 4; i++) {
        DrawBlinkingOption(70, y, colorNames[i], (i == colorSelection));
        y += OPTION_SPACING;
    }
}

static void DrawBlinkingOption(uint16_t x, uint16_t y, const char* text, uint8_t is_selected) {
    if(is_selected) {
        uint16_t bg = blinkState ? COLOR_WHITE : bgColor;
        uint16_t text_color = blinkState ? COLOR_BLACK : COLOR_WHITE;
        ILI9341_FillRectangle(x-2, y-2, OPTION_WIDTH+4, OPTION_HEIGHT, bg);
        ILI9341_DrawString(x, y, text, text_color, bg, 2);
    } else {
        ILI9341_FillRectangle(x-2, y-2, OPTION_WIDTH+4, OPTION_HEIGHT, bgColor);
        ILI9341_DrawString(x, y, text, COLOR_WHITE, bgColor, 2);
    }
}

static uint8_t IsButtonPressed(uint16_t buttonPin) {
    static uint32_t lastPressTime = 0;
    if(HAL_GPIO_ReadPin(BTN_PORT, buttonPin) == GPIO_PIN_RESET) {
        if(HAL_GetTick() - lastPressTime > 200) {
            lastPressTime = HAL_GetTick();
            return 1;
        }
    }
    return 0;
}

static void HandlePageNavigation(void) {
    if (IsButtonPressed(NEXT_BTN_PIN)) {
        switch(currentPage) {
            case PAGE_MAIN:
                currentMainOption = (MainOption_t)(((uint8_t)currentMainOption + 1) % 3);
                break;
            case PAGE_SECOND:
                currentSettingsOption = (SettingsOption_t)(((uint8_t)currentSettingsOption + 1) % 4);
                break;
            case PAGE_COLOR_SELECT:
                colorSelection = (colorSelection + 1) % 4;
                break;
        }
        needs_full_redraw = 1;
        HAL_Delay(200);
    }
    else if (IsButtonPressed(BACK_BTN_PIN)) {
        switch(currentPage) {
            case PAGE_MAIN:
                currentMainOption = (MainOption_t)(((uint8_t)currentMainOption - 1 + 3) % 3);
                break;
            case PAGE_SECOND:
                currentSettingsOption = (SettingsOption_t)(((uint8_t)currentSettingsOption - 1 + 4) % 4);
                break;
            case PAGE_COLOR_SELECT:
                colorSelection = (colorSelection - 1 + 4) % 4;
                break;
        }
        needs_full_redraw = 1;
        HAL_Delay(200);
    }
    else if (IsButtonPressed(OPTION_BTN_PIN)) {
        switch(currentPage) {
            case PAGE_MAIN:
                if (currentMainOption == OPTION_SETTING) {
                    currentPage = PAGE_SECOND;
                    currentSettingsOption = COLOR_SELECT;
                }
                break;
            case PAGE_SECOND:
                if (currentSettingsOption == COLOR_SELECT) {
                    currentPage = PAGE_COLOR_SELECT;
                    colorSelection = 0;
                }
                else if (currentSettingsOption == BACK) {
                    currentPage = PAGE_MAIN;
                    currentMainOption = OPTION_PLAY;
                }
                break;
            case PAGE_COLOR_SELECT:
                switch(colorSelection) {
                    case 0: bgColor = COLOR_BLUE; break;
                    case 1: bgColor = COLOR_GREEN; break;
                    case 2: bgColor = COLOR_RED; break;
                    case 3: bgColor = COLOR_WHITE; break;
                }
                currentPage = PAGE_SECOND;
                currentSettingsOption = COLOR_SELECT;
                break;
        }
        needs_full_redraw = 1;
        HAL_Delay(200);
    }
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
    ILI9341_FillRectangle(x, y, barWidth, size, color);
    ILI9341_FillRectangle(x + barWidth + spacing, y, barWidth, size, color);
}

