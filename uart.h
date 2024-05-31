 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/
volatile uint16 USART_g_data;

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	BAUD_RATE_100, BAUD_RATE_200 , BAUD_RATE_300,
	BAUD_RATE_1200 , BAUD_RATE_2400 , BAUD_RATE_4800,
	BAUD_RATE_9600 , BAUD_RATE_19200 , BAUD_RATE_38400 ,
	BAUD_RATE_57600 , BAUD_RATE_115200
}UART_BaudRate;

typedef enum
{
	FIVE_BITS = 0 , SIX_BITS = 1 , SEVEN_BITS = 2 , EIGHT_BITS = 3 , NINE_BITS = 3
}UART_CharacterSize;

typedef enum
{
	ONE , TWO
}UART_StopBit;

typedef enum
{
	DISABLED = 0, EVEN_PARITY = 2, ODD_PARITY = 3
}UART_ParityMode;

typedef struct
{
	UART_BaudRate rate;
	UART_CharacterSize size;
	UART_StopBit number;
	UART_ParityMode parity_mode;
}UART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void UART_init(const UART_ConfigType * Config_Ptr);

void UART_sendByte(const uint16 data);

//uint16 UART_recieveByte(void);

void UART_sendString(const uint8 *Str);

void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
