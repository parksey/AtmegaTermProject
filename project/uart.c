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
 * 입력 받는 단어 1개 
 */ 
char recCh;
/*
 * 패킷완성을 위한 문자열  
 */
char resStr[20] = {0,};
/*
 * 패킷완성을 위한 문자열  
 */
char tempStr[20] = {0,};

/*
 * 입력받은 갯수 
 */
int count;

void Init_USART1(void){     
    UCSR1A = 0x00; // 1배속 전송모드,  생략 가능
    UCSR1B = (1<<RXEN1) | (1 <<TXEN1) | (1<<RXCIE1);
    UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);  //생략 가능 레지스터 초기값
    UCSR1C &= ~(1<<UMSEL1); // 비동기 모드의 선택, 생략 가능 
    
    UBRR1H = 0;
    UBRR1L = 15; // 57600bps     
    sei();
}


/*
 * 패킷처리 
 */
void sensor_off(){  // 외출
    PORTC = 0xff;   // LED
    
    /*
     * Timer
     */
    PORTE = 0x00;
    
    /*
     * LCD는 안꺼지네용 
     */
    PORTG = 0x00;
}

void sensor_OnOff(){       // LED ON OFF 패킷  확인 
    if (resStr[4] == 'F'){     // OFF 패킷 = LOCK 
        sensor_off();
        isAllSensorLock = true;
        isTimerOn = false;
    }
    else if(resStr[4] == 'N'){ // ON 패킷 = LOCK 해제
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
     * 타이머 시작 신호 
     */ 
    ctc_cnt = 0;
    user_input = 0;
    PORTF = 0xff; // LED OFF
     
    for( i=0; i < 4 && resStr[2+i] != '>'; i++){
        /*
         * 패킷이 잘못 들어옴을 감지 
         */
        if(resStr[2+i] < '0' || resStr[2+i] > '9'){
            return;
        }
        disp_time[i] = resStr[2+i];
        user_input *= 10;
        user_input += (resStr[2+i] - '0');
    } 
    
    /*
     *  5자리 이상 감지
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
 * 전체 LED ON/OFF 패킷 : <LAOFF> / <LAON>
 * 타이머 패킷 : <T830> / <T1010> / <TOFF>
 */
//interrupt [USART1_RXC] void usart1_receive(void)
ISR(USART_RXC_vect)
{
    recCh = UDR1;
    resStr[count++] = recCh;      
    if(count >= 10){ //  패킷 완성 안됨, 데이터 수 초과 -> 초기화  
        resStr_reset();  
    }
          
    if(resStr[0] != '<'){ // packet 만족 못함 -> 초기화 
        resStr_reset();  
    }
        
     
    if(recCh == '>'){
        if(resStr[1] != 'L' && resStr[1] !='T') {   // LED 확인 안됨 -> 초기화
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
                }else { // 숫자가 아닌 문자가 들어가면 문제 발생 해결 
                    if(!isRunning){timerSet();}
                }
                
            }
     
            memset(resStr,0,20);   
            count = 0;
        }
    }
}