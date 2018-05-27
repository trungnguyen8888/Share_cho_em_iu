#include <stdio.h>
#include <unistd.h>

/************************************************************************/
/* Function prototypes */
void lcd_init(void);
void lcd_instruction(unsigned char value);
void lcd_data(unsigned char value);
void lcd_send_string(unsigned char *strptr);
void lcd_enable(void);
void lcd_write(unsigned char value);

/************************************************************************/
/* Macro definitions */
#define   HOME 	0x80		    		
#define   NXTL	0xC0
#define   CLS	0x01

/************************************************************************/
/* Global variables */
unsigned char initLCD[ ]={0x02, 0x28, 0x0C, 0x06, 0x01, 0x80, 0x00};

/******************************************************************************
*
* Function Name    : main
* Comments     : The main task initializes pins as GPIOs in output mode
*
*
******************************************************************************/

int main(void)
{
  printf("\n******** LCD 16x2  running on UDOO Neo **********\n");

  //Export 6 used pines (E, RS and 4-bit bus)
  system("echo 148 > /sys/class/gpio/export");  //E
  system("echo 105 > /sys/class/gpio/export");  //RS
  system("echo 149 > /sys/class/gpio/export");  //DB7
  system("echo 140 > /sys/class/gpio/export");  //DB6
  system("echo 141 > /sys/class/gpio/export");  //DB5
  system("echo 142 > /sys/class/gpio/export");  //DB4
 
  //Configure pins as Output
  system("echo out > /sys/class/gpio/gpio148/direction");
  system("echo out > /sys/class/gpio/gpio105/direction");
  system("echo out > /sys/class/gpio/gpio149/direction");
  system("echo out > /sys/class/gpio/gpio140/direction");
  system("echo out > /sys/class/gpio/gpio141/direction");
  system("echo out > /sys/class/gpio/gpio142/direction");

  /* LCD initialization and sending messages */
  lcd_init();
  lcd_send_string("Hello i.MX6SX!");
  lcd_instruction(NXTL);
  lcd_send_string("LCD on UDOO Neo");

  printf("\n~~~~~~~~~~~~~~~~~ DEMO COMPLETED ~~~~~~~~~~~~~~~~\n\n");
return 0;
}

/******************************************************************************
*
* Function Name  : lcd_init
* Comments       : Initializes the LCD on 4-bit mode
*
******************************************************************************/
void lcd_init(void)
{
  unsigned char strindex=0;

  while(initLCD[strindex])
  {
    lcd_instruction(initLCD[strindex]);
    strindex++;
  }
}

/******************************************************************************
*
* Function Name  : lcd_instruction
* Comments       : Sends instructions to LCD (RS = 0)
*
******************************************************************************/
void lcd_instruction(unsigned char value)
{
  system("echo 0 > /sys/class/gpio/gpio105/value");   /* GPIO105 used as RS */	
	
  lcd_write( value&0xF0 );
  lcd_enable();
	
  lcd_write( (value<<4)&0xF0 );
  lcd_enable();
}

/******************************************************************************
*
* Function Name  : lcd_data
* Comments       : Sends data to LCD (RS = 1)
*
******************************************************************************/
void lcd_data(unsigned char value)
{
  system("echo 1 > /sys/class/gpio/gpio105/value");   /* GPIO105 used as RS */	
	
  lcd_write( value&0xF0 );
  lcd_enable();
	
  lcd_write( (value<<4)&0xF0 );
  lcd_enable();
}

/******************************************************************************
*
* Function Name  : lcd_send_string
* Comments       : Sends a string of characters to LCD
*
******************************************************************************/
void lcd_send_string(unsigned char *strptr)
{
  while(*strptr)
  {
    lcd_data(*strptr);
    strptr++;
  }
}

/******************************************************************************
*
* Function Name  : lcd_enable
* Comments       : Sends the enable pulse to LCD
*
******************************************************************************/
void lcd_enable(void)
{
  system("echo 1 > /sys/class/gpio/gpio148/value");   /* GPIO19 used as ENABLE */	
  //_time_delay(5);
  system("echo 0 > /sys/class/gpio/gpio148/value");   /* GPIO19 used as ENABLE */	
}

/******************************************************************************
*
* Function Name  : lcd_write
* Comments       : Sends the bits to the LCD bus
*
******************************************************************************/
void lcd_write(unsigned char value)
{
  //Bit 7
  if (value&0x80)   system("echo 1 > /sys/class/gpio/gpio149/value");
  else  system("echo 0 > /sys/class/gpio/gpio149/value");
  
  //Bit 6
  if (value&0x40)system("echo 1 > /sys/class/gpio/gpio140/value");
  else  system("echo 0 > /sys/class/gpio/gpio140/value");
  
  //Bit 5
  if (value&0x20) system("echo 1 > /sys/class/gpio/gpio141/value");
  else  system("echo 0 > /sys/class/gpio/gpio141/value");

  //Bit 4
  if (value&0x10) system("echo 1 > /sys/class/gpio/gpio142/value");
  else  system("echo 0 > /sys/class/gpio/gpio142/value");
}

/* EOF */
