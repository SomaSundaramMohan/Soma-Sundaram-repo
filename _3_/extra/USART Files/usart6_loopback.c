/*1. Write a pseudo code and Embedded C program to UART6 Configuration using the below conditions. And
transmit character ' A ' and Receive same character using hardware Loop back.
a. System Clock Initialization to 16MHZ.
b. 9600 8N1
c. Enable Polling (nothing but disable UART Interrupts) */
#include"sys_tick.h"
#define RCC_AHB1ENR *((int*)0x40023830)
#define GPIOC_MODER *((int*)0x40020800)
#define GPIOC_AFRL *((int*)0x40020820)
#define RCC_APB2ENR *((int*)0x40023844)
#define USART6_SR *((int*)0x40011400)
#define USART6_DR *((int*)0x40011404)
#define USART6_BRR *((int*)0x40011408)
#define USART6_CR1 *((int*)0x4001140c)
#define USAR6_CR2 *((int*)0x40011410)
char ch;
void USART6_Init()
{
	RCC_AHB1ENR |= 0x04; //Enable clock for PortC
	while(!(RCC_AHB1ENR & 0x04));
	GPIOC_MODER |= 0x0000A000; //Load "10" in PC6 & PC7 bits for alternate function mode
	GPIOC_AFRL |= 0x88000000;//Enable USART6 for PC6 & PC7
	RCC_APB2ENR |= (0x1<<5);//Enable USART6 clock
	while(!(RCC_APB2ENR & (0x1<<5)));
}
void USART6_CONFIG()
{
	USART6_BRR=0x683;  //16000000/(16*9600)=104.1875==>104+0.18==>68integral part
	USART6_CR1 |=0x200c; //Enabling USART6 UE(13),TE(3),RE(2)
}
void USART6_OUT_Char(char data)
{
	while(!(USART6_SR & (0x1<<7)));//check TXE bit - wait until TDR empty
	USART6_DR |= data;
	while(!(USART6_SR & (0x1<<6)));//wait for transmission complete
}
char USART6_In_char()
{
	while(!(USART6_SR & (0x1<<5)));//check RXNE bit - wait until RDR FULL
	return (USART6_DR & (0xFF));
}
void USART6_string_out(char str[])
{
	int i;
	for(i=0;str[i]!=0;i++)
	USART6_OUT_Char(str[i]);
}

int main()
{
	Sys_Tick_Init();
	USART6_Init();
	USART6_CONFIG();
	while(1)
	{
		USART6_OUT_Char('A');
	  ch=USART6_In_char();
		ml_Delay(500);
	}
}
