// 回転速度(35-255)
// ※値が大きいほど高速,余りにも小さい値は回転しません。
const uint8_t s = 200;

void setup() {
  Serial.begin(115200);

  ledcSetup(0,490,8); 
  ledcSetup(1,490,8); 
  ledcSetup(2,490,8); 
  ledcSetup(3,490,8); 
  
  ledcAttachPin(25,0);
  ledcAttachPin(26,1);
  ledcAttachPin(32,2);
  ledcAttachPin(33,3);
}
 
void loop() {
  // 正転(回転)
  Serial.println("正転");
  ledcWrite(2,0);
  ledcWrite(3,s);
  ledcWrite(0,s);
  ledcWrite(1,0);
  delay(1000);
 
  // 逆転(逆回転)
  Serial.println("逆転");
  ledcWrite(2,s);
  ledcWrite(3,0);
  ledcWrite(0,0);
  ledcWrite(1,s);
  delay(1000);
  
  // ブレーキ
  Serial.println("ブレーキ");
  ledcWrite(2,s);
  ledcWrite(3,s);
  ledcWrite(0,s);
  ledcWrite(1,s);
  delay(1000);
}
 
