/***********************************************************************************/
/* This is an simple application to illustrate IoT Temperature & Humidity Logger.  */
/* written by Prashanth from BSP Embed.                                            */   
/* This code is availbale from GitHub.                                             */ 
/* Demo is available on YouTube BSP Embed                                                   */
/* This is an example for our Monochrome OLEDs based on SSD1306 drivers            */
/*  Pick one up today in the adafruit shop!                                        */ 
/*  ------> http://www.adafruit.com/category/63_98                                 */
/* This example is for a 128x64 size display using I2C to communicate              */
/* 3 pins are required to interface (2 I2C and one reset)                          */
/* Adafruit invests time and resources providing this open source code,            */
/* please support Adafruit and open-source hardware by purchasing                  */
/* products from Adafruit!                                                         */
/* Written by Limor Fried/Ladyada for Adafruit Industries.                         */
/* BSD license, check license.txt for more information                             */
/* All text above, and the splash screen must be included in any redistribution    */ 
/************************************************************************************/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

#define OLED_RESET          4
#define SSD1306_LCDHEIGHT   64
Adafruit_SSD1306 display(OLED_RESET);

#define DHTTYPE             DHT11         
#define DHTPIN              4           /* What pin we're connected to */
#define Frequent            1           /* Frequency of upload in Minutes */

String apiKey         = "9FKQQUHY51BLPJ6M";   /* Replace with your thingspeak API key */
const char* ssid      = "WIFI_SSID";          /* Your Router's SSID */
const char* password  = "WIFI_PASSWORD";      /* Your Router's WiFi Password */
const char* server    = "api.thingspeak.com";

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

#if (SSD1306_LCDHEIGHT != 64)
  #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {         
  Wire.begin(0,2);                            /* I2C OLED SDA, SCL */
  Serial.begin(9600);
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  /* Initialize OLED */
  display.display();
  delay(250);
  ConnectAP();
  DispParam();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  display.setCursor(60,0);
  display.println(String(t));
  display.display();
  display.setCursor(60,18);
  display.println(String(h));
  display.display();
  if (client.connect(server,80)) {  
    String postStr = apiKey;
           postStr +="&field1=";
           postStr += String(t);
           postStr +="&field2=";
           postStr += String(h);
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
  }
  client.stop();
  ESP.deepSleep(Frequent * 60 * 1000000, WAKE_RF_DEFAULT);
}
/**************************************/
/* Connect to WiFi Router             */
/* (Access Point)                     */
/**************************************/
void ConnectAP(void) {
  WiFi.mode(WIFI_STA);    /* Set WiFi to station mode */
  WiFi.disconnect();     /* disconnect from an AP if it was Previously  connected*/
  delay(100);
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
 
  display.clearDisplay();                   /* For Display */
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Connecting");
  display.setCursor(0,18);
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    display.print('.');
    display.display();
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}
void DispParam(void) {
  display.clearDisplay();                   /* Clear the buffer */
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Temp:");
  display.setCursor(0,18);
  display.println("Humi:");
  display.display();
}
