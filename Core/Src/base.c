#include "base.h"

static u8 fac_us = 0; //us延时倍乘数


/*!
 *  \brief  初始化延迟函数
 *  \param  SYSCLK   系统时钟频率（如72Mhz，填写72）
 *  \note   SYSTICK的时钟固定为HCLK时钟的1/8
 */
void delay_init(u8 SYSCLK)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	fac_us=SYSCLK;	   
}	


/*!
 *  \brief  延迟ns
 *  \param  t ns数
 *  \note   delay_init后使用
 */
void delay_ns(u8 t) {
	do {
		;
	} while (--t);
}

/****************************************************************************
* 名    称: void delay_us(u32 nus)
* 功    能：延时nus
* 入口参数：要延时的微妙数
* 返回参数：无
* 说    明：nus的值,不要大于798915us
****************************************************************************/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};	 
}


//延时nms
//nms:要延时的ms数
void delay_ms(u16 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}

/*!
 *  \brief  按键扫描函数
 *  \param  mode 0不支持连按，1支持连按
 *  \retval 按键值
 *  \note   此函数有响应优先级,KEY0>KEY1
 */

//u8 KEY_Scan(u8 mode)
//{
//    static u8 key_up=1;     //按键松开标志
//		if(mode==1)key_up=1;    //支持连按
//		if(key_up&&(KEY0==0||KEY1==0))
//		{
//				delay_ms(20);
//        key_up=0;
//        if(KEY0==0)       return KEY0_PRES;
//        else if(KEY1==0)  return KEY1_PRES;
//    }else if(KEY0==1&&KEY1==1)key_up=1;
//    return 0;   //无按键按下
//}

