 /******************************************************************************
 *
 * Module: TIMER0
 *
 * File Name: timer.h
 *
 * Description: Header file for the TIMER0 AVR driver
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

/****************************************************************
 *                      Common Files                            *
 ****************************************************************/
#include"std_types.h"
#include"common_macros.h"
#include"micro_config.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum
{
	NORMAL , COMPARE
}TIMER0_Mode;

typedef enum
{
	NO_CLOCK , F__CPU , F__CPU_8 , F__CPU_64 , F__CPU_256 , F__CPU_1024
}TIMER0_Prescaler;


typedef struct
{
	TIMER0_Mode mode;
	uint8 initVal;
	TIMER0_Prescaler prescaler;
	uint8 compVal;
}TIMER0_ConfigType;

/****************************************************************
 *                      Functions Prototypes                    *
 ****************************************************************/
void TIMER0_init(const TIMER0_ConfigType * Config_Ptr);
void TIMER0_setCallBack(void(*a_ptr)(void));
void TIMER0_stop(void);
void TIMER0_setDutyCycle(uint8 a_duty);
#endif /* TIMER_H_ */
