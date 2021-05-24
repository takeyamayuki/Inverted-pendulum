#include <Arduino.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include <KalmanFilter.h>

MPU6050 mpu6050;
MPU6050 initialize;

#define GYR_GAIN 20.0
#define SAMPLE_TIME 10.0
#define STOP_ANGLE 15

KalmanFilter kalmanX(0.001, 0.003, 0.03);

unsigned long preTime = 0;
unsigned long lastTime;

float angleFiltered, angleFilteredOffset;
float input, output;
float pEffect, iEffect, dEffect, lastpEffect;
float motorPWM;
int timeChange; 

// motor pin
int m1a = 8, m1b = 9, m2a = 7, m2b = 6;

bool motor_stop = false;

// フィルタ処理
void filter() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    float angleX = (atan2(ay, az) * 180 / M_PI);
    float omega =  gx / GYR_GAIN;
    unsigned long now = millis();
    float dt = (now - preTime) / 1000.00;
    preTime = now;
    float K = 0.8;
    float A = K / (K + dt);
    angleFiltered = A * (angleFiltered + omega * dt) + (1 - A) * angleX;
}

// カルマンフィルタ
void kalmanFilter() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    float angleX = (atan2(ay, az) * 180 / M_PI);
    angleFiltered = kalmanX.update(angleX, gx / GYR_GAIN);
}

// PID処理
void pidPorcess() { 
  float kp = 15;
  float kd = 150;
  float ki = 0.1;

  // 計測時間チェック
  unsigned long now = millis();
  timeChange = (now - lastTime);

  // PID処理
  if(timeChange >= SAMPLE_TIME){
    input = angleFiltered - angleFilteredOffset;
    pEffect = input;
    iEffect += pEffect * timeChange;
    dEffect = (pEffect - lastpEffect) / timeChange;
    output = kp * pEffect + ki * iEffect + kd * dEffect;
    motorPWM = output ;
    lastpEffect = pEffect;
    lastTime = now;
  }
}

void setup() {

    // ピンの初期化
    pinMode(m1a, OUTPUT);
    pinMode(m1b, OUTPUT);
    pinMode(m2a, OUTPUT);
    pinMode(m2b, OUTPUT);

    // MPU6050初期化
    Wire.begin();
    Serial.begin(115200);
    mpu6050.initialize();
    initialize.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);;

    // 初期位置のチェック
    for (int i = 0; i < 1000; i++) {
       kalmanFilter();
       angleFilteredOffset = angleFiltered;
    }

    // 最初のPID処理
    if (abs(angleFiltered) < STOP_ANGLE)    {
        angleFiltered = 0;
        kalmanFilter();
        output = pEffect = iEffect = dEffect = 0;
        pidPorcess();
    }
}

void loop() {

    // フィルタ処理
    kalmanFilter();

    // 転倒チェック
    if (abs(angleFiltered) < STOP_ANGLE && !motor_stop) {
        pidPorcess();
        Serial.print(motorPWM); Serial.print("\t");
        Serial.print(angleFiltered);
        Serial.print("\n");

    } else if (abs(angleFiltered) >= STOP_ANGLE) {
        motor_stop = true;
    }

    // 転倒時はモーター停止
    if (motor_stop) {
        analogWrite(m1a, 0);
        analogWrite(m1b, 0);
        analogWrite(m2a, 0);
        analogWrite(m2b, 0);

    } else if (motorPWM > 0) {
        analogWrite(m1a, 0);
        analogWrite(m1b, min(128, abs(motorPWM)));
        analogWrite(m2a, 0);
        analogWrite(m2b, min(128, abs(motorPWM)));
        
    } else if (motorPWM < 0) {
        analogWrite(m1a, min(128, abs(motorPWM)));
        analogWrite(m1b, 0);
        analogWrite(m2a, min(128, abs(motorPWM)));
        analogWrite(m2b, 0);
    }
}
