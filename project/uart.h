#ifndef __UART_H__
#define __UART_H__

typedef enum{COM, BT} STATE;

void sensor_onoff(STATE);
void timer_set(STATE);
void str_reset(STATE);

void Init_USART1(void);
void Init_USART0(void);


#endif
