/*
 * main.c
 *
 * Created: 14-Jan-19 09:00:00
 * Authors: Daniel Schäfer, Christian Koch
 * Contact: code@schaeferservices.de
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "hwconf.h"
#include "function.h"
#include "LCD_student.h"

int nextState = 0;
volatile int measureMilliSeconds = 0;
volatile int measureSeconds = 0;
volatile int measureMinutes = 0;
volatile int measureHours = 0;
volatile int chaseSeconds = 0;
volatile int outputMilliSeconds = 0;
char o[16];

int main(void)
{	
	nextState = initSystem;
	
    while (1) 
    {
		/************************************************************************/
		/* implements reset function                                            */
		/************************************************************************/
		if(getReset_state())
		{
			nextState = initSystem;
			setReset_state(FALSE);
		}
		
		switch(nextState)
		{
			/************************************************************************/
			/* initialize system and printout this on LCD                           */
			/************************************************************************/
			case initSystem:
				LCD_init();
				confSensor1();
				confSensor2();
				controlLCD(LCDinitSystem);
				LED_OFF();
				nextState = initAndStartTimer;
				break;
			
			/************************************************************************/
			/* config and start timer                                               */
			/************************************************************************/
			case initAndStartTimer:
				confTimer();
				nextState = globalIntOn;
				break;
			
			/************************************************************************/
			/* enable global interrupts                                             */
			/************************************************************************/
			case globalIntOn:
				sei();
				nextState = checkSensor1;
				break;
			
			/************************************************************************/
			/* check sensor 1                                                       */
			/************************************************************************/
			case checkSensor1:
				controlLCD(LCDcheckSensor1);
				
				controlInterrupt(1, TRUE);
			
				resetTimer();
				nextState = sensorError;
				setS1_state(FALSE);
		
				while(getCounterSeconds() <= SENSOR_CHECK_TIME)
				{
					if(getS1_state())
					{
						nextState = checkSensor2;
						setS1_state(FALSE);
						break;
					}
				}
				controlInterrupt(1, FALSE);
				break;
			
			/************************************************************************/
			/* check sensor 2                                                       */
			/************************************************************************/
			case checkSensor2:
				controlLCD(LCDcheckSensor2);
				
				controlInterrupt(2, TRUE);
			
				resetTimer();
				nextState = sensorError;
				setS2_state(FALSE);
			
				while(getCounterSeconds() <= SENSOR_CHECK_TIME)
				{
					if(getS2_state())
					{
						nextState = sensorSuccess;
						setS2_state(FALSE);
						break;
					}
				}
				controlInterrupt(2, FALSE);
				break;
			
			/************************************************************************/
			/* printout sensor error message on LCD                                 */
			/************************************************************************/
			case sensorError:
				controlLCD(LCDsensorError);
				
				if(BUTTON_PRESSED())
				{
					nextState = checkSensor1;
				}
				break;
			
			/************************************************************************/
			/* printout sensor success message on LCD                               */
			/************************************************************************/
			case sensorSuccess:
				controlLCD(LCDsensorSuccess);
				nextState = sensorCombiBegin;
				break;
			
			/************************************************************************/
			/* enable sensor 1 and disable sensor 2                                 */
			/************************************************************************/
			case sensorCombiBegin:
				controlInterrupt(1,TRUE);
				controlInterrupt(2, FALSE);
				
				setS1_state(FALSE);
				
				nextState = waitToStart;
				break;
			
			/************************************************************************/
			/* wait for trigger on sensor 2                                         */
			/************************************************************************/
			case waitToStart:
				controlLCD(LCDwaitToStart);
				
				if(getS1_state())
				{
					nextState = startMeasure;
					setS1_state(FALSE);
				}
				break;
			
			/************************************************************************/
			/* start measurement, printout this on LCD and start chase              */
			/************************************************************************/
			case startMeasure:
				controlLCD(LCDstartMeasure);
				
				resetTimer();
				
				LED_ON();
				
				chaseSeconds = ((getCounterSeconds()) + CHASE_TIME);
				nextState = chaseTrigger;
				break;
			
			/************************************************************************/
			/* wait for successful chase                                            */
			/************************************************************************/
			case chaseTrigger:
				nextState = sensorCombiEnd;
				while(getCounterSeconds() <= chaseSeconds)
				{
					if(getS1_state())
					{
						nextState = restartChase;
						setS1_state(FALSE);
						setS2_state(FALSE);
						break;
					}
				}
				
				break;
			
			/************************************************************************/
			/* restart chase                                                        */
			/************************************************************************/
			case restartChase:
				chaseSeconds = getCounterSeconds() + CHASE_TIME;
				nextState = chaseTrigger;
				break;
			
			/************************************************************************/
			/* enable sensor 2 and disable sensor 1                                 */
			/************************************************************************/
			case sensorCombiEnd:
				controlInterrupt(1, FALSE);
				controlInterrupt(2, TRUE);
				setS1_state(FALSE);
				setS2_state(FALSE);
				
				nextState = waitToStop;				
				break;
				
			/************************************************************************/
			/* wait for trigger on sensor 2                                         */
			/************************************************************************/
			case waitToStop:
				if(getS2_state())
				{
					nextState = stopMeasure;
				}
				
				if((getCounterMilliSeconds() % (OUTPUT_TIME+1)) == OUTPUT_TIME)
				{					
					sprintf(o, "%02i:%02i:%02i:%03i", getCounterHours(), getCounterMinutes(), getCounterSeconds(), getCounterMilliSeconds());
					setA(o);
					
					controlLCD(LCDMeasureTime);
					LED_TOGGLE();
				}
				
				break;
				
			/************************************************************************/
			/* stop measurement and printout time on LCD                            */
			/************************************************************************/
			case stopMeasure:
				measureMilliSeconds = getCounterMilliSeconds();
				measureSeconds = getCounterSeconds();
				measureMinutes = getCounterMinutes();
				measureHours = getCounterHours();
				
				LED_OFF();
				
				sprintf(o, "%02i:%02i:%02i:%03i", measureHours, measureMinutes, measureSeconds, measureMilliSeconds);
				setA(o);
				
				controlLCD(LCDstopMeasure);
			
				nextState = waitToReset;
				break;
			
			/************************************************************************/
			/* wait for reset                                                       */
			/************************************************************************/
			case waitToReset:
				if(BUTTON_PRESSED())
				{
					nextState = sensorCombiBegin;
					break;
				}
				break;
		}
    }
}