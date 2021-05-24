//momtor_pin ７→３、８→５

#include <SoftwareSerial.h>
SoftwareSerial mySerial(14, 15); // RX, TX

int c[2], s;
float p, i, d, n;

void pid(int s){
  p=10*s+40;
  d=　
  
  return n;
}

void setup() {
  mySerial.begin(115200); // ソフトウェアシリアルの初期化
  Serial.begin(115200);
  delay(5000);
  
  preTime = micros();
  
  if (mySerial.available()) {
    c[0] = mySerial.read();
    Serial.println(c[0]);
    mySerial.flush();
  }
}

void loop() {
  
  if (mySerial.available()) {   //xiaoからmpu6050の値を獲得
    c[1] = mySerial.read();
    //Serial.println(c[1]);
    mySerial.flush();
  }

  s=c[0]-c[1];      //最初のイチとの差  s=回転角
  Serial.print(s);

  if(s==0){
      Serial.println("stop");
      analogWrite(5,s);
      analogWrite(9,s);
      analogWrite(6,s);
      analogWrite(3,s);
  }
  if(s>0){         //その差の分だけ回転させる s=モーターの回転量
      Serial.println("plus");
      s=pid(s);        
      analogWrite(5,s);
      analogWrite(9,0);
      analogWrite(6,0);
      analogWrite(3,s);
  }
  if(s<0){
      Serial.println("minus");
      s=-s;
      s=pid(s);
      analogWrite(5,0);
      analogWrite(9,s);
      analogWrite(6,s);
      analogWrite(3,0);
  }
  
}
