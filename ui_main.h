#pragma once
#include "ui_core.h"
#include "stm32f4xx_hal.h"  // Replace with your HAL header
#include <stdbool.h>


typedef enum {
	MAIN_OPTION_OCS,
	MAIN_OPTION_FAN,
	MAIN_OPTION_SENSOR,
	MAIN_OPTION_SETTINGS
}MainOption;



void MainPage_Init(void);
void MainPage_Draw(void);
void MainPage_ButtonHandler(ButtonEventType event);
void ledToggle(void);
void MainPage_ForceRedraw(void);