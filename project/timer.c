#include "header.h"
#include "timer.h"
#include "lcd.h"

extern bool isTimerOn;
extern bool isRunning;
/*
 * Uart를 통한 사용자 입력
 * 원래는 문자열 -> 숫자 변환 
 */
int user_input;

/*
 * 사용자 입력값 처리를 위한 변수 
 */
int temp_input; // == user_input


/*
 * CTC count값 
 */
int ctc_cnt;
int ls_cnt;

/* FND 처리 */
/*
 * Segment 1개의 숫자 표현을 위한 데이터셋  
 */
unsigned char num_data[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90}; // 7-segment의 숫자 0~9까지 표현
/*
 * FND 자리 변경
 */
unsigned int Port_fnd[] = {0x1f, 0x2f, 0x4f, 0x8f}; // PORTE.4 5 6 7 전환 

void FND_disp()
{
    /*
     * PORTE 0123 다른 용도 사용하기 위해 &사용 
     */
     PORTE = Port_fnd[3]; PORTB = num_data[user_input/1000%10]; _delay_us(1000); PORTB=0xff;
     PORTE = Port_fnd[2]; PORTB = num_data[user_input/100%10];  _delay_us(1000); PORTB=0xff;
     PORTE = Port_fnd[1]; PORTB = num_data[user_input/10%10];   _delay_us(1000); PORTB=0xff;
     PORTE = Port_fnd[0]; PORTB = num_data[user_input/1%10];    _delay_us(1000); PORTB=0xff;    
}

//interrupt [TIM2_COMP] void timer2_out_comp(void)
ISR(TIMER2_COMP_vect)
{       
    ctc_cnt++; 
    ls_cnt++;
    if(ls_cnt == 4001){
        ls_cnt = 0;                
    }
    
    if(ctc_cnt == 20000) // 1초 
    {     
        ctc_cnt = 0;
        user_input--;
        if(user_input == 0)
        {
            isTimerOn = false;
            isRunning = false;
            PORTE = 0x00;
            dispSome("Timer is Done","",5000);  
        }
        if(user_input%100 == 99){
            user_input -= 40;
        }        
    }             
}

void Init_Timer2(void)
{
    TCCR2 = 0x00;           // 초기화 
    TCCR2 |= (1 << WGM21) | (1 << COM20);   // CTC 모드
    OCR2 = 100;             // 카운터 비교 값 50us
    TCCR2 |= 1 << CS21;     // 8분주
    TIMSK = 1<<OCIE2;       // 출력비교 인터럽트 허가
}

void var_Init(){
    ctc_cnt = 0;
    ls_cnt = 0;
}

void Init_TimerINT(void)
{
    DDRB = 0xff;
    DDRE = 0xf0;
    Init_Timer2();
    var_Init();
    sei();
}