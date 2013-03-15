
#ifndef __WIFLY_H__
#define __WIFLY_H__

#include <Arduino.h>
#include <SoftwareSerial.h>

#define DEFAULT_WAIT_RESPONSE_TIME      600        // 600ms
#define DEFAULT_BAUDRATE                9600
#define MAX_CMD_LEN                     32
#define MAX_TRY_JOIN                    3

// Auth Modes for Network Authentication
// See WiFly manual for details
#define WIFLY_AUTH_OPEN        0    // Open (default)  
#define WIFLY_AUTH_WEP         1    // WEP-128
#define WIFLY_AUTH_WPA1        2    // WPA1
#define WIFLY_AUTH_WPA1_2      3    // Mixed-mode WPA1 and WPA2-PSK
#define WIFLY_AUTH_WPA2_PSK    4    // WPA2-PSK
#define WIFLY_AUTH_ADHOC       6    // Ad-hoc, join any Ad-hoc network

class WiFly : public SoftwareSerial
{
  public:
    WiFly(uint8_t rx, uint8_t tx);
    
    boolean setDefaultSettings();
    
    boolean init();
    boolean staticIP(const char *ip, const char *mask, const char *gateway);
    
    boolean join(const char *ssid, const char *phrase = NULL, int auth = WIFLY_AUTH_OPEN);
    boolean leave();
    
    boolean send(const uint8_t *data, int data_len, const char *ack = NULL, int timeout = DEFAULT_WAIT_RESPONSE_TIME);
    boolean sendCommand(const char *cmd, const char *ack = NULL, int timeout = DEFAULT_WAIT_RESPONSE_TIME);
  
    boolean commandMode();
    boolean dataMode();
    
    void clear();
    
    int timedRead(unsigned int timeout);
 //   int write(const uint8_t *buf, int len);
    
  private:
    boolean command_mode;
    boolean associated;
    uint8_t dhcp;
    
};

#endif // __WIFLY_H__

