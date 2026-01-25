#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Key.h"
#include "Scheduler.h"
#include "menu.h"
#include "OLED_Data.h"
#include "Delay.h"

int MyTimer_time[3]={0,0,0};//秒表时间(时，分，秒)

extern uint8_t menuflag;
extern uint8_t menuflag_max;
extern const uint8_t* const icon_pointers[];
uint8_t timerflag = 1;

void Show_Menu_timer_UI(void)
{
	MyRTC_Readtime();
	OLED_ShowImage(0,0,16,16,Return);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyTimer_time[0],MyTimer_time[1],MyTimer_time[2]);
	OLED_ShowString(0,48,"开始",OLED_8X16);
	OLED_ShowString(48,48,"暂停",OLED_8X16);
	OLED_ShowString(96,48,"重置",OLED_8X16);
}

void Show_Menu_open_movingUI(int moveflag)//+号向下移动（进入）；-号向上移动（退出）
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
				OLED_Printf(16,16+64+16-location,OLED_12X24,"%02d:%02d:%02d",MyTimer_time[0],MyTimer_time[1],MyTimer_time[2]);
				OLED_ShowString(0,48+64+16-location,"开始",OLED_8X16);
				OLED_ShowString(48,48+64+16-location,"暂停",OLED_8X16);
				OLED_ShowString(96,48+64+16-location,"重置",OLED_8X16);
			}
			OLED_Update();
			location++;
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
				OLED_Printf(16,16+location,OLED_12X24,"%02d:%02d:%02d",MyTimer_time[0],MyTimer_time[1],MyTimer_time[2]);
				OLED_ShowString(0,48+location,"开始",OLED_8X16);
				OLED_ShowString(48,48+location,"暂停",OLED_8X16);
				OLED_ShowString(96,48+location,"重置",OLED_8X16);
			}
			OLED_Update();
			location++;
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
	Show_Menu_open_movingUI(1);//滑动进入
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
				Show_Menu_open_movingUI(-1);
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


	
	





