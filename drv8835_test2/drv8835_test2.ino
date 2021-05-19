int s = 0; //7→４→３、８→５
void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  for(s=40; s<255; s++){
    analogWrite(5,0);
    analogWrite(9,s);
    analogWrite(6,s);
    analogWrite(3,0);
    delay(20);
  }
  
  for(s=40; s<255; s++){
    // 逆転(逆回転)
    analogWrite(5,s);
    analogWrite(9,0);
    analogWrite(6,0);
    analogWrite(3,s);
    delay(20);
  }
  
  // ブレーキ
  analogWrite(5,s);
  analogWrite(9,s);
  analogWrite(6,s);
  analogWrite(3,s);
  delay(1000);
}
