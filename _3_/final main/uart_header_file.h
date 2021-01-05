//SYSTICK TIMER FUNCTION DECLARATIONS
void milli_delay(int );
void Sys_Tick_Init(void);
void micro_delay(int d);

//UART FUNTION DECLARATIONS
void portc_USART6_init(void);
void USART6_config(void);
void UART6_transmit_data(unsigned char );
char UART6_receive_data(void);

//SW1 SW2 LEDs INITIALIZATION
void sw1_sw2_Leds_Init(void);
