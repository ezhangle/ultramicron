/**********************************************

Драйвер для внешнего дисплея от Nokia 2760 
Диоганаль: 1 дюйм.
Разрешение: 96 x 68

Автор: Shodan (www.радиокружок.net)

Это переделанный драйвер 3310 от Aheir с Радиокота.
Который я использовал в дозиметре Микрон-2

**********************************************/

#include <stdio.h>
#include "font5x8.h"
#include "main.h"
#include "delay.h"
#include "ext2760.h"

char lcd_buf[20];		        // текстовый буфер для вывода на LCD
unsigned char  LcdCache [LCD_CACHSIZE];	// Фреймбуфер
unsigned int   LcdCacheIdx=0;             // Текущий адрес во фреймбуфере



void display_on() // Инициализация порта LCD дисплея
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  
  Power.Display_active=ENABLE;
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  // ===============================================================================================  
  // LCD дисплей
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  // ===============================================================================================  
  // Ножка LCDLED
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;           // Ножка
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(     GPIOC,&GPIO_InitStructure);  // Загружаем конфигурацию
  
GPIO_SetBits(GPIOB,GPIO_Pin_7);// Включаем питание дисплея
GPIO_ResetBits(GPIOC,GPIO_Pin_13);// Включаем подсветку  
  delay_ms(10);
  
  LcdInit(); // Инициализируем дисплей
  LcdClear();
  
}

void display_off() // Инициализация порта LCD дисплея
{
  
  GPIO_InitTypeDef   GPIO_InitStructure;
  
  Power.Display_active=DISABLE;  
 
  GPIO_ResetBits(GPIOB,GPIO_Pin_7);// Выключаем питание дисплея
  
  // ===============================================================================================  
  // LCD дисплей
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB,GPIO_InitStructure.GPIO_Pin);// на землю
  
  // ===============================================================================================  
  
  GPIO_SetBits(GPIOC,GPIO_Pin_13);// Выключаем подсветку (Hi-Z)
  
}

void LcdSend (uint8_t data, uint8_t cmd)    //Sends data to display controller
{
  uint8_t		sdata=0,sbit=0,ibit=0;
  
  sdata=data;
  LCD_CLK_L;	//clk низкий уровень
  LCD_CS_L;		//CS низкий уровень
  if(cmd==lcd_CMD){LCD_DATA_L;} //идентефикатор кода
  else            {LCD_DATA_H;} //идентефикатор данных
  LCD_CLK_H;	//выдаём строб		
  LCD_CLK_L;	//опускаем
  ibit=0;
  while(ibit<8) //выдаем побитно данные в дисплей
  {
    sbit=sdata&0x80;
    if (sbit==0x80) {LCD_DATA_H;}
    else            {LCD_DATA_L; }
    LCD_CLK_H;	//выдаём строб		
    LCD_CLK_L;	//опускаем
    sdata=sdata<<1;
    ibit++;
  }
  LCD_DATA_L;
}


void LcdInit()
{
  
  LCD_CS_L;
  LCD_RST_L;
  delay_ms(10);
  LCD_RST_H;
  delay_ms(10);
  LcdSend(0xE2,lcd_CMD);
  LcdSend(0xAF,lcd_CMD);
  LcdSend(0xA4,lcd_CMD);
  LcdSend(0x2F,lcd_CMD);
  LcdSend(0xB0,lcd_CMD);
  LcdSend(0xA7,lcd_CMD);
  LcdSend(0x10,lcd_CMD);
  LcdSend(0x00,lcd_CMD);
  LcdSend(0x90+Settings.contrast,lcd_CMD); // Контраст 0x90 минимальный - 0x9f максимальный
	
  if(Settings.Display_reverse&0x01)LcdSend(0xC8,lcd_CMD);// зеркалит развертку по Y
  if(Settings.Display_reverse&0x02)LcdSend(0xA1,lcd_CMD);// зеркалит развертку по Х
  //LcdSend(0xA1,lcd_CMD);
  
}



void LcdUpdate (void)   //Copies the LCD cache into the device RAM
{
  int i=0,j=0;
  
  LcdSend(0xB0,lcd_CMD); //Позицианируем курсор на начало координат
  LcdSend(0x10,lcd_CMD);
  LcdSend(0x00,lcd_CMD);
  
  for (i = 0; i < (LCD_Y_RES>>3); i++)		//грузим данные строками (было деление на 8)
    for (j = LCD_X_RES-1; j>=0; j--)		//грузим данные столюиками по 8 пикселей
    {
      LcdSend(LcdCache[((i*LCD_X_RES)+j)], lcd_DATA); //вычисляем адрес в фрейм буфере, и данные от туда грузим в дисплей.
    }
}


void LcdClear (void)    //Clears the display
{
  int i=0;
  
  for (i = 0; i < LCD_CACHSIZE; i++) LcdCache[i] = 0;	//забиваем всю память 0
  LcdUpdate ();
}


void LcdClear_massive (void)    //Clears the display
{
  int i=0;
  
  for (i = 0; i < LCD_CACHSIZE; i++) LcdCache[i] = 0;	//забиваем всю память 0
}


void LcdPixel (unsigned char x, unsigned char y, unsigned char mode)     //Displays a pixel at given absolute (x, y) location, mode -> Off, On or Xor
{
  int index=0;
  unsigned char offset=0, data=0;
  
  if ( x > LCD_X_RES ) return;	//если передали в функцию муть - выходим
  if ( y > LCD_Y_RES ) return;
  
  //index = (((int)(y)/8)*96)+x;    //считаем номер байта в массиве памяти дисплея
	//offset  = y-((y/8)*8);          //считаем номер бита в этом байте
	index = (((int)(y)>>3)*96)+x;    //считаем номер байта в массиве памяти дисплея
  offset  = y-((y>>3)<<3);          //считаем номер бита в этом байте
  
  data = LcdCache[index];         //берем байт по найденному индексу
  
  if ( mode == PIXEL_OFF ) data &= ( ~( 0x01 << offset ) );	//редактируем бит в этом байте
  else if ( mode == PIXEL_ON ) data |= ( 0x01 << offset );
  else if ( mode  == PIXEL_XOR ) data ^= ( 0x01 << offset );
  
  LcdCache[index] = data;		//загружаем байт назад
}

void LcdLine (int x1, int y1, int x2, int y2, unsigned char mode)  	//Draws a line between two points on the display - по Брезенхейму
{
  signed int dy = 0;
  signed int dx = 0;
  signed int stepx = 0;
  signed int stepy = 0;
  signed int fraction = 0;
  
  if (x1>LCD_X_RES || x2>LCD_X_RES || y1>LCD_Y_RES || y2>LCD_Y_RES) return;
  
  dy = y2 - y1;
  dx = x2 - x1;
  if (dy < 0) 
  {
    dy = -dy;
    stepy = -1;
  }
  else stepy = 1;
  if (dx < 0)
  {
    dx = -dx;
    stepx = -1;
  }
  else stepx = 1;
  dy <<= 1;
  dx <<= 1;
  LcdPixel(x1,y1,mode);
  if (dx > dy)
  {
    fraction = dy - (dx >> 1); 
    while (x1 != x2)
    {
      if (fraction >= 0)
      {
        y1 += stepy;
        fraction -= dx;
      }
      x1 += stepx;
      fraction += dy;  
      LcdPixel(x1,y1,mode);
    }
  }
  else
  {
    fraction = dx - (dy >> 1);
    while (y1 != y2)
    {
      if (fraction >= 0)
      {
        x1 += stepx;
        fraction -= dy;
      }
      y1 += stepy;
      fraction += dx;
      LcdPixel(x1,y1,mode);
    }
  }
}


void LcdGotoXYFont (unsigned char x, unsigned char y)   //Sets cursor location to xy location. Range: 1,1 .. 14,6
{
  //if (x <= 14 && y <= 6) 
  LcdCacheIdx = ( (int)(y) - 1 ) * 96 + ( (int)(x) - 1 ) * 6;
}

void clean_lcd_buf (void)	//очистка текстового буфера
{
  char i=0;
  
  for (i=0; i<20; i++) lcd_buf[i] = 0;
} 

void LcdChr (int ch)	//Displays a character at current cursor location and increment cursor location
{
  char i=0;
  if(ch>0x7f)
  {
    for ( i = 0; i < 5; i++ ) LcdCache[LcdCacheIdx++] = lcd_font_table_rus[(ch*5+(i)-0x3C0)];	//выделяем байт-столбик из символа и грузим в массив - 5 раз
  }else{
    for ( i = 0; i < 5 ; i++ ) LcdCache[LcdCacheIdx++] = lcd_font_table[(ch*5+(i)-0xA0)];	//выделяем байт-столбик из символа и грузим в массив - 5 раз        
  }
  LcdCache[LcdCacheIdx++] = 0x00;	//добавляем пробел между символами
}

void LcdChrInv (int ch)	//Displays a character at current cursor location and increment cursor location
{
  unsigned char i=0;
  if(ch>0x7f)
  {     	
    for ( i = 0; i < 5; i++ ) LcdCache[LcdCacheIdx++] = ~(lcd_font_table_rus[(ch*5+i-0x3C0)]);	//выделяем байт-столбик из символа и грузим в массив - 5 раз
  }else{
    for ( i = 0; i < 5; i++ ) LcdCache[LcdCacheIdx++] = ~(lcd_font_table[(ch*5+i-0xA0)]);	//выделяем байт-столбик из символа и грузим в массив - 5 раз        
  }
  LcdCache[LcdCacheIdx++] = 0xFF;	//добавляем пробел между символами
}

void LcdString (unsigned char x, unsigned char y)	//Displays a string at current cursor location
{
  unsigned char i=0;
  
  if (x > 17 || y > 8) return;
  LcdGotoXYFont (x, y);
  for ( i = 0; i < 17; i++ ) if (lcd_buf[i]) LcdChr (lcd_buf[i]);
  clean_lcd_buf(); 
}

void LcdStringInv (unsigned char x, unsigned char y)	//Displays a string at current cursor location
{
  unsigned char i=0;
  
  if (x > 17 || y > 8) return;
  LcdGotoXYFont (x, y);
  for ( i = 0; i < 17-x; i++ ) if (lcd_buf[i]) LcdChrInv (lcd_buf[i]);
  clean_lcd_buf(); 
}

void LcdChrBold (int ch)	//Displays a bold character at current cursor location and increment cursor location
{
  unsigned char i=0;
  unsigned char a = 0, b = 0, c = 0;
  
  for ( i = 0; i < 5; i++ )
  {
    if(ch>0x7f)
    {
      c = lcd_font_table_rus[(ch*5+i-0x3C0)];		//выделяем столбец из символа
    }else{
      c = lcd_font_table[(ch*5+i-0xA0)];		//выделяем столбец из символа                
    }
    
    b =  (c & 0x01) * 3;            //"растягиваем" столбец на два байта 
    b |= (c & 0x02) * 6;
    b |= (c & 0x04) * 12;
    b |= (c & 0x08) * 24;
    
    c >>= 4;
    a =  (c & 0x01) * 3;
    a |= (c & 0x02) * 6;
    a |= (c & 0x04) * 12;
    a |= (c & 0x08) * 24;
    
    LcdCache[LcdCacheIdx] = b;	//копируем байты в экранный буфер
    LcdCache[LcdCacheIdx+1] = b;    //дублируем для получения жирного шрифта
    LcdCache[LcdCacheIdx+96] = a;
    LcdCache[LcdCacheIdx+97] = a;
    LcdCacheIdx = LcdCacheIdx+2;
  }
  LcdCache[LcdCacheIdx++] = 0x00;	//для пробела между символами
  LcdCache[LcdCacheIdx++] = 0x00;
}

void LcdStringBold (unsigned char x, unsigned char y)	//Displays a string at current cursor location
{
  unsigned char i=0;
  if (x > 17 || y > 8) return;
  LcdGotoXYFont (x, y);
  for ( i = 0; i < 17-x; i++ ) if (lcd_buf[i]) LcdChrBold (lcd_buf[i]); 
  clean_lcd_buf();
}

void LcdChrBig (int ch)	//Displays a character at current cursor location and increment cursor location
{
  unsigned char i=0;
  unsigned char a = 0, b = 0, c = 0;
  
  for ( i = 0; i < 5; i++ )
  {
    if(ch>0x7f)
    {
      c = lcd_font_table_rus[(ch*5+i-0x3C0)];		//выделяем столбец из символа     	            
    }else{
      c = lcd_font_table[(ch*5+i-0xA0)];		//выделяем столбец из символа
    }
    
    b =  (c & 0x01) * 3;            //"растягиваем" столбец на два байта 
    b |= (c & 0x02) * 6;
    b |= (c & 0x04) * 12;
    b |= (c & 0x08) * 24;
    
    c >>= 4;
    a =  (c & 0x01) * 3;
    a |= (c & 0x02) * 6;
    a |= (c & 0x04) * 12;
    a |= (c & 0x08) * 24;
    LcdCache[LcdCacheIdx] = b;
    LcdCache[LcdCacheIdx+96] = a;
    LcdCacheIdx = LcdCacheIdx+1;
  }
  
  LcdCache[LcdCacheIdx++] = 0x00;
}

void LcdStringBig (unsigned char x, unsigned char y)	//Displays a string at current cursor location
{
  unsigned char i=0;
  
  if (x > 17 || y > 8) return;
  LcdGotoXYFont (x, y);
  for ( i = 0; i < 17-x; i++ ) if (lcd_buf[i]) LcdChrBig (lcd_buf[i]); 
  clean_lcd_buf();
}
//////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////
void Draw_fon_graph(uint8_t x_start, uint8_t x_end, uint8_t y_start,uint8_t y_end)
{
  uint32_t x_lenght=0,y_lenght=0;
  uint32_t q=0,i=0,j=0, pointer=0;
  uint32_t scalling_factor=0;
  
  for(i=x_start;i<=x_end;i++)
  {
    for(j=y_start;j<=y_end;j++)
    {
      LcdPixel(i,j,0);
    }
  }
  
  x_lenght=x_end-x_start;
  y_lenght=y_end-y_start;
  
  scalling_factor=y_lenght/10;
  for(q=0;q<(x_lenght/2)+1;q++)
  {
			if(Detector_massive_pointer>=q)
			{
				pointer=Detector_massive_pointer-q;
			}else
			{
				pointer=(Settings.Second_count>>2)-(q-Detector_massive_pointer);
			}
			
			if(Detector_massive[pointer]>scalling_factor)scalling_factor=Detector_massive[pointer];
  }
  	
  for(q=x_end;q>x_start;q--)
  {
  		if(Detector_massive_pointer>=((x_end-q)/2))
			{
				pointer=Detector_massive_pointer-((x_end-q)/2);
			}else
			{
				pointer=(Settings.Second_count>>2)-(((x_end-q)/2)-Detector_massive_pointer);
			}

    i=Detector_massive[pointer];
    if(i>0) LcdLine(x_end-q,y_end,x_end-q,y_end-((i*y_lenght)/scalling_factor),1);
  }
  
  //risuem setku
  LcdLine(x_start,y_start,x_start,y_start-2,1); 
  LcdLine(x_start,y_end,x_start,y_end+2,1);
  
  LcdLine(x_end,y_start,x_end,y_start-2,1);
  LcdLine(x_end,y_end,x_end,y_end+2,1);
  
  LcdPixel(x_start,y_start+(y_lenght/2),1);
  LcdPixel(x_end,y_start+(y_lenght/2),1);
  
  for(j=x_lenght;j>0;j--)
  {
    if((j%4)==0)
    {
      LcdPixel(x_start+j,y_start,1); // up line
      LcdPixel(x_start+j,y_start+(y_lenght/2),1); // middle line
      LcdPixel(x_start+j,y_end,1); // down line
    }
    
    if((j%8)==0)
    {
      for(q=y_lenght;q>0;q--)
      {
        if((q%3)==0)
          LcdPixel(x_start+j,y_end-q,1); // vertical line
      }
    }
  }
  
}
//////////////////////////////////////////////////////////////////////////////////////


void Draw_fon_digit(uint8_t line, uint8_t start_char, uint8_t seconds)
{
  uint32_t  i=0,digit_num=0;
  
  
  i=(line*2)-1;
  
  sprintf(lcd_buf,"%6i",fon_level);
  LcdStringBold(start_char,i);
  
  
  sprintf(lcd_buf,"мкР");                            
  LcdString(((start_char+6)*2),i);                        
  
  sprintf(lcd_buf," ч ");                            
  LcdString(((start_char+6)*2),i+1);
  
  LcdLine(((start_char+5)*12)+5,i*8,((start_char+5)*12)+18+5,i*8,1);
  for(digit_num=0;digit_num<6;digit_num++)
  {
    LcdLine(((start_char+digit_num-1)*10)+(2*digit_num)   ,line*16,((start_char+digit_num-1)*10)+(2*digit_num)+10,line*16    ,1);
    LcdLine(((start_char+digit_num-1)*10)+(2*digit_num)   ,line*16,((start_char+digit_num-1)*10)+(2*digit_num)   ,(line*16)-3,1);
    LcdLine(((start_char+digit_num-1)*10)+(2*digit_num)+10,line*16,((start_char+digit_num-1)*10)+(2*digit_num)+10,(line*16)-3,1);
    
    /*  LcdLine(((start_char+digit_num-1)*10)+(2*digit_num)   ,(line*16)-19,((start_char+digit_num-1)*10)+(2*digit_num)+10,(line*16)-19    ,1);
    LcdLine(((start_char+digit_num-1)*10)+(2*digit_num)   ,(line*16)-19,((start_char+digit_num-1)*10)+(2*digit_num)   ,((line*16)-19)+3,1);
    LcdLine(((start_char+digit_num-1)*10)+(2*digit_num)+10,(line*16)-19,((start_char+digit_num-1)*10)+(2*digit_num)+10,((line*16)-19)+3,1);
    */
  }
}

//////////////////////////////////////////////////////////////////////////////////////
void LcdBatt(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t persent)	//рисуем батарейку с заполнением в %
{
  uint32_t horizon_line=0,horizon_line2=0,i=0;
  if(persent>100)persent=100;
  LcdLine(x1,y2,x2,y2,1);  //down
  LcdLine(x2,y1,x2,y2,1);  //right
  LcdLine(x1,y1,x1,y2,1);  //left
  LcdLine(x1,y1,x2,y1,1);  //up
  LcdLine(x1+7,y1-1,x2-7,y1-1,1);
  LcdLine(x1+7,y1-2,x2-7,y1-2,1);
  
  horizon_line=persent*(y2-y1-3)/100;
  for(i=0;i<horizon_line;i++) LcdLine(x1+2,y2-2-i,x2-2,y2-2-i,1);
  
  horizon_line2=(y2-y1-3);
  for(i=horizon_line2;i>horizon_line;i--) LcdLine(x1+2,y2-2-i,x2-2,y2-2-i,0);
}


