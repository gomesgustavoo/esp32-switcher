#include "udp_server.h"
#include "driver/gpio.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "lwip/sockets.h"

static const char *TAG = "ETHERNET";

#define PORT 5000  // Port to listen on

static esp_netif_t *eth_netif = NULL;

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
    .sw_reset_timeout_ms = 100,   // Default software reset timeout
    .rx_task_stack_size = 4096,  // Default RX task stack size
    .rx_task_prio = 15,          // Default RX task priority
    };
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_mac_config, &mac_config);

    // Configure PHY for WT32-ETH01
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = 1;              // Default PHY address
    phy_config.reset_gpio_num = -1;       // No reset GPIO
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

        // Echo the message back to the sender
        int err = sendto(sock, rx_buffer, len, 0, (struct sockaddr *)&source_addr, addr_len);
        if (err < 0) {
            ESP_LOGE(TAG, "Error sending response: errno %d", errno);
            break;
        }
    }
    // Cleanup
    close(sock);
    ESP_LOGI(TAG, "UDP server stopped");
    return ESP_OK;
}
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

