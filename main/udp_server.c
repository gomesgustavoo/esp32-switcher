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


static esp_netif_t *eth_netif = NULL;

int g_sock =-1;
struct sockaddr_in g_client_addr;
bool g_client_addr_initialized = false;

//Teste de fila de comandos com notification para corrigir o problema do TAKE
udp_command_t command_buffer[30];
volatile int head = 0;
volatile int tail = 0;

TaskHandle_t processor_task_handle;
static volatile bool processor_awake = false;

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
    .sw_reset_timeout_ms = 100,   // D
    .rx_task_stack_size = 4096, 
    .rx_task_prio = 15,          
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

    int buff_size = 8192;
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buff_size, sizeof(buff_size));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Socket bind failed: errno %d", errno);
        close(sock);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "UDP server listening on port %d", PORT);
    // Configurar o socket para não bloquear
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    // Listen for incoming data
    char rx_buffer[512];
    struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);
    g_sock = sock;

    xTaskCreatePinnedToCore(process_commands, "CommandProcessor", 4096, NULL, configMAX_PRIORITIES - 2, &processor_task_handle, 1);

    while (1) {
        int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0,
                           (struct sockaddr *)&source_addr, &addr_len);

         if (len < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                vTaskDelay(pdMS_TO_TICKS(10));
                continue;
            } else {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
        }

        rx_buffer[len] = '\0';  // Null-terminate received data
        //printf("Debug rx_buffer: %s \n", rx_buffer);
        udp_command_t cmd;

        // Otimização para copiar apenas o necessário
        //size_t rx_length = strnlen(rx_buffer, MAX_BUFFER_SIZE - 1);
        memcpy(cmd.command, rx_buffer, len);
        cmd.command[len] = '\0';
        // Atribuir o endereço
        cmd.source_addr = source_addr;

        if (cmd.command[1] == '1' && cmd.command[2] == '6' && cmd.command[3] == '8') {
            if (cmd.command[0] == 'O') {
            processor_awake = true;
            continue;
            } else {
            processor_awake = false;            
            }
        }
        enqueue_command(&cmd);
        taskYIELD();
        //process_command(&cmd, sock);
    }
    // Cleanup
    close(sock);
    ESP_LOGI(TAG, "UDP server stopped");
    return ESP_OK;
}
/*
void process_command(const udp_command_t *cmd, int sock) {
    //ESP_LOGI(TAG, "Processando comando: '%s'", cmd->command);
    parse_and_execute(cmd->command, &cmd->source_addr, sock);
}
*/

//Função de parsing principal, direciona o fluxo lógico da operação desejada no comando e chama a função correspondente
void parse_and_execute(char *command, const struct sockaddr_in *source_addr, int sock) {
    //printf("Debug Parsing, %s\n", command);
    // Verifica se o comando inicia com 'F' ou 'O'
    if (command[0] == 'F' || command[0] == 'O') {
        char *end;
        unsigned char led_id = (unsigned char)strtol(command + 1, &end, 10);
        //printf("Debug led_id: %u \n", led_id);
        if (*end == '\0' || *end == '\n' || *end == '\r') {
            int action = (command[0] == 'F') ? COMANDO_KEYLED_OFF : COMANDO_KEYLED_ON;
            ManageKeyLeds(action, led_id);
        }
    // Ligar / Desligar todos os Leds da mesa
    } else if (command[0] == 'A') {
        if (command[1] == '0' || command[1] == '1') {
            inicializaStatusOfKeyBoardLeds();
        } else {
            ManageKeyLeds(COMANDO_KEYLED_ON, ALL_LEDS);
        }
    // Handshake, retorna uma resposta e salva o cliente no escopo global
    } else if (command[0] == 'H' && command[1] == 'I') {
        const char *response = "4S - Esp32 Mago Switcher :";
        sendto(sock, response, strlen(response), 0, (struct sockaddr *)source_addr, sizeof(*source_addr));
        ESP_LOGI(TAG, "Handshake enviado para %s:%d", inet_ntoa(source_addr->sin_addr), ntohs(source_addr->sin_port));
        g_client_addr = *source_addr;
        g_client_addr_initialized = true;
    }
}
    /*
    else if (strncmp(command, "IP", 2) == 0) {
        esp_netif_ip_info_t ip_info;
        if (esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("ETH_DEF"), &ip_info) == ESP_OK) {
            char response[80];
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
        esp_restart();

    // Comando não reconhecido
    } else {
        ESP_LOGW(TAG, "Comando não reconhecido: '%s'", command);
    }*/


void enqueue_command(const udp_command_t *cmd) {
    int next_head = (head + 1) % 30;

    if (next_head == tail) {
        ESP_LOGW("BUFFER", "Buffer overflow, dropping command");
    } else {
        command_buffer[head] = *cmd;
        head = next_head;

        // Notifica apenas se necessário
        if (!processor_awake) {
            processor_awake = true;
            xTaskNotifyGive(processor_task_handle);
        }
    }
}

void process_commands(void *pvParameters) {
    udp_command_t cmd;

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        processor_awake = true;

        while (head != tail) {
            cmd = command_buffer[tail];
            tail = (tail + 1) % 30;
            //printf("Debug process, cmd: %s \n", cmd.command);
            parse_and_execute(cmd.command, &cmd.source_addr, g_sock);
        }

        processor_awake = false; 
    }
}