/*************************************************************
 *
 *
 * Author:  Thangvpv
 * Company: FPT Software
 * Descript: NodeMCU controling SIM800A project
 *
 *
 *************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include "SH1106.h"
#include "ESP8266WiFi.h"
#include "ESP8266WiFiMulti.h"
#include "ESP8266WebServer.h"

// define I2C pin
#define OLED_SDA  D1   // pin 1
#define OLED_SDC  D2   // pin 2
#define OLED_ADDR 0x3C // oled address
// define notification leds
#define LED1 2  // led at bottom
#define LED2 16 // led at top
// define notification status
const String statusOK      = "OK";
const String statusNG      = "NG";
const String statusPending = ".....";

// display data structure
typedef struct DISPLAY_STRUCTURE {
  const String displayTitle = "Thangvpv";
  String displayIP          = "";
  String displayDetail      = "";
  String displayNotif       = "";
  String displayNotifStatus = "";
}DISPLAY_STRUCTURE;
// web data structure
struct WEB_CONTENT {
  String pageRoot       = "";
  String pageLogin      = "";
  String pageWifiSubmit = "";
}pageLoader;

// instant oled display
SH1106 display(OLED_ADDR, OLED_SDA, OLED_SDC);
// instant oled display structure
DISPLAY_STRUCTURE dispSet;
// instant wifi
ESP8266WiFiMulti wifiMulti;
// instant http port
ESP8266WebServer server(80);

void displayData(bool isProccessing);
void initMultiConnection();
void initStandaloneMode();
void initWebServer();

void setup() {
  // start serial monitor
  Serial.begin(9600);
  delay(3000);
  Serial.println("Welcome");
  // start oled display monitor
  Serial.println("initializing oled display");
  if (!display.init()) {
    Serial.println("failed initializing oled display");
    if (display.connect()) {
      Serial.println("reinitializing oled display");
      display.connect();
      display.displayOff();
      delay(1000);
      display.displayOff();
    }
    else {
      Serial.println("display not found");
    }
  }
  Serial.println("display ready");
  dispSet.displayNotif = "ready";
  dispSet.displayNotifStatus = statusOK;
  displayData(false);
  delay(1000);

  // start connecting wifi
  initMultiConnection();

  // start web server
  initWebServer();
}

void loop() {

}

void displayData(bool isProccessing) {
  // display data on oled monitor
  // clear old buffer
  display.clear();
  // draw 2 line on top
  display.drawLine(0, 0, 127, 0);
  display.drawLine(0, 2, 127, 2);
  // display title
  display.drawString(40, 3, dispSet.displayTitle);
  // display ip address
  display.drawString(0, 15, dispSet.displayIP);
  // display details
  if (isProccessing) {
    dispSet.displayDetail = "";
  }
  display.drawString(0, 32, dispSet.displayDetail);
  // display notifications
  display.drawString(0, 50, dispSet.displayNotif);
  // display notification status
  display.drawString(108, 50, "[" + dispSet.displayNotifStatus + "]");

  // display buffers to the screen
  display.display();
  delay(2000);
}

void initMultiConnection() {
  // init connections
  Serial.println("init defined connections");
  dispSet.displayNotif       = "connecting";
  dispSet.displayNotifStatus = statusPending;
  displayData(true);
  // can add more AP connections,
  // ESP8266 will connect to the strongest
  wifiMulti.addAP("Dedsec", "2901991");
  wifiMulti.addAP("FPT Software Guest");
  Serial.println("connecting");
  size_t timeout = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    display.drawProgressBar(4, 32, 120, 8, timeout);
    display.display();
    timeout++;
    delay(500);
    if (timeout == 100) {
      // if cannot connect to the access points
      // then switch to the standalone mode
      Serial.println("cannot connect to access points");
      Serial.println("switch to the standalone mode");
      dispSet.displayNotifStatus = statusNG;
      displayData(false);
      initStandaloneMode();
      break;
    }
  }
  if (wifiMulti.run() == WL_CONNECTED) {
    // when connection established
    dispSet.displayIP          = WiFi.localIP().toString();
    dispSet.displayDetail      = WiFi.SSID();
    dispSet.displayNotif       = "connected";
    dispSet.displayNotifStatus = statusOK;
    displayData(false);

    Serial.println("");
    Serial.print("connected to: ");
    Serial.println(WiFi.SSID());
    Serial.print("ip address: ");
    Serial.println(WiFi.localIP());
  }
}

void initStandaloneMode() {
  // initializing standalone mode,
  // nodemcu act like an access point
  Serial.println("begin standalone mode");
  dispSet.displayNotif       = "init standalone mode";
  dispSet.displayNotifStatus = statusPending;
  displayData(true);
  const char* ssid = "Dedsec Base";
  const char* pass = "29011991";
  size_t timeout = 0;
  while (!WiFi.softAP(ssid, pass)) {
    Serial.print(".");
    display.drawProgressBar(4, 32, 120, 8, timeout);
    timeout++;
    delay(500);
    if (timeout == 100) {
      Serial.println("Access point initializing failed");
      Serial.println("Begin to connecting access points");
      dispSet.displayNotifStatus = statusNG;
      displayData(false);
      timeout = 0;
      delay(5000);
    }
  }
  dispSet.displayIP          = WiFi.localIP().toString();
  dispSet.displayDetail      = WiFi.SSID();
  dispSet.displayNotif       = "standalone mode start";
  dispSet.displayNotifStatus = statusOK;
  displayData(false);
}

void initWebServer() {
  // initializing web server, web server service
  Serial.println("initializing web server");
  dispSet.displayNotif       = "init web sever";
  dispSet.displayNotifStatus = statusPending;
  displayData(false);
}
