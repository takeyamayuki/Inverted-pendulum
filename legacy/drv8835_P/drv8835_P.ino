// 回転速度(35-255)
// ※値が大きいほど高速,余りにも小さい値は回転しません。
const uint8_t s = 255;
 
void setup() {
  Serial.begin(9600);
}
 
void loop() {
  // 正転(回転)
  Serial.println("正転");
  analogWrite(8,0);
  analogWrite(9,s);
  analogWrite(6,s);
  analogWrite(7,0);
  delay(1000);
 
  // 逆転(逆回転)
  Serial.println("逆転");
  analogWrite(8,s);
  analogWrite(9,0);
  analogWrite(6,0);
  analogWrite(7,s);
  delay(1000);
  
  // ブレーキ
  Serial.println("ブレーキ");
  analogWrite(8,s);
  analogWrite(9,s);
  analogWrite(6,s);
  analogWrite(7,s);
  delay(1000);
}
 
