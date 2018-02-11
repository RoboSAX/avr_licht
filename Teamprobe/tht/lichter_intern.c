/******************************************************************************
* Teamprobe/tht/lichter_intern.c                                              *
* ==============================                                              *
*                                                                             *
* Version: 1.0.2                                                              *
* Date   : 09.02.18                                                           *
* Author : Peter Weissig, Tina Lenk, Leander Herr                             *
*                                                                             *
******************************************************************************/


//**************************<Included files>***********************************
#include "lichter_intern.h"


//**************************<Macros>*******************************************
#define leds_setIr1(x)    ( x ? (PORTD|= _BV(3)) : (PORTD&= ~_BV(3)))
#define leds_setIr2(x)    ( x ? (PORTB|= _BV(3)) : (PORTB&= ~_BV(3)))

#define buttons_getBtnMode() ((PIND & _BV(4)) == 0x00)
#define buttons_getBtn1()    ((PINB & _BV(0)) == 0x00)
#define buttons_getBtn2()    ((PINC & _BV(3)) == 0x00)


//**************************<Types and Variables>******************************
struct sButtonState {
    uint8_t state1    : 1;
    uint8_t state2    : 1;
    uint8_t stateMode : 1;

    uint8_t flank1    : 1;
    uint8_t flank2    : 1;
    uint8_t flankMode : 1;

    uint8_t countdown1   ;
    uint8_t countdown2   ;
    uint8_t countdownMode;
};
struct sButtonState buttons;

struct sIrLeds {
    uint8_t state1    : 1;
    uint8_t state2    : 1;
    uint8_t counter   : 1;
};
volatile struct sIrLeds ir_leds;


//**************************<Prototypes>***************************************
void buttons_reset(void);


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

    // set buttons to input (disable pullups)
    DDRB &= ~_BV(0);
    DDRC &= ~_BV(3);
    DDRD &= ~_BV(4);

    // Timer 2 (IR-led)
    TCCR2A = _BV(WGM21);  // Output-Mode: none
                          // Operation-Mode: CTC til OCRA
    TCCR2B = _BV(CS20);   // Operation-Mode: CTC til OCRA
                          // Prescaler: 1
    OCR2A = 210;          // Top: 210 ==> 76KHz
    TIMSK2 = _BV(OCIE2A); // enable compare match interrupt

    // startup time
    delay_ms(50);

    // initialize variables
    buttons_reset();

    // enable interrupts
    sei();
}

//**************************[leds_clearAll]************************************
void leds_clearAll(void) {

    // schaltet alle LEDs aus
    // LED von beiden Teams starten in jedem Modus im ausgeschalteten Zustand
    led1_setRed(0);
    led1_setGreen(0);
    led1_setBlue(0);
    led1_setIr(0);

    led2_setRed(0);
    led2_setGreen(0);
    led2_setBlue(0);
    led2_setIr(0);
}

//**************************[leds_initTest]************************************
void leds_initTest(void) {

    leds_clearAll();


    // RGB 1 & IR1
    led1_setIr   (1);

    led1_setRed  (1);                   delay_licht(500);
    led1_setGreen(1); led1_setRed  (0); delay_licht(500);
    led1_setBlue (1); led1_setGreen(0); delay_licht(500);
                      led1_setBlue (0);

                      led1_setIr   (0);


    // RGB 2 & IR2
    led2_setIr   (1);

    led2_setRed  (1);                   delay_licht(500);
    led2_setGreen(1); led2_setRed  (0); delay_licht(500);
    led2_setBlue (1); led2_setGreen(0); delay_licht(500);
                      led2_setBlue (0);

                      led2_setIr   (0);

    leds_clearAll();
}

//**************************[led1_setIr]***************************************
void led1_setIr(uint8_t state) {

    ir_leds.state1 = state;
}

//**************************[led2_setIr]***************************************
void led2_setIr(uint8_t state) {

    ir_leds.state2 = state;
}

//**************************[buttons_reset]************************************
void buttons_reset(void) {

    buttons.state1    = 0;
    buttons.state2    = 0;
    buttons.stateMode = 0;

    buttons_clearAll();

    buttons.countdown1    = 0;
    buttons.countdown2    = 0;
    buttons.countdownMode = 0;
}

//**************************[buttons_clearAll]*********************************
void buttons_clearAll(void) {

    buttons.flank1    = 0;
    buttons.flank2    = 0;
    buttons.flankMode = 0;
}

//**************************[button1_readFlank]********************************
uint8_t button1_readFlank(void) {

    uint8_t result = buttons.flank1;
    buttons.flank1 = 0;

    return result;
}

//**************************[button2_readFlank]********************************
uint8_t button2_readFlank(void) {

    uint8_t result = buttons.flank2;
    buttons.flank2 = 0;

    return result;
}

//**************************[buttonMode_readFlank]*****************************
uint8_t buttonMode_readFlank(void) {

    uint8_t result = buttons.flankMode;
    buttons.flankMode = 0;

    return result;
}

//**************************[delay_licht]**************************************
void delay_licht(uint16_t mseconds) {

    uint8_t state;

    mseconds/= 10;
    do {
        // button1
        state = buttons_getBtn1();
        if (state != buttons.state1) {
            buttons.state1 = state;
            // state did change
            if (!state) {
                // released button
                buttons.countdown1 = 50;
            } else if (!buttons.countdown1) {
                // pushed button and no countdown
                buttons.flank1 = 1;
            }
        }
        if (buttons.countdown1) {
            buttons.countdown1--;
        }

        // button2
        state = buttons_getBtn2();
        if (state != buttons.state2) {
            buttons.state2 = state;
            // state did change
            if (!state) {
                // released button
                buttons.countdown2 = 50;
            } else if (!buttons.countdown2) {
                // pushed button and no countdown
                buttons.flank2 = 1;
            }
        }
        if (buttons.countdown2) {
            buttons.countdown2--;
        }

        // buttonMode
        state = buttons_getBtnMode();
        if (state != buttons.stateMode) {
            buttons.stateMode = state;
            // state did change
            if (!state) {
                // released button
                buttons.countdownMode = 20;
            } else if (!buttons.countdownMode) {
                // pushed button and no countdown
                buttons.flankMode = 1;
            }
        }
        if (buttons.countdownMode) {
            buttons.countdownMode--;
        }

        // wait 10ms
        delay_ms(10);
    } while (mseconds--);
}

//**************************[delay_licht]**************************************
ISR(TIMER2_COMPA_vect) {

    if (ir_leds.counter) {
        ir_leds.counter = 0;
        leds_setIr1(ir_leds.state1);
        leds_setIr2(ir_leds.state2);
    } else {
        ir_leds.counter = 1;
        leds_setIr1(0);
        leds_setIr2(0);
    }
}
