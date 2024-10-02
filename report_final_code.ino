//초음파 신고 코드
#include <Wire.h>
#include <math.h>
#include <SoftwareSerial.h>

// 핀 정의
const int trigPin = 2;
const int echoPin = 3;
const int bluetoothTX = 8;
const int bluetoothRX = 7;

// 변수 정의
long duration;
int distance;
unsigned long startTime1;
unsigned long elapsedTime1;
unsigned long startTime2;
unsigned long elapsedTime2;
bool isTiming1 = false;
bool isTiming2 = false;
bool alertSent = false;  // 응급상황 메시지 전송 여부

// 낙상 관련 변수
const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ;
float angleX, angleY;
int fallAngleThreshold_x = 60;  // 넘어짐 상태로 판단할 각도
int fallAngleThreshold_y = 90;
unsigned long fallStartTime = 0;  // 낙상 시작 시간
unsigned long fallDuration = 50;  // 임계값 초과 상태 유지 시간 (밀리초 단위)
bool fallen_flag = 0;

SoftwareSerial bluetooth(bluetoothTX, bluetoothRX);

void setup() {
  // 핀 모드 설정
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  // 시리얼 및 블루투스 시리얼 초기화
  Serial.begin(9600);
  bluetooth.begin(9600);
 
  // I2C 및 MPU6050 초기화
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // 전원 관리 레지스터
  Wire.write(0);     // 슬립 모드 해제
  Wire.endTransmission(true);

  Serial.println("Testing device connections...");
// 타이머1 설정 (CTC 모드)
  TCCR1A = 0;  // 타이머 설정: CTC 모드
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);  // 1024 프리스케일러 설정

  // OCR1A 설정 (1초 주기: 16MHz / 1024 프리스케일러 = 15625)
  OCR1A = 30000;  // 비교 일치 값 설정

  TIMSK1 = (1 << OCIE1A);  // 타이머 비교 일치 인터럽트 활성화

  sei();  // 모든 인터럽트 활성화


  pinMode(12, OUTPUT); // Timer1 RED LED
  pinMode(11, OUTPUT); // Timer2 GREEN LED
}
ISR(TIMER1_COMPA_vect) {
  // 각도 계산 및 낙상 감지 로직
  if (abs(angleX) > fallAngleThreshold_x || abs(angleY) > fallAngleThreshold_y) {
    if (fallStartTime == 0) {
      fallStartTime = millis();
    }
    if (millis() - fallStartTime > fallDuration) {
      Serial.println("Fall detected!");
      fallen_flag = 1;
    }
  } else {
    fallStartTime = 0;
    Serial.println("Stable.");
    fallen_flag = false;
  }
}

void loop() {
  // 초음파 센서로 거리 측정
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // MPU-6050로부터 데이터 읽기
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // 각도 계산
  angleX = atan2(AcY, AcZ) * 180 / PI;
  angleY = atan2(AcX, AcZ) * 180 / PI;


  // 타이머값1 작동 및 초기화-착용
  if (distance <= 5) {
    if (!isTiming1) {
      startTime1 = millis();
      isTiming1 = true;
    }
    elapsedTime1 = (millis() - startTime1) / 1000; // 경과 시간 계산 (초 단위)
    digitalWrite(12, HIGH); //RED ON
  }
  else {
    // 거리가 5cm 이상으로 떨어지면 시간 초기화
    isTiming1 = false;
    elapsedTime1 = 0;
    digitalWrite(12, LOW); //RED OFF
  }




  // 타이머값2 작동 및 초기화
  if (isTiming1) {


    if (fallen_flag) {


      if (!isTiming2) {
        startTime2 = millis();
        isTiming2 = true;
      }
      elapsedTime2 = (millis() - startTime2) / 1000; // 경과 시간 계산 (초 단위)
    }
    else {
      isTiming2 = false;
      elapsedTime2 = 0;
      fallen_flag = 0;
    }
  } else {
    // 타이머값1이 작동하지 않으면 타이머값2도 초기화
    isTiming2 = false;
    elapsedTime2 = 0;
    fallen_flag = 0;
  }

  // 초록색 LED 제어: 타이머2가 작동 중일 때만 켜짐
  if (isTiming2) {
    digitalWrite(11, HIGH); //GREEN ON
  } else {
    digitalWrite(11, LOW); //GREEN OFF
  }

  // 타이머2가  초과하면 응10초를급상황 메시지 전송
  if (elapsedTime2 > 10 && !alertSent) {
    bluetooth.println("응급상황");
    Serial.println("응급상황");
    alertSent = true;
  }

  // 타이머값2가 초기화되면 메시지 전송 플래그도 초기화
  if (elapsedTime2 == 0) {
    alertSent = false;
  }




  //timer1 print
  Serial.print("Time1: ");
  Serial.print(elapsedTime1);
  Serial.println(" seconds");



  // 타이머2 값을 시리얼 모니터에 출력
  Serial.print("Time2: ");
  Serial.print(elapsedTime2);
  Serial.println(" seconds");
 
  delay(1000); // 1초 간격으로 측정 및 전송
 
}
