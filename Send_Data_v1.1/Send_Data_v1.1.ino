// Include required libraries
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>
 
// Define DHT22 parameters
#define DHTPin 4
#define DHTType DHT22
#define BOARD_ID 2
 
// Create DHT Object
DHT dht(DHTPin, DHTType);
WiFiClient client;

String apiKey = "XT5KMD0VX2WETCDU";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "TP-LINK_259D6A";     // replace with your wifi ssid and wpa2 key
const char *pass =  "11223344";
const char* server = "api.thingspeak.com";

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//// Responder MAC Address (Replace with your responders MAC Address)
 
// Define data structure
typedef struct struct_message {
  float a;
  float b;
} struct_message;
 
// Create structured data object
struct_message myData;
 
// Register peer
//esp_now_peer_info_t peerInfo;
 
// Sent data callback function
void OnDataSent(uint8_t *macAddr, uint8_t sendStatus)
{
  Serial.print("\r\nLast Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}


 void init_EspNow()
 {
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    // Init ESP-NOW
//    if (esp_now_init() != 0) {
//      Serial.println("Error initializing ESP-NOW");
//      return;
//    } 
    // Set ESP-NOW role
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

    // Once ESPNow is successfully init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);
  
    // Register peer
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0); 
 }

void init_Wifi()
{
  Serial.println("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void setup()
{
   // Init Serial Monitor
  Serial.begin(115200);
  // Initiate DHT22
  dht.begin();

      if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
      return;
    } 
}
 
void loop()
{
 
  // Read DHT22 module values
  float temp = dht.readTemperature();
  delay(10);
  float humid = dht.readHumidity();
  delay(10);
  if (isnan(humid) || isnan(temp))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print("Humid: ");
    Serial.print(humid);
 
  // Add to structured data object
    myData.a = temp;
    myData.b = humid;
    if(myData.a < 30)
    {
      init_Wifi();
      if(client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
      {  
         String postStr = apiKey;
         postStr +="&field1=";
         postStr += String(myData.a);
         postStr +="&field2=";
         postStr += String(myData.b);
         postStr += "\r\n\r\n";
 
         client.print("POST /update HTTP/1.1\n");
         client.print("Host: api.thingspeak.com\n");
         client.print("Connection: close\n");
         client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
         client.print("Content-Type: application/x-www-form-urlencoded\n");
         client.print("Content-Length: ");
         client.print(postStr.length());
         client.print("\n\n");
         client.print(postStr);
 
         Serial.print("Temperature: ");
         Serial.print(myData.a);
         Serial.print(" degrees Celcius, Humidity: ");
         Serial.print(myData.b);
         Serial.println("%. Send to Thingspeak.");
     }
     client.stop();
     Serial.println("Waiting...");
     delay(1000);
  }
  else
  {
    init_EspNow();
      // Send data
    esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
  }
}
}
