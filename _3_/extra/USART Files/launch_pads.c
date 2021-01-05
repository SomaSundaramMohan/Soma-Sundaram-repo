#include"lcd.h"
#include"3_sws_intrpts.h"
//#define GPIOA_MODER *((int*)(0x40020000))
#define GPIOA_ODR *((int*)(0x40020014))
//#define GPIOC_MODER *((int*)(0x40020800))
#define GPIOC_ODR *((int*)(0x40020814))
#define GPIOC_IDR *((int*)(0x40020810))
//#define GPIOC_PUPDR *((int*)(0x4002080c))
#define GPIOC_AFRL *((int*)0x40020820)
#define RCC_APB2ENR *((int*)0x40023844)
#define USART6_SR *((int*)0x40011400)
#define USART6_DR *((int*)0x40011404)
#define USART6_BRR *((int*)0x40011408)
#define USART6_CR1 *((int*)0x4001140c)
#define USART6_CR2 *((int*)0x40011410)
#define NVIC_ISER2 *((int*)0xE000E108)
char ch=' ';
void sw_led_init()
{
        RCC_AHB1ENR |= 0x5; //set port a and port c clock
        while (!(RCC_AHB1ENR & 0x5)){;}
        GPIOA_MODER &= ~(0x3<<16);
        GPIOA_MODER |= (0x1<<16); //load 01 in 16-17 position to make PA8 as output (GREEN LED)
        GPIOC_MODER &= ~(0x3<<10);
        GPIOC_MODER |= (0x1<<10); //load 01 in 10-11 position to make PC5 as output (RED LED)
        GPIOA_ODR |= (0x1<<8);   // off green led
        GPIOC_ODR |= (0x1<<5);   //off red led
        GPIOC_PUPDR &= ~(0xf<<16);
        GPIOC_PUPDR |= (0x5<<16);  //enable pull_up resistor for PC9 and PC8 (inputs)
}
void USART6_Init()
{
/*	RCC_AHB1ENR |= 0x04; //Enable clock for PortC
	while(!(RCC_AHB1ENR & 0x04)); */
	GPIOC_MODER |= 0x0000A000; //Load "10" in PC6 & PC7 bits for alternate function mode
	GPIOC_AFRL |= 0x88000000;//Enable USART6 for PC6 & PC7
	RCC_APB2ENR |= (0x1<<5);//Enable USART6 clock
	while(!(RCC_APB2ENR & (0x1<<5)));
}
void USART6_CONFIG()
{
	USART6_BRR=0x683;  //16000000/(16*9600)=104.1875==>104+0.18==>68integral part
	USART6_CR1 |=0x202c; //Enabling USART6(13),TE(3),RE(2),RXNEIE(5)- RXNE interrupt enable,
                       //TCIE(6) - Transmission complete interrupt enable ,(TXEIE(7) - TXE interrupt enable)
	NVIC_ISER2 |= (0x1<<7); //Enabling UASRT6 interrupt in NVIC (71 position in NVIC table,7th bit in NVIC_ISRE2
}

void USART6_OUT_Char(char data)
{
	while(!(USART6_SR & (0x1<<7)));//check TXE bit - wait until TDR empty
	USART6_DR = data;
	while(!(USART6_SR & (0x1<<6)));//check TC bit - wait for transmission complete
}
int main()
{
	Sys_Tick_Init();
	ml_Delay(20);
	LCD_init();
	sw_led_init();
	PA0_PC9_PC8_interrupt_init();
  USART6_Init();
	USART6_CONFIG();
	while(1)
	{
		LCD_write_cmd(0x80);
		disp_string("Received:");
		LCD_write_data(ch);
		if(ch=='g' || ch=='G')
		{
			GPIOA_ODR &= ~(0x1<<8);//Turn on GREEN LED
		  ml_Delay(1000);
			ml_Delay(1000);
		  GPIOA_ODR |= (0x1<<8);//Turn off GREEN LED
			ch='0';
	  }
		else if(ch=='r' || ch=='R')
		{
			GPIOC_ODR &= ~(0x1<<5);//Turn on RED lED
			ml_Delay(1000);
			ml_Delay(1000);
			GPIOC_ODR |= (0x1<<5);//Turn off RED LED
			ch='0';
		}
		ml_Delay(10);
	}
}
void USART6_IRQHandler()
{
	if(USART6_SR & (0x1<<5)) //check RXNE bit (RDR FULL or EMPTY)
	{		
	ch=(USART6_DR&0xFF);
//	USART6_SR &= ~(0x1<<5); //clear RXNE not mandatory
	}
}
void EXTI9_5_IRQHandler()
{
        if(EXTI_PR & 0x1<<9) //check PC9_sw1 intertupt
        {
                USART6_OUT_Char('R');
					      EXTI_PR = 0x1<<9; //Clear PC9 interrupt
        }
        if(EXTI_PR & 0x1<<8) //check PC8_sw2 intertupt
        {
                USART6_OUT_Char('G');
					      EXTI_PR = 0x1<<8; //Clear PC8 interrupt
        }
        ml_Delay(20);
}
