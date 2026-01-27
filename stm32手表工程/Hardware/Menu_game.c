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
uint8_t Dino_end_reverseflag = 0;//用于协调结束画面的反色显示
//极速模式调节
uint8_t mode = 0;
uint8_t mode_sign = 0;

Dino_border borders[]={
{0,43,19,21},//小恐龙(B值变化)->0
{128,64-23,11,23},//Dino_cacti_1(A值变化)->1
{128,64-15,10,15},//Dino_cacti_1_s->2
{128,64-15,16,15},//Dino_cacti_2_s->3
{128,64-20,21,20},//Dino_cacti_2->4
{128,64-15,25,15},//Dino_cacti_3->5
{128,64-22,32,22},//Dino_cacti_4->6
{128,0,25,20},//Dino_bird->7(A值变化，B值选择)
};

uint32_t Dino_historicalscores[16]={0};
/*-----------------------函数变量------------------------*/
//Dino_bird_UI(已废弃)
//uint8_t Dino_bird_UI_count = 0;
//uint8_t Dino_bird_UI_sign = 0;//用于调节bird的动态变化
//uint16_t Dino_bird_UI_tick = 128*9;
//uint8_t Dino_bird_UI_i,Dino_bird_UI_j;
//uint16_t Dino_bird_UI_location = 0;

//Dino_cacti_UI
uint16_t Dino_cacti_UI_tick = 128*2;
uint8_t Dino_cacti_UI_i,Dino_cacti_UI_j;
uint16_t Dino_cacti_UI_location = 0;
//Dino_ground_UI
uint16_t Dino_ground_UI_tick = 128*2;
uint8_t Dino_ground_UI_i;
uint16_t Dino_ground_UI_location = 0;
//Dino_jumping_UI
int Dino_jumping_UI_location = 0;
uint8_t Dino_jumping_UI_sign = 0;//sign=1为上升过程，0为下降过程
//Dino_runing_UI
uint8_t Dino_runing_UI_count = 0;
uint8_t Dino_runing_UI_sign = 0;


void Dino_Reset(void)
{
//	Dino_bird_UI_count = 0;
//	Dino_bird_UI_sign = 0;
//	Dino_bird_UI_tick = 128*9;
//	Dino_bird_UI_location = 0;
	
	mode = 0;
	mode_sign = 0;
	Dino_Runflag = 0;
	
	Dino_cacti_UI_tick = 128*2;
	Dino_cacti_UI_location = 0;
	
	Dino_ground_UI_tick = 128*2;
	Dino_ground_UI_location = 0;
	
	Dino_jumping_UI_location = 0;
	Dino_jumping_UI_sign = 0;
	
	Dino_runing_UI_count = 0;
	Dino_runing_UI_sign = 0;
	
	borders[0].B=43;
	borders[1].A=128;
	borders[2].A=128;
	borders[3].A=128;
	borders[4].A=128;
	borders[5].A=128;
	borders[6].A=128;
//	borders[7]=         
}

int Dino_find_available_position(void)
{
	uint8_t i;
	for(i=0;i<16;i++)
	{
		if(Dino_historicalscores[i] == 0)return i;
	}
	return -1;
}
void Dino_score_sort(void)
{
	uint8_t i,j,min;
	for(i=0;i<16;i++)
	{
		min = 0;
		for(j=0;j<16-i;j++)
		{
			if(Dino_historicalscores[min] > Dino_historicalscores[j])min=j;
		}
		uint32_t temp = Dino_historicalscores[min];
		Dino_historicalscores[min] = Dino_historicalscores[15-i];
		Dino_historicalscores[15-i] = temp;
	}
}
void Dino_score_add(uint32_t score)
{
	Dino_score_sort();
	int position = Dino_find_available_position();
	if(position != -1)Dino_historicalscores[position]=score;
	else if(position == -1 && Dino_historicalscores[15] < score)Dino_historicalscores[15] = score;
}
void Dino_scoreslist_UI(void)
{
	Dino_score_sort();//先保证排序没问题
	uint8_t i;
	for(i=0;i<8;i++)OLED_Printf(0,8*i,OLED_6X8,"%02d",i+1);
	for(i=0;i<8;i++)OLED_Printf(64+6,8*i,OLED_6X8,"%02d",i+9);
	
	for(i=0;i<8;i++)OLED_Printf(63,8*i,OLED_6X8,"|");
	
	for(i=0;i<8;i++)OLED_Printf(64-38,8*i,OLED_6X8,"%06d",Dino_historicalscores[i]);
	for(i=0;i<8;i++)OLED_Printf(128-38,8*i,OLED_6X8,"%06d",Dino_historicalscores[i+8]);
	
}
Page Dino_scoreslist_Page(void)
{
	Previous_Pageflag = Pageflag;
	while(1)
	{
		uint8_t num=Key_GetNum();
		if(num == 3)return MENU_GAME_DINO_BEGIN;
		OLED_Clear();
		Dino_scoreslist_UI();
		OLED_Update();
	}
}
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
	if(Dino_end_reverseflag == 1)
	{
		OLED_ReverseArea(64-2-4*8,32-16-2,8*4,16);
		OLED_ReverseArea(64+2,32-16-2,8*4,16);
		OLED_ReverseArea(64-8-4-16,34,16,16);
		OLED_ReverseArea(64-8,34,16,16);
		OLED_ReverseArea(64+8+4,34,16,16);
		OLED_ReverseArea(0,borders[0].B,19,21);
	}
}
void Dino_runing_UI(void)
{
//	static uint8_t count = 0;
//	static uint8_t sign = 0;
	if(Dino_runing_UI_sign)
	{
		Dino_runing_UI_count++;
		OLED_ShowImage(0,43,19,21,Dinosaur1);
		if(Dino_runing_UI_count >= 3)
		{
			Dino_runing_UI_sign = 0;
			Dino_runing_UI_count = 0;
		}
	}
	else 
	{
		Dino_runing_UI_count++;
		OLED_ShowImage(0,43,19,21,Dinosaur2);
		if(Dino_runing_UI_count >= 3)
		{
			Dino_runing_UI_sign = 1;
			Dino_runing_UI_count = 0;
		}
	}
}

uint8_t Dino_jumping_UI(uint8_t flag,uint8_t speed)//1小跳 2大跳
{
//	static int location = 0;
//	static uint8_t sign = 0;//sign=1为上升过程，0为下降过程
	//速度对跳跃过程的控制-------------------------------------
	if(Dino_jumping_UI_sign == 0)
	{
		Dino_jumping_UI_location++;
		if(flag == 2 && Dino_jumping_UI_location <= 20)Dino_jumping_UI_location++;//跳跃速度变化
		if(speed >= 3 && flag == 2 && Dino_jumping_UI_location <= 30)Dino_jumping_UI_location++;
		if(speed >= 3 && flag == 1 && Dino_jumping_UI_location <= 16)Dino_jumping_UI_location++;
		if(speed >= 4 && flag == 2 && Dino_jumping_UI_location <= 35)Dino_jumping_UI_location++;
		if(speed >= 4 && flag == 1 && Dino_jumping_UI_location <= 22)Dino_jumping_UI_location++;
		if(speed >= 5 && flag == 2 && Dino_jumping_UI_location <= 38)Dino_jumping_UI_location++;
		if(speed >= 5 && flag == 1 && Dino_jumping_UI_location <= 26)Dino_jumping_UI_location++;
		
		borders[0].B = 43 - Dino_jumping_UI_location;//矩形定位
		OLED_ShowImage(0,43 - Dino_jumping_UI_location,19,21,Dinosaur_up);
	}
	else if(Dino_jumping_UI_sign == 1)
	{
		Dino_jumping_UI_location--;
		if(flag == 2 && Dino_jumping_UI_location <= 20 && Dino_jumping_UI_location >= 1)Dino_jumping_UI_location--;
		if(speed >= 3 && flag == 2 && Dino_jumping_UI_location >= 1 && Dino_jumping_UI_location <= 30)Dino_jumping_UI_location--;
		if(speed >= 3 && flag == 1 && Dino_jumping_UI_location >= 1 && Dino_jumping_UI_location <= 16)Dino_jumping_UI_location--;
		if(speed >= 4 && flag == 2 && Dino_jumping_UI_location >= 1 && Dino_jumping_UI_location <= 35)Dino_jumping_UI_location--;
		if(speed >= 4 && flag == 1 && Dino_jumping_UI_location >= 1 && Dino_jumping_UI_location <= 22)Dino_jumping_UI_location--;
		if(speed >= 5 && flag == 2 && Dino_jumping_UI_location >= 1 && Dino_jumping_UI_location <= 38)Dino_jumping_UI_location--;
		if(speed >= 5 && flag == 1 && Dino_jumping_UI_location >= 1 && Dino_jumping_UI_location <= 26)Dino_jumping_UI_location--;
		
		borders[0].B = 43 - Dino_jumping_UI_location;
		OLED_ShowImage(0,43 - Dino_jumping_UI_location,19,21,Dinosaur_up);
	}
	//跳跃限度-----------------------------------------------
	if(flag == 1)
	{
		if(Dino_jumping_UI_location >= 30)Dino_jumping_UI_sign = 1;//小跳为30像素
	}
	else if(flag == 2)
	{
		if(Dino_jumping_UI_location >= 44)Dino_jumping_UI_sign = 1;//大跳为42像素
	}
	if(Dino_jumping_UI_location <= 0)//一次跳跃结束，重置变量
	{
		Dino_jumping_UI_sign = 0;
		return 0;
	}
	return 1;//跳跃未结束
}

void Dino_ground_UI(void)//以3张图为一组，移动128*2次为一轮，更新随机值
{
//	static uint16_t tick = 128*2;
//	static uint8_t i;
//	static uint16_t location = 0;	
	if(Dino_ground_UI_tick >= 128*2)
	{
		Dino_ground_UI_tick = 0;
		Dino_ground_UI_i = rand()%2;//随机场景
	}
	if(Dino_ground_UI_i == 1)//概率0.5
	{
		OLED_ShowImage(0 - Dino_ground_UI_location,64-5,128,5,Dino_ground_1);
		OLED_ShowImage(128 - Dino_ground_UI_location,64-5,128,5,Dino_ground_2);
		OLED_ShowImage(128*2 - Dino_ground_UI_location,64-5,128,5,Dino_ground_1);
	}
	else
	{
		OLED_ShowImage(0 - Dino_ground_UI_location,64-5,128,5,Dino_ground_1);
		OLED_ShowImage(128 - Dino_ground_UI_location,64-5,128,5,Dino_ground_2);
		OLED_ShowImage(128*2 - Dino_ground_UI_location,64-5,128,5,Dino_ground_1);
	}
	Dino_ground_UI_location++;
	Dino_ground_UI_tick++;
	if(Dino_ground_UI_location >= 128*2)Dino_ground_UI_location = 0;
}
void Dino_cloud_UI(void)//放4页的中间两页，和仙人掌错开
{
	static uint16_t tick = 128*3;
	static uint8_t i,j,k;
	if(tick >= 128*3)
	{
		tick = 0;
		i =rand()%1;
		j = rand()%(128*2);//	j随机x坐标
		k = rand()%(128*2);
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
//	static uint16_t tick = 128*2;
//	static uint8_t i,j;
//	static uint16_t location = 0;	
	if(Dino_cacti_UI_tick >= 128*2)
	{
		Dino_cacti_UI_tick = 0;
		if(Dino_speed == 1)Dino_cacti_UI_i = rand()%3+1;//	i=1~3随机仙人掌类型
		else if(Dino_speed == 2)Dino_cacti_UI_i = rand()%5+1;//	i=1~5随机仙人掌类型
		else if(Dino_speed >= 3)Dino_cacti_UI_i = rand()%6+1;//	i=1~6随机仙人掌类型
		Dino_cacti_UI_j = rand()%100;//	j随机x坐标（避开右边那块，在检测时A值接近0可能会导致无限死亡）
	}
	if(Dino_cacti_UI_i == 1)
	{
		OLED_ShowImage(128+Dino_cacti_UI_j - Dino_cacti_UI_location,64-23,11,23,Dino_cacti_1);//统一放在第二页的随机位置
		borders[1].A = 128+Dino_cacti_UI_j - Dino_cacti_UI_location;//定位
	}
	else if(Dino_cacti_UI_i == 2)
	{
		OLED_ShowImage(128+Dino_cacti_UI_j - Dino_cacti_UI_location,64-15,10,15,Dino_cacti_1_s);
		borders[2].A = 128+Dino_cacti_UI_j - Dino_cacti_UI_location;
	}
	else if(Dino_cacti_UI_i == 3)
	{
		OLED_ShowImage(128+Dino_cacti_UI_j - Dino_cacti_UI_location,64-15,16,15,Dino_cacti_2_s);
		borders[3].A = 128+Dino_cacti_UI_j - Dino_cacti_UI_location;
	}
	else if(Dino_cacti_UI_i == 4)
	{
		OLED_ShowImage(128+Dino_cacti_UI_j - Dino_cacti_UI_location,64-20,21,20,Dino_cacti_2);
		borders[4].A = 128+Dino_cacti_UI_j - Dino_cacti_UI_location;
	}
	else if(Dino_cacti_UI_i == 5)
	{
		OLED_ShowImage(128+Dino_cacti_UI_j - Dino_cacti_UI_location,64-15,25,15,Dino_cacti_3);
		borders[5].A = 128+Dino_cacti_UI_j - Dino_cacti_UI_location;
	}
	else if(Dino_cacti_UI_i == 6)
	{
		OLED_ShowImage(128+Dino_cacti_UI_j - Dino_cacti_UI_location,64-22,32,22,Dino_cacti_4);
		borders[6].A = 128+Dino_cacti_UI_j - Dino_cacti_UI_location;
	}
	Dino_cacti_UI_tick++;
	Dino_cacti_UI_location++;
	if(Dino_cacti_UI_location >= 128*2)Dino_cacti_UI_location = 0;//重置
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
	//if(Dino_speed >= 4)Dino_bird_UI(Dino_speed);
}

uint8_t Dino_collision_judge(Dino_border* p)
{
	uint8_t i = 0;
	for(i=1;i<=6;i++)
	{
		//if(p[i].A+p[i].length >0 && p[i].A < 128)//只对在屏幕里的点判断（可能出现仙人掌移出最后A卡在0左附近，导致离奇死亡）
		if(p[0].B + p[0].high > p[i].B && p[0].length > p[i].A && p[i].A + p[i].length > 0 && p[0].B < p[i].B + p[i].high)return 1;//碰撞
	}
	return 0;//未碰撞
}
uint32_t Dino_Run(void)//注：返回值最高位是一个标志为，不算在分数里，要先分离
{
	Dino_Reset();
	uint32_t Dino_score = 0;//计分
	uint8_t Dino_speed = 1;//用于调整速度
	uint8_t Dino_speed_sign = 0;//用于辅助调整速度
	uint8_t Dino_background_sign = 0;//用于辅助背景调整
	uint8_t jumpoverflag = 0;//用于判断跳跃是否结束
	uint8_t Dino_collisionflag = 0;//用于判断是否碰撞
	while(1)
	{
		//场景与跳跃
		//极速模式
		if(mode_sign == 1)
		{
			mode_sign = 0;
			Dino_score = 20000;
		}
		//按键控制跳跃类别--------------------------------------------
		uint8_t num=Key_GetNum();
		if(num == 1)Dino_Runflag = 1;
		else if(num == 2)Dino_Runflag = 2;
		else if(num == 3 && mode == 0)
		{
			if(mode == 0)
			{
				Dino_score_UI(Dino_score);//这里的score多加了一轮，把显示的值对齐
				OLED_Update();
			}
			return Dino_score;//自尽
		}
		else if(num == 3 && mode == 1)return 0;//极速模式不给分
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
			//speed更大
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
		if(Dino_speed >= 7)
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
		//分数显示(极速模式不显示)----------------------------------------
		if(mode == 0)Dino_score_UI(Dino_score);
		//背景主题切换----------------------------------------------------
		if(Dino_speed == 3)OLED_Reverse();
		if(Dino_speed == 5)OLED_Reverse();
		if(Dino_speed == 7 && Dino_background_sign == 1)OLED_Reverse();
		//结束反色标志与OLED呈现-----------------------------------------------
		if(Dino_speed == 3 || Dino_speed == 5 || (Dino_speed == 7 && Dino_background_sign == 1))Dino_end_reverseflag = 1;
		else Dino_end_reverseflag = 0;
		OLED_Update();
		//碰撞判断--------------------------------------------------------
		Dino_collisionflag = Dino_collision_judge(borders);
		if(Dino_collisionflag == 1 && mode ==0)return Dino_score;
		else if(Dino_collisionflag == 1 && mode ==1)return 0;//极速模式不给分
		//动态变化-----------------------------------------------------------
			//分数对速度的转换
		if(Dino_score >= 1000 && Dino_score < 3000)Dino_speed = 2;
		if(Dino_score >= 3000 && Dino_score < 6000)Dino_speed = 3;
		if(Dino_score >= 6000 && Dino_score < 10000)Dino_speed = 4;
		if(Dino_score >= 10000 && Dino_score < 15000)Dino_speed = 5;
		if(Dino_score >= 15000 && Dino_score < 20000)Dino_speed = 6;
		if(Dino_score >= 20000)Dino_speed = 7;
			//速度对分数影响
		Dino_score++;
		if(Dino_speed == 2 && Dino_speed_sign == 1)Dino_score++;
		if(Dino_speed >= 3)Dino_score++;
		if(Dino_speed >= 4)Dino_score++;
		if(Dino_speed >= 5)Dino_score++;
		if(Dino_speed >= 6)Dino_score++;
		if(Dino_speed >= 7)Dino_score++;
			//分数对背景切换的影响
		if(Dino_speed == 7 && Dino_score/5000%2 == 0)Dino_background_sign = 1;
		else if(Dino_speed == 7 && Dino_score/5000%2 == 1)Dino_background_sign = 0;
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
			else if(game_endflag == 2)
			{
				uint32_t temp = Dino_Run();
				Dino_score_add(temp);
			}
			else if(game_endflag == 3)return MENU_GAME_DINO_SCORESLIST;
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
				uint32_t temp = Dino_Run();
				Dino_score_add(temp);
				return MENU_GAME_DINO_END;
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

void Dino_mode_tick(void)
{
	static uint16_t Dino_modetick = 0;
	if(Key_GetState() == 1)Dino_modetick++;
	else Dino_modetick = 0;
	if(Dino_modetick >= 1000)
	{
		mode = 1;
		mode_sign = 1;
	}
}
	



//void Dino_bird_UI(uint8_t Dino_speed)//放10页的中间
//{
////	static uint8_t count = 0;
////	static uint8_t sign = 0;//用于调节bird的动态变化
////	static uint16_t tick = 128*9;
////	static uint8_t i,j;
//	if(Dino_bird_UI_tick >= 128*9)
//	{
//		Dino_bird_UI_tick = 0;
//		Dino_bird_UI_i = rand()%2+1;
//		Dino_bird_UI_j = rand()%(128*5);//	j随机x坐标
//	}
////	static uint16_t location = 0;
//	if(Dino_bird_UI_sign)
//	{
//		Dino_bird_UI_count++;
//		if(Dino_bird_UI_i == 1)//上方
//		{
//			OLED_ShowImage(128+Dino_bird_UI_j-Dino_bird_UI_location,16,25,20,Dino_bird_1);//统一放在中间的随机位置
//			borders[7].B = 16;
//		}
//		else if(Dino_bird_UI_i == 2)//下方
//		{
//			OLED_ShowImage(128+Dino_bird_UI_j-Dino_bird_UI_location,40,25,20,Dino_bird_1);
//			borders[7].B = 40;
//		}
//		if(Dino_bird_UI_count >= 6)
//		{
//			Dino_bird_UI_sign = 0;
//			Dino_bird_UI_count = 0;
//		}
//	}
//	else
//	{
//		Dino_bird_UI_count++;
//		if(Dino_bird_UI_i == 1)//上方
//		{
//			OLED_ShowImage(128+Dino_bird_UI_j-Dino_bird_UI_location,16,25,20,Dino_bird_2);
//			borders[7].B = 16;
//		}
//		else if(Dino_bird_UI_i == 2)//下方
//		{
//			OLED_ShowImage(128+Dino_bird_UI_j-Dino_bird_UI_location,40,25,20,Dino_bird_2);
//			borders[7].B = 40;
//		}
//		if(Dino_bird_UI_count >= 6)
//		{
//			Dino_bird_UI_sign = 1;
//			Dino_bird_UI_count = 0;
//		}
//	}
//	
//	if(borders[7].A + borders[7].length <= 0)
//	{
//		// 鸟完全出屏后清空碰撞框，重置局部static变量
//		borders[7].A = 128*2; // 重置到屏幕右侧外更远的地方
//		borders[7].B = 0;     // 重置高度为0，彻底脱离恐龙碰撞区间
//	}
//	else borders[7].A = 128+Dino_bird_UI_j-Dino_bird_UI_location;

//	Dino_bird_UI_tick++;
//	Dino_bird_UI_location++;
//	if(Dino_bird_UI_location >= 128*9)Dino_bird_UI_location = 0;//重置
//}

