/**
 * CRCSCAN Generated Driver File.
 * 
 * @file crcscan.c
 * 
 * @ingroup  crcscan
 * 
 * @brief This file contains the API implementations for the CRCSCAN module.
 *
 * @version CRCSCAN Driver Version 1.0.0
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

/**
  Section: Included Files
*/

#include "../crc.h"

/**
  Section: CRCSCAN APIs
*/

int8_t CRC_Initialize(void) 
{
    //SRC CRC on entire flash; 
    CRCSCAN.CTRLB = 0x0;

    //ENABLE enabled; NMIEN disabled; RESET disabled; 
    CRCSCAN.CTRLA = 0x1;

    return 0;
}

bool CRC_Reset(void) 
{
    if ((CRCSCAN.CTRLA & CRCSCAN_NMIEN_bm) && (CRCSCAN.STATUS & CRCSCAN_BUSY_bm)) {
        return false;
    } else {
        CRCSCAN.CTRLA |= CRCSCAN_RESET_bm;
        return true;
    }
}

ISR(NMI_vect)
{
    /* Insert your CRCSCAN interrupt handling code here */
}
