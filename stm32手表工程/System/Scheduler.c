#include "stm32f10x.h"                  // Device header
#include "Key.h"
#include "menu.h"
#include "Menu_game.h"
#include "Timer.h"
#include "OLED.h"

extern uint8_t alarm_set_flag;
extern uint8_t alarm_set_flag2;
uint32_t scheduler_tick;
uint8_t task_num;

typedef struct task{
	void(*task)(void);
	uint8_t cycle;
	uint32_t last_run;
}task_t;

task_t tasks[]={0



};
/*长按确定键进入待机模式*/
void Standby_Tick(void)
{
	static uint16_t standby_tick;
	
	if(Key_GetState() == 3)standby_tick++;
	else standby_tick = 0;
	if(standby_tick > 2000)
	{
		standby_tick = 0;
		OLED_Clear();
		OLED_Update();
		BKP_WriteBackupRegister(BKP_DR6,1);//手动待机标志位，PWR_FLAG_SB有问题没搞明白
		PWR_EnterSTANDBYMode();
	}
}
/*倒计时闹钟响应*/
void MyAlarm_countdowntime_Tick(void)
{
	static uint16_t MyAlarm_countdowntime_tick = 500;
	if(MyAlarm_countdowntime_sign == 1)//满足闹钟条件才执行
	{
		MyAlarm_countdowntime_tick++;
		if(MyAlarm_countdowntime_tick >= 500)//500ms执行一次
		{
			MyAlarm_countdowntime_tick = 0;
			MyAlarm_ctrl_countdown(1);//里面执行完后会取消掉闹钟条件
		}
	}
	else MyAlarm_countdowntime_tick = 500;
}	
/*自定义闹钟响应*/
void MyAlarm_customizetime_Tick(void)
{
	static uint16_t MyAlarm_customizetime_tick = 500;
	if(alarm_set_flag2 == 0)alarm_set_flag = 0;//取消闹钟逻辑，将alarm_set_flag2置0，闹钟不在首页显示，与闹钟失效的功能可以重叠，如果alarm_set_flag在闹钟中断里置1，则可通过alarm_set_flag2忽视，同时闹钟的硬件标志位在闹钟中断里已经处理好了
	if(alarm_set_flag == 1)//满足闹钟条件才执行
	{
		MyAlarm_customizetime_tick++;
		if(MyAlarm_customizetime_tick >= 500)//500ms执行一次
		{
			MyAlarm_customizetime_tick = 0;
			MyAlarm_ctrl_customize(1);//里面执行完后会取消掉闹钟条件
		}
	}
	else MyAlarm_customizetime_tick = 500;
}

void Scheduler_Init(void)//Enable TIM1 to start the systick
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);			
	
	TIM_InternalClockConfig(TIM1);		
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;				
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;				
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);				
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);		
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);					
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	
	NVIC_InitTypeDef NVIC_InitStructure;						
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;//  < --TIM1特别	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);								
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);		//<<-----使用TIM1要多这行代码
	TIM_Cmd(TIM1, ENABLE);		

	task_num=sizeof(tasks)/sizeof(tasks[0]);
}
void TIM1_UP_IRQHandler(void)		//<<-----使用TIM1要注意中断名不一样
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)	//<<-----使用TIM1要注意中断标志位也不一样
	{
		scheduler_tick++;
		
		Key_Tick();//按键检测
		Clock_tick();//简洁模式定时
		Dino_mode_tick();//长按进入加速模式
		Standby_Tick();//长按进入待机模式
		//Key_Shutdown_Tick();//关机长按检测
		MyAlarm_countdowntime_Tick();//倒计时闹钟响应
		MyAlarm_customizetime_Tick();//自定义闹钟响应
		
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}

void Scheduler_run(void)
{
	uint8_t i;
	for(i=0;i<task_num;i++)
	{
		if(tasks[i].last_run+tasks[i].cycle < scheduler_tick)
		{
			tasks[i].last_run=scheduler_tick;
			tasks[i].task();
		}
	}
}

