/**
 * © 2023 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip 
 * software and any derivatives exclusively with Microchip products. 
 * It is your responsibility to comply with third party license terms 
 * applicable to your use of third party software (including open 
 * source software) that may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, 
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, 
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, 
 * PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE 
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, 
 * EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE 
 * DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, 
 * MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
 * THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU 
 * HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 *  @file    diag_wdt_startup.h
 * 	@defgroup diag_wdt_startup Watchdog Timer Startup
 *  @brief   Contains API prototypes for the Watchdog Timer start-up test.
 * 	@brief Fig. 1. Interface Diagram for the WDT Start-up Test: \n
 * 	@image docbook DIAG_WDT_Startup-interface.png width=600px
 * 	@brief Fig. 2. Sequence Diagram for the WDT Start-up Test: \n
 * 	@image docbook DIAG_WDT_Startup-sequence.png width=600px
 * 	@brief Fig. 3. Flowchart for the WDT Start-up Test: \n
 * 	@image docbook DIAG_WDT_Startup-teststate.png width=600px
 *
 * @note 
 * Microchip Technology Inc. has followed development methods required by 
 * Functional Safety Standards and performed extensive validation and static
 * testing to ensure that the code operates as intended. Any modification to the 
 * code can invalidate the results of Microchip's validation and testing.
 */

#ifndef DIAG_WDT_STARTUP_H
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_WDT_STARTUP_H

#include <stdint.h> 
#include "../../diag_common/diag_result_type.h"

/**
 * @ingroup diag_wdt_startup
 * @brief Indicates that the copy of the Reset Flag register is invalid
 */
#define DIAG_RSTFR_COPY_INVALID (0xFFU)

/**
 * @ingroup diag_wdt_startup
 * @brief Checks that the WDT is able to issue a timely system reset and 
 * that the WDT counter can be reset at start-up.
 * 
 * This diagnostic is intended to be executed at start-up, before entering the 
 * main function, as it involves multiple device resets. The diagnostic supports 
 * both the scenario where the WDT is enabled (and thus locked) through the FUSE
 * and when it is not. In the former scenario, the diagnostic execution time will 
 * depend on the time-out period chosen for the WDT, while in the latter scenario 
 * the WDT is enabled with the shortest WDT time-out period and disabled upon 
 * exiting the diagnostic.
 * 
 * This diagnostic function assumes global interrupts are disabled. 
 * 
 * Timer Counter A (TCA) is used to verify the timing of the WDT clock source
 * since the TCA clock source is independent. The timer is set up and teared down 
 * as part of running the diagnostic. It is assumed that the timer resource is free 
 * at start-up before entering main.
 * 
 * The diagnostic has the following execution flow: 
 * -# A system reset occurs.
 * -# Back up the Reset Flags register (RSTFR) and clear it.
 * -# Enable the WDT (or reset counter if enabled through FUSE).
 * -# Measure the time it takes for the WDT to issue a system reset.
 * -# Re-enter the diagnostic and check that the system reset occurred.
 * -# Check that the system reset did not occur too soon or too late (with tolerance).
 * -# Check that the WDT counter can be cleared without causing a reset.
 * -# Wait for a WDT system reset to ensure that the counter clear worked correctly.
 * -# Re-enter the diagnostic and store the result of the diagnostic.
 * -# Tear down the timer used for measurement and exit to main function.
 * -# @ref DIAG_WDT_GetResult() must be called from main to get the result of the diagnostic.
 *
 * @warning This diagnostic clears all reset flags in the RSTFR register. Use the 
 * @ref DIAG_WDT_GetRSTFRCopy() API to retrieve the copy of the register at start-up
 * to handle unexpected resets in the application. 
 * 
 * @param None.
 * @return None.
 */
 void DIAG_WDT_Startup(void);

/**
 * @ingroup diag_wdt_startup
 * @brief Returns the result of the @ref DIAG_WDT_Startup diagnostic.
 * @param None.
 * @return DIAG_PASS - The diagnostic detected no errors.
 * @return DIAG_FAIL - An error was detected.
 */
diag_result_t DIAG_WDT_GetResult(void);


/**
 * @ingroup diag_wdt_startup
 * @brief Returns the copy/backup of the RSTFR (Reset Flag Register).
 * 
 * This API should be called in main after performing the
 * @ref DIAG_WDT_Startup diagnostic to ensure that the reset reason is handled by 
 * the application. It is required because the @ref DIAG_WDT_Startup 
 * diagnostic clears the RSTFR register. 
 * @param None.
 * @return Copy of RSTFR register or @ref DIAG_RSTFR_COPY_INVALID if the backup in 
 * SRAM has been corrupted.  
 */
uint8_t DIAG_WDT_GetRSTFRCopy(void);

#endif // DIAG_WDT_STARTUP_H