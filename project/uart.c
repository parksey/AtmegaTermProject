#include "header.h"
#include "uart.h"
#include "lcd.h"
#include "timer.h"
#include <string.h>

extern bool isAllSensorLock;
extern bool isTimerOn;
extern int user_input;
extern bool isRunning;

extern int ctc_cnt; 

/*
 * �Է� �޴� �ܾ� 1�� 
 */ 
char recCh;
/*
 * ��Ŷ�ϼ��� ���� ���ڿ�  
 */
char resStr[20] = {0,};
/*
 * ��Ŷ�ϼ��� ���� ���ڿ�  
 */
char tempStr[20] = {0,};

/*
 * �Է¹��� ���� 
 */
int count;

void Init_USART1(void){     
    UCSR1A = 0x00; // 1��� ���۸��,  ���� ����
    UCSR1B = (1<<RXEN1) | (1 <<TXEN1) | (1<<RXCIE1);
    UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);  //���� ���� �������� �ʱⰪ
    UCSR1C &= ~(1<<UMSEL1); // �񵿱� ����� ����, ���� ���� 
    
    UBRR1H = 0;
    UBRR1L = 15; // 57600bps     
    sei();
}


/*
 * ��Ŷó�� 
 */
void sensor_off(){  // ����
    PORTC = 0xff;   // LED
    
    /*
     * Timer
     */
    PORTE = 0x00;
    
    /*
     * LCD�� �Ȳ����׿� 
     */
    PORTG = 0x00;
}

void sensor_OnOff(){       // LED ON OFF ��Ŷ  Ȯ�� 
    if (resStr[4] == 'F'){     // OFF ��Ŷ = LOCK 
        sensor_off();
        isAllSensorLock = true;
        isTimerOn = false;
    }
    else if(resStr[4] == 'N'){ // ON ��Ŷ = LOCK ����
        PORTC = 0x00; //test
        isAllSensorLock = false;
        isTimerOn = true;     
    }            
}

void timerSet(){
    char disp_time[5] = {0,};
    int i=0;  
    
    if(resStr[2] == '>'){return;}
    if(isAllSensorLock){
        dispSome("ALL Sensor", "is Locked", 8000);
        return;    
    }
    
    /*
     * Ÿ�̸� ���� ��ȣ 
     */ 
    ctc_cnt = 0;
    user_input = 0;
    PORTF = 0xff; // LED OFF
     
    for( i=0; i < 4 && resStr[2+i] != '>'; i++){
        /*
         * ��Ŷ�� �߸� ������ ���� 
         */
        if(resStr[2+i] < '0' || resStr[2+i] > '9'){
            return;
        }
        disp_time[i] = resStr[2+i];
        user_input *= 10;
        user_input += (resStr[2+i] - '0');
    } 
    
    /*
     *  5�ڸ� �̻� ����
     */
    if(i == 4 && resStr[6] != '>'){
        dispSome("timer Overflow","",1000);
        return;
    } 
    
    isTimerOn = true;
    isRunning = true;
    dispSome("Timer Start : ",disp_time, 0);
}

void resStr_reset(){
    memset(resStr,0,20);   
    count = 0;
}


/*
 * ��ü LED ON/OFF ��Ŷ : <LAOFF> / <LAON>
 * Ÿ�̸� ��Ŷ : <T830> / <T1010> / <TOFF>
 */
//interrupt [USART1_RXC] void usart1_receive(void)
ISR(USART_RXC_vect)
{
    recCh = UDR1;
    resStr[count++] = recCh;      
    if(count >= 10){ //  ��Ŷ �ϼ� �ȵ�, ������ �� �ʰ� -> �ʱ�ȭ  
        resStr_reset();  
    }
          
    if(resStr[0] != '<'){ // packet ���� ���� -> �ʱ�ȭ 
        resStr_reset();  
    }
        
     
    if(recCh == '>'){
        if(resStr[1] != 'L' && resStr[1] !='T') {   // LED Ȯ�� �ȵ� -> �ʱ�ȭ
            resStr_reset();  
        }
        else{
            
            if(resStr[1] == 'L'){
                sensor_OnOff();
            }
            else if(resStr[1] == 'T'){  
                if(resStr[2] == 'O'){                             
                    isTimerOn = false;
                    isRunning = false;
                    user_input = 0;
                    PORTE = 0x00;
                    resStr_reset();
                    dispSome("Timer is ","interrupted",8000);
                }else { // ���ڰ� �ƴ� ���ڰ� ���� ���� �߻� �ذ� 
                    if(!isRunning){timerSet();}
                }
                
            }
     
            memset(resStr,0,20);   
            count = 0;
        }
    }
}