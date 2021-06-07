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




/* SPI */
void BME280_SPI_vInit(BME280Handle_t *pxBME280,
		SPI_HandleTypeDef *pxSPIHandle,
		GPIO_TypeDef *pxSPICSGPIO, uint16_t uSPICSGPIOPIN)
{
	pxBME280->pxI2CHandle = NULL;
	pxBME280->uI2CSlaveAddress = 0;

	pxBME280->pxSPIHandle = pxSPIHandle;
	pxBME280->pxSPICSGPIO = pxSPICSGPIO;
	pxBME280->uSPICSGPIOPIN = uSPICSGPIOPIN;

	pxBME280->xMeasureRegData = xDefaultMeasureRegData;
	pxBME280->xMeasureRawData = xDefaultMeasureRawData;

	BME280_SPI_vReadCalibrationData(pxBME280);
}


HAL_StatusTypeDef BME280_SPI_vSetMode(BME280Handle_t *pxBME280)
{
	if (pxBME280->pxSPIHandle->Init.CLKPolarity == BME280_SPI_CPOL && pxBME280->pxSPIHandle->Init.CLKPhase == BME280_SPI_CPHA) return HAL_OK;

	/* Reinit SPI with proper spi mode for the peripheral */
	if (HAL_SPI_DeInit(pxBME280->pxSPIHandle) != HAL_OK) return HAL_ERROR;
	pxBME280->pxSPIHandle->Init.CLKPolarity = BME280_SPI_CPOL;
	pxBME280->pxSPIHandle->Init.CLKPhase = BME280_SPI_CPHA;
	if (HAL_SPI_Init(pxBME280->pxSPIHandle) != HAL_OK) return HAL_ERROR;

	return HAL_OK;
}


void BME280_SPI_vReadChipID(BME280Handle_t *pxBME280)
{
	static const uint8_t ucReadChipIDReg = BME280_SPI_READ | (BME280_CHIP_ADDRESS & 0x7F);
  uint8_t ucChipID;

  BME280_SPI_vSetMode(pxBME280);
	HAL_GPIO_WritePin(pxBME280->pxSPICSGPIO, pxBME280->uSPICSGPIOPIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(pxBME280->pxSPIHandle, &ucReadChipIDReg, sizeof(ucReadChipIDReg), 50);
	HAL_SPI_Receive(pxBME280->pxSPIHandle, &ucChipID, sizeof(ucChipID), 50);
	HAL_GPIO_WritePin(pxBME280->pxSPICSGPIO, pxBME280->uSPICSGPIOPIN, GPIO_PIN_SET);

	printf("Chip ID: %d\r\n", (int16_t)ucChipID);
}


void BME280_SPI_vReadCalibrationData(BME280Handle_t *pxBME280)
{

}







/* I2C */
void BME280_I2C_vInit(BME280Handle_t *pxBME280,
		I2C_HandleTypeDef *pxI2CHandle,
		uint8_t uI2CSlaveAddress )
{
	pxBME280->pxI2CHandle = pxI2CHandle;
	pxBME280->uI2CSlaveAddress = uI2CSlaveAddress;

	pxBME280->pxSPIHandle = NULL;
	pxBME280->pxSPICSGPIO = NULL;
	pxBME280->uSPICSGPIOPIN = 0;

	pxBME280->xMeasureRegData = xDefaultMeasureRegData;
	pxBME280->xMeasureRawData = xDefaultMeasureRawData;

	BME280_I2C_vReadCalibrationData(pxBME280);
}


void BME280_I2C_vReadCalibrationData(BME280Handle_t *pxBME280)
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



uint8_t BME280_I2C_vReadChipID(BME280Handle_t *pxBME280)
{
	uint8_t uChipID = 0;
	HAL_I2C_Mem_Read(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CHIP_ADDRESS, 1, &uChipID, 1, 50);
	return uChipID;
}



void BME280_I2C_vMeasureAllForced(BME280Handle_t *pxBME280)
{
	static const uint8_t uCtrlHum 	= 0b00000001;
	static const uint8_t uCtrlMeas	=	0b00100110;

	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_HUM_ADDRESS, 1, &uCtrlHum, 1, 50);
	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_MEAS_ADDRESS, 1, &uCtrlMeas, 1, 50);
}



void BME280_I2C_vMeasureForced(BME280Handle_t *pxBME280,
		uint8_t uPressureOversample,
		uint8_t uTemperatureOversample,
		uint8_t uHumidityOversample)
{
	uint8_t uCtrlHum = (0b00000111 & uHumidityOversample);
	uint8_t uCtrlMeas = (uTemperatureOversample << 5) | (uPressureOversample << 2) | (BME280_FORCED_MODE);

	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_HUM_ADDRESS, 1, &uCtrlHum, 1, 50);
	HAL_I2C_Mem_Write(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_CTRL_MEAS_ADDRESS, 1, &uCtrlMeas, 1, 50);
}



void BME280_I2C_vReadRawData(BME280Handle_t *pxBME280)
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



int32_t BME280_I2C_lCalculateTemperatureFine(BME280Handle_t *pxBME280)
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



int32_t BME280_I2C_lCompensateTemperatureData(BME280Handle_t *pxBME280)
{
	static const int32_t lTemperatureMin = -4000;
	static const int32_t lTemperatureMax = 8500;

	int32_t lTemperature;
	lTemperature = ((BME280_I2C_lCalculateTemperatureFine(pxBME280)) * 5 + 128) >> 8;

	lTemperature = lTemperature < lTemperatureMin ? lTemperatureMin : lTemperature;
	lTemperature = lTemperature > lTemperatureMax ? lTemperatureMax : lTemperature;

	return lTemperature;
}



uint32_t BME280_I2C_ulCompensatePressureData(BME280Handle_t *pxBME280)
{
	static const uint32_t ulPressureMin = 30000 * 256;
	static const uint32_t ulPressureMax = 110000 * 256;

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

	int64_t lVar1, lVar2, llPressure;
	uint32_t ulPressure;
	lVar1 = ((int64_t)lTemperatureFine) - 128000;
	lVar2 = lVar1 * lVar1 * (int64_t)llDigP6;
	lVar2 = lVar2 + ((lVar1*(int64_t)llDigP5) << 17);
	lVar2 = lVar2 + (((int64_t)llDigP4) << 35);
	lVar1 = ((lVar1 * lVar1 * (int64_t)llDigP3) >> 8) + ((lVar1 * (int64_t)llDigP2) << 12);
	lVar1 = (((((int64_t)1) << 47)+lVar1))*((int64_t)llDigP1) >> 33;

	if (lVar1 == 0) return ulPressureMin; /* Account for divide by zero */

	llPressure = 1048576 - lPressureRaw;
	llPressure = (((llPressure << 31)-lVar2) * 3125)/lVar1;
	lVar1 = (((int64_t)llDigP9) * (llPressure >> 13) * (llPressure >> 13)) >> 25;
	lVar2 = (((int64_t)llDigP8) * llPressure) >> 19;
	llPressure = (((llPressure + lVar1 + lVar2) >> 8) + (((int64_t)llDigP7) << 4));
	ulPressure = (uint32_t)llPressure;

	ulPressure = ulPressure < ulPressureMin ? ulPressureMin : ulPressure;
	ulPressure = ulPressure > ulPressureMax ? ulPressureMax : ulPressure;

	return ulPressure;
}



uint32_t BME280_I2C_ulCompensateHumidityData(BME280Handle_t *pxBME280)
{
	static const uint32_t ulHumidityMax = 102400;

	int32_t lHumidityRaw = (int32_t)pxBME280->xMeasureRawData.uHumidityRawData;
	int32_t lTemperatureFine = pxBME280->xCalibrationData.lTemperatureFine;
	int32_t lDigH1 = (int32_t)pxBME280->xCalibrationData.xDigH.ucH1;
	int32_t lDigH2 = (int32_t)pxBME280->xCalibrationData.xDigH.usH2;
	int32_t lDigH3 = (int32_t)pxBME280->xCalibrationData.xDigH.ucH3;
	int32_t lDigH4 = (int32_t)pxBME280->xCalibrationData.xDigH.sH4;
	int32_t lDigH5 = (int32_t)pxBME280->xCalibrationData.xDigH.sH5;
	int32_t lDigH6 = (int32_t)pxBME280->xCalibrationData.xDigH.cH6;

	uint32_t ulHumidity;

	ulHumidity = (lTemperatureFine - ((int32_t)76800));
	ulHumidity = (((((lHumidityRaw << 14) - ((lDigH4) << 20) - ((lDigH5) *
	ulHumidity)) + ((int32_t)16384)) >> 15) * (((((((ulHumidity *
	(lDigH6)) >> 10) * (((ulHumidity * (lDigH3)) >> 11) +
	((int32_t)32768))) >> 10) + ((int32_t)2097152)) * (lDigH2) +
	8192) >> 14));
	ulHumidity = ((ulHumidity - (((((ulHumidity >> 15) * (ulHumidity >> 15)) >> 7) *
	(lDigH1)) >> 4)) >> 12);
	ulHumidity = (ulHumidity > ulHumidityMax ? ulHumidityMax : ulHumidity);

	return ulHumidity;
}



float BME280_I2C_fCompensateTemperatureData(BME280Handle_t *pxBME280)
{
	static const float fTemperatureMin = -40.0f;
	static const float fTemperatureMax = 85.0f;

	float fTemperature;

	fTemperature = (float)(BME280_I2C_lCalculateTemperatureFine(pxBME280)) / 5120.0f;
	fTemperature = fTemperature < fTemperatureMin ? fTemperatureMin : fTemperature;
	fTemperature = fTemperature > fTemperatureMax ? fTemperatureMax : fTemperature;

	return fTemperature;
}



float BME280_I2C_fCompensatePressureData(BME280Handle_t *pxBME280)
{
	static const float fPressureMin = 30000.0f;
	static const float fPressureMax = 110000.0f;

	int32_t lPressureRaw = (int32_t)pxBME280->xMeasureRawData.ulPressureRawData;
	int32_t lTemperatureFine = pxBME280->xCalibrationData.lTemperatureFine;
	float fDigP1 = (float)pxBME280->xCalibrationData.xDigP.usP1;
	float fDigP2 = (float)pxBME280->xCalibrationData.xDigP.sP2;
	float fDigP3 = (float)pxBME280->xCalibrationData.xDigP.sP3;
	float fDigP4 = (float)pxBME280->xCalibrationData.xDigP.sP4;
	float fDigP5 = (float)pxBME280->xCalibrationData.xDigP.sP5;
	float fDigP6 = (float)pxBME280->xCalibrationData.xDigP.sP6;
	float fDigP7 = (float)pxBME280->xCalibrationData.xDigP.sP7;
	float fDigP8 = (float)pxBME280->xCalibrationData.xDigP.sP8;
	float fDigP9 = (float)pxBME280->xCalibrationData.xDigP.sP9;

	float fVar1, fVar2, fPressure;
	fVar1 = ((float)lTemperatureFine / 2.0f) - 64000.0f;
	fVar2 = fVar1 * fVar1 * (fDigP6) / 32768.0;
	fVar2 = fVar2 + fVar1 * (fDigP5) * 2.0;
	fVar2 = (fVar2 / 4.0) + ((fDigP4) * 65536.0);
	fVar1 = ((fDigP3) * fVar1 * fVar1 / 524288.0 + (fDigP2) * fVar1) / 524288.0;
	fVar1 = (1.0 + fVar1 / 32768.0) * (fDigP1);

	if (fVar1 == 0.0) return fPressureMin; /* Account for divide by zero */

	fPressure = 1048576.0 - (float)lPressureRaw;
	fPressure = (fPressure - (fVar2 / 4096.0)) * 6250.0 / fVar1;
	fVar1 = (fDigP9) * fPressure * fPressure / 2147483648.0;
	fVar2 = fPressure * (fDigP8) / 32768.0;
	fPressure = fPressure + (fVar1 + fVar2 + (fDigP7)) / 16.0;

	fPressure = fPressure > fPressureMax ? fPressureMax : fPressure;

	return fPressure;
}


float BME280_I2C_fCompensateHumidityData(BME280Handle_t *pxBME280)
{
	static const float fHumidityMin = 0.0f;
	static const float fHumidityMax = 100.0f;

	int32_t lHumidityRaw = (int32_t)pxBME280->xMeasureRawData.uHumidityRawData;
	int32_t lTemperatureFine = pxBME280->xCalibrationData.lTemperatureFine;
	float fDigH1 = (float)pxBME280->xCalibrationData.xDigH.ucH1;
	float fDigH2 = (float)pxBME280->xCalibrationData.xDigH.usH2;
	float fDigH3 = (float)pxBME280->xCalibrationData.xDigH.ucH3;
	float fDigH4 = (float)pxBME280->xCalibrationData.xDigH.sH4;
	float fDigH5 = (float)pxBME280->xCalibrationData.xDigH.sH5;
	float fDigH6 = (float)pxBME280->xCalibrationData.xDigH.cH6;

	float fHumidity;
	fHumidity = (((float)lTemperatureFine) - 76800.0f);
	fHumidity = (lHumidityRaw - ((fDigH4) * 64.0f + (fDigH5) / 16384.0f *
	fHumidity)) * ((fDigH2) / 65536.0f * (1.0f + (fDigH6) /
	67108864.0f * fHumidity *
	(1.0 + (fDigH3) / 67108864.0f * fHumidity)));
	fHumidity = fHumidity * (1.0f - (fDigH1) * fHumidity / 524288.0f);
	fHumidity = fHumidity < fHumidityMin ? fHumidityMin : fHumidity;
	fHumidity = fHumidity > fHumidityMax ? fHumidityMax : fHumidity;

	return fHumidity;
}




/* DEBUG */
void BME280_I2C_vPrintRawData(BME280Handle_t *pxBME280)
{
	printf("Pressure Raw Data: %lu\r\n", pxBME280->xMeasureRawData.ulPressureRawData);
	printf("Temperature Raw Data: %lu\r\n", pxBME280->xMeasureRawData.ulTemperatureRawData);
	printf("Humidity Raw Data: %u\r\n", pxBME280->xMeasureRawData.uHumidityRawData);
}


void BME280_I2C_vPrintCalibrationData(BME280Handle_t *pxBME280)
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



void BME280_I2C_vPrintlCompensatedData(BME280Handle_t *pxBME280)
{
	int32_t lCompensatedTemperatureData = BME280_I2C_lCompensateTemperatureData(pxBME280);
	uint32_t lCompensatedPressureData = BME280_I2C_ulCompensatePressureData(pxBME280);
	uint32_t lCompensatedHumidityData = BME280_I2C_ulCompensateHumidityData(pxBME280);

	printf("lTemperature Calibrated Data: %d\r\n", lCompensatedTemperatureData);
	printf("lPressure Calibrated Data: %u\r\n", lCompensatedPressureData);
	printf("lHumidity Calibrated Data: %u\r\n", lCompensatedHumidityData);
}



void BME280_I2C_vPrintfCompensatedData(BME280Handle_t *pxBME280)
{
	float fCompensatedTemperatureData = BME280_I2C_fCompensateTemperatureData(pxBME280);
	float fCompensatedPressureData = BME280_I2C_fCompensatePressureData(pxBME280);
	float fCompensatedHumidityData = BME280_I2C_fCompensateHumidityData(pxBME280);

	printf("fTemperature Calibrated Data: %.8fC\r\n", fCompensatedTemperatureData);
	printf("fPressure Calibrated Data: %.8fPa\r\n", fCompensatedPressureData);
	printf("fHumidity Calibrated Data: %.8f%%\r\n", fCompensatedHumidityData);
}


