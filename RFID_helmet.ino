#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   9    // reset 핀은 9번
#define SS_PIN    10   // SS 핀은 10번

#define LED_PIN   2    // LED를 D2 핀에 연결
#define BUZZER_PIN 3   // 부저를 D3 핀에 연결

MFRC522 mfrc(SS_PIN, RST_PIN); // MFRC522 객체 생성

void setup() {
  Serial.begin(9600);   // 시리얼 통신 속도 설정
  SPI.begin();          // SPI 초기화
  mfrc.PCD_Init();      // RFID 리더 초기화

  pinMode(LED_PIN, OUTPUT);     // LED 핀을 출력으로 설정
}

void loop() {
  if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) {
    delay(500); // 0.5초 딜레이
    return;
  }

  // 카드가 인식되면 LED와 부저를 작동시킴
  digitalWrite(LED_PIN, HIGH);   // LED 켜기

  // 카드 UID 출력
  Serial.print("Card UID:");
  for (byte i = 0; i < mfrc.uid.size; i++) {
    Serial.print(mfrc.uid.uidByte[i], DEC);
    Serial.print(" ");
  }
  Serial.println();

  delay(1000);                    // 1초 대기
  digitalWrite(LED_PIN, LOW);      // LED 끄기
}
