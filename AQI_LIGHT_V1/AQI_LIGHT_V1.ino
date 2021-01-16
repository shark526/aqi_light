/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/
#include <SimpleTimer.h>  //https://playground.arduino.cc/Code/SimpleTimer/
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

// Assign output variables to GPIO pins
#define redPin D1
#define greenPin D2
#define bluePin D0

SimpleTimer timer;
// Set web server port number to 80
WiFiServer server(80);

String pm25="";
int aqi=0;

// request for your own token here https://aqicn.org/data-platform/token/cn/, replace following "demo"
const char* locationHereAPIUrl = "http://api.waqi.info/feed/here/?token=demo";

void setLight()
{
  if(aqi<=50){
    //setColor(int red, int green, int blue)
    // green
    setColor(0, 255, 0);
  }
  else if (aqi<=100){
    // yellow
    setColor(255, 222, 51);
  }
  else if (aqi<=150){
    // orange
    setColor(255, 153, 51);
  }
  else if (aqi<=200){
    // red
    setColor(255, 0, 0);
  }
  else if (aqi<=300){
    // purple
    setColor(102, 0, 153);
  }
  else
  {
    // brown
    setColor(126, 0, 35);
  }
}

void GetAQI(){
  HTTPClient http;
  http.begin(locationHereAPIUrl );
  int httpCode = http.GET();
  if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      //Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
          DynamicJsonDocument doc(6144);

          DeserializationError err=deserializeJson(doc, payload);

          if(err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
          }
          //extract the pm2.5 values
          aqi = doc["data"]["aqi"];
          pm25=(String)(aqi);
      
          Serial.println("pm2.5 is :" + pm25);
          setLight();
      }
  } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  } 
  http.end();  
}

void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}

void setup() {
  Serial.begin(115200);
  
  // Initialize the output variables as outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  
  // Set outputs to LOW
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment and run it once, if you want to erase all the stored information
  //wifiManager.resetSettings();
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AQI_LIGHT");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  
  server.begin();

  testlight();
  GetAQI();
  timer.setInterval(600000L,GetAQI);//10 minute
}

void testlight()
{
  setColor(255, 0, 0);  // 红色
  delay(1000);
  setColor(0, 255, 0);  // 绿色
  delay(1000);
  setColor(0, 0, 255);  // 蓝色
  delay(1000);
  setColor(255, 255, 0);  // 黄色
  delay(1000);  
  setColor(80, 0, 80);  // 紫色
  delay(1000);
  setColor(0, 255, 255);  // 浅绿色
  delay(1000);
}

void loop(){
  timer.run();
}
