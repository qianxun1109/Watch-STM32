#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Key.h"
#include "Scheduler.h"
#include "menu.h"

uint8_t timeflag = 1;

//更改RTC内部时间戳
//sign:按键键码 object:更改目标对象
void change_RTCtime(uint8_t sign,uint8_t object)
{
//注：对数组的加减只是用于显示的值，数组的值根本来源于RTCcounter，要多一步利用数组的值重调RTC计数器
	MyRTC_Readtime();//载入最新的时间
	if(sign == 1)MyRTC_Time[object]++;
	else if(sign == 2)MyRTC_Time[object]--;
	MyRTC_SetTime();//调整RTCcounter的值
}
//无需关注进位等问题，localtime函数内部会解决

void Show_Setting_Time_UI1(void)//第一页
{
	MyRTC_Readtime();
	
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"年:",OLED_8X16);
	OLED_ShowString(0,32,"月:",OLED_8X16);
	OLED_ShowString(0,48,"日:",OLED_8X16);
	
	OLED_Printf(32,16,OLED_8X16,"%4d",MyRTC_Time[0]);
	OLED_Printf(32,32,OLED_8X16,"%2d",MyRTC_Time[1]);
	OLED_Printf(32,48,OLED_8X16,"%2d",MyRTC_Time[2]);
}

void Show_Setting_Time_UI2(void)//第二页
{
	MyRTC_Readtime();
	
	OLED_ShowString(0,0,"时:",OLED_8X16);
	OLED_ShowString(0,16,"分:",OLED_8X16);
	OLED_ShowString(0,32,"秒:",OLED_8X16);
	
	OLED_Printf(32,0,OLED_8X16,"%2d",MyRTC_Time[3]);
	OLED_Printf(32,16,OLED_8X16,"%2d",MyRTC_Time[4]);
	OLED_Printf(32,32,OLED_8X16,"%2d",MyRTC_Time[5]);
}

uint8_t SetTime(uint8_t flag)
{
	if(flag == 3)//年设置
	{
		while(1)
		{
			Show_Setting_Time_UI1();
			OLED_ReverseArea(32,16,8*4,16);//年含4位数
			OLED_Update();
			uint8_t num=Key_GetNum();//键码变换
			if(num == 1)change_RTCtime(1,0);
			else if(num == 2)change_RTCtime(2,0);
			else if(num == 3)return 3;//更改成功
		}
	}
	else if(flag == 4)//月
	{
		while(1)
		{
			Show_Setting_Time_UI1();
			OLED_ReverseArea(32,32,8*2,16);
			OLED_Update();
			uint8_t num=Key_GetNum();
			if(num == 1)change_RTCtime(1,1);
			else if(num == 2)change_RTCtime(2,1);
			else if(num == 3)return 4;
		}
	}
	else if(flag == 5)//日
	{
		while(1)
		{
			Show_Setting_Time_UI1();
			OLED_ReverseArea(32,48,8*2,16);
			OLED_Update();
			uint8_t num=Key_GetNum();
			if(num == 1)change_RTCtime(1,2);
			else if(num == 2)change_RTCtime(2,2);
			else if(num == 3)return 5;
		}
	}
	else if(flag == 6)//时
	{
		while(1)
		{
			Show_Setting_Time_UI2();
			OLED_ReverseArea(32,0,8*2,16);
			OLED_Update();
			uint8_t num=Key_GetNum();
			if(num == 1)change_RTCtime(1,3);
			else if(num == 2)change_RTCtime(2,3);
			else if(num == 3)return 6;
		}
	}
	else if(flag == 7)//分
	{
		while(1)
		{
			Show_Setting_Time_UI2();
			OLED_ReverseArea(32,16,8*2,16);
			OLED_Update();
			uint8_t num=Key_GetNum();
			if(num == 1)change_RTCtime(1,4);
			else if(num == 2)change_RTCtime(2,4);
			else if(num == 3)return 7;
		}
	}
	else if(flag == 8)//秒
	{
		while(1)
		{
			Show_Setting_Time_UI2();
			OLED_ReverseArea(32,32,8*2,16);
			OLED_Update();
			uint8_t num=Key_GetNum();
			if(num == 1)change_RTCtime(1,5);
			else if(num == 2)change_RTCtime(2,5);
			else if(num == 3)return 8;
		}
	}
	return 0;
}

Page Setting_time_Page(void)
{
	OLED_Clear();//涉及界面切换要先清屏
	Show_Setting_Time_UI1();
	OLED_Update();
	while(1)
	{
		uint8_t num=Key_GetNum();//键码变换,得到最新时刻键码
		if(num == 1)
		{
			timeflag--;
			if(timeflag < 1)timeflag = 8;
		}
		else if(num == 2)
		{
			timeflag++;
			if(timeflag >8)timeflag = 1;
		}
		else if(num == 3)
		{
			if(timeflag == 2)return SETTING;//跳出循环,切换页面(3是SETTING的枚举值)
			else if(timeflag >= 3 && timeflag <= 8)SetTime(timeflag);
		}
		switch(timeflag)
		{
			case 1://无光标
				OLED_Clear();
				Show_Setting_Time_UI1();
				OLED_Update();
				break;
			case 2://返回键
				Show_Setting_Time_UI1();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			case 3://年
				Show_Setting_Time_UI1();
				OLED_ReverseArea(0,16,16,16);
				OLED_Update();
				break;
			case 4://月
				Show_Setting_Time_UI1();
				OLED_ReverseArea(0,32,16,16);
				OLED_Update();
				break;
			case 5://日
				OLED_Clear();
				Show_Setting_Time_UI1();
				OLED_ReverseArea(0,48,16,16);
				OLED_Update();
				break;
			case 6://时
				OLED_Clear();
				Show_Setting_Time_UI2();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			case 7://分
				Show_Setting_Time_UI2();
				OLED_ReverseArea(0,16,16,16);
				OLED_Update();
				break;
			case 8://秒
				OLED_Clear();
				Show_Setting_Time_UI2();
				OLED_ReverseArea(0,32,16,16);
				OLED_Update();
				break;
			default:
				break;
		}
	}
}


