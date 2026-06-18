#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x84, 0x1F, 0xE8, 0x38, 0xC3, 0x70};

struct PacketData {
  int steerValue;
  int powerValue;
  int driveMode;
};


PacketData myData;

esp_now_peer_info_t peerInfo;

unsigned long lastPacketTime = 0;

void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) { 
  Serial.print("\r\nLast Packet Send Status:\t"); 
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail"); 
} 

 
void setup() {
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent); 
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  bool shouldSend = false;

  if(Serial.available()>0){
    int steer=Serial.parseInt();
    int power=Serial.parseInt();
    int gear=Serial.parseInt();

    if(Serial.read() == '\n') {
      myData.steerValue=steer;
      myData.powerValue=power;
      myData.driveMode=gear;
      lastPacketTime=millis();
      shouldSend=true;
    }
  }
  if(millis()-lastPacketTime > 500) {
    if(myData.powerValue != 0 || myData.steerValue != 90) {
      myData.steerValue = 90;
      myData.powerValue = 0;
      myData.driveMode = 0;
      shouldSend = true;
    }
  }
  if(shouldSend){ 
   esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
  }
}