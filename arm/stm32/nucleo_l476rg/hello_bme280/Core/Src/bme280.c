/*
 * bme280.c
 *
 *  Created on: Jun 4, 2021
 *      Author: victor
 */


/* APPLICATION */



/* IMPLEMENTATION */
#include "bme280.h"

/* [!] Can replace this static with memset to save memory */
const BME280MeasureRegData_t xDefaultMeasureRegData = {0};
const BME280MeasureRawData_t xDefaultMeasureRawData = {0};


void BME280_vInit(BME280Handle_t *pxBME280,
		I2C_HandleTypeDef *pxI2CHandle,
		uint8_t uI2CSlaveAddress )
{
	pxBME280->pxI2CHandle = pxI2CHandle;
	pxBME280->uI2CSlaveAddress = uI2CSlaveAddress;

	pxBME280->xMeasureRegData = xDefaultMeasureRegData;
	pxBME280->xMeasureRawData = xDefaultMeasureRawData;

	BME280_vReadCalibrationData(pxBME280);
}


void BME280_vReadCalibrationData(BME280Handle_t *pxBME280)
{
	HAL_I2C_Mem_Read(pxBME280->pxI2CHandle,
			(uint16_t)(pxBME280->uI2CSlaveAddress << 1),
			(uint16_t)BME280_CALIBRATE_ADDRESS_A, 1,
			(uint8_t *)&(pxBME280->xCalibrationData), 25,
			50
	);

	BME280CalibrationBRegData xCalibrationBRegData = {0};
	HAL_I2C_Mem_Read(pxBME280->pxI2CHandle,
			(uint16_t)(pxBME280->uI2CSlaveAddress << 1),
			(uint16_t)BME280_CALIBRATE_ADDRESS_B, 1,
			(uint8_t *)&xCalibrationBRegData, 8,
			50
	);

	// Realign
	pxBME280->xCalibrationData.xDigH.usH2 = ((uint16_t)xCalibrationBRegData.uc0xE2 << 8) + ((uint16_t)xCalibrationBRegData.uc0xE1);
	pxBME280->xCalibrationData.xDigH.ucH3 = xCalibrationBRegData.uc0xE3;
	pxBME280->xCalibrationData.xDigH.sH4 = ((uint16_t)xCalibrationBRegData.uc0xE4 << 4) + ((uint16_t)(xCalibrationBRegData.uc0xE5 & 0x0F));
	pxBME280->xCalibrationData.xDigH.sH5 = ((uint16_t)(xCalibrationBRegData.uc0xE6) << 4) + ((uint16_t)((xCalibrationBRegData.uc0xE5 & 0xF0) >> 4));
	pxBME280->xCalibrationData.xDigH.cH6 = ((int8_t)(xCalibrationBRegData.uc0xE7));
}



uint8_t BME280_vReadChipID(BME280Handle_t *pxBME280)
{
	uint8_t uChipID = 0;
	HAL_I2C_Mem_Read(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CHIP_ADDRESS, 1, &uChipID, 1, 50);
	return uChipID;
}



void BME280_vMeasureAllForced(BME280Handle_t *pxBME280)
{
	static const uint8_t uCtrlHum 	= 0b00000001;
	static const uint8_t uCtrlMeas	=	0b00100110;

	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_HUM, 1, &uCtrlHum, 1, 50);
	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_MEAS, 1, &uCtrlMeas, 1, 50);
}



void BME280_vMeasureForced(BME280Handle_t *pxBME280,
		uint8_t uPressureOversample,
		uint8_t uTemperatureOversample,
		uint8_t uHumidityOversample)
{
	uint8_t uCtrlHum = (0b00000111 & uHumidityOversample);
	uint8_t uCtrlMeas = (uTemperatureOversample << 5) | (uPressureOversample << 2) | (BME280_FORCED_MODE);

	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_HUM, 1, &uCtrlHum, 1, 50);
	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_MEAS, 1, &uCtrlMeas, 1, 50);
}



/* Read the raw register data [pressure, temperature, humidity] and cast 20-bit, 20-bit, 16-bit to the respective raw data */
void BME280_vReadRawData(BME280Handle_t *pxBME280)
{
	HAL_I2C_Mem_Read(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_MEASURE_DATA_ADDRESS, 1, (uint8_t *)&(pxBME280->xMeasureRegData), 8, 50);

	pxBME280->xMeasureRawData.ulPressureRawData =
			(pxBME280->xMeasureRegData.xPressureRegData.uMSB << 12) +
			(pxBME280->xMeasureRegData.xPressureRegData.uLSB << 4) +
			(pxBME280->xMeasureRegData.xPressureRegData.uXLSB >> 4);

	pxBME280->xMeasureRawData.ulTemperatureRawData =
			(pxBME280->xMeasureRegData.xTemperatureRegData.uMSB << 12) +
			(pxBME280->xMeasureRegData.xTemperatureRegData.uLSB << 4) +
			(pxBME280->xMeasureRegData.xTemperatureRegData.uXLSB >> 4);

	pxBME280->xMeasureRawData.uHumidityRawData =
			(pxBME280->xMeasureRegData.xHumidityRegData.uMSB << 8) +
			(pxBME280->xMeasureRegData.xHumidityRegData.uLSB);
}


/* DEBUG */
void BME280_vPrintRawData(BME280Handle_t *pxBME280)
{
	printf("Pressure Raw Data: %lu\r\n", pxBME280->xMeasureRawData.ulPressureRawData);
	printf("Temperature Raw Data: %lu\r\n", pxBME280->xMeasureRawData.ulTemperatureRawData);
	printf("Humidity Raw Data: %u\r\n", pxBME280->xMeasureRawData.uHumidityRawData);
}


void BME280_vPrintCalibrationData(BME280Handle_t *pxBME280)
{
	printf("dig_t1: %u\r\n", pxBME280->xCalibrationData.xDigT.usT1);
	printf("dig_t2: %hd\r\n", pxBME280->xCalibrationData.xDigT.sT2);
	printf("dig_t2: %hd\r\n", pxBME280->xCalibrationData.xDigT.sT3);
}



void BME280_vPrintCompensatedData(BME280Handle_t *pxBME280)
{
	int32_t var1;
	int32_t var2;
	int32_t lTemperatureFine;

	int32_t adcT = (int32_t)pxBME280->xMeasureRawData.ulTemperatureRawData;

	var1 = ((((adcT >> 3) - ((int32_t)pxBME280->xCalibrationData.xDigT.usT1 << 1))) * ((int32_t)pxBME280->xCalibrationData.xDigT.sT2)) >> 11;
	var2 = (((((adcT >> 4) - ((int32_t)pxBME280->xCalibrationData.xDigT.usT1)) * ((adcT >> 4) - ((int32_t)pxBME280->xCalibrationData.xDigT.usT1))) >> 12) *
			((int32_t)pxBME280->xCalibrationData.xDigT.sT3)) >> 14;

	lTemperatureFine = var1 + var2;

	double dTemperatureFine = ((double)(lTemperatureFine))/5120.0;


	printf("Temperature Raw Data: %.2lf\r\n", dTemperatureFine);
	//printf("Temperature Raw Data: %lf\r\n", dTemperatureFine);
	//printf("Temperature Raw Data: %f\r\n", 7.1f);

}


