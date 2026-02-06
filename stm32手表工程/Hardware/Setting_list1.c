#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Key.h"
#include "Scheduler.h"
#include "menu.h"

extern uint8_t Pageflag;
extern uint8_t Previous_Pageflag;
/*-----------------------------滑动速度设置--------------------------------*/
extern uint8_t Menu_speed;

uint8_t setting_movespeedflag = 1;

void Setting_movespeed_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"Speed:",OLED_8X16);
	OLED_Printf(100,16,OLED_8X16,"%1d",Menu_speed);
}
uint8_t SetMovespeed(void)
{
	OLED_Clear();
	while(1)
	{
		Setting_movespeed_UI();
		OLED_ReverseArea(100,16,8,16);
		OLED_Update();
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			Menu_speed--;
			if(Menu_speed < 1)Menu_speed = 3;
		}
		else if(num == 2)
		{
			Menu_speed++;
			if(Menu_speed > 3)Menu_speed = 1;
		}
		else if(num == 3)
		{
			BKP_WriteBackupRegister(BKP_DR3,Menu_speed);//掉电不丢失
			return 1;
		}
	}
}
Page Setting_movespeed_Page(void)
{
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			setting_movespeedflag--;
			if(setting_movespeedflag < 1)setting_movespeedflag = 2;
		}
		else if(num == 2)
		{
			setting_movespeedflag++;
			if(setting_movespeedflag >2)setting_movespeedflag = 1;
		}
		else if(num == 3)
		{
			if(setting_movespeedflag == 1)return SETTING;
			else if(setting_movespeedflag == 2)SetMovespeed();
		}
		OLED_Clear();
		switch(setting_movespeedflag)
		{
			case 1://返回键
				Setting_movespeed_UI();
				OLED_ReverseArea(0,0,16,16);
				break;
			case 2://Speed
				Setting_movespeed_UI();
				OLED_ReverseArea(0,16,8*6,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}

