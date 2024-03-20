/**
 *  (c) 2023 Microchip Technology Inc. and its subsidiaries.
 *
 *  Subject to your compliance with these terms, you may use Microchip software
 *  and any derivatives exclusively with Microchip products. You're responsible
 *  for complying with 3rd party license terms applicable to your use of 3rd
 *  party software (including open source software) that may accompany Microchip
 *  software.
 *
 *  SOFTWARE IS "AS IS." NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
 *  APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 *  INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 *  WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
 *  HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
 *  THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
 *  CLAIMS RELATED TO THE SOFTWARE WILL NOT EXCEED AMOUNT OF FEES, IF ANY,
 *  YOU PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 *  @file    diag_startup.c
 *  @brief   This file contains API to call on-startup diagnostics tests
 *
 */

#include "../../diag_library/memory/volatile/diag_sram_marchc_minus.h"

/**
 @def INIT0_SECTION
 This macro is used to define the attributes used to place a function in the 
 .init0 section and to not remove the function even if it is declared static 
 and not called anywhere by using the `used` attribute. 
 */
#define INIT0_SECTION __attribute__((used, __naked__, section(".init0")))

/**
 @brief This API runs at startup before `main()`. 
 All the tests which should execute before `main()`, should be called in this 
 function in the correct order.
 
 @param None
 @return None
 */
static void INIT0_SECTION DIAG_OnStartup(void)
{
    DIAG_SRAM_MarchStartup();
}
