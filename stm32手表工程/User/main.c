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
	/*外设初始化*/
	MyRTC_Init();
	OLED_Init();
	Key_Init();
	LED_Init();
	Timer_Init();
	Scheduler_Init();
	Alarm_Init();
	AD_Init();
	//PWM_Init();
	/*备份内容重加载*/
	alarm_set_flag2 = BKP_ReadBackupRegister(BKP_DR2);//BKP_DR2已经在相应地方和flag2同步置位和清零
	Menu_speed = BKP_ReadBackupRegister(BKP_DR3);
	uint16_t temp_L = BKP_ReadBackupRegister(BKP_DR4);
	uint16_t temp_H = BKP_ReadBackupRegister(BKP_DR5);
	RTC_SetAlarm(((uint32_t)temp_H<<16) | (uint32_t)temp_L);
	/*闹钟失效判断*/
	if(Alarm_GetCounter() < RTC_GetCounter())alarm_set_flag2 = 0;//如果在掉电时错过了闹钟，则闹钟失效，顺带把alarm_set_flag2手动置零
	/*待机闹钟唤醒判断*/
	if(BKP_ReadBackupRegister(BKP_DR6) == 1)
	{
		BKP_WriteBackupRegister(BKP_DR6,0);
		RTC_ClearFlag(RTC_FLAG_ALR);
		RTC_WaitForLastTask();
		PWR_ClearFlag(PWR_FLAG_SB);// 清除待机标志位
		//闹钟响应
		alarm_set_flag2 = 1;
		alarm_set_flag = 1;
	}
	/*随机种子*/
	srand(RTC_GetCounter());
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

//测试
//OLED_Clear();
//OLED_Update();
//Delay_s(1);
