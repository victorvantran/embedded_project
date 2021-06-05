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

	/* Realign */
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

	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_HUM_ADDRESS, 1, &uCtrlHum, 1, 50);
	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_MEAS_ADDRESS, 1, &uCtrlMeas, 1, 50);
}



void BME280_vMeasureForced(BME280Handle_t *pxBME280,
		uint8_t uPressureOversample,
		uint8_t uTemperatureOversample,
		uint8_t uHumidityOversample)
{
	uint8_t uCtrlHum = (0b00000111 & uHumidityOversample);
	uint8_t uCtrlMeas = (uTemperatureOversample << 5) | (uPressureOversample << 2) | (BME280_FORCED_MODE);

	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_HUM_ADDRESS, 1, &uCtrlHum, 1, 50);
	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_MEAS_ADDRESS, 1, &uCtrlMeas, 1, 50);
}



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



int32_t BME280_lCalculateTemperatureFine(BME280Handle_t *pxBME280)
{
	int32_t lTemperatureRaw = pxBME280->xMeasureRawData.ulTemperatureRawData;
	int32_t lDigT1 = pxBME280->xCalibrationData.xDigT.usT1;
	int32_t lDigT2 = pxBME280->xCalibrationData.xDigT.sT2;
	int32_t lDigT3 = pxBME280->xCalibrationData.xDigT.sT3;

	int32_t lVar1;
	int32_t lVar2;

	lVar1 = (int32_t)((lTemperatureRaw >> 3) - (lDigT1 << 1));
	lVar1 = (lVar1 * lDigT2) >> 11;
	lVar2 = (int32_t)((lTemperatureRaw >> 4) - lDigT1);
	lVar2 = (((lVar2 * lVar2) >> 12) * lDigT3) >> 14;
	pxBME280->xCalibrationData.lTemperatureFine = lVar1 + lVar2;

	return pxBME280->xCalibrationData.lTemperatureFine;
}



int32_t BME280_lCompensateTemperatureData(BME280Handle_t *pxBME280)
{
	static const int32_t lTemperatureMin = -4000;
	static const int32_t lTemperatureMax = 8500;

	int32_t lTemperature;
	lTemperature = ((BME280_lCalculateTemperatureFine(pxBME280)) * 5 + 128) >> 8;

	if (lTemperature < lTemperatureMin)
	{
			lTemperature = lTemperatureMin;
	}
	else if (lTemperature > lTemperatureMax)
	{
			lTemperature = lTemperatureMax;
	}

	return lTemperature;
}



uint32_t BME280_ulCompensatePressureData(BME280Handle_t *pxBME280)
{
	static const uint32_t lPressureMin = 3000000;
	static const uint32_t lPressureMax = 11000000;

	int32_t lPressureRaw = (int32_t)pxBME280->xMeasureRawData.ulPressureRawData;
	int32_t lTemperatureFine = pxBME280->xCalibrationData.lTemperatureFine;
	int64_t llDigP1 = (int64_t)pxBME280->xCalibrationData.xDigP.usP1;
	int64_t llDigP2 = (int64_t)pxBME280->xCalibrationData.xDigP.sP2;
	int64_t llDigP3 = (int64_t)pxBME280->xCalibrationData.xDigP.sP3;
	int64_t llDigP4 = (int64_t)pxBME280->xCalibrationData.xDigP.sP4;
	int64_t llDigP5 = (int64_t)pxBME280->xCalibrationData.xDigP.sP5;
	int64_t llDigP6 = (int64_t)pxBME280->xCalibrationData.xDigP.sP6;
	int64_t llDigP7 = (int64_t)pxBME280->xCalibrationData.xDigP.sP7;
	int64_t llDigP8 = (int64_t)pxBME280->xCalibrationData.xDigP.sP8;
	int64_t llDigP9 = (int64_t)pxBME280->xCalibrationData.xDigP.sP9;

	int64_t lVar1, lVar2, lPressure;
	lVar1 = ((int64_t)lTemperatureFine) - 128000;
	lVar2 = lVar1 * lVar1 * (int64_t)llDigP6;
	lVar2 = lVar2 + ((lVar1*(int64_t)llDigP5) << 17);
	lVar2 = lVar2 + (((int64_t)llDigP4) << 35);
	lVar1 = ((lVar1 * lVar1 * (int64_t)llDigP3) >> 8) + ((lVar1 * (int64_t)llDigP2) << 12);
	lVar1 = (((((int64_t)1) << 47)+lVar1))*((int64_t)llDigP1) >> 33;

	if (lVar1 == 0) return lPressureMin; // Account for divide by zero

	lPressure = 1048576 - lPressureRaw;
	lPressure = (((lPressure << 31)-lVar2) * 3125)/lVar1;
	lVar1 = (((int64_t)llDigP9) * (lPressure >> 13) * (lPressure >> 13)) >> 25;
	lVar2 = (((int64_t)llDigP8) * lPressure) >> 19;
	lPressure = ((lPressure + lVar1 + lVar2) >> 8) + (((int64_t)llDigP7) << 4);
	return (uint32_t)lPressure;
}




float BME280_fCompensateTemperatureData(BME280Handle_t *pxBME280)
{
	static const float fTemperatureMin = -4000.0f;
	static const float fTemperatureMax = 8500.0f;

	int32_t lTemperatureRaw = (int32_t)pxBME280->xMeasureRawData.ulTemperatureRawData;
	int32_t lDigT1 = pxBME280->xCalibrationData.xDigT.usT1;
	int32_t lDigT2 = pxBME280->xCalibrationData.xDigT.sT2;
	int32_t lDigT3 = pxBME280->xCalibrationData.xDigT.sT3;

	float fTemperature;

	fTemperature = (float)(BME280_lCalculateTemperatureFine(pxBME280)) / 5120.0f;

	if (fTemperature < fTemperatureMin)
	{
			fTemperature = fTemperatureMin;
	}
	else if (fTemperature > fTemperatureMax)
	{
			fTemperature = fTemperatureMax;
	}

	return fTemperature;
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
	printf("dig_t3: %hd\r\n", pxBME280->xCalibrationData.xDigT.sT3);

	printf("dig_p1: %u\r\n", pxBME280->xCalibrationData.xDigP.usP1);
	printf("dig_p2: %hd\r\n", pxBME280->xCalibrationData.xDigP.sP2);
	printf("dig_p3: %hd\r\n", pxBME280->xCalibrationData.xDigP.sP3);
	printf("dig_p4: %hd\r\n", pxBME280->xCalibrationData.xDigP.sP4);
	printf("dig_p5: %hd\r\n", pxBME280->xCalibrationData.xDigP.sP5);
	printf("dig_p6: %hd\r\n", pxBME280->xCalibrationData.xDigP.sP6);
	printf("dig_p7: %hd\r\n", pxBME280->xCalibrationData.xDigP.sP7);
	printf("dig_p8: %hd\r\n", pxBME280->xCalibrationData.xDigP.sP8);
	printf("dig_p9: %hd\r\n", pxBME280->xCalibrationData.xDigP.sP9);

	printf("dig_h1: %u\r\n", pxBME280->xCalibrationData.xDigH.ucH1);
	printf("dig_h2: %hd\r\n", pxBME280->xCalibrationData.xDigH.usH2);
	printf("dig_h3: %u\r\n", pxBME280->xCalibrationData.xDigH.ucH3);
	printf("dig_h4: %hd\r\n", pxBME280->xCalibrationData.xDigH.sH4);
	printf("dig_h5: %hd\r\n", pxBME280->xCalibrationData.xDigH.sH5);
	printf("dig_h6: %hd\r\n", pxBME280->xCalibrationData.xDigH.cH6);
}


void BME280_vPrintlCompensatedData(BME280Handle_t *pxBME280)
{
	int32_t lCompensatedTemperatureData = BME280_lCompensateTemperatureData(pxBME280);
	uint32_t lCompensatedPressureData = BME280_ulCompensatePressureData(pxBME280);

	printf("lTemperature Calibrated Data: %d\r\n", lCompensatedTemperatureData);
	printf("lPressure Calibrated Data: %u\r\n", lCompensatedPressureData);

}



void BME280_vPrintfCompensatedData(BME280Handle_t *pxBME280)
{
	float fCompensatedTemperatureData = BME280_fCompensateTemperatureData(pxBME280);
	printf("fTemperature Calibrated Data: %.8f\r\n", fCompensatedTemperatureData);
}


