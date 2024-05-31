 /******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Source file for the BUZZER AVR driver
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void BUZZER_on(void);
void BUZZER_off(void);

#endif /* BUZZER_H_ */
