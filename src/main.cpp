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
#include "PowerState.cpp"
#include <vector>


using namespace std;

// Replace with your network credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Set web server port number to 80
WiFiServer server(80);


// Assign output variables to GPIO pins
//https://learn.adafruit.com/assets/111179
const int TransferGPIO = 21;
const int StartGPIO = 27; //A10 
const int StopGPIO = 33; //A9 
Pin L1OnSense = Pin(15, false, "Utility L1 on/off", true);
Pin L2OnSense = Pin(32, false, "Utility L2 on/off", true);
Pin generatorOnSense = Pin(14, false, "Generator on/off", true);
const gpio_int_type_t int_type = GPIO_INTR_ANYEDGE;

GeneratorView view = GeneratorView();
PowerState powerState = PowerState();
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

  view.pins.push_back(L1OnSense);
  view.pins.push_back(L2OnSense);
  view.pins.push_back(generatorOnSense);

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
          PowerState::StateChangeTaskHandler,   /* Task function. */
          "Interupt Task Handler",     /* name of task. */
          10000,       /* Stack size of task */
          &powerState.tsqueue,
          2,           /* priority of the task */
          NULL,      /* Task handle to keep track of created task */
          1);          /* pin task to core 1 */ 

  //Listen for the state changes
  attachInterrupt(digitalPinToInterrupt(generatorOnSense.gpio), generatorSenseChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L1OnSense.gpio), L1SenseChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L2OnSense.gpio), L2SenseChange, CHANGE);
  
}

void generatorSenseChange()
{
    uint32_t now = xTaskGetTickCountFromISR();
    AMessage pxPointerToxMessage;

    pxPointerToxMessage.pin = &generatorOnSense;
    pxPointerToxMessage.time = now;
    xQueueSendToBackFromISR(powerState.tsqueue, (void *)&pxPointerToxMessage, NULL);
}

void L1SenseChange()
{
    uint32_t now = xTaskGetTickCountFromISR();
    AMessage pxPointerToxMessage;

    pxPointerToxMessage.pin = &L1OnSense;
    pxPointerToxMessage.time = now;
    xQueueSendToBackFromISR(powerState.tsqueue, (void *)&pxPointerToxMessage, NULL);
}

void L2SenseChange()
{
    uint32_t now = xTaskGetTickCountFromISR();
    AMessage pxPointerToxMessage;

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