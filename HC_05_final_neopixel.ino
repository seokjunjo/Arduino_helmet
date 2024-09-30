// HC-05
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

const int switchPinLeft = 4;  // 왼쪽 깜빡이 스위치
const int switchPinRight = 5; // 오른쪽 깜빡이 스위치
const int switchPinBrake = 6; // 브레이크 스위치

int valLeft = 0;      // 왼쪽 스위치 현재 상태
int oldValLeft = 0;   // 왼쪽 스위치 이전 상태

int valRight = 0;     // 오른쪽 스위치 현재 상태
int oldValRight = 0;  // 오른쪽 스위치 이전 상태

int valBrake = 0;     // 브레이크 스위치 현재 상태

void setup() {
  pinMode(switchPinLeft, INPUT_PULLUP);
  pinMode(switchPinRight, INPUT_PULLUP);
  pinMode(switchPinBrake, INPUT_PULLUP);
  
  Serial.begin(9600);
  mySerial.begin(9600);  // 블루투스 통신 속도 9600으로 설정
}

void loop() {
  // 왼쪽 스위치 상태 확인 및 블루투스 전송
  valLeft = digitalRead(switchPinLeft);
  if (valLeft == LOW && oldValLeft == HIGH) {
    delay(50);  // 디바운싱
    if (digitalRead(switchPinLeft) == LOW) {
      mySerial.write('L');  // 왼쪽 깜빡이 상태 토글 명령 전송
    }
  }
  oldValLeft = valLeft;

  // 오른쪽 스위치 상태 확인 및 블루투스 전송
  valRight = digitalRead(switchPinRight);
  if (valRight == LOW && oldValRight == HIGH) {
    delay(50);  // 디바운싱
    if (digitalRead(switchPinRight) == LOW) {
      mySerial.write('R');  // 오른쪽 깜빡이 상태 토글 명령 전송
    }
  }
  oldValRight = valRight;

  // 브레이크 스위치 상태 확인 및 블루투스 전송
  valBrake = digitalRead(switchPinBrake);
  if (valBrake == LOW) {
    mySerial.write('B');  // 브레이크 등 켜기
  } else {
    mySerial.write('b');  // 브레이크 등 끄기
  }
}
