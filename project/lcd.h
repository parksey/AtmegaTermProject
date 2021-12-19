#ifndef __LCD_H__
#define __LCD_H_

#define LCD_WDATA   PORTA
#define LCD_WINST   PORTA
#define LCD_CTRL    PORTG
#define LCD_EN      0
#define LCD_RW      1 // 읽기 :1, 쓰기 : 0
#define LCD_RS      2 // 데이터 :1, 명령어 :0

void init_lcd(void);
void lcd_data(unsigned char ch);
void lcd_comm(unsigned char ch);
void lcd_delay(double ms);
void lcd_char(unsigned char c);
void lcd_str(unsigned char *str);
void lcd_pos(unsigned char row, unsigned char col);
void lcd_clear(void);
void disp_some(char*, char*, int ms);

#endif