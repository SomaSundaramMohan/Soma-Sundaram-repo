//#include"lcd.h" //it already includes sys_tick.h
#define RCC_AHB1ENR *((int*)0x40023830) //already defined in lcd.h
#define GPIOA_MODER *((int*)0x40020000)
#define GPIOA_PUPDR *((int*)0x4002000c)
#define GPIOC_MODER *((int*)0x40020800)
#define GPIOC_PUPDR *((int*)0x4002080c)
#define RCC_APB2 *((int*)0x40023844)
#define SYSCFG_EXTICR1 *((int*)0x40013808)
#define SYSCFG_EXTICR3 *((int*)0x40013810)
#define EXTI_IMR *((int*)0x40013c00)
#define EXTI_FTSR *((int*)0x40013c0c)
#define EXTI_PR *((int*)0x40013c14)
#define NVIC_ISRE0 *((int*)0xE000E100)
void portA_C_init()
{
	RCC_AHB1ENR |= 0x5; //Enable port A  and port C clock
	while(!(RCC_AHB1ENR & 0x5));
	GPIOA_MODER &= ~0x3; //load 00 in 0-1 positions to make PA0 as input mode
	GPIOA_PUPDR &= ~0x3;
	GPIOA_PUPDR |= 0x1; //load 01 in 0-1 positions to activate Pull_Up resistor for PA0
	GPIOC_MODER &= (0xf<<16); //load 00 in 16-17 and 18-19 positions to make PC8 and PC9 as input
	GPIOC_PUPDR &= ~(0xf<<16);
	GPIOC_PUPDR |= 0x5<<16; //load 01 in PC8 and PC9 bits to enable pull_up resistor
}

void PA0_PC9_PC8_interrupt_init()
{
//Choose port
	RCC_APB2|=0x1<<14; //Enable clock for SYSCFG
	while(!(RCC_APB2 & 0x1<<14));
	SYSCFG_EXTICR1 |= 0x0; //Load 0000 in 0-3 positions for enable PA0 interrupt
	SYSCFG_EXTICR3 |= 0x22; //Load 0010 in 0-3 and 4-7 positions for enable PC8 and PC9 interrupt
//Enable device side
	EXTI_IMR |= 0x1; //set PA0 bit to non-mask in interrupt mask register 
	EXTI_IMR |= 0x3<<8; //set PC8 and PC9 positions
	EXTI_FTSR |= 0x1; //set PA0 bit to enable falling edge trigger
	EXTI_FTSR |= 0x3<<8; //set PC8,PC9 positions for falling edge
//Enable NVIC side
	NVIC_ISRE0 |=(0x1<<6); //set 6th position to enable EXT0 interrupt
	NVIC_ISRE0 |=(0x1<<23); //set 23rd position to anable EXT9_5 interrupt(PC8,PC9)
}
