/*
sistema automatizado de Controle de Irrigação
TCC Automação Indistrial 4º Modulo
*/
#include <16f877a.h>
#use delay(clock=4000000)
#fuses HS,NOWDT,PUT,NOLVP,NOCPD
//#include <regs_16f87x.h>
//#include <timer0.h>


//#include <mod_lcd_877a.c>
#include<Flex_LCD420.c>
//#include <mod_lcd_4x20.c>
/*
#define BT_START_STOP	  pin_C0 //botao liga/desliga
#define BT_SENSOR1		  pin_C1 //botao bobina
#define BT_FIADOR		  pin_C2 //botao fiador
#define BT_INCREMENTA			  pin_C3 //botao incrementa
#define BT_DECREMENTA			  pin_C4 //botao decrementa
#define BT_ESPIRAS		  pin_C5 //botao decrementa
#define FIM_CURSO_M2	  pin_B0 //Chave fim de curso motor M2
#define ENCODER_M1		  pin_B1 //Entrada encoder motor M1
#define ENA				  pin_B2 //Pino Enable L298 M1
#define ENB				  pin_B3 //Pino Enable L298 M2
#define IN1				  pin_B4 //Pino IN1 L298 M1
#define IN2				  pin_B5 //Pino IN2 L298 M1
#define IN3				  pin_B6 //Pino IN3 L298 M2
#define IN4				  pin_B7 //Pino IN4 L298 M2
#define LED_FIM_CURSO	  pin_D0 //Led fim de curso sinalizacao
#define LED_BOTOES   	  pin_D1 //Led Botoes presionados
*/
//********ENTRADAS BOTOES********************************
#define BT_START_STOP	  pin_c0 //botao liga/desliga
#define BT_SENSOR1		  pin_c1 //botao SENSOR1
#define BT_SENSOR2		  pin_c2 //botao SENSOR2
#define BT_SENSOR3		  pin_c3 //botao SENSOR3
#define BT_SENSOR4		  pin_c4 //botao SENSOR4
#define BT_INCREMENTA	  pin_c5 //botao INCREMENTA +
#define BT_DECREMENTA	  pin_c5 //botao DECREMENTA -
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
unsigned long int hor=0;
int min=0;
int seg=0;
int seg2=0;
int min2=0;
int conta=0;
long int num_voltas_cont,num_espiras;
long int temp_bobina,temp_fiador;
long int valor;
static boolean led;
static boolean flag_desligado;
static boolean flag_ligado;
static boolean flag_sentido_rotacao_M1;
static boolean flag_stop_start;
static boolean flag_encoder_M1,flag_encoder_M1_ativo;

float resultado1;
//***************************************************************************


//tratamento do timer contagem de tempo
#int_timer0
void trata_t0()
{
//****INICIO CONTAGEM DE SEGUNDOS****************

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

//************FIM INCREMENTO DE TEMPO*************




//MENSAGEM DE APRESENTACAO
void mensagem_apresenta()
{
	int pos;
	
	pos=16;
	
//lcd_escreve('\f');
//	while(pos>=1 && pos<=16 || pos>=239 && pos<=255)
//	{
	lcd_gotoxy(1,1);
	printf(lcd_putc,"\f***  IRRIGADOR  ****");
	printf(lcd_putc,"\n**  AUTOMATIZADO  **");	
	delay_ms(700);
//	pos=pos-1;
//	}
	return;
	}

//***************************************************************************
//***************************************************************************
//MENSAGEM Tempeatura
void mensagem_temperatura()
{
	float resultado1;
	lcd_gotoxy(1,4);
	printf(lcd_putc,"Amp.=%2.3f\n",resultado1);
	delay_ms(50);
	return;
	}

//***************************************************************************
//*******************MENSAGEM CONTAGEM DE TEMPO***************************
void mensagem_tempo()
{
	//int pos=0;
//	pos=1;
	lcd_gotoxy(1,3);
	printf(lcd_putc,"Time=%lu:%u:%u\r",hor,min,seg);	
   delay_ms(50);
	return;
	}
//***************************************************************************
void Menssagem_Display()
{
if(seg2!=seg)
{
	seg2=seg;
if(flag_stop_start)//Testa flag start/stop se ligado
{
lcd_putc('\f');
lcd_gotoxy(1,1);
printf(lcd_putc,"***  IRRIGADOR  ****");
lcd_gotoxy(1,2);
printf(lcd_putc,"**  AUTOMATIZADO  **");	
lcd_gotoxy(1,3);
printf(lcd_putc,"Time=%lu:%u:%u",hor,min,seg);	
lcd_gotoxy(1,4);
printf(lcd_putc,"mA=%lu",(valor*17));
//lcd_gotoxy(12,3);
//printf(lcd_putc,"mA=%f",resultado1);
lcd_gotoxy(12,4);
printf(lcd_putc,"M1Rodando");
delay_ms(50);
if((valor*17)>100)
{
//output_low(ENA);//sinal Alto enable1 RB2
}
}
else
{
lcd_putc('\f');
//delay_ms(10);
lcd_gotoxy(1,1);
lcd_putc("***  IRRIGADOR  ****");
//delay_ms(500);
lcd_gotoxy(1,2);
printf(lcd_putc,"**  AUTOMATIZADO  **");
//delay_ms(500);	
lcd_gotoxy(1,3);
printf(lcd_putc,"Time=%lu:%u:%u",hor,min,seg);
//delay_ms(500);
//lcd_gotoxy(12,3);
//printf(lcd_putc,"mA=%f",resultado1);	
lcd_gotoxy(1,4);
printf(lcd_putc,"mA=%lu",(valor*16));
//delay_ms(500);
lcd_gotoxy(12,4);
printf(lcd_putc,"M1Parado ");
delay_ms(500);
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
//inicio do programa
void main()
{

int32 val32;

setup_timer_0(RTCC_INTERNAL|RTCC_DIV_128);
enable_interrupts(GLOBAL|int_timer0);
setup_adc(ADC_CLOCK_INTERNAL);
set_timer0(131);
delay_ms(10);
SETUP_ADC_PORTS(A_ANALOG);
delay_ms(10);
set_adc_channel(0);
delay_ms(10);
set_tris_a(0x07);//Entrada sensor ampraragem do CI L268
set_tris_b(0x00);//Saida para o CI L198
set_tris_c(0x3F);//entradas Botoes 
set_tris_e(0x00);
set_tris_d(0x00);
num_espiras=0;//zera numeros de espira
output_b(0x00);//zera saida port B
output_c(0x00);//zera saida port C
output_d(0x00);//zera saida port D
output_e(0x00);//zera saida port E
flag_encoder_M1=0;//desativa encoder M1
flag_encoder_M1_ativo=1;//seta encoder em ativo
num_voltas_cont=0;//zera o contador de espiras
num_espiras=200;// inicia numero minimo de espiras
flag_sentido_rotacao_M1=1; //inicia rotação do motor M1 sentido horario
flag_stop_start=0; //inicia sistema com stop ativado
temp_bobina=60;// seta minimo de tempo para partida do motor M1
temp_fiador=80;// seta mnimo de tempo para partida do motor M2
flag_desligado=0;
flag_ligado=0;
seg2=59;
min2=1;
//lcd_ini();
delay_ms(100);
lcd_init();
delay_ms(200);
mensagem_apresenta();
//lcd_putc("\f");
// loop enquanto true
//output_low(ENA);//sinal Alto enable1 RB2
//output_low(ENB);//sinal Alto enable2 RB3
//output_low(IN1);//sinal baixo M1 RB4
//output_low(IN2);//sinal baixo M1 RB5
//output_low(IN3);//sinal baixo M2 RB6
//output_low(IN4);//sinal baixo M2 RB7
delay_ms(300);
set_adc_channel(0);
delay_ms(10);
valor=0;
  while(true)
   {
	   


valor=read_adc();
//if(valor)valor+=1;
val32=valor * 5 +((int32)valor * 113)/128; //converte tensao em temperatura
//val32=valor;
resultado1=val32;

//mensagem_tempo();
//mensagem_temperatura();
//mensagem_apresenta();
	
//*******Entrada Botao Stop/Start********************	
if(!input(BT_START_STOP))// Testa botao start/stop se pressionados
{
	
flag_stop_start=!flag_stop_start;// Altera estado Liga/Desliga sistema
delay_ms(500);
loop1:
output_high(LED_BOTOES);// Liga Led botoes
delay_ms(500);			
if(!input(BT_START_STOP))// Testa botao start/stop se presionado
{
	if(flag_stop_start)// Testa flag start/stop se ativado
	{
		flag_stop_start=1;
	}	
	if(!flag_stop_start) // Testa flag start/stop se Desativado
	{
		flag_stop_start=0;
	}
	delay_ms(500);	
goto loop1;
}
//output_low(ENA);//sinal Alto enable1 RB2
//output_low(ENB);//sinal Alto enable2 RB3
//output_low(IN1);//sinal baixo M1 RB4
//output_low(IN2);//sinal baixo M1 RB5
//output_low(IN3);//sinal baixo M2 RB6
//output_low(IN4);//sinal baixo M2 RB7
output_low(LED_BOTOES);
delay_ms(50);	
	}	
//***************************************************************
/*
if(num_voltas_cont==1000)
{
num_voltas_cont=0;
flag_sentido_rotacao_M1=!flag_sentido_rotacao_M1;

}*/			
if(flag_sentido_rotacao_M1)//Testa flag sentido do motor
{
	//sentido horario
//output_high(IN1);//sinal Alto IN1 RB4
//output_low(IN2);//sinal baixo IN2 RB5		
	}
	else
	{
		//sentido anti horario
//output_high(IN2);//sinal Alto IN2 RB5
//output_low(IN1);//sinal baixo IN1 RB4 		
		}		

//*****Verifica status de para movimentação dos motores***********
if(flag_stop_start)//Testa flag start/stop se ligado
{

	
//da pulso no enable motor M1 Roda o motor******************	
//output_high(ENA);//sinal Alto ENA L298 RB2
//delay_ms(temp_bobina);
//output_low(ENA);//sinal baixo ENA L298 RB2
//flag_ligado=1;
//num_voltas_cont=num_voltas_cont+1;
if((valor*17)>100)
{
//output_low(ENA);//sinal Alto enable1 RB2
}
//*************************************************************
}
else
{

//********** travar motores *******************************************
//output_low(ENA);//sinal Alto enable1
//output_low(ENB);//sinal Alto enable2
//output_low(pin_B4);//sinal baixo M1 
//output_low(pin_B5);//sinal baixo M1
//output_low(pin_B6);//sinal baixo M2 
//output_low(pin_B7);//sinal baixo M2 
//delay_ms(30);

}
//********botao seta tempo bobina
if(!input(BT_SENSOR1)) 
{
	output_high(LED_BOTOES);// Liga Led botoes
if(!input(BT_INCREMENTA))
{	
temp_bobina=temp_bobina+1;
delay_ms(300);
}
if(!input(BT_DECREMENTA))
{	
temp_bobina=temp_bobina-1;
delay_ms(300);
}
lcd_gotoxy(1,3);
printf(lcd_putc,"Temp M1=%lu \b",temp_bobina);
delay_ms(500);
}

//********botao seta tempo fiador
if(!input(BT_SENSOR2)) 
{
	output_high(LED_BOTOES);// Liga Led botoes
if(!input(BT_INCREMENTA))
{	
temp_fiador=temp_fiador+1;
delay_ms(300);
}
if(!input(BT_DECREMENTA))
{	
temp_fiador=temp_fiador-1;
delay_ms(300);
}
lcd_gotoxy(1,3);
printf(lcd_putc,"Temp Fia.=%lu \b",temp_fiador);
delay_ms(500);
}

//********botao seta quantidade de espiras
if(!input(BT_SENSOR3)) 
{
	output_high(LED_BOTOES);// Liga Led botoes
if(!input(BT_INCREMENTA))
{	
num_espiras=num_espiras+1;
delay_ms(300);
}
if(!input(BT_DECREMENTA))
{	
num_espiras=num_espiras-1;
delay_ms(300);
}
lcd_gotoxy(1,3);
printf(lcd_putc,"NUM ESP.=%lu \r",num_espiras);
delay_ms(500);
}

if(BT_SENSOR3 & BT_SENSOR2 & BT_SENSOR1)output_LOW(LED_BOTOES);// Desliga Led botoes

Menssagem_Display();

}
}

