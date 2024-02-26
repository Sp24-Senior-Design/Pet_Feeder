//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"

// for disabling brownout triggering
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// #define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
// const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ESP32-BT-Slave";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void setup() {
  // for disabling brownout triggering    
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector being triggered
  
  Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  // #ifdef USE_PIN
  //   SerialBT.setPin(pin);
  //   Serial.println("Using PIN");
  // #endif
}

void loop() {
  SerialBT.print("hello");
  // for tramsiting data
  if (Serial.available()) {
    // SerialBT.write(Serial.read());
    //SerialBT.print("hello");
  }
  // for receiving data
  if (SerialBT.available()) {
    //Serial.write(SerialBT.read());
    int message = SerialBT.read();
    Serial.println(message);
  }
  delay(5000); // 5 seconds
}
