 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the BUZZER AVR driver
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/

#include"buzzer.h"

/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/
//break flag
volatile uint8 BREAK = 0;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void BUZZER_on(void)
{
	BREAK = 0;
	//keep in this loop until BUZZER_off is called i.e break = 1
	while(!(BREAK))
	{
		SET_BIT(PORTD,PD6);
		_delay_ms(100);
		CLEAR_BIT(PORTD,PD6);
		_delay_ms(50);
	}
}


void BUZZER_off(void)
{
	BREAK = 1;
	CLEAR_BIT(PORTD,PD6);
}
