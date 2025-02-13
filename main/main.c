/*****************************************************************************
(C) Copyright 2025 - 4S Informatica Ind. e Com. LTDA. All rights reserved.

File Name: main.c

Projeto:	Teclado Mago na Rede 

Data: 05/02/2025			Rev 2.0

Autor: Gustavo Gomes Formento

Referência: Eduardo Artur Cunha

Compilador: ESP-IDF

Hardware: ESP32 Wirelass Tag WT32-eth01

Descricao:	Arquivo Main do projeto, responsável por inicializar os PCA's e fazer o
setup das tasks para o FreeRTOS.
*****************************************************************************/
#include "declaracoes.h"
#include "definicoes.h"
#include "registrosPCA9506.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "udp_server.h"


unsigned char timerTick;

extern unsigned char AuxVarToBlinkBanks[7+5+5+5];
unsigned char AuxVarToShowIfEncoderAandBAreConnected;
unsigned char AuxVarToShowVersionOfHardwareBoard;

extern unsigned char bufferLeituraPCA1[7+5+5+5];
extern unsigned char bufferLeituraPCA1_seminterrupcao[7+5+5+5];
extern unsigned char ArrayIndicaTecla[7+5+5+5][8];

extern unsigned char StatusOfKeyBoardLeds[7+5+5+5][2];
extern unsigned char StatusOfEncoderBoardLeds[2];

void app_main(void)
{
	//Inicializa o i2c, instala os drivers necessários
	i2c_master_init();

	//Variável responsável por auxiliar as iterações nos laços
	unsigned char cntTmp;

	inicializaArrayIndicaTecla();
		
	/***********************************************************************************
	 Confere se qual tipo de teclado está em operação
	************************************************************************************/
	//ESP Primeira sequencia de logs no monitor serial, ESP_OK quando encontra o PCA e ESP_FAIL quando não encontra
	
	AuxVarToShowVersionOfHardwareBoard = 0x00;
	
	if (CheckPcaDevice(ENDERECO_PCA_2_MM1300))
	{
		printf("Debug: Hardware version 56t sem expansao\n");
		AuxVarToShowVersionOfHardwareBoard |= HARDWARE_VERSION_56TECLASSEMEXPANSAO;
	}
	
	if (CheckPcaDevice(ENDERECO_PCA_3_MM1200_A))
	{
		printf("Debug: Hardware version 56t + pca 1\n");
		AuxVarToShowVersionOfHardwareBoard |= HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED;
	}

	if (CheckPcaDevice(ENDERECO_PCA_3_MM1200_B))
	{
		printf("Debug: Hardware version 56t + pca 2\n");
		AuxVarToShowVersionOfHardwareBoard |= HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED;
	}

	/***********************************************************************************
	inicializaPCAs
	************************************************************************************/
	//ESP Rotina de inicialização dos PCAs alterada para o funcionamento especifico no esp
	inicializaPCAs();
	vTaskDelay(pdMS_TO_TICKS(60));
	
	//Inicializa vertorzao de leitura de teclas
	for (cntTmp = 0; cntTmp < 5; cntTmp++)
	{
		bufferLeituraPCA1_seminterrupcao[cntTmp] = bufferLeituraPCA1[cntTmp];
	}
	
	lePCA8575RegistroUnico(ENDERECO_PCA8575D_MM1300, &bufferLeituraPCA1_seminterrupcao[5]); //seis le automatico

	for (cntTmp = 7; cntTmp < (7+5+5+5); cntTmp++) 
	{
		bufferLeituraPCA1_seminterrupcao[cntTmp] = bufferLeituraPCA1[cntTmp];
	}

	//Inicializa o StatusofKeyboardLeds
	inicializaStatusOfKeyBoardLeds();		

	/*
	//ESP rotina de testes
	leRegistro(0x22, (input_port_register_bank[0] | 0x80),  &bufferLeituraPCA1[0]);
	//Entra na condicional caso pressione e mantenha pressionado 3 teclas, equivalentes ao uchar 143
	if (bufferLeituraPCA1[2] == TesteCode) {
		vTaskDelay(pdMS_TO_TICKS(100));
		if (bufferLeituraPCA1[2] == TesteCode){
			vTaskDelay(pdMS_TO_TICKS(100));
			if (bufferLeituraPCA1[2] == TesteCode){
			printf("MODO DE TESTE ATIVADO");
			RunTestMode();
			}
		}
	}
	*/
	
	//Create a task de varredura das teclas com afinidade no CPU 0
	xTaskCreatePinnedToCore(readkey_task, "Task de Varredura", 8192, NULL, configMAX_PRIORITIES - 3, NULL, 0);

	// Create UDP server task on CPU1
    xTaskCreatePinnedToCore(udp_server_task, "UDP Server Task", 8192, NULL, configMAX_PRIORITIES - 1, NULL, 1);
}

//ESP Rotina responsável por inicializar o Status of keyboard leds que vai ser utilizado no ThreadReadKey
void inicializaStatusOfKeyBoardLeds(void)
{
	unsigned char PortIndex;
	//inicializa StatusOfKeyBoardLeds com todas as teclas apagadas
	for (PortIndex = 0; PortIndex < (7+5+5+5); PortIndex++)
	{
		StatusOfKeyBoardLeds[PortIndex][0] = 0xFF;
		StatusOfKeyBoardLeds[PortIndex][1] = 0xFF;
		AuxVarToBlinkBanks[PortIndex] = 0xFF;
	}
	StatusOfEncoderBoardLeds[0] = 0xFF;
	StatusOfEncoderBoardLeds[1] = 0xFF;

	RunKeyLedsOneTime();
}
//Task de varredura
void readkey_task(void *pvParameters) {
	printf("Task de Varredura started on CPU %d\n", xPortGetCoreID());
	while (1) {
		ThreadReadKey_SemInt();

		vTaskDelay(pdMS_TO_TICKS(30));
	}
}

// Inicia o servidor udp na porta 500
void udp_server_task(void *pvParameters) {
	printf("UDP Server Task started on CPU %d\n", xPortGetCoreID());
	start_udp_server(); 
	while (1){
		vTaskDelay(pdMS_TO_TICKS(500));
	}   
}