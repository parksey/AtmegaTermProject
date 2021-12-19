#include "header.h"
#include "lcd.h"
#include "adc.h"
#include <stdio.h>


int ky(){
	int adcRaw=0;
	float adcVoltage = 0;
	float light_Step;
	
	char message[50];
	char message2[50];
	
	int dummy = 0;
	
	dummy = Read_ADC_data(4);
	adcRaw = Read_ADC_data(4);
	adcVoltage = ((((float)(adcRaw + 1) * 5) / 1024));
	light_Step = light_parsing(adcVoltage);

	
	sprintf(message, "FireData : %4d " ,adcRaw);
	sprintf(message2, " %04d V, %02d ", (int)adcVoltage * 1000, (int)(light_Step * 10+4)/10);
	disp_some(message, message2, 0);

	return light_Step;
}

void ls(){
	/*
     *  조도센서 
     */
    int adcRaw=0;
	int dummy = 0;
    float adcVoltage = 0;
    float light_Step;
    
    char message[50];
    char message2[50];
	
	dummy = Read_ADC_data(1); //ADC 2개 사용 오류로 인해 더미로 데이터 가져가봤습니다.
	adcRaw = Read_ADC_data(1);
	adcVoltage = ((((float)(adcRaw + 1) * 5) / 1024));
	light_Step = light_parsing(adcVoltage);
	
	if(adcVoltage < 3)
	{
		PORTD |= 0x80; // 1000 0000
	}
	else
	{
		PORTD &= 0x7f; // 0111
	}
	
	sprintf(message, "RawData : %4d " ,adcRaw);
	sprintf(message2, " %04d V, %02d ", (int)adcVoltage * 1000, (int)(light_Step * 10+4)/10);
	disp_some(message, message2, 0);	
}


void lm35()
{
   int temp_adcRaw = 0;
   float adcmilliVoltage = 0;
   float Celsius = 0;
   char message[50] ={0, };
   char message2[50] = {0, };
   

    temp_adcRaw = Read_ADC_Data_Diff(0b01101);
    adcmilliVoltage = ( (( (float)temp_adcRaw * 5000) / 512) /10);
    Celsius = adcmilliVoltage / 20;    
        
    sprintf(message, "RawData: %04d ", temp_adcRaw);  
    //  sprintf(Message, "Temp(deg): %2.2f ", Celsius); 
    //sprintf(message2, "Temp(deg) : %2d.%2d ", (int)Celsius, (int)(Celsius * 100) % 100);
	sprintf(message2, "is : %2d.%2d ", (int)Celsius, (int)(Celsius * 100) % 100);
	disp_some("Current Temp", message2 , 0);	

}
