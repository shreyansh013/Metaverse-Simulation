#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);

long timer = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

float x_old = 0.0;
float x_new = 0.0;
float y_old = 0.0;
float y_new = 0.0;

void loop() {
  mpu6050.update();
//  x values
    x_new = mpu6050.getAccX();
    if(x_new - x_old > 0.5){
        Serial.print(x_new);
      }
    else{
        Serial.print("0");
      }

    Serial.print(" ");
    
//    y values
    y_new = mpu6050.getAccY();
    if(y_new - y_old > 0.5){
        Serial.print(y_new);
      }
    else{
        Serial.print("0");
      }
    
    Serial.print("\n");
} 
