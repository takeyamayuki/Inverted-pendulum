//momtor_pin ７→３、８→５
//motor pin  3>33>26 5>34>32 6>32>25 9>35>33 aaaa

#include <Wire.h>    //esp32 SDA=21,SCL=22

#define Kp      10
#define Ki      50
#define Kd      0.1

float duty = 0;
float dt, preTime;
float vol;
float P, I, D, preP;
float target;
int c1, c2, s=0;

#define MPU6050_ADDR         0x68 // MPU-6050 device address
#define MPU6050_SMPLRT_DIV   0x19 // MPU-6050 register address
#define MPU6050_CONFIG       0x1a
#define MPU6050_GYRO_CONFIG  0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6b
double offsetX = 0, offsetY = 0, offsetZ = 0;
double gyro_angle_x = 0, gyro_angle_y = 0, gyro_angle_z = 0;
float angleX, angleY, angleZ;
float interval, preInterval;
float acc_x, acc_y, acc_z, acc_angle_x, acc_angle_y;
float gx, gy, gz, dpsX, dpsY, dpsZ;
float x_kand=0.8;
float y_kand=0.8;
float sc_kand=0.05;
int i1=0, i2=0;

//加速度、ジャイロから角度を計算
void calcRotation(){

  int16_t raw_acc_x, raw_acc_y, raw_acc_z, raw_t, raw_gyro_x, raw_gyro_y, raw_gyro_z ;
  
  //レジスタアドレス0x3Bから、計14バイト分のデータを出力するようMPU6050へ指示
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 14, true);
  
  //出力されたデータを読み込み、ビットシフト演算
  raw_acc_x = Wire.read() << 8 | Wire.read();
  raw_acc_y = Wire.read() << 8 | Wire.read();
  raw_acc_z = Wire.read() << 8 | Wire.read();
  raw_t = Wire.read() << 8 | Wire.read();
  raw_gyro_x = Wire.read() << 8 | Wire.read();
  raw_gyro_y = Wire.read() << 8 | Wire.read();
  raw_gyro_z = Wire.read() << 8 | Wire.read();

  dpsX = ((float)raw_gyro_x) / 65.5; // LSB sensitivity: 65.5 LSB/dps @ ±500dps
  dpsY = ((float)raw_gyro_y) / 65.5;
  dpsZ = ((float)raw_gyro_z) / 65.5;

  //前回計算した時から今までの経過時間を算出
  interval = millis() - preInterval;
  preInterval = millis();
  
  //数値積分
  gyro_angle_x += (dpsX - offsetX) * (interval * 0.001);
  gyro_angle_y += (dpsY - offsetY) * (interval * 0.001);
  gyro_angle_z += (dpsZ - offsetZ) * (interval * 0.001);
  
  //相補フィルター
  angleX = (0.996 * gyro_angle_x) + (0.004 * acc_angle_x);
  angleY = (0.996 * gyro_angle_y) + (0.004 * acc_angle_y);
  angleZ = gyro_angle_z;
  gyro_angle_x = angleX;
  gyro_angle_y = angleY;
  gyro_angle_z = angleZ;
}

//I2c書き込み
void writeMPU6050(byte reg, byte data) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);   
  Wire.write(data);
  Wire.endTransmission();
}

//i2C読み込み
byte readMPU6050(byte reg) {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(reg);
  Wire.endTransmission(true);
  Wire.requestFrom(MPU6050_ADDR, 1/*length*/); 
  byte data =  Wire.read();
  return data;
}

void setup() {
  //Serial2.begin(4800); // ソフトウェアシリアルの初期化
  Serial.begin(9600);
  
  ledcSetup(0,490,8); 
  ledcSetup(1,490,8); 
  ledcSetup(2,490,8); 
  ledcSetup(3,490,8); 
  
  ledcAttachPin(26,0);
  ledcAttachPin(25,1);
  ledcAttachPin(32,2);
  ledcAttachPin(33,3);

  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  delay(100);
  
  //正常に接続されているかの確認
  if (readMPU6050(MPU6050_WHO_AM_I) != 0x68) {
    Serial.println("\nWHO_AM_I error.");
    while (true) ;
  }

  //設定を書き込む
  writeMPU6050(MPU6050_SMPLRT_DIV, 0x00);   // sample rate: 8kHz/(7+1) = 1kHz
  writeMPU6050(MPU6050_CONFIG, 0x00);       // disable DLPF, gyro output rate = 8kHz
  writeMPU6050(MPU6050_GYRO_CONFIG, 0x08);  // gyro range: ±500dps
  writeMPU6050(MPU6050_ACCEL_CONFIG, 0x00); // accel range: ±2g
  writeMPU6050(MPU6050_PWR_MGMT_1, 0x01);   // disable sleep mode, PLL with X gyro

  //キャリブレーション
  Serial.print("Calculate Calibration");
  for(int i = 0; i < 3000; i++){
    
    int16_t raw_acc_x, raw_acc_y, raw_acc_z, raw_t, raw_gyro_x, raw_gyro_y, raw_gyro_z ;
    
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 14, true);
  
    raw_acc_x = Wire.read() << 8 | Wire.read();
    raw_acc_y = Wire.read() << 8 | Wire.read();
    raw_acc_z = Wire.read() << 8 | Wire.read();
    raw_t = Wire.read() << 8 | Wire.read();
    raw_gyro_x = Wire.read() << 8 | Wire.read();
    raw_gyro_y = Wire.read() << 8 | Wire.read();
    raw_gyro_z = Wire.read() << 8 | Wire.read();
    dpsX = ((float)raw_gyro_x) / 65.5;
    dpsY = ((float)raw_gyro_y) / 65.5;
    dpsZ = ((float)raw_gyro_z) / 65.5;
    offsetX += dpsX;
    offsetY += dpsY;
    offsetZ += dpsZ;
    if(i % 1000 == 0){
      Serial.print(".");
    }
  }          
  Serial.println();

  offsetX /= 3000;
  offsetY /= 3000;
  offsetZ /= 3000;
 
  Serial.print("offsetX : ");
  Serial.println(offsetX);
  Serial.print("offsetY : ");
  Serial.println(offsetY);
  Serial.print("offsetZ : ");
  Serial.println(offsetZ);
  
  calcRotation();
  target=angleY;          //最初の角度保存
  preTime = micros();

  delay(10000);
}

void loop() {
  calcRotation();

  dt = (micros() - preTime) / 1000000;
  preTime = micros();
  P  = angleY - target;
  //I += P * dt;
  //D  = (P - preP) / dt;
  duty = Kp * P ; //+ Ki * I + Kd * D
  Serial.print(duty);
  Serial.print("  ");
  if(duty==0){
      ledcWrite(2,0);
      ledcWrite(3,0);
      ledcWrite(0,0);
      ledcWrite(1,0);
  }
  else if(duty<0){         //その差の分だけ回転させる s=モーターの回転量
      duty=-duty;   
      ledcWrite(2,duty);
      ledcWrite(3,0);
      ledcWrite(0,0);
      ledcWrite(1,duty);
  }
  else if(duty>0){
      ledcWrite(2,0);
      ledcWrite(3,duty);
      ledcWrite(0,duty);
      ledcWrite(1,0);
  }
  preP = P;
  Serial.println(angleY);
}
