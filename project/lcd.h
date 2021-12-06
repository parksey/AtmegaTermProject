#ifndef __LCD_H__
#define __LCD_H_

#define LCD_WDATA   PORTA
#define LCD_WINST   PORTA
#define LCD_CTRL    PORTG
#define LCD_EN      0
#define LCD_RW      1 // 읽기 :1, 쓰기 : 0
#define LCD_RS      2 // 데이터 :1, 명령어 :0


void Init_lcd_port(void);
void Init_Lcd(void);

void LCD_Data(unsigned char ch);
void LCD_Comm(unsigned char ch);
void LCD_delay(double ms);
void LCD_CHAR(unsigned char c);
void LCD_Str(unsigned char *str);
void LCD_Pos(unsigned char row, unsigned char col);
void LCD_Clear(void);
void dispSome(char*, char*, int ms);


#endif



