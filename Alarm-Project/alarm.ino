#include "U8glib.h"
#include <Wire.h>
#include <SoftwareSerial.h> 
#include <DFPlayer_Mini_Mp3.h>
#include <DFRobotDFPlayerMini.h>
#include "Arduino.h"
 
#define DS3231_I2C_ADDRESS 104 //RTC 모듈
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE); //u8g 생성자 (디스플레이)
 
byte seconds, minutes, hours, day, date, month, year; //현재 시간
int alhour10=0; //10의 자리
int alhour1=0; // 1의 자리
int almin10=0;
int almin1=0;
int alhour=0;
int almin=0;

char weekDay[4];
int sw1=2; //모드 이동 스위치
int sw2=7; //
int sw3=8; //
int COUNTPIN=3;

int mode=0;


void setup()
{
  pinMode(sw1,INPUT); 
  pinMode(sw2,INPUT);
  pinMode(COUNTPIN,INPUT);
  
  Wire.begin();
  Serial.begin(9600);

   mp3_set_serial (Serial);      // DFPlayer-mini mp3 module 시리얼 세팅
   delay(1);                     
   mp3_set_volume (15);          // 볼륨조절 값 0~30
}
 
void loop()
{
  if(digitalRead(sw1)==HIGH)
  {
    delay(150);
    mode++;
    if(mode==5)
    {
      mode=0;
    }
  }
  delay(1);

  
  watchConsole();
  get3231Date();

 
 /*
  Serial.print(weekDay);
  Serial.print(", 20");
  Serial.print(year, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(date, DEC);
  Serial.print(" - ");
  Serial.print(hours, DEC); 
  Serial.print(":"); 
  Serial.print(minutes, DEC); 
  Serial.print(":"); 
  Serial.println(seconds, DEC);*/


  switch(mode){
    case 0:
      mp3_stop();
      u8g.firstPage();  
      do {
        mode0draw();
      } while( u8g.nextPage() );
      break;
      
    case 1:

      u8g.firstPage();  
      do {
        countdraw();
      } while( u8g.nextPage() );
      break;
    
    case 2:   
      if(digitalRead(sw2)==HIGH)
     {
        delay(150);
        alhour10++;
        
      if(alhour10==3)
      {
         alhour10=0;
      }
      alhour=10*alhour10 + alhour1;
     } 
     else if(digitalRead(sw3)==HIGH)
     {
      delay(150);
        alhour1++;
      if(alhour1==10)
      {
         alhour1=0;
      }alhour=10*alhour10 + alhour1;
     }
     
     else{
      delay(1);   

      
      u8g.firstPage();  
      do {
        setdraw();
      } while( u8g.nextPage() );
      
      break;
      }
    case 3:
      mp3_stop();
      if(digitalRead(sw2)==HIGH)
     {
        delay(150);
        almin10++;
      if(almin10==7)
      {
         almin10=0;
      }
      almin=10*almin10+almin1;
     } else if(digitalRead(sw3)==HIGH)
     {
      delay(150);
        almin1++;
       
      if(almin1==10)
      {
         almin1=0;
      }almin=10*almin10+almin1;
     }
      
     else{
      delay(1);   

      
      u8g.firstPage();  
      do {
        setdraw();
      } while( u8g.nextPage() );
      delay(1);
      
      break;
      }
    
      
    case 4:
      u8g.firstPage();  
      do {
        alarmdraw();
      } while( u8g.nextPage() );
      if(alhour==int(hours) && almin==int(minutes))
      {
      mp3_play();
      }
      delay(1);
      break;
  }
  
  delay(1);
  
}
 
// 10진수를 2진화 10진수인 BCD 로 변환 (Binary Coded Decimal)
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}
 
void watchConsole()
{
  if (Serial.available()) {      // Look for char in serial queue and process if found
    if (Serial.read() == 84) {   //If command = "T" Set Date
      set3231Date();
      get3231Date();
      Serial.println(" ");
    }
  }
}
 
//시간설정
// T(설정명령) + 년(00~99) + 월(01~12) + 일(01~31) + 시(00~23) + 분(00~59) + 초(00~59) + 요일(1~7, 일1 월2 화3 수4 목5 금6 토7)
// 예: T1605091300002 (2016년 5월 9일 13시 00분 00초 월요일)
void set3231Date()
{
  year    = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  month   = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  date    = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  hours   = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  minutes = (byte) ((Serial.read() - 48) *10 +  (Serial.read() - 48));
  seconds = (byte) ((Serial.read() - 48) * 10 + (Serial.read() - 48));
  day     = (byte) (Serial.read() - 48);
 
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x00);
  Wire.write(decToBcd(seconds));
  Wire.write(decToBcd(minutes));
  Wire.write(decToBcd(hours));
  Wire.write(decToBcd(day));
  Wire.write(decToBcd(date));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}
 
 
void get3231Date()
{
  // send request to receive data starting at register 0
  Wire.beginTransmission(DS3231_I2C_ADDRESS); // 104 is DS3231 device address
  Wire.write(0x00); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7); // request seven bytes
 
  if(Wire.available()) {
    seconds = Wire.read(); // get seconds
    minutes = Wire.read(); // get minutes
    hours   = Wire.read();   // get hours
    day     = Wire.read();
    date    = Wire.read();
    month   = Wire.read(); //temp month
    year    = Wire.read();
       
    seconds = (((seconds & B11110000)>>4)*10 + (seconds & B00001111)); // convert BCD to decimal
    minutes = (((minutes & B11110000)>>4)*10 + (minutes & B00001111)); // convert BCD to decimal
    hours   = (((hours & B00110000)>>4)*10 + (hours & B00001111)); // convert BCD to decimal (assume 24 hour mode)
    day     = (day & B00000111); // 1-7
    date    = (((date & B00110000)>>4)*10 + (date & B00001111)); // 1-31
    month   = (((month & B00010000)>>4)*10 + (month & B00001111)); //msb7 is century overflow
    year    = (((year & B11110000)>>4)*10 + (year & B00001111));
  }
  else {
    //oh noes, no data!
  }
 
  switch (day) {
    case 1:
      strcpy(weekDay, "Sun");
      break;
    case 2:
      strcpy(weekDay, "Mon");
      break;
    case 3:
      strcpy(weekDay, "Tue");
      break;
    case 4:
      strcpy(weekDay, "Wed");
      break;
    case 5:
      strcpy(weekDay, "Thu");
      break;
    case 6:
      strcpy(weekDay, "Fri");
      break;
    case 7:
      strcpy(weekDay, "Sat");
      break;
  }
}

void mode0draw(void) {
   
  u8g.setFont(u8g_font_unifont);
  u8g.setScale2x2();
  u8g.drawStr( 5, 15, "MIN'S"); // x축 좌표, y축 좌표
  u8g.undoScale();
}

void countdraw(void) {
  // mode1  
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 5, 15, "Counting"); // x축 좌표, y축 좌표
  
  u8g.setScale2x2();
  u8g.drawStr( 26, 30, "00");
  u8g.undoScale();
}

void setdraw(void) {  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
 
  u8g.drawStr( 5, 15, "Time Set"); // x축 좌표, y축 좌표
  
 if(int(hours)>=10){
  u8g.setPrintPos(87,15);
  u8g.print(hours);
  }
  else{
  u8g.setPrintPos(97,15);
  u8g.print(hours);
  }
  u8g.drawStr( 102, 15, ":");
  if(int(minutes)>=10){
  u8g.setPrintPos(107,15);
  u8g.print(minutes);  
  }
  else{
  u8g.setPrintPos(117,15);
  u8g.print(minutes);  
  }
  
  u8g.setScale2x2();
  if(alhour >=10){
  u8g.setPrintPos(13,30);
  u8g.print(alhour);
  }
  else{
  u8g.setPrintPos(21,30);
  u8g.print(alhour);
  }
  u8g.drawStr( 26, 30, ":");
  if(almin >=10)
  {
  u8g.setPrintPos(32,30);
  u8g.print(almin);
  }
  else
  {
  u8g.setPrintPos(40,30);
  u8g.print(almin);
  }
  u8g.undoScale();
  delay(1);
}

void alarmdraw(void) {  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
 
  u8g.drawStr( 5, 15, "Alarm"); // x축 좌표, y축 좌표
  if(int(hours)>=10){
  u8g.setPrintPos(87,15);
  u8g.print(hours);
  }
  else{
  u8g.setPrintPos(97,15);
  u8g.print(hours);
  }
  u8g.drawStr( 102, 15, ":");
  if(int(minutes)>=10){
  u8g.setPrintPos(107,15);
  u8g.print(minutes);  
  }
  else{
  u8g.setPrintPos(117,15);
  u8g.print(minutes);  
  }
  
  u8g.setScale2x2();
  if(alhour >=10){
  u8g.setPrintPos(13,30);
  u8g.print(alhour);
  }
  else{
  u8g.setPrintPos(21,30);
  u8g.print(alhour);
  }
  u8g.drawStr( 26, 30, ":");
  if(almin >=10)
  {
  u8g.setPrintPos(32,30);
  u8g.print(almin);
  }
  else
  {
  u8g.setPrintPos(40,30);
  u8g.print(almin);
  }
  u8g.undoScale();
  delay(1);
}
