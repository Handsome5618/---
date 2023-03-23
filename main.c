#include <REGX52.H>
#include "LCD1602.h"
#include "DS18B20.h"
#include "Delay.h"
#include "Stepmotor.h"

sbit Open = P3^4;
sbit Close = P3^5;
sbit Move = P3^6;
sbit Light = P3^7;

void INT0_Init(void);

unsigned char MODE = 0;
unsigned char dir = 0;
unsigned char step = 0;
unsigned char StopFlat = 1;
unsigned char OpenFlat = 0;
unsigned char CloseFlat = 0;
unsigned char LightFlat = 0;
unsigned char MoveFlat = 0;

int main()
{
	DS18B20_ConvertT();		//�ϵ���ת��һ���¶ȣ���ֹ��һ�ζ����ݴ���
	Delay(1000);			//�ȴ�ת�����
	LCD_Init();
	INT0_Init();
	LCD_ShowString(1,1,"Temper:");
	
	while(1)
	{
		if(MODE == 0)	//�ֶ�ģʽ
		{
			LCD_ShowString(2,1,"Hand");
			if(Close == 0)
			{
				Delay(50);
				if(Close == 0)
				{
					StopFlat = 1;
					CloseFlat = 1;
				}
				while(CloseFlat && StopFlat)
				{
					LCD_ShowString(2,6,"Closing");
					Motor_Nrun();
					if(Move)
					{
						LCD_ShowString(2,6,"Close   ");
						CloseFlat = 0;
					}
				}
			}
			
			if(Open == 0)
			{
				Delay(50);
				if(Open == 0)
				{
					OpenFlat = 1;
					StopFlat = 1;
				}
				while(OpenFlat && StopFlat)
				{
					LCD_ShowString(2,6,"Opening");
					Motor_Prun();
					if(Move)
					{
						LCD_ShowString(2,6,"Open   ");
						OpenFlat = 0;
					}
				}
			}
			if(StopFlat == 0)
			{
					LCD_ShowString(2,6,"Stop   ");
			}
		}
		
		else if(MODE)	//�Զ�ģʽ
		{
			LCD_ShowString(2,1,"Auto");
			if(Light == 0)
			{
				LightFlat = 0;
				while(LightFlat == 0 && MoveFlat == 0)
				{
					LCD_ShowString(2,6,"Closing  HL");
					Motor_Nrun();
					if(Move)
					{
						LCD_ShowString(2,6,"Close      ");
						LightFlat = 2;
						MoveFlat = 1;
					}
				}
				while(LightFlat == 0 && MoveFlat == 2)
				{
					LCD_ShowString(2,6,"Closing  HL");
					Motor_Nrun();
					if(Move)
					{
						LCD_ShowString(2,6,"Close      ");
						LightFlat = 2;
						MoveFlat = 1;
					}
				}
			}

			else if(Light == 1 && MoveFlat == 1)
			{
				LightFlat = 1;
				while(LightFlat == 1 && MoveFlat == 1)
				{
					LCD_ShowString(2,6,"Opening  LL");
					Motor_Prun();
					if(Move)
					{
						LCD_ShowString(2,6,"Open       ");
						MoveFlat = 0;
						LightFlat = 2;
					}
				}
			}
			
			else if(Light == 1)
			{
				LightFlat = 1;
				while(LightFlat == 1 && MoveFlat == 0)
				{
					LCD_ShowString(2,6,"Opening  LL");
					Motor_Prun();
					if(Move)
					{
						LCD_ShowString(2,6,"Open       ");
						MoveFlat = 2;
						LightFlat = 2;
					}
				}
			}
		}
		
		Motor_Stop();
		ReadT();
	}
}

void INT0_Init(void)    //�ⲿ�жϺ�����ʼ��  �ӳ���                  
{   
	P3_2 = 1;
	P3_3 = 1;
	EA = 1;			//�����жϿ���
	EX0 = 1;		//���ⲿ�ж�0 
	IT0 = 1;		//�ⲿ�ж�0��Ϊ�͵�ƽ���� // 1��Ϊ�½��ش���
	EX1 = 1;		//���ⲿ�ж�1 
	IT1 = 1;		//�ⲿ�ж�1��Ϊ�͵�ƽ���� // 1��Ϊ�½��ش���
}

void Move_Flat() interrupt 0	//ģʽ�л� P3^2
{
	if(P3^2 == 0)
	{
		Delay(50);
		if(P3^2 == 0)
		{
			MODE = ~MODE;
			P3_1 = ~P3_1;	
			P3_2 = 1;
		}
	}
}

void Mode_Switch() interrupt 2	//ֹͣ�ж�  P3^3
{
	StopFlat = 0;
}
