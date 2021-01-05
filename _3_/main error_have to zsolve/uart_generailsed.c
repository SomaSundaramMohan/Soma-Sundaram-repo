/*write a embedded c program to uart6 configuration using the below contions and transmit character 'A' and recieve 
same character using the hardware loop back.
a)system clk initialization
b)9600 8N01
c)enable polling
*/
//void Sys_Tick_Init(void);
//void milli_delay(int);
#define RCC_AHB1ENR *((int*)(0x40023830))
#define GPIOC_MODER *((int*)(0x40020800))
#define GPIOC_AFRL *((int*)(0x40020820))
#define RCC_APB2ENR *((int*)(0x40023844))
#define UART_BRR  *((int*)(0x40011408))
#define UART_CR1  *((int*)(0x4001140C))
#define UART_SR  *((int*)(0x40011400))
#define UART_DR  *((int*)(0x40011404))

void UART_Init(void)
{
	RCC_AHB1ENR|= (0x1<<2);//set PORT C clock enable
	while(!RCC_AHB1ENR&(0x1<<2));//wait until port c enable
	GPIOC_MODER&=~(0x3<<12);//clear the 13th and 12th pins PC6
	GPIOC_MODER|=(0x2<<12);//Load "10" value in 12th and 13th pins PC5->alternate function mode
	GPIOC_MODER&=~(0x3<<14);//clear the 14th and 13th pins PC7
	GPIOC_MODER|=(0x2<<14);//Load "10" value in 14th and 13th pins PC7->alternate function mode
	GPIOC_AFRL |=0X88000000;//choosing usart6 pc7,pc6 MUX/PAD configuration
	RCC_APB2ENR |=(0x1<<5);//enbaling clk for uart6
	while(!RCC_APB2ENR&(0x1<<5));//wait until uart clk is enable
}

void UART_config()
{
	UART_BRR |=0x682;//baud rate calculation for 9600
	UART_CR1 |=(0x3<<2); //enable tx and rx bits->3rd and 2nd bits
	UART_CR1 |=(0x1<<13);//enable uart
}

void UART_OUT_char(unsigned char Data)
{
	while( (UART_SR & (0X1<<7))==0) //cheack tx bit->7th bit and wait until TDR empty
{
	;
}
UART_DR =Data;
}

char UART_IN_char(void)
{
	while(( UART_SR & (0X1<<5))==0) //Check RXNE bit ->5th bit and wait until RDR full
{
	;
}
return (UART_DR&0xff);
}

/*int main(void)
{
	UART_Init();
	UART_config();
	Sys_Tick_Init();
	while(1)
	{
		UART_OUT_char('A');
		temp=UART_IN_char();
		millisec_delay(100);
	}
}*/
