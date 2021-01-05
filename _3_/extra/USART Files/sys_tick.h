#define STK_CTRL *((int*)0xE000E010)
#define STK_LOAD *((int*)0xE000E014)
#define STK_VAL *((int*)0xE000E018)

void ml_Delay(int d)
{
	STK_LOAD = (16000*d); //16000 for 1ms delay
	while(!(STK_CTRL&(0x1<<16))); //check counter_flag (16th bit)
}
void mic_Delay(int d)
{
	STK_LOAD = (16*d); //16 for 1 mic secs delay
	while(!(STK_CTRL&(0x1<<16))); //check counter_flag (16th bit)
}
void Sys_Tick_Init()
{
	STK_CTRL |= 0x05; //101 for polling
	STK_VAL = 0x123; //write any value to clear 
}

/*
16000 - 1ms delay(0.001sec)
160000 - 10ms delay(0.01sec)
1600000 - 100ms delay(0.1sec)
16000000 - 1000ms delay(1sec)
16777215 - 1sec max delay
*/

