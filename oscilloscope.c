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

typedef enum {
	

	SETTING_BACK,
	SETTING_COUNT,
}OscOption;

uint8_t captureIndex = 0;
uint32_t icValue1 = 0, icValue2 = 0, icRising = 0, icFalling = 0;
float freq = 0, dutyCycle = 0;
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile uint8_t adc_ready = 0;



static OscOption current_option = SETTING_BACK;



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    adc_ready = 1;
}

void Osc_InitADC(void) {

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE);
}

float Process_ADC_Data(void) {
    static float filtered_voltage = 0;
    const float alpha = 0.1;  // Filter coefficient
    
    if(adc_ready) {
        uint32_t sum = 0;
        for(int i=0; i<ADC_BUFFER_SIZE; i++) {
            sum += adc_buffer[i];
        }
        float raw_voltage = (sum * 3.0f) / (4095.0f * ADC_BUFFER_SIZE);
        
        // Low-pass filter
        filtered_voltage = alpha * raw_voltage + (1-alpha) * filtered_voltage;
        
        adc_ready = 0;
        return filtered_voltage;
    }
    return -1;  // No new data
}

void Osc_Draw(void) {
    // Only clear screen if needed
    if (osc_state.needs_clear) {
        ILI9341_FillScreen(current_bg_Color);
        ILI9341_DrawString(100, 10, "OSCILLOSCOPE", COLOR_WHITE, current_bg_Color, 1);
        osc_state.needs_clear = 0;
    }

    float voltage = Process_ADC_Data();
    if (voltage >= 0) {
        // Update voltage text
        if (fabs(voltage - osc_state.prev_voltage) > 0.01f) {
            char voltage_str[20];
            snprintf(voltage_str, sizeof(voltage_str), "%2.2f V", voltage);
            ILI9341_DrawString(20, 60, "        ", COLOR_BLACK, current_bg_Color, 3);
            ILI9341_DrawString(20, 60, voltage_str, COLOR_YELLOW, current_bg_Color, 1);
            osc_state.prev_voltage = voltage;
        }

        // Update frequency text (new addition)
        static float prev_freq = 0;
        if (fabs(freq - prev_freq) > 0.1f) {  // Only update if frequency changed significantly
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
				//Update DutyCycle
				static float prev_DutyCycle = 0;
				if (fabs(dutyCycle - prev_DutyCycle) > 0.1f) {  // Only update if frequency changed significantly
            char duty_str[20];
            if (dutyCycle < 100) {
                snprintf(duty_str, sizeof(duty_str), "%3.1f %%", dutyCycle);
            } else {
                snprintf(duty_str, sizeof(duty_str), "%3.1f %%", dutyCycle/1000);
            }
            ILI9341_DrawString(20, 40, "            ", COLOR_BLACK, current_bg_Color, 1);
            ILI9341_DrawString(20, 40, duty_str, COLOR_CYAN, current_bg_Color, 1);
            prev_DutyCycle = dutyCycle;
        }
				
				
				
        // Update waveform (existing code)
        for (int i = 0; i < 240; i++) {
            int idx = i * ADC_BUFFER_SIZE / 240;
            int y = 150 - (adc_buffer[idx] * 100 / 4095);
            
            if (y != osc_state.prev_waveform[i]) {
                ILI9341_DrawPixel(i+20, osc_state.prev_waveform[i], current_bg_Color);
                ILI9341_DrawPixel(i+20, y, COLOR_YELLOW);
                osc_state.prev_waveform[i] = y;
            }
        }
    }
}


void Osc_ButtonHandler(ButtonEventType event) {

	switch(event)
	{
		case BUTTON_EVENT_SELECT:
				 UI_NavigateTo(PAGE_MAIN);
				break;
//		
//		case BUTTON_EVENT_DOWN:

//				break;
//			
//		case BUTTON_EVENT_BACK:
//				 UI_NavigateTo(PAGE_MAIN);
//		     
//			break;

						
		
}
	}


void OscPage_Init(void) {
    memset(&osc_state, 0, sizeof(osc_state));
    osc_state.needs_clear = 1; // Force clear on first draw
    Buttons_SetHandler(Osc_ButtonHandler);
}



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            uint32_t currentCapture = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
            
            if(captureIndex == 0) // First edge (rising)
            {
                icValue1 = currentCapture;
                captureIndex = 1;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
            }
            else if(captureIndex == 1) // Second edge (falling)
            {
                icFalling = currentCapture;
                captureIndex = 2;
                __HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
            }
            else if(captureIndex == 2) // Third edge (rising - complete period)
            {
                icValue2 = currentCapture;
                
                // Calculate period (handle overflow)
                uint32_t period = (icValue2 > icValue1) ? (icValue2 - icValue1) : ((0xFFFF - icValue1) + icValue2 + 1);
                
                // Calculate high time (handle overflow)
                uint32_t highTime = (icFalling > icValue1) ? (icFalling - icValue1) : ((0xFFFF - icValue1) + icFalling + 1);
                
                freq = 1000000.0f / period;           // Your existing frequency calculation
                dutyCycle = (highTime * 100.0f) / period;  // Duty cycle in %

                captureIndex = 0;
            }
        }
    }
}


