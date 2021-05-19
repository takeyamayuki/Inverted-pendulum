
#include <SoftwareSerial.h>

SoftwareSerial mySerial(14, 15); // RX, TX

void setup() {
  mySerial.begin(115200); // ソフトウェアシリアルの初期化
  Serial.begin(115200);
}

void loop() {
  int c;
  if (mySerial.available()) {
    c = mySerial.read();
    Serial.println(c);
    mySerial.flush();
  }
  
}
