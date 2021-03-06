/*	Author: brandon babu
 *  Partner(s) Name: 
 *	Lab Section:027
 *	Assignment: Lab #9  Exercise #1
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

void set_PWM(double frequency) {
	static double current_frequency; // keeps track of currently set frequency
	// will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } // stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter

		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short) (8000000 / (128 * frequency)) -1; }

		TCNT3 = 0; // resets counter
		current_frequency = frequency; // updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States{Start, init, c, d, e} state;
unsigned char a;
void Tick() {
	switch(state) { //state transitions
		case Start:
			state = init;
			break;
		case init:
			if (a == 0x01) {
				state = c;
			}
			else if (a == 0x02) {
				state = d;
			}
			else if (a == 0x04) {
				state = e;
			}
			else {
				state = init;
			}
			break;
		case c:
			if (a == 0x01) {
				state = c;
			}
			else {
				state = init;
			}
			break;
		case d:
			if (a == 0x02) {
				state = d;
			}
			else {
				state = init;
			}
			break;
		case e:
			if (a == 0x04) {
				state = e;
			}
			else {
				state = init;
			}
			break;
		default:
			state = init;
			break;
	}
	switch (state) { // state actions
		case Start:
			set_PWM(0);
			break;
		case init:
			set_PWM(0);
			break;
		case c:
			set_PWM(261.63);
			break;
		case d:
			set_PWM(293.66);
			break;
		case e:
			set_PWM(329.63);
			break;
		default:
			break;
	}
}

int main(void) {
    DDRB = 0xFF; PORTB = 0x00;
    DDRA = 0x00; PORTA = 0xFF;

    a = 0;
    state = Start;
    PWM_on();
    while (1) {
	    a = ~PINA & 0x07;
	    Tick();
    }
    return 1;
}
