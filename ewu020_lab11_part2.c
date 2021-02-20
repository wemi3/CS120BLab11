/*	Author: Emily Wu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *	Festive lights display w/ one shift register.
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/m56DXICSF5s
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include <../../timer.h>
#include <../../scheduler.h>

unsigned go = 0;
unsigned char value = 0x00;

void transmit_data(unsigned char data) {
	int i;
	for (i = 0; i < 8; ++i) {
		PORTC = 0x08;
		PORTC |= ((data >> i) & 0x01);
		PORTC |= 0x02;
	}
	PORTC |= 0x04;
	PORTC = 0x00;
}

enum lights1States { lights1Start, lights1Display };

int Lights1Tick(int state) {
	switch (state) {
		case lights1Start:
			value = 0x00;
			state = lights1Display;
			break;
		case lights1Display:
			if (value == 0x00) {
				value = 0xAA;
			} else if (value == 0xAA) {
				value = 0x55;
			} else if (value == 0x55) {
				value = 0xAA;
			}
			state = lights1Display;
			break;
		default:
			state = lights1Start;
			break;
	}
	return state;
}

enum lights2States { lights2Start, lights2Display, lights2Display2 };

int Lights2Tick(int state) {
	switch (state) {
		case lights2Start:
			value = 0x00;
			state = lights2Display;
			break;
		case lights2Display:
			if (value < 0xFF) {
				value = (value << 1) + 1;
				state = lights2Display;
			} else {
				state = lights2Display2;
			} 
			break;
		case lights2Display2:
			if (value > 0x00) {
				value = value << 1;
				state = lights2Display2;
			} else {
				state = lights2Display;
			}
			break;
		default:
			state = lights2Start;
			break;
	}
	return state;
}

enum lights3States { lights3Start, lights3Display, lights3Display2 };

unsigned char lowerNibble = 0x0F;
unsigned char upperNibble = 0xF0;

int Lights3Tick(int state) {

	switch(state) {
		case lights3Start:
			value = 0x00;
			state = lights3Display;
			break;
		case lights3Display:
			if (value > 0) {
				lowerNibble = lowerNibble << 1;
				upperNibble = upperNibble >> 1;
				value = (lowerNibble & 0x0F) + (upperNibble & 0xF0);
				state = lights3Display;
			} else {
				state = lights3Display2;
			}
			break;
		case lights3Display2:
			if (value < 0xFF) {
				lowerNibble = (lowerNibble << 1) + 1;
				upperNibble = (upperNibble >> 1) + 0x80;
				value = (lowerNibble & 0x0F) + (upperNibble & 0xF0);
				state = lights3Display2;
			} else {
				state = lights3Display;
			}
			break;
		default:
			state = lights3Start;
			break;
	}
	return state;
}



enum buttonStates { start, init, inc, dec, clr } buttonState;

void ButtonTick() {
	switch (buttonState) {
		case start:
			buttonState = init;
			break;
		case init:
			if (PINA == 0xFE && go < 2) {
				go++;
				value = 0x00;
				buttonState = inc;
			} else if (PINA == 0xFD && go > 0) {
				go--;
				value = 0x00;
				buttonState = dec;
			} else if (PINA == 0xFC) {
				if (go == 3) {
					go = 0;
				} else {
					go = 3;
				}
				buttonState = clr;
			} else {
				buttonState = init;
			}
			break;
		case inc:
			buttonState = (PINA == 0xFE) ? inc : init;
			break;
		case dec:
			buttonState = (PINA == 0xFD) ? dec : init;
			break;
		case clr:
			buttonState = (PINA == 0xFC) ? clr : init;
			break;
		default:
			buttonState = start;
			break;
	}
}

enum offStates { off };

int OffTick(int state) {
	switch (state) {
		case off:
			value = 0x00;
			state = off;
			break;
		default:
			state = off;
			break;
	}
	return state;
}

int main(void) {
    	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;

	static task task1, task2, task3, task4;
	task *tasks[] = { &task1, &task2, &task3, &task4 };

	//const char start = -1;

	task1.state = lights1Start;
	task1.period = 200;
	task1.elapsedTime = task1.period;
	task1.TickFct = &Lights1Tick;

	task2.state = lights2Start;
	task2.period = 200;
	task2.elapsedTime = task2.period;
	task2.TickFct = &Lights2Tick;

	task3.state = lights3Start;
	task3.period = 200;
	task3.elapsedTime = task3.period;
	task3.TickFct = &Lights3Tick;

	task4.state = off;
	task4.period = 500;
	task4.elapsedTime = task4.period;
	task4.TickFct = &OffTick;
	
	TimerSet(50);
	TimerOn();

    	/* Insert your solution below */
    	while (1) {
		ButtonTick();
		if (tasks[go]->elapsedTime == tasks[go]->period) {
			tasks[go]->state = tasks[go]->TickFct(tasks[go]->state);
			tasks[go]->elapsedTime = 0;
		}
		tasks[go]->elapsedTime += 50;
		transmit_data(value);
		while (!TimerFlag) {};
		TimerFlag = 0;
    	}
    	return 1;
}
