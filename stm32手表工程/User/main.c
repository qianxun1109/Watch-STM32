#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
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
#include "Setting_list1.h"
#include "Menu_alarm.h"
#include "AD.h"
#include "PWM.h"

Page Pageflag = CLOCK;
Page Previous_Pageflag;

int main(void)
{
	MyRTC_Init();
	OLED_Init();
	Key_Init();
	LED_Init();
	Timer_Init();
	Scheduler_Init();
	Alarm_Init();
	AD_Init();
	//PWM_Init();
	
	alarm_set_flag2 = BKP_ReadBackupRegister(BKP_DR2);//上电重建闹钟标志
	srand(66666666);
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
				Pageflag = Menu_alarm_Page();
				break;
			case MENU_LIGHT:
				Pageflag = Menu_light_Page();
				break;
			case MENU_GAME:
				Pageflag = Menu_game_Page();
				break;
			case MENU_GAME_DINO_END:
				Pageflag = Dino_end_Page();
				break;
			case MENU_GAME_DINO_BEGIN:
				Pageflag = Dino_begin_Page();
				break;
			case SETTING_MOVESPEED:
				Pageflag = Setting_movespeed_Page();
				break;
			case MENU_GAME_DINO_SCORESLIST:
				Pageflag = Dino_scoreslist_Page();
				break;
			case MENU_ALARM_CUSTOMIZE:
				Pageflag = Menu_alarm_customize_Page();
				break;
			case MENU_ALARM_COUNTDOWN:
				Pageflag = Menu_alarm_countdown_Page();
				break;
			case MENU_ALARM_COUNTDOWN_SETTING:
				Pageflag = Menu_alarm_countdown_setting_Page();
				break;
			
			default:
				break;
		}
	}
}

//测试块
//OLED_Clear();
//OLED_Update();
//Delay_s(1);

//Page Pageflag = CLOCK;
//Page Previous_Pageflag;

//int main(void)
//{
//	MyRTC_Init();
//	OLED_Init();
//	Key_Init();
//	LED_Init();
//	Timer_Init();
//	Scheduler_Init();
//	Alarm_Init();
//	AD_Init();
//	//MPU6050_Init();
//	

//	while (1)
//	{
//		OLED_Printf(0,0,OLED_8X16,"%02d",AD_GetValue());
//	}
//}
