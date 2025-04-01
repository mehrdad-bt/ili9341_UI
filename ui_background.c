#include "ui_background.h"
#include "ui_core.h"
#include "ili9341.h"
#include "buttons.h"




static uint8_t selected_index = 0;

static const ColorOption colors[] = {
	{COLOR_BLACK,			"Black"},
	{COLOR_BLUE,			"Blue"},
	{COLOR_RED,			"Red"},
	{COLOR_GREEN,			"Green"},
	{COLOR_AMBER,			"Amber"},
	{COLOR_PURPLE,			"Purple"},
};
	




static void ColorsPage_ButtonHandler(ButtonEventType event)
{
	switch(event) 
	{
		case BUTTON_EVENT_UP:
			selected_index = (selected_index + 1) %COLOR_COUNT;
		break;

		case BUTTON_EVENT_DOWN:
			selected_index = (selected_index - 1 + COLOR_COUNT) %COLOR_COUNT;
		break;

		case BUTTON_EVENT_SELECT:
			current_bg_Color = colors[selected_index].value;
			UI_NavigateTo(PAGE_SETTING);
			return; //skip redraw scince we are leaving
				
	}
	UI_UpdateDisplay();
	
}





void ColorsPage_Init(void)
{
	selected_index = 0;
	Buttons_SetHandler(ColorsPage_ButtonHandler);
	
}

void ColorsPage_Draw(void)
{
	//clear screen with current color
	ILI9341_FillScreen(current_bg_Color);
	ILI9341_DrawString(80,10,"SELECT COLOR", COLOR_WHITE, current_bg_Color, 2); //draw title
	
	//draw color options
	
	for(uint8_t i=0; i< COLOR_COUNT ; i++)
	{
		uint16_t y_pos = 50 + i * 30;
		uint16_t text_color = (colors[i].value == COLOR_BLACK) ? COLOR_WHITE : COLOR_BLACK;
		
		//highlight selected color
		if(i == selected_index)
		{
			
		ILI9341_FillRectangle(40, y_pos - 2, 240, 30, COLOR_WHITE);
		text_color = colors[i].value;	
			
		}
		//draw color preview box
		ILI9341_FillRectangle(45, y_pos + 2, 20, 20, colors[i].value);
		
		//draw color name

		ILI9341_DrawString(70,y_pos + 5, colors[i].name, text_color,(i == selected_index) ? COLOR_WHITE : current_bg_Color, 1);
		
	}
	//draw instructions
	
//	ILI9341_DrawString(30,220, "Press Select to confirm", COLOR_WHITE, current_bg_Color, 1);
}

