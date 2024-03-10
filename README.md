# Westinghouse Generator Controller via ESP32

Testing UI to confirm generator control works with the circuit board.
![POC UI while testing](/assets/PocUI.PNG "POC UI while testing")

Custom board to interface with Westinghouse WGen12000DF via transfer switch plug
![Custom Etched Board Front](/assets/Custom_Eteched_Board_Front.jpg "Custom Etched Board Front")

![Custom Etched Board Back](/assets/Custom_Eteched_Board_Back.jpg "Custom Etched Board Back")


# Setup environment
Install vs code
Install platformio plugin. 

## Native Unit Tests
You need to install mingw and set the bin folder to the path.
Download here: https://github.com/niXman/mingw-builds-binaries/releases
From the platform io widget Open Native, Open Advanced, and run Test.
If the code continues to be grayed out after a run, go to featheresp32 -> Miscellaneous and Rebuild IntelliSense Index.

## Wifi
set ENV_WIFI_PW and ENV_WIFI_SSID in your environment with the ssid and password

# Setting up to push code to the controller
Install the usb drivers
Follow directions here: https://bromleysat.com/installing-drivers-for-the-esp32

# To push any files modified in the "data" folder
CLick on Upload Filesystem image under Platfom in the PlatformIo plugin. For more information see:
https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/


# Pinout documentation
https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts

# PC817X octocoupler
https://www.farnell.com/datasheets/73758.pdf
