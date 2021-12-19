#include "header.h"
#include "timer.h"
#include "lcd.h"

/*
 * Uart를 통한 사용자 입력
 * 원래는 문자열 -> 숫자 변환 
 */
int userTimeInput;

/*
 * CTC count값 
 */
int timerCnt; // CTC 방식
int lsCnt;   // Light Sensor

/* FND 처리 */
/*
 * Segment 1개의 숫자 표현을 위한 데이터셋  
 */
unsigned char num_data[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90}; // 7-segment의 숫자 0~9까지 표현
/*p
 * FND 자리 변경
 */
unsigned int Port_fnd[] = {0x1f, 0x2f, 0x4f, 0x8f}; // PORTE.4 5 6 7 전환 

void fnd_disp()
{
    /*
     * PORTE 0123 다른 용도 사용하기 위해 &사용  B -> C test
     */
     PORTE = Port_fnd[3]; PORTC = num_data[userTimeInput/1000%10]; _delay_us(1000); PORTC=0xff;
     PORTE = Port_fnd[2]; PORTC = num_data[userTimeInput/100%10];  _delay_us(1000); PORTC=0xff;
     PORTE = Port_fnd[1]; PORTC = num_data[userTimeInput/10%10];   _delay_us(1000); PORTC=0xff;
     PORTE = Port_fnd[0]; PORTC = num_data[userTimeInput/1%10];    _delay_us(1000); PORTC=0xff;    
}

ISR(TIMER2_COMP_vect)
{       
    timerCnt++; 
    lsCnt++;
	
    if(lsCnt == 4001){ 
        lsCnt = 0;                
    }
    
    if(timerCnt == 20000) // 1초 
    {     
        timerCnt = 0;
        userTimeInput--;
        if(userTimeInput == 0)
        {
            isTimerOn = false;
            isRunning = false;
			isTimerDone = true;
			// To Change LED ON
            PORTD |= 0x80;
			PORTE = 0x00;
			lcdUserInputError = 5;
			disp_some("Timer is Done","",3000);
        }
        if(userTimeInput%100 == 99){
            userTimeInput -= 40;
        }        
    }             
}

void init_timer_Timer2(void)
{
    TCCR2 = 0x00;           // 초기화 
    TCCR2 |= (1 << WGM21) | (1 << COM20);   // CTC 모드
    OCR2 = 100;             // 카운터 비교 값 50us
    TCCR2 |= 1 << CS21;     // 8분주
    TIMSK = 1<<OCIE2;       // 출력비교 인터럽트 허가
}

void init_motor_Timer1(void)
{
	// 고속 PWM , TOP = ICR1;, 64분주
	TCCR1A |=  (1<<COM1A1) | (0<<COM1A0) | (1<<WGM11);
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11) | (1<<CS10);
	ICR1=4999;     //TOP
	OCR1A=375;     //0도
}

void emergency_motor_degree(){
	OCR1A=500;     //90도
}

void end_emergency(){
	OCR1A=250;     //-90도
}

void var_Init(){
	timerCnt = 0;
	lsCnt = 0;
}

void init_Timer(void)
{
	var_Init();
	init_timer_Timer2();
	init_motor_Timer1();	
	//sei();
}