 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/

#include "uart.h"

#define BAUD_PRESCALE (((F_CPU / ((Config_Ptr->rate) * 8UL))) - 1)

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                      ISR Definition                                         *
 *******************************************************************************/

ISR(USART_RXC_vect)
{

	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void UART_init(const UART_ConfigType * Config_Ptr)
{
	/* U2X = 1 for double transmission speed */
	UCSRA = (1<<U2X);
	if (Config_Ptr->size == NINE_BITS)
	{
		/************************** UCSRB Description **************************
		 * RXCIE = 1 Enable USART RX Complete Interrupt Enable
		 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
		 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
		 * RXEN  = 1 Receiver Enable
		 * RXEN  = 1 Transmitter Enable
		 * UCSZ2 = 1 For 9-bit data mode
		 * RXB8 & TXB8 are used for 9-bit data mode
		 * so masking is needed
		 ***********************************************************************/
		UCSRB = (UCSRB & 0x03) | (1<<RXEN) | (1<<TXEN) | (1<<RXCIE);
	}
	else
	{
		/************************** UCSRB Description **************************
		 * RXCIE = 1 Enable USART RX Complete Interrupt Enable
		 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
		 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
		 * RXEN  = 1 Receiver Enable
		 * RXEN  = 1 Transmitter Enable
		 * UCSZ2 = 0 For 5-bit , 6-bit , 7-bit and 8-bit data mode
		 * RXB8 & TXB8 not used for 5-bit , 6-bit , 7-bit and 8-bit data mode
		 ***********************************************************************/
		UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE);
	}
	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UPM1:0  = parity_mode to choose parity bit
	 * USBS    = whatever configured
	 * UCSZ1:0 = size For any data mode
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/ 	
	UCSRC = (1<<URSEL) | ((Config_Ptr->size)<<1) | ((Config_Ptr->parity_mode)<<4) | ((Config_Ptr->number)<<3);
	
	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = BAUD_PRESCALE>>8;
	UBRRL = BAUD_PRESCALE;
}
	
void UART_sendByte(const uint16 data)
{
	/* UDRE flag is set when the Tx buffer (UDR) is empty and ready for 
	 * transmitting a new byte so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}
	/* if 9-bit mode is used, the ninth bit is put in bit TXB8 in register UCSRB
	 */
	if ((UCSRB & (1<<2)))
	{
		UCSRB = (UCSRB & 0xFE) | (data>>8);
		/* Put the required data in the UDR register and it also clear the UDRE flag as
		 * the UDR register is not empty now */
		UDR = data;
	}
	else
	{
		/* Put the required data in the UDR register and it also clear the UDRE flag as
		 * the UDR register is not empty now */
		UDR = data;
	}

}


void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}		
	*******************************************************************/
}

/*
 * Description: Function to set the Call Back function address.
 */
void UART_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

/*
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;
	Str[i] = UART_recieveByte();
	while(Str[i] != '\n')
	{
		i++;
		Str[i] = UART_recieveByte();
	}
	Str[i] = '\0';
}
*/
