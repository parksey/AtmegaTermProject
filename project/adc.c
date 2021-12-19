#include "lcd.h"
#include "header.h"
#include "adc.h"

void Init_adc(void)
{
    ADCSRA = 0x00;
    ADMUX = ADC_AVCC_TYPE | (0 << ADLAR) | (0 << MUX0);
    ADCSRA = (1<<ADEN) | (1<<ADFR) | (3<<ADPS0);
}

unsigned int Read_ADC_data(unsigned char adc_input)
{
    unsigned int adc_Data = 0;
    
    ADMUX &= ~(0x1F);
    ADMUX |= (adc_input & 0x07);
    ADCSRA |= (1<<ADSC);
    while(!(ADCSRA & (1<<ADIF)));
    adc_Data = ADCL;
    adc_Data |= ADCH << 8;
    
    return adc_Data; 
}


unsigned int Read_ADC_Data_Diff(unsigned char adc_mux)
{
    unsigned int ADC_Data = 0;
    
    if(adc_mux < 8)
        return 0xFFFF;
        
    ADMUX &= ~(0x1F);
    ADMUX |= (adc_mux & 0x1f);
    ADCSRA |= (1<<ADSC);
    
    while(!(ADCSRA & (1<< ADIF)));
    
    ADC_Data = ADCL;
    ADC_Data |= ADCH << 8;
    
    return ADC_Data;    
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
