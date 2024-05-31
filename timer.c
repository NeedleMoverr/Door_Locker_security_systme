 /******************************************************************************
 *
 * Module: TIMER0
 *
 * File Name: timer.c
 *
 * Description: Source file for the TIMER0 AVR driver
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/

#include"timer.h"
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;


/****************************************************************
 *                      ISR Definitions                         *
 ****************************************************************/
ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/****************************************************************
 *                      Functions Definitions                   *
 ****************************************************************/
void TIMER0_init(const TIMER0_ConfigType * Config_Ptr)
{
	TCNT0 = Config_Ptr->initVal; /* Timer initial value */
	/*
	 * Configure timer control register:
	 * Non PWM mode FOC0=1
	 * Normal Port Operation
	 * clock = as configured
	 */
	TCCR0 = (TCCR0 & 0xF8) | (1<<FOC0) | (Config_Ptr->prescaler);
	/*
	 * Configurations specific to Compare Mode
	 */
	if((Config_Ptr->mode))
	{
		TCCR0 |= (1<<WGM01); /* Selecting Compare Mode */
		OCR0 = Config_Ptr->compVal; /* Selecting Compare Value */
		TIMSK = (TIMSK & 0xFC) | (1<<OCIE0); /* Enable Compare Match Interrupt */
	}
	else
	{
		/* Enable Overflow (Normal) Interrupt */
		TIMSK= (TIMSK & 0xFC) | (1<<TOIE0);
	}
}

void TIMER0_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}

void TIMER0_stop(void)
{
	/*
	 * Selecting no clock i.e stopping the timer
	 */
	TCCR0 = (TCCR0 & 0xF8);
}

void TIMER0_setDutyCycle(uint8 a_duty)
{
	OCR0 = a_duty;
}
