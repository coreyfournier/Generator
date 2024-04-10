# Westinghouse WGen12000DF Generator and Generac ATS Controller via ESP32 Feather

I live in an older neighborhood that is in a growing area of town. In the past few years the number of power outages has grown with the population. Given that we don't have access to natural gas, and I already had purchased WGen12000DF, I set out to see if I could automate the starting, stopping, and transfering to and from utility power. This isn't a complete step by step guide of how to exactly replicate what I did, but should serve as a sufficient starting part for the next dude.

Check out the [video](https://www.youtube.com/watch?v=OCHMWXXSAaE) of the first automated startup and transfer. 

You can find all of the items I purchased in the [shopping list](/ShoppingList.md)

## Control box
![Custom Control Box](/assets/Controller-Box.jpg "Custom Control Box")

## Custom Board
Back of the custom board. As you can see I wired an optocouptler backwards and had to reverse the wires.
![Back of custom board](/assets/Circuit-Board-Back.jpg "Back of custom board")

Front of board
![Custom Etched Board Front](/assets/Custom_Eteched_Board_Front.jpg "Custom Etched Board Front")


## Control UI
UI shows the current state of the sensing (Utility and Generator power) and last events that occurred. Also allows manual controlling of the transfer, starting, and stopping of the generator. Web app requires Wifi connection. Mine has a battery backup so it always statys connected.
![Controller UI](/assets/Controller-UI.png "Controller UI")

Web endpoints can be found at: [/state](/src/SimpleWeb/StateController.cpp) and [/data](/src/SimpleWeb/DataController.cpp)

## Automation Actions
All actions are [implemented](/src/States/IState.h) using a State Machine design pattern. You can see the state actions below in the diagram. Not all states have a specific class. To reduce the complexity and code some states in the diagram are sub states or events in the code.
![State Diagram](/assets/StateDiagram.png "State Diagram")

## Code
Application is written in c++ and the web interface is a very [simple html page](/data/index.html) with jQuery communicating to the backend.

# Setup environment
1. Install vs code
1. Install platformio plugin. 

## Wifi and Website
Set the following environment variables on your computer with the ssid and password of your wifi. 
* ENV_WIFI_PW 
* ENV_WIFI_SSID 

Once the controller starts up it will print out the IP for the website. You can also identify it on your router. If you are here, you should know how to do this.

## Setting up to push code to the controller
Install the usb drivers
Follow directions here: https://bromleysat.com/installing-drivers-for-the-esp32

## Deploying
### Web page or any modified in the "data" folder
Click on Upload Filesystem image under Platfom in the PlatformIo plugin. For more information see:
https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/


### Deploy Code
It will use a different task, but the same port as the previous instructions. You will need to delete the task to deploy the code and monitor it.
![Deploy](/assets/UploadAndMonitor.PNG "Deploy")

## Configure Project
Currently the only way to config the project is making changes to the gpio pins in [main](/src/main.cpp) and [config](/src/config.h). Once the changes are made you can then Upload and Monitor in the Platform IO Tasks.


# Native Unit Tests
You need to install mingw and set the bin folder to the path.
Download here: https://github.com/niXman/mingw-builds-binaries/releases
From the platform io widget Open Native, Open Advanced, and run Test.
If the code continues to be grayed out after a run, go to featheresp32 -> Miscellaneous and Rebuild IntelliSense Index.

# General Notes
## Westinghouse Generator
Pin connector for the generator: https://www.amazon.com/MECCANIXITY-Aviation-Connector-Terminals-Waterproof/dp/B09BMYB9Y4/

Reddit post with pin diagrams.
https://www.reddit.com/r/OffGrid/comments/mxygik/westinghouse_generator_automatic_transfer_switch/

## ESP32 Feather Pinout documentation
https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts

## PC817X octocoupler
https://www.farnell.com/datasheets/73758.pdf

## Generac 200amp ATS notes
![Custom Etched Board Front](/assets/Generac-ATC-pins.jpg "Custom Etched Board Front")
### Pin out
* T1 120v AC for battery charger. Needs neutral that's not supplied by the control module.
* N1 120v AC utility sense. Needs neutral that's not supplied by the control module.
* N2 120v AC utility sense. Needs neutral that's not supplied by the control module.
* N1 + N2 = 240v AC.
* 0 Ground is neg 12v DC from generator.
* 194 is positive 12v DC from generator.
* 23 Transfer, causes a transfer to utility when:
  * generator is running and connected to ATS.
  * 0 ground -12v DC is connected to 23.
  * +12v DC is supplied to 194.
  * I wasn't able to trigger the transfer via optocoupler. I could hear a small click in the ATS, but it never fully engaged. It only worked via relay.

