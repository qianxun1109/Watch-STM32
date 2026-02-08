#ifndef __MYRTC_H
#define __MYRTC_H
void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_Readtime(void);
void Alarm_Readtime(void);
uint32_t Alarm_GetCounter(void);

extern int MyRTC_Time[];
extern int weekday;



#endif
