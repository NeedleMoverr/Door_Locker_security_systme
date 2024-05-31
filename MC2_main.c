 /******************************************************************************
 *
 * Module: MC2
 *
 * File Name: MC2_main.c
 *
 * Description: Source file for the MC2 main
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/
#include"common_macros.h"
#include"micro_config.h"
#include"std_types.h"
#include"external_eeprom.h"
#include"i2c.h"
#include"motor.h"
#include"timer.h"
#include"uart.h"
#include"buzzer.h"

/*******************************************************************************
 *                                 Macros                                      *
 *******************************************************************************/
#define PASSWORD_SIZE 6
#define OVERFLOWS_PER_ONE_SEC 30
#define SLAVE_ADDRESS 0x1

/*******************************************************************************
 *                                 Global Variables                            *
 *******************************************************************************/

volatile uint8 g_state = 0 , g_counter=0 ,g_userInput[PASSWORD_SIZE] ,g_inputReceiveFlag = 0 , g_chosenOption;
volatile uint16 g_timer_counter=0;

/*******************************************************************************
 *                                 Functions Prototypes                        *
 *******************************************************************************/

uint8 passwordCheck(void);
void timer_Control(void);
void receive_Handler(void);

/*******************************************************************************
 *                                 Main Function                               *
 *******************************************************************************/

void main(void)
{
	/*
	 * two for loop counter,
	 * array to store password,
	 * variable to store number of wrong password entries,
	 * variable to record EEPROM function return status
	 */
	uint8 z=0, y=0 , firstPasswordEntry[PASSWORD_SIZE] , passwordWrongEntryCounter=0, EEPROM_Status;
	/*
	 * PORTS configuration
	 */
	/*
	 * PC6 and PC7 are motor pins
	 * and they are output pins
	 */
	SET_BIT(DDRC,PC6);
	SET_BIT(DDRC,PC7);
	/*
	 * buzzer pin
	 */
	SET_BIT(DDRD,PD6);
	/*
	 * Configuration structure for the timer:
	 * 1.Overflow mode
	 * 2.initial value=0
	 * 3.prescaler = 1024
	 * 4.compare value = 0 (not used in case of overflow mode)
	 */
	TIMER0_ConfigType TimerConfig = {OVERFLOW , 0 , F__CPU_1024 , 0};
	/*
	 * assigning call back function of the timer
	 */
	TIMER0_setCallBack(timer_Control);
	/*
	 * Configuration Structure for the UART:
	 * 1.Baud rate = 9600 kbps
	 * 2.Eight bits data mode
	 * 3.One stop bit
	 * 4.No parity bits
	 */
	UART_ConfigType UART = {BAUD_RATE_9600, EIGHT_BITS, ONE, DISABLED};
	/*
	 * UART initialization using configuration structure
	 */
	UART_init(&UART);
	/*
	 * assigning call back function of the UART
	 */
	UART_setCallBack(receive_Handler);
	/*
	 * Configuration Structure for the TWI:
	 * 1.Normal speed
	 * 2.SLAVE_ADDRESS = 0x1
	 */
	TWI_ConfigType TWI = {NORMAL , SLAVE_ADDRESS};
	/*
	 * TWI module initialization using configuration structure
	 */
	TWI_init(&TWI);
	/*
	 * EEPROM module initialization
	 */
	EEPROM_init();

	/*
	 * Enable global interrupt
	 */
	SREG |= (1<<7);
	/*
	 * In while(1) loop, the program keeps on checking which state to be in
	 */
	while(1)
	{
		/*
		 * state that waits for the user input to be received
		 */
		if((g_state == ENTER_NEW_PASSWORD) | (g_state == 0) )
		{
			/*
			 * this flag checks if the user entered the whole password or not yet
			 * when g_inputReceiveFlag = 0 this means user hasn't entered all of the 6 characters yet
			 * when g_inputReceiveFlag = 1 this means user has entered all of the 6 characters
			 */
			if(g_inputReceiveFlag == 1)
			{
				/*
				 * storing entered password in local array
				 * to compare it with the second password entry
				 */
				for(z = 0 ; z < PASSWORD_SIZE ; z++)
				{
					firstPasswordEntry[z] = g_userInput[z];
				}
				/*
				 * De-asserting receive flag
				 * in order not to enter the if condition in this state
				 * in the next iteration
				 * i.e guard condition
				 */
				g_inputReceiveFlag = 0;
				/*
				 * Transition to next state
				 */
				g_state = RE_ENTER_PASSWORD;
				/*
				 * Send the next state to MC1
				 * in order to also go to the same state of MC2
				 */
				UART_sendByte(g_state);

			}
		}
		/*
		 * state that waits for the user second input to be received
		 */
		else if(g_state == RE_ENTER_PASSWORD)
		{
			/*
			 * this flag checks if the user entered the whole password or not yet
			 * when g_inputReceiveFlag = 0 this means user hasn't entered all of the 6 characters yet
			 * when g_inputReceiveFlag = 1 this means user has entered all of the 6 characters
			 */
			if(g_inputReceiveFlag == 1)
			{
				/*
				 * This for loop checks the match between the first password entry
				 * and the second entry
				 */
				for(z = 0 ; z < PASSWORD_SIZE ; z++)
				{
					/*
					 * if no match:
					 * repeat first state
					 * and send state to MC1 for synchronization
					 */
					if(firstPasswordEntry[z] != g_userInput[z])
					{
						g_state = ENTER_NEW_PASSWORD;
						UART_sendByte(g_state);
						break;
					}
					/*
					 * this condition is true only if there is match between two passwords
					 */
					if(z==5)
					{
						/*
						 * store the password in EEPROM in six locations starting
						 * from address 0x0000
						 */
						for(y = 0 ; y < PASSWORD_SIZE ; y++)
						{
							EEPROM_Status = EEPROM_writeByte(y,firstPasswordEntry[y]);
							_delay_ms(100);
						}
					}
				}
				/*
				 * De-asserting receive flag
				 * in order not to enter the if condition in this state
				 * in the next iteration
				 * i.e guard condition
				 */
				g_inputReceiveFlag=0;
				/*
				 * if false this means that there is unmatch and it entered the if condition
				 * in the above for loop
				 * if true this means that there is match
				 */
				if(g_state != ENTER_NEW_PASSWORD)
				{
					/*
					 * change state to MAIN OPTIONS state
					 */
					g_state = MAIN_OPTIONS;
					/*
					 * inform MC1 with this transition
					 */
					UART_sendByte(g_state);
				}
			}
		}

		/*
		 * state that waits for the user to choose between:
		 * + Change password
		 * - Open Door
		 */
		else if(g_state == MAIN_OPTIONS)
		{
			/*
			 * this flag checks if the user entered the chosen option or not yet
			 * when g_inputReceiveFlag = 0 this means user hasn't entered option yet
			 * when g_inputReceiveFlag = 1 this means user has entered option
			 */
			if(g_inputReceiveFlag == 1)
			{
				switch(g_chosenOption)
				{
				case '+':
					g_state = ENTER_OLD_PASSWORD; //go to ENTER_OLD_PASSWORD state
					UART_sendByte(g_state); //inform MC1 for synchronization
					break;
				case '-':
					g_state = ENTER_YOUR_PASSWORD; //go to ENTER_YOUR_PASSWORD state
					UART_sendByte(g_state); //inform MC1 for synchronization
					break;
				}
				/*
				 * De-asserting receive flag
				 * in order not to enter the if condition in this state
				 * in the next iteration
				 * i.e guard condition
				 */
				g_inputReceiveFlag=0;
			}
		}
		/*
		 * state that waits for the user to enter OLD PASSWORD
		 */
		else if(g_state == ENTER_OLD_PASSWORD)
		{
			/*
			 * this flag checks if the user entered the whole password or not yet
			 * when g_inputReceiveFlag = 0 this means user hasn't entered all of the 6 characters yet
			 * when g_inputReceiveFlag = 1 this means user has entered all of the 6 characters
			 */
			if(g_inputReceiveFlag == 1)
			{
				/*
				 * if true this means wrong password entered
				 * if false this means right password entered
				 */
				if(!(passwordCheck()))
				{
					passwordWrongEntryCounter++; //increments by one on entry
					if(passwordWrongEntryCounter == 3) //this triggers threat detection
					{
						passwordWrongEntryCounter = 0; //to re use this counter
						g_state = ERROR_STATE; //go to ERROR_STATE
						UART_sendByte(g_state); //inform MC1 for synchronization
					}
					else
					{
						g_state = ENTER_OLD_PASSWORD; //go to ENTER_OLD_PASSWORD state
						UART_sendByte(g_state); //inform MC1 for synchronization
					}
				}
				else
				{
					/*
					 * in case wrong password entries then right entry
					 * passwordWrongEntryCounter needs to be 0 again
					 */
					passwordWrongEntryCounter = 0;
					g_state = ENTER_NEW_PASSWORD; //go to ENTER_NEW_PASSWORD state
					UART_sendByte(g_state); //inform MC1 for synchronization
				}
				/*
				 * De-asserting receive flag
				 * in order not to enter the if condition in this state
				 * in the next iteration
				 * i.e guard condition
				 */
				g_inputReceiveFlag = 0;
			}
		}
		/*
		 * state that waits for the user to enter PASSWORD
		 */
		else if(g_state == ENTER_YOUR_PASSWORD)
		{
			/*
			 * this flag checks if the user entered the whole password or not yet
			 * when g_inputReceiveFlag = 0 this means user hasn't entered all of the 6 characters yet
			 * when g_inputReceiveFlag = 1 this means user has entered all of the 6 characters
			 */
			if(g_inputReceiveFlag == 1)
			{
				/*
				 * if true this means wrong password entered
				 * if false this means right password entered
				 */
				if(!(passwordCheck()))
				{
					passwordWrongEntryCounter++; //increments by one on entry
					if(passwordWrongEntryCounter == 3) //this triggers threat detection
					{
						passwordWrongEntryCounter = 0; //to re use this counter
						g_state = ERROR_STATE; //go to ERROR_STATE
						UART_sendByte(g_state); //inform MC1 for synchronization
					}
					else
					{
						g_state = ENTER_YOUR_PASSWORD; //go to ENTER_YOUR_PASSWORD state
						UART_sendByte(g_state); //inform MC1 for synchronization
					}
				}
				else
				{
					/*
					 * in case wrong password entries then right entry
					 * passwordWrongEntryCounter needs to be 0 again
					 */
					passwordWrongEntryCounter = 0;
					g_state = OPENING; //go to OPENING state
					UART_sendByte(g_state); //inform MC1 for synchronization
				}
				/*
				 * De-asserting receive flag
				 * in order not to enter the if condition in this state
				 * in the next iteration
				 * i.e guard condition
				 */
				g_inputReceiveFlag = 0;
			}
		}
		/*
		 * state that initiates timer (15 sec) and operates motor
		 */
		else if(g_state == OPENING)
		{
			TIMER0_init(&TimerConfig); //Timer starts
			Motor_rotateClockWise(); // motor rotates clockwise (opens door)
			/*
			 * this while loop makes sure that this state is entered once at a time
			 * thus the timer won't be started many times
			 */
			while(g_state == OPENING);
		}
		/*
		 * state that stops motor
		 */
		else if(g_state == DOOR_IS_OPENED)
		{
			Motor_stop(); //stop motor door is opened
			/*
			 * state = door is opened
			 * so in the while(1) loop the program will keep entering this state
			 * until it changes, so this while loop prevents program
			 * from entering the state more than once
			 */
			while(g_state == DOOR_IS_OPENED);
		}
		/*
		 * state that closes the door
		 */
		else if(g_state == CLOSING)
		{
			Motor_rotateAntiClockWise(); //motor rotates anti clockwise
			/*
			 * state = closing
			 * so in the while(1) loop the program will keep entering this state
			 * until it changes, so this while loop prevents program
			 * from entering the state more than once
			 */
			while(g_state == CLOSING);
		}
		/*
		 * program goes to this state when password is entered wrong 3 times
		 */
		else if(g_state == ERROR_STATE)
		{
			/*
			 * starts timer
			 */
			TIMER0_init(&TimerConfig);
			/*
			 * buzzer operates
			 */
			BUZZER_on();
			/*
			 * state = ERROR_STATE
			 * so in the while(1) loop the program will keep entering this state
			 * until it changes, so this while loop prevents program
			 * from entering the state more than once
			 */
			while(g_state == ERROR_STATE);
		}
	}
}

/*******************************************************************************
 *                                 Functions Definitions                       *
 *******************************************************************************/

/*
 * Description : Function to handle the time-dependant operations
 * 	1. Counts 60 seconds in case of 3 wrong password entries and then turns off buzzer
 * 	2. Counts 15 seconds to open the door
 * 	3. Counts 3 seconds in which the motor is stopped
 * 	4. Counts 15 seconds while closing the door then stops the motor
 */
void timer_Control(void)
{
	//increments this global variable on each timer interrupt
	g_timer_counter++;
	/*
	 * checks state in which the timer is operating
	 */
	if(g_state == ERROR_STATE)
	{
		//this condition is true only after one min
		if(g_timer_counter == (OVERFLOWS_PER_ONE_SEC * 60))
		{
			TIMER0_stop();
			g_timer_counter=0; //to reuse timer again
			BUZZER_off(); //turns off buzzer after one minute
			g_state = MAIN_OPTIONS; //go to main options state
			UART_sendByte(g_state); //inform MC1 for synchronization
		}
	}
	else
	{
		//this condition is true only after 15 seconds
		if(g_timer_counter == (OVERFLOWS_PER_ONE_SEC * 15))
		{
			g_state = DOOR_IS_OPENED; //change state
			UART_sendByte(g_state); //inform MC1 for synchronization
		}
		//this condition is true only after 18 (15 clockwise + 3 stop) seconds
		else if(g_timer_counter == (OVERFLOWS_PER_ONE_SEC * 18))
		{
			g_state = CLOSING;//changes state
			UART_sendByte(g_state);//inform MC1 for synchronization
		}
		//this condition is true only after 33 (15 clockwise + 3 stop + 15 anticlockwise) seconds
		else if(g_timer_counter == (OVERFLOWS_PER_ONE_SEC * 33))
		{
			TIMER0_stop();
			g_timer_counter=0;
			Motor_stop();
			g_state = MAIN_OPTIONS;//changes state
			UART_sendByte(g_state);//inform MC1 for synchronization
		}
	}
}

/*
 * Description : Function to store the received data from MC1
 * 	1. Store chosen option from main options
 * 	2. Store entered password
 */

void receive_Handler(void)
{
	/*
	 * Checks the state in which the program operates
	 */
	if(g_state == MAIN_OPTIONS)
	{
		//store data from UART data register into global variable
		g_chosenOption = UDR;
		//raise this flag indicating complete receive
		g_inputReceiveFlag = 1;
	}
	else
	{
		//store data from UART data register into global array
		g_userInput[g_counter] = UDR ;
		g_counter++; //increments character counter
		if(g_counter == PASSWORD_SIZE)
		{
			g_counter = 0;
			//indicates complete receive
			g_inputReceiveFlag = 1;
		}
	}
}

/*
 * Description : Function to check the password match
 * 	1. Read stored password from EEPROM
 * 	2. Compare it with the entered password
 */

uint8 passwordCheck(void)
{
	/*
	 * 1.Loop counter
	 * 2.EEPROM status variable
	 * 3.Array to store password
	 */
	uint8 j, status ,storedPassword[PASSWORD_SIZE] ;
	for(j = 0 ; j < PASSWORD_SIZE ; j++)
	{
		//read stored password from EEPROM
		do
		{
			status = EEPROM_readByte(j, &storedPassword[j]);
		}while(status == ERROR);
		//if there is unmatch return ERROR
		if(g_userInput[j] != storedPassword[j])
		{
			return ERROR;
		}
	}
	//if password matches return SUCCESS
	return SUCCESS;
}
