#pragma once
#include "ui_core.h"
#include "buttons.h"

#define COLOR_COUNT 6
typedef struct {
	uint16_t value;
	const char* name;
}ColorOption;


void ColorsPage_Init(void);
void ColorsPage_Draw(void);
void ColorsPage_ButtonHandler(ButtonEventType event);