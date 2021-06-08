/*
 * piece.h
 *
 *  Created on: Jun 8, 2021
 *      Author: victor
 */

#ifndef INC_PIECE_H_
#define INC_PIECE_H_


#include <stdint.h>

#define MAX_PIECE_NAME_BYTE_SIZE 128
#define MAX_COMPOSITION_BYTE_SIZE 32768




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
	uint32_t ulCompositionSize;
	uint8_t pusComposition[MAX_COMPOSITION_BYTE_SIZE];
} CompositionHandle_t;



typedef struct
{
	// Information
	PieceInformationHandle_t xPieceInformation;
	PieceConfigurationHandle_t xPieceConfiguration;
	CompositionHandle_t xComposition;
} PieceHandle_t;



#endif /* INC_PIECE_H_ */
