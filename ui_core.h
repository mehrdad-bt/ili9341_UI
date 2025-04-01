#pragma once
#include "main.h"
#include "ili9341.h"

typedef enum {
	PAGE_MAIN,
	PAGE_SETTING,
	PAGE_COLOR_SELECT
}PageID;

extern uint16_t current_bg_Color;

void UI_Init(void);
void UI_NavigateTo(PageID page);
void UI_UpdateDisplay(void);

