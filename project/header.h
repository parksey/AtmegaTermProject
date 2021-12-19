#ifndef F_CPU
#define F_CPU 14745600
#endif

#ifndef __HEADER_H__
#define __HEADER_H__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


typedef enum{false, true} bool;


/*
 * 전역 공용 변수
 */
extern bool isAllSensorLock;
extern bool isTimerOn;
extern bool isRunning;
extern bool isTimerDone;
extern bool isEmergency;
extern int lcdUserInputError;

extern unsigned char Hello[];
extern unsigned char madeBy[];

#endif

