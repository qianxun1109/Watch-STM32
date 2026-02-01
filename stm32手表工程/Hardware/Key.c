#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"

uint8_t Key_Num;

//void Shutdown_Init(void)
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
// 
//	//PB12低电平，PB13高电平为开机
//	//PB12高电平，PB13低电平为开机
//	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
//	GPIO_SetBits(GPIOB,GPIO_Pin_13);
//}

void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	if (Key_Num)
	{
		Temp = Key_Num;
		Key_Num = 0;
		return Temp;
	}
	return 0;
}

uint8_t Key_GetState(void)
{
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
	{
		return 3;
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
	{
		return 2;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
	{
		return 1;
	}
	return 0;
}

void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t CurrState, PrevState;
	
	Count ++;
	if (Count >= 20)
	{
		Count = 0;
		
		PrevState = CurrState;
		CurrState = Key_GetState();
		
		if (CurrState != 0 && PrevState == 0)//检测按下瞬间
		{
			Key_Num = CurrState;
		}
//		if (CurrState == 0 && PrevState != 0)//检测松开瞬间
//		{
//			Key_Num = PrevState;
//		}
	}
}

//void Key_Shutdown_Tick(void)
//{

//	static uint16_t shutdown_tick = 0;
//	if(Key_GetState() == 3)shutdown_tick++;
//	else shutdown_tick = 0;
//	if(shutdown_tick >= 2000)
//	{
//		shutdown_tick = 0;
//		GPIO_ResetBits(GPIOB, GPIO_Pin_13);	
//		GPIO_SetBits(GPIOB,GPIO_Pin_12);	
//	}
//	
//}



