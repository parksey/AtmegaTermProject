#include "header.h"
#include "uart.h"
#include "lcd.h"
#include "timer.h"
#include <string.h>

/*
 * USART1 = 보드 
 * USART0 = 블루투스
 */
/*
 * 입력 받는 단어 1개 
 */ 
char chComInput;
char chBTInput;
/*
 * 패킷완성을 위한 문자열  
 */
char strComInput[20] = {0,};
char strBTInput[20] = {0,};
/*
 * 입력받은 갯수 
 */
int inputComCount = 0;
int inputBTCount = 0;

void Init_USART1(void){     
    UCSR1A = 0x00; // 1배속 전송모드,  생략 가능
    UCSR1B = (1<<RXEN1) | (1 <<TXEN1) | (1<<RXCIE1);
    UCSR1C = (1<<UCSZ11) | (1<<UCSZ10);  //생략 가능 레지스터 초기값
    UCSR1C &= ~(1<<UMSEL1); // 비동기 모드의 선택, 생략 가능 
    UBRR1H = 0;
    UBRR1L = 95; //9600bps     
    sei();
}
void Init_USART0(void){
	UCSR0A=0x00; // 1배속 전송모드,  생략 가능
	UCSR0B=0x98; // RX 인터럽트 수신 허가 | 수신허가 | 송신 허가
	UCSR0C=0x06; //생략 가능 레지스터 초기값
	UBRR0H=0;
	UBRR0L=95; 
}
/*
 * 패킷처리 
 */
void sensor_off(){  // 외출
    PORTD = 0x00;   // LED
    PORTE = 0x00;   // Timer
    disp_some("Outing Now","", 1000);
}
void sensor_onoff(STATE state){       // LED ON OFF 패킷  확인 
    char *tempStr = NULL;
	/* 두개의 UART중 하나라도 들어오면 해당 동작 수행 */
	if(state == COM){
	    tempStr = strComInput;
    }
    else if(state == BT){
	    tempStr = strBTInput;
    }
	
	if (tempStr[4] == 'F'){     // OFF 패킷 = LOCK 
        sensor_off();
		isAllSensorLock = true;
        isTimerOn = false;
    }
    else if(tempStr[4] == 'N'){ // ON 패킷 = LOCK 해제
		disp_some(Hello, madeBy, 3000);
        isAllSensorLock = false;
        isTimerOn = true;     
    }            
}

void timer_set(STATE state){
    char disp_time[5] = {0,};
	char *tempStr = NULL;
    int i=0;  
    
    if(strComInput[2] == '>'){return;} // <T>일때
    if(isAllSensorLock){  // 모든 센서 LOCK일때는 불가 : 오류
		lcdUserInputError = 2;
		disp_some("ALL Sensor", "is Locked", 3000);
        return;    
    }
	/* 두개의 UART중 하나라도 들어오면 해당 동작 수행 */
	if(state == COM){
		tempStr = strComInput;
	}
	else if(state == BT){
		tempStr = strBTInput;
	}
    
    /*
     * 타이머 시작 신호 
     */ 
    timerCnt = 0;
    userTimeInput = 0;
     
    for( i=0; (i < 4) && (tempStr[2+i] != '>'); i++){
        /*
         * 패킷이 잘못 들어옴을 감지 : 오류
         */
        if(tempStr[2+i] < '0' || tempStr[2+i] > '9'){
            return;
        }
        disp_time[i] = tempStr[2+i];
        userTimeInput *= 10;
        userTimeInput += (tempStr[2+i] - '0');
    } 
    
    /*
     *  5자리 이상 감지 : 오류
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

/* 패킷 문자열 초기화 */
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
	/* 두개의 UART중 하나라도 들어오면 해당 동작 수행 */
	if (state == COM){
		tempStr = strComInput;
	}
	else if (state == BT){
		tempStr = strBTInput;
	}
	
	if(tempStr[1] != 'L' && tempStr[1] !='T' && tempStr[1] != 'E' && tempStr[1] != 'O') {   // 패킷 입력 오류
		lcdUserInputError = 3;
		disp_some("Packet input", "is Locked", 3000);
		str_reset(state);
	}
	else{
		if(tempStr[1] == 'O'){ // OPEN
			emergency_motor_degree();
		}
		if(tempStr[1] == 'L'){  //  센서 ON/OFF
			sensor_onoff(state);
		}
		else if(tempStr[1] == 'T'){ 
			if(tempStr[2] == 'O'){ // TOFF
				PORTB=0x00;
				isTimerOn = false; // 타이머 켜져있는 상태 막기
				isRunning = false; // 타이머 켜지고 동작중
				isTimerDone = false;
				userTimeInput = 0;
				
				lcdUserInputError=4;
				disp_some("Timer is ","interrupted",3000);
				PORTC = 0x00;
				PORTE = 0x00;
				str_reset(state);
			}
			else { // 숫자가 아닌 문자가 들어가면 문제 발생 해결, <T시간>
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
 * 전체 LED ON/OFF 패킷 : <LAOFF> / <LAON>
 * 타이머 패킷 : <T830> / <T1010> / <TOFF>
 * 응급상황 종료 <EOFF> 추가해야함
 */
/*
 * COM
 */
ISR(USART1_RX_vect)
{
    chComInput = UDR1;
	UDR1 = chComInput; //echo back, 확인용
	
    strComInput[inputComCount++] = chComInput;      
    
	if(inputComCount >= 10){ //  패킷 완성 안됨, 데이터 수 초과 -> 초기화  
        str_reset(COM);  
    }    
    if(strComInput[0] != '<'){ // packet 만족 못함 -> 초기화 
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

	if(inputBTCount >= 10){ //  패킷 완성 안됨, 데이터 수 초과 -> 초기화
		str_reset(BT);
	}
	if(strBTInput[0] != '<'){ // packet 만족 못함 -> 초기화
		str_reset(BT);
	}
	
	if(chBTInput == '>'){
		packet_processing(BT);
	}
}
