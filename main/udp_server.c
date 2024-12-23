#include "udp_server.h"
#include "declaracoes.h"
#include "definicoes.h"
#include "driver/gpio.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "lwip/sockets.h"
#include <string.h>

static const char *TAG = "ETHERNET";

#define PORT 5000  // Port to listen on
#define MAX_BUFFER_SIZE 256

static esp_netif_t *eth_netif = NULL;

// Endereço do cliente global
static int g_sock =-1;
static struct sockaddr_in g_client_addr;
static bool g_client_addr_initialized = false;

// Function to initialize Ethernet
esp_err_t configure_ethernet(void) {
    ESP_LOGI(TAG, "Configuring Ethernet for WT32-ETH01...");

    // Initialize the TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Enable external oscillator for RMII clock on GPIO16
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(GPIO_NUM_16),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
    ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_16, 1));
    ESP_LOGI(TAG, "External oscillator enabled on GPIO16");

    // Create a default Ethernet netif
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    eth_netif = esp_netif_new(&netif_cfg);
    if (!eth_netif) {
        ESP_LOGE(TAG, "Failed to create Ethernet netif");
        return ESP_FAIL;
    }
    ESP_ERROR_CHECK(esp_netif_dhcpc_start(eth_netif));


    // Configure MAC for WT32-ETH01
    eth_esp32_emac_config_t esp32_mac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp32_mac_config.smi_gpio.mdc_num = GPIO_NUM_23; // MDC pin
    esp32_mac_config.smi_gpio.mdio_num = GPIO_NUM_18; // MDIO pin

    eth_mac_config_t mac_config = {
    .sw_reset_timeout_ms = 100,   // Default
    .rx_task_stack_size = 4096,  // Default 
    .rx_task_prio = 15,          // Default
    };
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_mac_config, &mac_config);

    // Configure PHY for WT32-ETH01
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = 1;
    phy_config.reset_gpio_num = -1;
    phy_config.reset_timeout_ms = 200;
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);

    // Configure the Ethernet driver
    esp_eth_config_t eth_config = {
        .mac = mac,
        .phy = phy,
        .check_link_period_ms = 2000,  // Periodic link check
        .stack_input = NULL           // Use default TCP/IP stack input
    };
    esp_eth_handle_t eth_handle = NULL;

    // Install the Ethernet driver
    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config, &eth_handle));
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));

    ESP_LOGI(TAG, "Starting Ethernet driver...");
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    ESP_LOGI(TAG, "Ethernet interface configured and started");

    return ESP_OK;
}

//Inicializa o udp socket com afinidade no CPU 1, Loop infinito de leitura e processamento de comandos
esp_err_t start_udp_server(void) {
    ESP_LOGI(TAG, "Starting UDP server...");

    if (configure_ethernet() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure Ethernet");
        return ESP_FAIL;
    }

    // Create a socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create socket: errno %d", errno);
        return ESP_FAIL;
    }

    // Bind the socket to a port
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Socket bind failed: errno %d", errno);
        close(sock);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "UDP server listening on port %d", PORT);

    // Listen for incoming data
    char rx_buffer[128];
    struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);

    while (1) {
        int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0,
                           (struct sockaddr *)&source_addr, &addr_len);

        if (len < 0) {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            break;
        }

        rx_buffer[len] = '\0';  // Null-terminate received data
        ESP_LOGI(TAG, "Received %d bytes from %s:%d: '%s'", len,
                 inet_ntoa(source_addr.sin_addr), ntohs(source_addr.sin_port), rx_buffer);

        // Processar o comando recebido
        udp_command_t cmd;
        cmd.source_addr = source_addr;
        strncpy(cmd.command, rx_buffer, MAX_BUFFER_SIZE - 1);
        cmd.command[MAX_BUFFER_SIZE - 1] = '\0';

        process_command(&cmd, sock);
    }
    // Cleanup
    close(sock);
    ESP_LOGI(TAG, "UDP server stopped");
    return ESP_OK;
}

void process_command(const udp_command_t *cmd, int sock) {
    ESP_LOGI(TAG, "Processando comando: '%s'", cmd->command);
    parse_and_execute(cmd->command, &cmd->source_addr, sock);
}

//Função de parsing principal, direciona o fluxo lógico da operação desejada no comando e chama a função correspondente
void parse_and_execute(const char *command, struct sockaddr_in *source_addr, int sock) {
    // Ligar Led específico, O<id>
    if (strncmp(command, "O", 1) == 0) {
        unsigned long led_id = strtoul(command + 1, NULL, 10);
        unsigned char led_id_char = (unsigned char)led_id;
        ManageKeyLeds(COMANDO_KEYLED_ON, led_id_char);
        ESP_LOGI(TAG, "Ligar Led: %u", led_id_char);

    // Desligar Led específico F<id>
    } else if (strncmp(command, "F", 1) == 0) {
        unsigned long led_id = strtoul(command + 1, NULL, 10);
        unsigned char led_id_char = (unsigned char)led_id;
        ManageKeyLeds(COMANDO_KEYLED_OFF, led_id_char);
        ESP_LOGI(TAG, "Desligar LED: %d", led_id_char);

    // Ligar / Desligar todos os Leds da mesa
    } else if (strncmp(command, "A", 1) == 0) {
        int toggle = strtol(command + 1, NULL, 16);
        if (toggle) {
            const char *response = "Reiniciando o estado dos leds, ALL OFF ";
            sendto(sock, response, strlen(response), 0, (struct sockaddr *)source_addr, sizeof(*source_addr));
            inicializaStatusOfKeyBoardLeds();    
            ESP_LOGI(TAG, "ALL LEDS OFF");
        } else {
            ManageKeyLeds(COMANDO_KEYLED_ON, ALL_LEDS);
            ESP_LOGI(TAG, "Turn ALL LEDs ON");
        }

    // Handshake, retorna uma resposta e salva o cliente no escopo global
    } else if (strncmp(command, "HI", 2) == 0) {
        const char *response = "4S - Esp32 Mago Switcher :";
        sendto(sock, response, strlen(response), 0, (struct sockaddr *)source_addr, sizeof(*source_addr));
        ESP_LOGI(TAG, "Handshake enviado para %s:%d", 
                 inet_ntoa(source_addr->sin_addr), ntohs(source_addr->sin_port));
        // Salva o endereço do cliente em variáveis globais
        g_sock = sock;
        g_client_addr = *source_addr;
        g_client_addr_initialized = true;

    // Retorna o endereço de IP e outros dados ao cliente
    } else if (strncmp(command, "IP", 2) == 0) {
        esp_netif_ip_info_t ip_info;
        if (esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("ETH_DEF"), &ip_info) == ESP_OK) {
            char response[128];
            snprintf(response, sizeof(response), 
                     "IP Address: " IPSTR "\nNetmask: " IPSTR "\nGateway: " IPSTR,
                     IP2STR(&ip_info.ip), IP2STR(&ip_info.netmask), IP2STR(&ip_info.gw));
            sendto(sock, response, strlen(response), 0, (struct sockaddr *)source_addr, sizeof(*source_addr));
            ESP_LOGI(TAG, "Enviado IP info: %s", response);
        } else {
            const char *error_response = "Erro ao obter o IP";
            sendto(sock, error_response, strlen(error_response), 0, (struct sockaddr *)source_addr, sizeof(*source_addr));
            ESP_LOGE(TAG, "Erro ao obter informações de IP");
        }

    // Reboot do ESP32
    } else if (strncmp(command, "REBOOT", 6) == 0) {
        const char *response = "ESP32 está reiniciando...\n";
        sendto(sock, response, strlen(response), 0, (struct sockaddr *)source_addr, sizeof(*source_addr));
        ESP_LOGI(TAG, "Reboot solicitado pelo cliente: %s:%d", 
                 inet_ntoa(source_addr->sin_addr), ntohs(source_addr->sin_port));
        precise_delay_us(100000);
        esp_restart();

    // Comando não reconhecido
    } else {
        ESP_LOGW(TAG, "Comando não reconhecido: '%s'", command);
    }
}

//Indica que um botão foi pressionado na rede
void udp_send_buttonDown(unsigned char buttonId) {
    if (!g_client_addr_initialized || g_sock < 0) {
        ESP_LOGE(TAG, "Erro: Socket ou endereço do cliente não inicializado");
        return;
    }

    char response[10];

    snprintf(response, sizeof(response), "D%u", buttonId);

    int err = sendto(g_sock, response, strlen(response), 0, (struct sockaddr *)&g_client_addr, sizeof(g_client_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Erro ao enviar botão pressionado: %s", strerror(errno));
    } else {
        ESP_LOGI(TAG, "Mensagem enviada: %s (Bytes enviados: %d)", response, err);
    }
}

//Indica que um botão foi solto na rede
void udp_send_buttonUp(unsigned char buttonId) {
    if (!g_client_addr_initialized || g_sock < 0) {
        ESP_LOGE(TAG, "Erro: Socket ou endereço do cliente não inicializado");
        return;
    }

    char response[10];

    snprintf(response, sizeof(response), "U%u", buttonId);

    int err = sendto(g_sock, response, strlen(response), 0, (struct sockaddr *)&g_client_addr, sizeof(g_client_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Erro ao enviar botão pressionado: %s", strerror(errno));
    } else {
        ESP_LOGI(TAG, "Mensagem enviada: %s (Bytes enviados: %d)", response, err);
    }
}

//Função para imprimir o ip
void log_ip_address(esp_netif_t *netif) {
    esp_netif_ip_info_t ip_info;
    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
        ESP_LOGI(TAG, "IP Address: " IPSTR, IP2STR(&ip_info.ip));
        ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&ip_info.netmask));
        ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&ip_info.gw));
    } else {
        ESP_LOGE(TAG, "Failed to get IP address");
        }
    }

