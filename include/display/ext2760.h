#ifndef __ext2760_H
#define __ext2760_H

#include "stm32l1xx.h"

//***********************************************************
//Настройки контроллера дисплея и переменные для работы с ним
//***********************************************************

  
// Ножка RST
#define LCD_RST_H   GPIOB->BSRRL = GPIO_Pin_3;
#define LCD_RST_L   GPIOB->BSRRH = GPIO_Pin_3;

// Ножка DATA
#define LCD_DATA_H  GPIOB->BSRRL = GPIO_Pin_5;
#define LCD_DATA_L  GPIOB->BSRRH = GPIO_Pin_5;

// Ножка CLK
#define LCD_CLK_H   GPIOB->BSRRL = GPIO_Pin_6;
#define LCD_CLK_L   GPIOB->BSRRH = GPIO_Pin_6;

// Ножка CS
#define LCD_CS_H    GPIOB->BSRRL = GPIO_Pin_8;
#define LCD_CS_L    GPIOB->BSRRH = GPIO_Pin_8;



#define PIXEL_OFF	0		// режимы отображения пикселя - используются в графических функциях
#define PIXEL_ON	1
#define PIXEL_XOR	2

#define LCD_X_RES               96	// разрешение экрана    
#define LCD_Y_RES               72    
#define LCD_CACHSIZE          LCD_X_RES*LCD_Y_RES>>3  
//#define LCD_CACHSIZE          LCD_X_RES*LCD_Y_RES/8  

#define Cntr_X_RES              96    	// разрешение контроллера
#define Cntr_Y_RES              72   
//#define Cntr_buf_size           Cntr_X_RES*Cntr_Y_RES/8
#define Cntr_buf_size           Cntr_X_RES*Cntr_Y_RE>>3

#define lcd_CMD 1
#define lcd_DATA 2

extern char lcd_buf[];

//***************************************************
//****************Прототипы функций******************
//***************************************************
void init_lcd_port(void); // Инициализация порта LCD дисплея
void LcdInit(void);
void LcdClear (void);    //Clears the display
void LcdPixel (unsigned char x, unsigned char y, unsigned char mode);     //Displays a pixel at given absolute (x, y) location, mode -> Off, On or Xor
void LcdStringBold (unsigned char x, unsigned char y);	//Displays a string at current cursor location


//void LcdSend (unsigned char data, unsigned char cmd);    			//Sends data to display controller
void LcdUpdate (void);   							//Copies the LCD cache into the device RAM
void LcdClear (void);    							//Clears the display
void LcdInit ( void );								//Настройка SPI и дисплея
//void LcdContrast (unsigned char contrast); 					//contrast -> Contrast value from 0x00 to 0x7F
//void LcdMode (unsigned char mode); 						//режимы дисплея: 0 - blank, 1 - all on, 2 - normal, 3 - inverse
//void LcdPwrMode (void);								//инвертирует состояние вкл/выкл дисплея
//void LcdPwrOff (void);								//выкл дисплея
void LcdPwrOn (void);								//вкл дисплея
void LcdPixel (unsigned char x, unsigned char y, unsigned char mode);     	//Displays a pixel at given absolute (x, y) location, mode -> Off, On or Xor
void LcdLine (int x1, int y1, int x2, int y2, unsigned char mode);  		//Draws a line between two points on the display
//void LcdCircle(char x, char y, char radius, unsigned char mode);		//рисуем круг с координатами центра и радиусом
//void LcdBatt(int x1, int y1, int x2, int y2, unsigned char persent);		//рисуем батарейку и заполняем ее на %
void LcdGotoXYFont (unsigned char x, unsigned char y);   			//Sets cursor location to xy location. Range: 1,1 .. 14,6
void clean_lcd_buf (void);							//очистка текстового буфера
void LcdChr (int ch);								//Displays a character at current cursor location and increment cursor location
void LcdString (unsigned char x, unsigned char y);				//Displays a string at current cursor location
void LcdChrBold (int ch);							//Печатает символ на текущем месте, большой и жирный)
void LcdStringBold (unsigned char x, unsigned char y);				//Печатает большую и жирную строку
void LcdChrBig (int ch);							//Печатает символ на текущем месте, большой
void LcdStringBig (unsigned char x, unsigned char y);				//Печатает большую строку
//***************************************************
// UPDATE ##1
void LcdBar(int x1, int y1, int x2, int y2, unsigned char persent);		// рисует прогресс-бар и заполняет его на "процент"
void LcdBarLine(unsigned char line, unsigned char persent);			// рисуем прошресс-бар в указанной строке
void LcdStringInv (unsigned char x, unsigned char y);                           // печатает строку в инверсном шрифте (удобно для настроек)



void Draw_fon_graph(uint8_t x_start, uint8_t x_end, uint8_t y_start,uint8_t y_end);
void Draw_fon_digit(uint8_t line, uint8_t start_char, uint8_t seconds);
void LcdBatt(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t persent);	//рисуем батарейку с заполнением в %
void LcdClear_massive (void);
void display_on(void);
void display_off(void);
#endif
