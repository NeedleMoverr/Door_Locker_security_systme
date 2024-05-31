 /******************************************************************************
 *
 * Module: MC1 (HMI)
 *
 * File Name: FinalProject_MC1_main.c
 *
 * Description: Source file for the MC1 (HMI) main
 *
 * Author: Magdy Mohamed
 *
 *******************************************************************************/
#include"common_macros.h"
#include"micro_config.h"
#include"std_types.h"
#include"keypad.h"
#include"lcd.h"
#include"timer.h"
#include"uart.h"
/*******************************************************************************
 *                                MACROS                                       *
 *******************************************************************************/

#define PASSWORD_SIZE 6

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/* Global variable to hold the number of the state in the application */
volatile uint8 g_displayMessage;

/*******************************************************************************
 *                           Functions Prototypes                              *
 *******************************************************************************/

void setDisplayMessage(void);
void EnterNewPassword(void);
void ReEnterPassword(void);
void EnterYourPassword(void);
void EnterOldPassword(void);

/*******************************************************************************
 *                               Main Function                                 *
 *******************************************************************************/

void main(void)
{
	/*
	 * Variable to store the chosen option whether (+) change password
	 * or (-) open door
	 */
	uint8 chosenOption;
	/*
	 * Configuration Structure for the UART:
	 * 1.Baud rate = 9600 kbps
	 * 2.Eight bits data mode
	 * 3.One stop bit
	 * 4.No parity bits
	 */

	UART_ConfigType UART = {BAUD_RATE_9600, EIGHT_BITS, ONE, DISABLED};

	/*
	 * PORTS Configuration
	 */
	KEYPAD_PORT_DIR = 0; /* PORTA is input port */
	LCD_DATA_PORT_DIR = 0xF0; /*Upper pins of PORTC are output pins, lower pins are not used*/
	/*
	 * LCD pins configuration PORTD
	 */
	SET_BIT(LCD_CTRL_PORT_DIR,RS);
	SET_BIT(LCD_CTRL_PORT_DIR,RW);
	SET_BIT(LCD_CTRL_PORT_DIR,E);
	/*
	 * MODULES INITIALIZATION
	 */
	LCD_init(); //LCD initialization
	UART_init(&UART); //UART initialization using configuration structure
	/*
	 * Assigning the call back function of the UART
	 * On UART interrupt, setDisplayMessage function is called
	 */
	UART_setCallBack(setDisplayMessage);
	/*
	 * Enable global interrupt
	 */
	SREG |= (1<<7);
	/*
	 * Asking user to enter new password on first-time operation
	 */
	EnterNewPassword();
	/*
	 * In while(1) loop, the program keeps on checking which state to be in
	 */
	while(1)
	{
		/*
		 * Display "Enter new password"
		 */
		if(g_displayMessage == ENTER_NEW_PASSWORD)
		{
			EnterNewPassword();
		}
		/*
		 * Display re-enter password
		 */
		else if(g_displayMessage == RE_ENTER_PASSWORD)
		{
			ReEnterPassword();
		}
		/*
		 * Display Main Options
		 */
		else if(g_displayMessage == MAIN_OPTIONS)
		{
			/*
			 * clear screen and display + Change PW
			 *                          - Open Door
			 */
			LCD_clearScreen();
			LCD_goToRowColumn(0,0);
			LCD_displayString("+ Change PW");
			LCD_goToRowColumn(1,0);
			LCD_displayString("- Open Door  ");
			/*
			 * get chosen option from the user
			 */
			chosenOption = KeyPad_getPressedKey();
			_delay_ms(1500);
			/*
			 * display chosen option
			 */
			LCD_displayCharacter(chosenOption);
			/*
			 * send chosen option to MC2
			 */
			UART_sendByte(chosenOption);
			_delay_ms(1500);
		}
		/*
		 * Display Enter Old PW
		 */
		else if(g_displayMessage == ENTER_OLD_PASSWORD)
		{
			EnterOldPassword();
		}
		/*
		 * Display Enter Your Password
		 */
		else if(g_displayMessage == ENTER_YOUR_PASSWORD)
		{
			EnterYourPassword();
		}
		/*
		 * Display "Opening"
		 */
		else if(g_displayMessage == OPENING)
		{
			LCD_clearScreen();
			LCD_goToRowColumn(0,0);
			LCD_displayString("Opening...");
			/*
			 * keep in this state until it changes
			 * in order not to keep entering this state and keeps writing "opening"
			 * on the lcd
			 */
			while(g_displayMessage == OPENING);
		}
		/*
		 * Display "door is opened"
		 */
		else if(g_displayMessage == DOOR_IS_OPENED)
		{
			LCD_clearScreen();
			LCD_goToRowColumn(0,0);
			LCD_displayString("Door is opened.");
			/*
			 * keep in this state until it changes
			 * in order not to keep entering this state and keeps writing "door is opened"
			 * on the lcd
			 */
			while(g_displayMessage == DOOR_IS_OPENED);
		}
		/*
		 * display "closing"
		 */
		else if(g_displayMessage == CLOSING)
		{
			LCD_clearScreen();
			LCD_goToRowColumn(0,0);
			LCD_displayString("Closing...");
			/*
			 * keep in this state until it changes
			 * in order not to keep entering this state and keeps writing "closing"
			 * on the lcd
			 */
			while(g_displayMessage == CLOSING);
		}
		/*
		 * DISPLAY "ERROR!!"
		 */
		else if(g_displayMessage == ERROR_STATE)
		{
			LCD_clearScreen();
			LCD_goToRowColumn(0,0);
			LCD_displayString("ERROR!!");
			/*
			 * keep in this state until it changes
			 * in order not to keep entering this state and keeps writing "ERROR!!"
			 * on the lcd
			 */
			while(g_displayMessage == ERROR_STATE);
		}

	}
}


/*******************************************************************************
 *                           Functions Definitions                             *
 *******************************************************************************/

/*
 * Description : Function to handle new password entry
 * 	1. Reads password from user
 * 	2. Displays '*'
 * 	3. Sends entered password to MC2
 */

void EnterNewPassword(void)
{
	/*
	 * Array to store entered password
	 * and counter for the for loop
	 */
	uint8 password[PASSWORD_SIZE],i;
	/*
	 * Display "Enter new password"
	 */
	LCD_clearScreen();
	LCD_goToRowColumn(0,0);
	LCD_displayString("Enter New PW :");
	LCD_goToRowColumn(1,0);
	/*
	 * get number of characters from user equal to PASSWORD_SIZE
	 */
	for(i = 0 ; i < PASSWORD_SIZE ; i++)
	{
		password[i] = KeyPad_getPressedKey(); //get input from user number by number
		LCD_displayCharacter('*'); //display * for each entered character
		UART_sendByte(password[i]); //send entered password to MC2
		_delay_ms(4000);
	}

}

/*
 * Description : Function to handle second password entry
 * 1. Reads password second entry from user
 * 2. Displays '*'
 * 3. Sends entered password to MC2
 */

void ReEnterPassword(void)
{
	/*
	 * Array to store entered password
	 * and counter for the for loop
	 */
	uint8 password[PASSWORD_SIZE],i;
	/*
	 * Display "Re-Enter new password"
	 */
	LCD_clearScreen();
	LCD_displayString("Re-enter PW :");
	LCD_goToRowColumn(1,0);
	/*
	 * get number of characters from user equal to PASSWORD_SIZE
	 */
	for(i = 0 ; i < PASSWORD_SIZE ; i++)
	{
		password[i] = KeyPad_getPressedKey(); //get input from user number by number
		LCD_displayCharacter('*'); //display * for each entered character
		UART_sendByte(password[i]); //send entered password to MC2
		_delay_ms(4000);
	}
}

/*
 * Description : Function to handle password entry
 * 1. Reads password entry from user
 * 2. Displays '*'
 * 3. Sends entered password to MC2
 */


void EnterYourPassword(void)
{
	/*
	 * counter for the for loop
	 */
	uint8 i;
	/*
	 * Display "Enter your password"
	 */
	LCD_clearScreen();
	LCD_goToRowColumn(0,0);
	LCD_displayString("Enter Your PW :");
	LCD_goToRowColumn(1,0);
	/*
	 * get number of characters from user equal to PASSWORD_SIZE
	 */
	for(i = 0 ; i < PASSWORD_SIZE ; i++)
	{
		UART_sendByte(KeyPad_getPressedKey()); //send entered password to MC2
		LCD_displayCharacter('*'); //display * for each entered character
		_delay_ms(4000);
	}
}

/*
 * Description : Function to handle old password entry
 * 1. Reads old password entry from user
 * 2. Displays '*'
 * 3. Sends entered password to MC2
 */


void EnterOldPassword(void)
{
	/*
	 * counter for the for loop
	 */
	uint8 i;
	/*
	 * Display "Enter OLD password"
	 */
	LCD_clearScreen();
	LCD_goToRowColumn(0,0);
	LCD_displayString("Enter Old PW :");
	LCD_goToRowColumn(1,0);
	/*
	 * get number of characters from user equal to PASSWORD_SIZE
	 */
	for(i = 0 ; i < PASSWORD_SIZE ; i++)
	{
		UART_sendByte(KeyPad_getPressedKey()); //send entered password to MC2
		LCD_displayCharacter('*'); //display * for each entered character
		_delay_ms(4000);
	}
}

/*
 * Description : Function to update the current state of the program
 * 1. Reads data from UART data register
 * 2. Updates current state and display different message on LCD
 */


void setDisplayMessage(void)
{
	/*
	 * MC1 state equals that received from MC2 through UART
	 * so here it gets the value of the data register of UART
	 */
	g_displayMessage = UDR;
}
