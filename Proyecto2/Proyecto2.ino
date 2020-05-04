//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   IE3027: Electrónica Digital 2 - 2019
*/
//***************************************************************************************************************************************
//AUTOR PROYECTO : DEBORA LÓPEZ, CARNÉ - 17065
#include <stdint.h>
#include <stdbool.h>
//#include <lcd_registers.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void JUEGO(void);
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

//***************************************************************************************************************************************
// DEFINIENDO BOTONES
//***************************************************************************************************************************************
const int buttonPin = PUSH2;     // the number of the pushbutton pin
int buttonState = 0;
int c = 0;
//****************************** BUB***********************************
const int buttonPin1 = PA_7;     // the number of the pushbutton pin
const int buttonPin2 = PF_1;     // the number of the pushbutton pin
const int buttonPin3 = PE_2;     // the number of the pushbutton pin
const int buttonPin4 = PE_3;     // the number of the pushbutton pin
const int buttonPin5 = PA_6;     // the number of the pushbutton pin
//****************************** BOB***********************************
const int buttonPin6 = PA_7;     // the number of the pushbutton pin
const int buttonPin7 = PF_1;     // the number of the pushbutton pin
const int buttonPin8 = PE_2;     // the number of the pushbutton pin
const int buttonPin9 = PE_3;     // the number of the pushbutton pin
const int buttonPin10 = PA_6;     // the number of the pushbutton pin
//****************************** BUB***********************************
int DERECHA_BUB = 0;
int IZQUIERDA_BUB = 0;
int SALTANDO_BUB = 0;
int BAJANDO_BUB = 0;
int BURBUJA_BUB = 0;
//****************************** BOB***********************************
int DERECHA_BOB = 0;
int IZQUIERDA_BOB = 0;
int SALTANDO_BOB = 0;
int BAJANDO_BOB = 0;
int BURBUJA_BOB = 0;
//***************************************************************************************************************************************
// DEFINIENDO POSICIONES
//***************************************************************************************************************************************
//****************************** BUB***********************************
int PBUB = 0;
int y = 190;
int x1 = 18;
//****************************** BOB***********************************
int x2 = 18;
int y2 = 190;
int PBUB1 = 0;
//****************************** BENZON ***********************************
int x = 180;
int x3 = 245;
int A = 0;
//***************************************************************************************************************************************
// TABLERO DE PUNTOS
//***************************************************************************************************************************************
String text1 = "P1";
String text3 = "0000";
String text2 = "P2";
String text4 = "0000";
//***************************************************************************************************************************************
// ESTADOS
//***************************************************************************************************************************************
int Estado1 = 0;
int Estado2 = 0;
int Estado3 = 0;
int Estado4 = 0;
int Estado5 = 0;
int Estado6 = 0;
int Estado7 = 0;
int Estado8 = 0;
int Estado9 = 0;
int Estado10 = 0;
int Estado11 = 0;
int Estado12 = 0;
//***************************************************************************************************************************************
// GRAFICOS
//***************************************************************************************************************************************
extern uint8_t INTRO_ESTRELLA[];
extern uint8_t GOOD_LOCK[];
extern uint8_t PLATAFORMA_ALTO[];
extern uint8_t PLATAFORMA_LARGO[];
extern uint8_t BUB[];
extern uint8_t BUB_SALTANDO[];
extern uint8_t BUB_BAJANDO[];
extern uint8_t BOB[];
extern uint8_t BOB_SALTANDO[];
extern uint8_t BOB_BAJANDO[];
extern uint8_t BENZO[];
extern uint8_t CHERRY[];
extern uint8_t PINA[];
extern uint8_t PERA[];
extern uint8_t MANZANA[];
//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);
  pinMode(buttonPin5, INPUT_PULLUP);
  pinMode(buttonPin6, INPUT_PULLUP);
  pinMode(buttonPin7, INPUT_PULLUP);
  pinMode(buttonPin8, INPUT_PULLUP);
  pinMode(buttonPin9, INPUT_PULLUP);
  pinMode(buttonPin10, INPUT_PULLUP);
  //FillRect(0, 0, 319, 206, 0x421b);
  //LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    c = 1;
  }
  if (c == 0) {
    LCD_Bitmap(73, 30, 160, 180, INTRO_ESTRELLA);
  }
  if (c == 1) {
    unsigned char f = 0;
    for (f = 0; f < 2; f++) {
      switch (f) { // SEMAFORO
        case 0:
          LCD_Clear(0x00);
          LCD_Bitmap(55 , 30, 210, 170, GOOD_LOCK);
          delay(1000);
          LCD_Clear(0x00);
          break;
        case 1:
          do {
            JUEGO();
          } while ((c == 1) && (c == 1));
          break;
      }
    }
  }
}
//******************************************************************************************************************************************
void JUEGO(void) {
  DERECHA_BUB = digitalRead(buttonPin1);
  IZQUIERDA_BUB = digitalRead(buttonPin2);
  SALTANDO_BUB = digitalRead(buttonPin3);
  BAJANDO_BUB = digitalRead(buttonPin4);
  BURBUJA_BUB = digitalRead(buttonPin5);
  DERECHA_BOB = digitalRead(buttonPin6);
  IZQUIERDA_BOB = digitalRead(buttonPin7);
  SALTANDO_BOB = digitalRead(buttonPin8);
  BAJANDO_BOB = digitalRead(buttonPin9);
  BURBUJA_BOB = digitalRead(buttonPin10);
  //***************************************************************************************************************************************
  LCD_Print(text1, 25, 3, 2, 0xbfc3, 0x0000);
  LCD_Print(text3, 65, 5, 1, 0xffff, 0x0000);
  LCD_Print(text2, 220, 3, 2, 0x4e7b, 0x0000);
  LCD_Print(text4, 260, 5, 1, 0xffff, 0x0000);
  //***************************************************************************************************************************************
  // PLATAFORMA DE JUEGO
  //***************************************************************************************************************************************
  //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
  LCD_Bitmap(0, 20, 16, 160, PLATAFORMA_ALTO);
  LCD_Bitmap(0, 160, 16, 56, PLATAFORMA_ALTO);
  LCD_Bitmap(303, 20, 16, 160, PLATAFORMA_ALTO);
  LCD_Bitmap(303, 160, 16, 56, PLATAFORMA_ALTO);

  LCD_Bitmap(16, 20, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(120, 20, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(223, 20, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(16, 208, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(96, 208, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(176, 208, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(223, 208, 80, 8, PLATAFORMA_LARGO);

  LCD_Bitmap(50, 180, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(180, 180, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(115, 150, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(50, 120, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(180, 120, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(115, 90, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(50, 60, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(180, 60, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(135, 72, 13, 17, CHERRY);
  LCD_Bitmap(135, 190, 13, 17, CHERRY);
  LCD_Bitmap(80, 162, 13, 17, PINA);
  LCD_Bitmap(195, 102, 13, 17, PERA);
  LCD_Bitmap(80, 42, 13, 17, MANZANA);
  //******************************************************************************************************************************************
  if (DERECHA_BUB == HIGH && IZQUIERDA_BUB == LOW) {
    Estado1 = 1;
  }
  if (Estado1 == 1 &&  DERECHA_BUB == LOW && IZQUIERDA_BUB == LOW)  { //cuando el pulsador se suelta se
    Estado1 = 0;
    x1 = x1 + 10;
    FillRect(x1 - 10, y, 10, 18, 0x0000);
    PBUB = 0;
  }

  if (IZQUIERDA_BUB == HIGH &&  DERECHA_BUB == LOW ) {
    Estado2 = 1;
  }
  if (Estado2 == 1 && IZQUIERDA_BUB == LOW &&  DERECHA_BUB == LOW) { //cuando el pulsador se suelta se
    Estado2 = 0;
    x1 = x1 - 10;
    FillRect(x1 + 10, y, 18, 18, 0x0000);
    PBUB = 1;
  }
  if (SALTANDO_BUB == HIGH && BAJANDO_BUB == LOW) {
    Estado3 = 1;
  }
  if (Estado3 == 1 &&  SALTANDO_BUB == LOW &&  BAJANDO_BUB == LOW)  { //cuando el pulsador se suelta se
    Estado3 = 0;
    y = y - 30 ;
    FillRect(x1 - 10, y + 30, 26, 18, 0x0000);
  }
  if (BAJANDO_BUB == HIGH && SALTANDO_BUB == LOW) {
    Estado4 = 1;
  }
  if (Estado4 == 1 &&  SALTANDO_BUB == LOW &&  BAJANDO_BUB == LOW)  { //cuando el pulsador se suelta se
    Estado4 = 0;
    y = y + 30 ;
    FillRect(x1 - 10, y - 30, 26, 18, 0x0000);
  }
  int anim3 = (x1 / 11) % 2;
  LCD_Sprite(x1, y, 18, 18, BUB, 4, anim3, PBUB, 0);

  //******************************************************************************************************************************************
   // BENZON (VILLANOS)
  //***************************************************************************************************************************************
  if (x == 180 ) {
    A = 0;
  }
  if (x == 245 ) {
    x3 = 245;
    x = 0;
    A = 1;
  }
  if (x3 == 180 ) {
    x = 180;
    x3 = 0;
  }
  if (x != 245 && A == 0) {
    x++;
    int anim2 = (x - 16 / 11) % 2;
    LCD_Sprite(x, 41, 15, 18, BENZO, 2, anim2, 0, 0);
    V_line( x - 1, 41, 18, 0x0000);
    LCD_Sprite(x, 161, 15, 18, BENZO, 2, anim2, 0, 0);
    V_line( x - 1, 161, 18, 0x0000);
    x = x - 130;
    LCD_Sprite(x, 101, 15, 18, BENZO, 2, anim2, 0, 0);
    V_line( x - 1, 101, 18, 0x0000);
    x = x + 130;

  }
  if (x3 != 180 && A == 1 ) {
    x3--;
    int anim4 = (x3 - 16 / 11) % 2;
    LCD_Sprite(x3, 41, 15, 18, BENZO, 2, anim4, 1, 0);
    V_line( x3 + 15, 41, 18, 0x0000);
    LCD_Sprite(x3, 161, 15, 18, BENZO, 2, anim4, 1, 0);
    V_line( x3 + 15, 161, 18, 0x0000);
    x3 = x3 - 130;
    LCD_Sprite(x3, 101, 15, 18, BENZO, 2, anim4, 1, 0);
    V_line( x3 + 15, 101, 18, 0x0000);
    x3 = x3 + 130;
  }

  //******************************************************************************************************************************************

}

//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
  }
*/

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + w;
  y2 = y + h;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = w * h * 2 - 1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);

      //LCD_DATA(bitmap[k]);
      k = k - 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}
