#include <Wire.h>

// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = D6;
const uint8_t sda = D7;

// sensitivity scale factor respective to full scale setting provided in datasheet 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

void setup() {
  Serial.begin(115200);
  Wire.begin(sda, scl);
  MPU6050_Init();
}

double xOld=0.0, yOld=0.0;
double xNew=0.0, yNew=0.0;

/* FIRST X-Y ROATTION TRY VARS
//float gyro_angle_x = 0.0;
//float gyro_angle_y = 0.0;
//float dt;
////prev time for dt calculate
//float prev_time = 0.0;
//float curr_time = 0.0;
*/

/*SECOND TRY VARS*/
float pitch = 0;
float roll = 0;
float alpha = 0.95;
float prev_time = 0.0;
float curr_time = 0.0;
float dt;

void loop() {
  double Ax, Ay, Az, T, Gx, Gy, Gz;

/* FIRST X-Y ROTATION TRY
  //rotation x-y computation dt here
  curr_time = millis() / 1000.0;
  dt = curr_time - prev_time;
  prev_time = curr_time;
  */
  
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  
  //divide each with their sensitivity scale factor
  Ax = (double)AccelX/AccelScaleFactor;
  Ay = (double)AccelY/AccelScaleFactor;
//  Az = (double)AccelZ/AccelScaleFactor;
  
//  xNew = Ax*5;
//  if(abs(xNew - xOld) > 0.5){
//    xOld = xNew;
//    Serial.print(xNew*5);    
//  }
//  else{
//    Serial.print("0");
//  }
//
//  Serial.print(" ");
//
//  yNew = Ay*5;
//  if(abs(yNew - yOld) > 0.5){
//    yOld = yNew;
//    Serial.print(yNew*5);
//  }
//  else{
//    Serial.print("0");
//  }
//  
  
//  T = (double)Temperature/340+36.53; //temperature formula
  Gx = (double)GyroX/GyroScaleFactor;
  Gy = (double)GyroY/GyroScaleFactor;

/*SECOND TRY COMPUTATION*/
  pitch = atan2(-Ax, sqrt(Ay * Ay + Az * Az)) * RAD_TO_DEG;
  roll = atan2(Ay, Az) * RAD_TO_DEG;

  curr_time = millis() / 1000.0;
  dt = curr_time - prev_time;
  prev_time = curr_time;

  float gyro_pitch = roll + Gx * dt;
  float gyro_roll = -pitch + Gy * dt;

  pitch = alpha * pitch + (1 - alpha) * gyro_pitch;
  roll = alpha * roll + (1 - alpha) * gyro_roll;

  Serial.print(pitch);
  Serial.print(" ");

/*FIRST X-Y ROTATION TRY COMPUTATION AND PRINT
  gyro_angle_x += GyroX * dt;
  gyro_angle_y += GyroY * dt;

  Serial.print(gyro_angle_x);
  Serial.print(" ");
  Serial.print(gyro_angle_y);
  */
//  Gz = (double)GyroZ/GyroScaleFactor;
//  Serial.print(" ");
//  Serial.print(GyroX);
//  Serial.print(" ");
//  Serial.print(GyroY);
//  Serial.print(" ");
//  Serial.print(GyroZ);

//  Serial.print("Ax: "); Serial.print(Ax*2);
//  Serial.print(" Ay: "); Serial.print(Ay*2);
  Serial.print("\n");
//  Serial.print(" Az: "); Serial.print(Az);
//  Serial.print(" T: "); Serial.print(T);
//  Serial.print(" Gx: "); Serial.print(Gx);
//  Serial.print(" Gy: "); Serial.print(Gy);
//  Serial.print(" Gz: "); Serial.println(Gz);

  delay(100);
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

// read all 14 register
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
//  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
//  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
//  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
//  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

//configure MPU6050
void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}
