
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
  
  // wait for initilization of wifly
  delay(3000);
  
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
  char c;
  while (wifly.receive(&c, 1, 300) > 0) {
    Serial.print((char)c);
  }
  
  if (wifly.commandMode()) {
    Serial.println("Enter command mode. Send \"$$$\"(with \\r) to exit command mode");
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
