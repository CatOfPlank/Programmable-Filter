#include "MAX262.H"
#include "head_define.h"
extern float Lp_Q_Value,Hp_Q_Value;
/*
MAX262��˫���׿��ص�����Դ�˲���,ÿ�����׶��ܵ�������,����ϳ�2��\4�׵�ͨ\��ͨ\��ͨ\����\ȫͨ�˲���
����5V����

*/

void MAX262_GPIO_Init(void)               // MAX262��������
{
	//��һ��MAX262 PE7-PE14
	GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOE_CLK_ENABLE();      	
    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14; 
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_NOPULL;         
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	//�ڶ���MAX262 PG7-PG14
    __HAL_RCC_GPIOG_CLK_ENABLE();      	
    GPIO_Initure.Pin=GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14; 
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_NOPULL;         
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);

}

//-----------------------------------------------------------------
// u8 Fn(float f)                     
//-----------------------------------------------------------------
// ��������: ����f��Ƶ�ʿ�����N
// ��ڲ���: ��ֹƵ��(����Ƶ��) fc
// �� �� ֵ: ������N
//-----------------------------------------------------------------
//����ģʽ1,3,4
uint8_t Fn1()           
{
    //return fclk/fc*2/PI-26;         //ԭ����*2/pi;��*0.637  fclk��M��λ��f��λkhz
	return FCLK_FC*0.63661977f-26;    //=63

}
//����ģʽ2
uint8_t Fn2()                      //Ƶ�ʹؼ���
{
    //return (fclk2*637)-26;         //ԭ����*2/pi;��*0.637  fclk��M��λ��f��λkhz
	//return fclk/fc/1.11072-26;
	return FCLK_FC*0.90031691f-26;
}

//-----------------------------------------------------------------
// u8 Qn(float q)                     
//-----------------------------------------------------------------
// ��������: ����Qֵ�Ŀ�����N
// ��ڲ���: Qֵ
// �� �� ֵ: ������N
//-----------------------------------------------------------------
//����ģʽ1,3,4
uint8_t Qn1(float q)                 
{
    return 128-(64/q);  //�����оƬ�ֲṫʽ
}
//����ģʽ2
uint8_t Qn2(float q)                 
{
    return 128-(90.51f/q);  //�����оƬ�ֲṫʽ
}


//-----------------------------------------------------------------
// void Filter1(uint8_t mode, float q)
//-----------------------------------------------------------------
// ��������: �˲�����ģʽ��Ƶ���Լ�Qֵ������
// ��ڲ���: ģʽ mode, Qֵ q
//-----------------------------------------------------------------
void Filter1(uint8_t mode, float q)
{
	uint8_t i=0,sf, sq;
	uint8_t a = 0x03;
	sf = Fn1();                     // �����Ƶ�ʿ�����N
	sq = Qn1(q);                     // ���Q��Ӧ�Ŀ�����N
	LE_H;                           // ʹ�ܶ�����
	delay_ns(200);
	WR_H;                           // д�˿�����
	delay_ns(200);
	GPIOE -> ODR &= 0xC0FF;	          // д���һ���˲����ĵ�ַ(������λȫ��д��)
	delay_ns(200);
	WR_L;                           //д�˿�����
	delay_ns(200);
// 	GPIOE->BRR = 0x0300;
	GPIOE -> ODR |= ((uint16_t)((mode-1) & 0x03) << 8);    // ��ģʽ�������͸�D1, D0
	delay_ns(200);
	WR_H;                           // д�˿�����
	delay_ns(200);	
	for(i = 0; i < 3; i++)
	{
		GPIOE -> ODR &= 0xC0FF;	          // (������λȫ��д��)
		GPIOE -> ODR |= (uint16_t)(i+1)<<10; // д���ַ(�ı�Aλ)
		delay_ns(200);
		WR_L;                             // д������
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;            //����λ��0
		GPIOE->ODR |= ((uint16_t)(sf & a) << (8-2*i));		  // ��f�Ŀ�����Nд��(��8λƵ�ʿ����ִӵ�λ��ʼ��������D1\D0)
		delay_ns(200);                   
		WR_H;                             // д������
		a = a << 2;                       // a����2λ		
	}
	a = 0x03;
	for(i = 0; i < 4; i++)
	{
		GPIOE -> ODR &= 0xC0FF;	          // ����ַ������λ��0
		GPIOE -> ODR |= (uint16_t)(i+4)<<10;          // д���ַ
		delay_ns(200);
		WR_L;                             // д������
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;              //����λ��0
		GPIOE->ODR |= ((uint16_t)(sq & a) << (8-2*i));		  // ��Q�Ŀ�����Nд��
		delay_ns(200);                   
		WR_H;                             // д������
		a = a << 2;                       // a����2λ		
	}
}

//-----------------------------------------------------------------
// void Filter2(uint8_t mode, float f, float q)
//-----------------------------------------------------------------
// ��������: �˲���2��ģʽ��Ƶ���Լ�Qֵ������
// ��ڲ���: ģʽ mode, ��ֹ/���� Ƶ�� f, Qֵ q
//-----------------------------------------------------------------
void Filter2(uint8_t mode, float q)
{
	uint8_t i=0,sf, sq;
	uint8_t a = 0x03;
	sf = Fn1();                     // �����Ƶ�ʿ�����N
	sq = Qn1(q);                     // ���Q��Ӧ�Ŀ�����N
	LE_H;                           // ʹ�ܶ�����
	delay_ns(200);
	WR_H;                           // д�˿�����
	delay_ns(200);
	GPIOE -> ODR &= 0xC0FF;	          // д��ģʽ�ĵ�ַ(������λȫ��д��)(����BRR�����Ǹ�1����,��0����)
	GPIOE -> ODR |= ((uint16_t)(i+8) << 10); 
	delay_ns(200);
	WR_L;                           //д�˿�����
	delay_ns(200);
// 	GPIOE->BRR = 0x0300;
	GPIOE -> ODR |= ((uint16_t)((mode-1) & 0x03) << 8);    // ��ģʽ�������͸�D1, D0 (ֱ�Ӷ�������λ��ֵ,����λ����)
	delay_ns(200);
	WR_H;                           // д�˿�����
	delay_ns(200);	
	for(i = 0; i < 3; i++)
	{
		GPIOE -> ODR &= 0xC0FF;	          // (������λȫ��д��)
		GPIOE -> ODR |= (uint16_t)(i+9)<<10; // д���ַ(�ı�Aλ)
		delay_ns(200);
		WR_L;                             // д������
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;            //����λ��0
		GPIOE->ODR |= ((uint16_t)(sf & a) << (8-2*i));		  // ��f�Ŀ�����Nд��(��8λƵ�ʿ����ִӵ�λ��ʼ��������D1\D0)
		delay_ns(200);                   
		WR_H;                             // д������
		a = a << 2;                       // a����2λ		
	}
	a = 0x03;
	for(i = 0; i < 4; i++)
	{
		GPIOE -> ODR &= 0xC0FF;	          // ����ַ������λ��0
		GPIOE -> ODR |= (uint16_t)(i+12)<<10;          // д���ַ
		delay_ns(200);
		WR_L;                             // д������
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;              //����λ��0
		GPIOE->ODR |= ((uint16_t)(sq & a) << (8-2*i));		  // ��Q�Ŀ�����Nд��
		delay_ns(200);                   
		WR_H;                             // д������
		a = a << 2;                       // a����2λ		
	}
}

void Filter3(uint8_t mode, float q)
{
	uint8_t i=0,sf, sq;
	uint8_t a = 0x03;
	sf = Fn1();                     // �����Ƶ�ʿ�����N
	sq = Qn1(q);                     // ���Q��Ӧ�Ŀ�����N
	LE_H_2;                           // ʹ�ܶ�����
	delay_ns(200);
	WR_H_2;                           // д�˿�����
	delay_ns(200);
	GPIOG -> ODR &= 0xC0FF;	          // д��ģʽ�ĵ�ַ(������λȫ��д��)(����BRR�����Ǹ�1����,��0����)
	GPIOG -> ODR |= ((uint16_t)(i+8) << 10); 
	delay_ns(200);
	WR_L_2;                           //д�˿�����
	delay_ns(200);
// 	GPIOE->BRR = 0x0300;
	GPIOG -> ODR |= ((uint16_t)((mode-1) & 0x03) << 8);    // ��ģʽ�������͸�D1, D0 (ֱ�Ӷ�������λ��ֵ,����λ����)
	delay_ns(200);
	WR_H_2;                           // д�˿�����
	delay_ns(200);	
	for(i = 0; i < 3; i++)
	{
		GPIOG -> ODR &= 0xC0FF;	          // (������λȫ��д��)
		GPIOG -> ODR |= (uint16_t)(i+9)<<10; // д���ַ(�ı�Aλ)
		delay_ns(200);
		WR_L_2;                             // д������
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;            //����λ��0
		GPIOG->ODR |= ((uint16_t)(sf & a) << (8-2*i));		  // ��f�Ŀ�����Nд��(��8λƵ�ʿ����ִӵ�λ��ʼ��������D1\D0)
		delay_ns(200);                   
		WR_H_2;                             // д������
		a = a << 2;                       // a����2λ		
	}
	a = 0x03;
	for(i = 0; i < 4; i++)
	{
		GPIOG -> ODR &= 0xC0FF;	          // ����ַ������λ��0
		GPIOG -> ODR |= (uint16_t)(i+12)<<10;          // д���ַ
		delay_ns(200);
		WR_L;                             // д������
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;              //����λ��0
		GPIOG->ODR |= ((uint16_t)(sq & a) << (8-2*i));		  // ��Q�Ŀ�����Nд��
		delay_ns(200);                   
		WR_H_2;                             // д������
		a = a << 2;                       // a����2λ		
	}
}

void Filter4(uint8_t mode, float q)
{
	uint8_t i=0,sf, sq;
	uint8_t a = 0x03;
	sf = Fn1();                     // �����Ƶ�ʿ�����N
	sq = Qn1(q);                     // ���Q��Ӧ�Ŀ�����N
	LE_H_2;                           // ʹ�ܶ�����
	delay_ns(200);
	WR_H_2;                           // д�˿�����
	delay_ns(200);
	GPIOG -> ODR &= 0xC0FF;	          // д��ģʽ�ĵ�ַ(������λȫ��д��)(����BRR�����Ǹ�1����,��0����)
	GPIOG -> ODR |= ((uint16_t)(i+8) << 10); 
	delay_ns(200);
	WR_L_2;                           //д�˿�����
	delay_ns(200);
// 	GPIOE->BRR = 0x0300;
	GPIOG -> ODR |= ((uint16_t)((mode-1) & 0x03) << 8);    // ��ģʽ�������͸�D1, D0 (ֱ�Ӷ�������λ��ֵ,����λ����)
	delay_ns(200);
	WR_H_2;                           // д�˿�����
	delay_ns(200);	
	for(i = 0; i < 3; i++)
	{
		GPIOG -> ODR &= 0xC0FF;	          // (������λȫ��д��)
		GPIOG -> ODR |= (uint16_t)(i+9)<<10; // д���ַ(�ı�Aλ)
		delay_ns(200);
		WR_L_2;                             // д������
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;            //����λ��0
		GPIOG->ODR |= ((uint16_t)(sf & a) << (8-2*i));		  // ��f�Ŀ�����Nд��(��8λƵ�ʿ����ִӵ�λ��ʼ��������D1\D0)
		delay_ns(200);                   
		WR_H_2;                             // д������
		a = a << 2;                       // a����2λ		
	}
	a = 0x03;
	for(i = 0; i < 4; i++)
	{
		GPIOG -> ODR &= 0xC0FF;	          // ����ַ������λ��0
		GPIOG -> ODR |= (uint16_t)(i+12)<<10;          // д���ַ
		delay_ns(200);
		WR_L;                             // д������
		delay_ns(200);
// 		GPIOE->BRR = 0x0300;              //����λ��0
		GPIOG->ODR |= ((uint16_t)(sq & a) << (8-2*i));		  // ��Q�Ŀ�����Nд��
		delay_ns(200);                   
		WR_H_2;                             // д������
		a = a << 2;                       // a����2λ		
	}
}
Filter filter;
void MAX262_Init(void)
{
//    filter.mode=1;
//    filter.fc=10000;
//    filter.step=1000;
//    filter.buf=40;
    
	MAX262_GPIO_Init();                   // IO�ڳ�ʼ������
	Filter1(3,Lp_Q_Value);                    // (mode)���Ʒ�ʽmode, Ʒ������q 
	Filter2(3,Lp_Q_Value);                    // (mode)���Ʒ�ʽmode, Ʒ������q 
	
	Filter3(3,Hp_Q_Value);                    // (mode)���Ʒ�ʽmode, Ʒ������q ,��ͨ
	Filter4(3,Hp_Q_Value);                    // (mode)���Ʒ�ʽmode, Ʒ������q 
	
	delay_ms(300);	
	GPIOE -> ODR &= 0xC0FF;	
    
}



