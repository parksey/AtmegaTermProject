#ifndef F_CPU
#define F_CPU 14745600
#endif

#ifndef __HEADER_H__
#define __HEADER_H__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void Init_port();
typedef enum{false, true} bool;

#endif

