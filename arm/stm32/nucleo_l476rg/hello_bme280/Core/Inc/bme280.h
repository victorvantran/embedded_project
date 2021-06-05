/*
 * bme280.h
 *
 *  Created on: Jun 4, 2021
 *      Author: victor
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_


/* APPLICATION PROGRAMMER */
#include <string.h>


/* IMPLEMENTATION */

#define BME280_I2C_SLAVE_ADDRESS_A 0x76
#define BME280_I2C_SLAVE_ADDRESS_B 0x77

#define BME280_CHIP_ADDRESS 0xD0
#define BME280_DATA_ADDRESS 0xF7


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

BME280MeasureRegData_t xDefaultMeasureRegData = {0};


typedef struct
{
	uint32_t ulPressureRawData;
	uint32_t ulTemperatureRawData;
	uint16_t uHumidityRawData;
} BME280MeasureRawData_t;

BME280MeasureRawData_t xDefaultMeasureRawData = {0};



typedef struct
{
	I2C_HandleTypeDef *pxI2CHandle;
	uint8_t uI2CSlaveAddress;

	BME280MeasureRegData_t xMeasureRegData;
	BME280MeasureRawData_t xMeasureRawData;

} BME280Handle_t;




void BME280_vInit(BME280Handle_t *pxBME280,
		I2C_HandleTypeDef *pxI2CHandle,
		uint8_t uI2CSlaveAddress )
{
	pxBME280->pxI2CHandle = pxI2CHandle;
	pxBME280->uI2CSlaveAddress = uI2CSlaveAddress;

	pxBME280->xMeasureRegData = xDefaultMeasureRegData;
	pxBME280->xMeasureRawData = xDefaultMeasureRawData;
}

/* Read the raw register data [pressure, temperature, humidity] and cast 20-bit, 20-bit, 16-bit to the respective raw data */
void BME280_vMeasureRawData(BME280Handle_t *pxBME280)
{
	HAL_I2C_Mem_Read(pxBME280->pxI2CHandle, (uint16_t)(pxBME280->uI2CSlaveAddress << 1), (uint16_t)BME280_DATA_ADDRESS, 1, (uint8_t *)&(pxBME280->xMeasureRegData), 8, 50);

	pxBME280->xMeasureRawData.ulPressureRawData =
			(pxBME280->xMeasureRegData.xPressureRegData.uMSB << 12) +
			(pxBME280->xMeasureRegData.xPressureRegData.uLSB << 4) +
			(pxBME280->xMeasureRegData.xPressureRegData.uXLSB >> 4);

	pxBME280->xMeasureRawData.ulTemperatureRawData =
			(pxBME280->xMeasureRegData.xTemperatureRegData.uMSB << 12) +
			(pxBME280->xMeasureRegData.xTemperatureRegData.uLSB << 4) +
			(pxBME280->xMeasureRegData.xTemperatureRegData.uXLSB >> 4);

	pxBME280->xMeasureRawData.uHumidityRawData =
			(pxBME280->xMeasureRegData.xTemperatureRegData.uMSB << 8) +
			(pxBME280->xMeasureRegData.xTemperatureRegData.uLSB);
}


/* DEBUG */
void BME280_vPrintRawData(BME280Handle_t *pxBME280)
{
	printf("Pressure Raw Data: %lu\r\n", pxBME280->xMeasureRawData.ulPressureRawData);
	printf("Temperature Raw Data: %lu\r\n", pxBME280->xMeasureRawData.ulTemperatureRawData);
	printf("Humidity Raw Data: %u\r\n", pxBME280->xMeasureRawData.uHumidityRawData);
}


#endif /* INC_BME280_H_ */
