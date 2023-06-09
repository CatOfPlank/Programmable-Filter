#include "AD9959.H"
#include "filter.h"
//#include "Algorithm.h"

float Amp_Freq9V[61]={
1.033057851,1.033057851,1.034958602,1.036866359,1.038781163,1.040703053,1.042632067,1.044568245,1.046511628,1.048462255,1.050420168,1.052385407,1.054358013,1.056338028,1.058325494,1.060320452,1.062322946,1.064333018,1.066350711,1.068376068,1.072194425,1.076040172,1.079913607,1.083815029,1.087744743,1.091703057,1.093874304,1.096054205,1.098242812,1.100440176,1.102646351,1.10486139,1.107085346,1.109318273,1.111560226,1.11381126,1.116071429,1.120358515,1.124678663,1.129032258,1.133419689,1.137841352,1.14229765,1.146788991,1.150747986,1.154734411,1.158748552,1.162790698,1.166861144,1.170960187,1.175088132,1.179245283,1.185958254,1.192748092,1.199616123,1.206563707,1.213592233,1.222493888,1.231527094,1.240694789,1.25
};
float Amp_Freq5V[61]={
1,1,1.001335113,1.002673797,1.004016064,1.00536193,1.006711409,1.008064516,1.009421265,1.010781671,1.012145749,1.013513514,1.01488498,1.016260163,1.017639077,1.019021739,1.020408163,1.02145046,1.022494888,1.023541453,1.024590164,1.025641026,1.026694045,1.027749229,1.028806584,1.029866117,1.030927835,1.035196687,1.03950104,1.043841336,1.048218029,1.052631579,1.057082452,1.061571125,1.066098081,1.070663812,1.075268817,1.078748652,1.082251082,1.08577633,1.089324619,1.092896175,1.096491228,1.100110011,1.103752759,1.107419712,1.111111111,1.114827202,1.118568233,1.122334456,1.126126126,1.129943503,1.133786848,1.137656428,1.141552511,1.145475372,1.149425287,1.15942029,1.169590643,1.179941003,1.19047619
};

sweep_freq sweepfreq;
extern my_Filter my_Filter1;
//u32 SinFre[5] = {50, 50, 50, 50};
//u32 SinAmp[5] = {1023, 1023, 1023, 1023};
//u32 SinPhr[5] = {0, 4095, 4095*3, 4095*2};

u8 CSR_DATA0[1] = {0x10};       // 开 CH0
u8 CSR_DATA1[1] = {0x20};       // 开 CH1
u8 CSR_DATA2[1] = {0x40};       // 开 CH2
u8 CSR_DATA3[1] = {0x80};       // 开 CH3		
																	
u8 FR2_DATA[2] = {0x00,0x00};       //default Value = 0x0000
u8 CFR_DATA[3] = {0x00,0x03,0x02};  //default Value = 0x000302	   
																	
u8 CPOW0_DATA[2] = {0x00,0x00};     //default Value = 0x0000   @ = POW/2^14*360
																	
u8 LSRR_DATA[2] = {0x00,0x00};      //default Value = 0x----
																	
u8 RDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------
																	
u8 FDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------


//延时
void dds_delay(u32 length)
{
    length = length*12;
    while(length--);
}

//IO口初始化
void Intserve(void)		   
{   
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();        
    __HAL_RCC_GPIOB_CLK_ENABLE();        
    __HAL_RCC_GPIOE_CLK_ENABLE();        
    __HAL_RCC_GPIOH_CLK_ENABLE();        
    __HAL_RCC_GPIOG_CLK_ENABLE();        
	
    GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_5; 
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_12; 
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6; 
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_3; 
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_6|GPIO_PIN_7; 
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    AD9959_PWR=0;
    AD9959_CS = 1;
    AD9959_SCLK = 0;
    AD9959_UPDATE = 0;
    AD9959_PS0 = 0;
    AD9959_PS1 = 0;
    AD9959_PS2 = 0;
    AD9959_PS3 = 0;
    AD9959_SDIO0 = 0;
    AD9959_SDIO1 = 0;
    AD9959_SDIO2 = 0;
    AD9959_SDIO3 = 0;
}
//AD9959复位
void IntReset(void)	  
{
    AD9959_Reset = 0;
    dds_delay(1);
    AD9959_Reset = 1;
    dds_delay(30);
    AD9959_Reset = 0;
}
 //AD9959更新数据
void IO_Update(void)  
{
	AD9959_UPDATE = 0;
	dds_delay(2);
	AD9959_UPDATE = 1;
	dds_delay(4);
	AD9959_UPDATE = 0;
}

/*--------------------------------------------
函数功能：控制器通过SPI向AD9959写数据
RegisterAddress: 寄存器地址
NumberofRegisters: 所含字节数
*RegisterData: 数据起始地址
temp: 是否更新IO寄存器
----------------------------------------------*/
void WriteData_AD9959(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp)
{
	u8	ControlValue = 0;
	u8	ValueToWrite = 0;
	u8	RegisterIndex = 0;
	u8	i = 0;

	ControlValue = RegisterAddress;
    //写入地址
	AD9959_SCLK = 0;
	AD9959_CS = 0;	 
	for(i=0; i<8; i++)
	{
		AD9959_SCLK = 0;
		if(0x80 == (ControlValue & 0x80))
		AD9959_SDIO0= 1;	  
		else
		AD9959_SDIO0= 0;	  
		AD9959_SCLK = 1;
		ControlValue <<= 1;
	}
	AD9959_SCLK = 0;
    //写入数据
	for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i=0; i<8; i++)
		{
			AD9959_SCLK = 0;
			if(0x80 == (ValueToWrite & 0x80))
			AD9959_SDIO0= 1;	  
			else
			AD9959_SDIO0= 0;	  
			AD9959_SCLK = 1;
			ValueToWrite <<= 1;
		}
		AD9959_SCLK = 0;		
	}	
	if(temp==1)
	IO_Update();	
    AD9959_CS = 1;
} 

/*---------------------------------------
函数功能：设置通道输出频率
Channel:  输出通道
Freq:     输出频率
---------------------------------------*/
void Write_frequence(u8 Channel,u32 Freq)
{	 
	  u8 CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//中间变量
	  u32 Temp;            
	  Temp=(u32)Freq*8.589934592;	   //将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	  CFTW0_DATA[3]=(u8)Temp;
	  CFTW0_DATA[2]=(u8)(Temp>>8);
	  CFTW0_DATA[1]=(u8)(Temp>>16);
	  CFTW0_DATA[0]=(u8)(Temp>>24);
	  if(Channel==0)	  
	  {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA0,1);//控制寄存器写入CH0通道
        WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH0设定频率
	  }
	  else if(Channel==1)	
	  {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA1,1);//控制寄存器写入CH1通道
        WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH1设定频率	
	  }
	  else if(Channel==2)	
	  {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA2,1);//控制寄存器写入CH2通道
        WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH2设定频率	
	  }
	  else if(Channel==3)	
	  {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA3,1);//控制寄存器写入CH3通道
        WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,3);//CTW0 address 0x04.输出CH3设定频率	
	  }																																																																										 
} 
/*---------------------------------------
函数功能：设置通道输出幅度
Channel:  输出通道
Ampli:    输出幅度
---------------------------------------*/
void Write_Amplitude(u8 Channel, u16 Ampli)
{ 
    u16 A_temp;//=0x23ff;
    u8 ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 

    A_temp=Ampli|0x1000;
    ACR_DATA[2] = (u8)A_temp;  //低位数据
    ACR_DATA[1] = (u8)(A_temp>>8); //高位数据
    if(Channel==0)
    {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA0,1); 
        WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
    }
    else if(Channel==1)
    {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA1,1); 
        WriteData_AD9959(ACR_ADD,3,ACR_DATA,1);
    }
    else if(Channel==2)
    {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA2,1); 
        WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
    }
    else if(Channel==3)
    {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA3,1); 
        WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
    }
}

/*---------------------------------------
函数功能：设置通道输出相位
Channel:  输出通道
Phase:    输出相位,范围：0~16383(对应角度：0°~360°)
---------------------------------------*/
void Write_Phase(u8 Channel,u16 Phase)
{
    u16 P_temp=0;
    P_temp=(u16)Phase;
    CPOW0_DATA[1]=(u8)P_temp;
    CPOW0_DATA[0]=(u8)(P_temp>>8);
    if(Channel==0)
    {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA0,0); 
        WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
    }
    else if(Channel==1)
    {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA1,0); 
        WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
    }
    else if(Channel==2)
    {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA2,0); 
        WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
    }
    else if(Channel==3)
    {
        WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0); 
        WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
    }
}	 


//AD9959初始化
void Init_AD9959(void)  
{    
 	u8 FR1_DATA[3] = {0xD0,0x00,0x00};//20倍频 Charge pump control = 75uA FR1<23> -- VCO gain control =0时 system clock below 160 MHz;

    Intserve();  //IO口初始化
    IntReset();  //AD9959复位  
	
    WriteData_AD9959(FR1_ADD,3,FR1_DATA,1);//写功能寄存器1
    //  WriteData_AD9959(FR2_ADD,2,FR2_DATA,0);
    //  WriteData_AD9959(CFR_ADD,3,CFR_DATA,1);
    //  WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
    //  WriteData_AD9959(ACR_ADD,3,ACR_DATA,0);
    //  WriteData_AD9959(LSRR_ADD,2,LSRR_DATA,0);
    //  WriteData_AD9959(RDW_ADD,2,RDW_DATA,0);
    //  WriteData_AD9959(FDW_ADD,4,FDW_DATA,1);
    //写入初始频率
//    sweepfreq.flag=0;
//    sweepfreq.start=0;
//    sweepfreq.step=10000;
//    sweepfreq.time=500;
//    sweepfreq.end=200000;

//    sinwave.freq = 100000;
//    sinwave.phase = 0;
//    sinwave.amp = 500;
//    
//	Out_freq(0,sinwave.freq); 
//	Write_Phase(0, sinwave.phase);   
//	Out_mV(0, sinwave.amp);
//    
//    HAL_Delay(300);
} 

void Out_mV(u8 ch, float nmV)
{
    //nmV=nmV/590*1024;
    nmV=nmV/500*1024;   //阻抗匹配会衰减一半，所以这里多输出一倍
    if(nmV>1023)nmV=1023;
    Write_Amplitude(ch,nmV);
    //Write_Amplitude(ch,nmV*Amp_Freq5V[(u32)sinwave.freq/1000000]);
}

void Out_freq(u8 ch, u32 freq,float amp)
{
    static float freq_before=100; 
    if(freq>FreqMax)freq=FreqMax;
    if(freq<FreqMin)freq=FreqMin;
	Write_Amplitude(ch,amp);
    Write_frequence(ch,freq);
//    if(freq_before != sinwave.freq) //如果频率变化则也触发幅度变化
//    {
//        Out_mV(ch, 500);
//        freq_before=sinwave.freq;
//    }-	
}



