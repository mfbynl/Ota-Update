/* Nodemcu8266 uzaktan yazılım kontrolü ve güncelleme
  
  Bu uygulamada nodemcu8266 kartını uzaktan yazılım versiyonunu kontrol edebiliyor,
  yeni yazılım geldiğinde de güncelleyebiliyoruz.
  
  mfbynl-no masters no slaves
  05.07.2022
*/



#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

 
const String FirmwareVer={"0.2"}; 
#define URL_fw_Version "/media/esp8266firmware/ESP8266/version.txt"
#define URL_fw_Bin "iothook.com/media/esp8266firmware/ESP8266/firmware.bin"
const char* host = "iothook.com";
const int httpsPort = 80;


const char* ssid = "****";
const char* password = "*****";


  
void FirmwareUpdate()
{  
  WiFiClient client;
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }
  client.print(String("GET ") + URL_fw_Version + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      //Serial.println("Headers received");
      break;
    }
  }
  String payload = client.readStringUntil('\n');

  payload.trim();
  if(payload.equals(FirmwareVer) )
  {   
     Serial.println("Cihaz yazilimi güncel"); 
  }
  else
  {
    Serial.println("Yeni yazilim bulundu");
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW); 
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "iothook.com", 80, "/media/esp8266firmware/ESP8266/firmware.bin");
        
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    } 
  }
 }  
void connect_wifi();
unsigned long previousMillis_2 = 0;
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 10000;
const long mini_interval = 1000;
 void repeatedCall(){
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillis) >= interval) 
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      
      FirmwareUpdate();
    }

    if ((currentMillis - previousMillis_2) >= mini_interval) {
      static int idle_counter=0;
      previousMillis_2 = currentMillis;    
      Serial.print(" Active fw version:");
      Serial.println(FirmwareVer);
      Serial.print("Idle Loop....");
      Serial.println(idle_counter++);
     if(idle_counter%2==0)
      digitalWrite(LED_BUILTIN, HIGH);
     else 
      digitalWrite(LED_BUILTIN, LOW);
     if(WiFi.status() == !WL_CONNECTED) 
          connect_wifi();
   }
 }

  
void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start");
  WiFi.mode(WIFI_STA);
  connect_wifi();  
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(5, OUTPUT);
  
}
void loop()
{
  repeatedCall();
  digitalWrite(5,HIGH);
  delay(100);
  digitalWrite(5,LOW);
  delay(100);
      
}

void connect_wifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("O");
  }                                   
  Serial.println("Connected to WiFi");
}
