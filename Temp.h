#pragma once
#include "ui_core.h"
#include "stm32f4xx_hal.h"


#define AHT10_ADD 0x38
#define AHT10_INIT 0xE1
#define AHT10_RESET 0xBA
#define AHT10_TRIG 0xAC

void TempPage_ButtonHandler(ButtonEventType event);
void TempPage_Init(void);
void TempPage_Draw(void);



