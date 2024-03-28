#ifndef PIO_UNIT_TESTING

/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/
#define ST(A) #A
#define STR(A) ST(A)

#include <WiFi.h>
#include <stdio.h>
#include "Arduino.h"
#include "SimpleWeb/DataController.cpp"
#include "SimpleWeb/StateController.cpp"
#include "SimpleWeb/IndexController.cpp"
#include "SimpleWeb/Router.h"
#include "SimpleWeb/IController.h"
#include "IO/PowerState.h"
#include "IO/Pin.cpp"
#include "States/Orchestration.cpp"
#include "IO/RtosIO.cpp"
#include "IO/RtosQueue.cpp"
#include "IO/RtosSerial.cpp"
#include "States/ChangeMessage.cpp"
#include "States/PinChange.cpp"
#include "Devices/PowerDevice.cpp"
#include "Devices/StartableDevice.cpp"
#include "Devices/TransferSwitch.cpp"

using namespace std;
using namespace States;
using namespace IO;

// Replace with your network credentials set in the environment
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const int WebServerPort = 80;
// Assign output variables to GPIO pins
//https://learn.adafruit.com/assets/111179
const int TransferGPIO = 12;
const int StartGPIO = 27; //A10 
const int StopGPIO = 33; //A9 AMessage
const int L1OnSenseGpio = 15;
const int L2OnSenseGpio = 32;
const int GeneratorOnSenseGpio = 14;


WiFiServer server(WebServerPort);

Pin* L1OnSense = new Pin(L1OnSenseGpio, false, "Utility L1 on/off", true, PinRole::UtilityOnL1);
Pin* L2OnSense = nullptr; //Pin(L2OnSenseGpio, false, "Utility L2 on/off", true, PinRole::UtilityOnL2);
Pin* generatorL1OnSense = new Pin(GeneratorOnSenseGpio, false, "Generator on/off", true, PinRole::GeneratorOnL1);
Pin* transfer = new Pin(TransferGPIO, false, "Transfer", PinRole::Transfer);
Pin* genStart = new Pin(StartGPIO, false, "Start Generator", PinRole::Start);
Pin* genStop = new Pin(StopGPIO, false, "Stop Generator", PinRole::Stop);
IO::RtosIO board = IO::RtosIO();
IO::RtosQueue<States::ChangeMessage> stateQueue = IO::RtosQueue<States::ChangeMessage>();
IO::RtosQueue<States::PinChange> pinQueue = IO::RtosQueue<States::PinChange>();
IO::RtosSerial serialOutput = IO::RtosSerial();

Devices::StartableDevice generator = Devices::StartableDevice(
  generatorL1OnSense,
  nullptr,
  genStart,
  genStop,
  &board
);
Devices::PowerDevice utility = Devices::PowerDevice(L1OnSense, &board);
Devices::TransferSwitch transferSwitch = Devices::TransferSwitch(transfer, &board, &serialOutput);

Orchestration* view = new Orchestration( 
  &utility,
  &generator,
  &transferSwitch,
  &board,
  &stateQueue,
  &pinQueue,
  &serialOutput);

TaskHandle_t webSiteTask;
TaskHandle_t sensorTask;

void WebsiteTaskHandler(void * pvParameters );
void generatorSenseChange();
void L1SenseChange();
void L2SenseChange();


void setup() {
  Serial.begin(115200);
  
  //mount the file system
  SPIFFS.begin(true);

  if(ssid == nullptr)
    Serial.print("Wifi SSID not provided, will not connect or run the webserver.");
  else
  {
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
  }

  /*Configure all of the GPIO pins*/
  board.SetPinMode(TransferGPIO, OUTPUT, false);
  board.SetPinMode(StartGPIO, OUTPUT, false);
  board.SetPinMode(StopGPIO, OUTPUT, false);
  board.SetPinMode(L1OnSense->gpio, INPUT);
  if(L2OnSense != nullptr) board.SetPinMode(L2OnSense->gpio, INPUT);
  board.SetPinMode(generatorL1OnSense->gpio, INPUT);
    

  xTaskCreate(
        WebsiteTaskHandler,   /* Task function. */
        "Website task",     /* name of task. */
        10000,       /* Stack size of task */
        NULL,        /* parameter of the task */
        1,           /* priority of the task */
        &webSiteTask      /* Task handle to keep track of created task */
        );
  
  delay(500);

  xTaskCreatePinnedToCore(
        /* Task function. */
          [](void *params){ view->WaitAndListenForPinChanges(); },
          "Interupt Task Handler",     /* name of task. */
          10000,       /* Stack size of task */
          NULL,
          2,           /* priority of the task */
          NULL,      /* Task handle to keep track of created task */
          1);          /* pin task to core 1 */ 
   
  xTaskCreatePinnedToCore(
        /* Task function. */
          [](void *params){ view->WaitAndListenForStateChanges(); },
          "State Task Handler",     /* name of task. */
          10000,       /* Stack size of task */
          NULL,
          3,           /* priority of the task */
          NULL,      /* Task handle to keep track of created task */
          1);          /* pin task to core 1 */ 


  //Listen for the state changes
  attachInterrupt(digitalPinToInterrupt(generatorL1OnSense->gpio), generatorSenseChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L1OnSense->gpio), L1SenseChange, CHANGE);
  if(L2OnSense != nullptr)
    attachInterrupt(digitalPinToInterrupt(L2OnSense->gpio), L2SenseChange, CHANGE);  
}

void generatorSenseChange()
{
  view->PinChanged(*generatorL1OnSense, true);
}

void L1SenseChange()
{
  view->PinChanged(*L1OnSense, true);
}

void L2SenseChange()
{
  view->PinChanged(*L2OnSense, true);
}

void WebsiteTaskHandler(void * pvParameters)
{
  Serial.println("Website task running on core ");
  Serial.println(xPortGetCoreID());
  SimpleWeb::Router router = SimpleWeb::Router(server);
  Serial.println("Router setup ");
  
  //Controllers must be placed in the order in which they should check the header
  router.AddController(new SimpleWeb::DataController(view));
  router.AddController(new SimpleWeb::StateController(view));
  router.AddController(new SimpleWeb::IndexController());
  
  Serial.println("Router done ");

  while(true)
  {
    router.Check();
    //With out the delay it crashes???? idk
    board.TaskDelay(5);
  }
}

void loop(){}

#endif