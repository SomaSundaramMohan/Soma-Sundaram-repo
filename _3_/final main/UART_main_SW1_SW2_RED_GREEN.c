#include "uart_header_file.h"

#define RCC_AHB1ENR *((volatile int *)(0x40023830))   //RCC 0x4002 3800   //Address offset RCC AHB1ENR   0x30
#define GPIOC_MODER *((volatile int *)(0x40020800))		//PORT C 0x4002 0800 // Address offset for GPIOB_MODER : 0x00
//#define GPIOC_IDR   *((volatile int *)0x40020810)  	//PORT C  for wake up switch   //Address offset for GPIOA_IDR : 0x10 
//IDR is used when doing polling
#define GPIOC_PUPDR *((volatile int *)(0x4002080c))	//PORT C base: 0x40020800 PUPDR: 0x0c
	

#define RCC_APB2ENR *((volatile int *)(0x40023844))// RCC 0x40023800 offset:0x44(RM reference manual)->APB2 same for both PC9,PC8
#define SYSCFG_EXTICR3 *((volatile int *)(0x40013810))	//RCC base: 0x40013800(STM32 Datasheet PG:51) offset: 0x10(RM ref manual)->APB2
#define EXTI_FTSR *((volatile int *)(0x40013c0c))	//EXT1 base: 0x40013c00(STM32 Datasheet PG:51) offset: 0x0c(RM ref manual)->APB2
#define EXTI_IMR *((volatile int *)(0x40013c00))	//EXT1 base: 0x40013c00(STM32 Datasheet PG:51) offset: 0x00(RM ref manual)->APB2

#define NVIC_ISER0 *((volatile int *)(0xE000E100))//NVIC : 0xE000E100(ARM programming manual pg:208)offset:Address offset: 0x100 + 0x04*x, (x = 0 to 7) pg:210
	//this NVIC_ISER0 address is same for evry interrupt set enable register
//NVIC_ISER0 bits 0 to 31 are for interrupt 0 to 31, respectively->pg:210->arm programming manual
#define EXTI_PR *((volatile int *)(0x40013c14))	//EXT1 base: 0x40013c00(STM32 Datasheet PG:51) offset: 0x14(RM ref manual)->APB2 pg:203


#define GPIOA_MODER *((int*)(0x40020000))//green led mode 
#define GPIOA_ODR *((int*)(0x40020014))//green led output
//#define GPIOC_MODER *((int*)(0x40020800))//red led mode
#define GPIOC_ODR *((int*)(0x40020814))//red led output
#define GPIOC_IDR *((int*)(0x40020810))//idr port c

extern volatile int temp;
int SW1_FLAG;
int SW2_FLAG;

void Led_int()
{
	RCC_AHB1ENR|=(0x1);//PORT A clk inialisation
	while(!(RCC_AHB1ENR & (0x01)));//wait until PORT A clk is enable
	
	GPIOA_MODER&=~(0x3<<16);//clear the 17th and 16th pins green led PA8 mode8
	GPIOA_MODER|=(0x1<<16);//Load "01" value in 17th and 16th pins for green led PA8 mode8
	GPIOA_ODR|=(0x1<<8);//set green led pin (OFF)
	
}
void Port_C_Init(void)
{
	RCC_AHB1ENR|=(0x1<<2);//PORT C clk inialisation
	while(!(RCC_AHB1ENR & (0x01<<2)));//wait until PORT C clk is enable
	
	GPIOC_MODER&=~(0x3<<10);//clear the 11th and 10th pins red led PC5 mode5
	GPIOC_MODER|=(0x1<<10);//Load "01" value in 11th and 10th pins for red led pc5 mode5
	GPIOC_MODER &= ~(0x3<<18);//clear 19th and 18th bits of MODE register for PORT C PC9 switch1 -> GPIO input mode->so its value is 00 for input
	GPIOC_MODER &= ~(0x3<<16);//clear 17th and 16th bits of MODE register for PORT C PC8 switch2 -> GPIO input mode->so its value is 00 for input
	GPIOC_ODR|=(0x1<<5);//set red led pin (OFF)
	GPIOC_PUPDR &= ~(0x3<<18);//clear 19th and 18th bit for PORT C PC9 switch1 for output mode setting
	GPIOC_PUPDR |= (0x1<<18);//load value '01' to 19th and 18th bit for PORT C switch1 for output mode setting-> to eanble PULL UP register
	
	GPIOC_PUPDR &= ~(0x3<<16);//clear 17th and 16th bit for PORT C PC8 switch2 for output mode setting
	GPIOC_PUPDR |= (0x1<<16);//load value '01' to 17th and 16th bit for PORT C switch2 for output mode setting-> to eanble PULL UP register
	
}
void PC9_PC8_Interrupt_Init()
{
	RCC_APB2ENR |=(0x1<<14);//enabling APB2 BUS clk to enable interrupt
	//High performance BUS enable register
	while(!(RCC_APB2ENR & (0x01<<14)));//wait until APB2 BUS clk is enable
	SYSCFG_EXTICR3 |= 0x22; //system configuration is present in APB2 BUS. P8,p9 PIN->first 4 bits, PORT C so value is 0x0010 in four bits
	
//system configuration_External_Interrupt_Configuration_Register1//sets ports
  EXTI_IMR |= (0x3<<8); // Interrupt Mask register. PC9,PC8 pin set for interrupt->so set 9th and 8th bits to 1
	EXTI_FTSR |= (0x3<<8); // Falling_Trigger_selection_register. PC9,PC8 pin set for interrupt->so set 9th and 8th bits to 1//sets pins
	
	NVIC_ISER0 |= (0x1<<23);//Nested Vector Interrupt Controller _ Interrupt Set Enable Register
	//23rd bit set to 1,in vect0r table, EXTI9_5 corresponds to 23rd position. EXTI0 because we took PC9,PC8.
	//NVIC_ISER0 bits 0 to 31 are for interrupt 0 to 31, respectively
	
	
}
int main()
{
Led_int();
Port_C_Init();
PC9_PC8_Interrupt_Init();//APB2 BUS,external interrupt,NVIC initialization
Sys_Tick_Init();//timer initialization
portc_USART6_init();//UART initialization
USART6_config();//UART configuration

while(1)
 {
	 if(SW1_FLAG==1)//UP SW1 ON & DN SW2 OFF	->		RED LED ON (or) Check 'R' in Tera term window.
	 {
			UART6_transmit_data('R');
		 milli_delay(1000);
		 	SW1_FLAG=0;
	 }

	 if(SW2_FLAG==1)//UP SW OFF & DN_SW ON	->		GREEN LED ON (or) Check 'G' in Tera term window.
	 {
			UART6_transmit_data('G');
		  milli_delay(1000);
		 	 SW2_FLAG=0;
	 }
	 
	if(temp=='G')//GREEN LED ON 	<- 	UP SW OFF & DN_SW ON  (or) Type 'G' in Tera term window.
		{
			GPIOA_ODR &= ~(0X1<<8);//clear PA8 bit-> GREEN LED ON
			milli_delay(1000);//1s delay
			GPIOA_ODR |= (0X1<<8);//set PA8 bit-> GREEN LED OFF
		}
	 
	else if(temp=='R')//RED LED ON	<-	UP SW ON & DN_SW OFF(or) Type 'R' in Tera term window.
		{
			//((USART6_SR & (0x1<<5))==0) && ((USART6_DR & 0xff)=='R')
			GPIOC_ODR &= ~(0X1<<5);//clear PC5 bit-> RED LED ON
			milli_delay(1000);//1s delay
			GPIOC_ODR |= (0X1<<5);//set PC5 bit-> RED LED OFF
		}
	
 }
}
void EXTI9_5_IRQHandler (void)
{
	if(EXTI_PR & 0x1<<9)//pending register will be set when interrupt occurs->switch1
	{
		SW1_FLAG=1;//increment counter every time when interrupt occurs
		EXTI_PR|=(0x1<<9);//rc-w1 -> clear register when we write 1(9th bit)
	}
	if(EXTI_PR & 0x1<<8)//pending register will be set when interrupt occurs->switch2
	{
		SW2_FLAG=1;//increment counter every time when interrupt occurs
		EXTI_PR|=(0x1<<8);//rc-w1 -> clear register when we write 1(8th bit)
	}
}


