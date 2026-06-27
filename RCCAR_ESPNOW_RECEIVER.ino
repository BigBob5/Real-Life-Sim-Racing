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

//structure of the data packet to send
struct PacketData {
  int steerValue;
  int powerValue;
  int driveMode;
};

PacketData incomingData;
volatile bool newDataReceived = false;

//the callback function that ESP-NOW runs
void OnDataRecv(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingDataPtr, int len) { 
  memcpy(&incomingData, incomingDataPtr, sizeof(PacketData)); 
  newDataReceived = true; 
} 

void setup() {
  Serial.begin(115200);
  Steering.attach(26);

  //set motor driver control pins
  pinMode(R_EN, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_EN, OUTPUT); 
  pinMode(L_PWM, OUTPUT);
  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

  WiFi.mode(WIFI_STA);

  //initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv); 

  //set car straight and stopped
  incomingData.steerValue = 90;
  incomingData.powerValue = 0;
  incomingData.driveMode = 0; 

  lastPacketTime = millis();
}

void loop() {
  //new packet received
  if(newDataReceived){
    newDataReceived = false;
    lastPacketTime = millis();
    //move steering servo
    Steering.write(incomingData.steerValue);
    // Set drive mode based on gear
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
    //control motor
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

  //timeout failsafe: If we havent received new data for 500ms, stop car
  if(millis()-lastPacketTime>500){
    Steering.write(90);
    analogWrite(R_PWM,0);
    analogWrite(L_PWM,0);
  }

}
