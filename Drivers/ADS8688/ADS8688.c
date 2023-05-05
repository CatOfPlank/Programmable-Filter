#include "ADS8688.H"
#include "filter.h"

void ADS8688_SPI_WB(uint8_t com)
{
  uint8_t com_temp=com;
  ADS_8688_nCS_L;
  for(uint8_t s=0;s<8;s++)
  {
    if(com_temp&0x80)
    {
      ADS_8688_SDI_H;
    }
    else
    {
      ADS_8688_SDI_L;
    }
    ADS_8688_SCLK_H;
    com_temp<<=1;
    ADS_8688_SCLK_L;
  }
}
uint8_t ADS8688_SPI_RB(void)
{
  uint8_t Rdata=0;
  //ADS_8688_nCS_L;
  for(uint8_t s=0;s<8;s++)
  {
    Rdata<<=1;
    ADS_8688_SCLK_H;
    if(ADS_8688_SDO)
    {
      Rdata|=0x01;
    }
    else
    {
      Rdata&=0xFE;
    }
    ADS_8688_SCLK_L;
  }
  return Rdata;
}


void ADS8688_WriteCommandReg(uint16_t command)//写ADS8688命令寄存器
{
  ADS_8688_nCS_L;
  ADS8688_SPI_WB(command>>8 & 0XFF);
  ADS8688_SPI_WB(command & 0XFF);
  ADS_8688_nCS_H;
}
void ADS8688_Write_Program_Register(uint8_t Addr,uint8_t data)
{
  ADS_8688_nCS_L;
  ADS8688_SPI_WB(Addr<<1| 0X01);
  ADS8688_SPI_WB(data);
  ADS_8688_nCS_H;
}
u8 ADS8688_READ_Program_Register(uint8_t Addr)
{
  u8 data = 0;
  ADS_8688_nCS_L;
  ADS8688_SPI_WB(Addr<<1);
  data = ADS8688_SPI_RB();
  data = ADS8688_SPI_RB();
  ADS_8688_nCS_H;
  return data;
}

void ADS_Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


void Enter_RESET_MODE(void)//软件复位模式，复位 program registers
{
  ADS8688_WriteCommandReg(RST);
}
void AUTO_RST_Mode(void)//进入自动扫描模式
{
  ADS8688_WriteCommandReg(AUTO_RST);
}
void MAN_Ch_n_Mode(uint16_t ch)//手动模式
{
  ADS8688_WriteCommandReg(ch);	
}
void Set_CH_Range_Select(uint8_t ch,uint8_t range) //设置各个通道的范围
{
  ADS8688_Write_Program_Register(ch,range);
}
void Get_AUTO_RST_Mode_Data(uint16_t* outputdata, uint8_t chnum)
{
  //读取扫描通道序列的AD转换数据code到变量数组中
  u8 i=0,datal=0,datah=0;
  u16 data=0;
  for (i=0; i<chnum; i++)
  {
    ADS_8688_nCS_L;
    ADS8688_SPI_WB(0X00);
    ADS8688_SPI_WB(0X00);
    datah = ADS8688_SPI_RB();;
    datal = ADS8688_SPI_RB();;
    ADS_8688_nCS_H;
    data = datah<<8 | datal;	//高位在前，低位在后
    *(outputdata+i) = data;
  }
}
uint16_t Get_MAN_Ch_n_Mode_Data(void)
{
  u8 datah=0,datal=0;
  ADS_8688_nCS_L;
  ADS8688_SPI_WB(0X00);
  ADS8688_SPI_WB(0X00);
  datah = ADS8688_SPI_RB();
  datal = ADS8688_SPI_RB();
  ADS_8688_nCS_H;
  return (datah<<8 | datal);
}

void ADS8688_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOD_CLK_ENABLE();      
    __HAL_RCC_GPIOE_CLK_ENABLE();      
    __HAL_RCC_GPIOG_CLK_ENABLE();      
    __HAL_RCC_GPIOH_CLK_ENABLE();      
    __HAL_RCC_GPIOI_CLK_ENABLE();      
	
    GPIO_Initure.Pin=GPIO_PIN_13; 
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_0; 
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_6; 
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_9; 
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_3; 
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_7; 
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);
}

//初始化通道1，默认正负10V范围
void ADS8688_Init_Single()
{
  ADS_8688_RST_PD_L;
  ADS_Delay(0x1fff);
  ADS_8688_RST_PD_H;
  ADS_8688_DAISY_IN_L;
  Enter_RESET_MODE();
  ADS8688_Write_Program_Register(0X01,0XFF);
  ADS8688_READ_Program_Register(0X01);
  ADS_Delay(0x1fff);
  ADS8688_Write_Program_Register(0x02,0x00);//所有通道退出低功耗状态
  ADS8688_Write_Program_Register(0x01,0xff);//使能两个通道   
  Set_CH_Range_Select(CH1,0x00);//设置通道1的输入范围：+-2.5*Vref
//    //0x00 -> +-2.5*ref
//    //0x01 -> +-1.25*ref
//    //0x02 -> +-0.625*ref
//    //0x03 -> +2.5*ref
//    //0x04 -> +1.25*ref
  MAN_Ch_n_Mode(MAN_Ch_1);
  HAL_Delay(300);
}
u16 get_ADS_ch1(void)
{
    return Get_MAN_Ch_n_Mode_Data();//读取通道1数据,具体通道数由函数MAN_Ch_n_Mode()所决定的
}


//初始化多通道，默认全开、正负10V范围
void ADS8688_Init_Mult(void)
{
    
	ADS_8688_RST_PD_H;
  ADS_8688_DAISY_IN_L;
  delay_us(2);
  Enter_RESET_MODE();
  ADS8688_Write_Program_Register(0X01,0XFF);
  ADS8688_READ_Program_Register(0X01);
  delay_us(2);
  ADS8688_Write_Program_Register(0x02,0x00);//所有通道退出低功耗状态
  ADS8688_Write_Program_Register(0x01,0x03);//使能所有通道
	
	Set_CH_Range_Select(CH1,0x00);//设置通道1、2、3、4的输入范围：+-2.5*Vref 
	Set_CH_Range_Select(CH2,0x00); 
//	Set_CH_Range_Select(CH3,0x00);
//	Set_CH_Range_Select(CH4,0x00);
//	Set_CH_Range_Select(CH5,0x00);
//	Set_CH_Range_Select(CH6,0x00);
	/*
	Set_CH_Range_Select(CH2,0x00);
	Set_CH_Range_Select(CH3,0x00);
	Set_CH_Range_Select(CH4,0x00); 
	Set_CH_Range_Select(CH5,0x00); 
	Set_CH_Range_Select(CH6,0x00); 
	Set_CH_Range_Select(CH7,0x00); 
	
	*/
	// //0x00 -> +-2. 5*ref 
	// //0x01 -> +-1.25*ref 
	// //0x02 -> +-0.625*ref 
	// //0x03 -> +2.5*ref 
	// //0x04 -> +1.25*ref 
	//AUTO_RST_Mode();//进入自动扫描模式 
//	ADS8688_Write_Program_Register(0xa0,0x03);
//	delay_us(10);
  AUTO_RST_Mode();
}
static u16 ADS_Results[8]={0,};
void get_ADS_allch(float*result)
{
   Get_AUTO_RST_Mode_Data(ADS_Results,8);//自动扫描模式，自动扫描并转换8通道。转换数据存与Value数组中

   *result =  (ADS_Results[1-1]-ADS8688_ZERO)*AD_mV_Scale;
    result += AD_N;
    *result = (ADS_Results[2-1]-ADS8688_ZERO)*AD_mV_Scale;
    result += AD_N;
    *result = (ADS_Results[3-1]-ADS8688_ZERO)*AD_mV_Scale;
    result += AD_N;
    *result = (ADS_Results[4-1]-ADS8688_ZERO)*AD_mV_Scale;
    result += AD_N;
    *result = (ADS_Results[5-1]-ADS8688_ZERO)*AD_mV_Scale;
    result += AD_N;
    *result = (ADS_Results[6-1]-ADS8688_ZERO)*AD_mV_Scale;

}
