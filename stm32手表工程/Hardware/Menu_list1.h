#ifndef __MENU_LIST1_H
#define __MENU_LIST1_H
//秒表相关函数
void Show_Menu_timer_UI(void);
void Show_Menu_timer_movingUI(void);
Page Menu_timer_Page(void);
void Menu_timer_blink(void);
void Menu_timer_start(void);
void Menu_timer_stop(void);
void Menu_timer_reset(void);
//灯光相关函数
void Show_Menu_light_UI(void);
void Show_Menu_light_movingUI(int moveflag);
Page Menu_light_Page(void);
//游戏界面相关函数
Page Menu_game_Page(void);
void Show_Menu_game_UI(void);
void Show_Menu_game_movingUI(int moveflag);
//闹钟界面相关函数
void Show_Menu_alarm_UI(void);
void Show_Menu_alarm_movingUI(int moveflag);
Page Menu_alarm_Page(void);


#endif 

