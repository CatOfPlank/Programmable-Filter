#ifndef _MAX262_H_
#define _MAX262_H_

#include "base.h"

#define FCLK_FC 139.8f    //ʱ��Ƶ�����ֹƵ�ʵı�ֵ
//��ֵҪ������40.84-139.8֮��,Խ���������Խ����,
//��Ҫ����fclk���ܴ���4MHz�����fcС��28KHz
//ͨ����PWM��Ƶ���޸Ľ�ֹƵ��
//ʱ��PWM����Ƶ�ʳ���139.8��Ϊ��ֹƵ��


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

/*  �˲�������  */
typedef struct _Filter  
{
    u16 mode;       //�˲���ʽ�� 1-��ͨ��2-��ͨ
    float fc;       //��ֹƵ�ʣ� ��Hz��Ĭ��10KHz��
    float step;     //Ƶ�ʵ��ڲ�������Hz��Ĭ��1KHz��
    float buf;      //���ã��˴���ʾ��ѹ���� dB
}Filter;
extern Filter filter;


void MAX262_Init(void); //��ʼ������
void MAX262_GPIO_Init(void);


#endif