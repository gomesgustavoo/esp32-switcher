/*****************************************************************************
(C) Copyright 2021 - 4S Informatica Ind. e Com. LTDA. All rights reserved.

File Name: declaracoes.h

Data: 30/08/2021			Rev 1.0

Autor: Eduardo Artur Cunha 

Software: PSoC Designer 5.4 SP1 - Build 3191 - 04-March-2015.21:56:41

Compilador: Imagecraft Compiler Standard V7.0.5

Hardware: PSOC microcontroler - CY8C24894-24LT

Descricao:	Esqueletos das funções do projeto Teclado Mago 56 teclas com expansão
			

*****************************************************************************/

#include <stdint.h>
extern unsigned char AuxVarToShowVersionOfHardwareBoard;

//KeysManager.c
void ThreadProcessaComandosUSB(void);
void RunKeyLedsOneTime(void);
void RunKeysJustFirstLine(void);
void ManageKeyLeds(unsigned char comando, unsigned char KeyIndex);
unsigned char Key33And34And35PressedToEnterInTestMode(void);
void RunTestMode(void);
void ThreadReadKey_SemInt_Individualmente (unsigned char i2CAddress);
void ThreadReadKey_SemInt (void);
void GerenciaEncoders (void);
void inicializaArrayIndicaTecla(void);
void ApagaTodasAsTeclas(void);
void AcendeTodasAsTeclas(void);
void AcendeAsTeclasPares(void);
void AcendeAsTeclasImpares(void);
void AplicaValorFixoEmTodosOsPCAS(unsigned char ValorFixo);


/*
//main.c
void lowVoltageDetected(void);
void inicializaPlacaTecladoUSB(void);
*/
void app_main(void);
void inicializaStatusOfKeyBoardLeds(void);
void udp_server_task(void *pvParameters);
void readkey_task(void *pvParameters);
void precise_delay_us(int64_t dalay);

/*
//usbcomm.c
void inicializaUSB(void);
void ThreadUSB(void);
*/
/*
//timermanager.c
void inicializaTimer(void);
void timerManager(void);
*/
/*
//fila de recepcao
void RCQ_InitializeQueue(void);
void RCQ_InsertNewCommand(unsigned char command, 
						  unsigned char data1, 
						  unsigned char data2);
void RCQ_GetOldestCommand(unsigned char * rCommand, 
						  unsigned char* rData1, 
						  unsigned char* rData2);
unsigned char RCQ_hasCommands(void);
*/

/*
//fila de envio
void SCQ_InitializeQueue(void);
unsigned char SCQ_hasCommandsToSend(void);
void SCQ_GetOldestCommand(unsigned char * command, unsigned char * data1 , unsigned char * data2);
void SCQ_InsertNewCommand(	unsigned char command, 
							unsigned char data1, 
							unsigned char data2);
*/
							
							

//Protótipos I2C
void inicializaPCAs(void);
void leRegistro(unsigned char endereco, unsigned char reg, unsigned char * buf);
void leRegistroUnico(unsigned char endereco, unsigned char reg, unsigned char * buf);
void lePCA8575RegistroUnico(unsigned char endereco, unsigned char * buf);
unsigned int lePCA8575(unsigned char endereco);
void escreveRegistro(unsigned char endereco, unsigned char reg, unsigned char valor);
void escreve5RegistrosBurst(unsigned char endereco, unsigned char reg, 	unsigned char valor0,
																unsigned char valor1,
																	unsigned char valor2,
																	unsigned char valor3,
																	unsigned char valor4);
																		
unsigned char CheckPcaDevice(unsigned char endereco);
void escrevePCA8575(unsigned char endereco, unsigned char valor);
void escreve_2bytes_PCA8575(unsigned char endereco, unsigned char valor);
//ESP inicializa i2c
void i2c_master_init();
