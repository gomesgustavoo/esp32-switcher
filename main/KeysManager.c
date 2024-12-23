
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
#include "udp_server.h"
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
	switch (KeyIndex)
	{
		case TECLA_17:
		case TECLA_18: 
		case TECLA_19:
		case TECLA_20:
		case TECLA_21:
		case TECLA_22:
		case TECLA_23:
		case TECLA_24:
			return 0;
			break;
			
	 	case TECLA_25 :  
		case TECLA_26 :  
		case TECLA_27 :  
		case TECLA_28 :  
		case TECLA_29 :  
		case TECLA_30 :  
		case TECLA_31 :  
		case TECLA_32 :			
			return 1;
			break;
	 
		case TECLA_49 :  
		case TECLA_50 :  
		case TECLA_51 :  
		case TECLA_52 :  
		case TECLA_53 :  
		case TECLA_54 : 
		case TECLA_55 :  
		case TECLA_56 :			
			return 2;
			break;
		
		case TECLA_33 :  
		case TECLA_34 :  
		case TECLA_35 : 
		case TECLA_36 : 
		case TECLA_37 :  
		case TECLA_38 :  
		case TECLA_39 :  
		case TECLA_40 :			
			return 3;
			break;
		case TECLA_41 :  
		case TECLA_42 : 
		case TECLA_43 : 
		case TECLA_44 :  
		case TECLA_45 :
		case TECLA_46 : 
		case TECLA_47 :  
		case TECLA_48 :			
			return 4;
			break;
		case TECLA_1 :  
		case TECLA_2 :  
		case TECLA_3 :
		case TECLA_4 :  
		case TECLA_5 :  
		case TECLA_6 :  
		case TECLA_7 :  
		case TECLA_8 :		
			return 5;
			break;
		case TECLA_9 :  
		case TECLA_10 : 
		case TECLA_11 : 
		case TECLA_12 : 
		case TECLA_13 :  
		case TECLA_14 : 
		case TECLA_15 : 
		case TECLA_16 :		
			return 6;
			break;

		case TECLA_57 :  
		case TECLA_58 :  
		case TECLA_59 :  
		case TECLA_60 :  
		case TECLA_61 :  
		case TECLA_62 :  
		case TECLA_63 :  
		case TECLA_64 :			
			return 7;
			break;
		case TECLA_65 : 
		case TECLA_66 :  
		case TECLA_67 :  
		case TECLA_68 :  
		case TECLA_69 : 
		case TECLA_70 :  
		case TECLA_71 :
		case TECLA_72 :		
			return 8;
			break;
		case TECLA_89 :
		case TECLA_90 : 
		case TECLA_91 : 
		case TECLA_92 :  
		case TECLA_93 :  
		case TECLA_94 : 
		case TECLA_95 : 
		case TECLA_96 :		
			return 9;
			break;
		case TECLA_73 : 
		case TECLA_74 :  
		case TECLA_75 :  
		case TECLA_76 :  
		case TECLA_77 :  
		case TECLA_78 : 
		case TECLA_79 :  
		case TECLA_80 :			
			return 10;
			break;
		case TECLA_81 :  
		case TECLA_82 : 
		case TECLA_83 :  
		case TECLA_84 :  
		case TECLA_85 :  
		case TECLA_86 :  
		case TECLA_87 :  
		case TECLA_88 :		
			return 11;
			break;
		case TECLA_97 : 
		case TECLA_98 :
		case TECLA_99 : 
		case TECLA_100 : 
		case TECLA_101 :  
		case TECLA_102 :  
		case TECLA_103 :  
		case TECLA_104 :			
			return 12;
			break;
		case TECLA_105 :  
		case TECLA_106 :  
		case TECLA_107 :  
		case TECLA_108 :  
		case TECLA_109 :  
		case TECLA_110 :  
		case TECLA_111 :  
		case TECLA_112 :			
			return 13;
			break;
		case TECLA_129 : 
		case TECLA_130 : 
		case TECLA_131 : 
		case TECLA_132 : 
		case TECLA_133 : 
		case TECLA_134 : 
		case TECLA_135 :  
		case TECLA_136 :			
			return 14;
			break;
		case TECLA_113 : 
		case TECLA_114 :  
		case TECLA_115 : 
		case TECLA_116 : 
		case TECLA_117 :  
		case TECLA_118 : 
		case TECLA_119 :
		case TECLA_120 :		
			return 15;
			break;
		case TECLA_121 : 
		case TECLA_122 :  
		case TECLA_123 :  
		case TECLA_124 : 
		case TECLA_125 : 
		case TECLA_126 : 
		case TECLA_127 : 
		case TECLA_128 :			
			return 16;
			break;

		case TECLA_137 :
		case TECLA_138 : 
		case TECLA_139 :  
		case TECLA_140 : 
		case TECLA_141 : 
		case TECLA_142 :  
		case TECLA_143 :  
		case TECLA_144 :		
			return 17;
			break;
		case TECLA_145 :
		case TECLA_146 :  
		case TECLA_147 :  
		case TECLA_148 :  
		case TECLA_149 : 
		case TECLA_150 : 
		case TECLA_151 :
		case TECLA_152 :		
			return 18;
			break;
		case TECLA_169 : 
		case TECLA_170 : 
		case TECLA_171 :  
		case TECLA_172 :  
		case TECLA_173 : 
		case TECLA_174 : 
		case TECLA_175 : 
		case TECLA_176 :		
			return 19;
			break;
		case TECLA_153 : 
		case TECLA_154 :  
		case TECLA_155 :  
		case TECLA_156 : 
		case TECLA_157 :  
		case TECLA_158 :  
		case TECLA_159 :  
		case TECLA_160 :			
			return 20;
			break;
		case TECLA_161 :
		case TECLA_162 : 
		case TECLA_163 : 
		case TECLA_164 :  
		case TECLA_165 : 
		case TECLA_166 : 
		case TECLA_167 : 
		case TECLA_168 :			
			return 21;		
			break;		
		
		default:
			return 0;
			break;
	} 	
}

unsigned char GetByteMask_OfKey(unsigned char KeyIndex)
{
	switch (KeyIndex)
	{
		case TECLA_17:
		case TECLA_25:
		case TECLA_49:
		case TECLA_33:
		case TECLA_41:
		case TECLA_1:
		case TECLA_9:
		case TECLA_57:
		case TECLA_65:
		case TECLA_89:
		case TECLA_73:
		case TECLA_81:
		case TECLA_97:
		case TECLA_105:
		case TECLA_129:
		case TECLA_113:
		case TECLA_121:
		case TECLA_137:
		case TECLA_145:
		case TECLA_169:
		case TECLA_153:
		case TECLA_161:
			return 0xFE;
			break;
		case TECLA_2:
		case TECLA_10:
		case TECLA_18:
		case TECLA_26:
		case TECLA_34:
		case TECLA_42:	
		case TECLA_50:
		case TECLA_58:
		case TECLA_66:
		case TECLA_90:
		case TECLA_74:
		case TECLA_82:
		case TECLA_98:
		case TECLA_106:
		case TECLA_130:
		case TECLA_114:
		case TECLA_122:
		case TECLA_138:
		case TECLA_146:
		case TECLA_170:
		case TECLA_154:
		case TECLA_162:
			return 0xFD;
			break;
		case TECLA_3:
		case TECLA_19:
		case TECLA_27:
		case TECLA_35:
		case TECLA_43:
		case TECLA_51:
		case TECLA_11:
		case TECLA_59:
		case TECLA_67:
		case TECLA_91:
		case TECLA_75:
		case TECLA_83:
		case TECLA_99:
		case TECLA_107:
		case TECLA_131:
		case TECLA_115:
		case TECLA_123:
		case TECLA_139:
		case TECLA_147:
		case TECLA_171:
		case TECLA_155:
		case TECLA_163:
			return 0xFB;
			break;
		case TECLA_4:
		case TECLA_20:
		case TECLA_28:
		case TECLA_36:
		case TECLA_44:
		case TECLA_52:			
		case TECLA_12:
		case TECLA_60:
		case TECLA_68:
		case TECLA_92:
		case TECLA_76:
		case TECLA_84:
		case TECLA_100:
		case TECLA_108:
		case TECLA_132:
		case TECLA_116:
		case TECLA_124:
		case TECLA_140:
		case TECLA_148:
		case TECLA_172:
		case TECLA_156:
		case TECLA_164:
			return 0xF7;
			break;
		case TECLA_5:
		case TECLA_21:
		case TECLA_29:
		case TECLA_37:
		case TECLA_45:
		case TECLA_53:			
		case TECLA_13:
		case TECLA_61:
		case TECLA_69:
		case TECLA_93:
		case TECLA_77:
		case TECLA_85:
		case TECLA_101:
		case TECLA_109:
		case TECLA_133:
		case TECLA_117:
		case TECLA_125:
		case TECLA_141:
		case TECLA_149:
		case TECLA_173:
		case TECLA_157:
		case TECLA_165:
			return 0xEF;
			break;
		case TECLA_6:
		case TECLA_22:
		case TECLA_30:
		case TECLA_54:
		case TECLA_38:
		case TECLA_46:
		case TECLA_14:
		case TECLA_62:
		case TECLA_70:
		case TECLA_94:
		case TECLA_78:
		case TECLA_86:
		case TECLA_102:
		case TECLA_110:
		case TECLA_134:
		case TECLA_118:
		case TECLA_126:
		case TECLA_142:
		case TECLA_150:
		case TECLA_174:
		case TECLA_158:
		case TECLA_166:
			return 0xDF;
			break;
		case TECLA_7:
		case TECLA_23:
		case TECLA_31:
		case TECLA_39:
		case TECLA_47:
		case TECLA_55:			
		case TECLA_15:
		case TECLA_63:
		case TECLA_71:
		case TECLA_95:
		case TECLA_79:
		case TECLA_87:
		case TECLA_103:
		case TECLA_111:
		case TECLA_135:
		case TECLA_119:
		case TECLA_127:
		case TECLA_143:
		case TECLA_151:
		case TECLA_175:
		case TECLA_159:
		case TECLA_167:
			return 0xBF;
			break;
		case TECLA_8:
		case TECLA_24:
		case TECLA_32:
		case TECLA_40:
		case TECLA_48:
		case TECLA_56:
		case TECLA_16:
		case TECLA_64:
		case TECLA_72:
		case TECLA_96:
		case TECLA_80:
		case TECLA_88:
		case TECLA_104:
		case TECLA_112:
		case TECLA_136:
		case TECLA_120:
		case TECLA_128:
		case TECLA_144:
		case TECLA_152:
		case TECLA_176:
		case TECLA_160:
		case TECLA_168:
			return 0x7F;
			break;


		
		default:
			return 0;
			break;
	} 
}

unsigned char GetI2CAddress_OfKey(unsigned char KeyIndex)
{
	if ((KeyIndex >= TECLA_1) && (KeyIndex <= TECLA_16))
		return ENDERECO_PCA8575D_MM1300;
	else if((KeyIndex >= TECLA_17) && (KeyIndex <= TECLA_47))
		return ENDERECO_PCA_2_MM1300;
	else if((KeyIndex >= TECLA_48) && (KeyIndex <= TECLA_56))
		return ENDERECO_PCA_2_MM1300;
	else if((KeyIndex >= TECLA_57) && (KeyIndex <= TECLA_96))
		return ENDERECO_PCA_3_MM1200_A;
	else if((KeyIndex >= TECLA_97) && (KeyIndex <= TECLA_136))
		return ENDERECO_PCA_3_MM1200_B;
	else if((KeyIndex >= TECLA_137) && (KeyIndex <= TECLA_138))
		return ENDERECO_PCA_3_MM1200_C;
	else if((KeyIndex >= TECLA_139) && (KeyIndex <= TECLA_167))
		return ENDERECO_PCA_3_MM1200_C;
	else if((KeyIndex >= TECLA_168) && (KeyIndex <= TECLA_176))
		return ENDERECO_PCA_3_MM1200_C;
	else return 0;
}

void ManageKeyLeds(unsigned char comando, unsigned char KeyIndex)
{
	unsigned char PortIndex, PosPort;

	if (comando == COMANDO_KEYLED_ON)
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
	}

	else if (comando == COMANDO_KEYLED_OFF)
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
	//unsigned int i;
	
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
	/*
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
	*/
	
	//Apaga todos os Leds
	//ApagaTodasAsTeclas();
	
	//ConfereERotacionaLedsDeTecladosExpansores(); 
	

	
	//Restabelece situação dos Leds
	
	for (cntBank = 0; cntBank < 5; cntBank++)
	{
		escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[cntBank], temp_io_register[cntBank]);
	}

	escreveRegistro(ENDERECO_PCA8575D_MM1300, temp_io_register[5], temp_io_register[6]);
	
	//restaura situacao dos leds  para teclados expansores
	/*
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
	*/
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
	printf("AcendeTodasAsTeclas chamada!!\n");
}

void ApagaTodasAsTeclas(void)
{
	AplicaValorFixoEmTodosOsPCAS(0xFF);
	printf("ApagaTodasAsTeclas chamada!!\n");
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
	
	unsigned char bufferLeituraPCAParaMenuDoTeste;
	unsigned char bufferLeituraPCAParaDefinirModoDeTesteApertoTecla[(7+5+5+5)];
	unsigned char cntBank;
	unsigned char TempPort0;
	unsigned char ExitKeyPressedWhenLow;
	
	TempPort0 = 0xFF;
	ExitKeyPressedWhenLow = 1;
	

	//ESP Loop responsável pelo menu de escolha de testes no modo de testes, le o banco {x} de input do PCA9506
	while (ExitKeyPressedWhenLow)
	{		
		//Configura o banco 2 como entrada
		escreveRegistro(ENDERECO_PCA_2_MM1300, io_configuration_register_banks[2], 0xFF); //Configura como entrada		
		//Efetua a leitura do banco
		leRegistroUnico(ENDERECO_PCA_2_MM1300, (input_port_register_bank[2]),&bufferLeituraPCAParaMenuDoTeste);
		printf("Debug leu o banco %u\n", bufferLeituraPCAParaMenuDoTeste);
		
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
		else if (bufferLeituraPCAParaMenuDoTeste == 0xBF){
			ApagaTodasAsTeclas();

			TempPort0 = 0xFF;

			while (ExitKeyPressedWhenLow) {					
						if (bufferLeituraPCAParaMenuDoTeste == 0xFC) // apertando teclas de saida do modo teste de tecla
						{
							printf("Saindo do modo teste de teclas\n");
							bufferLeituraPCAParaMenuDoTeste = 0xFF;
							RunKeyLedsOneTime();
							ExitKeyPressedWhenLow = 0;
						} 
				}
			}
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
			
		  	// verifica se houve alteração
			if ((((bufferLeituraPCA1_imediatamenteAposPolling[cntBank])&varBitSelect) != ((bufferLeituraPCA1_seminterrupcao[cntBank])&varBitSelect)))
			{
				if (((bufferLeituraPCA1_imediatamenteAposPolling[cntBank])& varBitSelect) == 0x00)
				{
					udp_send_buttonDown(ArrayIndicaTecla[cntBank][cntTmp]);
					printf("TECLA PRESSIONADA: %u\n", ArrayIndicaTecla[cntBank][cntTmp]);
					ManageKeyLeds(COMANDO_KEYLED_ON, ArrayIndicaTecla[cntBank][cntTmp]);
					bufferLeituraPCA1_seminterrupcao[cntBank] &= ~varBitSelect;
				}
				else 
				{
					udp_send_buttonUp(ArrayIndicaTecla[cntBank][cntTmp]);
					printf("TECLA SOLTA: %u\n", ArrayIndicaTecla[cntBank][cntTmp]);
					bufferLeituraPCA1_seminterrupcao[cntBank] |= varBitSelect;
				}
			}
		}
	}
	
}