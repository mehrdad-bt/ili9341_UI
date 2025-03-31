#ifndef UI_H
#define UI_H

#include "main.h"
#include "ili9341.h"




	#define NEXT_BTN_PIN GPIO_PIN_0
	#define BACK_BTN_PIN GPIO_PIN_1
	#define OPTION_BTN_PIN GPIO_PIN_2
	#define BTN_PORT GPIOB
	

	#define COLOR_OPTION_1 COLOR_BLUE
	#define COLOR_OPTION_2 COLOR_GREEN
	#define COLOR_OPTION_3 COLOR_RED
	#define COLOR_OPTION_4 COLOR_WHITE

	#define BACK_TEXT_X 10
	#define BACK_TEXT_Y 10
	#define BACK_TEXT_WIDTH 70
	#define BACK_TEXT_HEIGHT 20
	
	
// Type definitions
typedef enum {
    PAGE_MAIN,
    PAGE_SECOND,
    PAGE_COLOR_SELECT
} Page_t;

typedef enum {
    OPTION_PLAY,
    OPTION_PAUSE, 
    OPTION_SETTING,
    OPTION_BG_COLOR
} SelectedOption_t;

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
	}ImageButton_t;

// Global variables
extern Page_t currentPage;
extern SelectedOption_t currentOption;
extern uint16_t bgColor;

	

	
// Function prototypes
void UI_Init(void);
void UI_HandleInput(void);
void UI_UpdateDisplay(void);
void UI_HandlePlayerButton(void);
void UI_DrawPlayerControls(void);
uint8_t IsBottonPressed(uint16_t bottonPin);

#endif

