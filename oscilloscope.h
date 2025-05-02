#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "stm32f4xx_hal.h"
#include "ui_core.h"

#define ADC_BUFFER_SIZE 480

typedef struct {
    uint8_t needs_clear;
    float prev_voltage;
    int prev_waveform[240];
    uint32_t samples_per_div;
} OscilloscopeState;

typedef enum {
    TIMEDIV_1MS,
    TIMEDIV_2MS,
    TIMEDIV_5MS,
    TIMEDIV_10MS,
    TIMEDIV_COUNT
} TimeDivSetting;

typedef enum {
    SETTING_BACK,
    SETTING_COUNT
} OscOption;

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern OscilloscopeState osc_state;
extern const uint32_t adc_sample_rates[TIMEDIV_COUNT];
extern const float time_div_values[TIMEDIV_COUNT];
extern TimeDivSetting current_time_div;
extern volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
extern volatile uint8_t adc_ready;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void Osc_InitADC(void);
float Process_ADC_Data(void);
void Osc_Draw(void);
void Osc_ButtonHandler(ButtonEventType event);
void OscPage_Init(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void UpdateTimebaseSettings(void);
uint8_t IsGridPixel(int x, int y);
#endif /* OSCILLOSCOPE_H */
