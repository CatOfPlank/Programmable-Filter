#ifndef __BASE_H
#define __BASE_H	

#include "stdlib.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "gpio.h"

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

#define LED0(n)		(n?HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_RESET))
#define LED0_T (HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin)) //IO电平翻转
#define LED1(n)		(n?HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET))
#define LED1_T (HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin)) //IO电平翻转

#define KEY0  HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)//读取按键0
#define KEY1  HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)//读取按键1
#define KEY2  HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)//读取按键2

#define KEY0_PRES	1	
#define KEY1_PRES	2	
#define KEY2_PRES	3	

void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);
void delay_ns (u8 t);
uint8_t KEY_Scan(uint8_t mode);  	//按键扫描函数	
typedef struct myFilter
{
	u16 cur_fre;  //当前频率
	u32 sample_rate;  //采样率
	u16 resolution_fre	; //频率分辨率
	u16 show_points;
	u32 cutoff_fre; 	//滤波器截止频率
	u8 filterKind;		//滤波器类型
	u8 State;  //状态
	u8 Mode;   //平常模式和扫频模式
	u16 amp_dB;  //放大倍数
	float THD; //总谐波失真
	
}my_Filter;
enum SysMode{  //系统模式,正常滤波和扫频
Normal = 0, Sweep = 1,sysReset = 2
};
enum FilterKind{	//低通，高通，椭圆
LP = 0, HP = 1, ELL = 2,Amp = 3
};
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014 
#define GPIOJ_ODR_ADDr    (GPIOJ_BASE+20) //0x40022414
#define GPIOK_ODR_ADDr    (GPIOK_BASE+20) //0x40022814

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
#define GPIOJ_IDR_Addr    (GPIOJ_BASE+16) //0x40022410 
#define GPIOK_IDR_Addr    (GPIOK_BASE+16) //0x40022810 

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

#define PJout(n)   BIT_ADDR(GPIOJ_ODR_Addr,n)  //输出 
#define PJin(n)    BIT_ADDR(GPIOJ_IDR_Addr,n)  //输入

#define PKout(n)   BIT_ADDR(GPIOK_ODR_Addr,n)  //输出 
#define PKin(n)    BIT_ADDR(GPIOK_IDR_Addr,n)  //输入


#endif

