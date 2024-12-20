#include "declaracoes.h"			//Declaracoes p/ AFV801CS_v2.01
#include "definicoes.h"				//Definicoes p/ AFV801CS_v2.01
#include "registrosPCA9506.h"
//Includes de bibliotecas que permitem o funcionamento correto do ESP, nativas do ESP-IDF
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include <stdint.h>
#include <stdio.h>


/**************************************************************************
 Registros do PCA9506 já em arrays, facilitando implementação de rotinas
 de comunicação I2C.
***************************************************************************/
unsigned char input_port_register_bank[] = {
	INPUT_PORT_REGISTER_BANK_0,
	INPUT_PORT_REGISTER_BANK_1,
	INPUT_PORT_REGISTER_BANK_2,
	INPUT_PORT_REGISTER_BANK_3,
	INPUT_PORT_REGISTER_BANK_4
};

unsigned char output_port_register_banks[] = {
	OUTPUT_PORT_REGISTER_BANK_0,
	OUTPUT_PORT_REGISTER_BANK_1,
	OUTPUT_PORT_REGISTER_BANK_2,
	OUTPUT_PORT_REGISTER_BANK_3,
	OUTPUT_PORT_REGISTER_BANK_4
};

unsigned char io_configuration_register_banks[] = {
	IO_CONFIGURATION_REGISTER_BANK_0,
	IO_CONFIGURATION_REGISTER_BANK_1,
	IO_CONFIGURATION_REGISTER_BANK_2,
	IO_CONFIGURATION_REGISTER_BANK_3,
	IO_CONFIGURATION_REGISTER_BANK_4
};

unsigned char mask_interrupt_register_banks[] = {
	MASK_INTERRUPT_REGISTER_BANK_0,
	MASK_INTERRUPT_REGISTER_BANK_1,
	MASK_INTERRUPT_REGISTER_BANK_2,
	MASK_INTERRUPT_REGISTER_BANK_3,
	MASK_INTERRUPT_REGISTER_BANK_4
};

unsigned char polarity_inversion_register_banks[] = {
	POLARITY_INVERSION_REGISTER_BANK_0,
	POLARITY_INVERSION_REGISTER_BANK_1,
	POLARITY_INVERSION_REGISTER_BANK_2,
	POLARITY_INVERSION_REGISTER_BANK_3,
	POLARITY_INVERSION_REGISTER_BANK_4
};

extern unsigned char timeoutI2C;

unsigned char bufferLeituraPCA1[7+5+5+5];
unsigned char bufferLeituraPCA1_imediatamenteAposPolling[7+5+5+5];

// Função para fazer a configuração específica do i2c para o ESP32
void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

/************************************************************************************

Inicialização dos dois PCAs do teclado. Por padrão, todas as teclas ficam configuradas
como entradas. Para acender os LEDs, as rotinas devem configurar o pino como saída
antes de escrever.

*************************************************************************************/
void inicializaPCAs_Individualmente(unsigned char i2cAddress) {
    uint8_t cntTmp;
    i2c_cmd_handle_t cmd;
    esp_err_t status;

    // Primeiro passo na configuração
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i2cAddress << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, (io_configuration_register_banks[0] | 0x80), true);  // Ativa auto-incremento

    // Configura todos os 5 bancos como entradas (0xFF)
    for (cntTmp = 0; cntTmp < 5; cntTmp++) {
        i2c_master_write_byte(cmd, 0xFF, true);
    }
    i2c_master_stop(cmd);
    status = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK) {
        ESP_LOGE("I2C", "Erro ao inicializar o PCA, erro: %s", esp_err_to_name(status));
    } else {
        ESP_LOGI("I2C", "PCA inicializado corretamente, 5 bancos configurados como entrada");
    }
    i2c_cmd_link_delete(cmd);


    // Configura todas as saídas para manter LEDs apagados inicialmente    
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i2cAddress << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, (output_port_register_banks[0] | 0x80), true);
    for (cntTmp = 0; cntTmp < 5; cntTmp++) {
        i2c_master_write_byte(cmd, 0x00, true);  // LEDs apagados
    }
    i2c_master_stop(cmd);
    status = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK){
        ESP_LOGE("I2C", "Erro ao configurar as saídas para manter os leds apagados inicialmente, erro: %s", esp_err_to_name(status));
    }
    else {
        ESP_LOGI("I2C", "Sucesso ao configurar as saídas para manter os leds apagados inicialmente");
    }
    i2c_cmd_link_delete(cmd);
    
    // Leitura para atualizar variáveis
    if (i2cAddress == HARDWARE_VERSION_56TECLASSEMEXPANSAO) {
        leRegistro(i2cAddress, (input_port_register_bank[0] | 0x80), &bufferLeituraPCA1[0]);
       //leRegistro(i2cAddress, (input_port_register_bank[0] | 0x80), &bufferLeituraPCA1_imediatamenteAposPolling[0]);
    } else { 
        leRegistro(i2cAddress, (input_port_register_bank[0] | 0x80), &bufferLeituraPCA1[0]);
        //leRegistro(i2cAddress, (input_port_register_bank[0] | 0x80), &bufferLeituraPCA1_imediatamenteAposPolling[0]);
    }
    
    // Mantém polaridade original
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i2cAddress << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, (polarity_inversion_register_banks[0] | 0x80), true);
    for (cntTmp = 0; cntTmp < 5; cntTmp++) {
        i2c_master_write_byte(cmd, 0x00, true);  // Polaridade normal
    }
    i2c_master_stop(cmd);
    status = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK){
        ESP_LOGE("I2C", "Erro ao manter a polaridade original do PCA, erro: %s", esp_err_to_name(status));
    }
    else {
        ESP_LOGI("I2C", "Sucesso ao manter a polaridade original");
    }
    i2c_cmd_link_delete(cmd);
    
}

void inicializaPCAs(void)
{
    //ESP Rotina de inicialização de PCAs especifica para M1-56t
    inicializaPCAs_Individualmente(ENDERECO_PCA_2_MM1300);
    printf("inicialização do primeiro PCA concluída, continuando a rotina \n");

    //ESP por estar utilizando um PCA de menos pinos, sem registradores inicializar é necessário
    //inicializaPCAs_Individualmente(0x21);
    
    /*Rotina de inicialização de PCAs original
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSEMEXPANSAO)) ==
		(HARDWARE_VERSION_56TECLASSEMEXPANSAO)) //inicializa base
	{ 
		inicializaPCAs_Individualmente(ENDERECO_PCA_2_MM1300);
	}
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) //se teclado expansão 1 presente
	{ 
		inicializaPCAs_Individualmente(ENDERECO_PCA_3_MM1200_A);
	}
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) //se teclado expansão 2 presente
	{
		inicializaPCAs_Individualmente(ENDERECO_PCA_3_MM1200_B); 
	}
	
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) //se teclado expansão 3 presente
	{
		inicializaPCAs_Individualmente(ENDERECO_PCA_3_MM1200_C);
	}
    */
	
}

void leRegistro(unsigned char endereco, unsigned char reg, unsigned char *buf) {
    esp_err_t status;  // Responsavel por retornar se houve erro durante o envio dos comandos i2c
    i2c_cmd_handle_t cmd;
    unsigned char cntTmp; 

    // Cria um comando I2C e envia Start
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_WRITE, true);

    // Envia o endereço do registro que queremos ler
    i2c_master_write_byte(cmd, reg, true);

    // Envia Repeat Start para iniciar a leitura
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_READ, true);

    // Lê os primeiros bytes com ACK, mantendo cntTmp no loop
    for (cntTmp = 0; cntTmp < 4; cntTmp++) {
        *buf = i2c_master_read_byte(cmd, buf, I2C_MASTER_ACK);
        buf++;
    }
    // Le o ultimo byte com NACK para finalizar a comunicação corretamente
    *buf = i2c_master_read_byte(cmd, buf, I2C_MASTER_NACK);

    // Envia Stop para finalizar a transação
    i2c_master_stop(cmd);

    // Executa o comando e armazena o resultado em `status`
    status = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if (status != ESP_OK) {
        // Mensagem de erro se houver problema na comunicação
        // É possivel verificar com maior clareza qual erro ocorreu, não está implementado
        printf("Erro na leitura do registro: %s\n", esp_err_to_name(status));
    }
}

unsigned int lePCA8575(unsigned char endereco)
{
    esp_err_t status;
    unsigned int retorno;
    //Separa em dois bytes, dividindo a leitura e futuramente serão juntos como retorno
    unsigned char PCA8575_ReadByte0; 
    unsigned char PCA8575_ReadByte1;

    //Inicializa o retorno e os bytes menos e mais significativos
    retorno = 0x0000;
    PCA8575_ReadByte0 = 0;
    PCA8575_ReadByte1 = 0;
    
    //Cria o buffer de comandos para a comunicação i2c
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    //Inicializa o i2c enviando o Start, depois envia que a operação será de leitura
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_READ, true);

    //Realiza a leitura do PCA8575, dividindo a leitura em 2 de modo que mantenha o padrão de envio NACK pro ultimo
    i2c_master_read_byte(cmd, &PCA8575_ReadByte0, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &PCA8575_ReadByte1, I2C_MASTER_NACK);

    //Finaliza o buffer de comandos
    i2c_master_stop(cmd);
    //Inicia o envio dos comandos i2c do buffer, é possível implementar um caso de erro mais explicativo
    status = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK) {
        ESP_LOGE("I2C", "Erro ao executar a Leitura do PCA8575");
        return retorno; // Retorna 0 em caso de erro
    }
    //Finaliza a comunicação i2c
    i2c_cmd_link_delete(cmd);

    //Montagem da variável retorno, juntando os bytes menos e mais significativos
    retorno = PCA8575_ReadByte1;
    retorno = (retorno << 8) & 0xFF00;
    retorno |= PCA8575_ReadByte0;

    return retorno;
}

void lePCA8575RegistroUnico(unsigned char endereco, unsigned char * buf)
{
	unsigned int retorno;
	unsigned char PCA8575_ReadByte;
	
	PCA8575_ReadByte = 0;
	retorno = lePCA8575(endereco);
	
	PCA8575_ReadByte = (retorno & 0x00FF);
	*buf = PCA8575_ReadByte;
	buf++;
	PCA8575_ReadByte = ((retorno>>8) & 0x00FF);
	*buf = PCA8575_ReadByte;
}

void leRegistroUnico(unsigned char endereco, unsigned char reg, unsigned char * buf)
{
    esp_err_t status;
    //Cria o buffer de comandos para a comunicação i2c
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    //Inicio da comunicação i2c
    i2c_master_start(cmd);
    //Envia o endereço com a operação de escrita para o próximo comando
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_WRITE, true);
    //Envia o registrador que será lido
    i2c_master_write_byte(cmd, reg, true);

    //Inicia novamente a comunicação i2c mas agora será leitura
    i2c_master_start(cmd);
    //Indica que será uma operação de leitura
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_READ, true);
    //Realiza a leitura com NACK
    *buf = i2c_master_read_byte(cmd, buf, I2C_MASTER_NACK);
    //Finaliza a comunicação
    i2c_master_stop(cmd);
    //Executa o comando, é possível implementar um caso de erro mais explicativo
    status = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK) {
        ESP_LOGE("I2C", "Erro ao executar a Leitura de Registro Unico no PCA9506");
    }/*ESP Debug
    else {
        ESP_LOGI("I2C", "Sucesso ao ler registro único no PCA9506");
    }
    */
    //Libera o buffer de comandos
    i2c_cmd_link_delete(cmd);
}

void escreveRegistro(unsigned char endereco, unsigned char reg, unsigned char valor)
{
    esp_err_t status;
    //Cria o buffer de comandos para a comunicação i2c
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    //Inicio da comunicação i2c
    i2c_master_start(cmd);
    //Define uma operação de escrita, envia o registrador que será ecrito e por fim envia o valor a ser escrito
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, valor, true);
    //Finaliza a comunicação i2c
    i2c_master_stop(cmd);
    //Executa o buffer de comandos, é possivel implementar casos de erro mais explicativos
    status = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK) {
        ESP_LOGE("I2C", "Erro ao executar a Escrita de Registro no PCA9506");
    }/*ESP Debug
    else {
        ESP_LOGI("I2C", "Sucesso ao escrever no PCA9506, reg: %u, valor: %u", reg, valor);
    }
    */
    //Libera o buffer de comandos
    i2c_cmd_link_delete(cmd);
}

void escrevePCA8575(unsigned char endereco, unsigned char valor)
{
    esp_err_t status;
    //Cria o buffer de comandos para a comunicação i2c
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    //Inicio da comunicação i2c
    i2c_master_start(cmd);
    //Define uma operação de escrita e escreve
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, valor, true);

    //Finaliza a comunicação i2c
    i2c_master_stop(cmd);
    //Executa o buffer de comandos, é possivel implementar casos de erro mais explicativos
    status = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK) {
        ESP_LOGE("I2C", "Erro ao executar a Escrita de Registro no PCA9506");
    }
    //Libera o buffer de comandos
    i2c_cmd_link_delete(cmd);
}
//ESP
void escreve_2bytes_PCA8575(unsigned char endereco, unsigned char valor)
{
    esp_err_t status;
    //Cria o buffer de comandos para a comunicação i2c
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    uint16_t data;

    data = (valor << 8 );
    data = data | valor;
    //printf("debug, data: %d, valor: %u\n", data, valor);

    uint8_t low_byte = data & 0xFF;        // Byte menos significativo (P0 a P7)
    uint8_t high_byte = (data >> 8) & 0xFF; // Byte mais significativo (P8 a P15)
    //printf("debug, low_byte = %d e high_byte = %d", low_byte, high_byte);

    //Inicio da comunicação i2c
    i2c_master_start(cmd);
    //Define uma operação de escrita e escreve
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, low_byte, true);  // Escreve os pinos P0 a P7
    i2c_master_write_byte(cmd, high_byte, true);  // Escreve os pinos P8 a P15

    //Finaliza a comunicação i2c
    i2c_master_stop(cmd);
    //Executa o buffer de comandos, é possivel implementar casos de erro mais explicativos
    status = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK) {
        ESP_LOGE("I2C", "Erro ao executar a Escrita de Registro no PCA9506");
    }
    //Libera o buffer de comandos
    i2c_cmd_link_delete(cmd);
}
unsigned char CheckPcaDevice(unsigned char endereco)
{
    esp_err_t status;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);

    status = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK) {
        printf("PCA no endereço %u não encontrado, ERRO: %s\n",endereco, esp_err_to_name(status));
        return 1;
    	}
    else {
        return 0;
    	}
        
    i2c_cmd_link_delete(cmd);
}

void escreve5RegistrosBurst(unsigned char endereco, unsigned char reg, unsigned char valor0,
                                                                       unsigned char valor1,
                                                                       unsigned char valor2,
                                                                       unsigned char valor3,
                                                                       unsigned char valor4)
{
    esp_err_t status;
    unsigned char i;
    //Cria o buffer de comandos para a comunicação i2c
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    //Inicio da comunicação i2c
    i2c_master_start(cmd);
    //Define uma operação de escrita, envia o registrador que será ecrito e por fim envia o valor a ser escrito
    i2c_master_write_byte(cmd, (endereco << 1) | I2C_MASTER_WRITE, true);
    //Ativa o auto incremento na escrita dos registradores
    i2c_master_write_byte(cmd, (reg | 0x80), true);
	//Loop de envio, enviando o valor referente a cada registrador maximo de 5 iterações
    for (i = 0; i < 5; i++)
	{
        if (i==0)
            i2c_master_write_byte(cmd, valor0, true);
        if (i==1)
            i2c_master_write_byte(cmd, valor1, true);
        if (i==2)
            i2c_master_write_byte(cmd, valor2, true);
        if (i==3)
            i2c_master_write_byte(cmd, valor3, true);
        if (i==4)
            i2c_master_write_byte(cmd, valor4, true);
    }
    //Finaliza a comunicação i2c
    i2c_master_stop(cmd);
    //Faz o envio dos comandos do buffer e retorna erro caso necessário
    status = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    if (status != ESP_OK) {
        printf("ERRO: %s\n", esp_err_to_name(status));
    }
    /*
    else{
        printf("Sucesso ao escrever os 5 valores: %u, %u, %u, %u, %u\n",valor0,
                                                                             valor1,
                                                                             valor2,
                                                                             valor3,
                                                                             valor4);                                                                            
    }
    */
    //Libera o buffer
    i2c_cmd_link_delete(cmd);
}
