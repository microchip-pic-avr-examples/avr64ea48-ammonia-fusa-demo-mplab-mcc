/**
 * Generated Pins header File
 * 
 * @file pins.h
 * 
 * @defgroup  pinsdriver Pins Driver
 * 
 * @brief This is generated driver header for pins. 
 *        This header file provides APIs for all pins selected in the GUI.
 *
 * @version Driver Version  1.1.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#ifndef PINS_H_INCLUDED
#define PINS_H_INCLUDED

#include <avr/io.h>
#include "./port.h"

//get/set BUZZER aliases
#define BUZZER_SetHigh() do { PORTD_OUTSET = 0x2; } while(0)
#define BUZZER_SetLow() do { PORTD_OUTCLR = 0x2; } while(0)
#define BUZZER_Toggle() do { PORTD_OUTTGL = 0x2; } while(0)
#define BUZZER_GetValue() (VPORTD.IN & (0x1 << 1))
#define BUZZER_SetDigitalInput() do { PORTD_DIRCLR = 0x2; } while(0)
#define BUZZER_SetDigitalOutput() do { PORTD_DIRSET = 0x2; } while(0)
#define BUZZER_SetPullUp() do { PORTD_PIN1CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define BUZZER_ResetPullUp() do { PORTD_PIN1CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define BUZZER_SetInverted() do { PORTD_PIN1CTRL  |= PORT_INVEN_bm; } while(0)
#define BUZZER_ResetInverted() do { PORTD_PIN1CTRL  &= ~PORT_INVEN_bm; } while(0)
#define BUZZER_DisableInterruptOnChange() do { PORTD.PIN1CTRL = (PORTD.PIN1CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define BUZZER_EnableInterruptForBothEdges() do { PORTD.PIN1CTRL = (PORTD.PIN1CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define BUZZER_EnableInterruptForRisingEdge() do { PORTD.PIN1CTRL = (PORTD.PIN1CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define BUZZER_EnableInterruptForFallingEdge() do { PORTD.PIN1CTRL = (PORTD.PIN1CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define BUZZER_DisableDigitalInputBuffer() do { PORTD.PIN1CTRL = (PORTD.PIN1CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define BUZZER_EnableInterruptForLowLevelSensing() do { PORTD.PIN1CTRL = (PORTD.PIN1CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)
#define PD1_SetInterruptHandler BUZZER_SetInterruptHandler

//get/set UART_RX aliases
#define UART_RX_SetHigh() do { PORTC_OUTSET = 0x2; } while(0)
#define UART_RX_SetLow() do { PORTC_OUTCLR = 0x2; } while(0)
#define UART_RX_Toggle() do { PORTC_OUTTGL = 0x2; } while(0)
#define UART_RX_GetValue() (VPORTC.IN & (0x1 << 1))
#define UART_RX_SetDigitalInput() do { PORTC_DIRCLR = 0x2; } while(0)
#define UART_RX_SetDigitalOutput() do { PORTC_DIRSET = 0x2; } while(0)
#define UART_RX_SetPullUp() do { PORTC_PIN1CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define UART_RX_ResetPullUp() do { PORTC_PIN1CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define UART_RX_SetInverted() do { PORTC_PIN1CTRL  |= PORT_INVEN_bm; } while(0)
#define UART_RX_ResetInverted() do { PORTC_PIN1CTRL  &= ~PORT_INVEN_bm; } while(0)
#define UART_RX_DisableInterruptOnChange() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define UART_RX_EnableInterruptForBothEdges() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define UART_RX_EnableInterruptForRisingEdge() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define UART_RX_EnableInterruptForFallingEdge() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define UART_RX_DisableDigitalInputBuffer() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define UART_RX_EnableInterruptForLowLevelSensing() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)
#define PC1_SetInterruptHandler UART_RX_SetInterruptHandler

//get/set UART_TX aliases
#define UART_TX_SetHigh() do { PORTC_OUTSET = 0x1; } while(0)
#define UART_TX_SetLow() do { PORTC_OUTCLR = 0x1; } while(0)
#define UART_TX_Toggle() do { PORTC_OUTTGL = 0x1; } while(0)
#define UART_TX_GetValue() (VPORTC.IN & (0x1 << 0))
#define UART_TX_SetDigitalInput() do { PORTC_DIRCLR = 0x1; } while(0)
#define UART_TX_SetDigitalOutput() do { PORTC_DIRSET = 0x1; } while(0)
#define UART_TX_SetPullUp() do { PORTC_PIN0CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define UART_TX_ResetPullUp() do { PORTC_PIN0CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define UART_TX_SetInverted() do { PORTC_PIN0CTRL  |= PORT_INVEN_bm; } while(0)
#define UART_TX_ResetInverted() do { PORTC_PIN0CTRL  &= ~PORT_INVEN_bm; } while(0)
#define UART_TX_DisableInterruptOnChange() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define UART_TX_EnableInterruptForBothEdges() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define UART_TX_EnableInterruptForRisingEdge() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define UART_TX_EnableInterruptForFallingEdge() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define UART_TX_DisableDigitalInputBuffer() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define UART_TX_EnableInterruptForLowLevelSensing() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)
#define PC0_SetInterruptHandler UART_TX_SetInterruptHandler

//get/set IO_PD4 aliases
#define IO_PD4_SetHigh() do { PORTD_OUTSET = 0x10; } while(0)
#define IO_PD4_SetLow() do { PORTD_OUTCLR = 0x10; } while(0)
#define IO_PD4_Toggle() do { PORTD_OUTTGL = 0x10; } while(0)
#define IO_PD4_GetValue() (VPORTD.IN & (0x1 << 4))
#define IO_PD4_SetDigitalInput() do { PORTD_DIRCLR = 0x10; } while(0)
#define IO_PD4_SetDigitalOutput() do { PORTD_DIRSET = 0x10; } while(0)
#define IO_PD4_SetPullUp() do { PORTD_PIN4CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define IO_PD4_ResetPullUp() do { PORTD_PIN4CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define IO_PD4_SetInverted() do { PORTD_PIN4CTRL  |= PORT_INVEN_bm; } while(0)
#define IO_PD4_ResetInverted() do { PORTD_PIN4CTRL  &= ~PORT_INVEN_bm; } while(0)
#define IO_PD4_DisableInterruptOnChange() do { PORTD.PIN4CTRL = (PORTD.PIN4CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define IO_PD4_EnableInterruptForBothEdges() do { PORTD.PIN4CTRL = (PORTD.PIN4CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define IO_PD4_EnableInterruptForRisingEdge() do { PORTD.PIN4CTRL = (PORTD.PIN4CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define IO_PD4_EnableInterruptForFallingEdge() do { PORTD.PIN4CTRL = (PORTD.PIN4CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define IO_PD4_DisableDigitalInputBuffer() do { PORTD.PIN4CTRL = (PORTD.PIN4CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define IO_PD4_EnableInterruptForLowLevelSensing() do { PORTD.PIN4CTRL = (PORTD.PIN4CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)
#define PD4_SetInterruptHandler IO_PD4_SetInterruptHandler

//get/set IO_DAC_TEST aliases
#define IO_DAC_TEST_SetHigh() do { PORTD_OUTSET = 0x40; } while(0)
#define IO_DAC_TEST_SetLow() do { PORTD_OUTCLR = 0x40; } while(0)
#define IO_DAC_TEST_Toggle() do { PORTD_OUTTGL = 0x40; } while(0)
#define IO_DAC_TEST_GetValue() (VPORTD.IN & (0x1 << 6))
#define IO_DAC_TEST_SetDigitalInput() do { PORTD_DIRCLR = 0x40; } while(0)
#define IO_DAC_TEST_SetDigitalOutput() do { PORTD_DIRSET = 0x40; } while(0)
#define IO_DAC_TEST_SetPullUp() do { PORTD_PIN6CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define IO_DAC_TEST_ResetPullUp() do { PORTD_PIN6CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define IO_DAC_TEST_SetInverted() do { PORTD_PIN6CTRL  |= PORT_INVEN_bm; } while(0)
#define IO_DAC_TEST_ResetInverted() do { PORTD_PIN6CTRL  &= ~PORT_INVEN_bm; } while(0)
#define IO_DAC_TEST_DisableInterruptOnChange() do { PORTD.PIN6CTRL = (PORTD.PIN6CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define IO_DAC_TEST_EnableInterruptForBothEdges() do { PORTD.PIN6CTRL = (PORTD.PIN6CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define IO_DAC_TEST_EnableInterruptForRisingEdge() do { PORTD.PIN6CTRL = (PORTD.PIN6CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define IO_DAC_TEST_EnableInterruptForFallingEdge() do { PORTD.PIN6CTRL = (PORTD.PIN6CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define IO_DAC_TEST_DisableDigitalInputBuffer() do { PORTD.PIN6CTRL = (PORTD.PIN6CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define IO_DAC_TEST_EnableInterruptForLowLevelSensing() do { PORTD.PIN6CTRL = (PORTD.PIN6CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)
#define PD6_SetInterruptHandler IO_DAC_TEST_SetInterruptHandler

//get/set SW0 aliases
#define SW0_SetHigh() do { PORTB_OUTSET = 0x4; } while(0)
#define SW0_SetLow() do { PORTB_OUTCLR = 0x4; } while(0)
#define SW0_Toggle() do { PORTB_OUTTGL = 0x4; } while(0)
#define SW0_GetValue() (VPORTB.IN & (0x1 << 2))
#define SW0_SetDigitalInput() do { PORTB_DIRCLR = 0x4; } while(0)
#define SW0_SetDigitalOutput() do { PORTB_DIRSET = 0x4; } while(0)
#define SW0_SetPullUp() do { PORTB_PIN2CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define SW0_ResetPullUp() do { PORTB_PIN2CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define SW0_SetInverted() do { PORTB_PIN2CTRL  |= PORT_INVEN_bm; } while(0)
#define SW0_ResetInverted() do { PORTB_PIN2CTRL  &= ~PORT_INVEN_bm; } while(0)
#define SW0_DisableInterruptOnChange() do { PORTB.PIN2CTRL = (PORTB.PIN2CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define SW0_EnableInterruptForBothEdges() do { PORTB.PIN2CTRL = (PORTB.PIN2CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define SW0_EnableInterruptForRisingEdge() do { PORTB.PIN2CTRL = (PORTB.PIN2CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define SW0_EnableInterruptForFallingEdge() do { PORTB.PIN2CTRL = (PORTB.PIN2CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define SW0_DisableDigitalInputBuffer() do { PORTB.PIN2CTRL = (PORTB.PIN2CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define SW0_EnableInterruptForLowLevelSensing() do { PORTB.PIN2CTRL = (PORTB.PIN2CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)
#define PB2_SetInterruptHandler SW0_SetInterruptHandler

//get/set LED0 aliases
#define LED0_SetHigh() do { PORTB_OUTSET = 0x8; } while(0)
#define LED0_SetLow() do { PORTB_OUTCLR = 0x8; } while(0)
#define LED0_Toggle() do { PORTB_OUTTGL = 0x8; } while(0)
#define LED0_GetValue() (VPORTB.IN & (0x1 << 3))
#define LED0_SetDigitalInput() do { PORTB_DIRCLR = 0x8; } while(0)
#define LED0_SetDigitalOutput() do { PORTB_DIRSET = 0x8; } while(0)
#define LED0_SetPullUp() do { PORTB_PIN3CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define LED0_ResetPullUp() do { PORTB_PIN3CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define LED0_SetInverted() do { PORTB_PIN3CTRL  |= PORT_INVEN_bm; } while(0)
#define LED0_ResetInverted() do { PORTB_PIN3CTRL  &= ~PORT_INVEN_bm; } while(0)
#define LED0_DisableInterruptOnChange() do { PORTB.PIN3CTRL = (PORTB.PIN3CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define LED0_EnableInterruptForBothEdges() do { PORTB.PIN3CTRL = (PORTB.PIN3CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define LED0_EnableInterruptForRisingEdge() do { PORTB.PIN3CTRL = (PORTB.PIN3CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define LED0_EnableInterruptForFallingEdge() do { PORTB.PIN3CTRL = (PORTB.PIN3CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define LED0_DisableDigitalInputBuffer() do { PORTB.PIN3CTRL = (PORTB.PIN3CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define LED0_EnableInterruptForLowLevelSensing() do { PORTB.PIN3CTRL = (PORTB.PIN3CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)
#define PB3_SetInterruptHandler LED0_SetInterruptHandler

/**
 * @ingroup  pinsdriver
 * @brief GPIO and peripheral I/O initialization
 * @param none
 * @return none
 */
void PIN_MANAGER_Initialize();

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for BUZZER pin. 
 *        This is a predefined interrupt handler to be used together with the BUZZER_SetInterruptHandler() method.
 *        This handler is called every time the BUZZER ISR is executed. 
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param none
 * @return none
 */
void BUZZER_DefaultInterruptHandler(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for BUZZER pin input-sense-config functionality.
 *        Allows selecting an interrupt handler for BUZZER at application runtime
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param InterruptHandler function pointer.
 * @return none
 */
void BUZZER_SetInterruptHandler(void (* interruptHandler)(void)) ; 

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for UART_RX pin. 
 *        This is a predefined interrupt handler to be used together with the UART_RX_SetInterruptHandler() method.
 *        This handler is called every time the UART_RX ISR is executed. 
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param none
 * @return none
 */
void UART_RX_DefaultInterruptHandler(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for UART_RX pin input-sense-config functionality.
 *        Allows selecting an interrupt handler for UART_RX at application runtime
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param InterruptHandler function pointer.
 * @return none
 */
void UART_RX_SetInterruptHandler(void (* interruptHandler)(void)) ; 

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for UART_TX pin. 
 *        This is a predefined interrupt handler to be used together with the UART_TX_SetInterruptHandler() method.
 *        This handler is called every time the UART_TX ISR is executed. 
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param none
 * @return none
 */
void UART_TX_DefaultInterruptHandler(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for UART_TX pin input-sense-config functionality.
 *        Allows selecting an interrupt handler for UART_TX at application runtime
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param InterruptHandler function pointer.
 * @return none
 */
void UART_TX_SetInterruptHandler(void (* interruptHandler)(void)) ; 

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for IO_PD4 pin. 
 *        This is a predefined interrupt handler to be used together with the IO_PD4_SetInterruptHandler() method.
 *        This handler is called every time the IO_PD4 ISR is executed. 
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param none
 * @return none
 */
void IO_PD4_DefaultInterruptHandler(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for IO_PD4 pin input-sense-config functionality.
 *        Allows selecting an interrupt handler for IO_PD4 at application runtime
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param InterruptHandler function pointer.
 * @return none
 */
void IO_PD4_SetInterruptHandler(void (* interruptHandler)(void)) ; 

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for IO_DAC_TEST pin. 
 *        This is a predefined interrupt handler to be used together with the IO_DAC_TEST_SetInterruptHandler() method.
 *        This handler is called every time the IO_DAC_TEST ISR is executed. 
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param none
 * @return none
 */
void IO_DAC_TEST_DefaultInterruptHandler(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for IO_DAC_TEST pin input-sense-config functionality.
 *        Allows selecting an interrupt handler for IO_DAC_TEST at application runtime
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param InterruptHandler function pointer.
 * @return none
 */
void IO_DAC_TEST_SetInterruptHandler(void (* interruptHandler)(void)) ; 

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for SW0 pin. 
 *        This is a predefined interrupt handler to be used together with the SW0_SetInterruptHandler() method.
 *        This handler is called every time the SW0 ISR is executed. 
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param none
 * @return none
 */
void SW0_DefaultInterruptHandler(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for SW0 pin input-sense-config functionality.
 *        Allows selecting an interrupt handler for SW0 at application runtime
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param InterruptHandler function pointer.
 * @return none
 */
void SW0_SetInterruptHandler(void (* interruptHandler)(void)) ; 

/**
 * @ingroup  pinsdriver
 * @brief Default Interrupt Handler for LED0 pin. 
 *        This is a predefined interrupt handler to be used together with the LED0_SetInterruptHandler() method.
 *        This handler is called every time the LED0 ISR is executed. 
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param none
 * @return none
 */
void LED0_DefaultInterruptHandler(void);

/**
 * @ingroup  pinsdriver
 * @brief Interrupt Handler Setter for LED0 pin input-sense-config functionality.
 *        Allows selecting an interrupt handler for LED0 at application runtime
 * @pre PIN_MANAGER_Initialize() has been called at least once
 * @param InterruptHandler function pointer.
 * @return none
 */
void LED0_SetInterruptHandler(void (* interruptHandler)(void)) ; 
#endif /* PINS_H_INCLUDED */
