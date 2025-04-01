#pragma once
#include "ui_core.h"

typedef enum {
	MAIN_OPTION_PLAY,
	MAIN_OPTION_PAUSE,
	MAIN_OPTION_SETTINGS
}MainOption;


void MainPage_Init(void);
void MainPage_Draw(void);
