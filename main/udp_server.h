#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include "esp_err.h"
#include "esp_netif.h"

// Function to initialize and start the UDP server
esp_err_t start_udp_server(void);
void log_ip_address(esp_netif_t *netif);

#endif // UDP_SERVER_H
