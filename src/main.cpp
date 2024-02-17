/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/
#define ST(A) #A
#define STR(A) ST(A)

#include <WiFi.h>
#include <stdio.h>
#include <ArduinoJson.h>
#include "GeneratorView.cpp"
#include "SimpleWeb/DataController.cpp"
#include "SimpleWeb/IndexController.cpp"
#include "SimpleWeb/Router.h"
#include "SimpleWeb/IController.h"
#include <vector>


using namespace std;

// Replace with your network credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Set web server port number to 80
WiFiServer server(80);


// Assign output variables to GPIO pins
//https://learn.adafruit.com/assets/111179
const int TransferGPIO = 26; //A0
const int StartGPIO = 25; //A1
const int StopGPIO = 34; //A2
const int L1SenseGPIO = 16; //IO16
const int L2SenseGPIO = 17; //IO17
const int GeneratorOnSenseGPIO = 21; //IO21

GeneratorView view = GeneratorView();
TaskHandle_t webSiteTask;
TaskHandle_t sensorTask;

void WebsiteTaskHandler( void * pvParameters );
void SensorTaskHandler( void * pvParameters );


void setup() {
  Serial.begin(115200);
  
  //mount the file system
  SPIFFS.begin(true);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();  

  /*Configure all of the GPIO pins*/
  pinMode(TransferGPIO, OUTPUT);
  digitalWrite(TransferGPIO, LOW);
  view.pins.push_back(Pin(TransferGPIO, false, "Transfer"));

  pinMode(StartGPIO, OUTPUT);
  digitalWrite(StartGPIO, LOW);
  view.pins.push_back(Pin(StartGPIO, false, "Start Generator"));

  pinMode(StopGPIO, OUTPUT);
  digitalWrite(StopGPIO, LOW);
  view.pins.push_back(Pin(StopGPIO, false, "Stop Generator"));

  /* Input pins */
  pinMode(L1SenseGPIO, INPUT);
  pinMode(L2SenseGPIO, INPUT);
  pinMode(GeneratorOnSenseGPIO, INPUT);
  

  xTaskCreatePinnedToCore(
        WebsiteTaskHandler,   /* Task function. */
        "Task1",     /* name of task. */
        10000,       /* Stack size of task */
        NULL,        /* parameter of the task */
        1,           /* priority of the task */
        &webSiteTask,      /* Task handle to keep track of created task */
        0);          /* pin task to core 0 */  
  
  delay(500);

  xTaskCreatePinnedToCore(
      SensorTaskHandler,   /* Task function. */
      "SensorTask",     /* name of task. */
      10000,       /* Stack size of task */
      NULL,        /* parameter of the task */
      1,           /* priority of the task */
      &sensorTask,      /* Task handle to keep track of created task */
      1);          /* pin task to core 1 */  
}



void WebsiteTaskHandler(void * pvParameters)
{
  Serial.println("Website task running on core ");
  Serial.println(xPortGetCoreID());
  SimpleWeb::Router router = SimpleWeb::Router(server);
  Serial.println("Router setup ");
  
  //Controllers must be placed in the order in which they should check the header
  router.AddController(new SimpleWeb::DataController(view));
  router.AddController(new SimpleWeb::IndexController());
  
  Serial.println("Router done ");

  while(true)
  {
    router.Check();
    //With out the delay it crashes???? idk
    delay(5);
  }
}

void SensorTaskHandler( void * pvParameters ){
  Serial.println("Sensor task running on core ");
  Serial.println(xPortGetCoreID());
  
  while(true)
  {
    delay(4000);
    //Not sure why A1 isn't 25? or maybe it is???
    Serial.printf("L1=%i\n", digitalRead(L1SenseGPIO));    
    Serial.printf("L2=%i\n", digitalRead(L2SenseGPIO));    
    Serial.printf("Gen On=%i\n", digitalRead(GeneratorOnSenseGPIO));    
  } 
}

int lastValue = 0;
unsigned long lastChange = 0;
bool isClimbing = false;

void loop()
{
  /*
  float frequency = 0;
  float period = 0;
  //auto temp = analogReadMilliVolts(A2);
  auto temp = digitalRead(A2);

  Serial.printf("A2=%i\n", temp);

  delay(2000);
  lastValue = temp;
*/
}