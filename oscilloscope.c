#include "oscilloscope.h"
#include "ui_core.h"
#include "ili9341.h"
#include "buttons.h"
#include "ui_background.h"
#include <stdio.h>
#include <string.h>
#include <math.h> 

ADC_HandleTypeDef hadc1;
OscilloscopeState osc_state = {0};
TIM_HandleTypeDef htim2;

const uint32_t adc_sample_rates[TIMEDIV_COUNT] = {
    1000000,  // 1ms/div: 1MHz (1µs/sample)
    500000,   // 2ms/div: 500kHz (2µs/sample)
    200000,   // 5ms/div: 200kHz (5µs/sample)
    100000    // 10ms/div: 100kHz (10µs/sample)
};

TimeDivSetting current_time_div = TIMEDIV_1MS;
const float time_div_values[TIMEDIV_COUNT] = {0.001f, 0.002f, 0.005f, 0.01f};

uint8_t captureIndex = 0;
uint32_t icValue1 = 0, icValue2 = 0, icRising = 0, icFalling = 0;
float freq = 0, dutyCycle = 0;
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile uint8_t adc_ready = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    adc_ready = 1;
}

void Osc_InitADC(void) {
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE);
}

float Process_ADC_Data(void) {
    static float filtered_voltage = 0;
    const float alpha = 0.1;
    
    if(adc_ready) {
        uint32_t sum = 0;
        for(int i=0; i<ADC_BUFFER_SIZE; i++) {
            sum += adc_buffer[i];
        }
        float raw_voltage = (sum * 3.0f) / (4095.0f * ADC_BUFFER_SIZE);
        filtered_voltage = alpha * raw_voltage + (1-alpha) * filtered_voltage;
        adc_ready = 0;
        return filtered_voltage;
    }
    return -1;
}


//void Osc_Draw(void) {
//    // Measurement display area (top section)
//    #define MEASUREMENT_AREA_HEIGHT 70
//    #define GRID_START_Y (MEASUREMENT_AREA_HEIGHT + 10)
//    #define GRID_END_Y 310
//    #define GRID_START_X 50
//    #define GRID_END_X 290
//    #define GRID_CENTER_Y ((GRID_END_Y + GRID_START_Y)/2)

//    // Clear screen if needed
//    if (osc_state.needs_clear) {
//        ILI9341_FillScreen(COLOR_BLACK);
//        
//        // Draw measurement headers
//        ILI9341_DrawString(100, 10, "OSCILLOSCOPE", COLOR_CYAN, COLOR_BLACK, 1);
//        ILI9341_DrawString(20, 30, "Freq:", COLOR_WHITE, COLOR_BLACK, 1);
//        ILI9341_DrawString(20, 50, "Duty:", COLOR_WHITE, COLOR_BLACK, 1);
//        ILI9341_DrawString(120, 30, "Voltage:", COLOR_WHITE, COLOR_BLACK, 1);
//        ILI9341_DrawString(200, 30, "Time/div:", COLOR_WHITE, COLOR_BLACK, 1);
//        
//        // Draw oscilloscope grid (below measurements)
//        // Main axes (bright green)
//        ILI9341_DrawLine(GRID_START_X, GRID_START_Y, GRID_START_X, GRID_END_Y, COLOR_GREEN);  // Vertical axis
//        ILI9341_DrawLine(GRID_START_X, GRID_CENTER_Y, GRID_END_X, GRID_CENTER_Y, COLOR_GREEN); // Horizontal axis
//        
//        // Vertical divisions (every 20 pixels)
//        for(int y = GRID_START_Y; y <= GRID_END_Y; y += 20) {
//            ILI9341_DrawLine(GRID_START_X-5, y, GRID_START_X+5, y, COLOR_DARKGRAY);  // Tick marks
//            if(y != GRID_CENTER_Y) {  // Don't draw grid line on center axis
//                ILI9341_DrawLine(GRID_START_X, y, GRID_END_X, y, COLOR_DARKGRAY);
//            }
//        }
//        
//        // Horizontal divisions (every 30 pixels)
//        for(int x = GRID_START_X; x <= GRID_END_X; x += 30) {
//            ILI9341_DrawLine(x, GRID_CENTER_Y-5, x, GRID_CENTER_Y+5, COLOR_DARKGRAY);  // Tick marks
//            if(x != GRID_START_X) {  // Don't draw grid line on vertical axis
//                ILI9341_DrawLine(x, GRID_START_Y, x, GRID_END_Y, COLOR_DARKGRAY);
//            }
//        }
//        
//        osc_state.needs_clear = 0;
//    }

//    // Update measurements display
//    float voltage = Process_ADC_Data();
//    if (voltage >= 0) {
//        if (fabs(voltage - osc_state.prev_voltage) > 0.01f) {
//            char voltage_str[20];
//            snprintf(voltage_str, sizeof(voltage_str), "%2.2f V", voltage);
//            ILI9341_DrawString(180, 30, "        ", COLOR_BLACK, COLOR_BLACK, 1);
//            ILI9341_DrawString(180, 30, voltage_str, COLOR_YELLOW, COLOR_BLACK, 1);
//            osc_state.prev_voltage = voltage;
//        }
//    }

//    // Update frequency display
//    static float prev_freq = 0;
//    if (fabs(freq - prev_freq) > 0.1f) {
//        char freq_str[20];
//        if (freq < 1000) {
//            snprintf(freq_str, sizeof(freq_str), "%3.1f Hz", freq);
//        } else {
//            snprintf(freq_str, sizeof(freq_str), "%3.1f kHz", freq/1000);
//        }
//        ILI9341_DrawString(60, 30, "            ", COLOR_BLACK, COLOR_BLACK, 1);
//        ILI9341_DrawString(60, 30, freq_str, COLOR_CYAN, COLOR_BLACK, 1);
//        prev_freq = freq;
//    }

//    // Update duty cycle display
//    static float prev_DutyCycle = 0;
//    if (fabs(dutyCycle - prev_DutyCycle) > 0.1f) {
//        char duty_str[20];
//        snprintf(duty_str, sizeof(duty_str), "%3.1f %%", dutyCycle);
//        ILI9341_DrawString(60, 50, "            ", COLOR_BLACK, COLOR_BLACK, 1);
//        ILI9341_DrawString(60, 50, duty_str, COLOR_CYAN, COLOR_BLACK, 1);
//        prev_DutyCycle = dutyCycle;
//    }

//    // Update time division display
//    static TimeDivSetting prev_time_div = TIMEDIV_COUNT;
//    if(current_time_div != prev_time_div) {
//        char time_div_str[20];
//        snprintf(time_div_str, sizeof(time_div_str), "%d ms/div", 
//                (int)(time_div_values[current_time_div] * 1000));
//        ILI9341_DrawString(260, 30, "          ", COLOR_BLACK, COLOR_BLACK, 1);
//        ILI9341_DrawString(260, 30, time_div_str, COLOR_GREEN, COLOR_BLACK, 1);
//        prev_time_div = current_time_div;
//    }

//    // Draw waveform on grid
//    for (int i = 0; i < (GRID_END_X-GRID_START_X); i++) {
//        uint32_t sample_step = (ADC_BUFFER_SIZE / (GRID_END_X-GRID_START_X)) * (time_div_values[TIMEDIV_1MS] / time_div_values[current_time_div]);
//        uint32_t idx = i * sample_step;
//        if(idx >= ADC_BUFFER_SIZE) idx = ADC_BUFFER_SIZE - 1;
//        
//        // Map to grid coordinates
//        int x = GRID_START_X + i;
//        int y = GRID_CENTER_Y - (adc_buffer[idx] * (GRID_END_Y-GRID_START_Y)/2 / 4095);
//        
//        // Constrain to grid area
//        y = (y < GRID_START_Y) ? GRID_START_Y : y;
//        y = (y > GRID_END_Y) ? GRID_END_Y : y;
//        
//        if (y != osc_state.prev_waveform[i]) {
//            // Only erase if not on grid line
//            if(!IsGridPixel(GRID_START_X+i, osc_state.prev_waveform[i])) {
//                ILI9341_DrawPixel(GRID_START_X+i, osc_state.prev_waveform[i], COLOR_BLACK);
//            }
//            // Draw new sample point
//            ILI9341_DrawPixel(x, y, COLOR_YELLOW);
//            osc_state.prev_waveform[i] = y;
//        }
//    }
//}

//uint8_t IsGridPixel(int x, int y) {
//    // Check if position is on any grid line
//    if(x == GRID_START_X) return 1;
//    if((x - GRID_START_X) % 30 == 0) return 1;
//    if(y == GRID_CENTER_Y) return 1;
//    if((y - GRID_START_Y) % 20 == 0) return 1;
//    return 0;
//}


void Osc_Draw(void) {
    if (osc_state.needs_clear) {
                ILI9341_FillScreen(COLOR_BLACK);
        
        // Draw oscilloscope grid background
        // Main axes (bright green)
//        ILI9341_DrawLine(50, 50, 50, 230, COLOR_GREEN);  // Vertical axis
//        ILI9341_DrawLine(50, 140, 290, 140, COLOR_GREEN); // Horizontal axis
       
        // Vertical divisions (every 20 pixels, 5 divisions total)
//        for(int y = 50; y <= 230; y += 20) {
//            ILI9341_DrawLine(45, y, 55, y, COLOR_DARKGRAY);  // Tick marks
//            if(y != 140) {  // Don't draw grid line on center axis
//                ILI9341_DrawLine(50, y, 290, y, COLOR_DARKGRAY);
//            }
//        }
//        
//        // Horizontal divisions (every 30 pixels, 8 divisions total)
//        for(int x = 50; x <= 290; x += 30) {
//            ILI9341_DrawLine(x, 135, x, 145, COLOR_DARKGRAY);  // Tick marks
//            if(x != 50) {  // Don't draw grid line on vertical axis
//                ILI9341_DrawLine(x, 50, x, 230, COLOR_DARKGRAY);
//            }
//        }
        ILI9341_DrawString(100, 10, "OSCILLOSCOPE", COLOR_WHITE, current_bg_Color, 1);
        ILI9341_DrawString(180, 20, "Time/div:", COLOR_WHITE, current_bg_Color, 1);
        osc_state.needs_clear = 0;
    }

    float voltage = Process_ADC_Data();
    if (voltage >= 0) {
        if (fabs(voltage - osc_state.prev_voltage) > 0.01f) {
            char voltage_str[20];
            snprintf(voltage_str, sizeof(voltage_str), "%2.2f V", voltage);
            ILI9341_DrawString(20, 60, "        ", COLOR_BLACK, current_bg_Color, 3);
            ILI9341_DrawString(20, 60, voltage_str, COLOR_YELLOW, current_bg_Color, 1);
            osc_state.prev_voltage = voltage;
        }
    }

    static float prev_freq = 0;
    if (fabs(freq - prev_freq) > 0.1f) {
        char freq_str[20];
        if (freq < 1000) {
            snprintf(freq_str, sizeof(freq_str), "%3.1f Hz", freq);
        } else {
            snprintf(freq_str, sizeof(freq_str), "%3.1f kHz", freq/1000);
        }
        ILI9341_DrawString(20, 20, "            ", COLOR_BLACK, current_bg_Color, 1);
        ILI9341_DrawString(20, 20, freq_str, COLOR_CYAN, current_bg_Color, 1);
        prev_freq = freq;
    }

    static float prev_DutyCycle = 0;
    if (fabs(dutyCycle - prev_DutyCycle) > 0.1f) {
        char duty_str[20];
        snprintf(duty_str, sizeof(duty_str), "%3.1f %%", dutyCycle);
        ILI9341_DrawString(20, 40, "            ", COLOR_BLACK, current_bg_Color, 1);
        ILI9341_DrawString(20, 40, duty_str, COLOR_CYAN, current_bg_Color, 1);
        prev_DutyCycle = dutyCycle;
    }

    static TimeDivSetting prev_time_div = TIMEDIV_COUNT;
    if(current_time_div != prev_time_div) {
        char time_div_str[20];
        snprintf(time_div_str, sizeof(time_div_str), "%d ms/div", 
                (int)(time_div_values[current_time_div] * 1000));
        ILI9341_DrawString(180, 30, "          ", COLOR_BLACK, current_bg_Color, 1);
        ILI9341_DrawString(180, 30, time_div_str, COLOR_GREEN, current_bg_Color, 1);
        prev_time_div = current_time_div;
    }

    for (int i = 0; i < 240; i++) {
        uint32_t sample_step = (ADC_BUFFER_SIZE / 240) * (time_div_values[TIMEDIV_1MS] / time_div_values[current_time_div]);
        uint32_t idx = i * sample_step;
        if(idx >= ADC_BUFFER_SIZE) idx = ADC_BUFFER_SIZE - 1;
        
        int y = 200 - (adc_buffer[idx] * 100 / 4095);
        
        if (y != osc_state.prev_waveform[i]) {
            ILI9341_DrawPixel(i+50, osc_state.prev_waveform[i], current_bg_Color);
            ILI9341_DrawPixel(i+50, y, COLOR_YELLOW);
            osc_state.prev_waveform[i] = y;
        }
    }
}

void Osc_ButtonHandler(ButtonEventType event) {
    switch(event) {
        case BUTTON_EVENT_SELECT:
            UI_NavigateTo(PAGE_MAIN);
            break;
        case BUTTON_EVENT_UP:
            if(current_time_div > 0) {
                current_time_div--;
                UpdateTimebaseSettings();
            }
            break;
        case BUTTON_EVENT_DOWN:
            if(current_time_div < TIMEDIV_COUNT-1) {
                current_time_div++;
                UpdateTimebaseSettings();
            }
            break;
    }
}

void OscPage_Init(void) {
    memset(&osc_state, 0, sizeof(osc_state));
    osc_state.needs_clear = 1;
    current_time_div = TIMEDIV_1MS;
    osc_state.samples_per_div = (uint32_t)(time_div_values[TIMEDIV_1MS] * adc_sample_rates[TIMEDIV_1MS]);
    Buttons_SetHandler(Osc_ButtonHandler);
    UpdateTimebaseSettings();
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if(htim->Instance == TIM2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        uint32_t currentCapture = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
        
        if(captureIndex == 0) {
            icValue1 = currentCapture;
            captureIndex = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if(captureIndex == 1) {
            icFalling = currentCapture;
            captureIndex = 2;
            __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
        }
        else if(captureIndex == 2) {
            icValue2 = currentCapture;
            uint32_t period = (icValue2 > icValue1) ? (icValue2 - icValue1) : ((0xFFFF - icValue1) + icValue2 + 1);
            uint32_t highTime = (icFalling > icValue1) ? (icFalling - icValue1) : ((0xFFFF - icValue1) + icFalling + 1);
            freq = 1000000.0f / period;
            dutyCycle = (highTime * 100.0f) / period;
            captureIndex = 0;
        }
    }
}

void UpdateTimebaseSettings(void) {
    HAL_ADC_Stop_DMA(&hadc1);
    osc_state.samples_per_div = (uint32_t)(time_div_values[current_time_div] * adc_sample_rates[current_time_div]);
    htim2.Instance->ARR = (SystemCoreClock / adc_sample_rates[current_time_div]) - 1;
    HAL_TIM_Base_Init(&htim2);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE);
    osc_state.needs_clear = 1;
}
