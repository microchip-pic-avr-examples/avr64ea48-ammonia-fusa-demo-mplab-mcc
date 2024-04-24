/**
 * Generated Driver File
 * 
 * @file pins.c
 * 
 * @ingroup  pinsdriver
 * 
 * @brief This is generated driver implementation for pins. 
 *        This file provides implementations for pin APIs for all pins selected in the GUI.
 *
 * @version Driver Version 1.1.0
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

#include "../pins.h"

static void (*BUZZER_InterruptHandler)(void);
static void (*UART_RX_InterruptHandler)(void);
static void (*UART_TX_InterruptHandler)(void);
static void (*AMMONIA_OUT_InterruptHandler)(void);
static void (*IO_DAC_TEST_InterruptHandler)(void);
static void (*SW0_InterruptHandler)(void);
static void (*T4OUT_InterruptHandler)(void);
static void (*T1OUT_InterruptHandler)(void);
static void (*TINT_InterruptHandler)(void);
static void (*T2OUT_InterruptHandler)(void);
static void (*T3OUT_InterruptHandler)(void);
static void (*LED0_InterruptHandler)(void);
static void (*HEATER_InterruptHandler)(void);

void PIN_MANAGER_Initialize()
{

  /* OUT Registers Initialization */
    PORTA.OUT = 0x0;
    PORTB.OUT = 0x0;
    PORTC.OUT = 0x1;
    PORTD.OUT = 0x0;
    PORTE.OUT = 0x0;
    PORTF.OUT = 0x0;

  /* DIR Registers Initialization */
    PORTA.DIR = 0x0;
    PORTB.DIR = 0x8;
    PORTC.DIR = 0x1;
    PORTD.DIR = 0x43;
    PORTE.DIR = 0x0;
    PORTF.DIR = 0x0;

  /* PINxCTRL registers Initialization */
    PORTA.PIN0CTRL = 0x0;
    PORTA.PIN1CTRL = 0x0;
    PORTA.PIN2CTRL = 0x0;
    PORTA.PIN3CTRL = 0x0;
    PORTA.PIN4CTRL = 0x0;
    PORTA.PIN5CTRL = 0x0;
    PORTA.PIN6CTRL = 0x0;
    PORTA.PIN7CTRL = 0x0;
    PORTB.PIN0CTRL = 0x0;
    PORTB.PIN1CTRL = 0x0;
    PORTB.PIN2CTRL = 0x88;
    PORTB.PIN3CTRL = 0x80;
    PORTB.PIN4CTRL = 0x0;
    PORTB.PIN5CTRL = 0x0;
    PORTB.PIN6CTRL = 0x0;
    PORTB.PIN7CTRL = 0x0;
    PORTC.PIN0CTRL = 0x0;
    PORTC.PIN1CTRL = 0x0;
    PORTC.PIN2CTRL = 0x0;
    PORTC.PIN3CTRL = 0x0;
    PORTC.PIN4CTRL = 0x0;
    PORTC.PIN5CTRL = 0x0;
    PORTC.PIN6CTRL = 0x0;
    PORTC.PIN7CTRL = 0x0;
    PORTD.PIN0CTRL = 0x0;
    PORTD.PIN1CTRL = 0x0;
    PORTD.PIN2CTRL = 0x0;
    PORTD.PIN3CTRL = 0x0;
    PORTD.PIN4CTRL = 0x4;
    PORTD.PIN5CTRL = 0x0;
    PORTD.PIN6CTRL = 0x4;
    PORTD.PIN7CTRL = 0x2;
    PORTE.PIN0CTRL = 0x0;
    PORTE.PIN1CTRL = 0x2;
    PORTE.PIN2CTRL = 0x0;
    PORTE.PIN3CTRL = 0x2;
    PORTE.PIN4CTRL = 0x0;
    PORTE.PIN5CTRL = 0x0;
    PORTE.PIN6CTRL = 0x0;
    PORTE.PIN7CTRL = 0x0;
    PORTF.PIN0CTRL = 0x0;
    PORTF.PIN1CTRL = 0x0;
    PORTF.PIN2CTRL = 0x0;
    PORTF.PIN3CTRL = 0x0;
    PORTF.PIN4CTRL = 0x0;
    PORTF.PIN5CTRL = 0x0;
    PORTF.PIN6CTRL = 0x0;
    PORTF.PIN7CTRL = 0x0;

  /* PORTMUX Initialization */
    PORTMUX.ACROUTEA = 0x0;
    PORTMUX.CCLROUTEA = 0x0;
    PORTMUX.EVSYSROUTEA = 0x0;
    PORTMUX.SPIROUTEA = 0x0;
    PORTMUX.TCAROUTEA = 0x3;
    PORTMUX.TCBROUTEA = 0x0;
    PORTMUX.TWIROUTEA = 0x0;
    PORTMUX.USARTROUTEA = 0x0;
    PORTMUX.USARTROUTEB = 0x0;

  // register default ISC callback functions at runtime; use these methods to register a custom function
    BUZZER_SetInterruptHandler(BUZZER_DefaultInterruptHandler);
    UART_RX_SetInterruptHandler(UART_RX_DefaultInterruptHandler);
    UART_TX_SetInterruptHandler(UART_TX_DefaultInterruptHandler);
    AMMONIA_OUT_SetInterruptHandler(AMMONIA_OUT_DefaultInterruptHandler);
    IO_DAC_TEST_SetInterruptHandler(IO_DAC_TEST_DefaultInterruptHandler);
    SW0_SetInterruptHandler(SW0_DefaultInterruptHandler);
    T4OUT_SetInterruptHandler(T4OUT_DefaultInterruptHandler);
    T1OUT_SetInterruptHandler(T1OUT_DefaultInterruptHandler);
    TINT_SetInterruptHandler(TINT_DefaultInterruptHandler);
    T2OUT_SetInterruptHandler(T2OUT_DefaultInterruptHandler);
    T3OUT_SetInterruptHandler(T3OUT_DefaultInterruptHandler);
    LED0_SetInterruptHandler(LED0_DefaultInterruptHandler);
    HEATER_SetInterruptHandler(HEATER_DefaultInterruptHandler);
}

/**
  Allows selecting an interrupt handler for BUZZER at application runtime
*/
void BUZZER_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    BUZZER_InterruptHandler = interruptHandler;
}

void BUZZER_DefaultInterruptHandler(void)
{
    // add your BUZZER interrupt custom code
    // or set custom function using BUZZER_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for UART_RX at application runtime
*/
void UART_RX_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    UART_RX_InterruptHandler = interruptHandler;
}

void UART_RX_DefaultInterruptHandler(void)
{
    // add your UART_RX interrupt custom code
    // or set custom function using UART_RX_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for UART_TX at application runtime
*/
void UART_TX_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    UART_TX_InterruptHandler = interruptHandler;
}

void UART_TX_DefaultInterruptHandler(void)
{
    // add your UART_TX interrupt custom code
    // or set custom function using UART_TX_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for AMMONIA_OUT at application runtime
*/
void AMMONIA_OUT_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    AMMONIA_OUT_InterruptHandler = interruptHandler;
}

void AMMONIA_OUT_DefaultInterruptHandler(void)
{
    // add your AMMONIA_OUT interrupt custom code
    // or set custom function using AMMONIA_OUT_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for IO_DAC_TEST at application runtime
*/
void IO_DAC_TEST_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    IO_DAC_TEST_InterruptHandler = interruptHandler;
}

void IO_DAC_TEST_DefaultInterruptHandler(void)
{
    // add your IO_DAC_TEST interrupt custom code
    // or set custom function using IO_DAC_TEST_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for SW0 at application runtime
*/
void SW0_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    SW0_InterruptHandler = interruptHandler;
}

void SW0_DefaultInterruptHandler(void)
{
    // add your SW0 interrupt custom code
    // or set custom function using SW0_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for T4OUT at application runtime
*/
void T4OUT_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    T4OUT_InterruptHandler = interruptHandler;
}

void T4OUT_DefaultInterruptHandler(void)
{
    // add your T4OUT interrupt custom code
    // or set custom function using T4OUT_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for T1OUT at application runtime
*/
void T1OUT_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    T1OUT_InterruptHandler = interruptHandler;
}

void T1OUT_DefaultInterruptHandler(void)
{
    // add your T1OUT interrupt custom code
    // or set custom function using T1OUT_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for TINT at application runtime
*/
void TINT_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    TINT_InterruptHandler = interruptHandler;
}

void TINT_DefaultInterruptHandler(void)
{
    // add your TINT interrupt custom code
    // or set custom function using TINT_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for T2OUT at application runtime
*/
void T2OUT_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    T2OUT_InterruptHandler = interruptHandler;
}

void T2OUT_DefaultInterruptHandler(void)
{
    // add your T2OUT interrupt custom code
    // or set custom function using T2OUT_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for T3OUT at application runtime
*/
void T3OUT_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    T3OUT_InterruptHandler = interruptHandler;
}

void T3OUT_DefaultInterruptHandler(void)
{
    // add your T3OUT interrupt custom code
    // or set custom function using T3OUT_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for LED0 at application runtime
*/
void LED0_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    LED0_InterruptHandler = interruptHandler;
}

void LED0_DefaultInterruptHandler(void)
{
    // add your LED0 interrupt custom code
    // or set custom function using LED0_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for HEATER at application runtime
*/
void HEATER_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    HEATER_InterruptHandler = interruptHandler;
}

void HEATER_DefaultInterruptHandler(void)
{
    // add your HEATER interrupt custom code
    // or set custom function using HEATER_SetInterruptHandler()
}
ISR(PORTA_PORT_vect)
{ 
    /* Clear interrupt flags */
    VPORTA.INTFLAGS = 0xff;
}

ISR(PORTB_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTB.INTFLAGS & PORT_INT2_bm)
    {
       SW0_InterruptHandler(); 
    }
    if(VPORTB.INTFLAGS & PORT_INT3_bm)
    {
       LED0_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTB.INTFLAGS = 0xff;
}

ISR(PORTC_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTC.INTFLAGS & PORT_INT1_bm)
    {
       UART_RX_InterruptHandler(); 
    }
    if(VPORTC.INTFLAGS & PORT_INT0_bm)
    {
       UART_TX_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTC.INTFLAGS = 0xff;
}

ISR(PORTD_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTD.INTFLAGS & PORT_INT1_bm)
    {
       BUZZER_InterruptHandler(); 
    }
    if(VPORTD.INTFLAGS & PORT_INT4_bm)
    {
       AMMONIA_OUT_InterruptHandler(); 
    }
    if(VPORTD.INTFLAGS & PORT_INT6_bm)
    {
       IO_DAC_TEST_InterruptHandler(); 
    }
    if(VPORTD.INTFLAGS & PORT_INT2_bm)
    {
       T4OUT_InterruptHandler(); 
    }
    if(VPORTD.INTFLAGS & PORT_INT7_bm)
    {
       T1OUT_InterruptHandler(); 
    }
    if(VPORTD.INTFLAGS & PORT_INT0_bm)
    {
       HEATER_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTD.INTFLAGS = 0xff;
}

ISR(PORTE_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTE.INTFLAGS & PORT_INT0_bm)
    {
       TINT_InterruptHandler(); 
    }
    if(VPORTE.INTFLAGS & PORT_INT1_bm)
    {
       T2OUT_InterruptHandler(); 
    }
    if(VPORTE.INTFLAGS & PORT_INT3_bm)
    {
       T3OUT_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTE.INTFLAGS = 0xff;
}

ISR(PORTF_PORT_vect)
{ 
    /* Clear interrupt flags */
    VPORTF.INTFLAGS = 0xff;
}

/**
 End of File
*/