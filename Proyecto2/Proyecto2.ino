//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   IE3027: Electrónica Digital 2 - 2019
*/
//***************************************************************************************************************************************
/*
    AUTOR DEL PROYECTO : DEBORA LÓPEZ, CARNÉ - 17065
*/
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <SPI.h>
#include <SD.h>
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
#include "pitches.h"
#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1

File  root, myFile;
String Select;
boolean operacionValida = true; // Sirve para validar la operación ingresada

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
const int buttonPin = PC_4;     // the number of the pushbutton pin
int buttonState = 0;
int c = 0;
//****************************** BUB***********************************
const int buttonPin1 = PA_7;     // the number of the pushbutton pin
const int buttonPin2 = PF_1;     // the number of the pushbutton pin
const int buttonPin3 = PE_2;     // the number of the pushbutton pin
const int buttonPin4 = PE_3;     // the number of the pushbutton pin
const int buttonPin5 = PA_6;     // the number of the pushbutton pin
//****************************** BOB***********************************
const int buttonPin6 = PC_5;     // the number of the pushbutton pin
const int buttonPin7 = PD_7;     // the number of the pushbutton pin
const int buttonPin8 = PC_7;     // the number of the pushbutton pin
const int buttonPin9 = PD_6;     // the number of the pushbutton pin
const int buttonPin10 = PC_6;     // the number of the pushbutton pin
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
int PUNTAJE1 = 0;
int PBUB = 0;
int R = 4;
int y = 190;
int x1 = 18;
int U = 0;
int XBU = 1;
int YBU = 0;
//****************************** BOB***********************************
int PUNTAJE2 = 0;
int x2 = 200;
int R1 = 4;
int y2 = 190;
int U1 = 0;
int PBOB = 0;
int XBO = 1;
int YBO = 0;
//****************************** BENZON **********************************
int PB1 = 0;
int PB2 = 0;
int PB3 = 0;
int RBEN = 2;
int XBEN = 15;
int RBEN1 = 2;
int XBEN1 = 15;
int RBEN2 = 2;
int XBEN2 = 15;
int x30 = 50;
int x31 = 180;
int x32 = 180;
int x40 = 115;
int x41 = 245;
int x42 = 245;
int y30 = 100;
int y31 = 160;
int y32 = 40 ;
int A30 = 0;
int A31 = 0;
int A32 = 0;
//****************************** BASES ***********************************
int x10 = 50;
int x11 = 180;
int x12 = 115;
int x13 = 50;
int x14 = 180;
int x15 = 115;
int x16 = 50;
int x17 = 180;
int y10 = 180;
int y11 = 180;
int y12 = 150;
int y13 = 120;
int y14 = 120;
int y15 = 90;
int y16 = 60;
int y17 = 60;
//****************************** FRUTAS ***********************************
int G1 = 0;
int G2 = 0;
int G3 = 0;
int G4 = 0;
int G5 = 0;
int G6 = 0;
int G7 = 0;
int y20 = 70;
int y21 = 190;
int y22 = 40;
int y23 = 100;
int x20 = 135;
int x21 = 136;
int x22 = 75;
int x23 = 215 ;
//***************************************************************************************************************************************
// TABLERO DE PUNTOS
//***************************************************************************************************************************************
String text1 = "P1";
String text2 = "P2";
String text10 = "BUB WIN";
String text20 = "BOB WIN";
String GO = "";
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
extern uint8_t BUB_DISPARANDO[];
extern uint8_t BUB_BURBUJA[];
extern uint8_t BOB[];
extern uint8_t BOB_SALTANDO[];
extern uint8_t BOB_BAJANDO[];
extern uint8_t BOB_DISPARANDO[];
extern uint8_t BOB_BURBUJA[];
extern uint8_t BENZO[];
extern uint8_t BENZO_BURBUJA[];
extern uint8_t CHERRY[];
extern uint8_t PINA[];
extern uint8_t PERA[];
extern uint8_t MANZANA[];
extern uint8_t PUNTAJE[];
//***************************************************************************************************************************************
// GRAFICOS
//***************************************************************************************************************************************
unsigned char* BBUB = BUB;
unsigned char* BBOB = BOB;
unsigned char* BBENZO = BENZO;
unsigned char* BBENZO1 = BENZO;
unsigned char* BBENZO2 = BENZO;
unsigned char* BMANZANA = MANZANA;
unsigned char* BCHERRY = CHERRY;
unsigned char* BPERA = PERA;
unsigned char* BPINA = PINA;
unsigned char* GL = GOOD_LOCK;

//***************************************************************************************************************************************
// MELODIA
//***************************************************************************************************************************************
// change this to make the song slower or faster
int tempo = 160;
// change this to whichever pin you want to use
int buzzer = 17;
int melody[] = {
  // Keyboard cat
  // Score available at https://musescore.com/user/142788/scores/147371
  NOTE_C4, 4, NOTE_E4, 4, NOTE_G4, 4, NOTE_E4, 4,
  NOTE_C4, 4, NOTE_E4, 8, NOTE_G4, -4, NOTE_E4, 4,
  NOTE_A3, 4, NOTE_C4, 4, NOTE_E4, 4, NOTE_C4, 4,
  NOTE_A3, 4, NOTE_C4, 8, NOTE_E4, -4, NOTE_C4, 4,
  NOTE_G3, 4, NOTE_B3, 4, NOTE_D4, 4, NOTE_B3, 4,
  NOTE_G3, 4, NOTE_B3, 8, NOTE_D4, -4, NOTE_B3, 4,

  NOTE_G3, 4, NOTE_G3, 8, NOTE_G3, -4, NOTE_G3, 8, NOTE_G3, 4,
  NOTE_G3, 4, NOTE_G3, 4, NOTE_G3, 8, NOTE_G3, 4,
  NOTE_C4, 4, NOTE_E4, 4, NOTE_G4, 4, NOTE_E4, 4,
  NOTE_C4, 4, NOTE_E4, 8, NOTE_G4, -4, NOTE_E4, 4,
  NOTE_A3, 4, NOTE_C4, 4, NOTE_E4, 4, NOTE_C4, 4,
  NOTE_A3, 4, NOTE_C4, 8, NOTE_E4, -4, NOTE_C4, 4,
  NOTE_G3, 4, NOTE_B3, 4, NOTE_D4, 4, NOTE_B3, 4,
  NOTE_G3, 4, NOTE_B3, 8, NOTE_D4, -4, NOTE_B3, 4,

  NOTE_C4, 4, NOTE_E4, 4, NOTE_G4, 4, NOTE_E4, 4,
  NOTE_C4, 4, NOTE_E4, 8, NOTE_G4, -4, NOTE_E4, 4,
  NOTE_A3, 4, NOTE_C4, 4, NOTE_E4, 4, NOTE_C4, 4,
  NOTE_A3, 4, NOTE_C4, 8, NOTE_E4, -4, NOTE_C4, 4,
  NOTE_G3, 4, NOTE_B3, 4, NOTE_D4, 4, NOTE_B3, 4,
  NOTE_G3, 4, NOTE_B3, 8, NOTE_D4, -4, NOTE_B3, 4,


  NOTE_G3, 4, NOTE_G3, 8, NOTE_G3, -4, NOTE_G3, 8, NOTE_G3, 4,
  NOTE_G3, 4, NOTE_G3, 4, NOTE_G3, 8, NOTE_G3, 4,
  NOTE_C4, 4, NOTE_E4, 4, NOTE_G4, 4, NOTE_E4, 4,
  NOTE_C4, 4, NOTE_E4, 8, NOTE_G4, -4, NOTE_E4, 4,
  NOTE_A3, 4, NOTE_C4, 4, NOTE_E4, 4, NOTE_C4, 4,
  NOTE_A3, 4, NOTE_C4, 8, NOTE_E4, -4, NOTE_C4, 4,
  NOTE_G3, 4, NOTE_B3, 4, NOTE_D4, 4, NOTE_B3, 4,
  NOTE_G3, 4, NOTE_B3, 8, NOTE_D4, -4, NOTE_B3, 4,
  // NOTE_G3,-1,
};
// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;
int thisNote = 0;
//extern uint8_t BBUB1 [] = {BUB[],BUB_SALTANDO[],BUB_BAJANDO[]};
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

  // Open serial communications and wait for port to open:
  //  Serial.begin(9600);
  //  while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Leonardo only
  //  }
  SPI.setModule(0);

  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  // pinMode(10, OUTPUT);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  //FillRect(0, 0, 319, 206, 0x421b);
  //LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
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
          myFile = SD.open("mario.txt");
          while (myFile.available()) {
            char caracter = myFile.read();
            GO = caracter;
            if (GO == "1") {
              GL = GOOD_LOCK;
            }
            else {
              GL = CHERRY;
            }
          }
          myFile.close();
          LCD_Clear(0x00);
          LCD_Bitmap(55 , 30, 210, 170, GL);
          delay(1000);
          LCD_Clear(0x00);

          break;
        case 1:
          do {
            //*************** MELODIA ***************************
            if (thisNote >= notes * 2) {
              thisNote = 0;
            }
            if (thisNote < notes * 2) {
              thisNote = thisNote + 2;
              // calculates the duration of each note
              divider = melody[thisNote + 1];
              if (divider > 0) {
                // regular note, just proceed
                noteDuration = (wholenote) / divider;
              } else if (divider < 0) {
                // dotted notes are represented with negative durations!!
                noteDuration = (wholenote) / abs(divider);
                noteDuration *= 1.5; // increases the duration in half for dotted notes
              }
              // we only play the note for 90% of the duration, leaving 10% as a pause
              tone(buzzer, melody[thisNote], noteDuration * 0.5);
              // Wait for the specief duration before playing the next note.
              delay(50);
              // stop the waveform generation before the next note.
              noTone(buzzer);
            }
            //******************************************************
            JUEGO();

          } while (G1 != 7);
          if (PUNTAJE1 > PUNTAJE2) {
            LCD_Print(text10, 160, 110, 200, 0xbfc3, 0x0000);
          }
          else {
            LCD_Print(text20, 160, 110, 200, 0x4e7b, 0x0000);
          }
          break;
      }
    }
  }
}
//******************************************************************************************************************************************
void JUEGO(void) {
  String text3 = String(PUNTAJE1);
  String text4 = String(PUNTAJE2);
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
  LCD_Print(text1, 35, 3, 2, 0xbfc3, 0x0000);
  LCD_Print(text3, 70, 5, 1, 0xffff, 0x0000);
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

  LCD_Bitmap(x10, y10, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(x11, y11, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(x12, y12, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(x13, y13, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(x14, y14, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(x15, y15, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(x16, y16, 80, 8, PLATAFORMA_LARGO);
  LCD_Bitmap(x17, y17, 80, 8, PLATAFORMA_LARGO);

  LCD_Bitmap(x20, y20, 13, 17, BMANZANA);
  LCD_Bitmap(x21, y21, 13, 17, BCHERRY);
  LCD_Bitmap(x22, y22, 13, 17, BPINA);
  LCD_Bitmap(x23, y23, 13, 17, BPERA);
  //**********************************************************  BUB  ********************************************************************************
  if (BURBUJA_BUB == HIGH ) {
    Estado5 = 1;
  }
  if (Estado5 == 1 && PBUB == 0 && BURBUJA_BUB == LOW && DERECHA_BUB == LOW && IZQUIERDA_BUB == LOW && SALTANDO_BUB == LOW &&  BAJANDO_BUB == LOW)  { //cuando el pulsador se suelta se
    Estado5 = 0;
    //x1 = x1 + 10;
    R = 1;
    BBUB = BUB_DISPARANDO;
    U = 1;
    XBU = x1;
    YBU = y;
  }
  if (Estado5 == 1 && PBUB == 1 && BURBUJA_BUB == LOW && DERECHA_BUB == LOW && IZQUIERDA_BUB == LOW && SALTANDO_BUB == LOW &&  BAJANDO_BUB == LOW)  { //cuando el pulsador se suelta se
    Estado5 = 0;
    //x1 = x1 + 10;
    R = 1;
    BBUB = BUB_DISPARANDO;
    U = 2;
    XBU = x1;
    YBU = y;
  }
  if (DERECHA_BUB == HIGH && IZQUIERDA_BUB == LOW && x1 < 278) {
    Estado1 = 1;
  }
  if (Estado1 == 1  &&  DERECHA_BUB == LOW && IZQUIERDA_BUB == LOW && SALTANDO_BUB == LOW &&  BAJANDO_BUB == LOW)  { //cuando el pulsador se suelta se
    Estado1 = 0;
    x1 = x1 + 10;
    R = 4;
    BBUB = BUB;
    FillRect(x1 - 10, y, 10, 18, 0x0000);
    PBUB = 0;
  }

  if (IZQUIERDA_BUB == HIGH &&  DERECHA_BUB == LOW && x1 > 18 ) {
    Estado2 = 1;
  }
  if (Estado2 == 1 && IZQUIERDA_BUB == LOW &&  DERECHA_BUB == LOW && SALTANDO_BUB == LOW &&  BAJANDO_BUB == LOW) { //cuando el pulsador se suelta se
    Estado2 = 0;
    x1 = x1 - 10;
    R = 4;
    BBUB = BUB;
    FillRect(x1 + 10, y, 18, 18, 0x0000);
    PBUB = 1;
  }
  if (SALTANDO_BUB == HIGH && BAJANDO_BUB == LOW && y > 40) {
    Estado3 = 1;
  }
  if (Estado3 == 1 &&  SALTANDO_BUB == LOW &&  BAJANDO_BUB == LOW &&  DERECHA_BUB == LOW && IZQUIERDA_BUB == LOW)  { //cuando el pulsador se suelta se
    Estado3 = 0;
    y = y - 30 ;
    R = 2;
    BBUB = BUB_SALTANDO;
    FillRect(x1 - 10, y + 30, 26, 18, 0x0000);
  }
  if (BAJANDO_BUB == HIGH && SALTANDO_BUB == LOW && y < 190) {
    Estado4 = 1;
  }
  if (Estado4 == 1 &&  SALTANDO_BUB == LOW &&  BAJANDO_BUB == LOW &&  DERECHA_BUB == LOW && IZQUIERDA_BUB == LOW)  { //cuando el pulsador se suelta se
    Estado4 = 0;
    y = y + 30 ;
    R = 2;
    BBUB = BUB_BAJANDO;
    FillRect(x1 - 10, y - 30, 26, 18, 0x0000);
  }
  if (( (((x1 < (x10 - 10)) || ((x1 > 118) && (x1 < (x11 - 10))) || (x1 > 248)) && (y == 160)) ||  (((x1 < (x10 - 10)) || ((x1 > 118) && (x1 < (x11 - 10))) || (x1 > 248)) && (y == 100)) || (((x1 < (x10 - 10)) || ((x1 > 118) && (x1 < (x11 - 10))) || (x1 > 248)) && (y == 40)) ||  (((x1 < (x12 - 10)) || (x1 > 183)) && (y == 130)) || (((x1 < (x12 - 10)) || (x1 > 183)) && (y == 70)) ) && (y < 190)  )  { //cuando el pensonaje no este sobre ninguna base
    y = y + 30 ;
    R = 2;
    BBUB = BUB_BAJANDO;
    FillRect(x1 - 10, y - 30, 26, 18, 0x0000);
  }
  //*********************************
  int anim3 = (x1 / 11) % 2;
  LCD_Sprite(x1, y, 18, 18, BBUB, R, anim3, PBUB, 0);
  //**********************************
  if (U == 1) {
    if (XBU != 260 && XBU < 270) {
      XBU++;
      int anim6 = (XBU - 16 / 11) % 2;
      LCD_Sprite(XBU + 18, YBU, 18, 18, BUB_BURBUJA, 1, anim6, PBUB, 0);
      V_line( (XBU + 18) - 1, YBU, 18, 0x0000);
    }
  }
  if (U == 0) {
    FillRect(XBU + 18, YBU, 18, 18, 0x0000);
  }
  if (U == 2) {
    if (XBU != 33 && XBU > 18) {
      XBU--;
      int anim6 = (XBU - 16 / 11) % 2;
      LCD_Sprite(XBU - 18, YBU, 18, 18, BUB_BURBUJA, 1, anim6, PBUB, 0);
      V_line( (XBU - 18) + 18, YBU, 18, 0x0000);
    }
  }
  if (U == 3)  {
    FillRect(XBU - 18, YBU, 18, 18, 0x0000);
  }
  //**********************************************************  BOB  ********************************************************************************
  if (BURBUJA_BOB == HIGH ) {
    Estado10 = 1;
  }
  if (Estado10 == 1 && PBOB == 0 && BURBUJA_BOB == LOW && DERECHA_BOB == LOW && IZQUIERDA_BOB == LOW && SALTANDO_BOB == LOW &&  BAJANDO_BOB == LOW)  { //cuando el pulsador se suelta se
    Estado10 = 0;
    //x1 = x1 + 10;
    R1 = 1;
    BBOB = BOB_DISPARANDO;
    U1 = 1;
    XBO = x2;
    YBO = y2;
  }
  if (Estado10 == 1 && PBOB == 1 && BURBUJA_BOB == LOW && DERECHA_BOB == LOW && IZQUIERDA_BOB == LOW && SALTANDO_BOB == LOW &&  BAJANDO_BOB == LOW)  { //cuando el pulsador se suelta se
    Estado10 = 0;
    //x1 = x1 + 10;
    R1 = 1;
    BBOB = BOB_DISPARANDO;
    U1 = 2;
    XBO = x2;
    YBO = y2;
  }
  if (DERECHA_BOB == HIGH && IZQUIERDA_BOB == LOW && x2 < 278) {
    Estado6 = 1;
  }
  if (Estado6 == 1  &&  DERECHA_BOB == LOW && IZQUIERDA_BOB == LOW && SALTANDO_BOB == LOW &&  BAJANDO_BOB == LOW)  { //cuando el pulsador se suelta se
    Estado6 = 0;
    x2 = x2 + 10;
    R1 = 4;
    BBOB = BOB;
    FillRect(x2 - 10, y2, 10, 18, 0x0000);
    PBOB = 0;
  }

  if (IZQUIERDA_BOB == HIGH &&  DERECHA_BOB == LOW && x2 > 18 ) {
    Estado7 = 1;
  }
  if (Estado7 == 1 && IZQUIERDA_BOB == LOW &&  DERECHA_BOB == LOW && SALTANDO_BOB == LOW &&  BAJANDO_BOB == LOW) { //cuando el pulsador se suelta se
    Estado7 = 0;
    x2 = x2 - 10;
    R1 = 4;
    BBOB = BOB;
    FillRect(x2 + 10, y2, 18, 18, 0x0000);
    PBOB = 1;
  }
  if (SALTANDO_BOB == HIGH && BAJANDO_BOB == LOW && y2 > 40) {
    Estado8 = 1;
  }
  if (Estado8 == 1 &&  SALTANDO_BOB == LOW &&  BAJANDO_BOB == LOW &&  DERECHA_BOB == LOW && IZQUIERDA_BOB == LOW)  { //cuando el pulsador se suelta se
    Estado8 = 0;
    y2 = y2 - 30 ;
    R1 = 2;
    BBOB = BOB_SALTANDO;
    FillRect(x2 - 10, y2 + 30, 26, 18, 0x0000);
  }
  if (BAJANDO_BOB == HIGH && SALTANDO_BOB == LOW && y2 < 190) {
    Estado9 = 1;
  }
  if (Estado9 == 1 &&  SALTANDO_BOB == LOW &&  BAJANDO_BOB == LOW &&  DERECHA_BOB == LOW && IZQUIERDA_BOB == LOW)  { //cuando el pulsador se suelta se
    Estado9 = 0;
    y2 = y2 + 30 ;
    R1 = 2;
    BBOB = BOB_BAJANDO;
    FillRect(x2 - 10, y2 - 30, 26, 18, 0x0000);
  }
  if (( (((x2 < (x10 - 10)) || ((x2 > 118) && (x2 < (x11 - 10))) || (x2 > 248)) && (y2 == 160)) ||  (((x2 < (x10 - 10)) || ((x2 > 118) && (x2 < (x11 - 10))) || (x2 > 248)) && (y2 == 100)) || (((x2 < (x10 - 10)) || ((x2 > 118) && (x2 < (x11 - 10))) || (x2 > 248)) && (y2 == 40)) ||  (((x2 < (x12 - 10)) || (x2 > 183)) && (y2 == 130)) || (((x2 < (x12 - 10)) || (x2 > 183)) && (y2 == 70)) ) && (y2 < 190)  )  { //cuando el pensonaje no este sobre ninguna base
    y2 = y2 + 30 ;
    R1 = 2;
    BBOB = BOB_BAJANDO;
    FillRect(x2 - 10, y2 - 30, 26, 18, 0x0000);
  }
  //*********************************
  int anim8 = (x2 / 11) % 2;
  LCD_Sprite(x2, y2, 18, 18, BBOB, R1, anim8, PBOB, 0);
  //**********************************
  if (U1 == 1) {
    if (XBO != 260 && XBO < 270) {
      XBO++;
      int anim8 = (XBO - 16 / 11) % 2;
      LCD_Sprite(XBO + 18, YBO, 18, 18, BOB_BURBUJA, 1, anim8, PBOB, 0);
      V_line( (XBO + 18) - 1, YBO, 18, 0x0000);
    }
  }
  if (U1 == 0) {
    FillRect(XBO + 18, YBO, 18, 18, 0x0000);
  }
  if (U1 == 2) {
    if (XBO != 33 && XBO > 18) {
      XBO--;
      int anim8 = (XBO - 16 / 11) % 2;
      LCD_Sprite(XBO - 18, YBO, 18, 18, BOB_BURBUJA, 1, anim8, PBOB, 0);
      V_line( (XBO - 18) + 18, YBO, 18, 0x0000);
    }
  }
  if (U1 == 3)  {
    FillRect(XBO - 18, YBO, 18, 18, 0x0000);
  }
  //******************************************************************************************************************************************
  // BENZON (VILLANOS)
  //******************************************************************************************************************************************
  if (x30 == 50 ) {
    A30 = 0;
  }
  if (x31 == 180 ) {
    A31 = 0;
  }
  if (x32 == 180 ) {
    A32 = 0;
  }
  //****************************************************************************************
  if (x30 == 115 ) {
    x40 = 115;
    x30 = 330;
    A30 = 1;
  }
  if (x31 == 245 ) {
    x41 = 245;
    x31 = 330;
    A31 = 1;
  }
  if (x32 == 245 ) {
    x42 = 245;
    x32 = 330;
    A32 = 1;
  }
  //**********************************************************************************************
  if (x40 == 50 ) {
    x30 = 50;
    x40 = 330;
  }
  if (x41 == 180 ) {
    x31 = 180;
    x41 = 330;
  }
  if (x42 == 180 ) {
    x32 = 180;
    x42 = 330;
  }
  //******************************************* DERECHA ********************************************
  if (x30 != 115 && A30 == 0 && PB1 == 0) {
    x30++;
    int anim2 = (x30 - 16 / 11) % 2;
    LCD_Sprite(x30, y30, XBEN, 18, BBENZO, RBEN, anim2, 0, 0);
    V_line( x30 - 1, y30, 18, 0x0000);
  }

  if (x31 != 245 && A31 == 0 && PB2 == 0) {
    x31++;
    int anim2 = (x31 - 16 / 11) % 2;
    LCD_Sprite(x31, y31, XBEN1, 18, BBENZO1, RBEN1, anim2, 0, 0);
    V_line( x31 - 1, y31, 18, 0x0000);
  }
  if (x32 != 245 && A32 == 0 && PB3 == 0) {
    x32++;
    int anim2 = (x32 - 16 / 11) % 2;
    LCD_Sprite(x32, y32, XBEN2, 18, BBENZO2, RBEN2, anim2, 0, 0);
    V_line( x32 - 1, y32, 18, 0x0000);
  }
  //******************************************* IZQUIERDA ********************************************
  if (x40 != 50 && A30 == 1 && PB1 == 0) {
    x40--;
    int anim2 = (x40 - 16 / 11) % 2;
    LCD_Sprite(x40, y30, XBEN, 18, BBENZO, RBEN, anim2, 1, 0);
    V_line( x40 + 15, y30, 18, 0x0000);
  }
  if (x41 != 180 && A31 == 1 && PB2 == 0) {
    x41--;
    int anim2 = (x41 - 16 / 11) % 2;
    LCD_Sprite(x41, y31, XBEN1, 18, BBENZO1, RBEN1, anim2, 1, 0);
    V_line( x41 + 15, y31, 18, 0x0000);
  }
  if (x42 != 180 && A32 == 1 && PB3 == 0) {
    x42--;
    int anim2 = (x42 - 16 / 11) % 2;
    LCD_Sprite(x42, y32, XBEN2, 18, BBENZO2, RBEN2, anim2, 1, 0);
    V_line( x42 + 15, y32, 18, 0x0000);
  }
  //******************************************************************************************************************************************
  // ***************************************** BUB ********************************************************
  if (BBENZO == PUNTAJE) {
    y30 = 220;
    G1 = G1 + 1;
    FillRect(x40, y30, XBEN, 18, 0x0000);
    FillRect(x30, y30, XBEN, 18, 0x0000);
    PB1 = 1;
  }
  if (BBENZO1 == PUNTAJE) {
    G1 = G1 + 1;
    y31 = 220;
    FillRect(x41, y31, XBEN1, 18, 0x0000);
    FillRect(x31, y31, XBEN1, 18, 0x0000);
    PB2 = 1;
  }
  if (BBENZO2 == PUNTAJE) {
    y32 = 220;
    G1 = G1 + 1;
    FillRect(x42, y32, XBEN2, 18, 0x0000);
    FillRect(x32, y32, XBEN2, 18, 0x0000);
    PB3 = 1;
  }
  //**************************************************************
  if (BMANZANA == PUNTAJE) {
    FillRect(x20, y20, 13, 17, 0x0000);
    G1 = G1 + 1;
    y20 = 220;
    x20 = 330;
  }
  if (BCHERRY == PUNTAJE) {
    FillRect(x21, y21, 13, 17, 0x0000);
    G1 = G1 + 1;
    y21 = 220;
    x21 = 330;
  }
  if (BPINA == PUNTAJE) {
    G1 = G1 + 1;
    FillRect(x22, y22, 13, 17, 0x0000);
    y22 = 220;
    x22 = 330;
  }
  if (BPERA == PUNTAJE) {
    G1 = G1 + 1;
    FillRect(x23, y23, 13, 17, 0x0000);
    y23 = 220;
    x23 = 330;
  }
  //**************************************************************
  if ((XBU + 36 == x30 || XBU + 36 == x40) && YBU == y30 ) {
    BBENZO = BENZO_BURBUJA;
    RBEN = 1;
    XBEN = 18;
    U = 0;
  }
  if ((XBU + 36 == x31 || XBU + 36 == x41) && YBU == y31 ) {
    BBENZO1 = BENZO_BURBUJA;
    RBEN1 = 1;
    XBEN1 = 18;
    U = 0;
  }
  if ((XBU + 36 == x32 || XBU + 36 == x42) && YBU == y32) {
    BBENZO2 = BENZO_BURBUJA;
    RBEN2 = 1;
    XBEN2 = 18;
    U = 0;
  }
  //********************************************************************
  if ((x1 + 18 == x30 || x1 + 18 == x40) && BBENZO == BENZO_BURBUJA ) {
    BBENZO = PUNTAJE;
    PUNTAJE1 = PUNTAJE1 + 100;
    RBEN = 1;
    XBEN = 18;
    YBU = 220;
  }
  if ((x1 + 18 == x31 || x1 + 18 == x41) && BBENZO1 == BENZO_BURBUJA ) {
    BBENZO1 = PUNTAJE;
    PUNTAJE1 = PUNTAJE1 + 100;
    RBEN1 = 1;
    XBEN1 = 18;
    YBU = 220;
  }
  if ((x1 + 18 == x32 || x1 + 18 == x42) && BBENZO2 == BENZO_BURBUJA ) {
    BBENZO2 = PUNTAJE;
    PUNTAJE1 = PUNTAJE1 + 100;
    RBEN2 = 1;
    XBEN2 = 18;
    YBU = 220;
  }
  // ***************************************** BOB ********************************************************
  //**************************************************************
  if ((XBO + 36 == x30 || XBO + 36 == x40) && YBO == y30 ) {
    BBENZO = BENZO_BURBUJA;
    RBEN = 1;
    XBEN = 18;
    U1 = 0;
  }
  if ((XBO + 36 == x31 || XBO + 36 == x41) && YBO == y31 ) {
    BBENZO1 = BENZO_BURBUJA;
    RBEN1 = 1;
    XBEN1 = 18;
    U1 = 0;
  }
  if ((XBO + 36 == x32 || XBO + 36 == x42) && YBO == y32) {
    BBENZO2 = BENZO_BURBUJA;
    RBEN2 = 1;
    XBEN2 = 18;
    U1 = 0;
  }
  //********************************************************************
  if ((x2 + 18 == x30 || x2 + 18 == x40) && BBENZO == BENZO_BURBUJA ) {
    BBENZO = PUNTAJE;
    PUNTAJE2 = PUNTAJE2 + 100;
    RBEN = 1;
    XBEN = 18;
    YBO = 220;
  }
  if ((x2 + 18 == x31 || x2 + 18 == x41) && BBENZO1 == BENZO_BURBUJA ) {
    BBENZO1 = PUNTAJE;
    PUNTAJE2 = PUNTAJE2 + 100;
    RBEN1 = 1;
    XBEN1 = 18;
    YBO = 220;
  }
  if ((x2 + 18 == x32 || x2 + 18 == x42) && BBENZO2 == BENZO_BURBUJA ) {
    BBENZO2 = PUNTAJE;
    PUNTAJE2 = PUNTAJE2 + 100;
    RBEN2 = 1;
    XBEN2 = 18;
    YBO = 220;
  }
  //******************* FRUTAS *********************************************
  //*********************** BUB *****************************
  if ( (x1 + 18 == x20 || x1 == x20 + 13 ) && y == y20) {
    BMANZANA = PUNTAJE;
    PUNTAJE1 = PUNTAJE1 + 50;
  }
  if (( x1 + 18 == x21 || x1 == x21 + 13) && y == y21 ) {
    BCHERRY = PUNTAJE;
    PUNTAJE1 = PUNTAJE1 + 50;
  }
  if ( (x1 + 18 == x22 || x1 == x22 + 13) && y == y22 ) {
    BPINA = PUNTAJE;
    PUNTAJE1 = PUNTAJE1 + 50;
  }
  if ( (x1 + 18 == x23 || x1 == x23 + 13) && y == y23 ) {
    BPERA = PUNTAJE;
    PUNTAJE1 = PUNTAJE1 + 50;
  }
  //*********************** BOB *****************************
  if ( (x2 + 18 == x20 || x2 == x20 + 13 ) && y2 == y20) {
    BMANZANA = PUNTAJE;
    PUNTAJE2 = PUNTAJE2 + 50;
  }
  if (( x2 + 18 == x21 || x2 == x21 + 13) && y2 == y21 ) {
    BCHERRY = PUNTAJE;
    PUNTAJE2 = PUNTAJE2 + 50;
  }
  if ( (x2 + 18 == x22 || x2 == x22 + 13) && y2 == y22 ) {
    BPINA = PUNTAJE;
    PUNTAJE2 = PUNTAJE2 + 50;
  }
  if ( (x2 + 18 == x23 || x2 == x23 + 13) && y2 == y23 ) {
    BPERA = PUNTAJE;
    PUNTAJE2 = PUNTAJE2 + 50;
  }
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
