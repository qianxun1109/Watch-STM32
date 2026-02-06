#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Key.h"
#include "Scheduler.h"
#include "menu.h"
#include "OLED_Data.h"
#include "Delay.h"
#include "LED.h"
#include "MyRTC.h"
#include "Timer.h"
#include "time.h"
#include "PWM.h"

extern uint8_t Pageflag;
extern uint8_t Previous_Pageflag;

int MyAlarm_time[3]={0};//闹钟时间(时，分，秒)
volatile int MyAlarm_countdowntime[3]={0};//倒计时时间
int MyAlarm_countdowntime_copy[3]={0};//用于倒计时重置回设置时间
//对于中断要修改的值，保证到内存地址读取，不被编译器优化
volatile uint8_t alarm_set_flag = 0;//闹钟是否设置的标志位
volatile uint8_t alarm_set_flag2 = 0;//用于控制首页闹钟标志显示与辅助闹钟开关的控制
uint8_t alarm_customizeflag = 1;//用于控制闹钟
uint8_t alarm_countdownflag = 1;
uint8_t alarm_countdown_settingflag = 1;

void MyAlarm_SetTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	//日期用MyRTC_Time
	MyRTC_Readtime();
	time_date.tm_year=MyRTC_Time[0] - 1900;
	time_date.tm_mon=MyRTC_Time[1] - 1;
	time_date.tm_mday=MyRTC_Time[2];
	//时间用MyAlarm_time
	time_date.tm_hour=MyAlarm_time[0];
	time_date.tm_min=MyAlarm_time[1];
	time_date.tm_sec=MyAlarm_time[2];
	time_date.tm_isdst = -1;
	
	time_cnt = mktime(&time_date) - 8*60*60;//转换成伦敦时间戳
	
	if(time_cnt == (time_t)-1)
	{
		OLED_Printf(0,0,OLED_12X24,"Fail!!!");
		OLED_Update();
		Delay_s(1);
	}
	uint32_t alarm_cnt = (uint32_t)time_cnt;//类型协调
	uint32_t temp = RTC_GetCounter();
	if(alarm_cnt <= temp)alarm_cnt += 60*60*24;//如果闹钟时间在此刻之前，则调到下一天
	
	RTC_SetAlarm(alarm_cnt);
	RTC_WaitForLastTask();
}
void Alarm_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//如果用pwm，这里要改
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	
	RTC_ITConfig(RTC_IT_ALR,ENABLE);
	RTC_WaitForLastTask();
	RTC_ClearFlag(RTC_FLAG_ALR);
	
	EXTI_InitTypeDef EXTI_Initstructure;
	EXTI_Initstructure.EXTI_Line=EXTI_Line17;
	EXTI_Initstructure.EXTI_LineCmd=ENABLE;
	EXTI_Initstructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_Initstructure.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_Initstructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=RTCAlarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStructure);
}
void RTCAlarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALR)==SET)
	{
		alarm_set_flag = 1;//闹钟开始
		RTC_ClearITPendingBit(RTC_IT_ALR);
		RTC_WaitForLastTask();
		RTC_ClearFlag(RTC_FLAG_ALR);
		RTC_WaitForLastTask();
		EXTI_ClearITPendingBit(EXTI_Line17);//必须把挂起位给清掉
	}
}
void Alarm_ON(void)
{
	//PWM_SetCompare2(50);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
}
void Alarm_OFF(void)
{
	//PWM_SetCompare2(0);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}
void Show_Menu_alarm_customize_UI(void)
{
	OLED_ShowImage(0,32-8,16,16,Cancel);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyAlarm_time[0],MyAlarm_time[1],MyAlarm_time[2]);
	OLED_ShowImage(128-16,32-8,16,16,OK);
}
void Show_Menu_alarm_countdown_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowImage(128-16,0,16,16,Set);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyAlarm_countdowntime[0],MyAlarm_countdowntime[1],MyAlarm_countdowntime[2]);
	OLED_ShowString(0,48,"开始",OLED_8X16);
	OLED_ShowString(48,48,"暂停",OLED_8X16);
	OLED_ShowString(96,48,"重置",OLED_8X16);
}
void Show_Menu_alarm_countdown_setting_UI(void)
{
	OLED_ShowImage(128-16,0,16,16,Return);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyAlarm_countdowntime[0],MyAlarm_countdowntime[1],MyAlarm_countdowntime[2]);
}

void alarm_customize_changetime(uint8_t sign,uint8_t flag)//1对应数字减小，中间数字下移	2对应数字增大，中间数字上移
{
	if(sign == 1)MyAlarm_time[flag]--;
	else if(sign == 2)MyAlarm_time[flag]++;
	//矫正
	if(MyAlarm_time[0] < 0)MyAlarm_time[0] = 23;
	if(MyAlarm_time[0] > 23)MyAlarm_time[0] = 0;
	if(MyAlarm_time[1] < 0)MyAlarm_time[1] = 59;
	if(MyAlarm_time[1] > 59)MyAlarm_time[1] = 0;
	if(MyAlarm_time[2] < 0)MyAlarm_time[2] = 59;
	if(MyAlarm_time[2] > 59)MyAlarm_time[2] = 0;
}
void alarm_countdown_changetime(uint8_t sign,uint8_t flag)//1对应数字减小，中间数字下移	2对应数字增大，中间数字上移
{
	if(sign == 1)MyAlarm_countdowntime[flag]--;
	else if(sign == 2)MyAlarm_countdowntime[flag]++;
	//矫正
	if(MyAlarm_countdowntime[0] < 0)MyAlarm_countdowntime[0] = 23;
	if(MyAlarm_countdowntime[0] > 23)MyAlarm_countdowntime[0] = 0;
	if(MyAlarm_countdowntime[1] < 0)MyAlarm_countdowntime[1] = 59;
	if(MyAlarm_countdowntime[1] > 59)MyAlarm_countdowntime[1] = 0;
	if(MyAlarm_countdowntime[2] < 0)MyAlarm_countdowntime[2] = 59;
	if(MyAlarm_countdowntime[2] > 59)MyAlarm_countdowntime[2] = 0;
}
uint8_t Setalarm_customize(uint8_t flag)
{
	OLED_Clear();
	if(flag == 2)//时设置
	{
		while(1)
		{
			Show_Menu_alarm_customize_UI();
			OLED_Update();
			uint8_t num=Key_GetNum();//键码变换
			if(num == 1)alarm_customize_changetime(1,0);
			else if(num == 2)alarm_customize_changetime(2,0);
			else if(num == 3)return 2;//更改成功
		}
	}
	else if(flag == 3)//分
	{
		while(1)
		{
			Show_Menu_alarm_customize_UI();
			OLED_Update();
			uint8_t num=Key_GetNum();
			if(num == 1)alarm_customize_changetime(1,1);
			else if(num == 2)alarm_customize_changetime(2,1);
			else if(num == 3)return 3;
		}
	}
	else if(flag == 4)//秒
	{
		while(1)
		{
			Show_Menu_alarm_customize_UI();
			OLED_Update();
			uint8_t num=Key_GetNum();
			if(num == 1)alarm_customize_changetime(1,2);
			else if(num == 2)alarm_customize_changetime(2,2);
			else if(num == 3)return 4;
		}
	}
	return 0;
}
uint8_t Setalarm_countdown(uint8_t flag)
{
	OLED_Clear();
	if(flag == 1)//时设置
	{
		while(1)
		{
			Show_Menu_alarm_countdown_setting_UI();
			OLED_Update();
			uint8_t num=Key_GetNum();//键码变换
			if(num == 1)alarm_countdown_changetime(1,0);
			else if(num == 2)alarm_countdown_changetime(2,0);
			else if(num == 3)return 1;//更改成功
		}
	}
	else if(flag == 2)//分
	{
		while(1)
		{
			Show_Menu_alarm_countdown_setting_UI();
			OLED_Update();
			uint8_t num=Key_GetNum();
			if(num == 1)alarm_countdown_changetime(1,1);
			else if(num == 2)alarm_countdown_changetime(2,1);
			else if(num == 3)return 2;
		}
	}
	else if(flag == 3)//秒
	{
		while(1)
		{
			Show_Menu_alarm_countdown_setting_UI();
			OLED_Update();
			uint8_t num=Key_GetNum();
			if(num == 1)alarm_countdown_changetime(1,2);
			else if(num == 2)alarm_countdown_changetime(2,2);
			else if(num == 3)return 3;
		}
	}
	return 0;
}
void alarm_blink(void)
{
	OLED_Clear();
	Show_Menu_alarm_countdown_UI();
	OLED_Update();
	Delay_ms(20);
}
void alarm_countdown_start(void)
{
	alarm_blink();
	TIM_Cmd(TIM2,ENABLE);
}
void alarm_countdown_stop(void)
{
	alarm_blink();
	TIM_Cmd(TIM2,DISABLE);
}
void alarm_countdown_reset(void)
{
	uint8_t flag = 0;
	for(uint8_t i=0;i<3;i++)if(MyAlarm_countdowntime[i] != MyAlarm_countdowntime_copy[i])flag = 1;//有不相等的值
	if(flag == 1)for(uint8_t i=0;i<3;i++)MyAlarm_countdowntime[i] = MyAlarm_countdowntime_copy[i];//复原
	if(flag == 0)for(uint8_t i=0;i<3;i++)MyAlarm_countdowntime[i] = 0;//清零
}
Page Menu_alarm_customize_Page(void)
{
	//闹钟时间初始化
	Alarm_Readtime();
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			alarm_customizeflag--;
			if(alarm_customizeflag < 1)alarm_customizeflag = 5;
		}
		else if(num == 2)
		{
			alarm_customizeflag++;
			if(alarm_customizeflag >5)alarm_customizeflag = 1;
		}
		else if(num == 3)
		{		
			if(alarm_customizeflag == 1)
			{
				alarm_set_flag2 = 0;//取消闹钟设置
				alarm_set_flag = 0;
				BKP_WriteBackupRegister(BKP_DR2,alarm_set_flag2);//掉电不丢失
				return MENU_ALARM;
			}
			else if(alarm_customizeflag >= 2 && alarm_customizeflag <= 4)Setalarm_customize(alarm_customizeflag);
			else if(alarm_customizeflag == 5)
			{
				alarm_set_flag2 = 1;
				MyAlarm_SetTime();
				BKP_WriteBackupRegister(BKP_DR2,alarm_set_flag2);//掉电不丢失，要和alarm_set_flag2同步更改
				return MENU_ALARM;
			}
		}
		OLED_Clear();
		Show_Menu_alarm_customize_UI();
		switch(alarm_customizeflag)
		{
			case 1://取消键
				OLED_ReverseArea(0,32-8,16,16);
				break;
			case 2://时
				OLED_ReverseArea(16,16,12*2,24);
				break;
			case 3://分
				OLED_ReverseArea(52,16,12*2,24);
				break;
			case 4://秒
				OLED_ReverseArea(88,16,12*2,24);
				break;
			case 5://确定
				OLED_ReverseArea(128-16,32-8,16,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}
Page Menu_alarm_countdown_Page(void)
{
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			alarm_countdownflag--;
			if(alarm_countdownflag < 1)alarm_countdownflag = 5;
		}
		else if(num == 2)
		{
			alarm_countdownflag++;
			if(alarm_countdownflag >5)alarm_countdownflag = 1;
		}
		else if(num == 3)
		{		
			if(alarm_countdownflag == 1)return MENU_ALARM;
			else if(alarm_countdownflag == 2)return MENU_ALARM_COUNTDOWN_SETTING;
			else if(alarm_countdownflag == 3 && (MyAlarm_countdowntime[0] != 0 || MyAlarm_countdowntime[1] != 0 || MyAlarm_countdowntime[2] != 0))alarm_countdown_start();//倒计时不为零才能开始
			else if(alarm_countdownflag == 4)alarm_countdown_stop();
			else if(alarm_countdownflag == 5)alarm_countdown_reset();
		}
		OLED_Clear();
		Show_Menu_alarm_countdown_UI();
		switch(alarm_countdownflag)
		{
			case 1://返回键
				OLED_ReverseArea(0,0,16,16);
				break;
			case 2://设置
				OLED_ReverseArea(128-16,0,16,16);
				break;
			case 3://开始
				OLED_ReverseArea(0,48,16*2,16);
				break;
			case 4://暂停
				OLED_ReverseArea(48,48,16*2,16);
				break;
			case 5://重置
				OLED_ReverseArea(96,48,16*2,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}
Page Menu_alarm_countdown_setting_Page(void)
{
	alarm_countdown_settingflag = 1;
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			alarm_countdown_settingflag--;
			if(alarm_countdown_settingflag < 1)alarm_countdown_settingflag = 4;
		}
		else if(num == 2)
		{
			alarm_countdown_settingflag++;
			if(alarm_countdown_settingflag >4)alarm_countdown_settingflag = 1;
		}
		else if(num == 3)
		{		
			if(alarm_countdown_settingflag == 4)
			{
				for(uint8_t i=0;i<3;i++)MyAlarm_countdowntime_copy[i] = MyAlarm_countdowntime[i];//设置退出时备份一份时间
				return MENU_ALARM_COUNTDOWN;
			}
			else if(alarm_countdown_settingflag >= 1 && alarm_countdown_settingflag <= 3)Setalarm_countdown(alarm_countdown_settingflag);
		}
		OLED_Clear();
		Show_Menu_alarm_countdown_setting_UI();
		switch(alarm_countdown_settingflag)
		{
			case 4://返回键
				OLED_ReverseArea(128-16,0,16,16);
				break;
			case 1://时
				OLED_ReverseArea(16,16,12*2,24);
				break;
			case 2://分
				OLED_ReverseArea(52,16,12*2,24);
				break;
			case 3://秒
				OLED_ReverseArea(88,16,12*2,24);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}
