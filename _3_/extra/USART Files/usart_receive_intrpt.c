///#include"sys_tick.h"
#include"lcd.h"
//#define RCC_AHB1ENR *((int*)0x40023830)
#define GPIOC_MODER *((int*)0x40020800)
#define GPIOC_AFRL *((int*)0x40020820)
#define RCC_APB2ENR *((int*)0x40023844)
#define USART6_SR *((int*)0x40011400)
#define USART6_DR *((int*)0x40011404)
#define USART6_BRR *((int*)0x40011408)
#define USART6_CR1 *((int*)0x4001140c)
#define USART6_CR2 *((int*)0x40011410)
#define NVIC_ISER2 *((int*)0xE000E108)
char ch=' ';
int TX_Flag=0;
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
	USART6_CR1 |=0x202c; //Enabling USART6(13),TE(3),RE(2),RXNEIE(5)- RXNE interrupt enable
	NVIC_ISER2 |= (0x1<<7); //Enabling UASRT6 interrupt in NVIC (71 position in NVIC table,7th bit in NVIC_ISRE2
}
int main()
{
	Sys_Tick_Init();
	ml_Delay(20);
	LCD_init();
	USART6_Init();
	USART6_CONFIG();
	while(1)
	{
		LCD_write_cmd(0x80);
		disp_string("Received:");
		LCD_write_data(ch);
		ml_Delay(500);
	}
}
void USART6_IRQHandler()
{
	if(USART6_SR & (0x1<<5)) //check RXNE bit (RDR FULL or EMPTY)
	ch=(USART6_DR&0xFF);
}
