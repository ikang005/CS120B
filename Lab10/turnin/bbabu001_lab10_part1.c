/*	Author: brandon babu
 *  Partner(s) Name: 
 *	Lab Section:027
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; //TimerISR() sets this to 1. C programmer should clear to 0

//internal variables for mapping AVR's ISR to our cleaner TimerISR() model.
unsigned long _avr_timer_M = 1; //start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; //Current internal count of 1ms ticks

void TimerOn(){
        //AVR timer/counter controller register TCCR1
        TCCR1B = 0x0B;

        //AVR output compare register OCR1A
        OCR1A = 125;

        //AVR timer interrupt mask register
        TIMSK1 = 0x02; //bit1: OCIE1A -- enables compare match interrupt

        //Initialize avr counter
        TCNT1 = 0;

        _avr_timer_cntcurr = _avr_timer_M;
        //      TimerISR will be called every _avr_timer_cntcurr milliseconds

        //Enable global interrupts
        SREG |= 0x80; //0x80: 1000000

}

void TimerOff(){
        TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR(){
        TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){

        _avr_timer_cntcurr--;
        if(_avr_timer_cntcurr == 0){
                TimerISR();
                _avr_timer_cntcurr = _avr_timer_M;
        }

}

//Set TimerISR() to tick every M ms
void TimerSet(unsigned long M){

        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;

}

unsigned char threeLEDs;
unsigned char blinkingLED;

enum ThreeStates{Start, zero, one, two}ThreeState;
void ThreeLEDTick() {
	switch(ThreeState) { // state transitions
		case Start:
			ThreeState = zero;
			break;
		case zero:
			ThreeState = one;
			break;
		case one:
			ThreeState = two;
			break;
		case two:
			ThreeState = zero;
			break;
		default:
			ThreeState = zero;
			break;
	}
	switch(ThreeState) { // state actions
		case Start:
			break;
		case zero:
			threeLEDs = 0x01;
			break;
		case one:
			threeLEDs = 0x02;
			break;
		case two:
			threeLEDs = 0x04;
			break;
		default:
			break;
	}
}

enum BlinkStates{on, off}BlinkState;
void BlinkLEDTick() {
	switch(BlinkState) {
		case on:
			BlinkState = off;
			break;
		case off:
			BlinkState = on;
			break;
		default:
			BlinkState = on;
			break;
	}
	switch(BlinkState) {
		case on:
			blinkingLED = 0x08;
			break;
		case off:
			blinkingLED = 0x00;
			break;
		default:
			break;
	}
}

enum CombineStates{out}CombineState;
void CombineLEDTick() {
	switch(CombineState) {
		case out:
			CombineState = out;
			break;
		default:
			CombineState = out;
			break;
	}
	switch(CombineState) {
		case out:
			PORTB = threeLEDs | blinkingLED;
			break;
		default:
			break;
	}
}

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	threeLEDs = 0x00;
	blinkingLED = 0x00;
	ThreeState = zero;
	BlinkState = on;
	CombineState = out;

    while (1) {
	    ThreeLEDTick();
	    BlinkLEDTick();
	    CombineLEDTick();

	    while (!TimerFlag);
	    TimerFlag = 0;
    }
    return 1;
}
