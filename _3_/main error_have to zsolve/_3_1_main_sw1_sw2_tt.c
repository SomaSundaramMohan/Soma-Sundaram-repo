//Experiment 3: Whenever SW1 is pressed check 'R' in tera term window, whenever SW2 is pressed check 'G' in tera term window. 
#define RCC_AHB1ENR *((int*)(0x40023830))//clock
#define GPIOC_MODER *((int*)(0x40020800))
#define GPIOC_IDR *((int*)(0x40020810))//idr port c
#define GPIOC_PUPDR *((int*)(0x4002080c))//pupdr port c
void Sys_Tick_init(void);
void millisec_delay(int);
void UART_Init(void);
void UART_config();
void UART_OUT_char(unsigned char);
char UART_IN_char(void);
//char temp;
void sw1_sw2_Leds_Init(void)//intialisation setup
{
	RCC_AHB1ENR|=(0x1<<2);//set PORT C clock enable
	while(!RCC_AHB1ENR & (0x1<<2));//wait until port c is enable
	GPIOC_MODER&=~(0x3<<16);//clear the 17th and 16th pins of pc8
	GPIOC_MODER&=~(0x3<<18);//clear the 19th and 18th pins of pc9
	GPIOC_PUPDR &= ~(0x3<<16);//clear 17th_16th bits of pull up pull down register for switch 1 PC8
	GPIOC_PUPDR |=(0X1<<16);//set 17th_16th bits of pull up pull down register for switch1 PC8
	GPIOC_PUPDR &= ~(0x3<<18);//clear 19th_18th bits of pull up pull down register for switch2 PC9
	GPIOC_PUPDR |=(0X1<<18);//set 19th_18th bits of pull up pull down register for switch2 PC9
}

int main()
{
UART_Init();
UART_config();
Sys_Tick_init();
sw1_sw2_Leds_Init();//intialisation setup
while(1)
 {
	if(!(GPIOC_IDR & (0X1<<9)))//switch 1 PC9
		{
	
		UART_OUT_char('R');
		//temp=UART_IN_char();
		millisec_delay(100);

		}
	if(!(GPIOC_IDR & (0X1<<8)))//switch 2 PC8
		{
		UART_OUT_char('G');
		//temp=UART_IN_char();
		millisec_delay(100);
		}
 }
}
