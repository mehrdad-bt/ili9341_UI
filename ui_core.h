#pragma once
#include "main.h"
#include "ili9341.h"
#include "buttons.h"

typedef enum {
	PAGE_MAIN,
	PAGE_SETTING,
	PAGE_COLOR_SELECT
}PageID;

extern PageID current_page;
extern uint16_t current_bg_Color;

void UI_Init(void);
void UI_NavigateTo(PageID page);
void UI_UpdateDisplay(void);
void UI_ButtonRouter(ButtonEventType event);

