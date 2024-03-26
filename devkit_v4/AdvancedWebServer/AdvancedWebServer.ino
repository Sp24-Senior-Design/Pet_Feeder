// ECE 477 TEAM 3 - SMART PET FEEDER

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <set>
#include "time.h"
#include <Arduino.h>
#include "HX711.h"
#include "soc/rtc.h"

// Initialize real time fetching
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -14400; // EST: GMT -04:00 (-14400 s)
const int daylightOffset_sec = 0;
struct tm timeinfo; // init time struct
char timeWeekDay[10] = "";
char timeHour[3] = "";
char timeMinute[3] = "";
char timeID[3] = "";
int idDay = -1;
int idHour = -1;

// Initialize LED pins
#define LED_PIN 4
#define INT_LED_PIN1 22 // other one is IO23 (solder connection was bad as of 3/25/24)

// Initialize web server
const char *ssid = "gern";
const char *password = "glenpppp0";
WebServer server(80);

// Initialize scheduling system on web page
using namespace std;
set<String> schedule;
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
#define LOADCELL_DOUT_PIN 19
#define LOADCELL_SCK_PIN 18
#define CALIBRATION_FACT 202.86
HX711 scale;
float bowlWeight = -1;
void init_scale() {
  // Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACT);
  scale.tare();
}


void handleRoot() {
  int html_size = 2000 * 10;
  char *temp = (char *) malloc(sizeof(*temp) * html_size);
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
      Weight of food in bowl: %f grams.\
      <br><br>\
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

  // Serial.println(strlen(html));
  snprintf(temp, html_size, html, bowlWeight, 
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
  server.send(200, "text/html", temp);
  free(temp);
}

void handleCheck() {
  String id = server.arg("id");
  Serial.print("Pressed: ");
  Serial.println(id);

  // toggle checkmark
  int row = (int) (id[0] - '0');
  int col = (int) (id[1] - '0');
  Serial.print("Row: ");
  Serial.print(row);
  Serial.print(" Col: ");
  Serial.println(col);
  checked[row][col] == "" ? checked[row][col] = "checked" : checked[row][col] = "";

  // update schedule set
  if (checked[row][col] == "checked") { // add to set
    schedule.insert(id);
  } else { // remove from set
    schedule.erase(id);
  }
  Serial.print("Schedule set: (");
  for (auto i : schedule) {
    Serial.print(i);
    Serial.print(" ");
  }
  Serial.println(")\n");

  // goes back to root web page
  server.sendHeader("Location", "/", true);  // Set the "Location" header to root URL
  server.send(302, "text/plain", "");        // Send a 302 Found status code for redirect
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}
 
void getTime() {
  // get local time
  getLocalTime(&timeinfo);

  // store time info
  strftime(timeWeekDay,10, "%A", &timeinfo); // day
  strftime(timeHour,3, "%H", &timeinfo); // hour
  // strftime(timeMinute,3, "%M", &timeinfo); // minute

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

  // Serial.print("timeHour: ");
  // Serial.println(timeHour);
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
  Serial.print("idHour idDay: ");
  Serial.print(idHour);
  Serial.print(" ");
  Serial.println(idDay);
  Serial.println();

  // print time
  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void handleMotor() {
  // update idHour and idDay
  getTime();

  char idFromTime[3];
  idFromTime[0] = (char) idHour + '0';
  idFromTime[1] = (char) idDay + '0';
  idFromTime[2] = '\0';

  if (schedule.count(idFromTime)) {
    // flash LED (gpio pin)
    digitalWrite(LED_PIN, HIGH); // Turn on LED
    delay(1000);
    digitalWrite(LED_PIN, LOW); // Turn off LED

    // remove id from schedule
    schedule.erase(idFromTime);
    checked[idHour][idDay] = ""; // uncheck box
  }
}

void handleScale() {
  scale.power_up();

  bowlWeight = scale.get_units();
  Serial.println("brendab sucks");
  Serial.println(bowlWeight);

  scale.power_down();
}



void setup(void) {
  init_checked();
  init_scale();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  pinMode(LED_PIN, OUTPUT); // Set the pin as output
  digitalWrite(LED_PIN, LOW); // Turn off LED
  pinMode(INT_LED_PIN1, OUTPUT); // Set the pin as output
  digitalWrite(INT_LED_PIN1, LOW); // Turn off LED

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // initialize handlers
  server.on("/", handleRoot);
  server.on("/check", handleCheck);  // for toggling checkmarks
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(INT_LED_PIN1, HIGH); // Turn on LED to indicate web server started
}


int i = 0;
void loop(void) {
  server.handleClient();
  if (!schedule.empty()) {
    handleMotor();
  }
  if (i % 10 == 0) { // 10 * ~100 ms delay
    handleScale();
  }
  delay(100); // allow the cpu to switch to other tasks
  i++;
}
