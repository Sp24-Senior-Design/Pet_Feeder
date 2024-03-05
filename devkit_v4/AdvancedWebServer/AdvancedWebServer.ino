/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   <script>\
      function getValue() {\
        var xhttp = new XMLHttpRequest();\
        xhttp.onreadystatechange = function() {\
          if (this.readyState == 4 && this.status == 200) {\
            document.getElementById('displayValue').innerText = this.responseText;\
          }\
        };\
        xhttp.open('GET', '/getvalue', true);\
        xhttp.send();\
      }\
    </script>\
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char *ssid = "gern";
const char *password = "glenpppp0";

WebServer server(80);

const int led = 13;

int hour = -1;

void handleRoot() {
  digitalWrite(led, 1);
  char temp[1200];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 1200,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      textarea { resize: none; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32-DevKitC V4!</h1>\
    <a href='http://172.20.10.8/'> Link to video stream! </a>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <br><br>\
    <form action=\"/gethour\" method=\"get\">\
      <label for=\"inputValue\">Enter hour:</label>\
      <input type=\"text\" id=\"inputValue\" name=\"value\">\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    <br><br>\
    <h1>Schedule:</h1>\
    <label>Hour:</label>\
    <textarea readonly id='displayValue' rows='1' cols='1'>%d</textarea>\
    <br><br>\
  </body>\
</html>",

           hr, min % 60, sec % 60, hour);
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleGetHour() {
  if (server.hasArg("value") && server.arg("value") != "") {
    int submittedValue = server.arg("value").toInt();
    hour = submittedValue;
    Serial.print("Received hour value: ");
    Serial.println(hour);
  }
  // server.send(200, "text/plain", "Submitted successfully");

  // goes back to root web page
  server.sendHeader("Location", "/", true);  // Set the "Location" header to root URL
  server.send(302, "text/plain", "");        // Send a 302 Found status code for redirect
}

void handleNotFound() {
  digitalWrite(led, 1);
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
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
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
  server.on("/gethour", handleGetHour);  // for get hour
  // server.on("/test.svg", drawGraph); // disable graph for now
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}
