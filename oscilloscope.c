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

typedef enum {
	

	SETTING_BACK,
	SETTING_COUNT,
}OscOption;


volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile uint8_t adc_ready = 0;


//static settingsOption current_option = SETTING_COLOR;
static OscOption current_option = SETTING_BACK;


//static void SettingsPage_ButtonHandler(ButtonEventType event) {
//    switch(event) {
//        case BUTTON_EVENT_UP:
//            current_option = (current_option + 1) % SETTING_COUNT;
//            break;
//        case BUTTON_EVENT_DOWN:
//            current_option = (current_option - 1 + SETTING_COUNT) % SETTING_COUNT;
//            break;
//        case BUTTON_EVENT_SELECT:
//            if (current_option == SETTING_COLOR) {
//                UI_NavigateTo(PAGE_COLOR_SELECT);  // Non-blocking
//                return;
//            } else {
//                UI_NavigateTo(PAGE_MAIN);  // Non-blocking
//                return;
//            }
//    }
//    // Non-blocking redraw
//    osMutexWait(RecursiveMutexHandle, osWaitForever);
//    SettingsPage_Draw();  // Avoid UI_UpdateDisplay() if possible
//    osMutexRelease(RecursiveMutexHandle);
//}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    adc_ready = 1;
}

void Osc_InitADC(void) {
 //   HAL_ADCEx_Calibration_Start(&hadc1);  // Calibrate ADC
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
    // Only clear screen if needed (e.g., when entering page)
    if (osc_state.needs_clear) {
        ILI9341_FillScreen(current_bg_Color);
        ILI9341_DrawString(100, 10, "OSCILLOSCOPE", COLOR_WHITE, current_bg_Color, 1);
        osc_state.needs_clear = 0;
    }

    float voltage = Process_ADC_Data();
    if (voltage >= 0) {
        // Update voltage text only if changed
        if (fabs(voltage - osc_state.prev_voltage) > 0.01f) {
            char voltage_str[20];
            snprintf(voltage_str, sizeof(voltage_str), "%2.2f V", voltage);
            
            // Clear previous text by redrawing with bg color
            ILI9341_DrawString(50, 50, "        ", COLOR_BLACK, current_bg_Color, 3);
            ILI9341_DrawString(250, 30, voltage_str, COLOR_YELLOW, current_bg_Color, 1);
            
            osc_state.prev_voltage = voltage;
        }

        // Update waveform
        for (int i = 0; i < 240; i++) {
            int idx = i * ADC_BUFFER_SIZE / 240;
            int y = 150 - (adc_buffer[idx] * 100 / 4095);
            
            // Only redraw changed pixels
            if (y != osc_state.prev_waveform[i]) {
                // Erase old pixel
                ILI9341_DrawPixel(i+20, osc_state.prev_waveform[i], current_bg_Color);
                // Draw new pixel
                ILI9341_DrawPixel(i+20, y, COLOR_YELLOW);
                osc_state.prev_waveform[i] = y;
            }
        }
    }
}


void Osc_ButtonHandler(ButtonEventType event) {

	switch(event)
	{
		case BUTTON_EVENT_UP:
//		  pwm_duty = (pwm_duty < DUTY_STEP) ? 0 : pwm_duty - DUTY_STEP;
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);
//			if(FanSpeed>0) FanSpeed--;
//		  UI_UpdateDisplay();
				break;
//			
		case BUTTON_EVENT_DOWN:
//			pwm_duty = (pwm_duty + DUTY_STEP) > 200 ? 200 : pwm_duty + DUTY_STEP;
//    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_duty);
//			if(FanSpeed < 7) FanSpeed++;
//			UI_UpdateDisplay();
				break;
			
		case BUTTON_EVENT_SELECT:
			 if(current_option == SETTING_BACK)
			{
				 UI_NavigateTo(PAGE_MAIN);
		     
			}
			break;
//			else if(current_option == MAIN_OPTION_PLAY)
//			{
//				 led_blink_enabled = 1;
//			}
						
		
}
	}


void OscPage_Init(void) {
    memset(&osc_state, 0, sizeof(osc_state));
    osc_state.needs_clear = 1; // Force clear on first draw
    Buttons_SetHandler(Osc_ButtonHandler);
}



/* Draw settings page */
//void Osc_Draw(void) {
//    // Clear screen with current background
//    ILI9341_FillScreen(current_bg_Color);
//    
//    // Draw title
//    ILI9341_DrawString(100, 10, "oscilloscope", COLOR_WHITE, current_bg_Color, 2);
//    
//    // Draw all options
////    const char* options[SETTING_COUNT] = {"Change Color", "Back to Main"};
//    
////    for(uint8_t i = 0; i < SETTING_COUNT; i++) {
////        uint16_t y_pos = 50 + i * 40;
////        ILI9341_DrawString(20, y_pos, options[i], COLOR_WHITE, current_bg_Color, 2);
////        // Highlight selected option
////        if(i == current_option) {
//// //           ILI9341_FillRectangle(20, y_pos - 5, 240, 30, COLOR_YELLOW);
////            ILI9341_DrawString(20, y_pos, options[i], COLOR_BLACK, COLOR_YELLOW, 2);
////        }
////    }
//		
//		
//		read_ADC_voltage();

//    
//    // Current color preview
////    ILI9341_FillRectangle(180, 150, 80, 40, current_bg_Color);
////    ILI9341_DrawRectangle(179, 149, 82, 42, COLOR_WHITE); // Border
////    ILI9341_DrawString(150, 200, "Current Color", COLOR_BLUE, current_bg_Color, 1);
//    
//    // Instructions
////    ILI9341_DrawString(30, 220, "Press OPTION to select", COLOR_BLUE, current_bg_Color, 1);
//}

//		


//		float read_ADC_voltage(void) {
//    uint32_t adc_value;
//    float voltage;
//		char raw_voltage[7];

//    // Start ADC conversion
//    HAL_ADC_Start(&hadc1);
//			while(1)
//			{
//    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
//        adc_value = HAL_ADC_GetValue(&hadc1);
//        voltage = (adc_value * 3.0f) / 4095.0f;  // 12-bit resolution, 3.3V reference
//    }
//    HAL_ADC_Stop(&hadc1);
//		sprintf(raw_voltage, "%02.2f V", voltage);
//		ILI9341_DrawString(200, 200, raw_voltage, COLOR_WHITE, current_bg_Color, 3);
//    return voltage;
//		HAL_Delay(200);
//		if(BUTTON_EVENT_SELECT) return 0;
//	}
//		
//}
		
