/**
 * CRCSCAN Generated Driver API Header File.
 * 
 * @file crcscan.h
 * 
 * @defgroup  crcscan CRCSCAN
 * 
 * @brief This file contains the API prototypes for the CRCSCAN module.
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

#ifndef CRCSCAN_H
#define CRCSCAN_H

/**
  Section: Included Files
*/

#include "../system/utils/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif 

/**
  Section: CRCSCAN APIs
*/

/**
 * @ingroup crcscan
 * @brief  Initializes the CRCSCAN module. This is called only once before calling other CRCSCAN APIs.
 * @param None.
 * @return None.
 */
int8_t CRC_Initialize(void);

/**
 * @ingroup crcscan
 * @brief  Resets the CRCSCAN module.
 * @param None.
 * @retval True - CRCSCAN module has been reset.
 * @retval False - CRCSCAN module has not been reset.
 */
bool CRC_Reset(void);

#ifdef __cplusplus
}
#endif

#endif    //CRCSCAN_H
