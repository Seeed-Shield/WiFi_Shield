
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "WiFly.h"

#define SSID      "STEST"
#define KEY       "mmmmmmmm"
#define AUTH      WIFLY_AUTH_WPA2_PSK

WiFly wifly(2, 3);

void setup() {
  Serial.begin(9600);
  Serial.println("TEST WIFLY");
  
  wifly.sendCommand("factory RESET\r", "Factory");
  wifly.init();
  
  Serial.println("Join " SSID );
  if (wifly.join(SSID, KEY, AUTH)) {
    Serial.println("OK");
  } else {
    Serial.println("Failed");
  }
  
  // get WiFly params
  wifly.sendCommand("get everthing\r");
  int c;
  while ((c = wifly.timedRead(300)) >= 0) {
    Serial.print((char)c);
  }
  
//  wifly.dataMode();
  if (wifly.commandMode()) {
    Serial.println("Enter command mode.");
  }
}

void loop() {
  while (wifly.available()) {
    Serial.write(wifly.read());
  }
  
  while (Serial.available()) {
    wifly.write(Serial.read());
  }
}
