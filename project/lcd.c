#include "header.h" //delay
#include "lcd.h"

void init_lcd(void){
    lcd_comm(0x30);
    _delay_us(4100);
    lcd_comm(0x30);
    _delay_us(100);
    lcd_comm(0x30);
    _delay_us(100);
    lcd_comm(0x38); // 초기화, 8bit
    _delay_us(1000);
    lcd_comm(0x0e); // disp On, Cur On, Blink Off
    _delay_us(1000);
    lcd_comm(0x01); // LCD Clear
    _delay_us(2000);
    lcd_comm(0x06);
    _delay_us(1000);
}

void lcd_data(unsigned char ch){
    //DR 선택 후 DDRAM  
    LCD_CTRL |= (1 << LCD_RS);  // RS : 1  데이터
    LCD_CTRL &= ~(1 << LCD_RW); // RW : 0  쓰기
    LCD_CTRL |= (1 << LCD_EN);
    _delay_us(50);
    LCD_WDATA = ch;
    _delay_us(50);
    LCD_CTRL &= ~(1 << LCD_EN);
}
void lcd_comm(unsigned char ch){
    // IR 선택 제어 명령 
    LCD_CTRL &= ~(1 << LCD_RS); // RS : 0 명령어
    LCD_CTRL &= ~(1 << LCD_RW); // RW : 0 쓰기 
    LCD_CTRL |= (1 << LCD_EN);
    _delay_us(50);
    LCD_WINST = ch;
    _delay_us(50);
    LCD_CTRL &= ~(1 << LCD_EN);
}
void lcd_delay(double ms){
	while(ms>0){
		_delay_ms(1);
		ms--;	
	}
} 
void lcd_char(unsigned char c){
    lcd_data(c);
    _delay_ms(1);
}
void lcd_str(unsigned char *str){
    while(*str != 0){
        lcd_char(*str);
        str++;
    }
}
void lcd_pos(unsigned char row, unsigned char col){
    lcd_comm(0x80 | ((row * 0x40) + col));
}
void lcd_clear(void){
    lcd_comm(0x01);
    lcd_delay(2);
}
void disp_some(char *disp, char *disp2, int ms){
    lcd_clear();
    lcd_pos(0,0);   //100us
    lcd_str(disp);  //100us
    lcd_pos(1,0);   //100us
    lcd_str(disp2); //100us
    
    if(ms != 0){
        lcd_delay(ms);
        lcd_clear();
    }
}