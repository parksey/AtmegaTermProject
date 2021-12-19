#include "header.h"
#include "uart.h"
#include "lcd.h"
#include "timer.h"
#include <string.h>

/*
 * USART1 = ���� 
 * USART0 = �������
 */
/*
 * �Է� �޴� �ܾ� 1�� 
 */ 
char chComInput;
char chBTInput;
/*
 * ��Ŷ�ϼ��� ���� ���ڿ�  
 */
char strComInput[20] = {0,};
char strBTInput[20] = {0,};
/*
 * �Է¹��� ���� 
 */
int inputComCount = 0;
int inputBTCount = 0;

void Init_USART1(void){     
    UCSR1A = 0x00; // 1��� ���۸��,  ���� ����
    UCSR1B = (1<<RXEN1) | (1 <<TXEN1) | (1<<RXCIE1);
    UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);  //���� ���� �������� �ʱⰪ
    UCSR1C &= ~(1<<UMSEL1); // �񵿱� ����� ����, ���� ���� 
    UBRR1H = 0;
    UBRR1L = 95; //9600bps     
    sei();
}
void Init_USART0(void){
	UCSR0A=0x00; // 1��� ���۸��,  ���� ����
	UCSR0B=0x98; // RX ���ͷ�Ʈ ���� �㰡 | �����㰡 | �۽� �㰡
	UCSR0C=0x06; //���� ���� �������� �ʱⰪ
	UBRR0H=0;
	UBRR0L=95; 
}
/*
 * ��Ŷó�� 
 */
void sensor_off(){  // ����
    PORTD = 0x00;   // LED
    PORTE = 0x00;   // Timer
    disp_some("Outing Now","", 1000);
}
void sensor_onoff(STATE state){       // LED ON OFF ��Ŷ  Ȯ�� 
    char *tempStr = NULL;
	/* �ΰ��� UART�� �ϳ��� ������ �ش� ���� ���� */
	if(state == COM){
	    tempStr = strComInput;
    }
    else if(state == BT){
	    tempStr = strBTInput;
    }
	
	if (tempStr[4] == 'F'){     // OFF ��Ŷ = LOCK 
        sensor_off();
		isAllSensorLock = true;
        isTimerOn = false;
    }
    else if(tempStr[4] == 'N'){ // ON ��Ŷ = LOCK ����
		disp_some(Hello, madeBy, 3000);
        isAllSensorLock = false;
        isTimerOn = true;     
    }            
}

void timer_set(STATE state){
    char disp_time[5] = {0,};
	char *tempStr = NULL;
    int i=0;  
    
    if(strComInput[2] == '>'){return;} // <T>�϶�
    if(isAllSensorLock){  // ��� ���� LOCK�϶��� �Ұ� : ����
		lcdUserInputError = 2;
		disp_some("ALL Sensor", "is Locked", 3000);
        return;    
    }
	/* �ΰ��� UART�� �ϳ��� ������ �ش� ���� ���� */
	if(state == COM){
		tempStr = strComInput;
	}
	else if(state == BT){
		tempStr = strBTInput;
	}
    
    /*
     * Ÿ�̸� ���� ��ȣ 
     */ 
    timerCnt = 0;
    userTimeInput = 0;
     
    for( i=0; (i < 4) && (tempStr[2+i] != '>'); i++){
        /*
         * ��Ŷ�� �߸� ������ ���� : ����
         */
        if(tempStr[2+i] < '0' || tempStr[2+i] > '9'){
            return;
        }
        disp_time[i] = tempStr[2+i];
        userTimeInput *= 10;
        userTimeInput += (tempStr[2+i] - '0');
    } 
    
    /*
     *  5�ڸ� �̻� ���� : ����
     */
    if(i == 4 && tempStr[6] != '>'){
		lcdUserInputError = 1;
		disp_some("timer Overflow","",3000);
        return;
    } 
    
    isTimerOn = true;
    isRunning = true;
	isTimerDone = false;
	PORTD= 0x00;

    disp_some("Timer Start : ",disp_time, 0);
}

/* ��Ŷ ���ڿ� �ʱ�ȭ */
void str_reset(STATE state){
	if(state == COM){
		memset(strComInput,0,20);  
		inputComCount = 0; 
	}
	else if(state == BT){
		memset(strBTInput,0,20);
		inputBTCount = 0;
	}
	
}

void packet_processing(STATE state){
	char *tempStr = NULL;		
	/* �ΰ��� UART�� �ϳ��� ������ �ش� ���� ���� */
	if (state == COM){
		tempStr = strComInput;
	}
	else if (state == BT){
		tempStr = strBTInput;
	}
	
	if(tempStr[1] != 'L' && tempStr[1] !='T' && tempStr[1] != 'E' && tempStr[1] != 'O') {   // ��Ŷ �Է� ����
		lcdUserInputError = 3;
		disp_some("Packet input", "is Locked", 3000);
		str_reset(state);
	}
	else{
		if(tempStr[1] == 'O'){ // OPEN
			emergency_motor_degree();
		}
		if(tempStr[1] == 'L'){  //  ���� ON/OFF
			sensor_onoff(state);
		}
		else if(tempStr[1] == 'T'){ 
			if(tempStr[2] == 'O'){ // TOFF
				PORTB=0x00;
				isTimerOn = false; // Ÿ�̸� �����ִ� ���� ����
				isRunning = false; // Ÿ�̸� ������ ������
				isTimerDone = false;
				userTimeInput = 0;
				
				lcdUserInputError=4;
				disp_some("Timer is ","interrupted",3000);
				PORTC = 0x00;
				PORTE = 0x00;
				str_reset(state);
			}
			else { // ���ڰ� �ƴ� ���ڰ� ���� ���� �߻� �ذ�, <T�ð�>
				if(!isRunning){timer_set(state);}
			}
		}
		else if(tempStr[1] == 'E' && tempStr[2] == 'O'){//<EOFF>
			end_emergency();
			isEmergency = false;
			isAllSensorLock = false;
			PORTD |= 0x80;
		}
		str_reset(state);
	}
	
}


/*
 * ��ü LED ON/OFF ��Ŷ : <LAOFF> / <LAON>
 * Ÿ�̸� ��Ŷ : <T830> / <T1010> / <TOFF>
 * ���޻�Ȳ ���� <EOFF> �߰��ؾ���
 */
/*
 * COM
 */
ISR(USART1_RX_vect)
{
    chComInput = UDR1;
	UDR1 = chComInput; //echo back, Ȯ�ο�
	
    strComInput[inputComCount++] = chComInput;      
    
	if(inputComCount >= 10){ //  ��Ŷ �ϼ� �ȵ�, ������ �� �ʰ� -> �ʱ�ȭ  
        str_reset(COM);  
    }    
    if(strComInput[0] != '<'){ // packet ���� ���� -> �ʱ�ȭ 
        str_reset(COM);  
    }   
     
    if(chComInput == '>'){
        packet_processing(COM);
	}
}

/*
 * BT
 */
ISR(USART0_RX_vect)
{
	chBTInput = UDR0;
	UDR1 = chBTInput; //echo back
	strBTInput[inputBTCount++] = chBTInput;

	if(inputBTCount >= 10){ //  ��Ŷ �ϼ� �ȵ�, ������ �� �ʰ� -> �ʱ�ȭ
		str_reset(BT);
	}
	if(strBTInput[0] != '<'){ // packet ���� ���� -> �ʱ�ȭ
		str_reset(BT);
	}
	
	if(chBTInput == '>'){
		packet_processing(BT);
	}
}
