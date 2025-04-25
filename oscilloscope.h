#pragma once
#include "buttons.h"
#include "ui_core.h"
#include "stm32f4xx_hal.h"
#define ADC_BUFFER_SIZE 256
extern volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
extern volatile uint8_t adc_ready;

typedef struct {
    float prev_voltage;
    uint16_t prev_waveform[240]; // Stores previous waveform points
    uint8_t needs_clear;
} OscilloscopeState;

extern OscilloscopeState osc_state;


float read_ADC_voltage(void); 
void Osc_Draw(void);
void OscPage_Init(void);
void Osc_ButtonHandler(ButtonEventType event);
void Osc_Draw(void);
void Osc_InitADC(void);