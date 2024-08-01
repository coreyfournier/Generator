#ifndef PIO_UNIT_TESTING

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
#include "IO/BoardTemp.cpp"
#include "States/ChangeMessage.cpp"
#include "States/PinChange.cpp"
#include "Devices/PowerDevice.cpp"
#include "Devices/StartableDevice.cpp"
#include "Devices/TransferSwitch.cpp"


#include <WiFiUdp.h>
#include <Syslog.h>

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
const int StartGPIO = 27;
const int StopGPIO = 27; 
const int L1OnSenseGpio = 15;
const int L2OnSenseGpio = 32;
const int GeneratorOnSenseGpio = 14;
int led = LED_BUILTIN;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udpClient;
Syslog* syslog = nullptr;

WiFiServer server(WebServerPort);

Pin* L1OnSense = new Pin(L1OnSenseGpio, false, "Utility L1 on/off", true, PinRole::UtilityOnL1);
Pin* L2OnSense = nullptr; //Pin(L2OnSenseGpio, false, "Utility L2 on/off", true, PinRole::UtilityOnL2);
Pin* generatorL1OnSense = new Pin(GeneratorOnSenseGpio, false, "Generator on/off", true, PinRole::GeneratorOnL1);
Pin* transfer = new Pin(TransferGPIO, false, "Transfer", PinRole::Transfer);
Pin* genStart = new Pin(StartGPIO, false, "Start Generator", PinRole::Start);
Pin* genStop = new Pin(StopGPIO, false, "Stop Generator", PinRole::Stop);
IO::RtosIO* board = new IO::RtosIO();
IO::BoardTemp boardTemp = IO::BoardTemp();
IO::RtosSerial* serialOutput = nullptr;
Orchestration* view;
IO::RtosQueue<States::ChangeMessage>* stateQueue;
IO::RtosQueue<States::PinChange>* pinQueue;
//Generator device
Devices::StartableDevice* generator;
Devices::PowerDevice* utility;
Devices::TransferSwitch* transferSwitch;


TaskHandle_t webSiteTask;
TaskHandle_t sensorTask;

void WebsiteTaskHandler(void * pvParameters );
void generatorSenseChange();
void L1SenseChange();
void L2SenseChange();

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  //Solid red when no longer connected
  digitalWrite(led, HIGH);

  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid, password);
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Connected to AP successfully!");
}

void setup() {
  Serial.begin(115200);

  //Enable the logging if the server is specified
  #ifdef LOG_SERVER_NAME
    // Create a new syslog instance with LOG_KERN facility
    syslog = new Syslog(udpClient, 
      LOG_SERVER_NAME, 
      (uint16_t)LOG_SERVER_PORT, 
      "genset", 
      "generator-control", 
      LOG_INFO, 
      SYSLOG_PROTO_BSD);
  #endif

  serialOutput = new IO::RtosSerial(syslog);
  stateQueue = new IO::RtosQueue<States::ChangeMessage>(serialOutput);
  pinQueue = new IO::RtosQueue<States::PinChange>(serialOutput, 30);
  
  //mount the file system
  SPIFFS.begin(true);

  if(ssid == nullptr)
    Serial.print("Wifi SSID not provided, will not connect or run the webserver.");
  else
  {
    WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

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
    serialOutput->Println("");
    serialOutput->Println("WiFi connected.");
    serialOutput->Println(IO::string_format("IP address: ", WiFi.localIP()));
    server.begin();  
  }
  

  /*Configure all of the GPIO pins*/
  board->SetPinMode(TransferGPIO, OUTPUT, false);
  board->SetPinMode(StartGPIO, OUTPUT, false);
  board->SetPinMode(StopGPIO, OUTPUT, false);
  board->SetPinMode(L1OnSense->gpio, INPUT);
  if(L2OnSense != nullptr) 
    board->SetPinMode(L2OnSense->gpio, INPUT);
  board->SetPinMode(generatorL1OnSense->gpio, INPUT);

  //Generator device
  generator = new Devices::StartableDevice(
    generatorL1OnSense,
    nullptr,
    genStart,
    genStop,
    board,
    GeneratorUsesMomentarySwitch
  );
  utility = new Devices::PowerDevice(L1OnSense, board);
  transferSwitch = new Devices::TransferSwitch(transfer, board, serialOutput);

  view = new Orchestration( 
    utility,
    generator,
    transferSwitch,
    board,
    stateQueue,
    pinQueue,
    serialOutput);    

  xTaskCreate(
        WebsiteTaskHandler,   /* Task function. */
        "Website task",     /* name of task. */
        10000,       /* Stack size of task */
        NULL,        /* parameter of the task */
        1,           /* priority of the task */
        &webSiteTask      /* Task handle to keep track of created task */
        );
  
  delay(500);

  xTaskCreate(
        /* Task function. */
          [](void *params){ view->WaitAndListenForPinChanges(); },
          "Interupt Task Handler",     /* name of task. */
          10000,       /* Stack size of task */
          NULL,
          3,           /* priority of the task */
          NULL      /* Task handle to keep track of created task */
          );
   
  xTaskCreate(
        /* Task function. */
          [](void *params){ view->WaitAndListenForStateChanges(); },
          "State Task Handler",     /* name of task. */
          10000,       /* Stack size of task */
          NULL,
          2,           /* Make sure we can handle a flood of tasks faster than they can be produced */
          NULL      /* Task handle to keep track of created task */
          );


  //Listen for the state changes
  attachInterrupt(digitalPinToInterrupt(generatorL1OnSense->gpio), generatorSenseChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L1OnSense->gpio), L1SenseChange, CHANGE);
  if(L2OnSense != nullptr)
    attachInterrupt(digitalPinToInterrupt(L2OnSense->gpio), L2SenseChange, CHANGE);  

  // set LED to be an output pin
  pinMode(led, OUTPUT);

  boardTemp.Setup();
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
    board->TaskDelay(5);
  }
}

void loop(){
  long rssi = WiFi.RSSI();
  serialOutput->Println(IO::string_format("readTemp1=%.1f readTemp2=%.1f wifiSignal=%i" , boardTemp.ToFahrenheit(boardTemp.ReadTemp1()), boardTemp.ToFahrenheit(boardTemp.ReadTemp2()), rssi));
  
  //Blink when connected to wifi
  for(int i = 0; i < 2000; i++)
  {
    if(WiFi.status() == WL_CONNECTED)
    {
      //Blink the LED to let me know it's still running
      digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000); 
      digitalWrite(led, LOW);     
      delay(500); 
    }
  }  
}
#endif