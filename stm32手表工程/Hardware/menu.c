#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Key.h"
#include "Scheduler.h"
#include "Setting_time.h"

uint8_t clkflag=1;
uint8_t settingflag=1;



/*-------------------首页时钟界面--------------------*/
void Show_Clock_UI(void)//首页时钟UI
{
	MyRTC_Readtime();//将此刻RTCcounter里的时间戳传递到MyRTC_Time数组里
	//图像绘制
	OLED_Printf(0,0,OLED_6X8,"%d-%d-%d",MyRTC_Time[0],MyRTC_Time[1],MyRTC_Time[2]);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyRTC_Time[3],MyRTC_Time[4],MyRTC_Time[5]);
	OLED_ShowString(0,48,"菜单",OLED_8X16);
	OLED_ShowString(96,48,"设置",OLED_8X16);
}

uint8_t CLK_Page(void)//首页时钟页切换逻辑
{
	while(1)
	{
		//键码变换逻辑
		uint8_t num=Key_GetNum();//取值后会自动将key_num清零，不会重复
		if(num == 1)
		{
			clkflag--;
			if(clkflag < 1)clkflag = 3;//flag值往复变换12312，32132
		}
		else if(num == 2)
		{
			clkflag++;
			if(clkflag >3)clkflag = 1;
		}
		else if(num == 3)//确定键
		{
			return clkflag;	//跳出循环,切换页面函数
		}
		switch(clkflag)//一级菜单内的界面变换
		{
			case 1:
				OLED_Clear();
				Show_Clock_UI();
				OLED_Update();
				break;
			case 2:
				OLED_Clear();//清除上次图像
				Show_Clock_UI();//准备UI背景
				OLED_ReverseArea(0,48,32,16);//填充光标
				OLED_Update();//呈现最终图像
				break;
			case 3:
				OLED_Clear();
				Show_Clock_UI();
				OLED_ReverseArea(96,47,32,17);
				OLED_Update();
				break;
			default:
				break;
		}
	}
}
/*---------------------设置界面--------------------------*/
void Show_Setting_UI(void)
{
	OLED_Clear();
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"日期时间设置",OLED_8X16);
}

uint8_t Setting_Page(void)
{
	//清空之前的界面，改换当下UI
	OLED_Clear();
	Show_Setting_UI();
	OLED_Update();
	while(1)
	{
		uint8_t num=Key_GetNum();//键码变换,得到最新时刻键码
		if(num == 1)
		{
			settingflag--;
			if(settingflag < 1)settingflag = 3;
		}
		else if(num == 2)
		{
			settingflag++;
			if(settingflag >3)settingflag = 1;
		}
		else if(num == 3)
		{
			OLED_Clear();		//清屏，为新页面显示准备
			//OLED_Update();		
			if(settingflag == 2)return 1;//跳出循环,切换页面函数
			else if(settingflag == 3)return 4;
			
		}
		switch(settingflag)//一级菜单内的界面变换
		{
			case 1://无光标
				OLED_Clear();//涉及界面切换要先清屏
				Show_Setting_UI();
				OLED_Update();
				break;
			case 2://返回键
				Show_Setting_UI();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			case 3://日期时间设置选项
				Show_Setting_UI();
				OLED_ReverseArea(0,16,16*6,16);
				OLED_Update();
				break;
			
			default:
				break;
		}
	}
}
/*---------------------菜单界面-------------------------------*/
void Show_Menu_UI(void)
{
	
}

void Show_pageflag_proc(void)
{
	OLED_ShowNum(64,48,clkflag,1,OLED_6X8);
}

