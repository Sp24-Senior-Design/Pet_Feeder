#include "esp_camera.h"
#include <WiFi.h>

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15 
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// #define CONFIG_RINGBUF_PLACE_ISR_FUNCTIONS_INTO_FLASH
// #undef  CONFIG_ESP_WIFI_IRAM_OPT
// #undef CONFIG_ESP_WIFI_RX_IRAM_OPT
// #undef CONFIG_SPI_FLASH_ROM_DRIVER_PATCH 
// #undef CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR
// #undef CONFIG_SPI_MASTER_ISR_IN_IRAM
// #undef CONFIG_SPI_SLAVE_ISR_IN_IRAM
// #define CONFIG_HAL_DEFAULT_ASSERTION_LEVEL 1
// #define CONFIG_HEAP_PLACE_FUNCTION_INTO_FLASH
// #define CONFIG_ESP32_REV_MIN 3

#define CONFIG_ESP_SYSTEM_ESP32_SRAM1_REGION_AS_IRAM

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
//#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD
//#define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
//#define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"

// Includes for disabling brownout detector being triggered
// #include "soc/soc.h"
// #include "soc/rtc_cntl_reg.h"

// ================================================================================================================================================
// For S2SBT
#include "BluetoothSerial.h"

// #define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
// const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "ESP32-BT-Slave";

// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

// #if !defined(CONFIG_BT_SPP_ENABLED)
// #error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
// #endif

BluetoothSerial SerialBT;

// ================================================================================================================================================

// ===========================
// Enter your WiFi credentials
// ===========================
// const char* ssid = "gern";
// const char* password = "glenpppp0";

void startCameraServer();
// void setupLedFlash(int pin);

// #define LED_BUILTIN 33

void setup() {
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector being triggered
  // pinMode(LED_BUILTIN, OUTPUT); // Set the pin as output
  // digitalWrite(LED_BUILTIN, HIGH); // Turn off internal LED
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_LATEST; // originally CAMERA_GRAB_WHEN_EMPTY
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10; // originally 12
  config.fb_count = 2; // originally 1
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  // if(config.pixel_format == PIXFORMAT_JPEG){
  //   if(psramFound()){
  //     config.jpeg_quality = 10;
  //     config.fb_count = 2;
  //     config.grab_mode = CAMERA_GRAB_LATEST;
  //   } else {
  //     // Limit the frame size when PSRAM is not available
  //     config.frame_size = FRAMESIZE_SVGA;
  //     config.fb_location = CAMERA_FB_IN_DRAM;
  //   }
  // } else {
  //   // Best option for face detection/recognition
  //   config.frame_size = FRAMESIZE_240X240;
// #if CONFIG_IDF_TARGET_ESP32S3
//     config.fb_count = 2;
// #endif
  // }

// #if defined(CAMERA_MODEL_ESP_EYE)
//   pinMode(13, INPUT_PULLUP);
//   pinMode(14, INPUT_PULLUP);
// #endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if(config.pixel_format == PIXFORMAT_JPEG){
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

// #if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
//   s->set_vflip(s, 1);
//   s->set_hmirror(s, 1);
// #endif

// #if defined(CAMERA_MODEL_ESP32S3_EYE)
//   s->set_vflip(s, 1);
// #endif

// Setup LED FLash if LED pin is defined in camera_pins.h
// #if defined(LED_GPIO_NUM)
//   setupLedFlash(LED_GPIO_NUM);
// #endif

  WiFi.begin("gern", "glenpppp0");
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  // digitalWrite(LED_BUILTIN, LOW); // Turn on internal LED when conneted to Wi-Fi

  // ================================================================================================================================================
  // for S2SBT
  SerialBT.begin(device_name); //Bluetooth device name
  //Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  // #ifdef USE_PIN
  //   SerialBT.setPin(pin);
  //   Serial.println("Using PIN");
  // #endif
  // ================================================================================================================================================
}

void loop() {
  // for tramsiting data
  SerialBT.print("hello");

  // for receiving data
  if (SerialBT.available()) {
    //Serial.write(SerialBT.read());
    int message = SerialBT.read();
    Serial.println(message);
  }

  delay(5000); // 5 seconds
}
