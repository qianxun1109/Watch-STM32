#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Key.h"
#include "Scheduler.h"
#include "menu.h"
#include "OLED_Data.h"
#include "Delay.h"
#include "LED.h"
#include "Menu_game.h"

extern uint8_t Pageflag;
extern uint8_t Previous_Pageflag;

extern uint8_t menuflag;
extern uint8_t menuflag_max;
extern const uint8_t* const icon_pointers[];
/*--------------------------秒表界面--------------------------------*/
volatile int MyTimer_time[3]={0,0,0};//秒表时间(时，分，秒)
uint8_t timerflag = 1;

void Show_Menu_timer_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyTimer_time[0],MyTimer_time[1],MyTimer_time[2]);
	OLED_ShowString(0,48,"开始",OLED_8X16);
	OLED_ShowString(48,48,"暂停",OLED_8X16);
	OLED_ShowString(96,48,"重置",OLED_8X16);
}

void Show_Menu_timer_movingUI(int moveflag)//+号向下移动（进入）；-号向上移动（退出）
{
	uint8_t location = 0;
	if(moveflag > 0)
	{
		while(location < 64+16)//总向下移动距离为64+16，其中64为基本翻页距离，16为额外间距
		{
			OLED_Clear();
			if(location < 48)//图标向上移出
			{
				OLED_ShowImage(48,16-location,32,32,icon_pointers[menuflag-1]);//中间
				if(menuflag > 1)OLED_ShowImage(0,16-location,32,32,icon_pointers[menuflag-1-1]);//左
				if(menuflag < menuflag_max)OLED_ShowImage(96,16-location,32,32,icon_pointers[menuflag-1+1]);//右
			}
			if(location > 16)//内容向上移入
			{			/*移植时修改为相关的UI，固定偏移+64+16-location*/
				OLED_ShowImage(0,0+64+16-location,16,16,Return);
				OLED_Printf(16,16+64+16-location,OLED_12X24,"%02d:%02d:%02d",MyTimer_time[0],MyTimer_time[1],MyTimer_time[2]);
				OLED_ShowString(0,48+64+16-location,"开始",OLED_8X16);
				OLED_ShowString(48,48+64+16-location,"暂停",OLED_8X16);
				OLED_ShowString(96,48+64+16-location,"重置",OLED_8X16);
			}
			OLED_Update();
			location+=2;
			if(Menu_speed >= 2)location+=2;
			if(Menu_speed >= 3)location+=2;
		}
	}
	else if(moveflag < 0)
	{
		while(location < 64+16)//总向上移动距离为64+16，其中64为基本翻页距离，16为额外间距
		{
			OLED_Clear();
			if(location > 16)
			{
				OLED_ShowImage(48,16+location-64-16,32,32,icon_pointers[menuflag-1]);//中间
				if(menuflag > 1)OLED_ShowImage(0,16+location-64-16,32,32,icon_pointers[menuflag-1-1]);//左
				if(menuflag < menuflag_max)OLED_ShowImage(96,16+location-64-16,32,32,icon_pointers[menuflag-1+1]);//右
			}
			if(location < 64)
			{				/*移植时修改为相关的UI，固定偏移+location*/
				OLED_ShowImage(0,0+location,16,16,Return);
				OLED_Printf(16,16+location,OLED_12X24,"%02d:%02d:%02d",MyTimer_time[0],MyTimer_time[1],MyTimer_time[2]);
				OLED_ShowString(0,48+location,"开始",OLED_8X16);
				OLED_ShowString(48,48+location,"暂停",OLED_8X16);
				OLED_ShowString(96,48+location,"重置",OLED_8X16);
			}
			OLED_Update();
			//移速控制
			location+=2;
			if(Menu_speed >= 2)location+=2;
			if(Menu_speed >= 3)location+=2;
		}
	}
}
void Menu_timer_blink(void)
{
	OLED_Clear();
	Show_Menu_timer_UI();
	OLED_Update();
	Delay_ms(20);
}
void Menu_timer_start(void)
{
	Menu_timer_blink();//闪烁一下
	TIM_Cmd(TIM2,ENABLE);//计时
}
void Menu_timer_stop(void)
{
	Menu_timer_blink();
	TIM_Cmd(TIM2,DISABLE);
}
void Menu_timer_reset(void)
{
	Menu_timer_blink();
	TIM_Cmd(TIM2,DISABLE);
	MyTimer_time[0]=0;
	MyTimer_time[1]=0;
	MyTimer_time[2]=0;
}

Page Menu_timer_Page(void)
{
	Show_Menu_timer_movingUI(1);//滑动进入
	
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			timerflag--;
			if(timerflag < 1)timerflag = 4;
		}
		else if(num == 2)
		{
			timerflag++;
			if(timerflag >4)timerflag = 1;
		}
		else if(num == 3)
		{		
			if(timerflag == 1)
			{
				Show_Menu_timer_movingUI(-1);
				return MENU;
			}
			else if(timerflag == 2)Menu_timer_start();
			else if(timerflag == 3)Menu_timer_stop();
			else if(timerflag == 4)Menu_timer_reset();
		}
		OLED_Clear();
		Show_Menu_timer_UI();
		switch(timerflag)
		{
			case 1://返回键
				OLED_ReverseArea(0,0,16,16);
				break;
			case 2://开始
				OLED_ReverseArea(0,48,16*2,16);
				break;
			case 3://暂停
				OLED_ReverseArea(48,48,16*2,16);
				break;
			case 4://重置
				OLED_ReverseArea(96,48,16*2,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}

/*------------------灯光界面-----------------------*/
uint8_t lightflag = 1;

void Show_Menu_light_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(19,28,"ON",OLED_12X24);
	OLED_ShowString(73,28,"OFF",OLED_12X24);
}
void Show_Menu_light_movingUI(int moveflag)//+号向下移动（进入）；-号向上移动（退出）
{
	uint8_t location = 0;
	if(moveflag > 0)
	{
		while(location < 64+16)//总向下移动距离为64+16，其中64为基本翻页距离，16为额外间距
		{
			OLED_Clear();
			if(location < 48)
			{
				OLED_ShowImage(48,16-location,32,32,icon_pointers[menuflag-1]);//中间
				if(menuflag > 1)OLED_ShowImage(0,16-location,32,32,icon_pointers[menuflag-1-1]);//左
				if(menuflag < menuflag_max)OLED_ShowImage(96,16-location,32,32,icon_pointers[menuflag-1+1]);//右
			}
			if(location > 16)
			{
				OLED_ShowImage(0,0+64+16-location,16,16,Return);
				OLED_ShowString(19,28+64+16-location,"ON",OLED_12X24);
				OLED_ShowString(73,28+64+16-location,"OFF",OLED_12X24);
			}
			OLED_Update();
			location+=2;
			if(Menu_speed >= 2)location+=2;
			if(Menu_speed >= 3)location+=2;
		}
	}
	else if(moveflag < 0)
	{
		while(location < 64+16)//总向上移动距离为64+16，其中64为基本翻页距离，16为额外间距
		{
			OLED_Clear();
			if(location > 16)
			{
				OLED_ShowImage(48,16+location-64-16,32,32,icon_pointers[menuflag-1]);//中间
				if(menuflag > 1)OLED_ShowImage(0,16+location-64-16,32,32,icon_pointers[menuflag-1-1]);//左
				if(menuflag < menuflag_max)OLED_ShowImage(96,16+location-64-16,32,32,icon_pointers[menuflag-1+1]);//右
			}
			if(location < 64)
			{
				OLED_ShowImage(0,0+location,16,16,Return);
				OLED_ShowString(19,28+location,"ON",OLED_12X24);
				OLED_ShowString(73,28+location,"OFF",OLED_12X24);
			}
			OLED_Update();
			location+=2;
			if(Menu_speed >= 2)location+=2;
			if(Menu_speed >= 3)location+=2;
		}
	}
}

void Menu_light_blink(void)
{
	OLED_Clear();
	Show_Menu_light_UI();
	OLED_Update();
	Delay_ms(20);
}

Page Menu_light_Page(void)
{
	Show_Menu_light_movingUI(1);//滑动进入
	
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			lightflag--;
			if(lightflag < 1)lightflag = 3;
		}
		else if(num == 2)
		{
			lightflag++;
			if(lightflag >3)lightflag = 1;
		}
		else if(num == 3)
		{		
			if(lightflag == 1)
			{
				Show_Menu_light_movingUI(-1);
				return MENU;
			}
			else if(lightflag == 2)
			{
				Menu_light_blink();
				//LED1_ON();
				LED2_ON();
			}
			else if(lightflag == 3)
			{
				Menu_light_blink();
				//LED1_OFF();
				LED2_OFF();
			}
		}
		OLED_Clear();
		Show_Menu_light_UI();
		switch(lightflag)
		{
			case 1://返回键
				OLED_ReverseArea(0,0,16,16);
				break;
			case 2://ON
				OLED_ReverseArea(19,28,12*2,24);
				break;
			case 3://OFF
				OLED_ReverseArea(73,28,12*3,28);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}

/*--------------------游戏界面--------------------------*/
uint8_t gameflag = 1;

void Show_Menu_game_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"Chrome Dino Run",OLED_8X16);
}
void Show_Menu_game_movingUI(int moveflag)//+号向下移动（进入）；-号向上移动（退出）
{
	uint8_t location = 0;
	if(moveflag > 0)
	{
		while(location < 64+16)//总向下移动距离为64+16，其中64为基本翻页距离，16为额外间距
		{
			OLED_Clear();
			if(location < 48)
			{
				OLED_ShowImage(48,16-location,32,32,icon_pointers[menuflag-1]);//中间
				if(menuflag > 1)OLED_ShowImage(0,16-location,32,32,icon_pointers[menuflag-1-1]);//左
				if(menuflag < menuflag_max)OLED_ShowImage(96,16-location,32,32,icon_pointers[menuflag-1+1]);//右
			}
			if(location > 16)
			{
				OLED_ShowImage(0,0+64+16-location,16,16,Return);
				OLED_ShowString(0,16+64+16-location,"Chrome Dino Run",OLED_8X16);
			}
			OLED_Update();
			location+=2;
			if(Menu_speed >= 2)location+=2;
			if(Menu_speed >= 3)location+=2;
		}
	}
	else if(moveflag < 0)
	{
		while(location < 64+16)//总向上移动距离为64+16，其中64为基本翻页距离，16为额外间距
		{
			OLED_Clear();
			if(location > 16)
			{
				OLED_ShowImage(48,16+location-64-16,32,32,icon_pointers[menuflag-1]);//中间
				if(menuflag > 1)OLED_ShowImage(0,16+location-64-16,32,32,icon_pointers[menuflag-1-1]);//左
				if(menuflag < menuflag_max)OLED_ShowImage(96,16+location-64-16,32,32,icon_pointers[menuflag-1+1]);//右
			}
			if(location < 64)
			{
				OLED_ShowImage(0,0+location,16,16,Return);
				OLED_ShowString(0,16+location,"Chrome Dino Run",OLED_8X16);
			}
			OLED_Update();
			location+=2;
			if(Menu_speed >= 2)location+=2;
			if(Menu_speed >= 3)location+=2;
		}
	}
}
Page Menu_game_Page(void)
{
	if(Previous_Pageflag == MENU)Show_Menu_game_movingUI(1);//从菜单跳转时滑动进入
	
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			gameflag--;
			if(gameflag < 1)gameflag = 2;
		}
		else if(num == 2)
		{
			gameflag++;
			if(gameflag >2)gameflag = 1;
		}
		else if(num == 3)
		{		
			if(gameflag == 1)
			{
				Show_Menu_game_movingUI(-1);
				return MENU;
			}
			else if(gameflag == 2)return MENU_GAME_DINO_BEGIN;
		}
		OLED_Clear();
		Show_Menu_game_UI();
		switch(gameflag)
		{
			case 1://返回键
				OLED_ReverseArea(0,0,16,16);
				break;
			case 2://Chrome Dino Run
				OLED_ReverseArea(0,16,8*15,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
	
}

/*---------------------------闹钟界面--------------------------------*/
uint8_t alarmflag = 1;

void Show_Menu_alarm_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"自定义",OLED_8X16);
	OLED_ShowString(0,32,"倒计时",OLED_8X16);
}
void Show_Menu_alarm_movingUI(int moveflag)//+号向下移动（进入）；-号向上移动（退出）
{
	uint8_t location = 0;
	if(moveflag > 0)
	{
		while(location < 64+16)//总向下移动距离为64+16，其中64为基本翻页距离，16为额外间距
		{
			OLED_Clear();
			if(location < 48)//图标向上移出
			{
				OLED_ShowImage(48,16-location,32,32,icon_pointers[menuflag-1]);//中间
				if(menuflag > 1)OLED_ShowImage(0,16-location,32,32,icon_pointers[menuflag-1-1]);//左
				if(menuflag < menuflag_max)OLED_ShowImage(96,16-location,32,32,icon_pointers[menuflag-1+1]);//右
			}
			if(location > 16)//内容向上移入
			{
				OLED_ShowImage(0,0+64+16-location,16,16,Return);
				OLED_ShowString(0,16+64+16-location,"自定义",OLED_8X16);
				OLED_ShowString(0,32+64+16-location,"倒计时",OLED_8X16);
			}
			OLED_Update();
			location+=2;
			if(Menu_speed >= 2)location+=2;
			if(Menu_speed >= 3)location+=2;
		}
	}
	else if(moveflag < 0)
	{
		while(location < 64+16)//总向上移动距离为64+16，其中64为基本翻页距离，16为额外间距
		{
			OLED_Clear();
			if(location > 16)
			{
				OLED_ShowImage(48,16+location-64-16,32,32,icon_pointers[menuflag-1]);//中间
				if(menuflag > 1)OLED_ShowImage(0,16+location-64-16,32,32,icon_pointers[menuflag-1-1]);//左
				if(menuflag < menuflag_max)OLED_ShowImage(96,16+location-64-16,32,32,icon_pointers[menuflag-1+1]);//右
			}
			if(location < 64)
			{
				OLED_ShowImage(0,0+location,16,16,Return);
				OLED_ShowString(0,16+location,"自定义",OLED_8X16);
				OLED_ShowString(0,32+location,"倒计时",OLED_8X16);
			}
			OLED_Update();
			location+=2;
			if(Menu_speed >= 2)location+=2;
			if(Menu_speed >= 3)location+=2;
		}
	}
}
Page Menu_alarm_Page(void)
{
	if(Previous_Pageflag == MENU)Show_Menu_alarm_movingUI(1);//从菜单跳转时滑动进入
	
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			alarmflag--;
			if(alarmflag < 1)alarmflag = 3;
		}
		else if(num == 2)
		{
			alarmflag++;
			if(alarmflag >3)alarmflag = 1;
		}
		else if(num == 3)
		{		
			if(alarmflag == 1)
			{
				Show_Menu_alarm_movingUI(-1);
				return MENU;
			}
			else if(alarmflag == 2)return MENU_ALARM_CUSTOMIZE;
			else if(alarmflag == 3)return MENU_ALARM_COUNTDOWN;
		}
		OLED_Clear();
		Show_Menu_alarm_UI();
		switch(alarmflag)
		{
			case 1://返回键
				OLED_ReverseArea(0,0,16,16);
				break;
			case 2://自定义
				OLED_ReverseArea(0,16,16*3,16);
				break;
			case 3://倒计时
				OLED_ReverseArea(0,32,16*3,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}



