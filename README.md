# Sobre
Repositório do projeto Mesa switcher de sinais, implementação da Mago Switcher da 4S com o microcontrolador Espressif ESP-32, o projeto espera como hardware a placa MM-1300 desenvolvida pela 4S, com o objetivo de enviar os comandos pela rede ao invés do protocolo USB como utilizado pelas mesas antecessoras, portanto em paralelo com a verificação dos botões, o ESP cria um UDP socket na porta 5000, onde envia teclas e recebe comandos.

# Desenvolvimento
O projeto foi escrito por mim, Gustavo com grande ajuda de colegas de trabalho sêniors que haviam desenvolvido mesas anteriores, fui designado para o projeto por demonstrar interesse grande no ESP32 e durante o projeto consegui extrair muito da capacidade total desse carinha, com a ajuda do freeRTOS separei em tasks e organizei os 2 núcleos físicos do ESP de modo que atingisse o máximo do desempenho

# Improves / Bug hunting
- [ ] Melhorar o tempo de resposta do led

# Organização de códigos fonte
*arquivos headers.h*
Todos os headers são extremamente importantes para o funcionamento do código portanto não removi apenas realizei alterações e adicionei declarações
- declaracoes.h
- definicoes.h
- registrosPCA9506.h
- udp_server.h

*arquivos source.c*
Já os arquivos de código fonte, alguns precisaram ser removidos para garantir o funcionamento adequado do código, principalmente os que envolviam diretamente o USB
*arquivos removidos*
- USBComm.c
- filaEnvio.c
- filaRecepcao.c
- timerManager.c -> esse foi removido pelo freeRTOS do esp não lidar bem com timers que não sejam nativos do sistema portanto se for necessário utilizarei os timers direto do scheduler do esp32
*arquivos fonte atualmente*
- main.c
- i2cManager.c
- KeysManager.c
- udp_server.c

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

# Comportamento na rede / API
*A mesa executa funções ao receber um dos seguintes comandos*
- O+*id* => Comando responsável por ligar um led isolado sendo id o endereço do led.
- F+*id* => Comando responsável por desligar um led isolado sendo id o endereço do led.
- A => Comando responsável por ligar todos os leds da placa.
- A+*valor qualquer* => Comando responsável por desligar todos os leds da placa.
- HI => *Handshake*, Comando responsável por responder ao cliente tentando se conectar.
- IP => Comando responsável por responder ao cliente informações sobre a rede, netmask gateway e endereço de IP.
- REBOOT => Reinicia o controlador Esp32, pode ser uma opção para contornar bugs, limpa os registros.

# Inicialização da Ethernet no ESP32
*configure_ethernet*
*Configuração da interface ethernet utilizando o módulo WT32-ETH01*

`ESP_ERROR_CHECK(esp_netif_init());
`ESP_ERROR_CHECK(esp_event_loop_create_default());
- Configura a Pilha TCP/IP e um loop de eventos para gerenciar as interações na rede.

`gpio_config_t gpio_cfg = { ... };
`ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
`ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_16, 1));
- O WT32-ETH01 utiliza o GPIO16 para gerar o clock RMII. Esse trecho configura o pino como saída e o ativa.

`esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
`eth_netif = esp_netif_new(&netif_cfg);
- Cria uma interface de rede Ethernet e inicia o cliente DHCP para obter um endereço IP.

`eth_esp32_emac_config_t esp32_mac_config = { ... };
`eth_mac_config_t mac_config = { ... };
`esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_mac_config, &mac_config);

`eth_phy_config_t phy_config = { ... };
`esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
- Configura os controladores ethernet:
	- *MAC*: (Media Access Controller): Define os pinos MDC/MDIO para comunicação.
	- *PHY*: (Physical Layer): Configura o chip LAN8720A para comunicação física.

`esp_eth_config_t eth_config = { ... };
`ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config, &eth_handle));
`ESP_ERROR_CHECK(esp_eth_start(eth_handle));
- Instala o driver e inicia a interface ethernet.

# Inicialização do Servidor UDP
*start_udp_server*
*Criação e configuração de um socket UDP na porta 5000*

`int sock = socket(AF_INET, SOCK_DGRAM, 0);
- Cria um socket UDP.

`struct sockaddr_in server_addr = { ... };
`bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
- Vincula o socket à porta 5000, permitindo que receba pacotes.

`while (1) {
    `int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, 
                       `(struct sockaddr *)&source_addr, &addr_len);
- Loop de Recepção, Aguarda a chegada de dados, quando recebe processa os comandos.

`udp_command_t cmd;
`cmd.source_addr = source_addr;
`strncpy(cmd.command, rx_buffer, MAX_BUFFER_SIZE - 1);
`cmd.command[MAX_BUFFER_SIZE - 1] = '\0';
`process_command(&cmd, sock);
- Processamento de comandos, os dados recebidos são armazenados na estrutura udp_command_t, e o comando é encaminhado para ser processado.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

# Endereçamento i2c
Ambos os expansores são controlados por i2c portanto precisam assumir um endereço, são eles:
- PCA9506DDG -> 0x22 
- PCA8575D -> 0x21
O endereço é definido pelos pinos A0, A1 e A2 em ambos os PCAs, no projeto [[Esp32 -> 4S-M1]] os endereços são os acima.


# Endereço dos registradores
ambos os PCAs possuem formas diferentes de controlar os registradores
### Para o PCA9506DDG
é dividido em 5 bancos de 8 pinos, as portas podem ser configuradas como entrada ou saída pelo DDR e o estado das portas é controlado pelo ODR
- DDR é o registrador que controla os pinos como entrada ou saída
	- 0x00 para configurar os pinos como *Output*
	- 0xFF para configurar os pinos como *Input*
- ODR é o registrador de saída que controla o estado dos pinos, HiGH ou LOW
	- 0xFF pinos configurados como *HIGH* e led *Desligado*
	- 0x00 pinos configurados como *LOW* e led *Ligado* 
- Registradores de entrada, cada banco possui um registrador de entrada separado
- Inversão de polaridade, controlam a inversçao de polaridade para as leituras da entrada
*DDR*
- P0_DIR = 0x18 -> pinos 0 à 7
- P1_DIR = 0x19 -> pinos 8 à 15
- P2_DIR = 0x1A -> pinos 16 à 23
- P3_DIR = 0x1B -> pinos 24 à 31
- P4_DIR = 0x1C -> pinos 32 à 39
*Input*
- P0_IN = 0x00 -> pinos 0 à 7
- P1_IN = 0x01 -> pinos 8 à 15
- P2_IN = 0x02 -> pinos 16 à 23
- P3_IN = 0x03 -> pinos 24 à 31
- P4_IN = 0x04 -> pinos 32 à 39
*ODR*
- P0_OUT = 0x08 -> pinos 0 à 7
- P1_OUT = 0x09 -> pinos 8 à 15
- P2_OUT = 0x0A -> pinos 16 à 23
- P3_OUT = 0x0B -> pinos 24 à 31
- P4_OUT = 0x0C -> pinos 32 à 39
### Para o PCA8575D
é dividido em 2 bancos de 8 pinos, não possui registrador de configuração, o estado da porta é configurado ao escrever diretamente na saída
- A leitura ou escrita ocorrem em uma única operação de 2 bytes, cada byde contem o estado de 8 pinos.
- Escrever nesses bytes define o estado do GPIO, bit alto(1) o pino funciona como uma entrada e bit baixo(0) como saída

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

# SDKCONFIG
## Essenciais
*Algumas configuracoes obrigatórias para o funcionamento do sistema*
- `ENABLE_BACKWARD_COMPATIBILITY` Habilita funcoes nativas de versoes antigas do kernel do FreeRTOS estou utilizando principalmente nos timer delays
