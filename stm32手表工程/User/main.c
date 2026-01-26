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
#include "Menu_game.h"
#include "stdlib.h"
#include "Delay.h"

//uint8_t Pageflag = CLOCK;
//uint8_t Previous_Pageflag;

//int main(void)
//{
//	MyRTC_Init();
//	OLED_Init();
//	Key_Init();
//	LED_Init();
//	Timer_Init();
//	Scheduler_Init();
//	//MPU6050_Init();
//	
//	srand((unsigned int)RTC_GetCounter());
//	while (1)
//	{
//		switch(Pageflag)
//		{
//			case CLOCK:
//				Pageflag = CLK_Page();
//				break;
//			case SETTING:
//				Pageflag = Setting_Page();
//				break;
//			case MENU:
//				Pageflag = Menu_Page();
//				break;
//			case SETTING_TIME:
//				Pageflag = Setting_time_Page();
//				break;
//			case MENU_TIMER:
//				Pageflag = Menu_timer_Page();
//				break;
//			case MENU_ALARM:
//				//Pageflag = Menu_alarm_Page();
//				break;
//			case MENU_LIGHT:
//				Pageflag = Menu_light_Page();
//				break;
//			case MENU_GAME:
//				Pageflag = Menu_game_Page();
//				break;
//			case MENU_GAME_END:
//				Pageflag = Dino_end_Page();
//				break;
//			case MENU_GAME_BEGIN:
//				Pageflag = Dino_begin_Page();
//				break;
//			
//			default:
//				break;

//		}
//	}
//}

//测试块
//OLED_Clear();
//OLED_Update();
//Delay_s(1);


uint8_t Pageflag = MENU_GAME_BEGIN;
uint8_t Previous_Pageflag;

int main(void)
{
	MyRTC_Init();
	OLED_Init();
	Key_Init();
	LED_Init();
	Timer_Init();
	Scheduler_Init();
	//MPU6050_Init();
	
	//srand((unsigned int)RTC_GetCounter());
	srand(555555555);
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
				Pageflag = Menu_game_Page();
				break;
			case MENU_GAME_END:
				Pageflag = Dino_end_Page();
				break;
			case MENU_GAME_BEGIN:
				Pageflag = Dino_begin_Page();
				break;
			
			default:
				break;

		}
	}
}


