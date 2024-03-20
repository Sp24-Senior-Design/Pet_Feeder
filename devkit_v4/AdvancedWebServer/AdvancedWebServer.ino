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
*/

/* old code, might be usefule later

// javascript for gettign a value from web page
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

// individual inputs and displays for the month, day, hour, and minute
    <form action=\"/getmonth\" method=\"get\">\
      <label for=\"inputValue\">Enter month (1-12):</label>\
      <input type=\"text\" id=\"inputValue\" name=\"value\">\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    <form action=\"/getday\" method=\"get\">\
      <label for=\"inputValue\">Enter day (1-31):</label>\
      <input type=\"text\" id=\"inputValue\" name=\"value\">\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    <form action=\"/gethour\" method=\"get\">\
      <label for=\"inputValue\">Enter hour (0-23):</label>\
      <input type=\"text\" id=\"inputValue\" name=\"value\">\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    <form action=\"/getminute\" method=\"get\">\
      <label for=\"inputValue\">Enter minute (0-59):</label>\
      <input type=\"text\" id=\"inputValue\" name=\"value\">\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    <br><br>\
    <h1>Schedule:</h1>\
    <label>Month:</label>\
    <textarea readonly id='displayValue' rows='1' cols='1'>%d</textarea>\
    <label>Day:</label>\
    <textarea readonly id='displayValue' rows='1' cols='1'>%d</textarea>\
    <label>Hour:</label>\
    <textarea readonly id='displayValue' rows='1' cols='1'>%d</textarea>\
    <label>Minute:</label>\
    <textarea readonly id='displayValue' rows='1' cols='1'>%d</textarea>\
    <br><br>\

// handlers for getting month, day, hour, minute
    void handleGetMonth() {
      if (server.hasArg("value") && server.arg("value") != "") {
        month = server.arg("value").toInt();
        Serial.print("Received month value: ");
        Serial.println(month);
      }
      // goes back to root web page
      server.sendHeader("Location", "/", true);  // Set the "Location" header to root URL
      server.send(302, "text/plain", "");        // Send a 302 Found status code for redirect
    }
    void handleGetDay() {
      if (server.hasArg("value") && server.arg("value") != "") {
        day = server.arg("value").toInt();
        Serial.print("Received day value: ");
        Serial.println(day);
      }
      // goes back to root web page
      server.sendHeader("Location", "/", true);  // Set the "Location" header to root URL
      server.send(302, "text/plain", "");        // Send a 302 Found status code for redirect
    }
    void handleGetHour() {
      if (server.hasArg("value") && server.arg("value") != "") {
        hour = server.arg("value").toInt();
        Serial.print("Received hour value: ");
        Serial.println(hour);
      }
      // server.send(200, "text/plain", "Submitted successfully");
      // goes back to root web page
      server.sendHeader("Location", "/", true);  // Set the "Location" header to root URL
      server.send(302, "text/plain", "");        // Send a 302 Found status code for redirect
    }
    void handleGetMinute() {
      if (server.hasArg("value") && server.arg("value") != "") {
        minute = server.arg("value").toInt();
        Serial.print("Received minute value: ");
        Serial.println(minute);
      }
      // goes back to root web page
      server.sendHeader("Location", "/", true);  // Set the "Location" header to root URL
      server.send(302, "text/plain", "");        // Send a 302 Found status code for redirect
    }

// draw graph function that came with the example code
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

*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <set>

const char *ssid = "gern";
const char *password = "glenpppp0";

WebServer server(80);

// const int led = 13;
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


void handleRoot() {
  // digitalWrite(led, 1);
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  int html_size = 6000;
  char temp[html_size];
  const char *html = "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
      textarea { resize: none; }\
      table { border-collapse: collapse; }\
      table, th, td { border: 1px solid black; }\
      th, td { padding: 10px; }\
    </style>\
    <script>\
    function toggleCheckmark() {\
      var checkbox = document.getElementById(\"myCheckbox\");\
      checkbox.checked = !checkbox.checked; // Toggle the checked state\
    }\
    function handleCheckboxChange {\
      fetch('/check', {\
        method: 'GET',\
        headers: {\
            'Content-Type': 'text/plain'\
        }\
      })\
    }\
    </script>\
  </head>\
  <body>\
    <h1>Pet Feeder Menu</h1>\
    <a href='http://172.20.10.8/'> Link to video stream! </a>\
    <p>Uptime: %02d:%02d:%02d</p>\
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
          <td>9:00am</td>\
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
          <td>10:00am</td>\
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
      </tbody>\
  </table>\
  </body>\
</html>";

  // Serial.println(strlen(html));
  snprintf(temp, html_size, html, hr, min % 60, sec % 60, checked[0][0], checked[0][1], checked[0][2], checked[0][3], checked[0][4], checked[0][5], checked[0][6],
                                                          checked[1][0], checked[1][1], checked[1][2], checked[1][3], checked[1][4], checked[1][5], checked[1][6]);
  server.send(200, "text/html", temp);
  // digitalWrite(led, 0);
}

// void handleRoot() {
//   // digitalWrite(led, 1);
//   int sec = millis() / 1000;
//   int min = sec / 60;
//   int hr = min / 60;

//   int html_size = 1000000;
//   char temp[html_size];
//   const char *html = "<html>\
//   <head>\
//     <meta http-equiv='refresh' content='5'/>\
//     <title>ESP32 Demo</title>\
//     <style>\
//       body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
//       textarea { resize: none; }\
//       table { border-collapse: collapse; }\
//       table, th, td { border: 1px solid black; }\
//       th, td { padding: 10px; }\
//     </style>\
//     <script>\
//     function toggleCheckmark() {\
//       var checkbox = document.getElementById(\"myCheckbox\");\
//       checkbox.checked = !checkbox.checked; // Toggle the checked state\
//     }\
//     function handleCheckboxChange {\
//       fetch('/check', {\
//         method: 'GET',\
//         headers: {\
//             'Content-Type': 'text/plain'\
//         }\
//       })\
//     }\
//     </script>\
//   </head>\
//   <body>\
//     <h1>Pet Feeder Menu</h1>\
//     <a href='http://172.20.10.8/'> Link to video stream! </a>\
//     <p>Uptime: %02d:%02d:%02d</p>\
//     <br><br>\
//     <table>\
//       <tbody>\
//         <th></th>\
//         <th>SUN</th>\
//         <th>MON</th>\
//         <th>TUE</th>\
//         <th>WED</th>\
//         <th>THU</th>\
//         <th>FRI</th>\
//         <th>SAT</th>\
//         <tr>\
//           <td>9:00am</td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='00'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='01'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='02'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='03'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='04'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='05'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='06'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//         </tr>\
//         <tr>\
//             <td>10:00am</td>\
//             <td> <form action=\"check\" method=\"get\">\
//               <input type='hidden' name='id' value='10'>\
//               <input type='checkbox' %s onchange='this.form.submit()'>\
//             </form> </td>\
//             <td> <form action=\"check\" method=\"get\">\
//               <input type='hidden' name='id' value='11'>\
//               <input type='checkbox' %s onchange='this.form.submit()'>\
//             </form> </td>\
//             <td> <form action=\"check\" method=\"get\">\
//               <input type='hidden' name='id' value='12'>\
//               <input type='checkbox' %s onchange='this.form.submit()'>\
//             </form> </td>\
//             <td> <form action=\"check\" method=\"get\">\
//               <input type='hidden' name='id' value='13'>\
//               <input type='checkbox' %s onchange='this.form.submit()'>\
//             </form> </td>\
//             <td> <form action=\"check\" method=\"get\">\
//               <input type='hidden' name='id' value='14'>\
//               <input type='checkbox' %s onchange='this.form.submit()'>\
//             </form> </td>\
//             <td> <form action=\"check\" method=\"get\">\
//               <input type='hidden' name='id' value='15'>\
//               <input type='checkbox' %s onchange='this.form.submit()'>\
//             </form> </td>\
//             <td> <form action=\"check\" method=\"get\">\
//               <input type='hidden' name='id' value='16'>\
//               <input type='checkbox' %s onchange='this.form.submit()'>\
//             </form> </td>\
//         </tr>\
//         <tr>\
//           <td>11:00am</td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='20'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='21'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='22'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='23'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='24'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='25'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='26'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//         </tr>\
//         <tr>\
//           <td>12:00pm</td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='30'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='31'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='32'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='33'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='34'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='35'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='36'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//         </tr>\
//         <tr>\
//           <td>1:00pm</td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='40'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='41'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='42'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='43'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='44'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='45'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='46'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//         </tr>\
//         <tr>\
//           <td>2:00pm</td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='50'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='51'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='52'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='53'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='54'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='55'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='56'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//         </tr>\
//         <tr>\
//           <td>3:00pm</td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='60'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='61'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='62'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='63'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='64'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='65'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='66'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//         </tr>\
//         <tr>\
//           <td>4:00pm</td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='70'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='71'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='72'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='73'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='74'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='75'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='76'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//         </tr>\
//         <tr>\
//           <td>5:00pm</td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='80'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='81'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='82'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='83'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='84'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='85'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='86'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//         </tr>\
//         <tr>\
//           <td>6:00pm</td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='90'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='91'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='92'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='93'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='94'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='95'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//           <td> <form action=\"check\" method=\"get\">\
//             <input type='hidden' name='id' value='96'>\
//             <input type='checkbox' %s onchange='this.form.submit()'>\
//           </form> </td>\
//         </tr>\
//       </tbody>\
//   </table>\
//   </body>\
// </html>";

//   // Serial.println(strlen(html));
//   snprintf(temp, html_size, html, hr, min % 60, sec % 60, checked[0][0], checked[0][1], checked[0][2], checked[0][3], checked[0][4], checked[0][5], checked[0][6], 
//                                                           checked[1][0], checked[1][1], checked[1][2], checked[1][3], checked[1][4], checked[1][5], checked[1][6],
//                                                           checked[2][0], checked[2][1], checked[2][2], checked[2][3], checked[2][4], checked[2][5], checked[2][6],
//                                                           checked[3][0], checked[3][1], checked[3][2], checked[3][3], checked[3][4], checked[3][5], checked[3][6],
//                                                           checked[4][0], checked[4][1], checked[4][2], checked[4][3], checked[4][4], checked[4][5], checked[4][6],
//                                                           checked[5][0], checked[5][1], checked[5][2], checked[5][3], checked[5][4], checked[5][5], checked[5][6],
//                                                           checked[6][0], checked[6][1], checked[6][2], checked[6][3], checked[6][4], checked[6][5], checked[6][6],
//                                                           checked[7][0], checked[7][1], checked[7][2], checked[7][3], checked[7][4], checked[7][5], checked[7][6],
//                                                           checked[8][0], checked[8][1], checked[8][2], checked[8][3], checked[8][4], checked[8][5], checked[8][6],
//                                                           checked[9][0], checked[9][1], checked[9][2], checked[9][3], checked[9][4], checked[9][5], checked[9][6]);
//   server.send(200, "text/html", temp);
//   // digitalWrite(led, 0);
// }

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
  // if (id == "00") {
  //   checked[0][0] == "" ? checked[0][0] = "checked" : checked[0][0] = "";
  // } else if (id == "01") {
  //   checked[0][1] == "" ? checked[0][1] = "checked" : checked[0][1] = "";
  // } else if (id == "02") {
  //   checked[0][2] == "" ? checked[0][2] = "checked" : checked[0][2] = "";
  // } else if (id == "03") {
  //   checked[0][3] == "" ? checked[0][3] = "checked" : checked[0][3] = "";
  // } else if (id == "04") {
  //   checked[0][4] == "" ? checked[0][4] = "checked" : checked[0][4] = "";
  // } else if (id == "05") {
  //   checked[0][5] == "" ? checked[0][5] = "checked" : checked[0][5] = "";
  // } else if (id == "06") {
  //   checked[0][6] == "" ? checked[0][6] = "checked" : checked[0][6] = "";
  // }

  // update schedule set
  if (checked[row][col] == "checked") { // add to set
    schedule.insert(id);
  } else { // remove from set
    schedule.erase(id);
  }
  Serial.print("Schedule set: ");
  for (auto i : schedule) {
    Serial.println(i);
  }
  Serial.println();

  // goes back to root web page
  server.sendHeader("Location", "/", true);  // Set the "Location" header to root URL
  server.send(302, "text/plain", "");        // Send a 302 Found status code for redirect
}

void handleNotFound() {
  // digitalWrite(led, 1);
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
  // digitalWrite(led, 0);
}

void setup(void) {
  init_checked();
  // pinMode(led, OUTPUT);
  // digitalWrite(led, 0);
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
  // server.on("/getmonth", handleGetMonth);  // for get month
  // server.on("/getday", handleGetDay);  // for get day
  // server.on("/gethour", handleGetHour);  // for get hour
  // server.on("/getminute", handleGetMinute);  // for get minute
  server.on("/check", handleCheck);  // for toggling checkmarks
  // server.on("/test.svg", drawGraph); // disable graph
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
