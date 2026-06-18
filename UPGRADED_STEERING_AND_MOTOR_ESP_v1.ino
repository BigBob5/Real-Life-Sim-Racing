#include <ESP32Servo.h>
#include <esp_now.h>
#include <WiFi.h>

int R_EN = 27;
int R_PWM = 12;
int L_EN = 14;
int L_PWM = 13;

Servo Steering;

bool Drive=false;
bool Reverse=false;

unsigned long lastPacketTime = 0;


struct PacketData {
  int steerValue;
  int powerValue;
  int driveMode;
};

PacketData incomingData;
volatile bool newDataReceived = false;

void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingDataPtr, int len) { 
  memcpy(&incomingData, incomingDataPtr, sizeof(PacketData)); 
  newDataReceived = true; 
} 

void setup() {
  Steering.attach(26);

 pinMode(R_EN, OUTPUT);
 pinMode(R_PWM, OUTPUT);
 pinMode(L_EN, OUTPUT); 
 pinMode(L_PWM, OUTPUT);
 digitalWrite(R_EN, HIGH);
 digitalWrite(L_EN, HIGH);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv); 

  incomingData.steerValue = 110;
  incomingData.powerValue = 0;
  incomingData.driveMode = 0; 

  lastPacketTime = millis();
}

void loop() {
if(newDataReceived){
  newDataReceived = false;
  lastPacketTime = millis();
  Steering.write(incomingData.steerValue);
  if(incomingData.driveMode==1)
  {
    Drive = true;
    Reverse = false;
  }
  else if (incomingData.driveMode==2)
  {
    Drive = false;
    Reverse = true;
  }
  else
  {
    Drive = false;
    Reverse = false; 
  }

  if(Drive)
  {
    analogWrite(R_PWM, incomingData.powerValue);
    analogWrite(L_PWM, 0);
  }
  else if(Reverse)
  {
    analogWrite(R_PWM, 0);
    analogWrite(L_PWM, incomingData.powerValue);
  }
  else{
    analogWrite(R_PWM,0);
    analogWrite(L_PWM,0);
  }
}

if(millis()-lastPacketTime>500){
  Steering.write(90);
  analogWrite(R_PWM,0);
  analogWrite(L_PWM,0);
}

}