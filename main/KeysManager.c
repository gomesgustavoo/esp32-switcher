
/*****************************************************************************
(C) Copyright 2021 - 4S Informatica Ind. e Com. LTDA. All rights reserved.

File Name: KeysManager.c

Data: 30/08/2021			Rev 1.0

Autor: Eduardo Artur Cunha

Software: PSoC Designer 5.4 SP1 - Build 3191 - 04-March-2015.21:56:41

Compilador: Imagecraft Compiler Standard V7.0.5

Hardware: PSOC microcontroler - CY8C24894-24LT

Descricao:	Arquivo contendo as rotinas para gerenciamento dos GPIs e GPOs
			

*****************************************************************************/

//#include <m8c.h>        // part specific constants and macros
//#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
#include "declaracoes.h"
#include "definicoes.h"
#include "registrosPCA9506.h"
#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

unsigned char comando, dado1, dado2;
unsigned char AuxVarToBlinkBanks[7+5+5+5];
unsigned char StatusOfKeyBoardLeds[(7+5+5+5)][2];
extern unsigned char g_has_received_some_led_usb_command;
extern unsigned char timerTick;
extern volatile unsigned int brutalWatchdog;
extern unsigned char bufferLeituraPCA1_imediatamenteAposPolling[7+5+5+5];
unsigned char bufferLeituraPCA1_seminterrupcao[7+5+5+5];
unsigned char ArrayIndicaTecla[7+5+5+5][8];
extern unsigned char AuxVarToShowIfEncoderAandBAreConnected;
unsigned char StatusOfEncoderBoardLeds[2];

unsigned char EncoderA_seqPin1, EncoderA_seqPin2;
unsigned char EncoderB_seqPin1, EncoderB_seqPin2;

unsigned char PRT2DR_Old, PRT2DR_Sampled;

/*ESP
void ThreadProcessaComandosUSB(void)
{
	//Checar fila de comandos recebidos
	if (RCQ_hasCommands() != 0)
	{
		RCQ_GetOldestCommand(&comando, &dado1, &dado2);

		if (comando == CMD_RESET)
		{
			inicializaPCAs();
			M8C_Reset;
		}
		else if (comando == CMD_KEYLED_ON_RUN_MODE)
		{
			RunKeyLedsOneTime();
		}
		else if (comando == CMD_CHECKRESET)
		{
			if (g_has_received_some_led_usb_command == 0x01)
			{
				SCQ_InsertNewCommand(CMD_CHECKRESET, 0x01, 0x01);
			}
			else 
			{
				SCQ_InsertNewCommand(CMD_CHECKRESET, 0x00, 0x00);
			}

		}
		else if (comando == CMD_KEYLED_ON)
		{
			g_has_received_some_led_usb_command = 0x01;
			ManageKeyLeds(CMD_KEYLED_ON, dado1);
		}
		else if (comando == CMD_KEYLED_OFF)
		{
			g_has_received_some_led_usb_command = 0x01;
			ManageKeyLeds(CMD_KEYLED_OFF, dado1);
		}
		else if (comando == CMD_KEYLED_BLINK)
		{
			g_has_received_some_led_usb_command = 0x01;
			ManageKeyLeds(CMD_KEYLED_BLINK, dado1);
		}
		
		else if (comando == CMD_READ_FW)
		{
			//Leitura de versao de firmware	
			SCQ_InsertNewCommand(CMD_READ_FW, FIRMWARE_VERSION, (AuxVarToShowVersionOfHardwareBoard | AuxVarToShowIfEncoderAandBAreConnected));
		}

	}

}
*/
void inicializaArrayIndicaTecla(void)
{
	unsigned char PosPort = 0;
	unsigned char PosBit = 0;

	unsigned char ArrayIndicaTeclaInicializadoLocal[7+5+5+5][8] = {
	{TECLA_17, TECLA_18, TECLA_19, TECLA_20, TECLA_21, TECLA_22, TECLA_23, TECLA_24},
	{TECLA_25, TECLA_26, TECLA_27, TECLA_28, TECLA_29, TECLA_30, TECLA_31, TECLA_32},
	{TECLA_49, TECLA_50, TECLA_51, TECLA_52, TECLA_53, TECLA_54, TECLA_55, TECLA_56},
	{TECLA_33, TECLA_34, TECLA_35, TECLA_36, TECLA_37, TECLA_38, TECLA_39, TECLA_40},
	{TECLA_41, TECLA_42, TECLA_43, TECLA_44, TECLA_45, TECLA_46, TECLA_47, TECLA_48},

	{TECLA_1, TECLA_2, TECLA_3, TECLA_4, TECLA_5, TECLA_6, TECLA_7, TECLA_8},
	{TECLA_9, TECLA_10, TECLA_11, TECLA_12, TECLA_13, TECLA_14, TECLA_15, TECLA_16},

	{TECLA_57, TECLA_58, TECLA_59, TECLA_60, TECLA_61, TECLA_62, TECLA_63, TECLA_64},
	{TECLA_65, TECLA_66, TECLA_67, TECLA_68, TECLA_69, TECLA_70, TECLA_71, TECLA_72},
	{TECLA_89, TECLA_90, TECLA_91, TECLA_92, TECLA_93, TECLA_94, TECLA_95, TECLA_96},
	{TECLA_73, TECLA_74, TECLA_75, TECLA_76, TECLA_77, TECLA_78, TECLA_79, TECLA_80},
	{TECLA_81, TECLA_82, TECLA_83, TECLA_84, TECLA_85, TECLA_86, TECLA_87, TECLA_88},
	
	{TECLA_97, TECLA_98, TECLA_99, TECLA_100, TECLA_101, TECLA_102, TECLA_103, TECLA_104},
	{TECLA_105, TECLA_106, TECLA_107, TECLA_108, TECLA_109, TECLA_110, TECLA_111, TECLA_112},
	{TECLA_129, TECLA_130, TECLA_131, TECLA_132, TECLA_133, TECLA_134, TECLA_135, TECLA_136},
	{TECLA_113, TECLA_114, TECLA_115, TECLA_116, TECLA_117, TECLA_118, TECLA_119, TECLA_120},
	{TECLA_121, TECLA_122, TECLA_123, TECLA_124, TECLA_125, TECLA_126, TECLA_127, TECLA_128},

	{TECLA_137, TECLA_138, TECLA_139, TECLA_140, TECLA_141, TECLA_142, TECLA_143, TECLA_144},
	{TECLA_145, TECLA_146, TECLA_147, TECLA_148, TECLA_149, TECLA_150, TECLA_151, TECLA_152},
	{TECLA_169, TECLA_170, TECLA_171, TECLA_172, TECLA_173, TECLA_174, TECLA_175, TECLA_176},
	{TECLA_153, TECLA_154, TECLA_155, TECLA_156, TECLA_157, TECLA_158, TECLA_159, TECLA_160},
	{TECLA_161, TECLA_162, TECLA_163, TECLA_164, TECLA_165, TECLA_166, TECLA_167, TECLA_168}
	};

	
	for (PosPort = 0; PosPort < (7+5+5+5); PosPort++)
	{
		for (PosBit = 0; PosBit < 8; PosBit++)
		{
			ArrayIndicaTecla[PosPort][PosBit] = ArrayIndicaTeclaInicializadoLocal[PosPort][PosBit];
		}
	}

	
}

unsigned char GetPortIndex_OfKey(unsigned char KeyIndex)
{
	unsigned char PosPort = 0;
	unsigned char PosBit = 0;
	
	unsigned char ArrayIndicaPort[(7+5+5+5)][8] = {
	{0, 0, 0, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1},
	{2, 2, 2, 2, 2, 2, 2, 2},
	{3, 3, 3, 3, 3, 3, 3, 3},
	{4, 4, 4, 4, 4, 4, 4, 4},
	
	{5, 5, 5, 5, 5, 5, 5, 5},
	{6, 6, 6, 6, 6, 6, 6, 6},
	
	{7, 7, 7, 7, 7, 7, 7, 7},
	{8, 8, 8, 8, 8, 8, 8, 8},
	{9, 9, 9, 9, 9, 9, 9, 9},
	{10, 10, 10, 10, 10, 10, 10, 10},
	{11, 11, 11, 11, 11, 11, 11, 11},
	
	{12, 12, 12, 12, 12, 12, 12, 12},
	{13, 13, 13, 13, 13, 13, 13, 13},
	{14, 14, 14, 14, 14, 14, 14, 14},
	{15, 15, 15, 15, 15, 15, 15, 15},
	{16, 16, 16, 16, 16, 16, 16, 16},	
	
	{17, 17, 17, 17, 17, 17, 17, 17},
	{18, 18, 18, 18, 18, 18, 18, 18},
	{19, 19, 19, 19, 19, 19, 19, 19},
	{20, 20, 20, 20, 20, 20, 20, 20},
	{21, 21, 21, 21, 21, 21, 21, 21}
	
	/*{0, 0, 0, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1},
	{2, 2, 2, 2, 2, 2, 2, 2},
	{3, 3, 3, 3, 3, 3, 3, 3},
	{4, 4, 4, 4, 4, 4, 4, 4},

	{0, 0, 0, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1},
	{2, 2, 2, 2, 2, 2, 2, 2},
	{3, 3, 3, 3, 3, 3, 3, 3},
	{4, 4, 4, 4, 4, 4, 4, 4},

	{0, 0, 0, 0, 0, 0, 0, 0},
	{1, 1, 1, 1, 1, 1, 1, 1},
	{2, 2, 2, 2, 2, 2, 2, 2},
	{3, 3, 3, 3, 3, 3, 3, 3},
	{4, 4, 4, 4, 4, 4, 4, 4}*/
	
	};
	
	for (PosPort = 0; PosPort < (7+5+5+5); PosPort++)
	{
		for (PosBit = 0; PosBit < 8; PosBit++)
		{
			if ((KeyIndex) == (ArrayIndicaTecla[PosPort][PosBit]))
				return ArrayIndicaPort[PosPort][PosBit];
		}
	}

	return 0;
}

unsigned char GetByteMask_OfKey(unsigned char KeyIndex)
{
	unsigned char PosPort = 0;
	unsigned char PosBit = 0;
	
	unsigned char ArrayIndicaBit[(7+5+5+5)][8] = {
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},

		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},

		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},

		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F},
		{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F}
		};
	
	for (PosPort = 0; PosPort < (7+5+5+5); PosPort++)
	{
		for (PosBit = 0; PosBit < 8; PosBit++)
		{
			if (KeyIndex == (ArrayIndicaTecla[PosPort][PosBit]))
				return ArrayIndicaBit[PosPort][PosBit];
		}
	}
	return 0;
}

unsigned char GetI2CAddress_OfKey(unsigned char KeyIndex)
{

	unsigned char PosPort = 0;
	unsigned char PosBit = 0;
	
	unsigned char ArrayIndicaI2CAddress[(7+5+5+5)][8] = {
		{ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300},
		{ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300},
		{ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300},
		{ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300},
		{ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300, ENDERECO_PCA_2_MM1300},

		{ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300},
		{ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300, ENDERECO_PCA8575D_MM1300},

		{ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A},
		{ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A},
		{ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A},
		{ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A},
		{ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A, ENDERECO_PCA_3_MM1200_A},

		{ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B},
		{ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B},
		{ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B},
		{ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B},
		{ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B, ENDERECO_PCA_3_MM1200_B},

		{ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C},
		{ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C},
		{ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C},
		{ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C},
		{ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C, ENDERECO_PCA_3_MM1200_C}
		};
	
	for (PosPort = 0; PosPort < (7+5+5+5); PosPort++)
	{
		for (PosBit = 0; PosBit < 8; PosBit++)
		{
			if (KeyIndex == (ArrayIndicaTecla[PosPort][PosBit]))
				return ArrayIndicaI2CAddress[PosPort][PosBit];
		}
	}
	return 0;
	
/*	switch (KeyIndex)
	{
		case TECLA_1:
		case TECLA_2:
		case TECLA_3:
		case TECLA_4:
		case TECLA_5:
		case TECLA_6:
		case TECLA_7:
		case TECLA_8:
		case TECLA_9:
		case TECLA_10:
		case TECLA_11:
		case TECLA_12:
		case TECLA_13:
		case TECLA_14:
		case TECLA_15:
		case TECLA_16:
			return ENDERECO_PCA8575D_MM1300;
			break;
		default:
			return ENDERECO_PCA_2_MM1300;
			break;
	}
*/
}


void ManageKeyLeds(unsigned char Cmd_On_or_Off_or_Blink, unsigned char KeyIndex)
{
	unsigned char PortIndex, PosPort;

	if (Cmd_On_or_Off_or_Blink == CMD_KEYLED_ON)
	{
		
		if (KeyIndex == ALL_LEDS)
		{
			for (PosPort = 0; PosPort < (7+5+5+5); PosPort++) // Preenche StatusOfKeyBoardLeds[x][0] = 0x00;
			{
				StatusOfKeyBoardLeds[PosPort][0] = 0x00;
			}
			AcendeTodasAsTeclas();

		}
		else {

			PortIndex = GetPortIndex_OfKey(KeyIndex);
			StatusOfKeyBoardLeds[PortIndex][0] = (StatusOfKeyBoardLeds[PortIndex][0]) & (GetByteMask_OfKey(KeyIndex));

			
			if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA8575D_MM1300)
			{
				escreveRegistro(ENDERECO_PCA8575D_MM1300, StatusOfKeyBoardLeds[5][0], StatusOfKeyBoardLeds[6][0]);
				
			}
			else if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA_2_MM1300){
				escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[PortIndex], (StatusOfKeyBoardLeds[PortIndex][0]));
			}
			else if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA_3_MM1200_A){
				escreveRegistro(ENDERECO_PCA_3_MM1200_A, io_configuration_register_banks[PortIndex-7], (StatusOfKeyBoardLeds[PortIndex][0]));
			}
			else if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA_3_MM1200_B){
				escreveRegistro(ENDERECO_PCA_3_MM1200_B, io_configuration_register_banks[PortIndex-12], (StatusOfKeyBoardLeds[PortIndex][0]));
			}
			else if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA_3_MM1200_C){
				escreveRegistro(ENDERECO_PCA_3_MM1200_C, io_configuration_register_banks[PortIndex-17], (StatusOfKeyBoardLeds[PortIndex][0]));
			}
			
		}
			
		/*ESP Ativar para uma futura mesa com encoder
		//gerencia leds do encoder
		if	((AuxVarToShowIfEncoderAandBAreConnected&PCA_ENCODER_A_CONNECTED) == PCA_ENCODER_A_CONNECTED)
		{
			if (KeyIndex == LED_ENCODER_A_STP1)
			{	StatusOfEncoderBoardLeds[0] &= ~0x01; }
			else if (KeyIndex == LED_ENCODER_A_STP2)
			{	StatusOfEncoderBoardLeds[0] &= ~0x02; }
			else if (KeyIndex == LED_ENCODER_A_STP3)
			{	StatusOfEncoderBoardLeds[0] &= ~0x04; }
			else if (KeyIndex == LED_ENCODER_A_STP4)
			{	StatusOfEncoderBoardLeds[0] &= ~0x08; }
			else if (KeyIndex == LED_ENCODER_A_STP5)
			{	StatusOfEncoderBoardLeds[0] &= ~0x10; }
			else if (KeyIndex == LED_ENCODER_A_STP6)
			{	StatusOfEncoderBoardLeds[0] &= ~0x20; }
			else if (KeyIndex == LED_ENCODER_A_STP7)
			{	StatusOfEncoderBoardLeds[0] &= ~0x40; }
			else if (KeyIndex == LED_ENCODER_A_SEL)
			{	StatusOfEncoderBoardLeds[0] &= ~0x80; }
			
			escrevePCA8575(ENDERECO_PCA_ENCODER_A, StatusOfEncoderBoardLeds[0]);
		}
		
		if	((AuxVarToShowIfEncoderAandBAreConnected&PCA_ENCODER_B_CONNECTED) == PCA_ENCODER_B_CONNECTED)
		{
			if (KeyIndex == LED_ENCODER_B_STP1)
			{	StatusOfEncoderBoardLeds[1] &= ~0x01; }
			else if (KeyIndex == LED_ENCODER_B_STP2)
			{	StatusOfEncoderBoardLeds[1] &= ~0x02; }
			else if (KeyIndex == LED_ENCODER_B_STP3)
			{	StatusOfEncoderBoardLeds[1] &= ~0x04; }
			else if (KeyIndex == LED_ENCODER_B_STP4)
			{	StatusOfEncoderBoardLeds[1] &= ~0x08; }
			else if (KeyIndex == LED_ENCODER_B_STP5)
			{	StatusOfEncoderBoardLeds[1] &= ~0x10; }
			else if (KeyIndex == LED_ENCODER_B_STP6)
			{	StatusOfEncoderBoardLeds[1] &= ~0x20; }
			else if (KeyIndex == LED_ENCODER_B_STP7)
			{	StatusOfEncoderBoardLeds[1] &= ~0x40; }
			else if (KeyIndex == LED_ENCODER_B_SEL)
			{	StatusOfEncoderBoardLeds[1] &= ~0x80; }
			
			escrevePCA8575(ENDERECO_PCA_ENCODER_B, StatusOfEncoderBoardLeds[1]);
		}
		*/


	}
	else if (Cmd_On_or_Off_or_Blink == CMD_KEYLED_OFF)
	{
		if (KeyIndex == ALL_LEDS)
		{
			for (PosPort = 0; PosPort < (7+5+5+5); PosPort++) // Preenche StatusOfKeyBoardLeds[x][0] = 0xFF;
			{
				StatusOfKeyBoardLeds[PosPort][0] = 0xFF;
			}
				ApagaTodasAsTeclas();
		}
		else {

			PortIndex = GetPortIndex_OfKey(KeyIndex);
			StatusOfKeyBoardLeds[PortIndex][0] = (StatusOfKeyBoardLeds[PortIndex][0]) | ~(GetByteMask_OfKey(KeyIndex));

			
			if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA8575D_MM1300)
			{
				escreveRegistro(ENDERECO_PCA8575D_MM1300, StatusOfKeyBoardLeds[5][0], StatusOfKeyBoardLeds[6][0]);
				
			}
			else if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA_2_MM1300){
				escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[PortIndex], (StatusOfKeyBoardLeds[PortIndex][0]));
			}
			else if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA_3_MM1200_A){
				escreveRegistro(ENDERECO_PCA_3_MM1200_A, io_configuration_register_banks[PortIndex-7], (StatusOfKeyBoardLeds[PortIndex][0]));
			}
			else if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA_3_MM1200_B){
				escreveRegistro(ENDERECO_PCA_3_MM1200_B, io_configuration_register_banks[PortIndex-12], (StatusOfKeyBoardLeds[PortIndex][0]));
			}
			else if ((GetI2CAddress_OfKey(KeyIndex)) == ENDERECO_PCA_3_MM1200_C){
				escreveRegistro(ENDERECO_PCA_3_MM1200_C, io_configuration_register_banks[PortIndex-17], (StatusOfKeyBoardLeds[PortIndex][0]));
			}			
			
		}
		
		/*
		//gerencia leds do encoder
		if	((AuxVarToShowIfEncoderAandBAreConnected&PCA_ENCODER_A_CONNECTED) == PCA_ENCODER_A_CONNECTED)
		{
			if (KeyIndex == LED_ENCODER_A_STP1)
			{	StatusOfEncoderBoardLeds[0] |= 0x01; }
			else if (KeyIndex == LED_ENCODER_A_STP2)
			{	StatusOfEncoderBoardLeds[0] |= 0x02; }
			else if (KeyIndex == LED_ENCODER_A_STP3)
			{	StatusOfEncoderBoardLeds[0] |= 0x04; }
			else if (KeyIndex == LED_ENCODER_A_STP4)
			{	StatusOfEncoderBoardLeds[0] |= 0x08; }
			else if (KeyIndex == LED_ENCODER_A_STP5)
			{	StatusOfEncoderBoardLeds[0] |= 0x10; }
			else if (KeyIndex == LED_ENCODER_A_STP6)
			{	StatusOfEncoderBoardLeds[0] |= 0x20; }
			else if (KeyIndex == LED_ENCODER_A_STP7)
			{	StatusOfEncoderBoardLeds[0] |= 0x40; }
			else if (KeyIndex == LED_ENCODER_A_SEL)
			{	StatusOfEncoderBoardLeds[0] |= 0x80; }
			
			escrevePCA8575(ENDERECO_PCA_ENCODER_A, StatusOfEncoderBoardLeds[0]);
		}
		
		if	((AuxVarToShowIfEncoderAandBAreConnected&PCA_ENCODER_B_CONNECTED) == PCA_ENCODER_B_CONNECTED)
		{
			if (KeyIndex == LED_ENCODER_B_STP1)
			{	StatusOfEncoderBoardLeds[1] |= 0x01; }
			else if (KeyIndex == LED_ENCODER_B_STP2)
			{	StatusOfEncoderBoardLeds[1] |= 0x02; }
			else if (KeyIndex == LED_ENCODER_B_STP3)
			{	StatusOfEncoderBoardLeds[1] |= 0x04; }
			else if (KeyIndex == LED_ENCODER_B_STP4)
			{	StatusOfEncoderBoardLeds[1] |= 0x08; }
			else if (KeyIndex == LED_ENCODER_B_STP5)
			{	StatusOfEncoderBoardLeds[1] |= 0x10; }
			else if (KeyIndex == LED_ENCODER_B_STP6)
			{	StatusOfEncoderBoardLeds[1] |= 0x20; }
			else if (KeyIndex == LED_ENCODER_B_STP7)
			{	StatusOfEncoderBoardLeds[1] |= 0x40; }
			else if (KeyIndex == LED_ENCODER_B_SEL)
			{	StatusOfEncoderBoardLeds[1] |= 0x80; }
			
			escrevePCA8575(ENDERECO_PCA_ENCODER_B, StatusOfEncoderBoardLeds[1]);
		}
		*/
	
	}
	
	
	//aplica off no registro de piscada
	if ((Cmd_On_or_Off_or_Blink == CMD_KEYLED_OFF) || (Cmd_On_or_Off_or_Blink == CMD_KEYLED_ON))
	{
		
		if (KeyIndex == ALL_LEDS)
		{
			for (PosPort = 0; PosPort < (7+5+5+5); PosPort++) // Preenche StatusOfKeyBoardLeds[x][1]
			{
				StatusOfKeyBoardLeds[PosPort][1] = 0xFF;
			}

		}
		else {

			PortIndex = GetPortIndex_OfKey(KeyIndex);
			StatusOfKeyBoardLeds[PortIndex][1] = (StatusOfKeyBoardLeds[PortIndex][1]) | ~(GetByteMask_OfKey(KeyIndex));
		}
		
	} 
	else if (Cmd_On_or_Off_or_Blink == CMD_KEYLED_BLINK)
	{
		
		if (KeyIndex == ALL_LEDS)
		{
			for (PosPort = 0; PosPort < (7+5+5+5); PosPort++) // Preenche StatusOfKeyBoardLeds[x][1]
			{
				StatusOfKeyBoardLeds[PosPort][1] = 0x00;
			}
	
		}
		else {

			PortIndex = GetPortIndex_OfKey(KeyIndex);
			StatusOfKeyBoardLeds[PortIndex][1] = (StatusOfKeyBoardLeds[PortIndex][1]) & (GetByteMask_OfKey(KeyIndex));
		}
		
	}

	

	
}

void RotacionaLedsDePCAEnderecado(unsigned char EnderecoPCA)
{
	unsigned char cntTmp;
	unsigned char cntBank;
	unsigned char barramento;
	unsigned int i;
	
	if (EnderecoPCA == ENDERECO_PCA_2_MM1300)//rotaciona teclas 1 a 16
	{
		//Acende o primeiro led e rotaciona até alçancar o ultimo led
		barramento = 0xFE;
		for (cntTmp = 0; cntTmp < 8; cntTmp++)
		{
			escreveRegistro(ENDERECO_PCA8575D_MM1300, barramento, 0xFF);
			for (i = 0; i < 20500 ; i++); // wait
			barramento = ~(barramento);
			barramento = barramento << 1;
			barramento = ~(barramento);
		}
		barramento = 0xFE;
		for (cntTmp = 0; cntTmp < 8; cntTmp++)
		{
			escreveRegistro(ENDERECO_PCA8575D_MM1300, 0xFF, barramento);
			for (i = 0; i < 20500 ; i++); // wait
			barramento = ~(barramento);
			barramento = barramento << 1;
			barramento = ~(barramento);
		}
		escreveRegistro(ENDERECO_PCA8575D_MM1300, 0xFF, 0xFF);//apaga leds
	}

	for (cntBank = 0; cntBank < 5; cntBank++)
	{
		if (cntBank == 2) //arranjo para banco 2 que não segue a sequencia
		{
			barramento = 0xFE;
			for (cntTmp = 0; cntTmp < 4; cntTmp++)
			{
				escreveRegistro(EnderecoPCA, io_configuration_register_banks[cntBank], barramento);
				for (i = 0; i < 20500 ; i++); // wait
				barramento = ~(barramento);
				barramento = barramento << 1;
				barramento = ~(barramento);
			}
			//apaga o barramento corrente
			escreveRegistro(EnderecoPCA, io_configuration_register_banks[cntBank], 0xFF);
		}
		else 
		{
			
			//Acende o primeiro led e rotaciona até alçancar o ultimo led
			barramento = 0xFE;
			for (cntTmp = 0; cntTmp < 8; cntTmp++)
			{
				escreveRegistro(EnderecoPCA, io_configuration_register_banks[cntBank], barramento);
				for (i = 0; i < 20500 ; i++); // wait
				barramento = ~(barramento);
				barramento = barramento << 1;
				barramento = ~(barramento);
			}
			//apaga o barramento corrente
			escreveRegistro(EnderecoPCA, io_configuration_register_banks[cntBank], 0xFF);
			
			if (cntBank == 4) //arranjo para banco 4 que não segue a sequencia, ao finalizar o 4 na rotina acima complemta 4 leds na porta 2
			{
				barramento = 0xEF;
				for (cntTmp = 0; cntTmp < 4; cntTmp++)
				{
					escreveRegistro(EnderecoPCA, io_configuration_register_banks[2], barramento); 
					for (i = 0; i < 20500 ; i++); // wait
					barramento = ~(barramento);
					barramento = barramento << 1;
					barramento = ~(barramento);
				}
				//apaga o barramento corrente
				escreveRegistro(EnderecoPCA, io_configuration_register_banks[2], 0xFF); 
			}
			
		}
	}
	
}

void ConfereERotacionaLedsDeTecladosExpansores(void)
{
	
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSEMEXPANSAO)) ==
		(HARDWARE_VERSION_56TECLASSEMEXPANSAO)) //inicializa base
	{ 
		RotacionaLedsDePCAEnderecado(ENDERECO_PCA_2_MM1300);
	}
	
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) //se teclado expansão 1 presente
	{ 
		brutalWatchdog = 0; //Feed the dog.	
		RotacionaLedsDePCAEnderecado(ENDERECO_PCA_3_MM1200_A);
	}
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) //se teclado expansão 2 presente
	{
		brutalWatchdog = 0; //Feed the dog.	
		RotacionaLedsDePCAEnderecado(ENDERECO_PCA_3_MM1200_B); 
	}
	
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) //se teclado expansão 3 presente
	{
		brutalWatchdog = 0; //Feed the dog.	
		RotacionaLedsDePCAEnderecado(ENDERECO_PCA_3_MM1200_C);
	}
	
}

void RunKeysJustFirstLine(void )
{
	unsigned char cntTmp;
	unsigned char cntBank;
	unsigned char barramento;
	unsigned char temp_io_register[2];
	unsigned int i;
	
	//salva situacao dos leds 
	lePCA8575RegistroUnico(ENDERECO_PCA8575D_MM1300, &temp_io_register[0]); //reg 6 pega automatico
	
	
	//Apaga todos os Leds 
	escreveRegistro(ENDERECO_PCA8575D_MM1300, 0xFF, 0xFF);
	
	//Acende o primeiro led e rotaciona até alçancar o ultimo led
	barramento = 0xFE;
	for (cntTmp = 0; cntTmp < 8; cntTmp++)
	{
		escreveRegistro(ENDERECO_PCA8575D_MM1300, barramento, 0xFF);
		for (i = 0; i < 20500 ; i++); // wait
		barramento = ~(barramento);
		barramento = barramento << 1;
		barramento = ~(barramento);
	}
	barramento = 0xFE;
	for (cntTmp = 0; cntTmp < 8; cntTmp++)
	{
		escreveRegistro(ENDERECO_PCA8575D_MM1300, 0xFF, barramento);
		for (i = 0; i < 20500 ; i++); // wait
		barramento = ~(barramento);
		barramento = barramento << 1;
		barramento = ~(barramento);
	}
	escreveRegistro(ENDERECO_PCA8575D_MM1300, 0xFF, 0xFF);//apaga leds

	

	
	//Restabelece situação dos Leds
	escreveRegistro(ENDERECO_PCA8575D_MM1300, temp_io_register[0], temp_io_register[1]);
	
}
	

void RunKeyLedsOneTime(void)
{
	//static unsigned char estadoAnteriorGPI;
	//unsigned char cntTmp;
	unsigned char cntBank;
	//unsigned char barramento;
	unsigned char temp_io_register[7+5+5+5];
	unsigned int i;
	
	//salva situacao dos leds 
	
	for (cntBank = 0; cntBank < 5; cntBank++)
	{
		leRegistroUnico(ENDERECO_PCA_2_MM1300, (io_configuration_register_banks[cntBank]), &temp_io_register[cntBank]);
	}
	//for (cntBank = 5; cntBank < 7; cntBank++)
	//{
	lePCA8575RegistroUnico(ENDERECO_PCA8575D_MM1300, &temp_io_register[5]); //reg 6 pega automatico
	//}
	
	//salva situacao dos leds  para teclados expansores
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) //se teclado expansão 1 presente
	{ 
		for (cntBank = 7; cntBank < 12; cntBank++)
		{
				leRegistroUnico(ENDERECO_PCA_3_MM1200_A, (io_configuration_register_banks[cntBank-7]), &temp_io_register[cntBank]);
		}							
	}
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) //se teclado expansão 2 presente
	{
		for (cntBank = 12; cntBank < 17; cntBank++)
		{
				leRegistroUnico(ENDERECO_PCA_3_MM1200_B, (io_configuration_register_banks[cntBank-12]), &temp_io_register[cntBank]);
		}							
	}
	
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) //se teclado expansão 3 presente
	{
		for (cntBank = 17; cntBank < 22; cntBank++)
		{
				leRegistroUnico(ENDERECO_PCA_3_MM1200_C, (io_configuration_register_banks[cntBank-17]), &temp_io_register[cntBank]);
		}							
	}	
	
	
	
	//Apaga todos os Leds 
	ApagaTodasAsTeclas();
	
	//ConfereERotacionaLedsDeTecladosExpansores(); 
	

	
	//Restabelece situação dos Leds
	
	for (cntBank = 0; cntBank < 5; cntBank++)
	{
		escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[cntBank], temp_io_register[cntBank]);
	}

	escreveRegistro(ENDERECO_PCA8575D_MM1300, temp_io_register[5], temp_io_register[6]);
	
	//restaura situacao dos leds  para teclados expansores
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) //se teclado expansão 1 presente
	{ 
		for (cntBank = 7; cntBank < 12; cntBank++)
		{
				escreveRegistro(ENDERECO_PCA_3_MM1200_A, (io_configuration_register_banks[cntBank-7]), temp_io_register[cntBank]);
		}							
	}
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) //se teclado expansão 2 presente
	{
		for (cntBank = 12; cntBank < 17; cntBank++)
		{
				escreveRegistro(ENDERECO_PCA_3_MM1200_B, (io_configuration_register_banks[cntBank-12]), temp_io_register[cntBank]);
		}							
	}
	
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) //se teclado expansão 3 presente
	{
		for (cntBank = 17; cntBank < 22; cntBank++)
		{
				escreveRegistro(ENDERECO_PCA_3_MM1200_C, (io_configuration_register_banks[cntBank-17]), temp_io_register[cntBank]);
		}							
	}	


}

unsigned char Key33And34And35PressedToEnterInTestMode(void)
{
	
	unsigned char bufferLeituraPCAParaTesteTecla33_34_35Pressed;
/*		
	unsigned char ArrayIndicaTecla[5][8] = {
	{TECLA_17, TECLA_18, TECLA_19, TECLA_20, TECLA_21, TECLA_22, TECLA_23, TECLA_24},
	{TECLA_25, TECLA_26, TECLA_27, TECLA_28, TECLA_29, TECLA_30, TECLA_31, TECLA_32},
	{TECLA_49, TECLA_50, TECLA_51, TECLA_52, TECLA_53, TECLA_54, TECLA_55, TECLA_56},
	{TECLA_33, TECLA_34, TECLA_35, TECLA_36, TECLA_37, TECLA_38, TECLA_39, TECLA_40},
	{TECLA_41, TECLA_42, TECLA_43, TECLA_44, TECLA_45, TECLA_46, TECLA_47, TECLA_48}
};	11100000 => 0x07 */
	
	//escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[4], 0xFF); //Configura como entrada

	//efetua leitura das portas
	leRegistroUnico(ENDERECO_PCA_2_MM1300, (input_port_register_bank[3]),&bufferLeituraPCAParaTesteTecla33_34_35Pressed);
		
	// reescreve configuração de teclas acesas e apagadas normalmente
	//escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[4], ((StatusOfKeyBoardLeds[cntBank][0])));
		//ESP Alteração feita para solucionar o erro de comparação
		if (bufferLeituraPCAParaTesteTecla33_34_35Pressed == (unsigned char)(~0x07))
			return 1;
		else return 0;
		
		
}


void AplicaValorFixoEmTodosOsPCAS(unsigned char ValorFixo)
{
	//ESP Rotina responsável por enviar um valor para todos os expansores PCAs da placa
	//Versão protótipo enviando diretamente para o PCA9506 e PCA8575 mas deve ser expansivel para mais PCAs

	//Configura o PCA9506 como output
	for (uint8_t i = 0; i < 5; i++) {
		escreveRegistro(0x22, io_configuration_register_banks[i], 0x00);
	}
	//Envia o valor presente em ValorFixo para os PCA9506
	for (int i = 0; i < 5; i++) {
		escreveRegistro(0x22, output_port_register_banks[i], ValorFixo);
	}
	//Envia o valor presente em ValorFixo para o PCA8575D
	escreve_2bytes_PCA8575(0x21, ValorFixo);
}

void AcendeTodasAsTeclas(void)
{
	AplicaValorFixoEmTodosOsPCAS(0x00);
}

void ApagaTodasAsTeclas(void)
{
	AplicaValorFixoEmTodosOsPCAS(0xFF);
}


void AcendeAsTeclasPares(void)
{
	AplicaValorFixoEmTodosOsPCAS(0x55);

}

void AcendeAsTeclasImpares(void)
{
	AplicaValorFixoEmTodosOsPCAS(0xAA);

}
void RunTestMode(void)
{
	//PARA VERSAO 40 TECLAS LAYOUT ANTIGO
	//Entra na rotina de testes ao inicar mantendo as teclas 6, 23 e 40 apertadas
	//tecla 1: acende todos
	//tecla 2: acende IMPAR
	//tecla 3: acende PAR
	//tecla 4 teste de teclas
	// apertando teclas 1 e 40 simultaneamente sai do modo de teste de teclas
	//tecla 5 Sai do modo de teste

	//PARA VERSAO 56 TECLAS LAYOUT NOVO
	//Entra na rotina de testes ao inicar mantendo as teclas 33, 34 e 35 apertadas
	//tecla 17: acende todos (ok)
	//tecla 18: apaga todos (ok)
	//tecla 19: acende IMPAR (ok)
	//tecla 20: acende PAR (ok)
	//tecla 21 teste de teclas
	// apertando teclas 47 e 48 simultaneamente sai do modo de teste de teclas (ok)
	//tecla 21 Sai do modo de teste (ok)
	

	
	unsigned char bufferLeituraPCAParaMenuDoTeste;
	//unsigned char bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[(7+5+5+5)];
	unsigned char cntBank;
	unsigned char TempPort0;
	//unsigned char Key47And48PressedWhenLow;
	//unsigned char bufferToCheckEncoder[3];
	
	
	// indica entrada no modo de teste
	//RunKeyLedsOneTime();
	//brutalWatchdog = 0; //Feed the dog.	
	//RunKeyLedsOneTime();
	//brutalWatchdog = 0; //Feed the dog.	
	
	TempPort0 = 0xFF;
	
	//Key47And48PressedWhenLow = 1;
	

	//ESP Loop responsável pelo menu de escolha de testes no modo de testes, le o banco {x} de input do PCA9506
	while (1)
	{		
		//Configura o banco 2 como entrada
		escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[2], 0xFF); //Configura como entrada		
		//Efetua a leitura do banco
		leRegistroUnico(ENDERECO_PCA_2_MM1300, (input_port_register_bank[2]),&bufferLeituraPCAParaMenuDoTeste);
		printf("Debug leu o banco %u", bufferLeituraPCAParaMenuDoTeste);
		
		escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[2], TempPort0);
		if (bufferLeituraPCAParaMenuDoTeste == 0xF8)
		{
			ApagaTodasAsTeclas();
			break;
		}
		if (bufferLeituraPCAParaMenuDoTeste == 0x7F) 
		{
			TempPort0 = 0xFF;
			ApagaTodasAsTeclas();
		}
		else if (bufferLeituraPCAParaMenuDoTeste == 0xF7)
		{
			TempPort0 = 0x00;
			AcendeTodasAsTeclas();
		}
		else if (bufferLeituraPCAParaMenuDoTeste == 0xEF)
		{
			TempPort0 = 0x55;
			AcendeAsTeclasPares();
		}
		else if (bufferLeituraPCAParaMenuDoTeste == 0xDF)
		{
			TempPort0 = 0xAA;
			AcendeAsTeclasImpares();
		}
		vTaskDelay(pdMS_TO_TICKS(120));
			/*
			//restabelece porta 0
			if ((bufferLeituraPCAParaMenuDoTeste|(~0x01)) == (~0x01)) //tecla 17: acende todos
			{
				AcendeTodasAsTeclas(); 
				
			}
			else if ((bufferLeituraPCAParaMenuDoTeste|(~0x02)) == (~0x02)) //tecla 18: apaga todas
			{
				ApagaTodasAsTeclas();
				
			}
			else if ((bufferLeituraPCAParaMenuDoTeste|(~0x04)) == (~0x04)) //tecla 19: acende IMPAR
			{
				AcendeAsTeclasImpares();
				
			}
			else if ((bufferLeituraPCAParaMenuDoTeste|(~0x08)) == (~0x08)) //tecla 20: acende PAR
			{
				AcendeAsTeclasPares();
				
			}
			*/
			
			
			//else if ((bufferLeituraPCAParaMenuDoTeste|(~0x10)) == (~0x10)) //tecla 21 teste de teclas
			//{
			//	ApagaTodasAsTeclas();
				
				/*/apaga teclas 1 a 16
				escreveRegistro(ENDERECO_PCA8575D_MM1300, 0xFF, 0xFF);
				
				//apaga leds do encoder
				if	((AuxVarToShowIfEncoderAandBAreConnected&PCA_ENCODER_A_CONNECTED) == PCA_ENCODER_A_CONNECTED)
				{
					escrevePCA8574(ENDERECO_PCA_ENCODER_A, 0xFF);
				}
					
				
				if	((AuxVarToShowIfEncoderAandBAreConnected&PCA_ENCODER_B_CONNECTED) == PCA_ENCODER_B_CONNECTED)
				{
					escrevePCA8574(ENDERECO_PCA_ENCODER_B, 0xFF);
				}

				
				for (cntBank = 0; cntBank < 5; cntBank++)
				{
					escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[cntBank], 0xFF); //Configura como entrada
					
				}*/
			//	TempPort0 = 0xFF;

				//espera timer estourar para garantir acendimento das teclas visível
			/*
				while (timerTick != 0);
				timerTick = VALOR_TIMEOUT_READKEY_OUTINT;

				while (Key47And48PressedWhenLow)
				{
					if (timerTick == 0)
					{
						//etapa 1 le as teclas para efetuar o teste
						for (cntBank = 0; cntBank < 7; cntBank++)
						{
							if (cntBank <= 4)
							{
								escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[cntBank], 0xFF); //Configura como entrada

								//efetua leitura das portas
								leRegistroUnico(ENDERECO_PCA_2_MM1300, (input_port_register_bank[cntBank]),&bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank]);
							}
							else if (cntBank == 5)//|| (cntBank == 6))
							{
								lePCA8575RegistroUnico(ENDERECO_PCA8575D_MM1300, &bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[5]); ////reg 6 pega automatico
							}
						}
						
						//etapa 1 para teclados expansores
						if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) ==
							(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) //se teclado expansão 1 presente
						{ 
							for (cntBank = 7; cntBank < 12; cntBank++)
							{
									escreveRegistro(ENDERECO_PCA_3_MM1200_A, io_configuration_register_banks[(cntBank-7)], 0xFF); //Configura como entrada

									//efetua leitura das portas
									leRegistroUnico(ENDERECO_PCA_3_MM1200_A, (input_port_register_bank[(cntBank-7)]),&bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank]);
							}							
						}
						if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) ==
							(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) //se teclado expansão 2 presente
						{
							for (cntBank = 12; cntBank < 17; cntBank++)
							{
									escreveRegistro(ENDERECO_PCA_3_MM1200_B, io_configuration_register_banks[(cntBank-12)], 0xFF); //Configura como entrada

									//efetua leitura das portas
									leRegistroUnico(ENDERECO_PCA_3_MM1200_B, (input_port_register_bank[(cntBank-12)]),&bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank]);
							}							
						}
						
						if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) ==
							(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) //se teclado expansão 3 presente
						{
							for (cntBank = 17; cntBank < 22; cntBank++)
							{
									escreveRegistro(ENDERECO_PCA_3_MM1200_C, io_configuration_register_banks[(cntBank-17)], 0xFF); //Configura como entrada

									//efetua leitura das portas
									leRegistroUnico(ENDERECO_PCA_3_MM1200_C, (input_port_register_bank[(cntBank-17)]),&bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank]);
							}							
						}						
						
						//etapa 2 ve se alguma tecla foi apertada e acende algum led						
						for (cntBank = 0; cntBank < 7; cntBank++)
						{
							if ((bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank]) != 0xFF) // alguma tecla foi apertada
							{
								if (cntBank == 0)
								{
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[3], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank])); 
								}
								else if (cntBank == 1){
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[4], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank])); 
								}
								else if (cntBank == 2){
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[0], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank])); 
								}
								else if (cntBank == 3){
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[0], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank])); 
								}
								else if (cntBank == 4){
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[1], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank]));

									if (((bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[4])|(~0xC0)) == (~0xC0)) // apertando teclas 47 e 48 simultaneamente sai do modo de teste de teclas
									{
										bufferLeituraPCAParaMenuDoTeste = 0xFF;
										RunKeyLedsOneTime();
										Key47And48PressedWhenLow = 0;
									} 
									
								}
								else if (cntBank == 5){
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[0], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank])); 
								}
								else if (cntBank == 6){
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[1], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank])); 
								}

								
								
							}
			*/
							/*
							else 
							{
								//ESP condicional bastante alterada, retirada as funções usb
									if ((bufferToCheckEncoder[0] == CMD_KEY_NOTIFY) && 
										(bufferToCheckEncoder[1] == ENCODER_A_CLICK) && 
										(bufferToCheckEncoder[2] == PRESSED) )
									{
										escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[0], 0xFE); 
									}
									if ((bufferToCheckEncoder[0] == CMD_KEY_NOTIFY) && 
										(bufferToCheckEncoder[1] == ENCODER_A_TURN) && 
										(bufferToCheckEncoder[2] == TURN_CLOCKWISE) )
									{
										escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[0], 0xFD); 
									}
									if ((bufferToCheckEncoder[0] == CMD_KEY_NOTIFY) && 
										(bufferToCheckEncoder[1] == ENCODER_A_TURN) && 
										(bufferToCheckEncoder[2] == TURN_ANTICLOCKWISE) )
									{
										escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[0], 0xFB); 
									}
									
									if ((bufferToCheckEncoder[0] == CMD_KEY_NOTIFY) && 
										(bufferToCheckEncoder[1] == ENCODER_B_CLICK) && 
										(bufferToCheckEncoder[2] == PRESSED) )
									{
										escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[2], 0xFE); 
									}
									if ((bufferToCheckEncoder[0] == CMD_KEY_NOTIFY) && 
										(bufferToCheckEncoder[1] == ENCODER_B_TURN) && 
										(bufferToCheckEncoder[2] == TURN_CLOCKWISE) )
									{
										escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[2], 0xFD); 
									}
									if ((bufferToCheckEncoder[0] == CMD_KEY_NOTIFY) && 
										(bufferToCheckEncoder[1] == ENCODER_B_TURN) && 
										(bufferToCheckEncoder[2] == TURN_ANTICLOCKWISE) )
									{
										escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[2], 0xFB); 
									}
							}
							*/
							/*
						}
						//etapa 2 para teclados expansores
						if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) ==
							(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) //se teclado expansão 1 presente
						{ 
							for (cntBank = 7; cntBank < 12; cntBank++)
							{
								if ((bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank]) != 0xFF) // alguma tecla foi apertada
								{
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[(cntBank-7)], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank])); 
								}								
							}							
						}
						if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) ==
							(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) //se teclado expansão 2 presente
						{
							for (cntBank = 12; cntBank < 17; cntBank++)
							{
								if ((bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank]) != 0xFF) // alguma tecla foi apertada
								{
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[(cntBank-12)], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank])); 
								}								
							}							
						}
						
						if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) ==
							(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) //se teclado expansão 3 presente
						{
							for (cntBank = 17; cntBank < 22; cntBank++)
							{
								if ((bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank]) != 0xFF) // alguma tecla foi apertada
								{
									escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[(cntBank-17)], (bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[cntBank])); 
								}								
							}							
						}							
						
					
						timerTick = VALOR_TIMEOUT_READKEY_OUTINT;	
			
					}
							*/
					/*
					else {
						
						GerenciaEncoders();
					
					}
					*/
						
					
					
					//Feed the dog.	
					//brutalWatchdog = 0; 
					
				/*
				}

				//apaga teclas 1 a 16
				escreveRegistro(ENDERECO_PCA8575D_MM1300, 0xFF, 0xFF);
				
				//apaga teclas 17 a 56
				for (cntBank = 0; cntBank < 5; cntBank++)
				{
					escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[cntBank], 0xFF); //Configura como entrada
				}
				
				
				Key47And48PressedWhenLow = 1;

			}
			else if ((bufferLeituraPCAParaMenuDoTeste|(~0x20)) == (~0x20)) //tecla 22 Sai do modo de teste
			{
				//apaga leds do encoder
				if	((AuxVarToShowIfEncoderAandBAreConnected&PCA_ENCODER_A_CONNECTED) == PCA_ENCODER_A_CONNECTED)
				{
					escrevePCA8575(ENDERECO_PCA_ENCODER_A, 0xFF);
				}
					
				
				if	((AuxVarToShowIfEncoderAandBAreConnected&PCA_ENCODER_B_CONNECTED) == PCA_ENCODER_B_CONNECTED)
				{
					escrevePCA8575(ENDERECO_PCA_ENCODER_B, 0xFF);
				}

				//apaga teclas 1 a 16
				escreveRegistro(ENDERECO_PCA8575D_MM1300, 0xFF, 0xFF);
				
				//apaga teclas 17 a 56
				for (cntBank = 0; cntBank < 5; cntBank++)
				{
					escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[cntBank], 0xFF); //Configura como entrada
				}
				RunKeyLedsOneTime();
				break;
				//TestWhile1 = 0;

			}
				
					
		
			timerTick = VALOR_TIMEOUT_READKEY_OUTINT;	
			
		}
		
				*/
		
		/******************************************************************
		 Feed the WTD
		*******************************************************************/ 
		//brutalWatchdog = 0; //Feed the dog.	
	
	}
}
	

void ThreadReadKey_SemInt(void)
{
	//ESP para a primeira versão funcional da Mesa vou utilizar os endereços específicos dos PCAs 9506 e 8575
	ThreadReadKey_SemInt_Individualmente(0x22);

	/*ESP trecho original que permite a escalabilidade para outras placas maiores, implementar em uma versão completa da mesa
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSEMEXPANSAO)) ==
		(HARDWARE_VERSION_56TECLASSEMEXPANSAO)) //inicializa base
	{ 
		ThreadReadKey_SemInt_Individualmente(ENDERECO_PCA_2_MM1300);
	}
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS1DETECTED)) //se teclado expansão 1 presente
	{ 
		//Timer8_Stop();
		ThreadReadKey_SemInt_Individualmente(ENDERECO_PCA_3_MM1200_A);
		//Timer8_Start();
	}
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS2DETECTED)) //se teclado expansão 2 presente
	{
		//Timer8_Stop();		
		ThreadReadKey_SemInt_Individualmente(ENDERECO_PCA_3_MM1200_B); 
		//Timer8_Start();
	}
	
	if (((AuxVarToShowVersionOfHardwareBoard)&(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) ==
		(HARDWARE_VERSION_56TECLASSCOM1EXPANSAO_POS3DETECTED)) //se teclado expansão 3 presente
	{
		//Timer8_Stop();
		ThreadReadKey_SemInt_Individualmente(ENDERECO_PCA_3_MM1200_C);
		//Timer8_Start();		
	}
	*/
	
}

void ThreadReadKey_SemInt_Individualmente (unsigned char i2CAddress) 
{
	//unsigned char * barramento;
	unsigned char cntBank, cntTmp, varBitSelect, cntBankInicialRefPCA, cntBankFinalRefPCA_1, cntBankFinalRefPCA_2;
	
	/*unsigned char ArrayIndicaTecla[5][8] = {
	{TECLA_7, TECLA_8, TECLA_9, TECLA_10, TECLA_11, TECLA_12, TECLA_13, TECLA_14},
	{TECLA_15, TECLA_16, TECLA_17, TECLA_18, TECLA_19, TECLA_20, TECLA_21, TECLA_22},
	{TECLA_24, TECLA_25, TECLA_26, TECLA_27, TECLA_28, TECLA_29, TECLA_30, TECLA_31},
	{TECLA_32, TECLA_33, TECLA_34, TECLA_35, TECLA_36, TECLA_37, TECLA_38, TECLA_39},
	{TECLA_6, TECLA_23, TECLA_40, TECLA_1, TECLA_2, TECLA_3, TECLA_4, TECLA_5}
};
	unsigned char ArrayIndicaTecla[7][8] = {
	{TECLA_17, TECLA_18, TECLA_19, TECLA_20, TECLA_21, TECLA_22, TECLA_23, TECLA_24},
	{TECLA_25, TECLA_26, TECLA_27, TECLA_28, TECLA_29, TECLA_30, TECLA_31, TECLA_32},
	{TECLA_49, TECLA_50, TECLA_51, TECLA_52, TECLA_53, TECLA_54, TECLA_55, TECLA_56},
	{TECLA_33, TECLA_34, TECLA_35, TECLA_36, TECLA_37, TECLA_38, TECLA_39, TECLA_40},
	{TECLA_41, TECLA_42, TECLA_43, TECLA_44, TECLA_45, TECLA_46, TECLA_47, TECLA_48},
	{TECLA_1, TECLA_2, TECLA_3, TECLA_4, TECLA_5, TECLA_6, TECLA_7, TECLA_8},
	{TECLA_9, TECLA_10, TECLA_11, TECLA_12, TECLA_13, TECLA_14, TECLA_15, TECLA_16}
};*/
	if (i2CAddress == ENDERECO_PCA_2_MM1300)
	{
		cntBankInicialRefPCA = 0;
		cntBankFinalRefPCA_1 = 5;
		cntBankFinalRefPCA_2 = 7;
	}
	else if (i2CAddress == ENDERECO_PCA_3_MM1200_A)
	{
		cntBankInicialRefPCA = 7;
		cntBankFinalRefPCA_1 = cntBankInicialRefPCA + 5;
		cntBankFinalRefPCA_2 = cntBankFinalRefPCA_1;
	} 
	else if (i2CAddress == ENDERECO_PCA_3_MM1200_B)
	{
		cntBankInicialRefPCA = 7+5;
		cntBankFinalRefPCA_1 = cntBankInicialRefPCA + 5;
		cntBankFinalRefPCA_2 = cntBankFinalRefPCA_1;
	}
	else if (i2CAddress == ENDERECO_PCA_3_MM1200_C)
	{
		cntBankInicialRefPCA = 7+5+5;
		cntBankFinalRefPCA_1 = cntBankInicialRefPCA + 5;
		cntBankFinalRefPCA_2 = cntBankFinalRefPCA_1;
	}
	else{
		cntBankInicialRefPCA = 0;
		cntBankFinalRefPCA_1 = 5;
		cntBankFinalRefPCA_2 = 7;
	}

	for (cntBank = cntBankInicialRefPCA; cntBank < cntBankFinalRefPCA_1; cntBank++) //esta é a logica do pca9506
	{
		escreveRegistro(i2CAddress, io_configuration_register_banks[(cntBank-cntBankInicialRefPCA)], 0xFF); //Configura como entrada

		//efetua leitura das portas
		leRegistroUnico(i2CAddress, (input_port_register_bank[(cntBank-cntBankInicialRefPCA)]),&bufferLeituraPCA1_imediatamenteAposPolling[cntBank]);
		
		if ((StatusOfKeyBoardLeds[cntBank][1]) == 0xFF) //se todas as teclas do banco não piscam,
		{	// reescreve configuração de teclas acesas e apagadas normalmente
			escreveRegistro(i2CAddress, io_configuration_register_banks[(cntBank-cntBankInicialRefPCA)], ((StatusOfKeyBoardLeds[cntBank][0])));
			
		}
		else // se alguma tecla pisca no banco
		{ 
			for (cntTmp = 0; cntTmp < 8; cntTmp++)
			{	
				if (cntTmp == 0)
					varBitSelect = BIT_0;
				else if (cntTmp == 1)
					varBitSelect = BIT_1;
				else if (cntTmp == 2)
					varBitSelect = BIT_2;
				else if (cntTmp == 3)
					varBitSelect = BIT_3;
				else if (cntTmp == 4)
					varBitSelect = BIT_4;
				else if (cntTmp == 5)
					varBitSelect = BIT_5;
				else if (cntTmp == 6)
					varBitSelect = BIT_6;
				else if (cntTmp == 7)
					varBitSelect = BIT_7;			

			
				if ((((StatusOfKeyBoardLeds[cntBank][1])) & varBitSelect) == varBitSelect) //se bit 0 não pisca, aplica configuracao original do bit
				{
					if ((((StatusOfKeyBoardLeds[cntBank][0])) & varBitSelect) == 0x00)
					{
						AuxVarToBlinkBanks[cntBank] &= ~varBitSelect;
					}
					else AuxVarToBlinkBanks[cntBank] |= varBitSelect;
				}
				else //aplica a piscada
				{
					AuxVarToBlinkBanks[cntBank] ^= varBitSelect;
				}
			}
			
			escreveRegistro(i2CAddress, io_configuration_register_banks[cntBank-cntBankInicialRefPCA], AuxVarToBlinkBanks[cntBank]);
		}
	}
	

	if (i2CAddress == ENDERECO_PCA_2_MM1300) //processa comando das teclas 1 a 16 que ocorre apenas na placa base
	{
		//for (cntBank = 5; cntBank < 7; cntBank++) //feito separado pois a logica do pca8575 é diferente
		//evita 1 leitura de i2c com o for estranho abaixo
		//escreveRegistro(ENDERECO_PCA8575D_MM1300, 0xFF, 0xFF); //configura como saida alta para efetuar leitura
		escreve_2bytes_PCA8575(ENDERECO_PCA8575D_MM1300, 0xFF);
		//efetua leitura das portas
		lePCA8575RegistroUnico(ENDERECO_PCA8575D_MM1300, &bufferLeituraPCA1_imediatamenteAposPolling[5]); //reg 6 pega automatico
		
		//escreveRegistro(ENDERECO_PCA8575D_MM1300, StatusOfKeyBoardLeds[5][0], StatusOfKeyBoardLeds[6][0]); //retorna o valor definido anteriormente

			
		if (((StatusOfKeyBoardLeds[5][1]) == 0xFF) && ((StatusOfKeyBoardLeds[6][1]) == 0xFF)) //se todas as teclas do banco não piscam,

		{	// reescreve configuração de teclas acesas e apagadas normalmente
			escreveRegistro(ENDERECO_PCA8575D_MM1300, StatusOfKeyBoardLeds[5][0], StatusOfKeyBoardLeds[6][0]);
			
		}
		else // se alguma tecla pisca no banco
		{ 
			for (cntBank = 5; cntBank < 7; cntBank++)
			{
				for (cntTmp = 0; cntTmp < 8; cntTmp++)
				{	
					if (cntTmp == 0)
						varBitSelect = BIT_0;
					else if (cntTmp == 1)
						varBitSelect = BIT_1;
					else if (cntTmp == 2)
						varBitSelect = BIT_2;
					else if (cntTmp == 3)
						varBitSelect = BIT_3;
					else if (cntTmp == 4)
						varBitSelect = BIT_4;
					else if (cntTmp == 5)
						varBitSelect = BIT_5;
					else if (cntTmp == 6)
						varBitSelect = BIT_6;
					else if (cntTmp == 7)
						varBitSelect = BIT_7;			

				
					if ((((StatusOfKeyBoardLeds[cntBank][1])) & varBitSelect) == varBitSelect) //se bit 0 não pisca, aplica configuracao original do bit
					{
						if ((((StatusOfKeyBoardLeds[cntBank][0])) & varBitSelect) == 0x00)
						{
							AuxVarToBlinkBanks[cntBank] &= ~varBitSelect;
						}
						else AuxVarToBlinkBanks[cntBank] |= varBitSelect;
					}
					else //aplica a piscada
					{
						AuxVarToBlinkBanks[cntBank] ^= varBitSelect;
					}
				}
				
				escreveRegistro(ENDERECO_PCA8575D_MM1300, AuxVarToBlinkBanks[5], AuxVarToBlinkBanks[6]);
			}
		}
	}
	
	//aqui varre o vetor e verifica teclas apertadas para gerar comando usb	
	for (cntBank = cntBankInicialRefPCA; cntBank < cntBankFinalRefPCA_2; cntBank++)
	{
		
		for (cntTmp = 0; cntTmp < 8; cntTmp++)
		{	
			if (cntTmp == 0)
				varBitSelect = 0x01;
			else if (cntTmp == 1)
				varBitSelect = 0x02;
			else if (cntTmp == 2)
				varBitSelect = 0x04;
			else if (cntTmp == 3)
				varBitSelect = 0x08;
			else if (cntTmp == 4)
				varBitSelect = 0x10;
			else if (cntTmp == 5)
				varBitSelect = 0x20;
			else if (cntTmp == 6)
				varBitSelect = 0x40;
			else if (cntTmp == 7)
				varBitSelect = 0x80;
			
		  	// verifica se houve alteração e notifica USB através de comando na fila
			if ((((bufferLeituraPCA1_imediatamenteAposPolling[cntBank])&varBitSelect) != ((bufferLeituraPCA1_seminterrupcao[cntBank])&varBitSelect)))
			{
				if (((bufferLeituraPCA1_imediatamenteAposPolling[cntBank])& varBitSelect) == 0x00)
				{
					//SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ArrayIndicaTecla[cntBank][cntTmp], PRESSED);
					printf("TECLA PRESSIONADA: %u\n", ArrayIndicaTecla[cntBank][cntTmp]);
					ManageKeyLeds(CMD_KEYLED_ON, ArrayIndicaTecla[cntBank][cntTmp]);
					bufferLeituraPCA1_seminterrupcao[cntBank] &= ~varBitSelect;
				}
				else 
				{
					//SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ArrayIndicaTecla[cntBank][cntTmp], RELEASED);
					printf("TECLA SOLTA: %u\n", ArrayIndicaTecla[cntBank][cntTmp]);
					bufferLeituraPCA1_seminterrupcao[cntBank] |= varBitSelect;
				}
			}
		}
	}
	
}



/*
void GerenciaEncoders (void) 
{
	//gerencia  encoder
	PRT2DR_Sampled = PRT2DR;

	
	//////////////////////////////////////////////
	//CLICK DOS ENCODERS 
	//verifica se houve alteração e notifica USB através de comando na fila
	
	if (((PRT2DR_Sampled)&BIT_6) != ((PRT2DR_Old)&BIT_6))
	{
		if (((PRT2DR_Sampled)& BIT_6) == 0x00)
		{
			SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ENCODER_A_CLICK, PRESSED);
		}
		else 
		{
			SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ENCODER_A_CLICK, RELEASED);
		}
	}
*/
	
	/*REMOVIDO POIS PLACA 56 PINOS NAO ACEITA 2 ENCODERS
	if (((PRT2DR_Sampled)&BIT_2) != ((PRT2DR_Old)&BIT_2))
	{
		if (((PRT2DR_Sampled)& BIT_2) == 0x00)
		{
			SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ENCODER_B_CLICK, PRESSED);
		}
		else 
		{
			SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ENCODER_B_CLICK, RELEASED);
		}
	}
	END REMOVIDO POIS PLACA 56 PINOS NAO ACEITA 2 ENCODERS*/
	//////////////////////////////////////////////
	//ROTACAO DOS ENCODERS 
/*
	//encoder PGM
	if (((((PRT2DR_Sampled)&BIT_4) != ((PRT2DR_Old)&BIT_4)) ||
		(((PRT2DR_Sampled)&BIT_5) != ((PRT2DR_Old)&BIT_5)) ) 
		)
	{	
		
		// Record the A and B signals in seperate sequences
		EncoderA_seqPin1 <<= 1;
		EncoderA_seqPin2 <<= 1;
		if (((PRT2DR_Sampled)&BIT_4) == BIT_4)
		{
			EncoderA_seqPin1 = EncoderA_seqPin1 | 0x01;
		}
		else 
		{
			EncoderA_seqPin1 = EncoderA_seqPin1 & ~0x01;
		}

		if (((PRT2DR_Sampled)&BIT_5) == BIT_5)
		{
			EncoderA_seqPin2 = EncoderA_seqPin2 | 0x01;
		}
		else 
		{
			EncoderA_seqPin2 = EncoderA_seqPin2 & ~0x01;
		}
		
    	// Mask the MSB four bits
    	EncoderA_seqPin1 &= 0b00001111;
    	EncoderA_seqPin2 &= 0b00001111;
		
		
 		// Compare the recorded sequence with the expected sequence
    	if (EncoderA_seqPin1 == 0b00001001 && EncoderA_seqPin2 == 0b00000011) 
		{	//sentido horario
			SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ENCODER_A_TURN, TURN_CLOCKWISE);
      	}
     
    	if (EncoderA_seqPin1 == 0b00000011 && EncoderA_seqPin2 == 0b00001001) 
		{
			//sentido antihorario
			SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ENCODER_A_TURN, TURN_ANTICLOCKWISE);
      	}
		
	}
	
*/
	
	/*END REMOVIDO POIS PLACA 56 PINOS NAO ACEITA 2 ENCODERS
	//encoder PST
	if ((((PRT2DR_Sampled)&BIT_0) != ((PRT2DR_Old)&BIT_0)) ||
		(((PRT2DR_Sampled)&BIT_1) != ((PRT2DR_Old)&BIT_1)) )
	{
		// Record the A and B signals in seperate sequences
		EncoderB_seqPin1 <<= 1;
		EncoderB_seqPin2 <<= 1;
		if (((PRT2DR_Sampled)&BIT_0) == BIT_0)
		{
			EncoderB_seqPin1 = EncoderB_seqPin1 | 0x01;
		}
		else 
		{
			EncoderB_seqPin1 = EncoderB_seqPin1 & ~0x01;
		}

		if (((PRT2DR_Sampled)&BIT_1) == BIT_1)
		{
			EncoderB_seqPin2 = EncoderB_seqPin2 | 0x01;
		}
		else 
		{
			EncoderB_seqPin2 = EncoderB_seqPin2 & ~0x01;
		}
		
    	// Mask the MSB four bits
    	EncoderB_seqPin1 &= 0b00001111;
    	EncoderB_seqPin2 &= 0b00001111;
		
		
 		// Compare the recorded sequence with the expected sequence
    	if (EncoderB_seqPin1 == 0b00001001 && EncoderB_seqPin2 == 0b00000011) 
		{	//sentido horario
			SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ENCODER_B_TURN, TURN_CLOCKWISE);
      	}
     
    	if (EncoderB_seqPin1 == 0b00000011 && EncoderB_seqPin2 == 0b00001001) 
		{
			//sentido antihorario
			SCQ_InsertNewCommand(CMD_KEY_NOTIFY,  ENCODER_B_TURN, TURN_ANTICLOCKWISE);
      	}
		
		
	}
	END REMOVIDO POIS PLACA 56 PINOS NAO ACEITA 2 ENCODERS*/
	

//	PRT2DR_Old = PRT2DR_Sampled;


