#ifndef __TIMER_H__
#define __TIMER_H__

void init_Timer();
void init_timer_Timer2();
void init_motor_Timer1();
void emergency_motor_degree();
void end_emergency();
void var_Init();
void fnd_disp();

/*
 * 사용 파일 : uart.c
 */
extern int userTimeInput;
extern int timerCnt;
extern int lsCnt;

#endif
