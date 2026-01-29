#ifndef __TIMER_H
#define __TIMER_H
extern volatile uint8_t MyAlarm_countdowntime_sign;

void Timer_Init(void);
void MyAlarm_ctrl_countdown(uint8_t flag);
void MyAlarm_ctrl_customize(uint8_t flag);

#endif
