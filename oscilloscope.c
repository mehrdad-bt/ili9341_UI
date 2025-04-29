#include "oscilloscope.h"
#include "ui_core.h"
#include "ili9341.h"
#include "buttons.h"
#include "ui_background.h"
#include <stdio.h>
#include <string.h>
#include <math.h>


ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim2;
OscilloscopeState osc_state = {0};

typedef enum {
    SETTING_BACK,
    SETTING_AUTOSET,
    SETTING_TRIGGER,
    SETTING_TIMEBASE,
    SETTING_VOLTAGE_SCALE,
    SETTING_EDGE_DISPLAY,
    SETTING_COUNT
} OscOption;

volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile uint8_t adc_ready = 0;
static OscOption current_option = SETTING_BACK;

void ILI9341_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    ILI9341_DrawPixel(x0, y0 + r, color);
    ILI9341_DrawPixel(x0, y0 - r, color);
    ILI9341_DrawPixel(x0 + r, y0, color);
    ILI9341_DrawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ILI9341_DrawPixel(x0 + x, y0 + y, color);
        ILI9341_DrawPixel(x0 - x, y0 + y, color);
        ILI9341_DrawPixel(x0 + x, y0 - y, color);
        ILI9341_DrawPixel(x0 - x, y0 - y, color);
        ILI9341_DrawPixel(x0 + y, y0 + x, color);
        ILI9341_DrawPixel(x0 - y, y0 + x, color);
        ILI9341_DrawPixel(x0 + y, y0 - x, color);
        ILI9341_DrawPixel(x0 - y, y0 - x, color);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    adc_ready = 1;
}

void Osc_InitADC(void) {
//    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE);
}

void Osc_InitTimerCapture(void) {
    // Start timer with input capture on both edges
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    osc_state.timer_freq = HAL_RCC_GetPCLK1Freq() * 2; // For STM32F4
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != 0) {
        osc_state.timer_freq /= 2;
    }
}

void TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    static uint32_t last_capture = 0;
    uint32_t current_capture;
    
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        current_capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        
        if (osc_state.edge_count < MAX_EDGE_EVENTS) {
            // Determine edge type
            if (htim->Instance->CCER & TIM_CCER_CC1P) {
                // Falling edge detected
                osc_state.edge_events[osc_state.edge_count].edge_type = 1;
            } else {
                // Rising edge detected
                osc_state.edge_events[osc_state.edge_count].edge_type = 0;
            }
            
            // Calculate timestamp
            if (last_capture > current_capture) {
                osc_state.edge_events[osc_state.edge_count].timestamp = 
                    (0xFFFFFFFF - last_capture) + current_capture;
            } else {
                osc_state.edge_events[osc_state.edge_count].timestamp = 
                    current_capture - last_capture;
            }
            
            osc_state.edge_count++;
        }
        
        last_capture = current_capture;
        
        // Toggle polarity for next edge detection
        htim->Instance->CCER ^= TIM_CCER_CC1P;
    }
}

float Process_ADC_Data(void) {
    static float filtered_voltage = 0;
    const float alpha = 0.1f;
    
    if(adc_ready) {
        uint32_t sum = 0;
        for(int i=0; i<ADC_BUFFER_SIZE; i++) {
            sum += adc_buffer[i];
        }
        float raw_voltage = (sum * 3.3f * 21.0f) / (4095.0f * ADC_BUFFER_SIZE);
        filtered_voltage = alpha * raw_voltage + (1-alpha) * filtered_voltage;
        adc_ready = 0;
        return filtered_voltage;
    }
    return -1.0f;
}

void Process_Edge_Events(void) {
    if (osc_state.edge_count < 2) return;
    
    // Calculate frequency from first two edges
    uint32_t period = osc_state.edge_events[1].timestamp - osc_state.edge_events[0].timestamp;
    osc_state.frequency = (float)osc_state.timer_freq / period;
    
    // Clear processed events
    osc_state.edge_count = 0;
}

void Osc_AutoSet(void) {
    uint16_t min_val = 4095;
    uint16_t max_val = 0;
    uint32_t sum = 0;
    
    for(int i = 0; i < ADC_BUFFER_SIZE; i++) {
        if(adc_buffer[i] < min_val) min_val = adc_buffer[i];
        if(adc_buffer[i] > max_val) max_val = adc_buffer[i];
        sum += adc_buffer[i];
    }
    
    float vpp = (max_val - min_val) * 3.3f / 4095.0f;
    osc_state.volts_per_div = (uint8_t)ceilf(vpp / 4.0f * 2.0f) / 2.0f;
    if(osc_state.volts_per_div < 0.5f) osc_state.volts_per_div = 0.5f;
    if(osc_state.volts_per_div > 5.0f) osc_state.volts_per_div = 5.0f;
    
    osc_state.trigger_level = ((max_val + min_val) / 2) / 4095.0f;
    osc_state.vpp = vpp;
    osc_state.autoset_active = 1;
    osc_state.show_edges = 1;
    
    // Use timer capture for frequency measurement
    Process_Edge_Events();
}

void Osc_DrawGrid(void) {
    for(int x = 20; x <= 260; x += 20) {
        ILI9341_DrawLine(x, 30, x, 210, COLOR_DARKGRAY);
    }
    for(int y = 30; y <= 210; y += 20) {
        ILI9341_DrawLine(20, y, 260, y, COLOR_DARKGRAY);
    }
    ILI9341_DrawLine(20, 120, 260, 120, COLOR_WHITE);
    ILI9341_DrawLine(140, 30, 140, 210, COLOR_WHITE);
}

void Osc_DrawMeasurements(void) {
    char info_str[40];
    
    snprintf(info_str, sizeof(info_str), "Vpp: %2.2fV", osc_state.vpp);
    ILI9341_DrawString(10, 220, info_str, COLOR_CYAN, current_bg_Color, 1);
    
    snprintf(info_str, sizeof(info_str), "Freq: %2.2fHz", osc_state.frequency);
    ILI9341_DrawString(100, 220, info_str, COLOR_CYAN, current_bg_Color, 1);
    
    snprintf(info_str, sizeof(info_str), "%dus/div", osc_state.timebase);
    ILI9341_DrawString(200, 220, info_str, COLOR_CYAN, current_bg_Color, 1);
    
    snprintf(info_str, sizeof(info_str), "%0.1fV/div", osc_state.volts_per_div);
    ILI9341_DrawString(200, 10, info_str, COLOR_CYAN, current_bg_Color, 1);
    
    snprintf(info_str, sizeof(info_str), "Trig: %s@%2.1fV", 
            osc_state.trigger_edge ? "FALL" : "RISE", 
            osc_state.trigger_level * 3.3f);
    ILI9341_DrawString(10, 10, info_str, COLOR_YELLOW, current_bg_Color, 1);
    
    if (osc_state.show_edges) {
        for (int i = 0; i < osc_state.edge_count && i < MAX_EDGE_EVENTS; i++) {
            int x_pos = 20 + (i * 5);
            if (x_pos > 260) break;
            
            if (osc_state.edge_events[i].edge_type == 0) {
                ILI9341_DrawCircle(x_pos, 120, 3, COLOR_RED);
            } else {
                ILI9341_DrawCircle(x_pos, 120, 3, COLOR_BLUE);
            }
        }
    }
}

void Osc_DrawMenu(void) {
    const char* menu_items[SETTING_COUNT] = {
        "Back to Main",
        "Auto Set",
        "Trigger Settings",
        "Timebase",
        "Voltage/Div",
        "Edge Display"
    };
    
    for(uint8_t i = 0; i < SETTING_COUNT; i++) {
        uint16_t y_pos = 30 + i * 25;
        if(i == current_option) {
            ILI9341_FillRectangle(5, y_pos - 2, 230, 20, COLOR_YELLOW);
            ILI9341_DrawString(10, y_pos, menu_items[i], COLOR_BLACK, COLOR_YELLOW, 1);
        } else {
            ILI9341_DrawString(10, y_pos, menu_items[i], COLOR_WHITE, current_bg_Color, 1);
        }
    }
}

void Osc_Draw(void) {
    if (osc_state.needs_clear) {
        ILI9341_FillScreen(current_bg_Color);
        if(current_option == SETTING_BACK) {
            ILI9341_DrawString(100, 10, "OSCILLOSCOPE", COLOR_WHITE, current_bg_Color, 1);
            Osc_DrawGrid();
        } else {
            Osc_DrawMenu();
        }
        osc_state.needs_clear = 0;
    }

    if(current_option == SETTING_BACK) {
        float voltage = Process_ADC_Data();
        if (voltage >= 0) {
            if (fabs(voltage - osc_state.prev_voltage) > 0.01f) {
                char voltage_str[20];
                snprintf(voltage_str, sizeof(voltage_str), "%2.2f V", voltage);
                ILI9341_DrawString(50, 30, "        ", current_bg_Color, current_bg_Color, 2);
                ILI9341_DrawString(50, 30, voltage_str, COLOR_YELLOW, current_bg_Color, 2);
                osc_state.prev_voltage = voltage;
            }

            for (int i = 0; i < WAVEFORM_WIDTH; i++) {
                int idx = i * ADC_BUFFER_SIZE / WAVEFORM_WIDTH;
                int y = 120 - ((adc_buffer[idx] - 2048) * 80 / 4096);
                
                if (y != osc_state.prev_waveform[i]) {
                    ILI9341_DrawPixel(i+20, osc_state.prev_waveform[i], current_bg_Color);
                    ILI9341_DrawPixel(i+20, y, COLOR_GREEN);
                    osc_state.prev_waveform[i] = y;
                }
            }
            
            if(osc_state.autoset_active) {
                Osc_DrawMeasurements();
                osc_state.autoset_active = 0;
            }
        }
    }
}

void Osc_ButtonHandler(ButtonEventType event) {
    switch(event) {
        case BUTTON_EVENT_UP:
            current_option = (current_option - 1 + SETTING_COUNT) % SETTING_COUNT;
            osc_state.needs_clear = 1;
            break;
            
        case BUTTON_EVENT_DOWN:
            current_option = (current_option + 1) % SETTING_COUNT;
            osc_state.needs_clear = 1;
            break;
            
        case BUTTON_EVENT_SELECT:
                Osc_AutoSet();
                osc_state.needs_clear = 1;

            break;
				case BUTTON_EVENT_BACK:
					UI_NavigateTo(PAGE_MAIN);
				break;
    }
}

void OscPage_Init(void) {
    memset(&osc_state, 0, sizeof(osc_state));
    osc_state.needs_clear = 1;
    osc_state.volts_per_div = 1.0f;
    osc_state.timebase = 100;
    osc_state.trigger_level = 0.5f;
    osc_state.show_edges = 1;
    Buttons_SetHandler(Osc_ButtonHandler);
    Osc_InitADC();
    Osc_InitTimerCapture();
}

