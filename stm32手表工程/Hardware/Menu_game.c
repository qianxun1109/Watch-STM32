#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Key.h"
#include "Scheduler.h"
#include "menu.h"
#include "OLED_Data.h"
#include "Delay.h"
#include "stdlib.h"
#include "Menu_game.h"

extern uint8_t Pageflag;
extern uint8_t Previous_Pageflag;

uint8_t game_endflag = 2;
uint8_t game_beginflag = 1;
uint8_t Dino_Runflag = 0;

Dino_border borders[]={
{0,43,19,21},//小恐龙(B值变化)
{128,64-23,11,23},//Dino_cacti_1(A值变化)
{128,64-20,21,20},//Dino_cacti_2
{128,64-15,25,15},//Dino_cacti_3
{128,64-22,32,22},//Dino_cacti_4
};

void Dino_start_UI(void)
{
	OLED_ShowImage(0,0,128,64,Chrome_Dino_run_start);
	OLED_ShowImage(52,24,16,16,Return);
	OLED_ShowImage(52,24+16,16,16,Begin);
}
void Dino_end_UI(void)
{
	OLED_Printf(64-2-4*8,32-16-2,OLED_8X16,"GAME");
	OLED_Printf(64+2,32-16-2,OLED_8X16,"OVER");
	OLED_ShowImage(64-8,34,16,16,Dino_restart);
	OLED_ShowImage(64+8+4,34,16,16,Dino_rank);
	OLED_ShowImage(64-8-4-16,34,16,16,Return);
	OLED_ShowImage(0,borders[0].B,19,21,Dinosaur_dead);
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
uint8_t Dino_jumping_UI(uint8_t flag,uint8_t speed)//1小跳 2大跳
{
	static int location = 0;
	static uint8_t sign = 0;//sign=1为上升过程，0为下降过程
	//速度对跳跃过程的控制-------------------------------------
	if(sign == 0)
	{
		location++;
		if(flag == 2 && location <= 20)location++;//跳跃速度变化
		if(speed >= 3 && flag == 2 && location <= 30)location++;
		if(speed >= 3 && flag == 1 && location <= 16)location++;
		if(speed >= 4 && flag == 2 && location <= 35)location++;
		if(speed >= 4 && flag == 1 && location <= 22)location++;
		if(speed >= 5 && flag == 2 && location <= 38)location++;
		if(speed >= 5 && flag == 1 && location <= 26)location++;
		
		borders[0].B = 43-location;//矩形定位
		OLED_ShowImage(0,43-location,19,21,Dinosaur_up);
	}
	else if(sign == 1)
	{
		location--;
		if(flag == 2 && location <= 20 && location >= 1)location--;
		if(speed >= 3 && flag == 2 && location >= 1 && location <= 30)location--;
		if(speed >= 3 && flag == 1 && location >= 1 && location <= 16)location--;
		if(speed >= 4 && flag == 2 && location >= 1 && location <= 35)location--;
		if(speed >= 4 && flag == 1 && location >= 1 && location <= 22)location--;
		if(speed >= 5 && flag == 2 && location >= 1 && location <= 38)location--;
		if(speed >= 5 && flag == 1 && location >= 1 && location <= 26)location--;
		
		borders[0].B = 43-location;
		OLED_ShowImage(0,43-location,19,21,Dinosaur_up);
	}
	//跳跃限度-----------------------------------------------
	if(flag == 1)
	{
		if(location >= 30)sign = 1;//小跳为30像素
	}
	else if(flag == 2)
	{
		if(location >= 42)sign = 1;//大跳为42像素
	}
	if(location <= 0)//一次跳跃结束，重置变量
	{
		sign = 0;
		return 0;
	}
	return 1;//跳跃未结束
}

void Dino_ground_UI(void)//以3张图为一组，移动128*2次为一轮，更新随机值
{
	static uint16_t tick = 128*2;
	static uint8_t i;
	if(tick >= 128*2)
	{
		tick = 0;
		i = rand()%2;//随机场景
	}
	static uint16_t location = 0;
	if(i == 1)//概率0.5
	{
		OLED_ShowImage(0-location,64-5,128,5,Dino_ground_1);
		OLED_ShowImage(128-location,64-5,128,5,Dino_ground_2);
		OLED_ShowImage(128*2-location,64-5,128,5,Dino_ground_1);
	}
	else
	{
		OLED_ShowImage(0-location,64-5,128,5,Dino_ground_1);
		OLED_ShowImage(128-location,64-5,128,5,Dino_ground_2);
		OLED_ShowImage(128*2-location,64-5,128,5,Dino_ground_1);
	}
	location++;
	tick++;
	if(location >= 128*2)location = 0;
}
void Dino_cloud_UI(void)//放4页的中间两页，和仙人掌错开
{
	static uint16_t tick = 128*3;
	static uint8_t i,j,k;
	if(tick >= 128*3)
	{
		tick = 0;
		i =rand()%1;
		j = rand()%128*2;//	j随机x坐标
		k = rand()%128*2;
	}
	static uint16_t location = 0;
	if(i == 0)
	{
		OLED_ShowImage(128-location+j,10,22,8,Dino_cloud_1);
		OLED_ShowImage(128-location+k,10,22,8,Dino_cloud_2);
		location++;//只在有云刷新时才++
	}
	tick++;
	if(location >= 128*3)location = 0;
}
void Dino_cacti_UI(uint8_t Dino_speed)
{
	static uint16_t tick = 128*2;
	static uint8_t i,j;
	if(tick >= 128*2)
	{
		tick = 0;
		if(Dino_speed == 2)i = rand()%3+1;//	i=1~3随机仙人掌类型
		else if(Dino_speed == 1)i = rand()%2+1;//	i=1~2随机仙人掌类型
		else if(Dino_speed >= 3)i = rand()%4+1;//	i=1~4随机仙人掌类型
		j = rand()%128;//	j随机x坐标
	}
	static uint16_t location = 0;
	if(i == 1)
	{
		OLED_ShowImage(128+j-location,64-23,11,23,Dino_cacti_1);//统一放在第二页的随机位置
		borders[1].A = 128+j-location;//定位
		borders[2].A = 128;//防止干扰位置判断
		borders[3].A = 128;
		borders[4].A = 128;
	}
	else if(i == 2)
	{
		OLED_ShowImage(128+j-location,64-20,21,20,Dino_cacti_2);
		borders[2].A = 128+j-location;
		borders[1].A = 128;
		borders[3].A = 128;
		borders[4].A = 128;
	}
	else if(i == 3)
	{
		OLED_ShowImage(128+j-location,64-15,25,15,Dino_cacti_3);
		borders[3].A = 128+j-location;
		borders[1].A = 128;
		borders[2].A = 128;
		borders[4].A = 128;
	}
	else if(i == 4)
	{
		OLED_ShowImage(128+j-location,64-22,32,22,Dino_cacti_4);
		borders[4].A = 128+j-location;
		borders[1].A = 128;
		borders[2].A = 128;
		borders[3].A = 128;
	}
	tick++;
	location++;
	if(location >= 128*2)location = 0;//重置
}
void Dino_score_UI(uint32_t Dino_score)
{
	OLED_Printf(128-6*6,0,OLED_6X8,"%06d",Dino_score);
}
void Dino_scene_UI(uint8_t Dino_speed)
{
	Dino_ground_UI();
	Dino_cloud_UI();
	Dino_cacti_UI(Dino_speed);
}



uint8_t Dino_collision_judge(Dino_border* p)
{
	uint8_t i = 0;
	for(i=1;i<=4;i++)
	{
		if(p[0].B + p[0].high > p[i].B && p[0].length > p[i].A && p[i].A + p[i].length > 0)return 1;//碰撞
	}
	return 0;//未碰撞
}
uint32_t Dino_Run(void)
{
	uint32_t Dino_score = 0;//计分
	uint8_t Dino_speed = 1;//用于调整速度
	uint8_t Dino_speed_sign = 0;//用于辅助调整速度
	uint8_t Dino_background_sign = 0;//用于辅助背景调整
	uint8_t jumpoverflag;//用于判断跳跃是否结束
	uint8_t Dino_collisionflag = 0;//用于判断是否碰撞
	while(1)
	{
		//场景与跳跃
		//按键控制跳跃类别--------------------------------------------
		uint8_t num=Key_GetNum();
		if(num == 1)Dino_Runflag = 1;
		else if(num == 2)Dino_Runflag = 2;
		else if(num == 3)return Dino_score;//自尽
		OLED_Clear();
		//速度对场景填充的影响-------------------------------------------
			//speed=1
		Dino_scene_UI(Dino_speed);
			//speed=2
		if(Dino_speed == 2 && Dino_speed_sign == 1)
		{
			Dino_speed_sign = 0;
			OLED_Clear();
			Dino_scene_UI(Dino_speed);
		}
		else if(Dino_speed == 2 && Dino_speed_sign == 0)Dino_speed_sign = 1;
			//speed=3
		if(Dino_speed >= 3)
		{
			OLED_Clear();
			Dino_scene_UI(Dino_speed);
		}
		if(Dino_speed >= 4)
		{
			OLED_Clear();
			Dino_scene_UI(Dino_speed);
		}
		if(Dino_speed >= 5)
		{
			OLED_Clear();
			Dino_scene_UI(Dino_speed);
		}
		if(Dino_speed >= 6)
		{
			OLED_Clear();
			Dino_scene_UI(Dino_speed);
		}
		//跳跃与奔跑的处理及速度对跳跃的影响-------------------------------------------------------
		switch(Dino_Runflag)
		{
			case 0://奔跑
				Dino_runing_UI();
				break;
			case 1://小跳
				jumpoverflag = Dino_jumping_UI(1,Dino_speed);
				if(jumpoverflag == 0)Dino_Runflag = 0;//跳跃结束
				break;
			case 2://大跳
				jumpoverflag = Dino_jumping_UI(2,Dino_speed);
				if(jumpoverflag == 0)Dino_Runflag = 0;
				break;
			default:
				break;
		}
		//背景主题切换----------------------------------------------------
		if(Dino_speed == 3)OLED_Reverse();
		if(Dino_speed == 5)OLED_Reverse();
		if(Dino_speed == 6 && Dino_background_sign == 1)OLED_Reverse();
		//分数显示与OLED呈现------------------------------------------------------
		Dino_score_UI(Dino_score);
		OLED_Update();
		//碰撞判断--------------------------------------------------------
		Dino_collisionflag = Dino_collision_judge(borders);
		if(Dino_collisionflag == 1)return Dino_score;
		//动态变化-----------------------------------------------------------
			//分数对速度的转换
		if(Dino_score >= 1000 && Dino_score < 3000)Dino_speed = 2;
		if(Dino_score >= 3000 && Dino_score < 6000)Dino_speed = 3;
		if(Dino_score >= 6000 && Dino_score < 10000)Dino_speed = 4;
		if(Dino_score >= 10000 && Dino_score < 15000)Dino_speed = 5;
		if(Dino_score >= 15000)Dino_speed = 6;
		
			//速度对分数影响
		Dino_score++;
		if(Dino_speed == 2 && Dino_speed_sign == 1)Dino_score++;
		if(Dino_speed == 3)Dino_score++;
		if(Dino_speed == 4)Dino_score++;
		if(Dino_speed == 5)Dino_score++;
		if(Dino_speed == 6)Dino_score++;
		
		//分数对背景切换的影响
		if(Dino_speed == 6 && Dino_score/5000%2 == 0)Dino_background_sign = 1;
		else if(Dino_speed == 6 && Dino_score/5000%2 == 1)Dino_background_sign = 0;
	}
}

Page Dino_end_Page(void)
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
			else if(game_endflag == 2)Dino_Run();
			//else if(game_endflag == 3)
		}
		//OLED_Clear();
		Dino_end_UI();
		switch(game_endflag)
		{
			case 1://退出键
				OLED_ReverseArea(64-8-4-16,34,16,16);
				break;
			case 2://重新开始
				OLED_ReverseArea(64-8,34,16,16);
				break;
			case 3://历史记录
				OLED_ReverseArea(64+8+4,34,16,16);
				break;
			default:
				break;
		}
		OLED_Update();
	}
}

Page Dino_begin_Page(void)
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
				Dino_Run();
				return MENU_GAME_END;
			}
		}
		OLED_Clear();
		Dino_start_UI();
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



