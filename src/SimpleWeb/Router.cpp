#pragma once
#include <vector>
#include <WiFi.h>
#include "IController.h"
#include "Router.h"
using namespace std;

namespace SimpleWeb
{

    /*Checks for new clients and handles them*/
    void Router::Check()
    {
        WiFiClient client = _server.available();   // Listen for incoming clients  

        if (client) 
        { 
            String header;
            bool isHandled = false;                 
            currentTime = millis();
            previousTime = currentTime;
            Serial.println("New Client.");          // print a message out in the serial port
            String currentLine = "";                // make a String to hold incoming data from the client

            Serial.printf("Connected=%i\n", client.connected());

            while (client.connected() && currentTime - previousTime <= timeoutTime) // 
            {  
                // loop while the client's connected
                currentTime = millis();
                // if there's bytes to read from the client,

                //Serial.printf("available=%i\n", client.available());

                if (client.available()) 
                {             
                    char c = client.read();             // read a byte, then
                    //Serial.println(c);                    // print it out the serial monitor
                    header += c;
                    if (c == '\n') 
                    {                                  
                        //Serial.printf("header=%s", header.c_str());
                              // if the byte is a newline character
                        // if the current line is blank, you got two newline characters in a row.
                        // that's the end of the client HTTP request, so send a response:
                        if (currentLine.length() == 0) 
                        {                       
                            Serial.printf("Checking controllers Header=%s\n", header.c_str());

                            for(int i=0; i< _controllers.size(); i++)
                            {   
                                if(_controllers[i]->Handler(client, header.c_str()))
                                {
                                    isHandled = true;
                                    break;
                                }
                            }
                            
                            if(!isHandled)
                            {
                                Serial.printf("unknown header\n");
                            }
                            // Break out of the while loop
                            break;
                        } 
                        else 
                        { 
                            // if you got a newline, then clear currentLine
                            currentLine = "";
                        }
                    } 
                    else if (c != '\r') 
                    {  
                        // if you got anything else but a carriage return character,
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
}