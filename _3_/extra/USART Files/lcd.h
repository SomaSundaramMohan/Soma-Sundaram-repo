#include"sys_tick.h"
#define RCC_AHB1ENR *((int*)(0x40023830))
#define GPIOB_MODER *((int*)(0x40020400))
#define GPIOB_ODR *((int*)(0x40020414))
#define EN (0x1<<8) //PB8
#define RS (0x1<<4) //PB4
#define RW (0x1<<5) //PB5
int i;
char str[10];
char* ftoa(float val) //float to ascii
{
	int i=0,n=val*100.0,x=0,num,c=0;
	if(n<0)
		{
		c=1;
		num=(-n);
		}
		else 
			num=n;
	if(n==0)
	{
		str[i]='0';
		str[i+1]=0;
		return str;
	}
	while(n)
	{
		i++;
		n/=10;
	}
	i++;
	if(c)
		i++;
	str[i]=0;
	for(i--,x=1;x<=2;x++,i--)
	{
		str[i]=num%10+48;
		num/=10;
	}
	str[i]='.';
	for(i--;i>=0;i--)
	{
		str[i]=num%10+48;
		num/=10;
		if(c)
			str[0]='-';
		
	}
	return str ;

}
char* itoa(int x)// int to ascii
{
	int i=0,n=x;
	if(x==0)
	{
		str[i]=48;
		str[i+1]=0;
		return str;
	}
	while(n)
	{
		i++;
		n=n/10;
	}
	str[i]=0;
	for(i--;i>=0;i--)
	{
		str[i]=x%10+48;
		x/=10;
	}
	return str;
}
void enable() //enables EN for 10ms
{
  GPIOB_ODR |= EN;
	ml_Delay(10);
	GPIOB_ODR &= ~EN;
}
void write_low_nibble(char c)
{
	GPIOB_ODR &= ~(0xf); //clearing D0-D3 positions
	GPIOB_ODR |= (c & 0xf); //loading low_nibble
	enable();
	ml_Delay(2);
}
void write_high_nibble(char c)
{
	GPIOB_ODR &= ~(0xf); //clearing D0-D3 positions
	GPIOB_ODR |= (c>>4); //loading high_nibble
	enable(); 
	ml_Delay(2);
	
}
void LCD_write_cmd(char c)
{ 
	GPIOB_ODR &= ~RS; // PB4-RS=0 to select data register
  write_high_nibble(c);
	write_low_nibble(c);
}
void LCD_write_data(char c)
{
	GPIOB_ODR |= RS; //PB4-RS=1 to select command register mode
  write_high_nibble(c);
	write_low_nibble(c);
}
void disp_string(char str[])
{
	for(i=0;str[i]!=0;i++) //Display str1
	LCD_write_data(str[i]);
}
void CreateCustomCharacter (char *ch, const char Location)
{ 
int i=0; 
LCD_write_cmd(0x40+(Location*8));     //Send the Address of CGRAM
//disp_string(ch);
	for (i=0; i<8; i++)
  LCD_write_data(ch[i]);         //Pass the bytes of pattern on LCD 
}
void LCD_init()
{
	//PORTB_initialization
	RCC_AHB1ENR |= 0x2; //set port b clock
	while (!(RCC_AHB1ENR & 0x2));
	GPIOB_MODER &= ~(0xfff);//clear PB0,B1,B2,B3,B4,B5 positions
	GPIOB_MODER &= ~(0x3<<8);//clear PB8 position
	GPIOB_MODER |= 0x15; //load 01 in PB0,PB1,PB2
	GPIOB_MODER |= 0x15<<6;//load 01 in PB3,PB4,PB5
	GPIOB_MODER |= 0x1<<16; //load 01 in PB8
	//LCD initialization
	GPIOB_ODR &= ~RS; //(PB4) RS=0 
	GPIOB_ODR &= ~RW; //(PB5) R/W=0
	GPIOB_ODR &= ~EN;  //(PB8) EN=0
	ml_Delay(20);
	LCD_write_cmd(0x02); //4 bit mode (0x38 for 8 bit mode)
  LCD_write_cmd(0x28); //enacle 2 line
  LCD_write_cmd(0x0c); //Display On cursor off
	LCD_write_cmd(0x06);//Entry mode
	LCD_write_cmd(0x01);//clear LCD
	LCD_write_cmd(0x80);
}

