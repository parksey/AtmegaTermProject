#include "header.h"
#include "timer.h"
#include "lcd.h"

/*
 * Uart�� ���� ����� �Է�
 * ������ ���ڿ� -> ���� ��ȯ 
 */
int userTimeInput;

/*
 * CTC count�� 
 */
int timerCnt; // CTC ���
int lsCnt;   // Light Sensor

/* FND ó�� */
/*
 * Segment 1���� ���� ǥ���� ���� �����ͼ�  
 */
unsigned char num_data[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90}; // 7-segment�� ���� 0~9���� ǥ��
/*p
 * FND �ڸ� ����
 */
unsigned int Port_fnd[] = {0x1f, 0x2f, 0x4f, 0x8f}; // PORTE.4 5 6 7 ��ȯ 

void fnd_disp()
{
    /*
     * PORTE 0123 �ٸ� �뵵 ����ϱ� ���� &���  B -> C test
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
    
    if(timerCnt == 20000) // 1�� 
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
    TCCR2 = 0x00;           // �ʱ�ȭ 
    TCCR2 |= (1 << WGM21) | (1 << COM20);   // CTC ���
    OCR2 = 100;             // ī���� �� �� 50us
    TCCR2 |= 1 << CS21;     // 8����
    TIMSK = 1<<OCIE2;       // ��º� ���ͷ�Ʈ �㰡
}

void init_motor_Timer1(void)
{
	// ��� PWM , TOP = ICR1;, 64����
	TCCR1A |=  (1<<COM1A1) | (0<<COM1A0) | (1<<WGM11);
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11) | (1<<CS10);
	ICR1=4999;     //TOP
	OCR1A=375;     //0��
}

void emergency_motor_degree(){
	OCR1A=500;     //90��
}

void end_emergency(){
	OCR1A=250;     //-90��
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