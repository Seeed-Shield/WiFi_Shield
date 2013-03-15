#include <string.h>
#include "WiFly.h"
#include "Debug.h"

WiFly::WiFly(uint8_t rx, uint8_t tx) : SoftwareSerial(rx, tx)
{
  instance = this;
  
  SoftwareSerial::begin(DEFAULT_BAUDRATE);
  setTimeout(DEFAULT_WAIT_RESPONSE_TIME);
  
  command_mode = false;
  associated = false;
}

boolean WiFly::setDefaultSettings()
{
  boolean result = true;
  // set time
  result = result & sendCommand("set c t 20\r", "AOK");
  
  // set size
  result = result & sendCommand("set c s 128\r", "AOK");

  // red led on when tcp connection active
  result = result & sendCommand("set s i 0x40\r", "AOK");

  // no string sent to the tcp client
  result = result & sendCommand("set c r 0\r", "AOK");

  // tcp protocol
  result = result & sendCommand("set i p 2\r", "AOK");

  // tcp retry
  result = result & sendCommand("set i f 0x7\r", "AOK");
  //no echo
  result = result & sendCommand("set u m 1\r", "AOK");
  // no auto join
  result = result & sendCommand("set w j 0\r", "AOK");
  
  // DHCP on
  result = result & sendCommand("set i d 1\r", "AOK");
  
  return result;
}

boolean WiFly::init()
{
  return setDefaultSettings();
}

boolean WiFly::staticIP(const char *ip, const char *mask, const char *gateway)
{
  boolean result = true;
  char cmd[MAX_CMD_LEN];
  
  result = sendCommand("set i d 0\r", "AOK");
  
  snprintf(cmd, MAX_CMD_LEN, "set i a %s\r", ip);
  result = result & sendCommand(cmd, "AOK");

  snprintf(cmd, MAX_CMD_LEN, "set i n %s\r", mask);
  result = result & sendCommand(cmd, "AOK");

  snprintf(cmd, MAX_CMD_LEN, "set i g %s\r", gateway);
  result = result & sendCommand(cmd, "AOK");
  
  return result;
}

boolean WiFly::join(const char *ssid, const char *phrase, int auth)
{
    char cmd[MAX_CMD_LEN];

    for (int i= 0; i < MAX_TRY_JOIN; i++) {
        // ssid
        snprintf(cmd, MAX_CMD_LEN, "set w s %s\r", ssid);
        if (!sendCommand(cmd, "AOK"))
            continue;

        //auth
        snprintf(cmd, MAX_CMD_LEN, "set w a %d\r", auth);
        if (!sendCommand(cmd, "AOK"))
            continue;

        //key
        if (auth != WIFLY_AUTH_OPEN) {
            if (auth == WIFLY_AUTH_WEP)
              snprintf(cmd, MAX_CMD_LEN, "set w k %s\r", phrase);
            else
              snprintf(cmd, MAX_CMD_LEN, "set w p %s\r", phrase);

            if (!sendCommand(cmd, "AOK"))
                continue;
        }

        //join the network
        if (!sendCommand("join\r", "Associated", 1000))
            continue;

        dataMode();

        associated = true;
        return true;
    }
    return false;
}

boolean leave()
{
  
}

boolean WiFly::send(const uint8_t *data, int data_len, const char *ack, int timeout)
{
  clear();
    
  // write data to wifly
  int write_bytes = 0;
  boolean write_error = false;
  unsigned long start_millis = millis();
  while (write_bytes < data_len) {
    if (write(data[write_bytes])) {
      write_bytes++;
      write_error = false;
    } else {         // failed to write, set timeout
      if (write_error) {
        if ((millis() - start_millis) > timeout) {
          DBG("Failed to write data to WiFly\r\n");
          return false;
        }
      } else {
        write_error = true;
        start_millis = millis();
      }
    }
  }
  flush();
  
  if (ack != NULL) {
    setTimeout(DEFAULT_WAIT_RESPONSE_TIME);
    return find((char *)ack);
  }
  return true;
}

boolean WiFly::sendCommand(const char *cmd, const char *ack, int timeout)
{
  if (!command_mode)
    commandMode();
  
  DBG(">");
  DBG(cmd);
  if (!send((uint8_t *)cmd, strlen(cmd), ack, timeout)) {
    DBG("\r\nFailed\r\n");
    return false;
  }
  DBG("\r\nOK\r\n");
  return true;
}
  
boolean WiFly::commandMode()
{
  if (command_mode)
    return true;
    
  DBG("Enter command mode: ");
  if (!send((uint8_t *)"$$$", 3, "CMD")) {
    if (!send((uint8_t *)"\r", 1, "ERR")) {
      DBG("Failed\r\n");
      return false;
    }
  }
  DBG("OK\r\n");
  command_mode = true;
  return true;
}

boolean WiFly::dataMode()
{
  if (command_mode) {
    DBG("Enter data mode: ");
    if (!send((uint8_t *)"exit\r", 5, "EXIT")) {
      if (send((uint8_t *)"\r", 1, "ERR")) {
        DBG("Failed\r\n");
        return false;
      }
    }
    DBG("OK\r\n");
    command_mode = false;
  }
  return true;
}

void WiFly::clear()
{
  while (timedRead(100) >= 0) {
  }
}

int WiFly::timedRead(unsigned int timeout)
{
  int c;
  unsigned long end_millis = millis() + timeout;
  do {
    c = read();
    if (c >= 0) return c;
  } while (millis() < end_millis);
  return -1;
}

#if 0
int WiFly::write(const uint8_t *buf, int len)
{
  size_t write_bytes = 0;
  while (write_bytes < len) {
    if (SoftwareSerial::write(buf[write_bytes]) == 0) {
      break;
    }
    write_bytes++;
  }
  return write_bytes;
}
#endif
