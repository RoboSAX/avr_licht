
/******************************************************************************
* robosax/lichter_demo.c                                                      *
* ======================                                                      *
*                                                                             *
* Version: ?.?.?                                                              *
* Date   : 08.02.18                                                           *
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
#define led1_readGreen()   ((PORTB & _BV(2)) != 0x00)
#define led1_readBlue()   ((PORTD & _BV(5)) != 0x00)
#define led1_readIr()   ((PORTD & _BV(3)) != 0x00)

#define led2_setRed(x)   ( x ? (PORTC|= _BV(0)) : (PORTC&= ~_BV(0)))
#define led2_setGreen(x) ( x ? (PORTC|= _BV(1)) : (PORTC&= ~_BV(1)))
#define led2_setBlue(x)  ( x ? (PORTC|= _BV(2)) : (PORTC&= ~_BV(2)))
#define led2_setIr(x)    ( x ? (PORTB|= _BV(3)) : (PORTB&= ~_BV(3)))

#define led2_readRed()   ((PORTC & _BV(0)) != 0x00)
#define led2_readGreen()   ((PORTC & _BV(1)) != 0x00)
#define led2_readBlue()   ((PORTC & _BV(2)) != 0x00)
#define led2_readIr()   ((PORTB & _BV(3)) != 0x00)

#define button_getBtnmode() ((PIND & _BV(4)) == 0x00)
#define button_getBtn1() ((PINB & _BV(0)) == 0x00)
#define button_getBtn2() ((PINC & _BV(3)) == 0x00)

//**************************<Definitions>**************************************
typedef struct{
    _Bool big;
    _Bool one;
    _Bool two;
    _Bool big_flank_up;
    _Bool one_flank_up;
    _Bool two_flank_up;
    _Bool big_flank_down;
    _Bool one_flank_down;
    _Bool two_flank_down;
}ButtonState; ButtonState buttons;

//**************************<Prototypes>***************************************
void init_hardware(void);
void led_clearAll(void);
void get_buttonState(void);
int main (void);
void modus_1(uint8_t *error_team1, uint8_t *error_team2);
void modus_2(void);
void modus_3(uint8_t *error_team1, uint8_t *error_team2);
void blink_multicolored_alternating(void);
void blink_green_alternating(void);
void blink_multicolored(void);

//**************************<Methods>******************************************

//**************************[init_hardware]************************************
void init_hardware(void) {

    // set leds to output
    DDRB = _BV(1) | _BV(2) | _BV(3);
    DDRC = _BV(0) | _BV(1) | _BV(2);
    DDRD = _BV(3) | _BV(5);

    // set buttons to input, set to pull-up
    DDRC &= ~_BV(3);
  //  PORTC |= _BV(3);
  //  PORTB |= _BV(0);

    // set big button to input, set to pull-up
    DDRD &= ~_BV(4);
  //  PORTD |= _BV(4);

    buttons.big = 0;
    buttons.one = 0;
    buttons.two = 0;
}

//**************************[led_clearAll]********************************************
void led_clearAll(void) {

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

//**************************[get_buttonState]**********************************
void get_buttonState(void) {
    _Bool big_old = buttons.big; _Bool one_old = buttons.one; _Bool two_old = buttons.two;

    buttons.big = button_getBtnmode();
    buttons.one = button_getBtn1();
    buttons.two = button_getBtn2();

    if(buttons.big != big_old || buttons.one != one_old || buttons.two != two_old || buttons.big_flank_down || buttons.big_flank_up || buttons.one_flank_down || buttons.one_flank_up || buttons.two_flank_down || buttons.two_flank_up){
        delay_ms(30);
        if(button_getBtnmode() && buttons.big && !big_old)
            buttons.big_flank_up = 1;
        else buttons.big_flank_up = 0;

        if(!button_getBtnmode() && !buttons.big && big_old)
            buttons.big_flank_down = 1;
        else buttons.big_flank_down = 0;

        if(button_getBtn1() && buttons.one && !one_old)
            buttons.one_flank_up = 1;
        else buttons.one_flank_up = 0;

        if(!button_getBtn1() && !buttons.one && one_old)
            buttons.one_flank_down= 1;
        else buttons.one_flank_down = 0;

        if(button_getBtn2() && buttons.two && !two_old)
            buttons.two_flank_up = 1;
        else buttons.two_flank_up = 0;

        if(!button_getBtn2() && !buttons.two && two_old)
            buttons.two_flank_down= 1;
        else buttons.two_flank_down = 0;
    }

}

//**************************[main]*********************************************
int main (void) {

    // initialize hardware
    init_hardware();

    //ToDo: modus aus und in EEPROM schreiben
     uint8_t modus = 1;
     blink_multicolored_alternating();
     led_clearAll();
     uint8_t error_count_team1 = 0;
     uint8_t error_count_team2 = 0;
     uint8_t *error_team1 = &error_count_team1;
     uint8_t *error_team2 = &error_count_team2;

    //Idee: rote LED blinkt je nach aktivem Modus zu Beginn 1, 2 oder 3 mal

    //blaue LED leuchtet für Team 1
    //grüne LED leuchtet für Team 2

    while (1)
    {
        //liest aktuelle Zustände der Knöpfe
        get_buttonState();

        //großer Knopf für Moduswechsel
        if (buttons.big_flank_up)
            modus++;
        if (modus >3)
            modus=1;

        // wenn der Modus geswitched wird, starten alle Lampen ausgeschaltet
        // und beide Fehlerzähler werden zurückgesetzt
        if (buttons.big_flank_up) {
            led_clearAll();
            error_count_team1 = 0;
            error_count_team2 = 0;

            switch (modus) {
                case  1: blink_multicolored_alternating();
                            break;
                case  2: blink_green_alternating();
                            break;
                case  3: blink_multicolored();
                            break;
                default: led1_setGreen(1); led1_setBlue(1); led1_setRed(1);
                            break;
            }
            led_clearAll();

        }

            switch (modus) {
                case  1: modus_1(error_team1, error_team2);
                            break;
                case  2: modus_2();
                            break;
                case  3: modus_3(error_team1, error_team2);
                            break;
                default: led1_setGreen(1); led1_setBlue(1); led1_setRed(1);
                            break;
            }
            // vielleicht unnötiges Delay, kann rausgenommen werden
           // delay_ms(10);
    }

    return (0);

}

//**************************[modus_1]*********************************************
void modus_1(uint8_t *error_team1, uint8_t *error_team2) {

//***Team1*************************
    if (buttons.one_flank_up) {
        //wenn Knopf gedrückt wird, obwohl Teamlicht (blau) schon an ist -> Fehler
        if (led1_readBlue()) {
            *error_team1+=1;
            if (*error_team1>=3) {
            //bei 3 Fehlern: eigenes Licht geht aus, das vom Gegner (grün) an
                led1_setBlue(0);
                led2_setGreen(1);
                *error_team1=0;
            }
        }
        //Teamlicht anschalten, wenn vorher aus
        else {
            led1_setBlue(1);
            led2_setGreen(0);
        }
    }

//***Team2*************************
    if (buttons.two_flank_up) {
        //wenn Knopf gedrückt wird, obwohl Teamlicht (grün) schon an ist -> Fehler
        if (led2_readGreen()) {
            *error_team2+=1;
            if (*error_team2>=3) {
            //bei 3 Fehlern: eigenes Licht geht aus, das vom Gegner (blau) an
                led1_setBlue(1);
                led2_setGreen(0);
                *error_team2=0;
            }
        }
        //Teamlicht anschalten, wenn vorher aus
        else {
            led1_setBlue(0);
            led2_setGreen(1);
        }
    }

}

//**************************[modus_2]*********************************************
void modus_2() {
    //mit Zuständen arbeiten
    if (buttons.one_flank_up) {
        led1_setGreen(1);
        led2_setGreen(0);
    }
    if (buttons.two_flank_up) {
        led1_setGreen(0);
        led2_setGreen(1);
    }
}

//**************************[modus_3]*********************************************
//unabhängige Lichter mit Fehlerzähler
void modus_3(uint8_t *error_team1, uint8_t *error_team2) {

//***Team1*************************
    if (buttons.one_flank_up) {
        //wenn Knopf gedrückt wird, obwohl Teamlicht (blau) schon an ist -> Fehler
        if (led1_readBlue()) {
            *error_team1+=1;
            if (*error_team1>=3) {
            //bei 3 Fehlern: eigenes Licht geht aus, das vom Gegner (grün) an
                led1_setBlue(0);
                *error_team1=0;
            }
        }
        //Teamlicht anschalten, wenn vorher aus
        else {
            led1_setBlue(1);
        }
    }

//***Team2*************************
    if (buttons.two_flank_up) {
        //wenn Knopf gedrückt wird, obwohl Teamlicht (grün) schon an ist -> Fehler
        if (led2_readBlue()) {
            *error_team2+=1;
            if (*error_team2>=3) {
            //bei 3 Fehlern: eigenes Licht geht aus, das vom Gegner (blau) an
                led2_setBlue(0);
                *error_team2=0;
            }
        }
        //Teamlicht anschalten, wenn vorher aus
        else {
            led2_setBlue(1);
        }
    }
}

void blink_multicolored_alternating() {


    led1_setGreen(1);    led1_setRed(1);
    delay_ms(200);
    led_clearAll();
    led2_setGreen(1);    led2_setRed(1);
    delay_ms(200);
    led_clearAll();
    led1_setGreen(1);    led1_setRed(1);
    delay_ms(200);
    led_clearAll();
    led2_setGreen(1);    led2_setRed(1);
    delay_ms(200);
    led_clearAll();
    led1_setGreen(1);    led1_setRed(1);
    delay_ms(200);
    led_clearAll();
    led2_setGreen(1);    led2_setRed(1);
    delay_ms(200);
    led_clearAll();


}

void blink_green_alternating(){
    led1_setGreen(1);
    delay_ms(200);
    led_clearAll();
    led2_setGreen(1);
    delay_ms(200);
    led_clearAll();

    led1_setGreen(1);
    delay_ms(200);
    led_clearAll();
    led2_setGreen(1);
    delay_ms(200);
    led_clearAll();

    led1_setGreen(1);
    delay_ms(200);
    led_clearAll();
    led2_setGreen(1);
    delay_ms(200);

}

void blink_multicolored() {
    led1_setBlue(1);
    led2_setBlue(1);
    delay_ms(200);

    led_clearAll();
    delay_ms(200);

    led1_setBlue(1);
    led2_setBlue(1);
    delay_ms(200);

    led_clearAll();
    delay_ms(200);

    led1_setBlue(1);
    led2_setBlue(1);
    delay_ms(200);

}
