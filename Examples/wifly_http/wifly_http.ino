
 
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <WiFly.h>
#include "HTTPClient.h"

#define SSID      "STEST"
#define KEY       "mmmmmmmm"
#define AUTH      WIFLY_AUTH_WPA2_PSK

// Pins
// Arduino       WiFly
//  2 - receive  TX   (Send from Wifly, Receive to Arduino)
//  3 - send     RX   (Send from Arduino, Receive to WiFly) 
WiFly wifly(2, 3);
HTTPClient http;

char buf[256];

void test()
{
  char scheme[6];
  char host[32];
  char path[128];
  char *url = "http://mbed.org/media/uploads/donatien/hello.txt";
  uint16_t port;
  
  http.parseURL(url, scheme, 6, host, 32, &port, path, 128);
  Serial.println(scheme);
  Serial.println(host);
  Serial.println(port);
  Serial.println(path);
  
  http.parseURL(url, host, 32, &port, path, 128);
  Serial.println(host);
  Serial.println(port);
  Serial.println(path);
  
  http.parseURL(url + 7, host, 32, &port, path, 128);
  Serial.println(host);
  Serial.println(port);
  Serial.println(path);
}
  

void setup() {
  Serial.begin(9600);
  Serial.println("TEST WIFLY");
  
//  test();

  delay(3000);
  
//  wifly.sendCommand("factory RESET\r", "Factory");
//  wifly.init();
  
  Serial.println("Join " SSID );
  if (wifly.join(SSID, KEY, AUTH)) {
    Serial.println("OK");
  } else {
    Serial.println("Failed");
  }
  
  wifly.clear();
  
//  http.get("http://mbed.org/media/uploads/donatien/hello.txt", buf, sizeof(buf), 10000);
  
//  wifly.dataMode();
  if (wifly.commandMode()) {
//    Serial.println("Enter command mode. Send \"exit\"(with \\r) to exit command mode");
  }
  
  wifly.sendCommand("set d n mbed.org\r", "AOK");
  wifly.sendCommand("set i r 80\r", "AOK");
  wifly.sendCommand("set i p 18\r", "AOK");
  wifly.sendCommand("set i a 0\r", "AOK");
  wifly.sendCommand("set c r 0\r", "AOK");
  
  wifly.sendCommand("open\r", "*OPEN*", 10000);
  
  wifly.send("GET /media/uploads/donatien/hello.txt HTTP/1.1\r\n"
             "Host: mbed.org\r\n"
             "Connection: close\r\n"
             "\r\n"
             );
}


void loop() {
 
#if 1
  while (wifly.available()) {
    Serial.write(wifly.read());
  }
  
  while (Serial.available()) {
    wifly.write(Serial.read());
  }
#endif
}
