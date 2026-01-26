#ifndef __MENU_GAME_H
#define __MENU_GAME_H

/*	(A,B)-----
	  |	   	 |
	  --------
*/
typedef struct Dino_Border{//用与碰撞判断
	int A;
	int B;
	int length;
	int high;
}Dino_border;

void Dino_end_UI(void);
void Dino_start_UI(void);
void Dino_runing_UI(void);
uint8_t Dino_jumping_UI(uint8_t flag,uint8_t speed);
void Dino_ground_UI(void);
void Dino_cloud_UI(void);
void Dino_cacti_UI(uint8_t Dino_speed);
void Dino_scene_UI(uint8_t Dino_speed);
void Dino_score_UI(uint32_t Dino_score);
uint8_t Dino_collision_judge(Dino_border* p);

uint32_t Dino_Run(void);

Page Dino_end_Page(void);
Page Dino_begin_Page(void);





#endif
