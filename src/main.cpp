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
IO::RtosIO board = IO::RtosIO();
IO::RtosQueue queue = IO::RtosQueue();
IO::RtosSerial s = IO::RtosSerial();
Orchestration view = Orchestration( 
  &es, 
  &board,
  &queue,
  &s);
IO::PowerState powerState = IO::PowerState();
//powerState.RegisterListner(*view);

TaskHandle_t webSiteTask;
TaskHandle_t sensorTask;

void WebsiteTaskHandler(void * pvParameters );
void SensorTaskHandler(void * pvParameters );
void vHandlingTask(void * pvParameters );
void vANInterruptHandler( void );
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

  Pin L1OnSense = Pin(L1OnSenseGpio, false, "Utility L1 on/off", true, PinRole::UtilityOnL1);
  Pin L2OnSense = Pin(L2OnSenseGpio, false, "Utility L2 on/off", true, PinRole::UtilityOnL2);
  Pin generatorOnSense = Pin(GeneratorOnSenseGpio, false, "Generator on/off", true, PinRole::GeneratorOnL1);
  view.AddPin(L1OnSense);
  view.AddPin(L2OnSense);
  view.AddPin(generatorOnSense);

  /*Configure all of the GPIO pins*/
  pinMode(TransferGPIO, OUTPUT);
  digitalWrite(TransferGPIO, LOW);
  
  view.AddPin(Pin(TransferGPIO, false, "Transfer", PinRole::Transfer));

  pinMode(StartGPIO, OUTPUT);
  digitalWrite(StartGPIO, LOW);
  view.AddPin(Pin(StartGPIO, false, "Start Generator", PinRole::Start));

  pinMode(StopGPIO, OUTPUT);
  digitalWrite(StopGPIO, LOW);
  view.AddPin(Pin(StopGPIO, false, "Stop Generator", PinRole::Stop));

   /* Input pins */
  gpio_int_type_t tt;
  
  pinMode(L1OnSense.gpio, INPUT);
  pinMode(L2OnSense.gpio, INPUT);
  pinMode(generatorOnSense.gpio, INPUT);
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

  // xTaskCreatePinnedToCore(
  //     SensorTaskHandler,   /* Task function. */
  //     "SensorTask",     /* name of task. */
  //     10000,       /* Stack size of task */
  //     NULL,        /* parameter of the task */
  //     1,           /* priority of the task */
  //     &sensorTask,      /* Task handle to keep track of created task */
  //     1);          /* pin task to core 1 */
   
  //AMessage xMessage;

  //tsqueue = xQueueCreate(4, sizeof(xMessage));
   
  xTaskCreatePinnedToCore(
          IO::PowerState::StateChangeTaskHandler,   /* Task function. */
          "Interupt Task Handler",     /* name of task. */
          10000,       /* Stack size of task */
          &powerState.tsqueue,
          2,           /* priority of the task */
          NULL,      /* Task handle to keep track of created task */
          1);          /* pin task to core 1 */ 

  xTaskCreatePinnedToCore(     
      //[](const Bar & first, const Bar & second) { return ...; }     
          [](void *params){ view.StateWaiter(); },   /* Task function. */
          "Change Task Handler",     /* name of task. */
          10000,       /* Stack size of task */
          NULL,
          3,           /* priority of the task */
          NULL,      /* Task handle to keep track of created task */
          1);          /* pin task to core 1 */ 

  //Listen for the state changes
  attachInterrupt(digitalPinToInterrupt(generatorOnSense.gpio), generatorSenseChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L1OnSense.gpio), L1SenseChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L2OnSense.gpio), L2SenseChange, CHANGE);
  
  view.Initalize();
}

void generatorSenseChange()
{
    uint32_t now = xTaskGetTickCountFromISR();
    IO::AMessage pxPointerToxMessage;

    pxPointerToxMessage.pin = &generatorOnSense;
    pxPointerToxMessage.time = now;
    xQueueSendToBackFromISR(powerState.tsqueue, (void *)&pxPointerToxMessage, NULL);
}

void L1SenseChange()
{
    uint32_t now = xTaskGetTickCountFromISR();
    IO::AMessage pxPointerToxMessage;

    pxPointerToxMessage.pin = &L1OnSense;
    pxPointerToxMessage.time = now;
    xQueueSendToBackFromISR(powerState.tsqueue, (void *)&pxPointerToxMessage, NULL);
}

void L2SenseChange()
{
    uint32_t now = xTaskGetTickCountFromISR();
    IO::AMessage pxPointerToxMessage;

    pxPointerToxMessage.pin = &L2OnSense;
    pxPointerToxMessage.time = now;
    xQueueSendToBackFromISR(powerState.tsqueue, (void *)&pxPointerToxMessage, NULL);
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