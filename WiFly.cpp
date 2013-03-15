#include <string.h>
#include "WiFly.h"
#include "Debug.h"

WiFly* WiFly::instance;

WiFly::WiFly(uint8_t rx, uint8_t tx) : SoftwareSerial(rx, tx)
{
  instance = this;
  
  SoftwareSerial::begin(DEFAULT_BAUDRATE);
  setTimeout(DEFAULT_WAIT_RESPONSE_TIME);
  
  command_mode = false;
  associated = false;
}

boolean WiFly::init()
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

boolean WiFly::leave()
{
  if (sendCommand("leave\r", "DeAuth")) {
    associated = false;
    return true;
  }
  return false;
}

boolean WiFly::connect(const char *host, uint16_t port)
{
  char cmd[32];
  snprintf(cmd, sizeof(cmd), "set d n %s\r", host);
  sendCommand(cmd, "AOK");
  snprintf(cmd, sizeof(cmd), "set i r %d\r", port);
  sendCommand(cmd, "AOK");
  sendCommand("set i p 18\r", "AOK");
  sendCommand("set i a 0\r", "AOK");
  sendCommand("set c r 0\r", "AOK");
  return sendCommand("open\r", "*OPEN*", 10000);
}

int WiFly::send(const char *str, int timeout)
{
  send((uint8_t *)str, strlen(str), timeout);
}

int WiFly::send(const uint8_t *data, int len, int timeout)
{
  int write_bytes = 0;
  boolean write_error = false;
  unsigned long start_millis;
  
  while (write_bytes < len) {
    if (write(data[write_bytes])) {
      write_bytes++;
      write_error = false;
    } else {         // failed to write, set timeout
      if (write_error) {
        if ((millis() - start_millis) > timeout) {
          DBG("Send data. Timeout!\r\n");
          break;
        }
      } else {
        write_error = true;
        start_millis = millis();
      }
    }
  }
  
  return write_bytes;
}

boolean WiFly::ask(const char *q, const char *a, int timeout)
{
  int q_len = strlen(q);
  if (send((uint8_t *)q, q_len, timeout) != q_len) {
    return false;
  }
  
  if (a != NULL) {
    setTimeout(DEFAULT_WAIT_RESPONSE_TIME);
    return find((char *)a);
  }
  
  return true;
}

boolean WiFly::sendCommand(const char *cmd, const char *ack, int timeout)
{
  clear();
  
  if (!command_mode) {
    commandMode();
  }
  
  DBG(">");
  DBG(cmd);
  
  if (!ask(cmd, ack, timeout)) {
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
  if (!ask("$$$", "CMD", 600)) {
    if (!ask("\r", "ERR")) {
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
    if (!ask("exit\r", "EXIT")) {
      if (ask("\r", "ERR")) {
        DBG("Failed\r\n");
        return false;
      }
    }
    command_mode = false;
    
    DBG("OK\r\n");
  }
  return true;
}

void WiFly::clear()
{
  char r;
  while (receive((uint8_t *)&r, 1, 100) == 1) {
  }
}

int WiFly::receive(uint8_t *buf, int len, int timeout)
{
  int read_bytes = 0;
  int ret;
  unsigned long end_millis;
  
  while (read_bytes < len) {
    end_millis = millis() + timeout;
    do {
      ret = SoftwareSerial::read();
      if (ret >= 0) {
        break;
     }
    } while (millis() < end_millis);
    
    if (ret < 0) {
      return read_bytes;
    }
    buf[read_bytes] = (char)ret;
    read_bytes++;
  }
}
