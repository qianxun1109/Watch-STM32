#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "MPU6050.h"
#include "menu.h"
#include "Key.h"
#include "Scheduler.h"
#include "Setting_time.h"
#include "Menu_list1.h"
#include "Timer.h"
#include "LED.h"

uint8_t Pageflag = CLOCK;

int main(void)
{
	LED_Init();
	Timer_Init();
	OLED_Init();
	MyRTC_Init();
	Key_Init();
	MPU6050_Init();
	Scheduler_Init();

	
	OLED_Update();
	while (1)
	{
		switch(Pageflag)
		{
			case CLOCK:
				Pageflag = CLK_Page();
				break;
			case SETTING:
				Pageflag = Setting_Page();
				break;
			case MENU:
				Pageflag = Menu_Page();
				break;
			case SETTING_TIME:
				Pageflag = Setting_time_Page();
				break;
			case MENU_TIMER:
				Pageflag = Menu_timer_Page();
				break;
			case MENU_ALARM:
				//Pageflag = Menu_alarm_Page();
				break;
			case MENU_LIGHT:
				Pageflag = Menu_light_Page();
				break;
			case MENU_GAME:
				//Pageflag = Menu_game_time_Page();
				break;
			
			default:
				break;
			
			
		}
	}
}
