
#define STK_CTRL *((int*)(0xE000E010))
#define STK_LOAD *((int*)(0xE000E014))
#define STK_VAL  *((int*)(0xE000E018))

void millisec_delay(int d)
{
	int i;
	STK_LOAD = (16000); //16000 for 1ms delay
	for(i=0;i<d;i++)
	{
	while(!(STK_CTRL&(0x1<<16))); //check counter_flag (16th bit)
	}
	
}
void microsec_delay(int d)
{
	int i;
	STK_LOAD = (16); //16 for 1 micro secs delay
	for(i=0;i<d;i++)
	{
	while(!(STK_CTRL&(0x1<<16))); //check counter_flag (16th bit)
	}
}

void Sys_Tick_init()
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
