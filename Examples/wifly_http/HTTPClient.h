

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#define HTTP_CLIENT_DEFAULT_TIMEOUT        30000  // 3s

#include <Arduino.h>
#include <WiFly.h>

class HTTPClient {
  public:
    HTTPClient();
    
    int get(const char *url, char *response, int len, int timeout = HTTP_CLIENT_DEFAULT_TIMEOUT);
    int post(const char *url, const char *data, char *respose, int len, int timeout = HTTP_CLIENT_DEFAULT_TIMEOUT);
    
//  private:
    int parseURL(const char *url, char *scheme, int max_scheme_len, char *host, int max_host_len, uint16_t *port, char *path, int max_path_len);
    int parseURL(const char *url, char *host, int max_host_len, uint16_t *port, char *path, int max_path_len);
 
    WiFly* wifly;
};

#endif // __HTTP_CLIENT_H__

