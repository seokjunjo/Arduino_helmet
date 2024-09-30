float vout = 0.0; 
float vin = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;
int ledPin = 2;  // LED를 연결할 디지털 핀

void setup() {
  Serial.begin(9600); // PC 모니터로 전압값을 쉽게 확인하기 위하여 시리얼 통신을 설정
  pinMode(A0, INPUT); // 전압 센서 입력
  pinMode(ledPin, OUTPUT);  // LED 출력으로 설정
}

void loop() {
  value = analogRead(A0);
  vout = (value * 5.0) / 1024.0;  // 전압값을 계산하는 공식
  vin = vout / ( R2 / ( R1 + R2) );

  Serial.print("V: ");
  Serial.println(vin); // 현재 전압값을 출력

  if (vin >= 0.0001) { 
    digitalWrite(ledPin, HIGH);  // LED 켜기
  } else {
    digitalWrite(ledPin, LOW); 
  }

  delay(100); 
}
