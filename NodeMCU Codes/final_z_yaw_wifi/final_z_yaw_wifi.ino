#include <Wire.h>
#include <MPU6050.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <stdlib.h>

MPU6050 mpu;

const uint8_t scl = D6;
const uint8_t sda = D7;

float accelZ_previous = 0.0;
float yaw = 0.0;

float prevTime = 0.0, currTime = 0.0;
float dt = 0.0;

float deg = 0.0;
float angle;

int value = 0;

// WiFi credentials
const char* ssid = "Galaxy A51C927";
const char* password = "Shreyansh";

// MQTT broker details
const char* broker = "broker.hivemq.com";
const int port = 1883;
const char* clientId = "Shreyansh_nodemcu_mpu6050";
const char* topic = "topic20103046/pub";
const char* topicSub = "topic20103046/sub";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void callback(char* topicSub, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();

  // Processing the message here
  value = atoi((char*)payload);
  if(value == 1){
    Serial.println("Reseting");
    deg = 0.0; 
    angle = 0.0;
  }
}


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(broker, port);
  client.setCallback(callback);
  
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect(clientId)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(topicSub);
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  Wire.begin(sda, scl);
  mpu.initialize();

  mpu.setDLPFMode(3); // Set digital low-pass filter to 44Hz

  mpu.CalibrateGyro();
  mpu.CalibrateAccel();
}

void loop() {
  float accelZ_m_s2 = mpu.getAccelerationZ() / 16384.0 * 9.81;

  currTime = millis() / 1000.0;
  dt = currTime - prevTime;
  prevTime = currTime;
  
  /*Calculated gyro and computed yaw here*/
  float gyroZ = mpu.getRotationZ();
  yaw = gyroZ * 0.0000611 * dt;

  
  if(abs(yaw) > 0.01){
    deg = deg+yaw;
    angle = deg * 128.57142;
    if(abs(angle) >= 360){
      deg = 0.0;
    }
  }
  
  /*Checking if movement in Z axis*/
  if (abs(accelZ_m_s2 - accelZ_previous) > 0.5) {
    Serial.print(abs(accelZ_m_s2 - accelZ_previous));
    Serial.println("Movement detected in Z-axis");
    Serial.print(1);
    Serial.print("||");
    Serial.print(angle);
    Serial.print(" deg");
    char msg[50];
    sprintf(msg, "1,%f deg", angle);
    client.publish(topic, msg);
  } else {
    Serial.print(0);
    Serial.print("||");
    Serial.print(angle);
    char msg[50];
    sprintf(msg, "0,%f deg", angle);
    client.publish(topic, msg);
  }
  accelZ_previous = accelZ_m_s2;

  Serial.print("\n");
  delay(100);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    if (client.connect(clientId)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}
