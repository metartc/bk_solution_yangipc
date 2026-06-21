#ifndef __YANGIPC_NETWORK_H__
#define __YANGIPC_NETWORK_H__

#include "lwip/sockets.h"
#include "net.h"

#define IP_QOS_PRIORITY_HIGHEST         (0xD0)
#define IP_QOS_PRIORITY_HIGH            (0xA0)
#define IP_QOS_PRIORITY_LOW             (0x20)
#define IP_QOS_PRIORITY_LOWEST          (0x00)

#define YANGIPC_SEND_MAX_RETRY (2000)
#define YANGIPC_SEND_MAX_DELAY (10)

int yangipc_wifi_sta_connect(char *ssid, char *key);
int yangipc_wifi_soft_ap_start(char *ssid, char *key, uint16_t channel);

int yangipc_socket_set_qos(int fd, int qos);
int yangipc_socket_sendto(int *fd, const struct sockaddr *dst, uint8_t *data, uint32_t length, int offset);
int yangipc_socket_write(int *fd, uint8_t *data, uint32_t length, int offset);

#endif
