#ifndef __OLED_DATA_H
#define __OLED_DATA_H

/*包含字符集的选择*/

#include <stdint.h>

/*字符集定义*/
/*以下两个宏定义只可解除其中一个的注释*/
#define OLED_CHARSET_UTF8			//定义字符集为UTF8
//#define OLED_CHARSET_GB2312		//定义字符集为GB2312

/*字模基本单元*/
typedef struct 
{
	
#ifdef OLED_CHARSET_UTF8			//定义字符集为UTF8
	char Index[5];					//汉字索引，空间为5字节
#endif
	
#ifdef OLED_CHARSET_GB2312			//定义字符集为GB2312
	char Index[3];					//汉字索引，空间为3字节
#endif
	
	uint8_t Data[32];				//字模数据
} ChineseCell_t;

/*ASCII字模数据声明*/
extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F6x8[][6];
extern const uint8_t OLED_F12x24[][36];

/*汉字字模数据声明*/
extern const ChineseCell_t OLED_CF16x16[];

/*图像数据声明*/
extern const uint8_t Diode[];
extern const uint8_t Return[];
extern const uint8_t Aila[];
extern const uint8_t Light_icon[];
extern const uint8_t Game_icon[];
extern const uint8_t Timer_icon[];
extern const uint8_t Alarm_icon[];
extern const uint8_t Return_icon[];
extern const uint8_t Target_icon[];
extern const uint8_t Chrome_Dino_run_start[];
extern const uint8_t Chrome_Dino_run_fail[];
extern const uint8_t Dinosaur1[];
extern const uint8_t Dinosaur2[];
extern const uint8_t Dinosaur_dead[];
extern const uint8_t Dinosaur_up[];
extern const uint8_t Begin[];
extern const uint8_t Dino_cacti_1[];
extern const uint8_t Dino_cacti_1_s[];
extern const uint8_t Dino_cacti_2[];
extern const uint8_t Dino_cacti_2_s[];
extern const uint8_t Dino_cacti_3[];
extern const uint8_t Dino_cacti_4[];
extern const uint8_t Dino_cloud_1[];
extern const uint8_t Dino_cloud_2[];
extern const uint8_t Dino_ground_1[];
extern const uint8_t Dino_ground_2[];
extern const uint8_t Dino_bird_1[];
extern const uint8_t Dino_bird_2[];
extern const uint8_t Dino_restart[];
extern const uint8_t Dino_rank[];
extern const uint8_t OK[];
extern const uint8_t Set[];
extern const uint8_t Clock[];
extern const uint8_t Cancel[];

/*按照上面的格式，在这个位置加入新的图像数据声明*/
//...

#endif


/*****************江协科技|版权所有****************/
/*****************jiangxiekeji.com*****************/
