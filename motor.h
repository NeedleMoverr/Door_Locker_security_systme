 /******************************************************************************
 *
 * Module: MOTOR
 *
 * File Name: motor.h
 *
 * Description: Header file for the MOTOR AVR driver
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/

#ifndef MOTOR_H_
#define MOTOR_H_

/****************************************************************
 *                      Common Files                            *
 ****************************************************************/
#include"std_types.h"
#include"common_macros.h"
#include"micro_config.h"

/****************************************************************
 *                      Functions Prototypes                    *
 ****************************************************************/
void Motor_stop(void);
void Motor_rotateAntiClockWise(void);
void Motor_rotateClockWise(void);
#endif /* MOTOR_H_ */
