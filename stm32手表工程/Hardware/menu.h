#ifndef __MENU_H
#define __MENU_H

typedef enum PAGE{
	CLOCK = 1,
	MENU = 2,
	SETTING = 3,
	SETTING_TIME = 4,
	MENU_TIMER = 5,
	MENU_ALARM = 6,
	MENU_LIGHT = 7,
	MENU_GAME = 8,
	MENU_GAME_DINO_END = 9,
	MENU_GAME_DINO_BEGIN = 10,
	SETTING_MOVESPEED = 11,
	MENU_GAME_DINO_SCORESLIST = 12,
	MENU_ALARM_CUSTOMIZE = 13,
	MENU_ALARM_COUNTDOWN = 14,
	MENU_ALARM_COUNTDOWN_SETTING = 15,
}Page;

extern uint8_t clkflag;
//CLOCK时钟界面相关函数
void Show_Clock_UI(void);
Page CLK_Page(void);
void Clock_tick(void);

//SETTING设置界面相关函数
void Show_Setting_UI(void);
Page Setting_Page(void);

//MENU菜单界面相关函数
void Show_Menu_UI(void);
void Show_Menu_movingUI(int moveflag,uint8_t previous,uint8_t current);
Page Menu_Page(void);




#endif
