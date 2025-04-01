#include "buttons.h"
#include "stm32f4xx_it.h"


static ButtonHandler current_button_handler =NULL;

//void Buttons_Init(void) {
//		
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	
//	//enable clock
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	
//	//configure pins
//	
//	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//	
//	
//	  /* EXTI interrupt init*/
//  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

//  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

//  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
//	
//}
	
void Buttons_SetHandler(ButtonHandler handler) 
{
	current_button_handler = handler;
}

//interrupt handler

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t last_interrupt_time = 0;
	uint32_t current_time = HAL_GetTick();
	
	if(current_time - last_interrupt_time >200)
	{
		last_interrupt_time = current_time;
		
		if(current_button_handler !=NULL)
		{
			if(GPIO_Pin == GPIO_PIN_0) {
				current_button_handler(BUTTON_EVENT_UP);
			}
		else if(GPIO_Pin == GPIO_PIN_1) {
			current_button_handler(BUTTON_EVENT_DOWN);
		}
		else if(GPIO_Pin == GPIO_PIN_2) {
			current_button_handler(BUTTON_EVENT_SELECT);
	}
}
		}
	}
	

