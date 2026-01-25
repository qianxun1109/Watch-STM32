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

uint8_t game_endflag = 2;

void Game_Run(void)
{

	//...
	
	
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

uint8_t game_beginflag = 1;
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



