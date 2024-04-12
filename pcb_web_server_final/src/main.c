// Web server code for ESP on PCB

#include <stdio.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "my_data.h"
#include <driver/gpio.h>

static const char *TAG = "ESP32 Server";

// Initialize HTML
int html_size = 20000; // specially construed size (15509)
const char *html = "\
  <html>\
    <head>\
      <title>ESP32 Demo</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
        textarea { resize: none; }\
        table { border-collapse: collapse; }\
        table, th, td { border: 1px solid black; }\
        th, td { padding: 10px; }\
      </style>\
    </head>\
    <body>\
      <h1>Pet Feeder Menu</h1>\
      <a href='http://172.20.10.8/'> Link to video stream! </a>\
      <br><br>\
      Bowl Status: %s!\
      <br><br>\
      Container Status: %s!\
      <br><br><br>\
      <table>\
        <tbody>\
          <th></th>\
          <th>SUN</th>\
          <th>MON</th>\
          <th>TUE</th>\
          <th>WED</th>\
          <th>THU</th>\
          <th>FRI</th>\
          <th>SAT</th>\
          <tr>\
            <td>9am</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='00'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='01'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='02'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='03'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='04'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='05'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='06'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
          <tr>\
            <td>10am</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='10'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='11'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='12'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='13'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='14'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='15'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='16'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
          <tr>\
            <td>11am</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='20'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='21'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='22'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='23'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='24'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='25'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='26'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
          <tr>\
            <td>12pm</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='30'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='31'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='32'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='33'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='34'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='35'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='36'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
          <tr>\
            <td>1pm</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='40'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='41'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='42'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='43'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='44'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='45'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='46'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
          <tr>\
            <td>2pm</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='50'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='51'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='52'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='53'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='54'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='55'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='56'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
          <tr>\
            <td>3pm</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='60'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='61'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='62'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='63'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='64'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='65'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='66'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
          <tr>\
            <td>4pm</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='70'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='71'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='72'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='73'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='74'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='75'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='76'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
          <tr>\
            <td>5pm</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='80'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='81'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='82'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='83'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='84'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='85'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='86'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
          <tr>\
            <td>6pm</td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='90'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='91'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='92'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='93'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='94'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='95'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
            <td> <form action=\"check\" method=\"get\">\
              <input type='hidden' name='id' value='96'>\
              <input type='checkbox' %s onchange='this.form.submit()'>\
            </form> </td>\
          </tr>\
        </tbody>\
      </table>\
    </body>\
  </html>";

// Initialize real time fetching
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "protocol_examples_common.h"
#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h"
#include "esp_sntp.h"
char timeWeekDay[10] = "";
char timeHour[10] = "";
char timeID[3] = "";
int idDay = -1;
int idHour = -1;
void obtain_time(void) {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  
  ESP_LOGI(TAG, "Initializing and starting SNTP");
  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("time.nist.gov"); // 0.pool.ntp.org, time.nist.gov, time.windows.com, and time.google.com
  esp_netif_sntp_init(&config);

  time_t now = 0;
  struct tm timeinfo = { 0 };
  int retry = 0;
  const int retry_count = 15;
  while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
  }
  time(&now);
  localtime_r(&now, &timeinfo);
  esp_netif_sntp_deinit();
}

// Initialize scheduling system on web page
#include "set.h"
Node *schedule = NULL;
char *checked[10][7];
// initialize all checkboxes to be empty
void init_checked() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 7; j++) {
      checked[i][j] = "";
    }
  }
}

// Initialize load cell
#include "HX711.h"
long bowlWeight = -1;
char *bowlStatus = "";
void init_scale(void) {
  HX711_init(LDCL_DATA, LDCL_SCLK, eGAIN_128); 
  HX711_tare();
}

// Initialize IR sensor
#include "IR.h"
uint16_t containerDist = -1;
char *containerStatus = "";
void init_IR() {
  gpio_reset_pin(IR_SDA);                         
  gpio_set_direction(IR_SDA, GPIO_MODE_OUTPUT);
  gpio_reset_pin(IR_SCL);                         
  gpio_set_direction(IR_SCL, GPIO_MODE_OUTPUT);
}

// Inialize motor
#define stepsPerRevolution 200
#define DIR 27
#define STEP 33
#define NRESET 26
#define NSLEEP 32
#define NENBL 25
void init_motor(void) {
  gpio_reset_pin(DIR);
  gpio_reset_pin(STEP);
  gpio_reset_pin(NRESET);
  gpio_reset_pin(NSLEEP);

  gpio_set_direction(DIR, GPIO_MODE_OUTPUT);
  gpio_set_direction(STEP, GPIO_MODE_OUTPUT);
  gpio_set_direction(NRESET, GPIO_MODE_OUTPUT);
  gpio_set_direction(NSLEEP, GPIO_MODE_OUTPUT);
  gpio_set_direction(NENBL, GPIO_MODE_OUTPUT);

  gpio_set_level(DIR, 1);
  gpio_set_level(NRESET, 1);
  gpio_set_level(NSLEEP, 0);
  gpio_set_level(NENBL, 0);
}



static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection() {
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}

void updatePage() {
  char *temp = (char *) malloc(sizeof(*temp) * html_size);
  snprintf(temp, html_size, html, bowlStatus, containerStatus,
            checked[0][0], checked[0][1], checked[0][2], checked[0][3], checked[0][4], checked[0][5], checked[0][6],
            checked[1][0], checked[1][1], checked[1][2], checked[1][3], checked[1][4], checked[1][5], checked[1][6],
            checked[2][0], checked[2][1], checked[2][2], checked[2][3], checked[2][4], checked[2][5], checked[2][6],
            checked[3][0], checked[3][1], checked[3][2], checked[3][3], checked[3][4], checked[3][5], checked[3][6],
            checked[4][0], checked[4][1], checked[4][2], checked[4][3], checked[4][4], checked[4][5], checked[4][6],
            checked[5][0], checked[5][1], checked[5][2], checked[5][3], checked[5][4], checked[5][5], checked[5][6],
            checked[6][0], checked[6][1], checked[6][2], checked[6][3], checked[6][4], checked[6][5], checked[6][6],
            checked[7][0], checked[7][1], checked[7][2], checked[7][3], checked[7][4], checked[7][5], checked[7][6],
            checked[8][0], checked[8][1], checked[8][2], checked[8][3], checked[8][4], checked[8][5], checked[8][6],
            checked[9][0], checked[9][1], checked[9][2], checked[9][3], checked[9][4], checked[9][5], checked[9][6]);
  httpd_resp_send(NULL, temp, html_size);
  free(temp);
}

esp_err_t get_handler(httpd_req_t *req) {
  char *temp = (char *) malloc(sizeof(*temp) * html_size);
  snprintf(temp, html_size, html, bowlStatus, containerStatus,
          checked[0][0], checked[0][1], checked[0][2], checked[0][3], checked[0][4], checked[0][5], checked[0][6],
          checked[1][0], checked[1][1], checked[1][2], checked[1][3], checked[1][4], checked[1][5], checked[1][6],
          checked[2][0], checked[2][1], checked[2][2], checked[2][3], checked[2][4], checked[2][5], checked[2][6],
          checked[3][0], checked[3][1], checked[3][2], checked[3][3], checked[3][4], checked[3][5], checked[3][6],
          checked[4][0], checked[4][1], checked[4][2], checked[4][3], checked[4][4], checked[4][5], checked[4][6],
          checked[5][0], checked[5][1], checked[5][2], checked[5][3], checked[5][4], checked[5][5], checked[5][6],
          checked[6][0], checked[6][1], checked[6][2], checked[6][3], checked[6][4], checked[6][5], checked[6][6],
          checked[7][0], checked[7][1], checked[7][2], checked[7][3], checked[7][4], checked[7][5], checked[7][6],
          checked[8][0], checked[8][1], checked[8][2], checked[8][3], checked[8][4], checked[8][5], checked[8][6],
          checked[9][0], checked[9][1], checked[9][2], checked[9][3], checked[9][4], checked[9][5], checked[9][6]);
  httpd_resp_send(req, temp, html_size);
  free(temp);
  return ESP_OK;
}

esp_err_t check_handler(httpd_req_t *req) {
  // Buffer to hold the URL query parameter
  char element_id[64]; // Adjust the size according to your requirement

  // Get the length of the query string
  size_t url_query_len = httpd_req_get_url_query_len(req);

  // If there's a query string and it fits in our buffer
  if (url_query_len > 0 && url_query_len < sizeof(element_id)) {
      // Get the query string
      if (httpd_req_get_url_query_str(req, element_id, sizeof(element_id)) == ESP_OK) {
          // Parse the query string to extract the element ID
          if (httpd_query_key_value(element_id, "id", element_id, sizeof(element_id)) == ESP_OK) {
              // At this point, `element_id` contains the value of the "id" parameter
              ESP_LOGI(TAG, "Host: %s", element_id);
          }
      }
  }

  // toggle checkmark
  int row = (int) (element_id[0] - '0');
  int col = (int) (element_id[1] - '0');
  if (strcmp(checked[row][col], "") == 0) {
      checked[row][col] = "checked";
  } else {
      checked[row][col] = "";
  }

  // update schedule set
  if (strcmp(checked[row][col], "checked") == 0) { // add to set
      insert(&schedule, (int) atoi(element_id));
  } else { // remove from set
      erase(&schedule, (int) atoi(element_id));
  }

  // update webpage
  char *temp = (char *) malloc(sizeof(*temp) * html_size);
  snprintf(temp, html_size, html, bowlStatus, containerStatus,
          checked[0][0], checked[0][1], checked[0][2], checked[0][3], checked[0][4], checked[0][5], checked[0][6],
          checked[1][0], checked[1][1], checked[1][2], checked[1][3], checked[1][4], checked[1][5], checked[1][6],
          checked[2][0], checked[2][1], checked[2][2], checked[2][3], checked[2][4], checked[2][5], checked[2][6],
          checked[3][0], checked[3][1], checked[3][2], checked[3][3], checked[3][4], checked[3][5], checked[3][6],
          checked[4][0], checked[4][1], checked[4][2], checked[4][3], checked[4][4], checked[4][5], checked[4][6],
          checked[5][0], checked[5][1], checked[5][2], checked[5][3], checked[5][4], checked[5][5], checked[5][6],
          checked[6][0], checked[6][1], checked[6][2], checked[6][3], checked[6][4], checked[6][5], checked[6][6],
          checked[7][0], checked[7][1], checked[7][2], checked[7][3], checked[7][4], checked[7][5], checked[7][6],
          checked[8][0], checked[8][1], checked[8][2], checked[8][3], checked[8][4], checked[8][5], checked[8][6],
          checked[9][0], checked[9][1], checked[9][2], checked[9][3], checked[9][4], checked[9][5], checked[9][6]);
  // ESP_LOGI(TAG, "html_size: %d", strlen(html));
  httpd_resp_send(req, temp, html_size);
  free(temp);

  return ESP_OK;
}

void getTime() {
  // get local time
  time_t now;
  struct tm timeinfo;
  time(&now);
  localtime_r(&now, &timeinfo);
  if (timeinfo.tm_year < (2016 - 1900)) { // check if time is already correct
    obtain_time();
    time(&now);
  }

  // store time info
  setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
  tzset();
  localtime_r(&now, &timeinfo);
  char strftime_buf[64];
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  ESP_LOGI(TAG, "The current date/time in New York is: %s", strftime_buf);
  
  strftime(timeHour,10, "%H", &timeinfo); // hour
  strftime(timeWeekDay,10, "%A", &timeinfo); // day
  printf("Hour: %s \n", timeHour);
  printf("Day: %s \n", timeWeekDay);
  ESP_LOGI(TAG, "timeWeekDay timeHour: %s %s", timeWeekDay, timeHour);

  if (strcmp(timeWeekDay, "Sunday") == 0) {
    idDay = 0;
  } else if (strcmp(timeWeekDay, "Monday") == 0) {
    idDay = 1;
  } else if (strcmp(timeWeekDay, "Tuesday") == 0) {
    idDay = 2;
  } else if (strcmp(timeWeekDay, "Wednesday") == 0) {
    idDay = 3;
  } else if (strcmp(timeWeekDay, "Thursday") == 0) {
    idDay = 4;
  } else if (strcmp(timeWeekDay, "Friday") == 0) {
    idDay = 5;
  } else {
    idDay = 6;
  }
 
  if (strcmp(timeHour, "09") == 0) {
    idHour = 0;
  } else if (strcmp(timeHour, "10") == 0) {
    idHour = 1;
  } else if (strcmp(timeHour, "11") == 0) {
    idHour = 2;
  } else if (strcmp(timeHour, "12") == 0) {
    idHour = 3;
  } else if (strcmp(timeHour, "13") == 0) {
    idHour = 4;
  } else if (strcmp(timeHour, "14") == 0) {
    idHour = 5;
  } else if (strcmp(timeHour, "15") == 0) {
    idHour = 6;
  } else if (strcmp(timeHour, "16") == 0) {
    idHour = 7;
  } else if (strcmp(timeHour, "17") == 0) {
    idHour = 8;
  } else if (strcmp(timeHour, "18") == 0) {
    idHour = 9;
  } else {
    idHour = -1;
  }
  
  // print id from time
  ESP_LOGI(TAG, "idHour idDay: %d %d", idHour, idDay);
}

void handleMotor() {
  // update idHour and idDay
  getTime();

  char idFromTime[3];
  idFromTime[0] = (char) idHour + '0';
  idFromTime[1] = (char) idDay + '0';
  idFromTime[2] = '\0';

  if (contains(schedule, (int) atoi(idFromTime))) {
    // turn on motor
    gpio_set_level(NSLEEP, 1);
    // Spin the stepper motor 1 revolution:
    for (int i = 0; i < stepsPerRevolution * 0.6; i++) {
      // These four lines result in 1 step:
      gpio_set_level(STEP, 1);
      vTaskDelay(1);
      gpio_set_level(STEP, 0);
      vTaskDelay(1);
    }
    // turn off motor
    gpio_set_level(NSLEEP, 0);

    // remove id from schedule
    erase(&schedule, (int) atoi(idFromTime));
    checked[idHour][idDay] = ""; // uncheck box
  }
}

void handleIR() {
  init_i2c(TFL_DIST_LO);
  i2c_start();
  i2c_write_addr();
  i2c_write_bit(1); // Read bit
  uint8_t low_bits = i2c_read_byte();

  i2c_stop();
  
  init_i2c(TFL_DIST_HI);
  i2c_start();
  i2c_write_addr();
  i2c_write_bit(1); // Read bit
  uint8_t upper_bits = i2c_read_byte();
  i2c_stop();

  uint16_t dist_val = upper_bits << 8;
  dist_val |= low_bits;

  // IR Sensor value (type: uint16_t)
  printf("(cm) DIST: %d", dist_val);

  containerDist = dist_val;

  if (containerDist < 4) {
    containerStatus = "Full";
  } else {
    containerStatus = "Low";
  }
}

void handleScale() {
  bowlWeight = HX711_get_units(1);
  printf("******* weight = %ld *********\n", bowlWeight);
  HX711_power_down();             
  vTaskDelay(pdMS_TO_TICKS(1000));
  HX711_power_up();

  if (bowlWeight < 3500) {
    bowlStatus = "Low";
  } else {
    bowlStatus = "Full";
  } 
}


/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL};

httpd_uri_t uri_check = {
    .uri = "/check",
    .method = HTTP_GET,
    .handler = check_handler,
    .user_ctx = NULL};

httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_check);
    }
    return server;
}

void stop_webserver(httpd_handle_t server)
{
    if (server)
    {
        httpd_stop(server);
    }
}



int i = 0;
void app_main(void)
{
  ESP_LOGE(TAG, "html_size: %d", html_size);

  // initialization
  nvs_flash_init();
  wifi_connection();
  init_checked();
  init_IR();
  init_scale();
  init_motor();

  start_webserver();
  while (1) {
    if (!empty(schedule)) {
      printf("running handleMotor()\n");
      handleMotor();
    }
    if (i % 10 == 0) { // 10 * ~100 ms delay
      printf("running handleScale()\n");
      handleScale();
      printf("running handleIR()\n");
      handleIR();
    }

    // getTime();
    printf("in main loop\n");
    display(schedule);
    vTaskDelay(pdMS_TO_TICKS(500));
    updatePage(); // maybe delete
    i++;
  }
}