#include "header.h"
#include "lcd.h"

void Init_lcd_port(void){
    DDRA = 0xff;
    DDRG = 0x0f;
}
void Init_Lcd(void){
    Init_lcd_port();
    
    LCD_Comm(0x30);
    _delay_us(4100);
    LCD_Comm(0x30);
    _delay_us(100);
    LCD_Comm(0x30);
    _delay_us(100);
    LCD_Comm(0x38);
    _delay_us(1000);
    LCD_Comm(0x0e);
    _delay_us(1000);
    LCD_Comm(0x01);
    _delay_us(2000);
    LCD_Comm(0x06);
    _delay_us(1000);
}

void LCD_Data(unsigned char ch){
    //DR 선택 후 DDRAM or CGRAM 값  
    LCD_CTRL |= (1 << LCD_RS);  // RS : 1  데이터
    LCD_CTRL &= ~(1 << LCD_RW); // RW : 0  쓰기
    LCD_CTRL |= (1 << LCD_EN);
    _delay_us(50);
    LCD_WDATA = ch;
    _delay_us(50);
    LCD_CTRL &= ~(1 << LCD_EN);
}
void LCD_Comm(unsigned char ch){
    // IR 선택 제어 명령 
    LCD_CTRL &= ~(1 << LCD_RS); // RS : 0 명령어
    LCD_CTRL &= ~(1 << LCD_RW); // RW : 0 쓰기 
    LCD_CTRL |= (1 << LCD_EN);
    _delay_us(50);
    LCD_WINST = ch;
    _delay_us(50);
    LCD_CTRL &= ~(1 << LCD_EN);
}
void LCD_delay(double ms){
	while(ms>0){
		_delay_ms(1);
		ms--;	
	}
    
} 
void LCD_CHAR(unsigned char c){
    LCD_Data(c);
    _delay_ms(1);
}
void LCD_Str(unsigned char *str){
    while(*str != 0){
        LCD_CHAR(*str);
        str++;
    }
}
void LCD_Pos(unsigned char row, unsigned char col){
    LCD_Comm(0x80 | ((row * 0x40) + col));
}
void LCD_Clear(void){
    LCD_Comm(0x01);
    LCD_delay(2);
}

void dispSome(char *disp, char *disp2, int ms){
    LCD_Clear();
    LCD_Pos(0,0);
    LCD_Str(disp);
    LCD_Pos(1,0);
    LCD_Str(disp2); 
    
    if(ms != 0){
        LCD_delay(ms);
        LCD_Clear();
    }
}