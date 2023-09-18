/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/
#define ST(A) #A
#define STR(A) ST(A)

#include <WiFi.h>
#include <stdio.h>
#include <ArduinoJson.h>
// basic file operations
#include <iostream>
#include <fstream>
#include "FS.h"
#include "SPIFFS.h"
#include "GeneratorView.cpp"
#include "SimpleWeb/DataController.cpp"
#include "SimpleWeb/Router.h"
#include "SimpleWeb/IController.h"
#include <vector>

using namespace std;

// Replace with your network credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Set web server port number to 80
WiFiServer server(80);
SimpleWeb::Router router = SimpleWeb::Router(server);

// Assign output variables to GPIO pins
const int output26 = 26;

GeneratorView view = GeneratorView();


class IndexController : public SimpleWeb::IController
{
  void getIndex(WiFiClient& client)
  {
    fs::FS fs = SPIFFS;
    if(SPIFFS.exists("/index.html"))
    {
      File file =  fs.open ("/index.html");

      Serial.println("- read from file:");
      while(file.available())
      {
          client.write(file.read());
      }

      file.close();
    }
    else{
      client.println("Can't find index.html");
      Serial.println("Can't find index.html");
    }
              
    // The HTTP response ends with another blank line
    client.println();
  }

  bool Handler(WiFiClient& client, const String& header)
  {
    if(header.indexOf("GET / HTTP/1.1") >= 0)
    {
      // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
      // and a content-type so the client knows what's coming, then a blank line:
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println("Connection: close");
      client.println(); 
      
      getIndex(client);  

      return true;
    }

    return false;
  }
};

void setup() {
  Serial.begin(115200);
  
  //mount the file system
  SPIFFS.begin(true);

  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);

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
  view.pins.push_back(Pin(output26, false, "Start relay"));

  
  //Controllers must be placed in the order in which they should check the header
  router.AddController(new SimpleWeb::DataController(view));
  router.AddController(new IndexController());
}

void loop(){
  router.Check();
}

