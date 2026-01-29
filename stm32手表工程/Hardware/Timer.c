#include "stm32f10x.h"                  // Device header
#include "menu.h"
#include "Menu_alarm.h"

extern void Alarm_ON(void);
extern volatile int MyTimer_time[3];
extern volatile int MyAlarm_countdowntime[3];
extern Page Pageflag;
extern volatile uint8_t alarm_set_flag;//闹钟是否设置的标志位
extern volatile uint8_t alarm_set_flag2;

volatile uint8_t MyAlarm_countdowntime_sign = 0;//闹钟响的标志

void Timer_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period=10000;		//1s触发中断
	TIM_TimeBaseInitStructure.TIM_Prescaler=7200;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	                                   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2,DISABLE);
}

void MyTimer_time_up(void)
{
	MyTimer_time[2]++;
	if(MyTimer_time[2] > 59)
	{
		MyTimer_time[1]++;
		MyTimer_time[2] = 0;
	}
	if(MyTimer_time[1] > 59)
	{
		MyTimer_time[0]++;
		MyTimer_time[1] = 0;
	}
}
void MyAlarm_ctrl_countdown(uint8_t flag)//闹钟响应逻辑//注：要放在scheduler的定时器里，TIM2在结束后会被关闭
{
	static uint8_t tick = 0;
	if(flag == 1)
	{
		if(tick < 6)
		{
			if(tick%2==0)Alarm_ON();
			else if(tick%2==1)Alarm_OFF();
			tick++;
		}
		else
		{	
			tick = 0;
			MyAlarm_countdowntime_sign = 0;//利用这个标志位切换执行状态可以避免static的弊端
		}       
	}
	else if(flag == 0)
	{
		tick = 0;
		Alarm_OFF();
	}
}

void MyAlarm_ctrl_customize(uint8_t flag)//闹钟响应逻辑//注：要放在scheduler的定时器里，TIM2在结束后会被关闭
{
	static uint8_t tick = 0;
	if(flag == 1)
	{
		if(tick < 6)
		{
			if(tick%2==0)Alarm_ON();
			else if(tick%2==1)Alarm_OFF();
			tick++;
		}
		else
		{	
			tick = 0;
			alarm_set_flag = 0;//闹钟结束
			alarm_set_flag2 = 0;//首页闹钟标志结束显示(放在闹钟响应完后再处理，避免和scheduler里的取消闹钟逻辑冲突)
		}       
	}
	else if(flag == 0)
	{
		tick = 0;
		Alarm_OFF();
	}
}

void MyAlarm_countdowntime_down(void)
{
	MyAlarm_countdowntime[2]--;
	if(MyAlarm_countdowntime[2] < 0)
	{
		MyAlarm_countdowntime[1]--;
		MyAlarm_countdowntime[2] = 59;
	}
	if(MyAlarm_countdowntime[1] < 0)
	{
		MyAlarm_countdowntime[0]--;
		MyAlarm_countdowntime[1] = 59;
	}
	if(MyAlarm_countdowntime[0] == 0 && MyAlarm_countdowntime[1] == 0 && MyAlarm_countdowntime[2] == 0)
	{
		alarm_countdown_stop();
		MyAlarm_countdowntime_sign = 1;
	}
}

void TIM2_IRQHandler()
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		if(Pageflag == MENU_TIMER)MyTimer_time_up();
		if(Pageflag == MENU_ALARM_COUNTDOWN)MyAlarm_countdowntime_down();
		
		
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}




