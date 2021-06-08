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


#define BME280_CHIP_ADDRESS 0xD0

#define BME280_MEASURE_DATA_ADDRESS 0xF7
#define BME280_MEASURE_DATA_BYTE_SIZE 8

#define BME280_CTRL_HUM_ADDRESS 0xF2
#define BME280_CTRL_MEAS_ADDRESS 0xF4

#define BME280_CALIBRATE_ADDRESS_A 0x88
#define BME280_CALIBRATE_BYTE_SIZE_A 25

#define BME280_CALIBRATE_ADDRESS_B 0xE1
#define BME280_CALIBRATE_BYTE_SIZE_B 8

#define BME280_SLEEP_MODE 0x00
#define BME280_FORCED_MODE 0x01
#define BME280_NORMAL_MODE 0x11


#define BME280_I2C_SLAVE_ADDRESS_A 0x76
#define BME280_I2C_SLAVE_ADDRESS_B 0x77

#define BME280_SPI_FCLK_KHZ 1000
/* SPI MODE 0 and 3 works for BME, but we are going with MODE 0 because it is common */
#define BME280_SPI_CPOL SPI_POLARITY_LOW
#define BME280_SPI_CPHA SPI_PHASE_1EDGE
#define BME280_SPI_WRITE (0 << 7)
#define BME280_SPI_READ (1 << 7)




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

	int32_t lTemperatureFine;
} BME280CalibrationData_t;


/* No need for BME280CalibrationARegData due to being able to read directly into. "
 * However, need to read into BME280CalibrationBRegData data due to having to re-align cast to "12-bits" */
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

	SPI_HandleTypeDef *pxSPIHandle;
	GPIO_TypeDef *pxSPICSGPIO;
	uint16_t uSPICSGPIOPIN;

	BME280CalibrationData_t xCalibrationData;

	BME280MeasureRegData_t xMeasureRegData;
	BME280MeasureRawData_t xMeasureRawData;

} BME280Handle_t;




/* SPI */
void BME280_SPI_vInit(BME280Handle_t *pxBME280,
		SPI_HandleTypeDef *pxSPIHandle,
		GPIO_TypeDef *pxSPICSGPIO, uint16_t uSPICSGPIOPIN);

/* Sets the proper SPI mode and clock frequency to the standards of the BME280 peripheral */
HAL_StatusTypeDef BME280_SPI_vSetMode(BME280Handle_t *pxBME280);

/* Read the BME280 Chip ID which should be 0x60 */
void BME280_SPI_vReadChipID(BME280Handle_t *pxBME280);

/* Read the calibration data into the BME280 Handle to be used for compensation formulas */
void BME280_SPI_vReadCalibrationData(BME280Handle_t *pxBME280);

/* Writes to the the BME280 to measure pressure, temperature, and/or humidity based on parameters in Force state before returning to sleep state */
void BME280_SPI_vMeasureForced(BME280Handle_t *pxBME280,
		uint8_t uPressureOversample,
		uint8_t uTemperatureOversample,
		uint8_t uHumidityOversample);

/* Writes to the the BME280 to measure pressure, temperature, and humidity in Force state before returning to sleep state */
void BME280_SPI_vMeasureAllForced(BME280Handle_t *pxBME280);

/* Read the raw register data [pressure, temperature, humidity] and cast 20-bit, 20-bit, 16-bit to the respective raw data */
void BME280_SPI_vReadRawData(BME280Handle_t *pxBME280);




/* I2C */
void BME280_I2C_vInit(BME280Handle_t *pxBME280,
		I2C_HandleTypeDef *pxI2CHandle,
		uint8_t uI2CSlaveAddress );

/* Read the BME280 Chip ID which should be 0x60 */
uint8_t BME280_I2C_vReadChipID(BME280Handle_t *pxBME280);

/* Read the calibration data into the BME280 Handle to be used for compensation formulas */
void BME280_I2C_vReadCalibrationData(BME280Handle_t *pxBME280);

/* Writes to the the BME280 to measure pressure, temperature, and/or humidity based on parameters in Force state before returning to sleep state */
void BME280_I2C_vMeasureForced(BME280Handle_t *pxBME280,
		uint8_t uPressureOversample,
		uint8_t uTemperatureOversample,
		uint8_t uHumidityOversample);

/* Writes to the the BME280 to measure pressure, temperature, and humidity in Force state before returning to sleep state */
void BME280_I2C_vMeasureAllForced(BME280Handle_t *pxBME280);


/* Read the raw register data [pressure, temperature, humidity] and cast 20-bit, 20-bit, 16-bit to the respective raw data */
void BME280_I2C_vReadRawData(BME280Handle_t *pxBME280);




/* Compensation Formulas */
/* Compensation formals are based on Bosch Sensortec BME280 Datasheet */

/* Calculate t_fine (Temperature Fine Resolution Value) that is used to calculate compensated temperature, pressure, and humidity */
int32_t BME280_I2C_lCalculateTemperatureFine(BME280Handle_t *pxBME280);

/* Returns temperature in [C] value * 100. Divide by 100.0f to get the proper [C] value.
 * Fast compensation due to no floating point calculations. */
int32_t BME280_I2C_lCompensateTemperatureData(BME280Handle_t *pxBME280);

/* Returns pressure in pre-Pascal pre-[Pa] unsigned 32-bit integer 24.8 format. Divide by 256.0f for [Pa] value. */
uint32_t BME280_I2C_ulCompensatePressureData(BME280Handle_t *pxBME280);

/* Returns humidity in pre-[%] as unsigned 32-bit integer 22.10 formate. Divide by 1024.0f for [%] value. */
uint32_t BME280_I2C_ulCompensateHumidityData(BME280Handle_t *pxBME280);

/* Returns temperature in [C] value.
 * Cortex-M4 FPU cannot support double precision operations, so opt for float. */
float BME280_I2C_fCompensateTemperatureData(BME280Handle_t *pxBME280);

/* Returns pressure in [Pa] value. */
float BME280_I2C_fCompensatePressureData(BME280Handle_t *pxBME280);

/* Returns humidity in [%] value. */
float BME280_I2C_fCompensateHumidityData(BME280Handle_t *pxBME280);




/* DEBUG */
void BME280_Debug_vPrintRawData(BME280Handle_t *pxBME280);

void BME280_Debug_vPrintCalibrationData(BME280Handle_t *pxBME280);

void BME280_Debug_vPrintlCompensatedData(BME280Handle_t *pxBME280);

void BME280_Debug_vPrintfCompensatedData(BME280Handle_t *pxBME280);




#endif /* INC_BME280_H_ */
