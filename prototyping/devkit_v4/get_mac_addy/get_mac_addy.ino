#define cam 1 // for ESP32-CAM for disabling brownout triggering


//----------------------------------------Load libraries
#include "WiFi.h"
//----------------------------------------

// For ESP32-CAM
#ifdef cam
  #include "soc/soc.h"
  #include "soc/rtc_cntl_reg.h"
#endif


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ VOID SETUP
void setup() {
  #ifdef cam
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector being triggered
  #endif
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

}