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
const gpio_int_type_t int_type = GPIO_INTR_ANYEDGE;

static QueueHandle_t tsqueue;

GeneratorView view = GeneratorView();
TaskHandle_t webSiteTask;
TaskHandle_t sensorTask;

void WebsiteTaskHandler(void * pvParameters );
void SensorTaskHandler(void * pvParameters );
void vHandlingTask(void * pvParameters );
void vANInterruptHandler( void );
void buttonIntTask(void *pvParameters);
void gpio_intr_handler();


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
  gpio_int_type_t tt;
  pinMode(L1SenseGPIO, INPUT);
  gpio_set_intr_type((gpio_num_t)L1SenseGPIO, GPIO_INTR_HIGH_LEVEL);
  pinMode(L2SenseGPIO, INPUT);
  gpio_set_intr_type((gpio_num_t)L2SenseGPIO, GPIO_INTR_HIGH_LEVEL);
  
  pinMode(GeneratorOnSenseGPIO, INPUT_PULLDOWN);
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
        1,           /* priority of the task */
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
  
  tsqueue = xQueueCreate(2, sizeof(uint32_t));

  xTaskCreatePinnedToCore(
      buttonIntTask,   /* Task function. */
      "Interupt Task Handler",     /* name of task. */
      10000,       /* Stack size of task */
      &tsqueue,        /* parameter of the task */
      2,           /* priority of the task */
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

void loop(){}


/* This task configures the GPIO interrupt and uses it to tell
   when the button is pressed.

   The interrupt handler communicates the exact button press time to
   the task via a queue.

   This is a better example of how to wait for button input!
*/
void buttonIntTask(void *pvParameters)
{
    printf("Waiting for button press interrupt on gpio %d...\r\n", GeneratorOnSenseGPIO);
    QueueHandle_t *tsqueue = (QueueHandle_t *)pvParameters;
    //gpio_set_interrupt(GeneratorOnSenseGPIO, int_type, gpio_intr_handler);    
    attachInterrupt(digitalPinToInterrupt(GeneratorOnSenseGPIO), gpio_intr_handler, CHANGE);

    uint32_t last = 0;
     while(1) {
        uint32_t button_ts;
        xQueueReceive(*tsqueue, &button_ts, portMAX_DELAY);
        button_ts *= portTICK_PERIOD_MS;
        if(last < button_ts-200) {
            printf("Button interrupt fired at %dms\r\n", button_ts);
            last = button_ts;
        }
    }
}

void gpio_intr_handler()
{
    uint32_t now = xTaskGetTickCountFromISR();
    xQueueSendToBackFromISR(tsqueue, &now, NULL);
}