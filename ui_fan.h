#pragma once
#include "ui_core.h"
#include "stm32f4xx_hal.h"
//typedef enum {
//	MAIN_OPTION_PLAY,
//	MAIN_OPTION_PAUSE,
//	MAIN_OPTION_SETTINGS
//}MainOption;



void FanPage_Init(void);
void FanPage_Draw(void);
void FanPage_ButtonHandler(ButtonEventType event);
void FanPage_ForceRedraw(void);
