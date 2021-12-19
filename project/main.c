/*
 * main.c
 *
 * Created: 2021-11-27 오후 3:55:39
 * Author: sypark
 */
#include "header.h"
#include "uart.h"
#include "timer.h"
#include "lcd.h"
#include "adc.h"
#include <math.h>
#include <stdio.h>

/*
 * 전체 동작 제어를 위한 변수
 */
bool isAllSensorLock = false;
bool isTimerOn = false;
bool isRunning = false;
bool isTimerDone = false;
bool isEmergency = false;
int lcdUserInputError = 0;

/*
 * LCD 출력
 */
unsigned char Hello[] = "Hello MP2 TP";
unsigned char madeBy[] = "With PSY & LWG";

void init_port(){
	/*
	 * PORTA & G : LCD
	 */
	DDRA = 0xff;
	DDRG = 0x0f;
	/*
	 * PD.7 : LED
	 */
    DDRD = 0x80;     
		  
	DDRB = 0xff; // 모터               : PB5
	DDRC = 0xff; // Timer2(FND Data)  : PB0~7 
	DDRE = 0xf0; // FND(LCD 출력)     : PE 4567 전환      
	
	/*
	 * PF1 : 조도
	 * PF2 : 온도용 GND
	 * PF3 : 온도용 Input, vcc
	 */
	PORTD = 0x00;
}

void init_all(){
	init_port();
    Init_USART0();
	Init_USART1();
    init_Timer();
    init_lcd();
    Init_adc();
}

void lcd_user_input_error(){
	switch(lcdUserInputError){
		//case 0 : break; 
		case 1: disp_some("timer Overflow","",1000); break;
		case 2: disp_some("ALL Sensor", "is Locked", 8000); break;
		case 3: disp_some("Packet input", "is Locked", 8000); break;
		case 4: disp_some("Timer is ","interrupted",3000); break;
		case 5: disp_some("Timer is Done","",3000); break;
	}
	lcdUserInputError = 0;
}

int main()
{
	int emergency =0;
    init_all(); 
    /*
     * 초기 화면 설정, 3초후 화면 지움 
     */
    disp_some(Hello, madeBy,3000); 

    while(1){
		/*
		if(isEmergency){
			dispSome("Emergency NOW", "HURRY UP", 0);
			continue;
		}
		
		if(ky() <3){
			emergency_motor_degree();
			isEmergency = true;
		}
		*/
		if(!isAllSensorLock){
			// SensorOn
			if(isRunning){
				fnd_disp();
			}
			else{
				// 나머지 센서 동작
				if(lsCnt % 4000 == 0){
					if(isTimerDone){ // Timer로 인한 기상
						// 온도
						lm35();
					} 
					else{ // Timer로인한 기상 X
						ls();
					}
				}
			}
		}
		else {
			// 현재상태 : 센서 OFF
			PORTD = 0x00;
		}	
    }  
	return 0;  
}
