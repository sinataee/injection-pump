/*
   siring pump code
   10/12/2021
   sina ataei
*/
/******************** libraries*********************/
#include <TimerOne.h>
#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
#include <TouchScreen.h>
#include <EEPROM.h>
/********************definition*********************/
//drive pin
#define Ppul 12
#define Pdir 11
//tft pin
#define CS   A3
#define RS   A2
#define WR   A1
#define RD   A0
#define RST  A4
//touch screen pin
#define XP 8
#define XM A2
#define YP A3
#define YM 9
//size of touch pressure
#define MINPRESSURE 100
#define MAXPRESSURE 800
//size of tft lcd
#define TS_LEFT 886
#define TS_RT   80
#define TS_TOP  80
#define TS_BOT  840
//Orientation of screen
#define Orientation 3
//some color
#define BLACK 0x0000
#define NAVY 0x000F
//#define DARKGREEN 0x03E0
//#define DARKCYAN 0x03EF
//#define MAROON 0x7800
//#define PURPLE 0x780F
#define OLIVE 0x7BE0
//#define LIGHTGREY 0xC618
//#define DARKGREY 0x7BEF
#define BLUE 0x001F
#define GREEN 0x07E0
//#define CYAN 0x07FF
#define RED 0xF800
//#define MAGENTA 0xF81F
//#define YELLOW 0xFFE0
#define WHITE 0xFFFF
//#define ORANGE 0xFD20
//#define GREENYELLOW 0xAFE5
//#define PINK 0xF81F
/********************tftlcd obj***************************/
MCUFRIEND_kbv tft(CS, RS, WR, RD, RST); //CS=A3,RS=A2,WR=A1,RD=A0,RST=A4
/********************TouchScreen obj*********************/
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //set up size of touch screen 
TSPoint tp; // making variabel for orientation of touch point
/********************variabel***********************/
int16_t h = tft.height() / 4, w = tft.width() / 3 ; //dividing height and width for generate first screen
float volume_initial = 60 ;
float pulstim ;// this variable for save time to generate pulse 
uint16_t xpos, ypos , zpos; // position for getting touch screen 
byte key = 1 ; //
float Q = 1; //
float area = 0 , v = 0;
unsigned long n = 0;
int num[20];
boolean p = 1;
float s_one = 6.6051985542 , s_two  , V_one = 60, V_two  ;// area and volum variables
int16_t wo = tft.width() / 3, ho = tft.height() / 5; //dividing height and width for generate key screen
/*********************expos fuction***********************/
//this function for setting device to right position
void expos (int pos) {
  if (pos == -1) { //left run on
    digitalWrite (Pdir , HIGH);
    for (int i = 0; i < 640; i++) {
      digitalWrite (Ppul , HIGH);
      delayMicroseconds(80);
      digitalWrite (Ppul , LOW);
      delayMicroseconds(80);
    }
  }
  else if (pos == -2) {//right run on
    digitalWrite (Pdir , LOW);
    for (int i = 0; i < 640; i++) {
      digitalWrite (Ppul , HIGH);
      delayMicroseconds(80);
      digitalWrite (Ppul , LOW);
      delayMicroseconds(80);
    }
  }
  else if (pos == -3) {//MIDDEL
    digitalWrite (Pdir , LOW);
    for (long i = 0; i < 100000; i++) {
      digitalWrite (Ppul , HIGH);
      delayMicroseconds(10);
      digitalWrite (Ppul , LOW);
      delayMicroseconds(10);
    }
  }
}
/**************************restfunc*******************************/
//this function for reset micro
void (* restfunc)(void) = 0;

/*********************screenexpos fuction***********************/
//this function for showing expos screen
void screenexpos() {
  tft.fillScreen(0x05AF);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.fillRect(0.2 * h, 0.2 * w, h, w, WHITE);
  tft.setCursor(h * 0.4, w * 0.6);
  tft.print(F("<="));
  tft.fillRect(0.2 * h, 1.6 * w, h, w, WHITE);
  tft.setCursor(h * 0.5, w * 2);
  tft.print(F("Mid"));
  tft.fillRect(2.6 * h, 0.2 * w, h, w, WHITE);
  tft.setCursor(h * 3, w * 0.6);
  tft.print(F("=>"));
  tft.fillRect(2.6 * h, 1.6 * w, h, w, WHITE);
  tft.setCursor(h * 2.7, w * 2);
  tft.print(F("Start"));
}

/*********************Texpos fuction***********************/
//this function for expos touch screen
int Texpos(uint16_t x , uint16_t y) {
  if (0.2 * h < x && x < 1.2 * h) {
    if (0.2 * w < y && y < 1.2 * w) return -1; //left
    if (1.6 * w < y && y < 2.6 * w) return -3; //mid
  }

  if (2.6 * h < x && x < 3.6 * h) {
    if (0.2 * w < y && y < 1.2 * w) return -2; //right
    if (1.6 * w < y && y < 2.6 * w) return -4; //start
  }
  return 0;
}
/*********************siring function **********************/
//this function for setting intrupt to run step motor
void siring() {
  Timer1.stop();
  Timer1.detachInterrupt();
  delay(500);
  digitalWrite (Pdir, HIGH);
  pulstim = ((1875.0 * area) / (2 * Q)) ;
  Timer1.initialize(pulstim);
  Timer1.attachInterrupt(TimerInt);
  Timer1.start();
}
/********************timerInt function********************/
// timer intrupt function
void TimerInt (void) {
  digitalWrite(Ppul, p);
  p = !p;
  n += 1;
}
/********************keyboard function*********************/
//this function for showing key pad screen
void showkeyboardscreen ()
{
  tft.fillScreen(0x05AF);
  tft.fillRect(0, 0, 4 * wo, ho, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.drawRect(0, ho, wo, ho, NAVY);
  tft.setCursor(wo * 0.4, ho * 1.33);
  tft.print(F("1"));
  tft.drawRect(wo, ho, wo, ho, NAVY);
  tft.setCursor(wo * 1.4, ho * 1.33);
  tft.print(F("2"));
  tft.drawRect(2 * wo, ho, wo, ho, NAVY);
  tft.setCursor(wo * 2.4, ho * 1.33);
  tft.print(F("3"));
  tft.drawRect(0, 2 * ho, wo, ho, NAVY);
  tft.setCursor(wo * 0.4, ho * 2.33);
  tft.print(F("4"));
  tft.drawRect(wo, 2 * ho, wo, ho, NAVY);
  tft.setCursor(wo * 1.4, ho * 2.33);
  tft.print(F("5"));
  tft.drawRect(2 * wo, 2 * ho, wo, ho, NAVY);
  tft.setCursor(wo * 2.4, ho * 2.33);
  tft.print(F("6"));
  tft.drawRect(0, 3 * ho, wo, ho, NAVY);
  tft.setCursor(wo * 0.4, ho * 3.33);
  tft.print(F("7"));
  tft.drawRect(wo, 3 * ho, wo, ho, NAVY);
  tft.setCursor(wo * 1.4, ho * 3.33);
  tft.print(F("8"));
  tft.drawRect(2 * wo, 3 * ho, wo, ho, NAVY);
  tft.setCursor(wo * 2.4, ho * 3.33);
  tft.print(F("9"));
  tft.drawRect(3 * wo, ho, wo, ho, NAVY);
  tft.setCursor(wo * 3.33, ho * 1.33);
  tft.print(F("0"));
  tft.drawRect(3 * wo, 2 * ho, wo, ho, NAVY);
  tft.setCursor(wo * 3.2, ho * 2.33);
  tft.setTextColor(GREEN);
  tft.print(F("OK"));
  tft.drawRect(3 * wo, 3 * ho, wo, ho, NAVY);
  tft.setCursor(wo * 3.05, ho * 3.3);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.print(F("CANCEL"));
  delay(200);

}
/********************decod TOUCH function*********************/
//this function for decoding key pad screen
int decodekeyboard (uint16_t x , uint16_t y)
{
  if (0 < x && x < wo) {
    if (ho < y && y < 2 * ho)              return 1;
    else if (2 * ho < y && y < 2.8 * ho)     return 4;
    else if (2.8 * ho < y && y < 4 * ho)     return 7;
  }
  else if (wo < x && x < 2 * wo) {
    if (ho < y && y < 2 * ho)              return 2;
    else if (2 * ho < y && y < 2.8 * ho)     return 5;
    else if (2.8 * ho < y && y < 4 * ho)     return 8;
  }
  else if (2 * wo < x && x < 3 * wo) {
    if (ho < y && y < 2 * ho)              return 3;
    else if (2 * ho < y && y < 3 * ho)     return 6;
    else if (3 * ho < y && y < 4 * ho)     return 9;
  }
  else if (3 * wo < x && x < 4 * wo) {
    if (ho < y && y < 2 * ho)              return -3; //0
    else if (2 * ho < y && y < 3 * ho)     return -1; //ok
    else if (3 * ho < y && y < 4 * ho)     return -2;//cancel
  }
  return 0;
}
/********************showfirstscreen function*********************/
//this function for showing first screen
void showfirstscreen ()
{
  tft.fillScreen(OLIVE);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(0.3 * w, 0.5 * h);
  tft.print(F("Q "));
  tft.fillRect(0.7 * w, h * 0.3, w * 2.7, h * 0.8 , WHITE);
  tft.setCursor(0.3 * w, h * 2);
  tft.print(F("A "));
  tft.fillRect(0.7 * w, h * 1.7, w * 2.7, 0.8 * h, WHITE);
  tft.fillRect(2*w, h * 2.6, 2 * w, 0.4 * h, RED); //back
  tft.setCursor(2.4 * w, h * 2.68);
  tft.print(F("====>"));
}
/********************showvolumescreen function*********************/
//this function for showing final screen
void showvolumescreen (void)
{

  tft.fillScreen(OLIVE);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(w, h * 0.5);
  tft.print(F("Q -> "));
  tft.print(Q);
  tft.fillRect(0.3 * w, h, 1.7 * w, h * 0.65, BLACK);
  tft.fillRect(2 * w, h, 1.7 * w, h * 0.65, WHITE);
}

/********************power_ten function*********************/
//this function for calculating number that another number power ten
long power_ten (int i) {
  long b = 1;
  if ( i == 0) return b;
  else
    for ( int j = 1 ; j < i + 1 ; j++) b *= 10;
  return b;
}

/********************convert function*********************/
//this function for decod number to integer
long convert (int *num)
{
  long val = 0 , b ;
  int I;
  for (int i = 0; i < 20; i++)  if (*(num + i) == -1) {
      I = i;
      break;
    }
  for (int i = 0; i < I; i++) {
    if (*(num + i) == -3) *(num + i) = 0;
    b = power_ten(I - 1 - i);
    val += *(num + i) * b ;
  }
  return val ;
}

/********************touchseting function*********************/
//this function for calculating position of touch screen and return Z(poke) position
uint16_t touchseting ()
{
  tp = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if ((zpos = tp.z) > MINPRESSURE && tp.z < MAXPRESSURE) {
    xpos = map(tp.y, TS_LEFT, TS_RT, 0, tft.width());
    ypos = map(tp.x, TS_TOP, TS_BOT, 0, tft.height());
  }
  return zpos;
}

/**********************setmode******************************/
void setmode() {
  int16_t g;// To display the integer correctly
  bool condition = 1;// Condition to stay in the while loop
  xpos = 0;
  ypos = 0;
  zpos = 0;
  showfirstscreen ();
  // getting a user number to calculate the injection time
  while (condition) {
    delay (50);
    touchseting ();
    if (0.7 * w < xpos && xpos < 3.4 * w ) {
      if (h * 0.3 < ypos && ypos < h * 1.1 ) { // If the user touches the Q box
        tft.drawRect(0.7 * w, h * 0.3, w * 2.7, h * 0.8 , BLACK);
        delay (500);
        xpos = 0;
        ypos = 0;
        zpos = 0;
        showkeyboardscreen ();
        g = 0.2 * w;// calculate next g
        for (int i = 0; i < 19;) {
          while (touchseting () > MINPRESSURE) {// if touch screen touch
            num[i] = decodekeyboard ( xpos , ypos );// calculate number and save in num variable
            xpos = 0;
            ypos = 0;
            zpos = 0;
            delay (200);
          }
          if (num[i] == -1) { // If it is -1, convert it to an integer
            Q = convert (num);
            for (i = 0; i < 20; i++) num[i] = 0;
            showfirstscreen ();
            break;
          }
          if (num[i] == -2) {// If it is -2, exchange it to zero
            for (i = 0; i < 20; i++) num[i] = 0;
            showfirstscreen ();
            break ;
          }
          if (num[i] != 0) {
            i += 1;
            tft.setTextColor(BLACK);
            tft.setTextSize(3);
            tft.setCursor(g, 0.3 * ho);
            if (num [i - 1] == -3)// cancle choice
              tft.print ('0');
            else
              tft.print (num[i - 1]);//show number in box
            g = g + 0.2 * w;
          }
        }
        delay(200);
      }

      else if (h * 1.7 < ypos && ypos < h * 2.5 ) {// If the user touches the Q box
        tft.drawRect(0.7 * w, h * 1.7, w * 2.7, h * 0.8 , BLACK);
        delay (500);
        xpos = 0;
        ypos = 0;
        zpos = 0;
        showkeyboardscreen ();
        for (int i = 0; i < 19;) {
          while (touchseting () > MINPRESSURE) {// if touch screen touch
            num[i] = decodekeyboard ( xpos , ypos );// calculate number and save in num variable
            xpos = 0;
            ypos = 0;
            zpos = 0;
            delay (200);
          }
          if (num[i] == -1) {// If it is -1, convert it to an integer
            key = convert (num);
            for (i = 0; i < 20; i++) num[i] = 0;
            showfirstscreen ();
            break;
          }
          if (num[i] == -2) {// If it is -2, exchange it to zero
            for (i = 0; i < 20; i++) num[i] = 0;
            showfirstscreen ();
            break ;
          }
          if (num[i] != 0) {
            i += 1;
            tft.setTextColor(BLACK);
            tft.setTextSize(3);
            tft.setCursor(g, 0.3 * ho);
            if (num [i - 1] == -3)// cancle choice
              tft.print ('0');
            else
              tft.print (num[i - 1]);//show number in box
            g = g + 0.2 * w;
          }
        }
      }

    }
    if (key == 1 ) {// If key was 1, select s_one for area and v_one for volume
      area = s_one;
      v = V_one ;
    }
    else if (key == 2) {// If key was 2, select s_two for area and v_two for volume
      area = s_two;
      v = V_two;
    }
    // show Q and area in box
    tft.setTextColor(BLACK);
    tft.setCursor(  w, 0.5 * h);
    tft.print (Q);
    tft.setCursor(  w, 2 * h);
    tft.print (area);
    if (0 * w < xpos && xpos < 4 * w ) {
      if (h * 2.6 < ypos && ypos < h * 3 ) {
        condition = 0;
      }
    }
  }
  screenexpos();
  delay (1000);
  int loc = 0;//To change position
  while (loc != -4) {
    while (touchseting () == 0);// It stays in the loop until the untouched screen
    loc = Texpos(xpos, ypos);// Convert coordinates to a given number
    if (loc == -3) expos(-3);// mid position run on
    if (loc == -1) expos(-1);// left run on
    if (loc == -2) expos(-2);// right run on
  }
  showvolumescreen ();
  siring();
  xpos = 0;
  ypos = 0;
  zpos = 0;
}
/********************************************************/
void setup() {
  pinMode (Ppul, OUTPUT);
  pinMode (Pdir, OUTPUT);
  Serial.begin(9600);
  tft.reset();
  tft.begin(tft.readID());
  tft.setRotation(Orientation);
  setmode();
}
void loop() {
  float ml;
  tft.fillRect(0.3 * w, h, 1.7 * w, h * 0.65, BLACK);
  tft.fillRect(2 * w, h, 1.7 * w, h * 0.65, WHITE);
  if (v - ml > 0)
    ml = n * pulstim * Q / 60000000 ;//calculate remaining volume 
  tft.setTextColor(WHITE);
  tft.setCursor(0.5 * w, h * 1.2);
  tft.print(v - ml);
  tft.setTextColor(BLACK);
  tft.setCursor(2.2 * w, h * 1.2);
  tft.print(ml);
  // show back key
  tft.fillRect(0.2 * h, 2 * w, 1.5 * h, w, WHITE);
  tft.setCursor(h * 0.5, w * 2.4);
  tft.print(F("BACK"));
  // show stop key
  tft.fillRect(2.1 * h, 2 * w, 1.5 * h, w, WHITE);
  tft.setCursor(h * 2.4, w * 2.4);
  tft.print(F("STOP"));
  while (touchseting () > MINPRESSURE) {
    if (2 * w < ypos && ypos < 3 * w ) {
      if (0.2 * h < xpos && xpos < 1.7 * h )//BACK
      {
        setmode();
        delay(100);
        xpos = 0;
        ypos = 0;
        zpos = 0;
      }
      if (2.1 * h < xpos && xpos < 3.6 * h )//stop
      {
        restfunc();
      }
    }
  }
}
