# Westinghouse Generator Controller via ESP32

Testing UI to confirm generator control works with the circuit board.
![POC UI while testing](/assets/PocUI.PNG "POC UI while testing")

Custom board to interface with Westinghouse WGen12000DF via transfer switch plug
![Custom Etched Board Front](/assets/Custom_Eteched_Board_Front.jpg "Custom Etched Board Front")

![Custom Etched Board Back](/assets/Custom_Eteched_Board_Back.jpg "Custom Etched Board Back")

#TODO / Test Results 
02/19/2024
* Generator produces 19v AC, was expecting closer to 10. This fries the optocoupler because the voltage is too high. You can see that the LED is also brighter because of this. Need some way to lower the voltage
* When the generator turns on the readings for L1 and L2 start start flipping as if there is some kind of interfearence. Upon futher inspection all of the optocouplers get cooked when it started up
* Only the start lead is need to start the generator. It must be momentary, one second seems to work. If left connected the fuse for the starter blows.

2/22/24
Need to vhance enum to represent startup and shut down stages. use queue to allow asynchronous execution and delay for state confirmation.

2/27/24
Notes for state change classes
Pin change
	Utility is off
	State do action Utility Off
		change state, wait to confirm utility is off
Utility off wait
	State do action utility off wait	
		confirm utility is still off
		change state, start generator
Start Generator 
	State do action Start generator
		Start generator....
		change state Starting generator Wait
Starting Generator Wait
	State do action starting generator
		is generator started?
		change state Generator warming Up Wait
Generator Warming Up Wait
	state do action Generator Warming up wait
	.......
		
	



# Setup environment
Install vs code
Install platformio plugin. 
## Native Unit Tests
You need to install mingw and set the bin folder to the path.
Download here: https://github.com/niXman/mingw-builds-binaries/releases

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
