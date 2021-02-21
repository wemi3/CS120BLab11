/*	Author: Emily Wu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *	Festive lights display w/ two shift registers.
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/kbiRvTRLJKU 
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include <../../timer.h>
#include <../../scheduler.h>

unsigned go = 0;
unsigned go2 = 0;
unsigned char value = 0x00;
unsigned char value2 = 0x00;

void transmit_data(unsigned char data, int regNumber) {
	int i;
	for (i = 0; i < 8; ++i) {
		if (regNumber == 1) {
			PORTC = 0x08;
		} else if (regNumber == 2) {
			PORTC = 0x20;
		}
		PORTC |= ((data >> i) & 0x01);
		PORTC |= 0x02;
	}
	if (regNumber == 1) {
		PORTC |= 0x04;
	} else if (regNumber == 2) {
		PORTC |= 0x10;
	}
	PORTC = 0x00;
}

enum lights1States { lights1Start, lights1Display };
unsigned char lights1 = 0x00;

int Lights1Tick(int state) {
/*	if (go == 0) {
		lights1 = value;
	}
	if (go2 == 0) {
		lights1 = value2;
	}*/
	switch (state) {
		case lights1Start:
			lights1 = 0x00;
			state = lights1Display;
			break;
		case lights1Display:
				if (lights1 == 0x00) {
					lights1 = 0xAA;
				} else if (lights1 == 0xAA) {
					lights1 = 0x55;
				} else if (lights1 == 0x55) {
					lights1 = 0xAA;
				} 
			state = lights1Display;
			break;
		default:
			state = lights1Start;
			break;
	}
	if (go == 0) {
		value = lights1;
	} 
	if (go2 == 0) {
		value2 = lights1;
	}
	return state;
}

enum lights2States { lights2Start, lights2Display, lights2Display2 };

unsigned char lights2 = 0x00;

int Lights2Tick(int state) {
	/*if (go == 1) {
		lights2 = value;
	}
	if (go2 == 1){
		lights2 = value2;
	}*/
	switch (state) {
		case lights2Start:
			lights2 = 0x00;
			state = lights2Display;
			break;
		case lights2Display:
			if (lights2 < 0xFF) {
				lights2 = (lights2 << 1) + 1;
				state = lights2Display;
			} else {
				state = lights2Display2;
			} 
			break;
		case lights2Display2:
			if (lights2 > 0x00) {
				lights2 = lights2 << 1;
				state = lights2Display2;
			} else {
				state = lights2Display;
			}
			break;
		default:
			state = lights2Start;
			break;
	}
	if (go == 1) {
		value = lights2;
	}
	if (go2 == 1) {
		value2 = lights2;
	}
	return state;
}

enum lights3States { lights3Start, lights3Display, lights3Display2 };

unsigned char lights3 = 0x00;

unsigned char lowerNibble = 0x0F;
unsigned char upperNibble = 0xF0;
unsigned char lowerNibble2 = 0x0F;
unsigned char upperNibble2 = 0xF0;

int Lights3Tick(int state) {
	/*if (go == 2) {
		lights3 = value;
	}
	if (go2 == 2) {
		lights3 = value2;
	}*/
	switch(state) {
		case lights3Start:
			lights3 = 0x00;
			state = lights3Display;
			break;
		case lights3Display:
			if (lights3 > 0) {
				lowerNibble = lowerNibble << 1;
				upperNibble = upperNibble >> 1;
				lights3 = (lowerNibble & 0x0F) + (upperNibble & 0xF0);
				state = lights3Display;
			} else {
				state = lights3Display2;
			}
			break;
		case lights3Display2:
			if (lights3 < 0xFF) {
				lowerNibble = (lowerNibble << 1) + 1;
				upperNibble = (upperNibble >> 1) + 0x80;
				lights3 = (lowerNibble & 0x0F) + (upperNibble & 0xF0);
				state = lights3Display2;
			} else {
				state = lights3Display;
			}	
			break;
		default:
			state = lights3Start;
			break;
	}
	if (go == 2) {
		value = lights3;
	}
	if (go2 == 2) {
		value2 = lights3;
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
				//value = 0x00;
				buttonState = inc;
			} else if (PINA == 0xFD && go > 0) {
				go--;
				//value = 0x00;
				buttonState = dec;
			} else if (PINA == 0xFC) {
				if (go == 3) {
					go = 0;
				} else {
					go = 3;
				}
				buttonState = clr;
			} else if (PINA == 0xEF && go2 < 2) {
				go2++;
				//value2 = 0x00;
				buttonState = inc;
			} else if (PINA == 0xDF && go2 > 0) {
				go2--;
				//value2 = 0x00;
				buttonState = dec;
			} else if (PINA == 0xCF) {
				if (go2 == 3) {
					go2 = 0;
				} else {
					go2 = 3;
				}
				buttonState = clr;
			} else {
				buttonState = init;
			}
			break;
		case inc:
			buttonState = (PINA == 0xFF) ? init : inc;
			break;
		case dec:
			buttonState = (PINA == 0xFF) ? init : dec;
			break;
		case clr:
			buttonState = (PINA == 0xFF) ? init : clr;
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
			if (go == 3) {
				value = 0x00;
			}
			if (go2 == 3) {
				value2 = 0x00;
			}
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
	
	TimerSet(100);
	TimerOn();

    	/* Insert your solution below */
    	while (1) {
		ButtonTick();
		if (tasks[go]->elapsedTime == tasks[go]->period) {
			tasks[go]->state = tasks[go]->TickFct(tasks[go]->state);
			tasks[go]->elapsedTime = 0;
		}
		transmit_data(value, 1);
		tasks[go]->elapsedTime += 100;
		if (tasks[go2]->elapsedTime == tasks[go2]->period) {
			tasks[go2]->state = tasks[go2]->TickFct(tasks[go2]->state);
			tasks[go2]->elapsedTime = 0;
		}
		transmit_data(value2, 2);
		if (go != go2) {
			tasks[go2]->elapsedTime += 100;
		}
		while (!TimerFlag) {};
		TimerFlag = 0;
    	}
    	return 1;
}
