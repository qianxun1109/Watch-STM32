#ifndef __MENU_ALARM_H
#define __MENU_ALARM_H

void Alarm_Init(void);
void Alarm_ON(void);
void Alarm_OFF(void);
Page Menu_alarm_customize_Page(void);
Page Menu_alarm_countdown_Page(void);
Page Menu_alarm_countdown_setting_Page(void);
void alarm_countdown_start(void);
void alarm_countdown_stop(void);

extern volatile uint8_t alarm_set_flag;
extern volatile uint8_t alarm_set_flag2;
extern int MyAlarm_time[3];
	
#endif
