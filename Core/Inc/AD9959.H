#ifndef _AD9959_H_
#define _AD9959_H_
#include "base.h"
#include "stdint.h"

//AD9959寄存器地址定义
#define CSR_ADD   0x00   //CSR 通道选择寄存器
#define FR1_ADD   0x01   //FR1 功能寄存器1
#define FR2_ADD   0x02   //FR2 功能寄存器2
#define CFR_ADD   0x03   //CFR 通道功能寄存器
#define CFTW0_ADD 0x04   //CTW0 通道频率转换字寄存器
#define CPOW0_ADD 0x05   //CPW0 通道相位转换字寄存器
#define ACR_ADD   0x06   //ACR 幅度控制寄存器
#define LSRR_ADD  0x07   //LSR 通道线性扫描寄存器
#define RDW_ADD   0x08   //RDW 通道线性向上扫描寄存器
#define FDW_ADD   0x09   //FDW 通道线性向下扫描寄存器

//AD9959管脚宏定义
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


typedef struct _sweep_freq   //正弦波形结构体
{
    u32 flag;   //是否使能
    u32 start;  //起始Hz    
    u32 step;   //步进Hz  
    u32 time;   //单次停留时间   
    u32 end;    //终点Hz
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
