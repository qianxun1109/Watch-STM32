#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Key.h"
#include "Scheduler.h"
#include "Setting_time.h"
#include "menu.h"
#include "OLED_Data.h"
#include "AD.h"

extern uint8_t Pageflag;
extern uint8_t Previous_Pageflag;
extern uint8_t alarm_set_flag2;

//这里对锂电池2/3分压后采集，根据3.3v为ADC的正极参考电压计算
uint16_t ADValue;		
float Voltage;
float Voltage_max = 4.2 * 2/3;//锂电池的满电电压为4.2v
float Voltage_min = 3.3 * 2/3;//锂电池电压为3.3v时认为电量为0
/*-----------------------------------首页时钟界面---------------------------------------*/
uint8_t clock_sign = 0;//0调节模式 1简洁模式
uint8_t clkflag=1;

void Clock_tick(void)//用于定时切换简洁模式
{
	static uint16_t clock_tick = 0;
	if(clock_sign == 0 && Pageflag == CLOCK)clock_tick++;//只在保持调节模式并在时钟页面时累计计时
	else clock_tick = 0;
	if(clock_tick > 1000*10)//10s未操作进入简洁模式
	{
		clock_tick = 0;
		clock_sign = 1;
	}
}
void Show_weekday(void)
{
	switch(weekday)
	{
		case 0:
			OLED_Printf(64,0,OLED_6X8,"Sun");
			break;
		case 1:
			OLED_Printf(64,0,OLED_6X8,"Mon");
			break;
		case 2:
			OLED_Printf(64,0,OLED_6X8,"Tue");
			break;
		case 3:
			OLED_Printf(64,0,OLED_6X8,"Wed");
			break;
		case 4:
			OLED_Printf(64,0,OLED_6X8,"Thu");
			break;
		case 5:
			OLED_Printf(64,0,OLED_6X8,"Fri");
			break;
		case 6:
			OLED_Printf(64,0,OLED_6X8,"Sat");
			break;
		default :
			break;
	}
}
void Show_Clock_UI(void)//首页时钟UI
{
	float sum = 0.0f;
	MyRTC_Readtime();//将此刻RTCcounter里的时间戳传递到MyRTC_Time数组里
	//图像绘制
	if(clock_sign == 0)//调节模式
	{
		OLED_Printf(0,0,OLED_6X8,"%d-%d-%d",MyRTC_Time[0],MyRTC_Time[1],MyRTC_Time[2]);
		OLED_ShowString(0,48,"菜单",OLED_8X16);
		OLED_ShowString(96,48,"设置",OLED_8X16);
		Show_weekday();
		//电量显示控制
		for(uint8_t i = 0;i<10;i++)
		{
			ADValue = AD_GetValue();
			Voltage = (float)ADValue * 3.3 / 4095;
			sum += Voltage;
		}
		Voltage = sum / 10;
		OLED_ShowImage(128-16,0,16,10,Battery);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.9)OLED_ClearArea(123,2,1,6);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.8)OLED_ClearArea(122,2,1,6);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.7)OLED_ClearArea(121,2,1,6);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.6)OLED_ClearArea(120,2,1,6);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.5)OLED_ClearArea(119,2,1,6);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.4)OLED_ClearArea(118,2,1,6);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.3)OLED_ClearArea(117,2,1,6);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.2)OLED_ClearArea(116,2,1,6);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.1)OLED_ClearArea(115,2,1,6);
		if(Voltage <= Voltage_min + (Voltage_max - Voltage_min) * 0.02)OLED_ClearArea(114,2,1,6);
		
		//闹钟开启标志
		if(alarm_set_flag2 == 1)OLED_ShowImage(128-16*2,0,16,16,Clock);
	}
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyRTC_Time[3],MyRTC_Time[4],MyRTC_Time[5]);
}

Page CLK_Page(void)//首页时钟页切换逻辑
{
	Previous_Pageflag = Pageflag;//追踪上一次的Pageflag，便于后续一些情况的处理
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
			//跳出循环,切换页面
			if(clkflag == 1)
			{
				if(clock_sign == 0)clock_sign = 1;
				else if(clock_sign == 1)clock_sign = 0;
				return CLOCK;
			}
			else if(clkflag == 2)return MENU;
			else if(clkflag == 3)return SETTING;
		}
		OLED_Clear();//清除上次图像
		Show_Clock_UI();//准备UI背景
		if(clock_sign == 1)clkflag = 1;//简洁模式光标重置
		switch(clkflag)//光标填充
		{
			case 1:
				OLED_Update();
				break;
			case 2:
				OLED_ReverseArea(0,48,32,16);
				break;
			case 3:
				OLED_ReverseArea(96,47,32,17);
				break;
			default:
				break;
		}
		OLED_Update();//呈现最终图像
	}
}
/*------------------------------------设置界面--------------------------------------------*/

uint8_t settingflag=1;

void Show_Setting_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"日期时间设置",OLED_8X16);
	OLED_ShowString(0,32,"移动速度设置",OLED_8X16);
}

Page Setting_Page(void)
{
	Previous_Pageflag = Pageflag;
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
			if(settingflag == 1)return CLOCK;//跳出循环,切换页面函数
			else if(settingflag == 2)return SETTING_TIME;
			else if(settingflag == 3)return SETTING_MOVESPEED;
			
		}
		OLED_Clear();
		Show_Setting_UI();
		switch(settingflag)
		{
			case 1://返回键
				OLED_ReverseArea(0,0,16,16);
				break;
			case 2://日期时间设置选项
				OLED_ReverseArea(0,16,16*6,16);
				break;
			case 3://移动速度设置选项
				OLED_ReverseArea(0,32,16*6,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}
/*-------------------------------------菜单界面---------------------------------------------*/

uint8_t menuflag = 1;
uint8_t menuflag_max = 5;
uint8_t Menu_speed = 2;

const uint8_t* const icon_pointers[]={
	Return_icon,//0
	Timer_icon,//1
	Alarm_icon,//2
	Light_icon,//3
	Game_icon,//4
	
};

void Show_Menu_UI(void)
{
	OLED_ShowImage(48-2,16-2,32+4,32+4,Target_icon);//注：光标要第一个写入，防止覆盖后面的内容，位置固定在正中间
	OLED_ShowImage(48,16,32,32,icon_pointers[menuflag-1]);//数值向左偏移一位
	if(menuflag > 1)OLED_ShowImage(0,16,32,32,icon_pointers[menuflag-1-1]);//flag=1左边没内容就不显示了
	if(menuflag < menuflag_max)OLED_ShowImage(96,16,32,32,icon_pointers[menuflag-1+1]);
}
void Show_Menu_movingUI(int moveflag,uint8_t previous,uint8_t current)//位置示意图： 左--1--2--3--右
{
	//对齐数组序号
	previous -=1;
	current -=1;
	//移动指标
	int location = 0, target;
	if(moveflag < 0)
	{
		target = 48;	//目标向右移48像素（即向右划对应flag减小）
		while(location < target)
		{
			OLED_Clear();
			OLED_ShowImage(48+location,16,32,32,icon_pointers[previous]);//位置从2到3
			OLED_ShowImage(0+location,16,32,32,icon_pointers[current]);//位置从1到2
			if(previous < menuflag_max-1/*对齐*/ && 96+location < 128)OLED_ShowImage(96+location,16,32,32,icon_pointers[previous+1]);//位置从3向右移出，current=menuflag_max右边无图不用移
			if(previous > 1 && -16+location > 0)OLED_ShowImage(-48+location,16,32,32,icon_pointers[current-1]);//位置从左移入到1
			OLED_Update();
			location+=2;
			if(Menu_speed >= 2)location+=2;//速度调整的实现
			if(Menu_speed >= 3)location+=2;
		}
	}
	else if(moveflag > 0)
	{
		target = -48;	//目标向左移48像素（即向左划对应flag增大）
		while(location > target)
		{
			OLED_Clear();
			OLED_ShowImage(48+location,16,32,32,icon_pointers[previous]);//位置从2到1
			if(current > 1 && 0+location < 0)OLED_ShowImage(0+location,16,32,32,icon_pointers[previous-1]);//位置从1向左移出
			OLED_ShowImage(96+location,16,32,32,icon_pointers[current]);//位置从3到2
			if(current < menuflag_max-1 && 128+16+location < 128)OLED_ShowImage(128+16+location,16,32,32,icon_pointers[current+1]);//位置从右移入到3
			OLED_Update();
			location-=2;
			if(Menu_speed >= 2)location-=2;
			if(Menu_speed >= 3)location-=2;
		}
	}
}

Page Menu_Page(void)
{
	//检测flag变化
	int moveflag = 0;
	uint8_t previous, current = menuflag; 
	
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();

		if(num == 1)
		{
			menuflag--;
			if(menuflag < 1)menuflag = menuflag_max;
		}
		else if(num == 2)
		{
			menuflag++;
			if(menuflag >menuflag_max)menuflag = 1;
		}
		else if(num == 3)
		{	
			if(menuflag == 1)return CLOCK;
			else if(menuflag == 2)return MENU_TIMER;
			else if(menuflag == 3)return MENU_ALARM;
			else if(menuflag == 4)return MENU_LIGHT;
			else if(menuflag == 5)return MENU_GAME;
			
		}
		//移动检测与实现
		previous = current;
		current = menuflag;
		moveflag = current - previous;//-1往左，+1往右
		switch(menuflag)
		{
			case 1://返回键
				if(moveflag == -1)Show_Menu_movingUI(moveflag,previous,current);//case由2到1的变化,限制边界情况下moveflag的值，函数内部协调不来
				break;
			case 2://秒表
			case 3://闹钟
			case 4://手电筒
				if(moveflag != 0)Show_Menu_movingUI(moveflag,previous,current);
				break;
			case 5://游戏
				if(moveflag == 1)Show_Menu_movingUI(moveflag,previous,current);
				break;
			default:
				break;
		}
		OLED_Clear();
		Show_Menu_UI();
		OLED_Update();
	}
}








