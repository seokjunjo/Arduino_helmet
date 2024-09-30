//HC-06
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

#define NUMPIXELS_LEFT 8   // 왼쪽 네오픽셀 수
#define NUMPIXELS_RIGHT 8  // 오른쪽 네오픽셀 수
#define NUMPIXELS_BRAKE 12 // 브레이크 네오픽셀 수

// 네오픽셀 설정
Adafruit_NeoPixel leftPixels = Adafruit_NeoPixel(NUMPIXELS_LEFT, 4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightPixels = Adafruit_NeoPixel(NUMPIXELS_RIGHT, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel brakePixels = Adafruit_NeoPixel(NUMPIXELS_BRAKE, 6, NEO_GRB + NEO_KHZ800);

bool stateLeft = false;
bool stateRight = false;
bool stateBrake = false;
unsigned long previousMillisLeft = 0;
unsigned long previousMillisRight = 0;
const long interval = 500;  // 깜빡이는 간격(0.5초)

// 네오픽셀 밝기 설정
const uint8_t brightness = 50;  // 밝기 0~255

void setup() {
  leftPixels.begin();
  rightPixels.begin();
  brakePixels.begin();

  leftPixels.setBrightness(brightness);
  rightPixels.setBrightness(brightness);
  brakePixels.setBrightness(brightness);
  
  Serial.begin(9600);
  mySerial.begin(9600);  // 블루투스 통신 속도 9600으로 설정
  
  // 초기 상태에서 모든 LED를 꺼둡니다.
  leftPixels.clear();
  leftPixels.show();
  rightPixels.clear();
  rightPixels.show();
  brakePixels.clear();
  brakePixels.show();
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
      stateBrake = true;  // 브레이크등 켜기
    } else if (receivedChar == 'b') {
      stateBrake = false; // 브레이크등 끄기
    }
  }

  unsigned long currentMillis = millis();

  // 왼쪽 네오픽셀 깜빡이 제어 (주황색)
  if (stateLeft) {
    if (currentMillis - previousMillisLeft >= interval) {
      previousMillisLeft = currentMillis;
      if (leftPixels.getPixelColor(0) == 0) {  // 꺼져 있으면
        for (int i = 0; i < NUMPIXELS_LEFT; i++) {
          leftPixels.setPixelColor(i, leftPixels.Color(255, 165, 0));  // 주황색으로 켜기
        }
      } else {
        leftPixels.clear();  // 모든 LED 끄기
      }
      leftPixels.show();
    }
  } else {
    leftPixels.clear();  // 상태가 꺼졌을 때 모든 LED 끄기
    leftPixels.show();
  }

  // 오른쪽 네오픽셀 깜빡이 제어 (주황색)
  if (stateRight) {
    if (currentMillis - previousMillisRight >= interval) {
      previousMillisRight = currentMillis;
      if (rightPixels.getPixelColor(0) == 0) {  // 꺼져 있으면
        for (int i = 0; i < NUMPIXELS_RIGHT; i++) {
          rightPixels.setPixelColor(i, rightPixels.Color(255, 165, 0));  // 주황색으로 켜기
        }
      } else {
        rightPixels.clear();  // 모든 LED 끄기
      }
      rightPixels.show();
    }
  } else {
    rightPixels.clear();  // 상태가 꺼졌을 때 모든 LED 끄기
    rightPixels.show();
  }

  // 브레이크등 제어 (빨간색)
  if (stateBrake) {
    for (int i = 0; i < NUMPIXELS_BRAKE; i++) {
      brakePixels.setPixelColor(i, brakePixels.Color(255, 0, 0));  // 빨간색으로 켜기
    }
    brakePixels.show();
  } else {
    brakePixels.clear();  // 브레이크등 끄기
    brakePixels.show();
  }
}
