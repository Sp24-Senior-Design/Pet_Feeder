<h2>Project Name:  Smart Pet Feeder</h2>
  Project website hosting project journals, documentation, media, and misc.: <a href="https://engineering.purdue.edu/477grp3" target="_blank">https://engineering.purdue.edu/477grp3</a>
			
<h2>Project Description:</h2> 
  Our project is the Smart Pet Feeder, with the purpose of helping pet owners who can't always be home to interact with or feed their pet (or maybe just forgetful owners). 
	Our device is programmed by the user through a simple LAN web server, allowing the programming of motors to control the amount of food released and also when it is released. 
	There is also a weight sensor to monitor the food level in the food bowl. 
	Our feeder also allows the owner to use one-way video to monitor their pet from a distance. 
	This video is streamed from a camera onto the web page in real-time over Wi-Fi. 
	In regards to power, our product will be safely powered by using a wall outlet and a voltage regulator to step down the voltage such that all of our peripherals can be powered correctly. 
	As a result, our product will make pet care more convenient for owners with our product.
<br><br>
Our stretch functionalities include:<br>
<ul>
  <li>Alerting pet owner when food reserve gets low
  <li>Accessing the web server from any network
  <li>Keeping memory of schedule when microcontroller resets
  <li>Including back-up batteries for the feeder
</ul>

<h2>Project Specific Design Requirements (PSDRs):</h2>  
  <ol>
    <li>PSDR #1 (Software): An ability to send data from the LAN web server hosted on the ESP32 to a web client over Wi-Fi and configure display of live video on the web page.</li> 
    <li>PSDR #2 (Software): An ability to receive data from a web client to store and program feeding schedule on the ESP32.</li>
    <li>PSDR #3 (Hardware): An ability for the microcontroller to interface with a stepper motor using a motor driver to control dispensing of food.</li>
    <li>PSDR #4 (Hardware): An ability to regulate voltage and current using a switching regulator from variable 12-20V rectifier to the consistent 3.3V needed for the ESP32.</li> 
    <li>PSDR #5 (Hardware): An ability to measure the amount of food in the food bowl using a load cell interfaced with an HX711 amplifier and ESP32 to be displayed on a web server.</li>
	</ol>
	<br>
	<ol>
    <li>Stretch PSDR #1 (Hardware): An ability to interface an infrared distance sensor with the ESP32 using I2C to measure the amount of food in the food reserve.</li>
  </ol>

<h2>Code:</h2>
Code for ESP32 on PCB (handles web server, motor, IR sensor, and load cell):<br>
<ul>
  <li>pcb_web_server_final/src/main.c
</ul>
Code for ESP32-CAM development board (handles another web server and camera):<br>
<ul>
  <li>camera/CameraWebServer/CameraWebServer.ino (programmed onto MCU)
  <li>camera/CameraWebServer/app_http.cpp (web server back-end)
  <li>camera/CameraWebServer/camera_index.h (web server front-end)
  <li>camera/CameraWebServer/camera_pins.h (configurable pins)
</ul>
<br>
We started with C/C++-based Arduino IDE to develop and prototype software/firmware and program the microcontroller.<br>
We ported the program for the ESP32 on PCB to the PlatiformIO IDE using the ESP-IDF framework to decrease overhead from Arduino's framework.
