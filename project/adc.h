#ifndef __ADC_H__
#define __ADC_H__

#define ADC_VREF_TYPE 0x00
#define ADC_AVCC_TYPE 0x40
#define ADC_RES_TYPE 0x80
#define ADC_2_56_TYPE 0xC0

/*
 * 조도센서 
 */
void Init_adc(void);
unsigned int Read_ADC_data(unsigned char adc_input);
unsigned int Read_ADC_Data_Diff(unsigned char adc_mux);
float light_parsing(float adcVoltage);

void lm35();
void ls();
int ky();

#endif


