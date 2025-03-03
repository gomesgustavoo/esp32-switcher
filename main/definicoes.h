/*****************************************************************************
(C) Copyright 2021 - 4S Informatica Ind. e Com. LTDA. All rights reserved.

File Name: definicoes.h

Data: 30/08/2021			Rev 1.0

Autor: Eduardo Artur Cunha 

Software: PSoC Designer 5.4 SP1 - Build 3191 - 04-March-2015.21:56:41

Compilador: Imagecraft Compiler Standard V7.0.5

Hardware: PSOC microcontroler - CY8C24894-24LT

Descricao:	Definições gerais do projeto.
			

*****************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t varredura_handle;

#define NOTIFY_PAUSE  (1UL << 0)
#define NOTIFY_RESUME (1UL << 1)

#define FIRMWARE_VERSION	42
#define HARDWARE_VERSION	0
#define HARDWARE_VERSION_40TECLAS	0 //versão inicial para teclado 40 teclas de 20
#define HARDWARE_VERSION_56TECLASSEMEXPANSAO	0x10 //versão hardware 2021 com reorganização de teclas
#define HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED	0x11 //versão hardware 2021 com reorganização de teclas e placa adicional
#define HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED	0x12 //versão hardware 2021 com reorganização de teclas e placa adicional
#define HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED	0x14 //versão hardware 2021 com reorganização de teclas e placa adicional


/********************************************
 Definicao de instruções asm em C
*********************************************/
#define NOP					asm("NOP")
#define PUSH_A				asm("push a")
#define PUSH_X				asm("push x")
#define POP_A				asm("pop a")
#define POP_X				asm("pop x")
#define PUSH				PUSH_A;PUSH_X
#define POP					POP_X; POP_A

/********************************************
 Definicao de máscaras em função de bit
*********************************************/
#define BIT_0				0x01
#define BIT_1				0x02
#define BIT_2				0x04
#define BIT_3				0x08
#define BIT_4				0x10
#define BIT_5				0x20
#define BIT_6				0x40
#define BIT_7				0x80

/********************************************
 Definições da comunicação pela rede
*********************************************/
#define COMANDO_KEYLED_ON 0x73
#define COMANDO_KEYLED_OFF 0X74
#define COMANDO_RESET 0x75

#define TECLA_1					0x41
#define TECLA_2					0x42
#define TECLA_3					0x43
#define TECLA_4					0x44
#define TECLA_5					0x45
#define TECLA_6					0x46
#define TECLA_7					0x47
#define TECLA_8					0x48
#define TECLA_9					0x49
#define TECLA_10				0x4a
#define TECLA_11				0x4b
#define TECLA_12				0x4c
#define TECLA_13				0x4d
#define TECLA_14				0x4e
#define TECLA_15				0x4f
#define TECLA_16				0x50
#define TECLA_17				0x51
#define TECLA_18				0x52
#define TECLA_19				0x53
#define TECLA_20				0x54
#define TECLA_21				0x55
#define TECLA_22				0x56
#define TECLA_23				0x57
#define TECLA_24				0x58
#define TECLA_25				0x59
#define TECLA_26				0x5a
#define TECLA_27				0x5b
#define TECLA_28				0x5c
#define TECLA_29				0x5d
#define TECLA_30				0x5e
#define TECLA_31				0x5f
#define TECLA_32				0x60
#define TECLA_33				0x61
#define TECLA_34				0x62
#define TECLA_35				0x63
#define TECLA_36				0x64
#define TECLA_37				0x65
#define TECLA_38				0x66
#define TECLA_39				0x67
#define TECLA_40				0x68

// Para teclado MM-1300 de 56 teclas:
#define TECLA_41				0x69
#define TECLA_42				0x6A
#define TECLA_43				0x6B
#define TECLA_44				0x6C
#define TECLA_45				0x6D
#define TECLA_46				0x6E
#define TECLA_47				0x6F
#define TECLA_48				0xA0
#define TECLA_49				0xA1
#define TECLA_50				0xA2
#define TECLA_51				0xA3
#define TECLA_52				0xA4
#define TECLA_53				0xA5
#define TECLA_54				0xA6
#define TECLA_55				0xA7
#define TECLA_56				0xA8

// Para teclado MM-1300 + 1 expansão MM-1200, total de 96 teclas:
#define TECLA_57				0xA9
#define TECLA_58				0xAA
#define TECLA_59				0xAB
#define TECLA_60				0xAC
#define TECLA_61				0xAD
#define TECLA_62				0xAE
#define TECLA_63				0xAF
#define TECLA_64				0xB0
#define TECLA_65				0xB1
#define TECLA_66				0xB2
#define TECLA_67				0xB3
#define TECLA_68				0xB4
#define TECLA_69				0xB5
#define TECLA_70				0xB6
#define TECLA_71				0xB7
#define TECLA_72				0xB8
#define TECLA_73				0xB9
#define TECLA_74				0xBA
#define TECLA_75				0xBB
#define TECLA_76				0xBC
#define TECLA_77				0xBD
#define TECLA_78				0xBE
#define TECLA_79				0xBF
#define TECLA_80				0xC0
#define TECLA_81				0xC1
#define TECLA_82				0xC2
#define TECLA_83				0xC3
#define TECLA_84				0xC4
#define TECLA_85				0xC5
#define TECLA_86				0xC6
#define TECLA_87				0xC7
#define TECLA_88				0xC8
#define TECLA_89				0xC9
#define TECLA_90				0xCA
#define TECLA_91				0xCB
#define TECLA_92				0xCC
#define TECLA_93				0xCD
#define TECLA_94				0xCE
#define TECLA_95				0xCF
#define TECLA_96				0xD0

// Para teclado MM-1300 + 2 expansões MM-1200, total de 136 teclas:
#define TECLA_97				0xD1
#define TECLA_98				0xD2
#define TECLA_99				0xD3
#define TECLA_100				0xD4
#define TECLA_101				0xD5
#define TECLA_102				0xD6
#define TECLA_103				0xD7
#define TECLA_104				0xD8
#define TECLA_105				0xD9
#define TECLA_106				0xDA
#define TECLA_107				0xDB
#define TECLA_108				0xDC
#define TECLA_109				0xDD
#define TECLA_110				0xDE
#define TECLA_111				0xDF
#define TECLA_112				0xE0
#define TECLA_113				0xE1
#define TECLA_114				0xE2
#define TECLA_115				0xE3
#define TECLA_116				0xE4
#define TECLA_117				0xE5
#define TECLA_118				0xE6
#define TECLA_119				0xE7
#define TECLA_120				0xE8
#define TECLA_121				0xE9
#define TECLA_122				0xEA
#define TECLA_123				0xEB
#define TECLA_124				0xEC
#define TECLA_125				0xED
#define TECLA_126				0xEE
#define TECLA_127				0xEF
#define TECLA_128				0xF0
#define TECLA_129				0xF1
#define TECLA_130				0xF2
#define TECLA_131				0xF3
#define TECLA_132				0xF4
#define TECLA_133				0xF5
#define TECLA_134				0xF6
#define TECLA_135				0xF7
#define TECLA_136				0xF8

// Para teclado MM-1300 + 3 expansões MM-1200, total de 176 teclas:
#define TECLA_137				0xFB
#define TECLA_138				0xFC
#define TECLA_139				0x10
#define TECLA_140				0x11
#define TECLA_141				0x12
#define TECLA_142				0x13
#define TECLA_143				0x14
#define TECLA_144				0x15
#define TECLA_145				0x16
#define TECLA_146				0x17
#define TECLA_147				0x18
#define TECLA_148				0x19
#define TECLA_149				0x1A
#define TECLA_150				0x1B
#define TECLA_151				0x1C
#define TECLA_152				0x1D
#define TECLA_153				0x1E
#define TECLA_154				0x1F
#define TECLA_155				0x20
#define TECLA_156				0x21
#define TECLA_157				0x22
#define TECLA_158				0x23
#define TECLA_159				0x24
#define TECLA_160				0x25
#define TECLA_161				0x26
#define TECLA_162				0x27
#define TECLA_163				0x28
#define TECLA_164				0x29
#define TECLA_165				0x2A
#define TECLA_166				0x2B
#define TECLA_167				0x2C
#define TECLA_168				0x2E
#define TECLA_169				0x2F
#define TECLA_170				0x30
#define TECLA_171				0x31
#define TECLA_172				0x32
#define TECLA_173				0x33
#define TECLA_174				0x34
#define TECLA_175				0x35
#define TECLA_176				0x36

#define ALL_LEDS				0x7E

#define LED_ENCODER_A_STP1		0x80
#define LED_ENCODER_A_STP2		0x81
#define LED_ENCODER_A_STP3		0x82
#define LED_ENCODER_A_STP4		0x83
#define LED_ENCODER_A_STP5		0x84
#define LED_ENCODER_A_STP6		0x85
#define LED_ENCODER_A_STP7		0x86
#define LED_ENCODER_A_SEL		0x87
#define LED_ENCODER_B_STP1		0x88
#define LED_ENCODER_B_STP2		0x89
#define LED_ENCODER_B_STP3		0x8A
#define LED_ENCODER_B_STP4		0x8B
#define LED_ENCODER_B_STP5		0x8C
#define LED_ENCODER_B_STP6		0x8D
#define LED_ENCODER_B_STP7		0x8E
#define LED_ENCODER_B_SEL		0x8F

#define ENCODER_A_CLICK					0x90
#define ENCODER_A_TURN					0x91

#define ENCODER_B_CLICK					0x92
#define ENCODER_B_TURN					0x93

#define TURN_CLOCKWISE					0x94
#define TURN_ANTICLOCKWISE				0x95


#define PRESSED					0x70
#define RELEASED				0x72


/***************************************
 Defines I2C
***************************************/
#define ENDERECO_PCA_1	0x24 //PCa A2=1, A1=0, A0=0
#define ENDERECO_PCA_2_MM1300	0x22 //PCa A2=0, A1=1, A0=0 
#define ENDERECO_PCA_3_MM1200_A	0x20 //PCa A2=0, A1=0, A0=0 // JP1 Low, JP2 Low, JP3 Low; 
#define ENDERECO_PCA_3_MM1200_B	0x23 //PCa A2=0, A1=1, A0=1 // JP1 High, JP2 High, JP3 Low;
#define ENDERECO_PCA_3_MM1200_C	0x25 //PCa A2=1, A1=0, A0=1 // JP1 High, JP2 Low, JP3 High;

#define ENDERECO_PCA8575D_MM1300 0x21

#define ENDERECO_PCA_ENCODER_A	0x38 //PCa
#define ENDERECO_PCA_ENCODER_B	0x39 //PCa

#define PCA_ENCODER_A_CONNECTED 0x80
#define PCA_ENCODER_B_CONNECTED 0x40


#define TEMPO_TIMEOUT_I2C	10

//Defines I2C para Esp32
#define I2C_MASTER_SCL_IO 15        // Pino SCL do ESP32
#define I2C_MASTER_SDA_IO 14        // Pino SDA do ESP32
#define I2C_MASTER_FREQ_HZ 100000   // Frequência do I2C
#define I2C_MASTER_NUM I2C_NUM_0   
