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

/*
 * 조도 200ms trig
 */
extern int ls_cnt;

void Init_port(){
    DDRD = 0xFF;     
    PORTD = 0x00;           
}

void init_all(){
    Init_USART1();
    Init_TimerINT();
    Init_port();
    Init_Lcd();
    Init_adc();
}

void main_display(char* Hello, char* made){
    LCD_Pos(0,0);
    LCD_Str(Hello);
    LCD_Pos(1,0);
    LCD_Str(made);
}

float light_parsing(float adcVoltage){
    return -0.0354 * pow(adcVoltage,6)
                     +0.5534 * pow(adcVoltage,5)
                     -3.3318 * pow(adcVoltage,4)
                     +9.7896 * pow(adcVoltage,3)
                     -14.712 * pow(adcVoltage,2)
                     +12 * adcVoltage
                     - 0.2201;
}


int main()
{
    unsigned char Hello[] = "Hello MP2 TP";
    unsigned char madeBy[] = "With PSY & LWG";
    unsigned char index = 0x00;
    
    /*
     *  조도센서 
     */
    int res=0;
    int adcRaw=0;
    float adcVoltage = 0;
    float light_Step;
    
    float adcmilliVoltage = 0;
    char message[50];
    char message2[50];
    
    init_all(); 
    
    /*
     * 초기 화면 설정, 3초후 화면 지움 
     */
    main_display(Hello, madeBy); 
    LCD_delay(3000);
    LCD_Clear();
    
    while(1){
        if(!isAllSensorLock){  
            // SensorOn
            if(isTimerOn){
                FND_disp();            
            }
            else{
                // 나머지 센서 동작
                if(ls_cnt % 4000 == 0){
                    adcRaw = Read_ADC_data(1);
                    adcVoltage = ((((float)(adcRaw + 1) * 5) / 1024));
                    light_Step = light_parsing(adcVoltage);
                
                    if(adcVoltage < 3)
                    {
                        PORTD = 0xff;
                    }                
                    else
                    {
                        PORTD = 0x00;
                    }
                
                    sprintf(message, "RawData : %4d " ,adcRaw);
                    sprintf(message2, " %04d V, %02d ", (int)adcVoltage * 1000, (int)(light_Step * 10+4)/10);
                    dispSome(message, message2, 0); 
                }
            }
        }
        else {
            // 현재상태 : 센서 OFF
			
        } 
    }  
	return 0;  
}
