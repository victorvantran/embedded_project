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


void Piece_vParseCommand(PieceHandle_t *pxPiece)
{
	memcpy(&pxPiece->xPieceInstruction.usCommand, pxPiece->xComposition.pusComposition + pxPiece->xPieceInstruction.ulInstructionCounter, sizeof(pxPiece->xPieceInstruction.usCommand));
	pxPiece->xPieceInstruction.ulInstructionCounter += 1;

	printf("Command: %u\r\n", pxPiece->xPieceInstruction.usCommand);

	switch (pxPiece->xPieceInstruction.usCommand)
	{
	case 0b00000000:
		printf("END OF COMPOSITION\r\n");
		return;
	case 0b01111111:
		Piece_vConfigureAll(pxPiece);
		break;
	default:
		if (bIsPlayCommand(pxPiece->xPieceInstruction.usCommand))
		{
			uint8_t usNumNotes = (uint8_t)((0x03) & (pxPiece->xPieceInstruction.usCommand) + 1);
			Piece_vPlayNotes(pxPiece, usNumNotes);
		}
		else
		{
			printf("UNKNOWN COMMAND\r\n");
		}
		break;
	}

	Piece_vParseCommand(pxPiece);
}







void Piece_vSetMovement(PieceHandle_t *pxPiece, uint8_t usMovement)
{
	pxPiece->xPieceConfiguration.usMovement = usMovement;
	printf("Movement: %u\r\n", pxPiece->xPieceConfiguration.usMovement);
}

void Piece_vSetBPM(PieceHandle_t *pxPiece, uint16_t uBPM)
{
	pxPiece->xPieceConfiguration.uBPM = uBPM;
	printf("BPM: %u\r\n", pxPiece->xPieceConfiguration.uBPM);
}

void Piece_vSetDynamic(PieceHandle_t *pxPiece, uint8_t usDynamic)
{
	pxPiece->xPieceConfiguration.usDynamic = usDynamic;
	printf("Dynamic: %u\r\n", pxPiece->xPieceConfiguration.usDynamic);
}

void Piece_vConfigureAll(PieceHandle_t *pxPiece)
{
	printf("Configuring All piece...\r\n");

	uint8_t usMovement;
	memcpy(&usMovement, pxPiece->xComposition.pusComposition + pxPiece->xPieceInstruction.ulInstructionCounter, sizeof(usMovement));
	pxPiece->xPieceInstruction.ulInstructionCounter += sizeof(usMovement);

	uint16_t uBPM;
	memcpy(&uBPM, pxPiece->xComposition.pusComposition + pxPiece->xPieceInstruction.ulInstructionCounter, sizeof(uBPM));
	pxPiece->xPieceInstruction.ulInstructionCounter += sizeof(uBPM);

	uint8_t usDynamic;
	memcpy(&usDynamic, pxPiece->xComposition.pusComposition + pxPiece->xPieceInstruction.ulInstructionCounter, sizeof(usDynamic));
	pxPiece->xPieceInstruction.ulInstructionCounter += sizeof(usDynamic);


	Piece_vSetMovement(pxPiece, usMovement);
	Piece_vSetBPM(pxPiece, uBPM);
	Piece_vSetDynamic(pxPiece, usDynamic);
}


void Piece_vPlayNote(PieceHandle_t *pxPiece, uint8_t *pusMaxBeatValue)
{
	memcpy(&pxPiece->xPieceInstruction.uPlay, pxPiece->xComposition.pusComposition + pxPiece->xPieceInstruction.ulInstructionCounter, sizeof(pxPiece->xPieceInstruction.uPlay));
	pxPiece->xPieceInstruction.ulInstructionCounter += sizeof(pxPiece->xPieceInstruction.uPlay);

	uint8_t usTechnique = (uint8_t)(((0xF000) & (pxPiece->xPieceInstruction.uPlay)) >> 12);
	uint8_t usBeatValue = (uint8_t)(((0x0F00) & (pxPiece->xPieceInstruction.uPlay)) >> 8);
	uint8_t usNoteBase = (uint8_t)(((0x00C0) & (pxPiece->xPieceInstruction.uPlay)) >> 6); // String
	uint8_t usNoteOffset = (uint8_t)((0x003F) & (pxPiece->xPieceInstruction.uPlay));

	printf("Play note:\r\n");
	printf("	Technique: %u\r\n", usTechnique);
	printf("	Beat Value: %u\r\n", usBeatValue);
	printf("	Note Base: %u\r\n", usNoteBase);
	printf("	Note Offset: %u\r\n", usNoteOffset);

	if (usBeatValue < *pusMaxBeatValue)
	{
		*pusMaxBeatValue = usBeatValue;
	}
}


void Piece_vPlayNotes(PieceHandle_t *pxPiece, uint8_t usNumNotes)
{
	printf("Play Notes: %u\r\n", usNumNotes);

	uint8_t usMaxBeatValue = 0xFF; // lower value means longer beat
	for (int16_t i = 0; i < usNumNotes; i++)
	{
		Piece_vPlayNote(pxPiece, &usMaxBeatValue);
	}

	// osDelay
}




uint8_t bIsPlayCommand(uint8_t usCommand)
{
	return usCommand >= 0b10000000;
}



void Piece_Debug_vPrintPointer(PieceHandle_t *pxPiece, FIL *pFil)
{
	printf("Pointer: %u\r\n", f_tell(pFil));
}
