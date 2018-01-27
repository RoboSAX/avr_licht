/******************************************************************************
* Test/tht/lichter_test.c                                                     *
* =======================                                                     *
*                                                                             *
* Version: 1.0.0                                                              *
* Date   : 26.01.18                                                           *
* Author : Peter Weissig, Tina Lenk                                           *
*                                                                             *
******************************************************************************/


//**************************<Included files>***********************************
#include <avr/io.h>
#include <inttypes.h>

#include "lichter.h"


//**************************<Macros>*******************************************
#define led1_setRed(x)   ( x ? (PORTB|= _BV(1)) : (PORTB&= ~_BV(1)))
#define led1_setGreen(x) ( x ? (PORTB|= _BV(2)) : (PORTB&= ~_BV(2)))
#define led1_setBlue(x)  ( x ? (PORTD|= _BV(5)) : (PORTD&= ~_BV(5)))
#define led1_setIr(x)    ( x ? (PORTD|= _BV(3)) : (PORTD&= ~_BV(3)))

#define led1_readRed()   ((PORTB & _BV(1)) != 0x00)
#define led1_readGreen() ((PORTB & _BV(2)) != 0x00)
#define led1_readBlue()  ((PORTD & _BV(5)) != 0x00)
#define led1_readIr()    ((PORTD & _BV(3)) != 0x00)


#define led2_setRed(x)   ( x ? (PORTC|= _BV(0)) : (PORTC&= ~_BV(0)))
#define led2_setGreen(x) ( x ? (PORTC|= _BV(1)) : (PORTC&= ~_BV(1)))
#define led2_setBlue(x)  ( x ? (PORTC|= _BV(2)) : (PORTC&= ~_BV(2)))
#define led2_setIr(x)    ( x ? (PORTB|= _BV(3)) : (PORTB&= ~_BV(3)))

#define led2_readRed()   ((PORTC & _BV(0)) != 0x00)
#define led2_readGreen() ((PORTC & _BV(1)) != 0x00)
#define led2_readBlue()  ((PORTC & _BV(2)) != 0x00)
#define led2_readIr()    ((PORTB & _BV(3)) != 0x00)


#define button_getBtnmode() ((PIND & _BV(4)) == 0x00)
#define button_getBtn1()    ((PINB & _BV(0)) == 0x00)
#define button_getBtn2()    ((PINC & _BV(3)) == 0x00)


//**************************<Prototypes>***************************************
void init_hardware(void);
void leds_clearAll(void);
void leds_initTest(void);
int main (void);


//**************************<Methods>******************************************

//**************************[init_hardware]************************************
void init_hardware(void) {

    // set leds to output
    DDRB = _BV(1) |  // led1 red
           _BV(2) |  // led1 green
           _BV(3);   // led2 IR
    DDRC = _BV(0) |  // led2 red
           _BV(1) |  // led2 green
           _BV(2);   // led2 blue
    DDRD = _BV(3) |  // led1 IR
           _BV(5);   // led1 blue
}

//**************************[leds_clearAll]************************************
void leds_clearAll(void) {

    led1_setRed  (0);
    led1_setGreen(0);
    led1_setBlue (0);
    led1_setIr   (0);

    led2_setRed  (0);
    led2_setGreen(0);
    led2_setBlue (0);
    led2_setIr   (0);
}

//**************************[leds_initTest]************************************
void leds_initTest(void) {

    leds_clearAll();

    // RGB 1
    led1_setRed  (1);                   delay_ms(500);
    led1_setGreen(1); led1_setRed  (0); delay_ms(500);
    led1_setBlue (1); led1_setGreen(0); delay_ms(500);
                      led1_setBlue (0);

    // RGB 2
    led2_setRed  (1);                   delay_ms(500);
    led2_setGreen(1); led2_setRed  (0); delay_ms(500);
    led2_setBlue (1); led2_setGreen(0); delay_ms(500);
                      led2_setBlue (0);

    // IR1 && IR2
    led1_setIr   (1);                   delay_ms(500);
    led2_setIr   (1); led1_setIr   (0); delay_ms(500);
    led1_setIr   (1); led2_setIr   (0); delay_ms(500);
    led2_setIr   (1); led1_setIr   (0); delay_ms(500);
                      led2_setIr   (0);
}

//**************************[main]*********************************************
int main (void) {

    // initialize hardware
    init_hardware();


    // initial test
    leds_initTest();

    // test buttons + leds
    uint8_t i = 0;
    uint8_t flag;

    while (1) {
        flag = 0;

        // buttons 1+2
        if (button_getBtn1()) {
            led1_setBlue(1);

            flag = 1;
        } else {
            led1_setBlue(0);
        }

        if (button_getBtn2()) {
            led2_setBlue(1);

            flag = 1;
        } else {
            led2_setBlue(0);
        }

        // button mode
        if (button_getBtnmode()) {
            led1_setRed(1);
            led2_setRed(1);

            flag = 1;
        } else {
            led1_setRed(0);
            led2_setRed(0);
        }

        // optional heartbeat (green)
        if (flag == 0) {
            if (i == 0) {
                led1_setGreen(1);
                led2_setGreen(0);
            } else if (i == 50) {
                led2_setGreen(1);
                led1_setGreen(0);
            }
        } else {
            led1_setGreen(0);
            led2_setGreen(0);
        }

        // heartbeat (ir)
        if (i == 0) {
            led1_setIr(1);
            led2_setIr(0);
        } else if (i == 50) {
            led2_setIr(1);
            led1_setIr(0);
        }

        i++;
        if (i > 100) { i = 0;}

        // main delay
        delay_ms(10);
    }

    return (0);

}
