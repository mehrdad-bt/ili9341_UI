#pragma once
#include "stm32f4xx_hal.h"

typedef enum {
	BUTTON_EVENT_UP,
	BUTTON_EVENT_DOWN,
	BUTTON_EVENT_SELECT
}ButtonEventType;


typedef void (*ButtonHandler)(ButtonEventType event);

void Buttons_init(void);
void Buttons_SetHandler(ButtonHandler handler);

