#include "MAX262.H"
#include "head_define.h"
extern float Lp_Q_Value,Hp_Q_Value;
/*
MAX262是双二阶开关电容有源滤波器,每个二阶都能单独配置,可组合成2阶\4阶低通\高通\带通\带阻\全通滤波器
正负5V供电

*/

void MAX262_GPIO_Init(void)               // MAX262引脚配置
{
	//第一个MAX262 PE7-PE14
	GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOE_CLK_ENABLE();      	
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14; 
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	//第二个MAX262 PG7-PG14
    __HAL_RCC_GPIOG_CLK_ENABLE();      	
    GPIO_Initure.Pin=GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14; 
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);

}

//-----------------------------------------------------------------
// u8 Fn(float f)                     
//-----------------------------------------------------------------
// 函数功能: 计算f的频率控制字N
// 入口参数: 截止频率(中心频率) fc
// 返 回 值: 控制字N
//-----------------------------------------------------------------
//适配模式1,3,4
uint8_t Fn1()           
{
    //return fclk/fc*2/PI-26;         //原本是*2/pi;即*0.637  fclk用M单位，f单位khz
	return FCLK_FC*0.63661977f-26;    //=63

}
//适配模式2
uint8_t Fn2()                      //频率关键字
{
    //return (fclk2*637)-26;         //原本是*2/pi;即*0.637  fclk用M单位，f单位khz
	//return fclk/fc/1.11072-26;
	return FCLK_FC*0.90031691f-26;
}

//-----------------------------------------------------------------
// u8 Qn(float q)                     
//-----------------------------------------------------------------
// 函数功能: 计算Q值的控制字N
// 入口参数: Q值
// 返 回 值: 控制字N
//-----------------------------------------------------------------
//适配模式1,3,4
uint8_t Qn1(float q)                 
{
    return 128-(64/q);  //具体见芯片手册公式
}
//适配模式2
uint8_t Qn2(float q)                 
{
    return 128-(90.51f/q);  //具体见芯片手册公式
}


//-----------------------------------------------------------------
// void Filter1(uint8_t mode, float q)
//-----------------------------------------------------------------
// 函数功能: 滤波器的模式、频率以及Q值的设置
// 入口参数: 模式 mode, Q值 q
//-----------------------------------------------------------------
void Filter1(uint8_t mode, float q)
{
	uint8_t i=0,sf, sq;
	uint8_t a = 0x03;
	sf = Fn1();                     // 求出其频率控制字N
	sq = Qn1(q);                     // 求出Q对应的控制字N
	LE_H;                           // 使能端拉高
	delay_ns(200);
	WR_H;                           // 写端口拉高
	delay_ns(200);
	GPIOE -> ODR &= 0xC0FF;	          // 写入第一级滤波器的地址(对六个位全部写低)
	delay_ns(200);
	WR_L;                           //写端口拉低
	delay_ns(200);
// 	GPIOE->BRR = 0x0300;
	GPIOE -> ODR |= ((uint16_t)((mode-1) & 0x03) << 8);    // 将模式控制字送给D1, D0
	delay_ns(200);
	WR_H;                           // 写端口拉高
	delay_ns(200);	
	for(i = 0; i < 3; i++)
	{
		GPIOE -> ODR &= 0xC0FF;	          // (对六个位全部写低)
		GPIOE -> ODR |= (uint16_t)(i+1)<<10; // 写入地址(改变A位)
		delay_ns(200);
		WR_L;                             // 写入拉低
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;            //数据位清0
		GPIOE->ODR |= ((uint16_t)(sf & a) << (8-2*i));		  // 将f的控制字N写入(将8位频率控制字从低位开始依次送入D1\D0)
		delay_ns(200);                   
		WR_H;                             // 写入拉高
		a = a << 2;                       // a左移2位		
	}
	a = 0x03;
	for(i = 0; i < 4; i++)
	{
		GPIOE -> ODR &= 0xC0FF;	          // 将地址与数据位清0
		GPIOE -> ODR |= (uint16_t)(i+4)<<10;          // 写入地址
		delay_ns(200);
		WR_L;                             // 写入拉低
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;              //数据位清0
		GPIOE->ODR |= ((uint16_t)(sq & a) << (8-2*i));		  // 将Q的控制字N写入
		delay_ns(200);                   
		WR_H;                             // 写入拉高
		a = a << 2;                       // a左移2位		
	}
}

//-----------------------------------------------------------------
// void Filter2(uint8_t mode, float f, float q)
//-----------------------------------------------------------------
// 函数功能: 滤波器2的模式、频率以及Q值的设置
// 入口参数: 模式 mode, 截止/中心 频率 f, Q值 q
//-----------------------------------------------------------------
void Filter2(uint8_t mode, float q)
{
	uint8_t i=0,sf, sq;
	uint8_t a = 0x03;
	sf = Fn1();                     // 求出其频率控制字N
	sq = Qn1(q);                     // 求出Q对应的控制字N
	LE_H;                           // 使能端拉高
	delay_ns(200);
	WR_H;                           // 写端口拉高
	delay_ns(200);
	GPIOE -> ODR &= 0xC0FF;	          // 写入模式的地址(对六个位全部写低)(这里BRR作用是赋1归零,赋0不变)
	GPIOE -> ODR |= ((uint16_t)(i+8) << 10); 
	delay_ns(200);
	WR_L;                           //写端口拉低
	delay_ns(200);
// 	GPIOE->BRR = 0x0300;
	GPIOE -> ODR |= ((uint16_t)((mode-1) & 0x03) << 8);    // 将模式控制字送给D1, D0 (直接对这两个位赋值,其他位清零)
	delay_ns(200);
	WR_H;                           // 写端口拉高
	delay_ns(200);	
	for(i = 0; i < 3; i++)
	{
		GPIOE -> ODR &= 0xC0FF;	          // (对六个位全部写低)
		GPIOE -> ODR |= (uint16_t)(i+9)<<10; // 写入地址(改变A位)
		delay_ns(200);
		WR_L;                             // 写入拉低
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;            //数据位清0
		GPIOE->ODR |= ((uint16_t)(sf & a) << (8-2*i));		  // 将f的控制字N写入(将8位频率控制字从低位开始依次送入D1\D0)
		delay_ns(200);                   
		WR_H;                             // 写入拉高
		a = a << 2;                       // a左移2位		
	}
	a = 0x03;
	for(i = 0; i < 4; i++)
	{
		GPIOE -> ODR &= 0xC0FF;	          // 将地址与数据位清0
		GPIOE -> ODR |= (uint16_t)(i+12)<<10;          // 写入地址
		delay_ns(200);
		WR_L;                             // 写入拉低
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;              //数据位清0
		GPIOE->ODR |= ((uint16_t)(sq & a) << (8-2*i));		  // 将Q的控制字N写入
		delay_ns(200);                   
		WR_H;                             // 写入拉高
		a = a << 2;                       // a左移2位		
	}
}

void Filter3(uint8_t mode, float q)
{
	uint8_t i=0,sf, sq;
	uint8_t a = 0x03;
	sf = Fn1();                     // 求出其频率控制字N
	sq = Qn1(q);                     // 求出Q对应的控制字N
	LE_H_2;                           // 使能端拉高
	delay_ns(200);
	WR_H_2;                           // 写端口拉高
	delay_ns(200);
	GPIOG -> ODR &= 0xC0FF;	          // 写入模式的地址(对六个位全部写低)(这里BRR作用是赋1归零,赋0不变)
	GPIOG -> ODR |= ((uint16_t)(i+8) << 10); 
	delay_ns(200);
	WR_L_2;                           //写端口拉低
	delay_ns(200);
// 	GPIOE->BRR = 0x0300;
	GPIOG -> ODR |= ((uint16_t)((mode-1) & 0x03) << 8);    // 将模式控制字送给D1, D0 (直接对这两个位赋值,其他位清零)
	delay_ns(200);
	WR_H_2;                           // 写端口拉高
	delay_ns(200);	
	for(i = 0; i < 3; i++)
	{
		GPIOG -> ODR &= 0xC0FF;	          // (对六个位全部写低)
		GPIOG -> ODR |= (uint16_t)(i+9)<<10; // 写入地址(改变A位)
		delay_ns(200);
		WR_L_2;                             // 写入拉低
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;            //数据位清0
		GPIOG->ODR |= ((uint16_t)(sf & a) << (8-2*i));		  // 将f的控制字N写入(将8位频率控制字从低位开始依次送入D1\D0)
		delay_ns(200);                   
		WR_H_2;                             // 写入拉高
		a = a << 2;                       // a左移2位		
	}
	a = 0x03;
	for(i = 0; i < 4; i++)
	{
		GPIOG -> ODR &= 0xC0FF;	          // 将地址与数据位清0
		GPIOG -> ODR |= (uint16_t)(i+12)<<10;          // 写入地址
		delay_ns(200);
		WR_L;                             // 写入拉低
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;              //数据位清0
		GPIOG->ODR |= ((uint16_t)(sq & a) << (8-2*i));		  // 将Q的控制字N写入
		delay_ns(200);                   
		WR_H_2;                             // 写入拉高
		a = a << 2;                       // a左移2位		
	}
}

void Filter4(uint8_t mode, float q)
{
	uint8_t i=0,sf, sq;
	uint8_t a = 0x03;
	sf = Fn1();                     // 求出其频率控制字N
	sq = Qn1(q);                     // 求出Q对应的控制字N
	LE_H_2;                           // 使能端拉高
	delay_ns(200);
	WR_H_2;                           // 写端口拉高
	delay_ns(200);
	GPIOG -> ODR &= 0xC0FF;	          // 写入模式的地址(对六个位全部写低)(这里BRR作用是赋1归零,赋0不变)
	GPIOG -> ODR |= ((uint16_t)(i+8) << 10); 
	delay_ns(200);
	WR_L_2;                           //写端口拉低
	delay_ns(200);
// 	GPIOE->BRR = 0x0300;
	GPIOG -> ODR |= ((uint16_t)((mode-1) & 0x03) << 8);    // 将模式控制字送给D1, D0 (直接对这两个位赋值,其他位清零)
	delay_ns(200);
	WR_H_2;                           // 写端口拉高
	delay_ns(200);	
	for(i = 0; i < 3; i++)
	{
		GPIOG -> ODR &= 0xC0FF;	          // (对六个位全部写低)
		GPIOG -> ODR |= (uint16_t)(i+9)<<10; // 写入地址(改变A位)
		delay_ns(200);
		WR_L_2;                             // 写入拉低
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;            //数据位清0
		GPIOG->ODR |= ((uint16_t)(sf & a) << (8-2*i));		  // 将f的控制字N写入(将8位频率控制字从低位开始依次送入D1\D0)
		delay_ns(200);                   
		WR_H_2;                             // 写入拉高
		a = a << 2;                       // a左移2位		
	}
	a = 0x03;
	for(i = 0; i < 4; i++)
	{
		GPIOG -> ODR &= 0xC0FF;	          // 将地址与数据位清0
		GPIOG -> ODR |= (uint16_t)(i+12)<<10;          // 写入地址
		delay_ns(200);
		WR_L;                             // 写入拉低
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;              //数据位清0
		GPIOG->ODR |= ((uint16_t)(sq & a) << (8-2*i));		  // 将Q的控制字N写入
		delay_ns(200);                   
		WR_H_2;                             // 写入拉高
		a = a << 2;                       // a左移2位		
	}
}
Filter filter;
void MAX262_Init(void)
{
//    filter.mode=1;
//    filter.fc=10000;
//    filter.step=1000;
//    filter.buf=40;
    
	MAX262_GPIO_Init();                   // IO口初始化配置
	Filter1(3,Lp_Q_Value);                    // (mode)控制方式mode, 品质因数q 
	Filter2(3,Lp_Q_Value);                    // (mode)控制方式mode, 品质因数q 
	
	Filter3(3,Hp_Q_Value);                    // (mode)控制方式mode, 品质因数q ,高通
	Filter4(3,Hp_Q_Value);                    // (mode)控制方式mode, 品质因数q 
	
	delay_ms(300);	
	GPIOE -> ODR &= 0xC0FF;	
    
}




