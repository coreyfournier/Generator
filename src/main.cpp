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
#include "SimpleWeb/IndexController.cpp"
#include "SimpleWeb/Router.h"
#include "SimpleWeb/IController.h"
#include "IO/PowerState.h"
#include "IO/Pin.cpp"
#include "States/Orchestration.cpp"
#include "States/Initial.cpp"
#include "IO/RtosIO.cpp"
#include "IO/RtosQueue.cpp"
#include "IO/RtosSerial.cpp"
#include "States/ChangeMessage.cpp"
#include "Devices/PowerDevice.cpp"
#include "Devices/StartableDevice.cpp"
#include "Devices/TransferSwitch.cpp"

using namespace std;
using namespace States;
using namespace IO;

// Replace with your network credentials set in the environment
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Set web server port number to 80
WiFiServer server(80);

// Assign output variables to GPIO pins
//https://learn.adafruit.com/assets/111179
const int TransferGPIO = 21;
const int StartGPIO = 27; //A10 
const int StopGPIO = 33; //A9 AMessage
const int L1OnSenseGpio = 15;
const int L2OnSenseGpio = 32;
const int GeneratorOnSenseGpio = 14;

Pin L1OnSense = Pin(L1OnSenseGpio, false, "Utility L1 on/off", true, PinRole::UtilityOnL1);
Pin L2OnSense = Pin(L2OnSenseGpio, false, "Utility L2 on/off", true, PinRole::UtilityOnL2);
Pin generatorL1OnSense = Pin(GeneratorOnSenseGpio, false, "Generator on/off", true, PinRole::GeneratorOnL1);
Pin transfer = Pin(TransferGPIO, false, "Transfer", PinRole::Transfer);
Pin genStart = Pin(StartGPIO, false, "Start Generator", PinRole::Start);
Pin genStop = Pin(StopGPIO, false, "Stop Generator", PinRole::Stop);
IO::RtosIO board = IO::RtosIO();

Devices::StartableDevice generator = Devices::StartableDevice(
  &generatorL1OnSense,
  nullptr,
  &genStart,
  &genStop,
  &board
);
Devices::PowerDevice utility = Devices::PowerDevice(&L1OnSense, &L2OnSense, &board);
Devices::TransferSwitch transferSwitch = Devices::TransferSwitch(&transfer, &board);

const gpio_int_type_t int_type = GPIO_INTR_ANYEDGE;

class EventStub : public IEvent
{
  public:
  void Change(Event e)
  {
    Serial.printf("Event=%s", IEvent::ToName(e).c_str());
  }
};

EventStub es = EventStub();

IO::RtosQueue queue = IO::RtosQueue();
IO::RtosSerial s = IO::RtosSerial();
Orchestration* view = new Orchestration( 
  &es, 
  &utility,
  &generator,
  &transferSwitch,
  &board,
  &queue,
  &s);

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
  

  /*NOTE: this needs to be moved into the board operations**/
  pinMode(StartGPIO, OUTPUT);
  digitalWrite(StartGPIO, LOW);
  pinMode(StopGPIO, OUTPUT);
  digitalWrite(StopGPIO, LOW);  
  pinMode(L1OnSense.gpio, INPUT);
  pinMode(L2OnSense.gpio, INPUT);
  pinMode(generatorL1OnSense.gpio, INPUT);

   /* Input pins */
  gpio_int_type_t tt;
  

  //gpio_set_intr_type((gpio_num_t)GeneratorOnSenseGPIO, GPIO_INTR_ANYEDGE);

    // gpio_config_t io_conf = {};
    // io_conf.intr_type = GPIO_INTR_ANYEDGE;
    // io_conf.mode = GPIO_MODE_INPUT;
    // io_conf.pin_bit_mask = BIT64(GPIO_NUM_21);
    // io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    // gpio_config(&io_conf);

    // esp_err_t  installResponse = gpio_install_isr_service(0);
    // if(installResponse == ESP_OK)
    // {
    //   gpio_isr_handler_add(GPIO_NUM_21, button_isr_handler, NULL);
    // }
    // else
    //   Serial.printf("Failed to install %i\n", installResponse);
    
  

  xTaskCreatePinnedToCore(
        WebsiteTaskHandler,   /* Task function. */
        "Task1",     /* name of task. */
        10000,       /* Stack size of task */
        NULL,        /* parameter of the task */
        2,           /* priority of the task */
        &webSiteTask,      /* Task handle to keep track of created task */
        0);          /* pin task to core 0 */  
  
  delay(500);

   
  xTaskCreatePinnedToCore(
        /* Task function. */
          [](void *params){ view->WaitAndListen(); },
          "Interupt Task Handler",     /* name of task. */
          10000,       /* Stack size of task */
          NULL,
          3,           /* priority of the task */
          NULL,      /* Task handle to keep track of created task */
          1);          /* pin task to core 1 */ 


  //Listen for the state changes
  attachInterrupt(digitalPinToInterrupt(generatorL1OnSense.gpio), generatorSenseChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L1OnSense.gpio), L1SenseChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L2OnSense.gpio), L2SenseChange, CHANGE);  

  view->PinCount();
}

void generatorSenseChange()
{
  view->PinChanged(generatorL1OnSense, true);
}

void L1SenseChange()
{
  view->PinChanged(L1OnSense, true);
}

void L2SenseChange()
{
  view->PinChanged(L2OnSense, true);
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

void loop(){}

#endif