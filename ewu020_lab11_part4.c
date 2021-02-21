/*	Author: Emily Wu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *	Daisy chained shift registers.
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/9T8oGKu20EM
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include <../../timer.h>

unsigned short value = 0x02FF;

void transmit_data(unsigned short data) {
	int i;
	for (i = 0; i < 16; ++i) {
		PORTC = 0x08;
		PORTC |= ((data >> i) & 0x01);
		PORTC |= 0x02;
	}
	PORTC |= 0x04;
	PORTC = 0x00;
}

enum States { start, init, inc, dec, clr } state;

void Tick() {
	switch (state) {
		case start:
			state = init;
			break;
		case init:
			if (PINA == 0xFE && value < 0xFFFF) {
				value++;
				state = inc;
			} else if (PINA == 0xFD && value > 0x0000) {
				value--;
				state = dec;
			} else if (PINA == 0xFC) {
				value = 0x0000;
				state = clr;
			} else {
				state = init;
			}
			break;
		case inc:
			state = init;
			break;
		case dec:
			state = init;
			break;
		case clr: 
			state = (PINA == 0xFC) ? clr : init;
		default:
			state = start;
			break;
	}
}

int main(void) {
    	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;

	TimerSet(100);
	TimerOn();

    	/* Insert your solution below */
    	while (1) {
		Tick();
		transmit_data(value);
		while (!TimerFlag) {};
		TimerFlag = 0;
    	}
    	return 1;
}
