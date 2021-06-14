/*
 * mpu6050.h
 *
 *  Created on: Jun 12, 2021
 *      Author: victor
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_




#define MPU6050_BASE_SLAVE_ADDRESS 0x68
#define MPU6050_REGISTER_WHO_AM_I 0x75
#define MPU6050_REGISTER_ACELL_XOUT 0x3B
#define MPU6050_REGISTER_GYRO_XOUT 0x43
#define MPU6050_REGISTER_TEMP_OUT 0x41


#define MPU6050_REGISTER_PWR_MGMT_1 0x6B
#define MPU6050_REGISTER_CONFIG 0x1A
#define MPU6050_REGISTER_GYRO_CONFIG 0x1B
#define MPU6050_REGISTER_ACCEL_CONFIG 0x1C
#define MPU6050_REGISTER_SMPRT_DIV 0x19
#define MPU6050_REGISTER_INT_STATUS 0x3A


/* APPLICATION */
#include "stm32l4xx_hal.h"
#define MPU6050_AD0 0
#define MPU6050_SLAVE_ADDRESS ((MPU6050_BASE_SLAVE_ADDRESS) | (MPU6050_AD0))
#define MPU6050_DEFAULT_TIMEOUT 50





/* IMPLEMENTATION */
#include <math.h>



typedef struct
{
	uint8_t ucClockSource;
	uint8_t ucGyroFullScale;
	uint8_t ucAcellFullScale;
	uint8_t ucConfigDLPF;
	uint8_t ucSleepMode;
} MPU6050ConfigHandle_t;



typedef struct
{
	I2C_HandleTypeDef *pxI2C;
} MPU6050Handle_t;



HAL_StatusTypeDef MPU6050_I2C_vConfig(MPU6050Handle_t *pxMPU6050, MPU6050ConfigHandle_t *pxMPU6050Config)
{
	uint8_t pwrmgmt1Data = 0x80;
	HAL_StatusTypeDef xStatus = HAL_I2C_Mem_Write(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_PWR_MGMT_1, 1, &pwrmgmt1Data, 1, MPU6050_DEFAULT_TIMEOUT);
	HAL_Delay(100);




	uint8_t ucBuffer;
	ucBuffer = pxMPU6050Config->ucClockSource & 0x07;
	ucBuffer |= (pxMPU6050Config->ucSleepMode << 6) & 0x40;
	xStatus = HAL_I2C_Mem_Write(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_PWR_MGMT_1, 1, &ucBuffer, 1, MPU6050_DEFAULT_TIMEOUT);
	HAL_Delay(100);


	ucBuffer = 0;
	ucBuffer = pxMPU6050Config->ucConfigDLPF & 0x07;
	xStatus = HAL_I2C_Mem_Write(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_CONFIG, 1, &ucBuffer, 1, MPU6050_DEFAULT_TIMEOUT);


	ucBuffer = 0;
	ucBuffer = pxMPU6050Config->ucGyroFullScale & 0x18;
	xStatus = HAL_I2C_Mem_Write(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_GYRO_CONFIG, 1, &ucBuffer, 1, MPU6050_DEFAULT_TIMEOUT);


	ucBuffer = 0;
	ucBuffer = pxMPU6050Config->ucAcellFullScale & 0x18;
	xStatus = HAL_I2C_Mem_Write(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_ACCEL_CONFIG, 1, &ucBuffer, 1, MPU6050_DEFAULT_TIMEOUT);


	ucBuffer = 0;
	ucBuffer = 0x04;
	xStatus = HAL_I2C_Mem_Write(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_SMPRT_DIV, 1, &ucBuffer, 1, MPU6050_DEFAULT_TIMEOUT);
}




HAL_StatusTypeDef MPU6050_I2C_vInit(MPU6050Handle_t *pxMPU6050,
		I2C_HandleTypeDef *pxI2C,
		MPU6050ConfigHandle_t *pxMPU6050Config)
{
	pxMPU6050->pxI2C = pxI2C;

	if (pxMPU6050->pxI2C == NULL)
	{
		return HAL_ERROR;
	}

	MPU6050_I2C_vConfig(pxMPU6050, pxMPU6050Config);

	return HAL_OK;
}





HAL_StatusTypeDef MPU6050_I2C_xGetWhoAmI(MPU6050Handle_t *pxMPU6050, uint8_t *pucWhoAmI)
{
	HAL_StatusTypeDef xStatus = HAL_I2C_Mem_Read(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_WHO_AM_I, 1, pucWhoAmI, 1, MPU6050_DEFAULT_TIMEOUT);
	*pucWhoAmI = (*pucWhoAmI & 0b01111110) >> 1;
	return xStatus;
}



void MPU6050_I2C_Debug_vReadWhoAmI(MPU6050Handle_t *pxMPU6050)
{
	uint8_t ucWhoAmI;
	MPU6050_I2C_xGetWhoAmI(pxMPU6050, &ucWhoAmI);
	printf("Who Am I: %u\r\n", (uint16_t)ucWhoAmI);
}




/*
typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
} TestHandle_t;


HAL_StatusTypeDef MPU6050_I2C_xGetAccelerometer(MPU6050Handle_t *pxMPU6050, TestHandle_t *pxT)
{
	HAL_StatusTypeDef xStatus = HAL_I2C_Mem_Read(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_ACELL_XOUT, 1, pxT, 6, MPU6050_DEFAULT_TIMEOUT);
	return xStatus;
}


void MPU6050_I2C_Debug_vReadAccelerometer(MPU6050Handle_t *pxMPU6050)
{
	TestHandle_t t;
	MPU6050_I2C_xGetAccelerometer(pxMPU6050, &t);
	printf("Accelerometer X: %d\r\n", t.x);
	printf("Accelerometer Y: %d\r\n", t.y);
	printf("Accelerometer Z: %d\r\n", t.z);

}
*/


void MPU6050_I2C_Debug_vReadGyroscope(MPU6050Handle_t *pxMPU6050)
{
	uint8_t i2cBuf[2];
	uint8_t GyroArr[6];
	HAL_StatusTypeDef xStatus = HAL_I2C_Mem_Read(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_INT_STATUS, 1, &i2cBuf[1], 1, MPU6050_DEFAULT_TIMEOUT);
	if (i2cBuf[1] && 0x01)
	{
		xStatus = HAL_I2C_Mem_Read(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_GYRO_XOUT, 1, GyroArr, 6, MPU6050_DEFAULT_TIMEOUT);

		int16_t gX = (GyroArr[0] << 8) + GyroArr[1];
		int16_t gY = (GyroArr[2] << 8) + GyroArr[3];
		int16_t gZ = (GyroArr[4] << 8) + GyroArr[5];

		//printf("Gyroscope X: %d\r\n", gX);
		//printf("Gyroscope Y: %d\r\n", gY);
		//printf("Gyroscope Z: %d\r\n", gZ);



		float fScale = (500.0f/32768.0f);
		printf("fGyroscope X: %.8f\r\n", (float)gX * fScale);
		//printf("fAccelerometer Y: %.8f\r\n", (float)aY * fScale);
		//printf("fAccelerometer Z: %.8f\r\n", (float)aZ * fScale);

	}
}




void MPU6050_I2C_Debug_vReadAccelerometer(MPU6050Handle_t *pxMPU6050)
{
	uint8_t i2cBuf[2];
	uint8_t AcceArr[6], GyroArr[6];
	HAL_StatusTypeDef xStatus = HAL_I2C_Mem_Read(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_INT_STATUS, 1, &i2cBuf[1], 1, MPU6050_DEFAULT_TIMEOUT);
	if (i2cBuf[1] && 0x01)
	{
		xStatus = HAL_I2C_Mem_Read(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_ACELL_XOUT, 1, AcceArr, 6, MPU6050_DEFAULT_TIMEOUT);

		int16_t aX = (AcceArr[0] << 8) + AcceArr[1];
		int16_t aY = (AcceArr[2] << 8) + AcceArr[3];
		int16_t aZ = (AcceArr[4] << 8) + AcceArr[5];

		//printf("Accelerometer X: %d\r\n", aX);
		//printf("Accelerometer Y: %d\r\n", aY);
		//printf("Accelerometer Z: %d\r\n", aZ);


		//float fScale = 1.0f; //(16000.0f/32768.0f);
		//printf("fAccelerometer X: %.8f\r\n", (float)aX * fScale);
		//printf("fAccelerometer Y: %.8f\r\n", (float)aY * fScale);
		//printf("fAccelerometer Z: %.8f\r\n", (float)aZ * fScale);


		float fAX = ((float)aX/32768.0f) * 90.0f;
		float fAY = ((float)aY/32768.0f) * 90.0f;
		float fAZ = ((float)aZ/32768.0f) * 90.0f;


		float fPitch = atan2f(-fAY, fAZ);// - M_PI;

		fPitch = fPitch * 180.0 / M_PI;
		if (fPitch < -180.0f) fPitch += 360.0f;

		printf("fPitch Z: %.8f\r\n", fPitch);

	}
}




void MPU6050_I2C_Debug_vReadTemperature(MPU6050Handle_t *pxMPU6050)
{
	uint16_t usTemp;

	HAL_StatusTypeDef xStatus = HAL_I2C_Mem_Read(pxMPU6050->pxI2C, (uint16_t)(MPU6050_SLAVE_ADDRESS << 1), (uint16_t)MPU6050_REGISTER_ACELL_XOUT, 1, &usTemp, 2, MPU6050_DEFAULT_TIMEOUT);

	printf("T: %u\r\n", usTemp);


}



#endif /* INC_MPU6050_H_ */
