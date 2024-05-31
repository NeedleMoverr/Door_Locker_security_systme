 /******************************************************************************
 *
 * Module: MOTOR
 *
 * File Name: motor.c
 *
 * Description: Source file for the MOTOR AVR driver
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/

#include"motor.h"

/****************************************************************
 *                      Functions Definitions                   *
 ****************************************************************/
void Motor_stop(void)
{
	CLEAR_BIT(PORTC,PC6);
	CLEAR_BIT(PORTC,PC7);
}

void Motor_rotateAntiClockWise(void)
{
	SET_BIT(PORTC,PC6);
	CLEAR_BIT(PORTC,PC7);
}

void Motor_rotateClockWise(void)
{
	CLEAR_BIT(PORTC,PC6);
	SET_BIT(PORTC,PC7);
}
