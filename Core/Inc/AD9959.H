#ifndef _AD9959_H_
#define _AD9959_H_
#include "base.h"
#include "stdint.h"

//AD9959�Ĵ�����ַ����
#define CSR_ADD   0x00   //CSR ͨ��ѡ��Ĵ���
#define FR1_ADD   0x01   //FR1 ���ܼĴ���1
#define FR2_ADD   0x02   //FR2 ���ܼĴ���2
#define CFR_ADD   0x03   //CFR ͨ�����ܼĴ���
#define CFTW0_ADD 0x04   //CTW0 ͨ��Ƶ��ת���ּĴ���
#define CPOW0_ADD 0x05   //CPW0 ͨ����λת���ּĴ���
#define ACR_ADD   0x06   //ACR ���ȿ��ƼĴ���
#define LSRR_ADD  0x07   //LSR ͨ������ɨ��Ĵ���
#define RDW_ADD   0x08   //RDW ͨ����������ɨ��Ĵ���
#define FDW_ADD   0x09   //FDW ͨ����������ɨ��Ĵ���

//AD9959�ܽź궨��
#define AD9959_PS0			PFout(8)
#define AD9959_PS1			PFout(6)
#define AD9959_PS2			PFout(4)
#define AD9959_PS3			PFout(2)
#define AD9959_SDIO0		PFout(0)
#define AD9959_SDIO1		PEout(5)
#define AD9959_SDIO2		PEout(4)

//define GND
#define AD9959_PWR	        PFout(9)
#define AD9959_Reset		PFout(7)
#define AD9959_UPDATE	    PFout(5)
#define AD9959_CS			PFout(3)
#define AD9959_SCLK		    PFout(1)
#define AD9959_SDIO3		PEout(6)

#define FreqMax 200000000.0f
#define FreqMin 50.0f
//#define BaseV 300.0f

//extern float Amp_Freq[61];


typedef struct _sweep_freq   //���Ҳ��νṹ��
{
    u32 flag;   //�Ƿ�ʹ��
    u32 start;  //��ʼHz    
    u32 step;   //����Hz  
    u32 time;   //����ͣ��ʱ��   
    u32 end;    //�յ�Hz
}sweep_freq;
extern sweep_freq sweepfreq;

void Out_freq(u8 ch, u32 freq,float amp);
void Out_mV(u8 ch, float nmV);

void Init_AD9959(void);
//void Write_frequence(u8 Channel,u32 Freq);
//void Write_Amplitude(u8 Channel, u16 Ampli);
//void Write_Phase(u8 Channel,u16 Phase);
void Sweep_Freq(void);
















#endif