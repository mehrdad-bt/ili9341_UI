#ifndef UI_H
#define UI_H

#include "main.h"
#include "ili9341.h"

#define NEXT_BTN_PIN GPIO_PIN_0
#define BACK_BTN_PIN GPIO_PIN_1
#define OPTION_BTN_PIN GPIO_PIN_2
#define BTN_PORT GPIOB

// Type definitions
typedef enum {
    PAGE_MAIN,
    PAGE_SECOND,
    PAGE_COLOR_SELECT
} Page_t;

typedef enum {
    OPTION_PLAY,
    OPTION_PAUSE, 
    OPTION_SETTING
} MainOption_t;

typedef enum {
    COLOR_SELECT,
    SETTING1,
    SETTING2,
    BACK
} SettingsOption_t;

typedef enum { 
    PLAYER_STOPPED,
    PLAYER_PLAYING,
    PLAYER_PAUSED
} PlayerState;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    const uint16_t* normal_img;
    const uint16_t* pressed_img;
    uint8_t is_pressed;
} ImageButton_t;

// Global variables
extern Page_t currentPage;
extern MainOption_t currentMainOption;
extern SettingsOption_t currentSettingsOption;
extern PlayerState playerState;
extern uint16_t bgColor;

// Function prototypes
void UI_Init(void);
void UI_HandleInput(void);
void UI_UpdateDisplay(void);
void UI_HandlePlayerButton(void);
void UI_DrawPlayerControls(void);

#endif