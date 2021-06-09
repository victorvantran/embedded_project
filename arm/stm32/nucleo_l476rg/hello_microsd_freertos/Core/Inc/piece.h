/*
 * piece.h
 *
 *  Created on: Jun 8, 2021
 *      Author: victor
 */

#ifndef INC_PIECE_H_
#define INC_PIECE_H_


#include "fatfs.h"
#include <stdint.h>


#define MAX_PIECE_NAME_BYTE_SIZE 128
#define MAX_COMPOSITION_BYTE_SIZE 32768



// [xx|xxxxxx] -> [string|note]
typedef enum
{
	Rest 	=	0b00000000,

	// G String
	G_G3 	= 0b00000001,

	G_Gs3 = 0b00000010,
	G_Ab3 = 0b00000011,

	G_A3 	= 0b00000100,

	G_As3 = 0b00000101,
	G_Bb3 = 0b00000110,

	G_B3 	= 0b00000111,

	G_C4 	= 0b00001000,

	G_Cs4 = 0b00001001,
	G_Db4 = 0b00001010,

	G_D4 	= 0b00001011,

	G_Ds4 = 0b00001100,
	G_Eb4 = 0b00001101,

	G_E4 	= 0b00001110,

	G_F4 	= 0b00001111,

	G_Fs4 = 0b00010000,
	G_Gb4 = 0b00010001,

	G_G4 	= 0b00010010,

	G_Gs4 = 0b00010011,
	G_Ab4 = 0b00010100,

	G_A4 	= 0b00010101,

	G_As4 = 0b00010110,
	G_Bb4 = 0b00010111,

	G_B4 	= 0b00011000,

	G_C5 	= 0b00011001,

	G_Cs5 = 0b00011010,
	G_Db5 = 0b00011011,

	G_D5 	= 0b00011100,




	G_Ds5 = 0b00011101,
	G_Eb5 = 0b00011110,

	G_E5 	= 0b00011111,

	G_F5 	= 0b00100000,

	G_Fs5 = 0b00100001,
	G_Gb5 = 0b00100010,

	G_G5 	= 0b00100011,

	G_Gs5 = 0b00100100,
	G_Ab5 = 0b00100101,

	G_A5 	= 0b00100110,

	G_As5 = 0b00100111,
	G_Bb5 = 0b00101000,

	G_B5 	= 0b00101001,

	G_C6 	= 0b00101010,



	// D String
	D_D4 	= 0b01000001,

	D_Ds4 = 0b01000010,
	D_Eb4 = 0b01000011,

	D_E4 	= 0b01000100,

	D_F4 	= 0b01000101,

	D_Fs4 = 0b01000110,
	D_Gb4 = 0b01000111,

	D_G4 	= 0b01001000,

	D_Gs4 = 0b01001001,
	D_Ab4 = 0b01001010,

	D_A4 	= 0b01001011,

	D_As4 = 0b01001100,
	D_Bb4 = 0b01001101,

	D_B4 	= 0b01001110,

	D_C5 	= 0b01001111,

	D_Cs5 = 0b01010000,
	D_Db5 = 0b01010001,

	D_D5 	= 0b01010010,

	D_Ds5 = 0b01010011,
	D_Eb5 = 0b01010100,

	D_E5 	= 0b01010101,

	D_F5 	= 0b01010110,

	D_Fs5 = 0b01010111,
	D_Gb5 = 0b01011000,

	D_G5 	= 0b01011001,

	D_Gs5 = 0b01011010,
	D_Ab5 = 0b01011011,

	D_A5 	= 0b01011100,

	D_As5 = 0b01011101,
	D_Bb5 = 0b01011110,

	D_B5 	= 0b01011111,

	D_C6 	= 0b01100000,

	D_Cs6 = 0b01100001,
	D_Db6 = 0b01100010,

	D_D6 	= 0b01100011,

	D_Ds6 = 0b01100100,
	D_Eb6 = 0b01100101,

	D_E6 	= 0b01100110,

	D_F6 	= 0b01100111,

	D_Fs6 = 0b01101000,
	D_Gb6 = 0b01101001,

	D_G6 	= 0b01101010,


	// A String
	A_A4 	= 0b10000001,

	A_As4 = 0b10000010,
	A_Bb4 = 0b10000011,

	A_B4 	= 0b10000100,

	A_C5 	= 0b10000101,

	A_Cs5 = 0b10000110,
	A_Db5 = 0b10000111,

	A_D5 	= 0b10001000,

	A_Ds5 = 0b10001001,
	A_Eb5 = 0b10001010,

	A_E5 	= 0b10001011,

	A_F5 	= 0b10001100,

	A_Fs5 = 0b10001101,
	A_Gb5 = 0b10001110,

	A_G5 	= 0b10001111,

	A_Gs5 = 0b10010000,
	A_Ab5 = 0b10010001,

	A_A5 	= 0b10010010,

	A_As5 = 0b10010011,
	A_Bb5 = 0b10010100,

	A_B5 	= 0b10010101,

	A_C6 	= 0b10010110,

	A_Cs6 = 0b10010111,
	A_Db6 = 0b10011000,

	A_D6 	= 0b10011001,

	A_Ds6 = 0b10011010,
	A_Eb6 = 0b10011011,

	A_E6 	= 0b10011100,

	A_F6 	= 0b10011101,

	A_Fs6 = 0b10011110,
	A_Gb6 = 0b10011111,

	A_G6 	= 0b10100000,

	A_Gs6 = 0b10100001,
	A_Af6 = 0b10100010,

	A_A6 	= 0b10100011,

	A_As6 = 0b10100100,
	A_Bf6 = 0b10100101,

	A_B6 	= 0b10100110,

	A_C7 	= 0b10100111,

	A_Cs7 = 0b10101000,
	A_Db7 = 0b10101001,

	A_D7 	= 0b10101010,


	// E String
	E_E5 	= 0b11000001,

	E_F5 	= 0b11000010,

	E_Fs5 = 0b11000011,
	E_Gb5 = 0b11000100,

	E_G5 	= 0b11000101,

	E_Gs5 = 0b11000110,
	E_Ab5 = 0b11000111,

	E_A5 	= 0b11001000,

	E_As5 = 0b11001001,
	E_Bb5 = 0b11001010,

	E_B5 	= 0b11001011,

	E_C6 	= 0b11001100,

	E_Cs6 = 0b11001101,
	E_Db6 = 0b11001110,

	E_D6 	= 0b11001111,

	E_Ds6 = 0b11010000,
	E_Eb6 = 0b11010001,

	E_E6 	= 0b11010010,

	E_F6 	= 0b11010011,

	E_Fs6 = 0b11010100,
	E_Gb6 = 0b11010101,

	E_G6 	= 0b11010110,

	E_Gs6 = 0b11010111,
	E_Af6 = 0b11011000,

	E_A6 	= 0b11011001,

	E_As6 = 0b11011010,
	E_Bf6 = 0b11011011,

	E_B6 	= 0b11011100,

	E_C7 	= 0b11011101,

	E_Cs7 = 0b11011110,
	E_Db7 = 0b11011111,

	E_D7 	= 0b11100000,

	E_Ds7 = 0b11100001,
	E_Eb7 = 0b11100010,

	E_E7 	= 0b11100011,

	E_F7 	= 0b11100100,

	E_Fs7 = 0b11100101,
	E_Gb7 = 0b11100110,

	E_G7 	= 0b11100111,

	E_Gs7 = 0b11101000,
	E_Ab7 = 0b11101001,

	E_A7 	= 0b11101010,

} StringNote_t;


typedef struct
{
	FATFS fs;
	FATFS *pfs;
	FIL fil;
	FRESULT fres;
	DWORD fre_clust;
	uint32_t totalSpace, freeSpace;
} PieceFATFSHandle_t;


typedef struct
{
	uint32_t ulInstructionCounter;
	uint8_t usCommand;
	uint16_t uNote;
} PieceInstructionHandle_t;


typedef struct
{
	uint8_t pusName[MAX_PIECE_NAME_BYTE_SIZE];
} PieceInformationHandle_t;



typedef struct
{
	uint8_t usMovement;
	uint16_t uBPM;
} PieceConfigurationHandle_t;



typedef struct
{
	uint32_t ulCompositionByteSize;
	uint8_t pusComposition[MAX_COMPOSITION_BYTE_SIZE];
} PieceCompositionHandle_t;



typedef struct
{
	// Information
	PieceInstructionHandle_t xPieceInstruction;
	PieceInformationHandle_t xPieceInformation;
	PieceConfigurationHandle_t xPieceConfiguration;
	PieceCompositionHandle_t xComposition;
} PieceHandle_t;



void Piece_vInit(PieceHandle_t *pxPiece, FIL *pFil);
void Piece_vSetCompositionByteSize(PieceHandle_t *pxPiece, FIL *pFil);
void Piece_vSetComposition(PieceHandle_t *pxPiece, FIL *pFil);
void Piece_vParseCommand(PieceHandle_t *pxPiece, FIL *pFil);
void Piece_vConfigure(PieceHandle_t *pxPiece, FIL *pFil);


bool bIsPlayCommand(uint8_t usCommand);


void Piece_Debug_vPrintPointer(PieceHandle_t *pxPiece, FIL *pFil);


#endif /* INC_PIECE_H_ */
