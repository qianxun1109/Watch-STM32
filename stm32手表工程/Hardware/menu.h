#ifndef __MENU_H
#define __MENU_H

typedef enum PAGE{
	CLOCK = 1,
	MENU = 2,
	SETTING = 3,
	SETTING_TIME = 4,
	

}Page;

extern uint8_t clkflag;
//CLOCK界面相关函数
void Show_Clock_UI(void);
uint8_t CLK_Page(void);
uint8_t Setting_Page(void);
//设置界面相关函数
void Show_Setting_UI(void);
uint8_t Setting_Page(void);

void Show_pageflag_proc(void);


#endif
