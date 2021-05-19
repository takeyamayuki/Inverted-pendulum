
//HardwareSerial Serial2(2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial2.available()){
    int c=Serial2.read();
    Serial.println(c);
    Serial2.flush();
  }
}
