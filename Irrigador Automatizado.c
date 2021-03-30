/*
sistema automatizado de Irrigação
TCC Automação Indistrial 4º Modulo 2015
*/
#include <16f877a.h>
#use delay(clock=4000000)
#fuses HS,NOWDT,PUT,NOLVP,NOCPD
//#include <regs_16f87x.h>
//#include <timer0.h>

#include<Flex_LCD420.c>
//********ENTRADAS BOTOES********************************
#define BT_START_STOP	  pin_c0 //botao liga/desliga
#define BT_SENSOR1		  pin_c1 //botao SENSOR1
#define BT_SENSOR2		  pin_c2 //botao SENSOR2
#define BT_SENSOR3		  pin_c3 //botao SENSOR3
#define BT_SENSOR4		  pin_c4 //botao SENSOR4
#define BT_INCREMENTA	  pin_c5 //botao INCREMENTA +
#define BT_DECREMENTA	  pin_c6 //botao DECREMENTA -
//*******************************************************
//*****************ENTRADA SENSORES**********************
#define SENSOR_BATERIA	pin_a0 //SENSOR TENSÃO BATERIA
#define SENSOR1			pin_a1 //ENTRADA SENSOR1
#define SENSOR2			pin_a2 //ENTRADA SENSOR2
#define SENSOR3			pin_a3 //ENTRADA SENSOR3
#define SENSOR4			pin_a4 //ENTRADA SENSOR4
//*******************************************************
//***************SAIDAS DRIVERS**************************
#define RELE_BAT		pin_b0//SAIDA RELE BATERIA
#define M1				pin_b1//SAIDA MOTOR1
#define M2				pin_b2//SAIDA MOTOR2
#define M3				pin_b3//SAIDA MOTOR3
#define M4				pin_b4//SAIDA MOTOR4
#define LED_BOTOES		pin_b6//SAIDA LED BOTOES
#define LED_TIMER		pin_b7//SAIDA LED TIMER
//*******************************************************
static boolean led;
static boolean flag_start_stop;
static boolean flag_ligado;
static boolean flag_M1;
static boolean flag_M2;
static boolean flag_M3;
static boolean flag_M4;
int hor=0;
int min=0;
int seg=0;
int seg2=0;
int conta=0;
int Kp; //ganho proporcional
int Ki; //ganho integral
int32 Sp,Dv,Mv,Pv;
int32 set_sensor1, set_sensor2, set_sensor3, set_sensor4;
float ler_sensor1, ler_sensor2, ler_sensor3, ler_sensor4;
float ler_bat;

//*******************************************************

//tratamento do timer contagem de tempo
#int_timer0
void trata_t0()
{
//*************INICIO CONTAGEM DE SEGUNDOS***************

static int conta;
	set_timer0(131+get_timer0());//contagem de tempo 1 segundo
	conta++;
if(conta==125)
{
	conta=0;
	led=!led;
	output_bit(LED_TIMER,led);
	}

if(conta==0)
{
	seg=seg+1;
	
}	

if(seg>59)
{
	seg=0;
	min=min+1;
	}
	if(min>59)
	{
		min=0;
		hor=hor+1;
		}
}

//************FIM INCREMENTO DE TEMPO********************




//*************MENSAGEM DE APRESENTACAO******************
void mensagem_apresenta()
{
	int pos;
	
	pos=20;
	
lcd_putc('\f');
	while(pos>=1 && pos<=20 || pos>=20 && pos<=40)
	{
	lcd_gotoxy(pos,1);
	printf(lcd_putc,"***  IRRIGADOR  ****\r");	
	delay_ms(20);
	pos=pos-1;
	}
	
	pos=20;
	while(pos>=1 && pos<=20 || pos>=235 && pos<=255)
	{
	lcd_gotoxy(pos,2);
	printf(lcd_putc,"**  AUTOMATIZADO  **\r");	
	delay_ms(20);
	pos=pos-1;
	}		
	
	return;
	}

//***************************************************************************

//*******************MENSAGEM CONTAGEM DE TEMPO***************************
void mensagem_tempo()
{
	//int pos=0;
//	pos=1;
	lcd_gotoxy(1,3);
	printf(lcd_putc,"Time=%u:%u:%u\r",hor,min,seg);	
   delay_ms(50);
	return;
	}
//***************************************************************************
void Menssagem_Display()
{
if(seg2!=seg)
{
	seg2=seg;
if(flag_start_stop)//Testa flag start/stop se ligado
{
lcd_putc('\f');
lcd_gotoxy(1,1);
printf(lcd_putc,"S1=%2.0u%%\b",set_sensor1);
lcd_gotoxy(1,2);
printf(lcd_putc,"S2=%2.0u%%\b",set_sensor2);
lcd_gotoxy(1,3);
printf(lcd_putc,"S3=%2.0u%%\b",set_sensor3);
lcd_gotoxy(1,4);
printf(lcd_putc,"S4=%2.0u%%\b",set_sensor4);

lcd_gotoxy(7,1);
printf(lcd_putc,"V1=%2.0f%%\b",ler_sensor1);
lcd_gotoxy(7,2);
printf(lcd_putc,"V2=%2.0f%%\b",ler_sensor2);
lcd_gotoxy(7,3);
printf(lcd_putc,"V3=%2.0f%%\b",ler_sensor3);
lcd_gotoxy(7,4);
printf(lcd_putc,"V4=%2.0f%%\b",ler_sensor4);

lcd_gotoxy(13,1);
printf(lcd_putc,"M1=%1.0X\b",flag_M1);
lcd_gotoxy(13,2);
printf(lcd_putc,"M2=%1.0X\b",flag_M2);
lcd_gotoxy(13,3);
printf(lcd_putc,"M3=%1.0X\b",flag_M3);
lcd_gotoxy(13,4);
printf(lcd_putc,"M4=%1.0X\b",flag_M4);	

lcd_gotoxy(18,1);
printf(lcd_putc,"BAT\b");
lcd_gotoxy(17,2);
printf(lcd_putc,"%2.1f",ler_bat);

lcd_gotoxy(17,3);
printf(lcd_putc,"SIST\b");
lcd_gotoxy(18,4);
printf(lcd_putc,"LIG\b");
delay_ms(50);
}
else
{
lcd_putc('\f');
lcd_gotoxy(1,1);
printf(lcd_putc,"S1=%2.0u%%\b",set_sensor1);
lcd_gotoxy(1,2);
printf(lcd_putc,"S2=%2.0u%%\b",set_sensor2);
lcd_gotoxy(1,3);
printf(lcd_putc,"S3=%2.0u%%\b",set_sensor3);
lcd_gotoxy(1,4);
printf(lcd_putc,"S4=%2.0u%%\b",set_sensor4);

lcd_gotoxy(7,1);
printf(lcd_putc,"V1=%2.0f%%\b",ler_sensor1);
lcd_gotoxy(7,2);
printf(lcd_putc,"V2=%2.0f%%\b",ler_sensor2);
lcd_gotoxy(7,3);
printf(lcd_putc,"V3=%2.0f%%\b",ler_sensor3);
lcd_gotoxy(7,4);
printf(lcd_putc,"V4=%2.0f%%\b",ler_sensor4);

lcd_gotoxy(13,1);
printf(lcd_putc,"M1=%1.0X\b",flag_M1);
lcd_gotoxy(13,2);
printf(lcd_putc,"M2=%1.0X\b",flag_M2);
lcd_gotoxy(13,3);
printf(lcd_putc,"M3=%1.0X\b",flag_M3);
lcd_gotoxy(13,4);
printf(lcd_putc,"M4=%1.0X\b",flag_M4);	

lcd_gotoxy(18,1);
printf(lcd_putc,"BAT\b");
lcd_gotoxy(17,2);
printf(lcd_putc,"%2.1f",ler_bat);

lcd_gotoxy(17,3);
printf(lcd_putc,"SIST\b");
lcd_gotoxy(18,4);
printf(lcd_putc,"DES\b");
delay_ms(50);
}
}

return;
}
//===buzzer====================
void buzzer()
{
int i;

 for (i=0; i<=100; i++) 
{
output_high(pin_a5);//sinal Alto buzzer
delay_us(400);
output_low(pin_a5);//sinal baixo buzzer
delay_us(400);
output_high(pin_a5);//sinal Alto buzzer
delay_us(400);
output_low(pin_a5);//sinal baixo buzzer

}

return;
}
//*******************************************************
//********************Trata Sensores*********************
void trata_sensores()//leitura dos sensores e comparação 
{					 //com set-poit
set_adc_channel(0);
delay_ms(10);	   
ler_bat=read_adc();
ler_bat=(ler_bat/256)*18;
delay_ms(10);

set_adc_channel(1);
delay_ms(10);	   
ler_sensor1=read_adc();
ler_sensor1=(ler_sensor1/256)*100;
delay_ms(10);

set_adc_channel(2);
delay_ms(10);	   
ler_sensor2=read_adc();
ler_sensor2=(ler_sensor2/256)*100;
delay_ms(10);

set_adc_channel(3);
delay_ms(10);	   
ler_sensor3=read_adc();
ler_sensor3=(ler_sensor3/256)*100;
delay_ms(10);

set_adc_channel(4);
delay_ms(10);	   
ler_sensor4=read_adc();
ler_sensor4=(ler_sensor4/256)*100;
delay_ms(10);
	if(set_sensor1>ler_sensor1)//SENSOR1
	{
		flag_M1=1;//Liga motor M1
		}
		else
		{
			flag_M1=0;//Desliga motor M1
			}

	if(set_sensor2>ler_sensor2)//SENSOR2
	{
		flag_M2=1;//Liga motor M2
		}
		else
		{
			flag_M2=0;//Desliga motor M2
			}

	if(set_sensor3>ler_sensor3)//SENSOR3
	{
		flag_M3=1;//Liga motor M3
		}
		else
		{
			flag_M3=0;//Desliga motor M3
			}

	if(set_sensor4>ler_sensor4)//SENSOR4
	{
		flag_M4=1;//Liga motor M4
		}
		else
		{
			flag_M4=0;//Desliga motor M4
			}

	return;
	}
//*******************************************************
	
//*************Trata saidas Motores**********************
void trata_motores()
{

if(flag_ligado)//Sidas para motores**********************
{

if(flag_M1)//MOTOR1
{

	output_high(M1);
	delay_ms(Mv);
	output_low(M1);
	Mv=(Mv/Ki);
	output_high(M1);
	delay_ms(Mv);
	output_low(M1);	
	}
	else
	{
		output_low(M1);
		}	
	
	
if(flag_M2)//MOTOR2
{

	output_high(M2);
	delay_ms(Mv);
	output_low(M2);
	Mv=(Mv/Ki);
	output_high(M2);
	delay_ms(Mv);
	output_low(M2);		
	}
	else
	{
		output_low(M2);
		}	
	
	
if(flag_M3)//MOTOR3
{

	output_high(M3);
	delay_ms(Mv);
	output_low(M3);
	Mv=(Mv/Ki);
	output_high(M3);
	delay_ms(Mv);
	output_low(M3);		
	}
	else
	{
		output_low(M3);
		}	
	
if(flag_M4)//MOTOR4
{

	output_high(M4);
	delay_ms(Mv);
	output_low(M4);
	Mv=(Mv/Ki);
	output_high(M4);
	delay_ms(Mv);
	output_low(M4);		
	}
	else
	{
		output_low(M4);
		}		
	}
return;
}
//*******************************************************

//******************Trata botões*************************
void trata_booes()
{
//*******Entrada Botao Stop/Start********************	
if(!input(BT_START_STOP))// Testa botao start/stop se pressionados
{
	
flag_start_stop=!flag_start_stop;// Altera estado Liga/Desliga sistema
delay_ms(500);
loop1:
output_high(LED_BOTOES);// Liga Led botoes
delay_ms(500);			
if(!input(BT_START_STOP))// Testa botao start/stop se presionado
{
	if(flag_start_stop)// Testa flag start/stop se ativado
	{
		flag_start_stop=1;
	}	
	if(!flag_start_stop) // Testa flag start/stop se Desativado
	{
		flag_start_stop=0;
	}
	delay_ms(500);	
goto loop1;
}
output_low(RELE_BAT);//RELE BATERIA
output_low(M1);//MOTOR1
output_low(M2);//MOTOR2
output_low(M3);//MOTOR3
output_low(M4);//MOTOR4
delay_ms(10);	
	}	
//*******************************************************
output_low(LED_BOTOES);// Desliga Led botoes	
//********BOTAO SENSOR1**********************************
if(!input(BT_SENSOR1)) 
{
	output_high(LED_BOTOES);// Liga Led botoes
if(!input(BT_INCREMENTA))
{
if(set_sensor1<=99)
{		
set_sensor1=set_sensor1+1;
}
if(set_sensor1>99)
{
	set_sensor1=99;
}	
delay_ms(300);
}
if(!input(BT_DECREMENTA))
{	
if(set_sensor1>1)
{		
set_sensor1=set_sensor1-1;
}
if(set_sensor1<1)set_sensor1=1;
delay_ms(300);
}
lcd_gotoxy(1,1);
printf(lcd_putc,"S1=%2.0u%%\b",set_sensor1);
delay_ms(300);
}
//*******************************************************
//********BOTAO SENSOR2**********************************
if(!input(BT_SENSOR2)) 
{
	output_high(LED_BOTOES);// Liga Led botoes
if(!input(BT_INCREMENTA))
{
if(set_sensor2<=99)
{		
set_sensor2=set_sensor2+1;
}
if(set_sensor2>99)set_sensor2=99;
delay_ms(300);
}
if(!input(BT_DECREMENTA))
{	
if(set_sensor2>1)
{		
set_sensor2=set_sensor2-1;
}
if(set_sensor2<1)set_sensor2=1;
delay_ms(300);
}
lcd_gotoxy(1,2);
printf(lcd_putc,"S2=%2.0u%%\b",set_sensor2);
delay_ms(300);
}
//*******************************************************
//********BOTAO SENSOR3**********************************
if(!input(BT_SENSOR3)) 
{
	output_high(LED_BOTOES);// Liga Led botoes
if(!input(BT_INCREMENTA))
{
if(set_sensor3<=99)
{		
set_sensor3=set_sensor3+1;
}
if(set_sensor3>99)set_sensor3=99;
delay_ms(300);
}
if(!input(BT_DECREMENTA))
{	
if(set_sensor3>1)
{		
set_sensor3=set_sensor3-1;
}
if(set_sensor3<1)set_sensor3=1;
delay_ms(300);
}
lcd_gotoxy(1,3);
printf(lcd_putc,"S3=%2.0u%%\b",set_sensor3);
delay_ms(300);
}
//*******************************************************
//********BOTAO SENSOR4**********************************
if(!input(BT_SENSOR4)) 
{
	output_high(LED_BOTOES);// Liga Led botoes
if(!input(BT_INCREMENTA))
{
if(set_sensor4<=99)
{		
set_sensor4=set_sensor4+1;
}
if(set_sensor4>99)set_sensor4=99;
delay_ms(300);
}
if(!input(BT_DECREMENTA))
{	
if(set_sensor4>1)
{		
set_sensor4=set_sensor4-1;
}
if(set_sensor4<1)set_sensor4=1;
delay_ms(300);
}
lcd_gotoxy(1,4);
printf(lcd_putc,"S4=%2.0u%%\b",set_sensor4);
delay_ms(300);
}
return;
}

//*******************************************************
//********************INICIO PROGRAMA********************
//*******************************************************
void main()
{
setup_timer_0(RTCC_INTERNAL|RTCC_DIV_128);
enable_interrupts(GLOBAL|int_timer0);
set_timer0(131);
delay_ms(10);
set_tris_a(0x3F);//Entradas Analogicas
set_tris_b(0x00);//Saidas Drivers
set_tris_c(0x7F);//Entradas Botoes
set_tris_d(0x00);//Saidas Dados Display 
set_tris_e(0x07);//Entradas Analogicas
output_b(0x00);//Zeras saida
setup_adc(ADC_CLOCK_INTERNAL);
delay_ms(10);
SETUP_ADC_PORTS(ALL_ANALOG);
set_sensor1=50;
set_sensor2=50;
set_sensor3=50;
set_sensor4=50;
delay_ms(100);
lcd_init();
delay_ms(100);
mensagem_apresenta();
delay_ms(500);
Kp=10;
Ki=10;

  while(true)
   {

//*****Verifica status de para movimentação dos motores**
if(flag_start_stop)//Testa flag start/stop se ligado
{
flag_ligado=1;
delay_ms(10);
//*******************************************************
}
else
{
flag_ligado=0;
delay_ms(10);

}
trata_booes();
trata_sensores();
trata_motores();
Menssagem_Display();
}
}

