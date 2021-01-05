//Whenever type G in tera term window green led is ON, whenever type 'R' in tera term window red led is ON. 
#define RCC_AHB1ENR *((int*)(0x40023830))//clock
#define GPIOA_MODER *((int*)(0x40020000))//green led mode 
#define GPIOA_ODR *((int*)(0x40020014))//green led output
#define GPIOC_MODER *((int*)(0x40020800))//red led mode
#define GPIOC_ODR *((int*)(0x40020814))//red led output
//#define GPIOC_IDR *((int*)(0x40020810))//idr port c
//#define GPIOC_PUPDR *((int*)(0x4002080c))//pupdr port c

void Sys_Tick_init(void);
void millisec_delay(int);
void UART_Init(void);
void UART_config();
void UART_OUT_char(unsigned char);
char UART_IN_char(void);
void green_red_Leds_Init(void)//intialisation setup
{
	RCC_AHB1ENR|=(0x1<<2);//set PORT C clock enable
	while(!RCC_AHB1ENR & (0x1<<2));//wait until port c is enable
	RCC_AHB1ENR|=(0x1);//set PORT A clock enable
	while(!RCC_AHB1ENR & (0x1));//wait until port a is enable
	GPIOC_MODER&=~(0x3<<10);//clear the 11th and 10th pins red led PC5 mode5
	GPIOC_MODER|=(0x1<<10);//Load "01" value in 11th and 10th pins for red led pc5 mode5
	GPIOA_MODER&=~(0x3<<16);//clear the 17th and 16th pins red led PA8 mode8
	GPIOA_MODER|=(0x1<<16);//Load "01" value in 17th and 16th pins for green led PA8 mode8
	//GPIOC_PUPDR &= ~(0x3<<16);//clear 17th_16th bits of pull up pull down register for switch 1 PC8
	//GPIOC_PUPDR |=(0X1<<16);//set 17th_16th bits of pull up pull down register for switch1 PC8
	//GPIOC_PUPDR &= ~(0x3<<18);//clear 19th_18th bits of pull up pull down register for switch2 PC9
	//GPIOC_PUPDR |=(0X1<<18);//set 19th_18th bits of pull up pull down register for switch2 PC9
	GPIOA_ODR|=(0x1<<8);//set green led pin (OFF)
	GPIOC_ODR|=(0x1<<5);//set red led pin (OFF)
	
}

int main()
{
UART_Init();
UART_config();
Sys_Tick_init();
green_red_Leds_Init();//intialisation setup
while(1)
 {
	if(UART_IN_char()=='R')
		{
			GPIOC_ODR &= ~(0X1<<5);//clear PC5 bit-> RED LED ON
			millisec_delay(200);
			GPIOC_ODR|=(0x1<<5);//set red led pin (OFF)
      millisec_delay(400);	
		}
	if(UART_IN_char()=='G')
		{
			GPIOA_ODR &= ~(0X1<<8);//clear PA8 bit-> GREEN LED ON
			millisec_delay(300);
			GPIOA_ODR|=(0x1<<8);//set green led pin (OFF)
			millisec_delay(500);
		}
 }
}
