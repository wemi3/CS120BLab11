/*	Author: Emily Wu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #11  Exercise #5
 *	Exercise Description: [optional - include for your own benefit]
 *	Jumping game using daisy chained shift registers.
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/8fb_zpkp7zA
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include <../../timer.h>
#include <../../scheduler.h>

unsigned short player = 0x0001;
unsigned short enemy = 0x8000;
unsigned short value = 0x0000;

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

enum PlayerStates { playerStart, move, wait};

int PlayerTick(int state) {
	switch (state) {
		case playerStart:
			player = 0x0001;
			state = wait;
			break;
		case move:
			if (PINA == 0xFE && player < 0x8000) {
				player = player << 1;
				state = wait;
			} else if (PINA == 0xFB && player > 0x0001) {
				player = player >> 1;
				state = wait;
			} else if (PINA == 0xFC) {
				if (player <= 0x1000) {
					player = player << 3;
				} else if (player > 0x1000) {
					player = 0x8000;
				}
				state = wait;
			} else if (PINA == 0xF9) {
				if (player >= 0x0008) {
					player = player >> 3;
				} else if (player < 0x0008) {
					player = 0x0001;
				}
				state = wait;
			} else {
				state = move;
			}
			break;
		case wait:
			state = (PINA == 0xFF) ? move : wait;
			break;
		default:
			state = playerStart;
			break;
	}
	return state;
}

enum EnemyStates { enemyStart, goLeft, goRight };

int EnemyTick(int state) {
	switch(state) {
		case enemyStart:
			enemy = 0x8000;
			state = goLeft;
			break;
		case goLeft:
			if (enemy < 0x8000) {
				enemy = enemy << 1;
				state = goLeft;
			} else {
				state = goRight;
			}
			break;
		case goRight:
			if (enemy > 0x0001) {
				enemy = enemy >> 1;
				state = goRight;
			} else {
				state = goLeft;
			}
			break;
		default:
			state = enemyStart;
			break;
	}
	return state;
}

enum DisplayStates { displayStart, display, collision };

int i = 0;

int DisplayTick(int state) {
	switch(state) {
		case displayStart:
			state = display;
			break;
		case display:
			if (player != enemy) {
				value = player | enemy;
				state = display;
			} else if (player == enemy) {
				value = 0x0000;
				state = collision;
			}
			break;
		case collision:
			if (i < 5) {
				value = ~value;
				i++;
				state = collision;
			} else {
				value = 0x0000;
				player = 0x0001;
				enemy = 0x8000;
				i = 0;
				state = display;
			}
			break;
		default:
			state = displayStart;
			break;
	}
	return state;
}



int main(void) {
    	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;

	static task task1, task2, task3;
	task *tasks[] = { &task1, &task2, &task3 };
	const unsigned numTasks = sizeof(tasks)/sizeof(task*);

	//const char start = -1;

	task1.state = playerStart;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &PlayerTick;

	task2.state = enemyStart;
	task2.period = 500;
	task2.elapsedTime = task2.period;
	task2.TickFct = &EnemyTick;

	task3.state = displayStart;
	task3.period = 100;
	task3.elapsedTime = task3.period;
	task3.TickFct = &DisplayTick;

	TimerSet(100);
	TimerOn();

    	/* Insert your solution below */
    	while (1) {
		for (unsigned k = 0; k < numTasks; k++) {
			if (tasks[k]->elapsedTime == tasks[k]->period) {
				tasks[k]->state = tasks[k]->TickFct(tasks[k]->state);
				tasks[k]->elapsedTime = 0;
			}
			tasks[k]->elapsedTime += 100;
		}
		transmit_data(value);
		while (!TimerFlag) {};
		TimerFlag = 0;
    	}
    	return 1;
}
