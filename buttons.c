#include "buttons.h"
#include "stm32f4xx_it.h"
#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"       // For queue functions
#include "task.h"        // For ISR-safe functions


static ButtonHandler current_button_handler =NULL;


	
void Buttons_SetHandler(ButtonHandler handler) 
{
	current_button_handler = handler;
}

//interrupt handler

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = HAL_GetTick();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (current_time - last_interrupt_time > 200) {
        last_interrupt_time = current_time;
        
        ButtonEventType event;
        if (GPIO_Pin == GPIO_PIN_0) {
            event = BUTTON_EVENT_UP;
        }
        else if (GPIO_Pin == GPIO_PIN_1) {
            event = BUTTON_EVENT_DOWN;
        }
        else if (GPIO_Pin == GPIO_PIN_2) {
            event = BUTTON_EVENT_SELECT;
        }				
        else {
            return;
        }
        
        // Send to FreeRTOS queue (thread-safe)
        xQueueSendFromISR(buttonQueue, &event, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


	

