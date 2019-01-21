/*
 * function.h
 *
 * Created: 20.01.2019 16:26:34
 *  Author: Daniel Schäfer
 * Contact: code@schaeferservices.de
 */ 

#ifndef FUNCTION_H_
#define FUNCTION_H_

typedef enum {FALSE, TRUE} boolean;
	
/************************************************************************/
/* setting for sensor check time in seconds                             */
/************************************************************************/
#define SENSOR_CHECK_TIME 10

/************************************************************************/
/* setting for chase time in seconds                                    */
/************************************************************************/
#define CHASE_TIME 2

/************************************************************************/
/* setting for LCD output time in milliseconds                          */
/************************************************************************/
#define OUTPUT_TIME 10

/************************************************************************/
/* setting for reset time in seconds                                    */
/************************************************************************/
#define RESET_TIME 3

/************************************************************************/
/* declare states of state machine                                      */
/************************************************************************/
enum
{
	initSystem, initAndStartTimer, globalIntOn, checkSensor1, checkSensor2, sensorCombiBegin, sensorError, sensorSuccess,
	waitToStart, startMeasure, chaseTrigger, restartChase, sensorCombiEnd, waitToStop, stopMeasure, waitToReset
};

/************************************************************************/
/* declare LCD outputs                                                  */
/************************************************************************/
enum
{
	none, LCDinitSystem, LCDcheckSensor1, LCDcheckSensor2, LCDsensorError, LCDsensorSuccess, LCDwaitToStart, 
	LCDstartMeasure, LCDMeasureTime, LCDstopMeasure
};

/************************************************************************/
/* prototypes of functions                                              */
/************************************************************************/
void resetTimer();
void confSensor1();
void confSensor2();
void confTimer();
void LCD_cls();
void LCD_setPosition(unsigned char a, unsigned char b);
void LCD_printString(char *a);
void controlInterrupt(int sensor, boolean state);
void controlLCD(int msg);
int getCounterMilliSeconds();
int getCounterSeconds();
int getCounterMinutes();
int getCounterHours();
boolean getS1_state();
boolean getS2_state();
boolean getReset_state();
void setReset_state(boolean stateSet);
void setS1_state(boolean S1Set);
void setS2_state(boolean S2Set);
void setA(char aSet[16]);

#endif /* FUNCTION_H_ */