#ifndef _MAX262_H_
#define _MAX262_H_

#include "base.h"

#define FCLK_FC 139.8f    //时钟频率与截止频率的比值
//比值要控制在40.84-139.8之间,越大输出波形越优美,
//但要满足fclk不能大于4MHz即最大fc小于28KHz
//通过改PWM波频率修改截止频率
//时钟PWM波的频率除以139.8即为截止频率


#define	WR_L	    PEout(7)=0
#define	WR_H	    PEout(7)=1
#define	D0			PEout(8)
#define	D1		    PEout(9)
#define	A0			PEout(10)
#define	A1		    PEout(11)
#define	A2			PEout(12)
#define	A3		    PEout(13)
#define	LE_L	    PEout(14)=0
#define	LE_H	    PEout(14)=1

#define	WR_L_2	    PGout(15)=0
#define	WR_H_2	    PGout(15)=1
#define	D0_2		PGout(8)
#define	D1_2		PGout(9)
#define	A0_2		PGout(10)
#define	A1_2		PGout(11)
#define	A2_2		PGout(12)
#define	A3_2		PGout(13)
#define	LE_L_2	    PGout(14)=0
#define	LE_H_2	    PGout(14)=1

/*  滤波器参数  */
typedef struct _Filter  
{
    u16 mode;       //滤波方式： 1-低通；2-高通
    float fc;       //截止频率： （Hz，默认10KHz）
    float step;     //频率调节步进：（Hz，默认1KHz）
    float buf;      //备用：此处表示电压增益 dB
}Filter;
extern Filter filter;


void MAX262_Init(void); //初始化函数
void MAX262_GPIO_Init(void);


#endif
