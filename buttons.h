#pragma once
#include "stm32f4xx_hal.h"

typedef enum {
	BUTTON_EVENT_NONE,
	BUTTON_EVENT_UP,
	BUTTON_EVENT_DOWN,
	BUTTON_EVENT_SELECT,
	BUTTON_EVENT_RIGHT,
	BUTTON_EVENT_LEFT,
	BUTTON_EVENT_BACK
}ButtonEventType;


typedef void (*ButtonHandler)(ButtonEventType event);

void Buttons_init(void);
void Buttons_SetHandler(ButtonHandler handler);

