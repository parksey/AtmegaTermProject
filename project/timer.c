#include "header.h"
#include "timer.h"
#include "lcd.h"

extern bool isTimerOn;
extern bool isRunning;
/*
 * Uart�� ���� ����� �Է�
 * ������ ���ڿ� -> ���� ��ȯ 
 */
int user_input;

/*
 * ����� �Է°� ó���� ���� ���� 
 */
int temp_input; // == user_input


/*
 * CTC count�� 
 */
int ctc_cnt;
int ls_cnt;

/* FND ó�� */
/*
 * Segment 1���� ���� ǥ���� ���� �����ͼ�  
 */
unsigned char num_data[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90}; // 7-segment�� ���� 0~9���� ǥ��
/*
 * FND �ڸ� ����
 */
unsigned int Port_fnd[] = {0x1f, 0x2f, 0x4f, 0x8f}; // PORTE.4 5 6 7 ��ȯ 

void FND_disp()
{
    /*
     * PORTE 0123 �ٸ� �뵵 ����ϱ� ���� &��� 
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
    
    if(ctc_cnt == 20000) // 1�� 
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
    TCCR2 = 0x00;           // �ʱ�ȭ 
    TCCR2 |= (1 << WGM21) | (1 << COM20);   // CTC ���
    OCR2 = 100;             // ī���� �� �� 50us
    TCCR2 |= 1 << CS21;     // 8����
    TIMSK = 1<<OCIE2;       // ��º� ���ͷ�Ʈ �㰡
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