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
/*
define WIFI_SSID and WIFI_PASSWORD in the config file.
*/
#include "config.h"
#include "GeneratorView.cpp"

using namespace std;

// Replace with your network credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
void getData(WiFiClient client, GeneratorView data);
void getIndex(WiFiClient client);

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

GeneratorView view = GeneratorView();

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
  while (WiFi.status() != WL_CONNECTED) {
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
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients  

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {         
            if(header.indexOf("POST /data") >= 0)
            {
              StaticJsonDocument<256> doc;
              String s = client.readStringUntil('\n');              
              DeserializationError error = deserializeJson(doc, s.c_str());   

              if (error) 
              {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());

                doc["success"] = false;
                doc["message"] = error.f_str();                

                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 500 Internal Server Error");
                client.println("Content-type:text/json");
                client.println("Connection: close");
                client.println();
              }
              else
              {
                if(doc["state"].as<bool>())
                  digitalWrite(doc["gpio"].as<int>(), HIGH);
                else
                  digitalWrite(doc["gpio"].as<int>(), LOW);

                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/json");
                client.println("Connection: close");
                client.println();
              }

              serializeJson(doc, client);              
            }
            else if(header.indexOf("GET /data") >= 0)
            {   
              StaticJsonDocument<200> doc;      
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/json");
              client.println("Connection: close");
              client.println(); 
              
              Serial.printf("data...");
              Serial.printf("pins=%i\n", sizeof(view.pins));

              for(int i=0; i< view.pins.size(); i++)
              {
                Pin pin = view.pins[i];
                Serial.printf("digitalRead...\n");
                pin.state = (digitalRead(pin.gpio) ==  HIGH);
                doc["pins"][i]["gpio"] = pin.gpio;
                doc["pins"][i]["state"] = (digitalRead(pin.gpio) ==  HIGH);
                doc["pins"][i]["name"] = pin.name;                
              }                            
              serializeJson(doc, client);
            }
            else if(header.indexOf("GET /") >= 0)
            {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println(); 
              getIndex(client);              
            }
            else
            {
              Serial.printf("unknown header");
            }
            // Break out of the while loop
            break;
          } 
          else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } 
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }  
}

void getIndex(WiFiClient client)
{
  fs::FS fs = SPIFFS;
  if(SPIFFS.exists("/index.html"))
  {
    File file =  fs.open ("/index.html");

    Serial.println("- read from file:");
    while(file.available()){
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

void getData(WiFiClient client, GeneratorView data)
{

  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
  client.println(".button-off {background-color: #555555;}");
  client.println(".button-on { background-color: #4CAF50;");
  client.println("</style>");
  client.println("</head>");
  
  // Web Page Heading
  client.println("<body><h1>ESP32 Web Server</h1>");
  
  
  // If the output26State is off, it displays the ON button       
  for(int i=0; i< sizeof(data.pins); i++)
  {
    Pin pin = data.pins[i];

    client.printf("<p>GPIO %i - %s </p>", pin.gpio, pin.name);

    client.printf(
      "<p><a href=\"/%i/%s\"><button class=\"button button-%s\">%s</button></a></p>", 
      pin.gpio, 
      pin.state ? "off" : "on",
      pin.state ? "off" : "on",
      pin.state ? "OFF" : "ON");
  }
  
  
  client.println("</body></html>");
            
  // The HTTP response ends with another blank line
  client.println();
}

