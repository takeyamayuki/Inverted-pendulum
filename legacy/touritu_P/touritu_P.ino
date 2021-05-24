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

void kalmanFilter() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    float angleX = (atan2(ay, az) * 180 / M_PI);
    angleFiltered = kalmanX.update(angleX, gx / GYR_GAIN);
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
}

void loop() {

    kalmanFilter();

    if(angleFiltered > angleFilteredOffset){
      //どのピンでどの方向か確認するところから
      }
    

}
