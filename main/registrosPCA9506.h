/*****************************************************************************************
	(C) Copyright 2011 - 4S Informatica Ind. e Com. LTDA. All rights reserved.
	
	Projeto: Teclado Mago 56 teclas com expansão
	
	Arquivo: registrosPCA9506.h
	
	Autor(es):  Eduardo Artur Cunha
	
	Data: 05/10/2011
	
	Revisão: 1.0
	
	IDE: PSoC Designer 5.4 SP1 - Build 3191 - 04-March-2015.21:56:41
	
	Compilador: Imagecraft Compiler Standard V7.0.5
	
	Descrição: 	Contem os endereços dos registros do PCA-9506DGG para uso nas rotinas.
				
	Controle de versão:
	1.0 - Versão inicial
********************************************************************************************/

#ifndef REGISTROSPCA9506_H_
#define REGISTROSPCA9506_H_

/*********************************************
Definições de registro de entrada. Utilizada
somente para leitura, pois o PORT estará 
configurado para entrada.
**********************************************/
#define	INPUT_PORT_REGISTER_BANK_0	0x00
#define	INPUT_PORT_REGISTER_BANK_1	0x01
#define	INPUT_PORT_REGISTER_BANK_2	0x02
#define	INPUT_PORT_REGISTER_BANK_3	0x03
#define	INPUT_PORT_REGISTER_BANK_4	0x04

extern unsigned char input_port_register_bank[5];

/*********************************************
Definições de registro de saida. Utilizada
para leitura e escrita.
**********************************************/

#define	OUTPUT_PORT_REGISTER_BANK_0	0x08
#define	OUTPUT_PORT_REGISTER_BANK_1	0x09
#define	OUTPUT_PORT_REGISTER_BANK_2	0x0A
#define	OUTPUT_PORT_REGISTER_BANK_3	0x0B
#define	OUTPUT_PORT_REGISTER_BANK_4	0x0C

extern unsigned char output_port_register_banks[5];

/*********************************************
Definições de registro de configuração de
PORT para definir se será entrada ou saida.
**********************************************/

#define	IO_CONFIGURATION_REGISTER_BANK_0	0x18
#define	IO_CONFIGURATION_REGISTER_BANK_1	0x19
#define	IO_CONFIGURATION_REGISTER_BANK_2	0x1A
#define	IO_CONFIGURATION_REGISTER_BANK_3	0x1B
#define	IO_CONFIGURATION_REGISTER_BANK_4	0x1C

extern unsigned char io_configuration_register_banks[5];

/*********************************************
Definições de registro para mascara de 
interrupção.
**********************************************/

#define MASK_INTERRUPT_REGISTER_BANK_0		0x20
#define MASK_INTERRUPT_REGISTER_BANK_1		0x21
#define MASK_INTERRUPT_REGISTER_BANK_2		0x22
#define MASK_INTERRUPT_REGISTER_BANK_3		0x23
#define MASK_INTERRUPT_REGISTER_BANK_4		0x24

extern unsigned char mask_interrupt_register_banks[5];

/**********************************************
 Registros de inversao de polaridade
***********************************************/
#define POLARITY_INVERSION_REGISTER_BANK_0	0x10
#define POLARITY_INVERSION_REGISTER_BANK_1	0x11
#define POLARITY_INVERSION_REGISTER_BANK_2	0x12
#define POLARITY_INVERSION_REGISTER_BANK_3	0x13
#define POLARITY_INVERSION_REGISTER_BANK_4	0x14

extern unsigned char polarity_inversion_register_banks[5];

#endif /*REGISTROSPCA9506_H_*/

