/*
 * ESP8266.h
 *
 *  Created on: May 22, 2021
 *      Author: victor
 */

#ifndef INC_ESP8266_H_
#define INC_ESP8266_H_


/* APPLICATION PROGRAMMER */
#include "stm32l4xx_hal.h"


/* IMPLEMENTATION */
void initESP8266(const char *ssid, const char *password);

void initServer(void);


#endif /* INC_ESP8266_H_ */
