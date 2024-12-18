#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include "esp_err.h"
#include "esp_netif.h"
#include "lwip/sockets.h"

// Define o tamanho máximo de buffer para comandos UDP
#define MAX_BUFFER_SIZE 256

// Estrutura para representar um comando UDP recebido
typedef struct {
    struct sockaddr_in source_addr;
    char command[MAX_BUFFER_SIZE];
} udp_command_t;

// Declarações de funções disponíveis para outros arquivos
esp_err_t start_udp_server(void);
esp_err_t configure_ethernet(void);
void log_ip_address(esp_netif_t *netif);
void process_command(const udp_command_t *cmd, int sock);
void parse_and_execute(const char *command, struct sockaddr_in *source_addr, int sock);
void buttonDown(unsigned char buttonId);

#endif // UDP_SERVER_H
