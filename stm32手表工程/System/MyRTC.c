#include "stm32f10x.h"                  // Device header
#include <time.h>
#include "MyRTC.h"

//修改时间戳：先改MyRTC_Time数组数据，再调用MyRTC_SetTime函数

uint16_t MyRTC_Time[6]={2026,1,22,22,38,0};

void MyRTC_Init(void)//注意：vbat不掉电的情况下mcu复位不能重复执行初始化
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);//用PWR使能对rtc和bkp的访问
	
	//上电后只执行一次初始化，复位时不再执行，通过给BKP_DR1随便写一个值进行区分
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		//时钟源开启（启动振荡器）
		RCC_LSEConfig(RCC_LSE_ON);//lse在vbat不掉电时不会停止
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)!=SET);//等待LSE启动完毕ready//如何起振不了，会在这卡死
		//时钟源配置使能
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		//等待时机（防止意外）
		RTC_WaitForLastTask();//等待上次任务结束，写操作前要调用
		RTC_WaitForSynchro();//等待APB与RTC时钟源同步，读操作前要调用（以防读到rtc未来得及更新数据前的错误值）
		//配置重装值
		RTC_SetPrescaler(32768-1);//得到1Hz频率，LSE频率一般为32768
		RTC_WaitForLastTask();//将写操作隔开，避免冲突（时钟问题）（建议每次写入都隔开）
		//根据MyRTC_Time数组数据设置时间戳
		MyRTC_SetTime();
		RTC_WaitForLastTask();
		
		BKP_WriteBackupRegister(BKP_DR1,0xA5A5);//标记初始化完成，复位不再执行
	}
	else//再等等防止意外
	{
		RTC_WaitForLastTask();
		RTC_WaitForSynchro();
	}
}

//根据时间设置时间戳
void MyRTC_SetTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_date.tm_year=MyRTC_Time[0] - 1900;//time.h规定从1900年算起
	time_date.tm_mon=MyRTC_Time[1] - 1;//月的值为0~11，要处理
	time_date.tm_mday=MyRTC_Time[2];//day值从1开始不用补
	time_date.tm_hour=MyRTC_Time[3];//小时就是从0到23，与月份不同，无需再处理
	time_date.tm_min=MyRTC_Time[4];
	time_date.tm_sec=MyRTC_Time[5];
	
	time_cnt = mktime(&time_date);//转换成时间戳
	
	RTC_SetCounter(time_cnt - 8*60*60);//北京时区转换回伦敦时区，符合规范
	RTC_WaitForLastTask();
}
//根据时间戳获取时间
void MyRTC_Readtime(void)
{
	time_t time_cnt;
	time_cnt = RTC_GetCounter() + 8*60*60;//时区偏移，实现时间戳与北京时间的对应
	  
	struct tm time_date;
	//由于单片机没有系统时间，也无法区分时区，默认伦敦时间，要手动进行时区偏移
	time_date = *localtime(&time_cnt);//localtime函数根据当前时间戳返回一个tm类型的结构体指针
	
	MyRTC_Time[0]=time_date.tm_year + 1900;
	MyRTC_Time[1]=time_date.tm_mon + 1;
	MyRTC_Time[2]=time_date.tm_mday;
	MyRTC_Time[3]=time_date.tm_hour;
	MyRTC_Time[4]=time_date.tm_min;
	MyRTC_Time[5]=time_date.tm_sec;
}
