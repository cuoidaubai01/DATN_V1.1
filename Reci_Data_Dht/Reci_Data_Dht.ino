// Include required libraries
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>

#define LED1 D0
#define LED2 D1
#define CBAS1 D2 //cam bien anh sang 1
#define CBAS2 D3 // cam bien anh sang 2
#define FAN1 D6 // quat 1
#define FAN2 D7 // quat 2

// Define data structure
typedef struct struct_message {
    float a;
    float b;
} struct_message;
 
// Create structured data object
struct_message myData;
 
// Callback function
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) 
{
  // Get incoming data
  memcpy(&myData, incomingData, sizeof(myData));
  
  // Print to Serial Monitor
  Serial.print("Temp: ");
  Serial.println(myData.a);
  if(myData.a > 30)
  {
    Serial.print("nhiet do cao vl");
    analogWrite(FAN1,255);
    analogWrite(FAN2,255);
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH); 
  }
  else
  {
    analogWrite(FAN1,LOW);
    analogWrite(FAN2,LOW);
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
  }
  Serial.print("Humidity: ");
  Serial.println(myData.b); 
}
 
void setup()
{
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(FAN1,OUTPUT);
  pinMode(FAN2,OUTPUT);
  pinMode(CBAS1,INPUT);
  pinMode(CBAS2,INPUT);
  //Mặc định khi khởi động chân để kick relay tắt đi
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(FAN1,LOW);
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop()
{
}
