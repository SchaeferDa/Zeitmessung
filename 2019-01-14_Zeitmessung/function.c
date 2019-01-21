/*
 * function.c
 *
 * Created: 20.01.2019 16:26:47
 *  Author: Daniel Schäfer
 * Contact: code@schaeferservices.de
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "function.h"
#include "hwconf.h"
#include "LCD_student.h"

/************************************************************************/
/* initialize variables                                                 */
/************************************************************************/
volatile int counterMilliSeconds = 0;
volatile int counterSeconds = 0;
volatile int counterMinutes = 0;
volatile int counterHours = 0;
volatile int resetSeconds = 0;
boolean S1_state;
boolean S2_state;
int prev_msg = 0;
char a[16];
boolean Reset_state;

/************************************************************************/
/* configuration for Timer0                                             */
/************************************************************************/
void confTimer()
{
	// configure Timer 0 as "CTC mode"
	TCCR0A = TCCR0A & ~(1 << WGM00);
	TCCR0A = TCCR0A | (1 << WGM01);
	TCCR0B = TCCR0B & ~(1 << WGM02);
	
	// set prescaler to 64
	TCCR0B = TCCR0B & ~(1 << CS02);
	TCCR0B = TCCR0B | (1 << CS00 | 1 << CS01);
	
	// configure timer interrupt with compare value 249
	OCR0A = 249;
	TIMSK0 = TIMSK0 | (1 << OCIE0A);
}

/************************************************************************/
/* configuration for sensor 1                                           */
/************************************************************************/
void confSensor1()
{
	// configure sensor 1 as input
	DDR_S1 = DDR_S1 & ~(1 << MASK_S1);
	
	// configure sensor 1 interrupt as rising edge interrupt
	EICRA = EICRA | (1 << ISC01) | (1 << ISC00);
}

/************************************************************************/
/* configuration for sensor 2                                           */
/************************************************************************/
void confSensor2()
{
	// configure sensor 2 as input
	DDR_S2 = DDR_S2 & ~(1 << MASK_S2);
	
	// configure sensor 2 interrupt as rising edge interrupt
	EICRA = EICRA | (1 << ISC11) | (1 << ISC10);
}

/************************************************************************/
/* interrupt service routine for timer                                  */
/************************************************************************/
ISR(TIMER0_COMPA_vect)
{
	counterMilliSeconds++;
	
	if(counterMilliSeconds >= 1000)
	{
		counterSeconds++;
		counterMilliSeconds = 0;
		
		if(BUTTON_PRESSED())
		{
			resetSeconds++;
		}
		else if(!BUTTON_PRESSED())
		{
			resetSeconds = 0;
		}
		
		if(resetSeconds >= RESET_TIME)
		{
			Reset_state = TRUE;
			resetSeconds = 0;
		}
	}
	
	if(counterSeconds >= 60)
	{
		counterMinutes++;
		counterSeconds = 0;
	}
	
	if(counterMinutes >= 60)
	{
		counterHours++;
		counterMinutes = 0;
	}
}

/************************************************************************/
/* interrupt service routine for sensor 1                               */
/************************************************************************/
ISR(INT0_vect)
{
	S1_state = TRUE;
}

/************************************************************************/
/* interrupt service routine for sensor 2                               */
/************************************************************************/
ISR(INT1_vect)
{
	S2_state = TRUE;
}

/************************************************************************/
/* function for resetting Timer0                                        */
/************************************************************************/
void resetTimer()
{
	counterMilliSeconds = 0;
	counterSeconds = 0;
	counterMinutes = 0;
	counterHours = 0;
	TCNT0 = 0;
}

/************************************************************************/
/*  controls interrupts                                                 */
/************************************************************************/
void controlInterrupt(int sensor, boolean state)
{
	if(sensor == 1)
	{
		if(state)
		{
			EIMSK = EIMSK | (1 << INT0);
		}
		else
		{
			EIMSK = EIMSK & ~(1 << INT0);
		}
		
	}
	else if(sensor == 2)
	{
		if(state)
		{
			EIMSK = EIMSK | (1 << INT1);
		}
		else
		{
			EIMSK = EIMSK & ~(1 << INT1);
		}
		
	}
}

/************************************************************************/
/* controls LCD output                                                  */
/************************************************************************/
void controlLCD(int msg)
{
	
	if((msg != prev_msg))
	{
		LCD_cls();
		switch(msg)
		{
			case LCDinitSystem:
			LCD_setPosition(0, 0);
			LCD_printString("initialize");
			LCD_setPosition(1, 0);
			LCD_printString("system ...");
			break;
			
			case LCDcheckSensor1:
			LCD_setPosition(0, 0);
			LCD_printString("Please trigger");
			LCD_setPosition(1, 0);
			LCD_printString("Sensor 1");
			break;
			
			case LCDcheckSensor2:
			LCD_setPosition(0, 0);
			LCD_printString("Please trigger");
			LCD_setPosition(1, 0);
			LCD_printString("Sensor 2");
			break;
			
			case LCDsensorError:
			LCD_setPosition(0, 0);
			LCD_printString("Error while");
			LCD_setPosition(1, 0);
			LCD_printString("checking sensors.");
			break;
			
			case LCDsensorSuccess:
			LCD_setPosition(0,0);
			LCD_printString("Successfully");
			LCD_setPosition(1,0);
			LCD_printString("checked.");
			break;
			
			case LCDwaitToStart:
			LCD_setPosition(0,0);
			LCD_printString("Ready for");
			LCD_setPosition(1,0);
			LCD_printString("measurement.");
			break;
			
			case LCDstartMeasure:
			LCD_setPosition(0, 0);
			LCD_printString("Measurement");
			LCD_setPosition(1, 0);
			LCD_printString("started.");
			break;
			
			case LCDstopMeasure:
			LCD_setPosition(0,0);
			LCD_printString("Measure stopped");
			LCD_setPosition(1,0);
			LCD_printString(a);
			break;
		}
	}
	
	if(msg == LCDMeasureTime)
	{
		LCD_setPosition(0, 0);
		LCD_printString("Current time:");
		LCD_setPosition(1,0);
		LCD_printString(a);
	}
				
	prev_msg = msg;
}

/************************************************************************/
/* control functions for LCD                                            */
/************************************************************************/
void LCD_cls()
{
	LCD_clear_screan();
}
void LCD_setPosition(unsigned char a, unsigned char b) 
{
	set_Curser_position(b, a);
}
void LCD_printString(char *a)
{
	LCD_print_String(a);
}
	
/************************************************************************/
/* Getter and Setter                                                    */
/************************************************************************/
int getCounterMilliSeconds()
{
	return counterMilliSeconds;
}
int getCounterSeconds()
{
	return counterSeconds;
}
int getCounterMinutes()
{
	return counterMinutes;
}
int getCounterHours()
{
	return counterHours;
}
boolean getS1_state()
{
	return S1_state;
}
boolean getS2_state()
{
	return S2_state;
}
void setS1_state(boolean S1Set)
{
	S1_state = S1Set;
}
void setS2_state(boolean S2Set)
{
	S2_state = S2Set;
}
void setA(char aSet[16])
{
	strncpy(a, aSet, 16);
}

boolean getReset_state()
{
	return Reset_state;
}

void setReset_state(boolean stateSet)
{
	Reset_state = stateSet;
}