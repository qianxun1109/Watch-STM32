#include "stm32f10x.h"                  // Device header

extern int MyTimer_time[3];

void Timer_Init()
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

void TIM2_IRQHandler()
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		MyTimer_time_up();
		
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
