/*
 * piece.c
 *
 *  Created on: Jun 8, 2021
 *      Author: victor
 */

#include "piece.h"


void Piece_vInit(PieceHandle_t *pxPiece, FIL *pFil)
{
	uint32_t ulNumBytesRead;
	f_read(pFil, pxPiece->xPieceInformation.pusName, sizeof(pxPiece->xPieceInformation.pusName), &ulNumBytesRead);
	printf("Name: %s\r\n", pxPiece->xPieceInformation.pusName);
}


void Piece_vSetCompositionByteSize(PieceHandle_t *pxPiece, FIL *pFil)
{
	uint32_t ulNumBytesRead;
	f_read(pFil, &(pxPiece->xComposition.ulCompositionByteSize), sizeof(pxPiece->xComposition.ulCompositionByteSize), &ulNumBytesRead);
	printf("Composition size: %u\r\n", pxPiece->xComposition.ulCompositionByteSize);
}


void Piece_vSetComposition(PieceHandle_t *pxPiece, FIL *pFil)
{
	Piece_vSetCompositionByteSize(pxPiece, pFil);

	uint32_t ulNumBytesRead;
	f_read(pFil, pxPiece->xComposition.pusComposition,
			pxPiece->xComposition.ulCompositionByteSize < sizeof(pxPiece->xComposition.pusComposition) ? pxPiece->xComposition.ulCompositionByteSize : sizeof(pxPiece->xComposition.pusComposition),
			&ulNumBytesRead);

	pxPiece->xPieceInstruction.ulInstructionCounter = 0;
}

void Piece_vParseCommand(PieceHandle_t *pxPiece, FIL *pFil)
{
	memcpy(&pxPiece->xPieceInstruction.usCommand, pxPiece->xComposition.pusComposition + pxPiece->xPieceInstruction.ulInstructionCounter, sizeof(pxPiece->xPieceInstruction.usCommand));
	//f_read(pFil, &(pxPiece->xPieceInstruction.usCommand), sizeof(pxPiece->xPieceInstruction.usCommand), &pxPiece->xPieceInstruction.usNumBytesRead);
	//printf("Command: %c\r\n", pxPiece->xComposition.pusComposition + pxPiece->xPieceInstruction.ulInstructionCounter);
	printf("Command: %u\r\n", pxPiece->xPieceInstruction.usCommand);

	pxPiece->xPieceInstruction.ulInstructionCounter++;
}



void Piece_vConfigure(PieceHandle_t *pxPiece, FIL *pFil)
{
	printf("Initializing piece.\r\n");

}




void Piece_Debug_vPrintPointer(PieceHandle_t *pxPiece, FIL *pFil)
{
	printf("Pointer: %u\r\n", f_tell(pFil));
}
