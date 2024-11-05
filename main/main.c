/*****************************************************************************
(C) Copyright 2021 - 4S Informatica Ind. e Com. LTDA. All rights reserved.

File Name: main.c

Projeto:	Teclado Mago 56 teclas com expansão

Data: 30/08/2021			Rev 1.0

Autor: Eduardo Artur Cunha 

Software: PSoC Designer 5.4 SP1 - Build 3191 - 04-March-2015.21:56:41

Compilador: Imagecraft Compiler Standard V7.0.5

Hardware: PSOC microcontroler - CY8C24894-24LT

Descricao:	Código-fonte do projeto de GPI e conversor serial USB.

*****************************************************************************/

//#include <m8c.h>        // part specific constants and macros
//#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "declaracoes.h"
#include "definicoes.h"
#include "registrosPCA9506.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

//#pragma interrupt_handler lowVoltageDetected

unsigned char TesteCode = 143;
unsigned char timerTick;
unsigned int brutalWatchdog;
extern unsigned char AuxVarToBlinkBanks[7+5+5+5];
unsigned char AuxVarToShowIfEncoderAandBAreConnected;
unsigned char AuxVarToShowVersionOfHardwareBoard;


//unsigned char g_has_received_some_led_usb_command;

extern unsigned char bufferLeituraPCA1[7+5+5+5];
extern unsigned char bufferLeituraPCA1_seminterrupcao[7+5+5+5];
extern unsigned char ArrayIndicaTecla[7+5+5+5][8];

extern unsigned char StatusOfKeyBoardLeds[7+5+5+5][2];
extern unsigned char StatusOfEncoderBoardLeds[2];
//extern unsigned char PRT2DR_Old, PRT2DR_Sampled;

// TODO testar no raspberry ubuntu


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
	
	if (CheckPcaDevice(ENDERECO_PCA_1))
	{
		AuxVarToShowVersionOfHardwareBoard |= HARDWARE_VERSION_40TECLAS; //0x00
	}

	if (CheckPcaDevice(ENDERECO_PCA_2_MM1300))
	{
		AuxVarToShowVersionOfHardwareBoard |= HARDWARE_VERSION_56TECLASSEMEXPANSAO;
	}
	
	if (CheckPcaDevice(ENDERECO_PCA_3_MM1200_A))
	{
		AuxVarToShowVersionOfHardwareBoard |= HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED;
	}

	if (CheckPcaDevice(ENDERECO_PCA_3_MM1200_B))
	{
		AuxVarToShowVersionOfHardwareBoard |= HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED;
	}
	
	if (CheckPcaDevice(ENDERECO_PCA_3_MM1200_C))
	{
		AuxVarToShowVersionOfHardwareBoard |= HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED;	
	}

	/***********************************************************************************
	inicializaPCAs
	************************************************************************************/
	//ESP Rotina de inicialização dos PCAs alterada para o funcionamento especifico no esp
	inicializaPCAs();
	
	//Inicializa vertorzao de leitura de teclas
	for (cntTmp = 0; cntTmp < 5; cntTmp++)
	{
		bufferLeituraPCA1_seminterrupcao[cntTmp] = bufferLeituraPCA1[cntTmp];
	}
	
	//for (cntTmp = 5; cntTmp < 7; cntTmp++) 
	//{
	
		lePCA8575RegistroUnico(ENDERECO_PCA8575D_MM1300, &bufferLeituraPCA1_seminterrupcao[5]); //seis le automatico
	//}

	for (cntTmp = 7; cntTmp < (7+5+5+5); cntTmp++) 
	{
		bufferLeituraPCA1_seminterrupcao[cntTmp] = bufferLeituraPCA1[cntTmp];
	}		
	
	/***********************************************************************************
	 Thread responsável por processar rotina de testes
	************************************************************************************/
	/*
	if (Key33And34And35PressedToEnterInTestMode())
	{
		if (Key33And34And35PressedToEnterInTestMode())
		{	
			if (Key33And34And35PressedToEnterInTestMode())
			{
				if (Key33And34And35PressedToEnterInTestMode())
				{
					RunTestMode();
				
				}
			}
		}
	}
	*/
	//ESP rotina de testes
	leRegistro(0x22, (input_port_register_bank[0] | 0x80),  &bufferLeituraPCA1[0]);
	//Entra na condicional caso pressione e mantenha pressionado 3 teclas, equivalentes ao uchar 143
	if (bufferLeituraPCA1[2] == TesteCode) {
		if (bufferLeituraPCA1[2] == TesteCode){
			if (bufferLeituraPCA1[2] == TesteCode){
			printf("MODO DE TESTE ATIVADO");
			RunTestMode();
			}
		}
	}


	//Loop infinito da aplicação
	while (1)
	{	
		printf("chegou no loop da aplicação\n");
		
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

/****************************************************************************************
 Rotina responsável por tratar a interrupção por queda de tensão
****************************************************************************************/
/*
void lowVoltageDetected(void )
{
	RunKeysJustFirstLine();
	RunKeysJustFirstLine();
	inicializaPCAs();
}
*/

/***************************************************************************************
 Todas inicializações de interrupções e periféricos são feitas nesta rotina
****************************************************************************************/
/*
void inicializaPlacaTecladoUSB(void)
{
	unsigned char PortIndex;

	INT_VC = 0x00; //Limpa vetor de interrupções
	M8C_EnableGInt ; //Inicializa interrupções globais
	//M8C_DisableGInt;	
	M8C_EnableIntMask(INT_MSK0, INT_MSK0_VOLTAGE_MONITOR); //Habilita interrupção de supervisor de tensão
	//M8C_DisableIntMask(INT_MSK0, INT_MSK0_VOLTAGE_MONITOR); //Habilita interrupção de supervisor de tensão
	M8C_DisableIntMask(INT_MSK0, INT_MSK0_GPIO);   // C code to enable GPIO interrupt
	
	RCQ_InitializeQueue(); //Inicializa fila de recepcao de comandos
	SCQ_InitializeQueue(); //Inicializa fila de envio
	
	//inicializaUSB();
	inicializaTimer();
	
	PRT3DR |= 0x02;
	
	//M8C_EnableIntMask(INT_MSK0, INT_MSK0_GPIO);   // C code to enable GPIO interrupt
	
	//inicializa StatusOfKeyBoardLeds com todas as teclas apagadas
	for (PortIndex = 0; PortIndex < (7+5+5+5); PortIndex++)
	{
		StatusOfKeyBoardLeds[PortIndex][0] = 0xFF;
		StatusOfKeyBoardLeds[PortIndex][1] = 0xFF;
		AuxVarToBlinkBanks[PortIndex] = 0xFF;
	}
	
	g_has_received_some_led_usb_command = 0x00;
	
	StatusOfEncoderBoardLeds[0] = 0xFF;
	StatusOfEncoderBoardLeds[1] = 0xFF;
	
	PRT2DR_Old = 0xFF;
	PRT2DR_Sampled = 0xFF;
	
	
	timerTick = VALOR_TIMEOUT_READKEY_OUTINT;
*/
