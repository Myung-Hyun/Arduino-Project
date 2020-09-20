#include <DFRobotDFPlayerMini.h>

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
 
void setup () {
    Serial.begin (9600);
    mp3_set_serial (Serial);      // DFPlayer-mini mp3 module 시리얼 세팅
    delay(1);                     // 볼륨값 적용을 위한 delay
    mp3_set_volume (20);          // 볼륨조절 값 0~30
}
 
void loop () {        
    mp3_play();    //0001 파일 플레이
    delay (5000);
    mp3_next();
    delay (5000);
   
    
    //원하는 파일에서 mp3_stop();으로 멈춰 놓고 코드 돌리면 멈췄던 파일 재생 가능.
}
