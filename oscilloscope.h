#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "stm32f4xx_hal.h"
#include "buttons.h"

#define ADC_BUFFER_SIZE 1024
#define WAVEFORM_WIDTH 240
#define MAX_EDGE_EVENTS 50

typedef struct {
    uint32_t timestamp;
    uint8_t edge_type; 
} EdgeEvent;

typedef struct {
    uint8_t needs_clear;
    float prev_voltage;
    int16_t prev_waveform[240];
    float vpp;
    float frequency;
    float trigger_level;
    uint8_t trigger_edge;
    uint8_t timebase;
    uint8_t volts_per_div;
    uint8_t autoset_active;
    uint8_t show_edges;
    EdgeEvent edge_events[MAX_EDGE_EVENTS];
    uint16_t edge_count;
    uint32_t last_capture;
    uint32_t timer_freq;
} OscilloscopeState;

extern OscilloscopeState osc_state;
extern volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
extern volatile uint8_t adc_ready;

void Osc_InitADC(void);
void Osc_InitTimerCapture(void);
void OscPage_Init(void);
void Osc_Draw(void);
void Osc_ButtonHandler(ButtonEventType event);
void Osc_AutoSet(void);
float Process_ADC_Data(void);
void TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

#endif // OSCILLOSCOPE_H
