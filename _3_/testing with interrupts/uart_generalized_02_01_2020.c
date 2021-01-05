/*3. Develop a Communication between two lunch pads using UART6 protocol.

The switches (PC8 and PC9) are inputs, LEDs (PA8, PC5) are outputs, and the UART6 is used to communicate
Raayan Mini Board 1					Raayan Mini Board 2/Laptop

UP SW ON & DN SW OFF	->		RED LED ON (or) Check 'R' in Tera term window. 

UP SW OFF & DN_SW ON	->		GREEN LED ON (or) Check 'G' in Tera term window.

GREEN	LED ON					<-		UP SW OFF & DN_SW ON (or) Type 'G' in Tera term window.

RED	LED	ON						<-		UP SW ON & DNSW OFF (or) Type 'R' in Tera term window.

*/
#include "uart_header_file.h" 

#define RCC_AHB1ENR *((int *)(0x40023830))//port c clock initalization definition 
#define GPIOC_MODER *((int *)(0x40020800))//base:0x40020800 offset:0x00
//clock Address for UART device(initalisation phase)
#define RCC_APB2ENR *((int *)(0x40023844))//RCC base: 0x40023800(stm32 datasheet pg:51) APB2ENR offset:0x44(RM ref manual)
#define GPIOC_AFRL  *((int *)(0x40020820))//GPIOC base: 0x40020800 AFRL offset: 0x20 (pg:162 RM ref manual)
#define USART6_BRR  *((int *)(0x40011408))//UART6 base: 0x40011400 (pg:52-> stm32) BRR offset:0x08 (RM ref manual)
#define USART6_CR1  *((int *)(0x4001140c))//UART6 base: 0x40011400 (pg:52-> stm32) CR1 offset:0x0c (RM ref manual)
#define USART6_SR   *((int *)(0x40011400))//UART6 base: 0x40011400 (pg:52-> stm32) SR offset:0x00 (RM ref manual)
#define USART6_DR   *((int *)(0x40011404))//UART6 base: 0x40011400 (pg:52-> stm32) DR offset:0x04 (RM ref manual)

#define NVIC_ISER2  *((volatile int *)(0xE000E108))//NVIC : 0xE000E100(ARM programming manual pg:208)offset:Address offset: 0x100 + 0x04*x, (x = 0 to 7) pg:210
	/*
=>0x100 +(0x04*2)
=>0x100+0x08
=> 0x108 (address offset)
=>0xE000E108->complete address
(base address+address offset)
*/
volatile int USART_flag_Tx, USART_flag_Rx;
volatile char temp=' ';

void portc_USART6_init(void)
{
	RCC_AHB1ENR |=(0x1<<2);//port c clock enable
	while((RCC_AHB1ENR & (0x1<< 2))==0);//wait till port c clock is enable
	//GPIO pins arenegative logic pins
	GPIOC_MODER&=~(0x3<<12);//clear pc6 bit for transmission pin
	GPIOC_MODER |=(0x2<<12);//load '10' to pc6 bit to select alternate function
	GPIOC_MODER&=~(0x3<<14);//clear pc7 bit for receiving pin
	GPIOC_MODER |=(0x2<<14);//load "10" to pc7 (15th and 14th bits) to select alternate function
	GPIOC_AFRL |=(0x8<<24);// GPIO alternate function low register PC6 set as UART pin 
	GPIOC_AFRL |=(0x8<<28);// GPIO alternate function low register PC7 set as UART pin
	//in pc6 and pc7 though pin is only one pins several functions are assigned to the pins in alternate mode. so we have to select USART6.
	//6th and 7th pins belongs to AFRL. AFRL is a 16X4 MUX so it has 4 sTelection lines. (pg 151 RM ref manual)
	//USART6 pins belongs to 8th input of mux, so we need to load '1000' in GPIO_AFRL 27th,26th,25th,24th bits->pc6 and 31st,30th,29th,28th bits->pc7(pg:162 RM ref manual)
	RCC_APB2ENR |=(0x1<<5);//USART pripheral clock enable
	while(!(RCC_APB2ENR & 0x1<<5));//wait till USART6 clock is enable
	USART6_CR1 |=(0x1<<5);//enabling RXNEIE to enable interrupts for UART 
	NVIC_ISER2 |= (0x1<<7);//71st interrupt(USART6 global interrupt) is nothing but 7th bit in ISER2
}

void USART6_config(void)
{
		USART6_BRR |= (0x682);//to fix baud rate value
/*According to question UART configuration parameter is 9600 8N1(9600->speed,8 ->data bit,N->no parity, 1->stop)
	FORMULA:baudrate=system clock/(16*9600(given))=16M/(16*9600) = 104.1875 == 0x682 (pg:522 RM ref manual)
USART_BRR register starting 4 bits for fraction, next 12 bits for mantissa (pg:558 RM ref manual)
*/
/*USART6_CR1->control register 1, word length -> 8 bits->default value is zero so no change, 
	Bit 12 M: Word length
This bit determines the word length. It is set or cleared by software.
0: 1 Start bit, 8 Data bits, n Stop bit
1: 1 Start bit, 9 Data bits, n Stop bit
Note: The M bit must not be modified during a data transfer (both transmission and reception)
*/
//parity bit->bit 10-> 0 already set->no parity is given in question
/*USART6_CR2-> control register 2:stop bit: bit 13 and 12 "00"-> for 1->default 
Bits 13:12 STOP: STOP bits
These bits are used for programming the stop bits.
00: 1 Stop bit
01: 0.5 Stop bit
10: 2 Stop bits
*/	
//by default USART is disable
USART6_CR1 |= (0x1<<13);//USART6 enable
/*Bit 13 UE: USART enable
When this bit is cleared, the USART prescalers and outputs are stopped and the end of the
current byte transfer in order to reduce power consumption. This bit is set and cleared by
software.
0: USART prescaler and outputs disabled
1: USART enabled*/
USART6_CR1 |= (0x3<<2);//USART6 Tx and Rx enable

}
void UART6_transmit_data(unsigned char ch)
{
while((USART6_SR & (0x1<<7))==0);//polling-> check Tx bit->wait until TDR register is  EMPTY
USART6_DR |= ch;//writing data in to Data Register to transmit
while(!(USART6_SR & (0x1<<6)));//to check transmission complete bit
}
/*Bit 7 TXE: Transmit data register empty
This bit is set by hardware when the content of the TDR register has been transferred into
the shift register. An interrupt is generated if the TXEIE bit =1 in the USART_CR1 register. It
is cleared by a write to the USART_DR register.
0: Data is not transferred to the shift register
1: Data is transferred to the shift register)
Note: This bit is used during single buffer transmission.*/

/*char UART6_receive_data(void)//asynchronous 
{
	//while((USART6_SR & (0x1<<5))==0);//check RXNE bit wait until RDR register is FULL
	if(USART_flag_Rx==0)
	{
	USART_flag_Rx=0;
	return (USART6_DR & 0xff); // returning 8 bit of received data in UART6_DR register to main
	}
	return (USART6_DR & 0x00);
}
*/
/*Bit 5 RXNE: Read data register not empty
This bit is set by hardware when the content of the RDR shift register has been transferred
to the USART_DR register. An interrupt is generated if RXNEIE=1 in the USART_CR1
register. It is cleared by a read to the USART_DR register. The RXNE flag can also be
cleared by writing a zero to it. This clearing sequence is recommended only for multibuffer
communication.
0: Data is not received
1: Received data is ready to be read*/
/*
int main()
{
portc_USART6_init();//UART initialization. 
USART6_config();// UART configuration
Sys_Tick_Init();
while(1)
{
	UART6_transmit_data('A');
	//temp=UART6_receive_data();
	milli_delay(100);
}
return 0;
}
*/
void USART6_IRQHandler(void)
{

	/*if((USART6_SR & (0x1<<7))==1)//checking TXE bit in USART6_SR
	{
		USART_flag_Tx=1;
	}
*/
	if((USART6_SR & (0x1<<5))==1)//checking RXNE bit in USART6_SR
	{
		//USART_flag_Rx=1;
		//USART_flag_Tx=0;
		temp=(USART6_DR & 0xff);
	}
	
}
