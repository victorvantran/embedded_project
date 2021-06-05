/*
 * bme280.h
 *
 *  Created on: Jun 4, 2021
 *      Author: victor
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_


/* APPLICATION */



/* IMPLEMENTATION */
#include <stdint.h>
#include <string.h>
#include "stm32l4xx_hal.h"

#define BME280_I2C_SLAVE_ADDRESS_A 0x76
#define BME280_I2C_SLAVE_ADDRESS_B 0x77

#define BME280_CHIP_ADDRESS 0xD0
#define BME280_MEASURE_DATA_ADDRESS 0xF7

#define BME280_CTRL_HUM 0xF2
#define BME280_CTRL_MEAS 0xF4

#define BME280_CALIBRATE_ADDRESS_A 0x88
#define BME280_CALIBRATE_ADDRESS_B 0xE1




#define BME280_SLEEP_MODE 0x00
#define BME280_FORCED_MODE 0x01
#define BME280_NORMAL_MODE 0x11




typedef struct
{
	struct
	{
		uint16_t usT1;
		int16_t sT2;
		int16_t sT3;
	} xDigT;

	struct
	{
		uint16_t usP1;
		int16_t sP2;
		int16_t sP3;
		int16_t sP4;
		int16_t sP5;
		int16_t sP6;
		int16_t sP7;
		int16_t sP8;
		int16_t sP9;
	} xDigP;

	struct
	{
		uint8_t ucH1;
		int16_t usH2;
		uint8_t ucH3;
		int16_t sH4;
		int16_t sH5;
		int8_t cH6;
	} xDigH;
} BME280CalibrationData_t;


/* No need for BME280CalibrationARegData due to being able to read directly into " */

/* Need to read into calibration b data due to having to re-align cast to "12-bits" */
typedef struct
{
	uint8_t uc0xE1;
	uint8_t uc0xE2;
	uint8_t uc0xE3;
	uint8_t uc0xE4;
	uint8_t uc0xE5;
	uint8_t uc0xE6;
	uint8_t uc0xE7;
} BME280CalibrationBRegData;


typedef struct
{
	struct
	{
		uint8_t uMSB;
		uint8_t uLSB;
		uint8_t uXLSB;
	} xPressureRegData;

	struct
	{
		uint8_t uMSB;
		uint8_t uLSB;
		uint8_t uXLSB;
	} xTemperatureRegData;

	struct
	{
		uint8_t uMSB;
		uint8_t uLSB;
	} xHumidityRegData;
} BME280MeasureRegData_t;




typedef struct
{
	uint32_t ulPressureRawData;
	uint32_t ulTemperatureRawData;
	uint16_t uHumidityRawData;
} BME280MeasureRawData_t;




typedef struct
{
	I2C_HandleTypeDef *pxI2CHandle;
	uint8_t uI2CSlaveAddress;

	BME280CalibrationData_t xCalibrationData;

	BME280MeasureRegData_t xMeasureRegData;
	BME280MeasureRawData_t xMeasureRawData;

} BME280Handle_t;




void BME280_vInit(BME280Handle_t *pxBME280,
		I2C_HandleTypeDef *pxI2CHandle,
		uint8_t uI2CSlaveAddress );


void BME280_vReadCalibrationData(BME280Handle_t *pxBME280);

/* */
uint8_t BME280_vReadChipID(BME280Handle_t *pxBME280);


/* */
void BME280_vMeasureAllForced(BME280Handle_t *pxBME280);


/* */
void BME280_vMeasureForced(BME280Handle_t *pxBME280,
		uint8_t uPressureOversample,
		uint8_t uTemperatureOversample,
		uint8_t uHumidityOversample);


/* Read the raw register data [pressure, temperature, humidity] and cast 20-bit, 20-bit, 16-bit to the respective raw data */
void BME280_vReadRawData(BME280Handle_t *pxBME280);

/* DEBUG */
void BME280_vPrintRawData(BME280Handle_t *pxBME280);


#endif /* INC_BME280_H_ */
