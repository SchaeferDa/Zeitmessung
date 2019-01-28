/*
 * hwconf.c
 *
 * Created: 07-Jan-19 09:00:00
 * Authors: Daniel Schäfer, Christian Koch, Sebastian Schaffring
 * Contact: code@schaeferservices.de
 */ 

#ifndef HWCONF_H_
#define HWCONF_H_

/************************************************************************/
/* hardware defines for S1                                              */
/************************************************************************/
#define DDR_S1 DDRD
#define PIN_S1 PIND
#define PORT_S1 PORTD
#define MASK_S1 2
#define S1_TRIGGERED() ((PIN_S1 & (1 << MASK_S1)) == 0)

/************************************************************************/
/* hardware defines for S2                                              */
/************************************************************************/
#define DDR_S2 DDRD
#define PIN_S2 PIND
#define PORT_S2 PORTD
#define MASK_S2 3
#define S2_TRIGGERED() ((PIN_S2 & (1 << MASK_S2)) == 0)

/************************************************************************/
/* hardware defines for reset buttons                                   */
/************************************************************************/
#define DDR_BUTTON DDRD
#define PIN_BUTTON PIND
#define PORT_BUTTON PORTD
#define MASK_BUTTON 4
#define BUTTON_PRESSED() ((PIN_BUTTON & (1 << MASK_BUTTON)) == (1 << MASK_BUTTON))

/************************************************************************/
/* hardware defines for led                                             */
/************************************************************************/
#define DDR_LED DDRB
#define PIN_LED PINB
#define PORT_LED PORTB
#define MASK_LED 5
#define LED_TOGGLE() (PORT_LED = (PORT_LED ^ (1 << MASK_LED)))
#define LED_OFF() (PORT_LED = (PORT_LED & ~(1 << MASK_LED)))
#define LED_ON() (PORT_LED = (PORT_LED | (1 << MASK_LED)))

#endif /* HWCONF_H_ */