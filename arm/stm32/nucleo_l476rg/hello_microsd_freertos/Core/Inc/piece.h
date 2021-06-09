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

	G_Ds5 = 0b00011110,
	G_Eb5 = 0b00011111,

	G_E5 	= 0b00100000,

	G_F5 	= 0b00100001,

	G_Fs5 = 0b00100010,
	G_Gb5 = 0b00100011,

	G_G5 	= 0b00100100,

	G_Gs5 = 0b00100101,
	G_Ab5 = 0b00100110,

	G_A5 	= 0b00100111,

	G_As5 = 0b00101000,
	G_Bb5 = 0b00101001,

	G_B5 	= 0b00101010,

	G_C6 	= 0b00101011,

	// D String


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
