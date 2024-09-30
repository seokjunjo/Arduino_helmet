#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

// 핀 번호 설정
const int latchPinLeft = 8;   // 왼쪽 깜빡이 래치 핀
const int clockPinLeft = 12;  // 왼쪽 깜빡이 클럭 핀
const int dataPinLeft = 11;   // 왼쪽 깜빡이 데이터 핀

const int latchPinRight = 7;  // 오른쪽 깜빡이 래치 핀
const int clockPinRight = 13; // 오른쪽 깜빡이 클럭 핀
const int dataPinRight = 10;  // 오른쪽 깜빡이 데이터 핀

const int latchPinBrake = 4;  // 브레이크등 래치 핀
const int clockPinBrake = 9;  // 브레이크등 클럭 핀 (5번 대신 9번으로 변경)
const int dataPinBrake = 6;   // 브레이크등 데이터 핀

int stateLeft = 0;    // 왼쪽 LED 상태 (켜짐/꺼짐)
int stateRight = 0;   // 오른쪽 LED 상태 (켜짐/꺼짐)
int stateBrake = 0;   // 브레이크등 상태 (켜짐/꺼짐)

unsigned long previousMillisLeft = 0;
unsigned long previousMillisRight = 0;
const long interval = 500 / 8;  // 각 LED가 켜지는 시간 (밀리초 단위)
const long lastLedDelay = 100;  // 마지막 LED가 켜진 후 유지되는 시간 (밀리초 단위)

void setup() {
  // 시프트 레지스터 핀을 출력으로 설정
  pinMode(latchPinLeft, OUTPUT);
  pinMode(clockPinLeft, OUTPUT);
  pinMode(dataPinLeft, OUTPUT);

  pinMode(latchPinRight, OUTPUT);
  pinMode(clockPinRight, OUTPUT);
  pinMode(dataPinRight, OUTPUT);

  pinMode(latchPinBrake, OUTPUT);
  pinMode(clockPinBrake, OUTPUT);  // 9번 핀 설정
  pinMode(dataPinBrake, OUTPUT);

  Serial.begin(9600);
  mySerial.begin(9600);  // 통신 속도 9600으로 설정
}

void loop() {
  // 블루투스 데이터 수신
  if (mySerial.available()) {
    char receivedChar = mySerial.read();
    if (receivedChar == 'L') {
      stateLeft = !stateLeft;  // 왼쪽 LED 상태 토글
    } else if (receivedChar == 'R') {
      stateRight = !stateRight;  // 오른쪽 LED 상태 토글
    } else if (receivedChar == 'B') {
      stateBrake = 1;  // 브레이크등 켜기
    } else if (receivedChar == 'b') {
      stateBrake = 0;  // 브레이크등 끄기
    }
  }

  // 현재 시간
  unsigned long currentMillis = millis();

  // 왼쪽 깜빡이 점등
  if (stateLeft) {
    if (currentMillis - previousMillisLeft >= interval) {
      previousMillisLeft = currentMillis;
      static int ledStateLeft = 0;
      shiftOutLeft((1 << (ledStateLeft + 1)) - 1);  // 왼쪽 LED 순차 점등
      ledStateLeft++;
      if (ledStateLeft >= 8) {
        delay(lastLedDelay);  // 마지막 LED 유지 시간
        ledStateLeft = 0;
        shiftOutLeft(0);  // 모든 왼쪽 LED 끄기
      }
    }
  } else {
    shiftOutLeft(0);  // 모든 왼쪽 LED 끄기
  }

  // 오른쪽 깜빡이 점등
  if (stateRight) {
    if (currentMillis - previousMillisRight >= interval) {
      previousMillisRight = currentMillis;
      static int ledStateRight = 0;
      shiftOutRight((1 << (ledStateRight + 1)) - 1);  // 오른쪽 LED 순차 점등
      ledStateRight++;
      if (ledStateRight >= 8) {
        delay(lastLedDelay);  // 마지막 LED 유지 시간
        ledStateRight = 0;
        shiftOutRight(0);  // 모든 오른쪽 LED 끄기
      }
    }
  } else {
    shiftOutRight(0);  // 모든 오른쪽 LED 끄기
  }

  // 브레이크등 점등
  if (stateBrake) {
    shiftOutBrake(0xFF);  // 브레이크등 켜기 (모든 LED ON)
  } else {
    shiftOutBrake(0x00);  // 브레이크등 끄기 (모든 LED OFF)
  }
}

// 왼쪽 시프트 레지스터 출력 갱신 함수
void shiftOutLeft(byte data) {
  digitalWrite(latchPinLeft, LOW);
  shiftOut(dataPinLeft, clockPinLeft, MSBFIRST, data);
  digitalWrite(latchPinLeft, HIGH);
}

// 오른쪽 시프트 레지스터 출력 갱신 함수
void shiftOutRight(byte data) {
  digitalWrite(latchPinRight, LOW);
  shiftOut(dataPinRight, clockPinRight, MSBFIRST, data);
  digitalWrite(latchPinRight, HIGH);
}

// 브레이크등 시프트 레지스터 출력 갱신 함수
void shiftOutBrake(byte data) {
  digitalWrite(latchPinBrake, LOW);
  shiftOut(dataPinBrake, clockPinBrake, MSBFIRST, data);  // 9번 핀을 통해 시프트 레지스터 제어
  digitalWrite(latchPinBrake, HIGH);
}
