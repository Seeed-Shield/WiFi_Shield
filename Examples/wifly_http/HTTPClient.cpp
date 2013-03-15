
#include <string.h>
#include "HTTPClient.h"
#include "Debug.h"

HTTPClient::HTTPClient()
{
  wifly = WiFly::getInstance();
}

int HTTPClient::get(const char *url, char *response, int len, int timeout)
{
  char host[32];
  uint16_t port;
  char path[64];
  
  if (parseURL(url, host, sizeof(host), &port, path, sizeof(path)) != 0) {
    DBG("Failed to parse URL.\r\n");
    return -1;
  }
  
  if (port == 0) {
    port = 80;
  }
  
  if (!wifly->connect(host, port)) {
    DBG("Failed to connect.\r\n");
    return -2;
  }
  
  // Send request
  char buf[256];
  snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\nHost: %s\r\n", "GET", path, host);
  if (!wifly->send(buf)) {
    DBG("Failed to send request.\r\n");
    return -3;
  }
  
  // Send all headers
  wifly->send("Connection: close\r\n");
  
  // Close headers
  wifly->send("\r\n", 2);
  
  // Send body
  
  // Receive response
  return wifly->receive((uint8_t *)response, len, timeout);
}

int HTTPClient::post(const char *url, const char *data, char *respose, int len, int timeout)
{
  return 0;
}

int HTTPClient::parseURL(const char *url, char *scheme, int max_scheme_len, char *host, int max_host_len, uint16_t *port, char *path, int max_path_len)
{
  char *scheme_ptr = (char *)url;
  char *host_ptr = (char *)strstr(url, "://");
  if (host_ptr == NULL) {
    DBG("Could't find host.\r\n");
    return -1;
  }
  
  int scheme_len = host_ptr - scheme_ptr;
  if (max_scheme_len < (scheme_len + 1)) {
    DBG("Scheme buffer is too small.\r\n");
    return -2;
  }
  memcpy(scheme, scheme_ptr, scheme_len);
  scheme[scheme_len] = '\0';
  
  host_ptr += 3;
  int host_len = 0;
  
  char *port_ptr = strchr(host_ptr, ':');
  if (port_ptr != NULL) {
    host_len = port_ptr - host_ptr;
    port_ptr++;
    if (sscanf(port_ptr, "%hu", port) != 1) {
      DBG("Could not find port.\r\n");
      return -3;
    }
  } else {
    *port = 0;
  }
  
  char *path_ptr = strchr(host_ptr, '/');
  if (host_len == 0) {
    host_len = path_ptr - host_ptr;
  }
  
  if (max_host_len < (host_len + 1)) {
    DBG("Host buffer is too small.\r\n");
    return -4;
  }
  memcpy(host, host_ptr, host_len);
  host[host_len] = '\0';
  
  int path_len;
  char *fragment_ptr = strchr(host_ptr, '#');
  if (fragment_ptr != NULL) {
    path_len = fragment_ptr - path_ptr;
  } else {
    path_len = strlen(path_ptr);
  }
  
  if (max_path_len < (path_len + 1)) {
    DBG("Path buffer is too small.\r\n");
    return -5;
  }
  memcpy(path, path_ptr, path_len);
  path[path_len] = '\0';
  
  return 0;
}

int HTTPClient::parseURL(const char *url, char *host, int max_host_len, uint16_t *port, char *path, int max_path_len)
{
  char *scheme_ptr = (char *)url;
  char *host_ptr = (char *)strstr(url, "://");
  if (host_ptr != NULL) {
    if (strncmp(scheme_ptr, "http://", 7)) {
      DBG("Bad scheme\r\n");
      return -1;
    }
    host_ptr += 3;
  } else {
    host_ptr = (char *)url;
  }

  int host_len = 0;
  char *port_ptr = strchr(host_ptr, ':');
  if (port_ptr != NULL) {
    host_len = port_ptr - host_ptr;
    port_ptr++;
    if (sscanf(port_ptr, "%hu", port) != 1) {
      DBG("Could not find port.\r\n");
      return -3;
    }
  } else {
    *port = 80;
  }
  
  char *path_ptr = strchr(host_ptr, '/');
  if (host_len == 0) {
    host_len = path_ptr - host_ptr;
  }
  
  if (max_host_len < (host_len + 1)) {
    DBG("Host buffer is too small.\r\n");
    return -4;
  }
  memcpy(host, host_ptr, host_len);
  host[host_len] = '\0';
  
  int path_len;
  char *fragment_ptr = strchr(host_ptr, '#');
  if (fragment_ptr != NULL) {
    path_len = fragment_ptr - path_ptr;
  } else {
    path_len = strlen(path_ptr);
  }
  
  if (max_path_len < (path_len + 1)) {
    DBG("Path buffer is too small.\r\n");
    return -5;
  }
  memcpy(path, path_ptr, path_len);
  path[path_len] = '\0';
  
  return 0;
}


  
