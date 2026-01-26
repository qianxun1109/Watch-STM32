#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Key.h"
#include "Scheduler.h"
#include "menu.h"
#include "OLED_Data.h"
#include "Delay.h"

extern uint8_t Pageflag;
extern uint8_t Previous_Pageflag;

uint8_t game_endflag = 2;
uint8_t game_beginflag = 1;
uint8_t game_runflag = 0;

void Game_start_UI(void)
{
	OLED_ShowImage(0,0,128,64,Chrome_Dino_run_start);
	OLED_ShowImage(52,24,16,16,Return);
	OLED_ShowImage(52,24+16,16,16,Begin);
}
void Game_fail_UI(void)
{
	OLED_ShowImage(0,0,128,64,Chrome_Dino_run_fail);
}

void Dino_runing_UI(void)
{
	static uint8_t count = 0;
	static uint8_t sign = 0;
	if(sign)
	{
		count++;
		OLED_ShowImage(0,43,19,21,Dinosaur1);
		if(count >= 3)
		{
			sign = 0;
			count = 0;
		}
	}
	else 
	{
		count++;
		OLED_ShowImage(0,43,19,21,Dinosaur2);
		if(count >= 3)
		{
			sign = 1;
			count = 0;
		}
	}
}
uint8_t Dino_jumping_UI(uint8_t flag)//1小跳 2大跳
{
	static uint8_t location = 0;
	static uint8_t sign = 0;
	if(sign == 0)
	{
		location++;
		OLED_ShowImage(0,43-location,19,21,Dinosaur_up);
	}
	else if(sign == 1)
	{
		location--;
		OLED_ShowImage(0,43-location,19,21,Dinosaur_up);
	}
	if(flag == 1)
	{
		if(location >= 25)sign = 1;//小跳25像素
	}
	else if(flag == 2)
	{
		if(location >= 39)sign = 1;//大跳39像素
	}
	if(location == 0)//一次跳跃结束，重置变量
	{
		sign = 0;
		return 0;
	}
	return 1;//跳跃未结束
}
void Dino_scene_UI(void)
{
	
	
}



void Game_Run(void)
{
	uint8_t jumpoverflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)game_runflag = 1;
		else if(num == 2)game_runflag = 2;
		else if(num == 3)break;//自尽
		OLED_Clear();
		switch(game_runflag)
		{
			case 0://奔跑
				Dino_runing_UI();
				break;
			case 1://小跳
				jumpoverflag = Dino_jumping_UI(1);
				if(jumpoverflag == 0)game_runflag = 0;//跳跃结束
				break;
			case 2://大跳
				jumpoverflag = Dino_jumping_UI(2);
				if(jumpoverflag == 0)game_runflag = 0;
				break;
			default:
				break;
		}
		OLED_Update();
	}
}

Page Game_end_Page(void)
{
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			game_endflag--;
			if(game_endflag < 1)game_endflag = 3;
		}
		else if(num == 2)
		{
			game_endflag++;
			if(game_endflag >3)game_endflag = 1;
		}
		else if(num == 3)
		{		
			if(game_endflag == 1)return MENU_GAME;
			//else if(game_endflag == 2)
			//else if(game_endflag == 3)
		}
		//OLED_Clear();
		Game_fail_UI();
		switch(game_endflag)
		{
			case 1://退出键
				OLED_ReverseArea(32,33,20,16);
				break;
			case 2://重新开始
				OLED_ReverseArea(32+25,33,20,16);
				break;
			case 3://历史记录
				OLED_ReverseArea(32+25+25,33,20,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}

Page Game_begin_Page(void)
{
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 1)
		{
			game_beginflag--;
			if(game_beginflag < 1)game_beginflag = 2;
		}
		else if(num == 2)
		{
			game_beginflag++;
			if(game_beginflag >2)game_beginflag = 1;
		}
		else if(num == 3)
		{		
			if(game_beginflag == 1)return MENU_GAME;
			else if(game_beginflag == 2)
			{
				Game_Run();
				return MENU_GAME_END;
			}
		}
		OLED_Clear();
		Game_start_UI();
		switch(game_beginflag)
		{
			case 1://退出键
				OLED_ReverseArea(52,24,16,16);
				break;
			case 2://开始
				OLED_ReverseArea(52,24+16,16,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}



