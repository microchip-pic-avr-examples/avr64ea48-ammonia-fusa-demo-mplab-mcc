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
 *  @file    diag_sram_marchc_minus.h
 *  @defgroup diag_sram_marchc_minus SW_SRAM_MARCH_TEST_01 - SRAM March Diagnostics
 *  @brief   Contains API prototypes for the SRAM March C- Test.
 * 	@brief Figure 1. Unified Modeling Language (UML) Interface Diagram for the
 *  SW_SRAM_MARCH_TEST_01 APIs, using the March C- algorithm: \n
 *  @image docbook SW_SRAM_MARCH_TEST_01_marchCm-interfaces.png width=600px
 *  @brief Figure 2. UML Sequence Diagram for using 
 * 	the Start-Up and Periodic SW_SRAM_MARCH_TEST_01 APIs with the March C- Algorithm: \n
 *  @image docbook SW_SRAM_MARCH_TEST_01_marchCm-sequence.png width=600px
 * 	@brief There is an alternative UML sequence diagram for using the 
 *  start-up SW_SRAM_MARCH_TEST_01 API and the March C- function directly on a user-defined 
 *  section of the data. The data backup and its restoration is handled by the user application. \n
 *  @brief Figure 3. Alternate UML Sequence Diagram
 *  @image docbook SW_SRAM_MARCH_TEST_01_marchCm-alternative-sequence.png width=600px
 *  @note 
 * Microchip Technology Inc. has followed development methods required by 
 * Functional Safety Standards and performed extensive validation and static
 * testing to ensure that the code operates as intended. Any modification to the 
 * code can invalidate the results of Microchip's validation and testing.
 */
 
#ifndef DIAG_SRAM_MARCHC_MINUS_H
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_SRAM_MARCHC_MINUS_H

#include <xc.h>
#include "../../../diag_common/diag_result_type.h"
#include "../../../diag_common/config/diag_config.h"

/**
 * @ingroup diag_sram_marchc_minus
 * @def DIAG_SRAM_END_ADDR
 * Defines the end address of the SRAM used by the 
 * application. It sets the stack pointer to the correct address 
 * before calling the @ref DIAG_SRAM_MarchCMinus() function in the
 * @ref SRAM_MARCH_STARTUP mode to check for valid input parameters.
 */
#define DIAG_SRAM_END_ADDR (INTERNAL_SRAM_START + DIAG_SRAM_DATA_REGION_LEN - 1U)

#if defined (__DOXYGEN__)
/**
 * @ingroup diag_sram_marchc_minus
 * @def DIAG_SRAM_MARCH_CLK_FRQ_REG
 * @brief Used to access the correct register when modifing the main clock
 * frequency in @ref DIAG_SRAM_MarchStartup(), if 
 * @ref DIAG_SRAM_MARCH_ALT_CLK_FRQ_ENABLED is not equal to zero.
 * @misradeviation{@advisory,2.5} False positive - The tool does not 
 * recognize that @ref DIAG_SRAM_MARCH_CLK_FRQ_REG 
 * is used as an argument when calling ccp_write_io() in 
 * @ref DIAG_SRAM_MarchStartup(). 
 */
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_SRAM_MARCH_CLK_FRQ_REG

/**
 * @ingroup diag_sram_marchc_minus
 * @def DIAG_SRAM_MARCH_DEFAULT_CLK_FRQ
 * @brief Used to set the main clock frequency back to default in 
 * @ref DIAG_SRAM_MarchStartup() if @ref DIAG_SRAM_MARCH_ALT_CLK_FRQ_ENABLED is not
 * equal to zero.
 * @misradeviation{@advisory,2.5} False positive - The 
 * tool does not recognize that @ref DIAG_SRAM_MARCH_DEFAULT_CLK_FRQ is used 
 * as an argument when calling ccp_write_io() in @ref DIAG_SRAM_MarchStartup().
 */
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_SRAM_MARCH_DEFAULT_CLK_FRQ

#elif defined (DIAG_AVR_Dx_DEVICE)
#define DIAG_SRAM_MARCH_CLK_FRQ_REG     (CLKCTRL.OSCHFCTRLA)
#define DIAG_SRAM_MARCH_DEFAULT_CLK_FRQ (CLKCTRL_FRQSEL_4M_gc)
#elif defined (DIAG_AVR_Ex_DEVICE)
#define DIAG_SRAM_MARCH_CLK_FRQ_REG     (CLKCTRL.MCLKCTRLB)
#define DIAG_SRAM_MARCH_DEFAULT_CLK_FRQ (CLKCTRL_PDIV_DIV6_gc | CLKCTRL_PEN_bm)
#elif defined (DIAG_AVR_TINYx_DEVICE)
#define DIAG_SRAM_MARCH_CLK_FRQ_REG     (CLKCTRL.MCLKCTRLB)
#define DIAG_SRAM_MARCH_DEFAULT_CLK_FRQ (CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm)  
#endif //defined (__DOXYGEN__)

/**
 * @ingroup diag_sram_marchc_minus 
 * @enum diag_sram_march_mode_t
 * @brief Enumeration used as input parameter to 
 * @ref DIAG_SRAM_MarchCMinus() for cases where it tests the entire SRAM at start-up, or only a part of it. 
 * @var diag_sram_march_mode_t:: SRAM_MARCH_STARTUP
 * 0x7EU - Assumes compliance with AoU-SRAM_MARCH_TEST-01 to 
 * AoU-SRAM_MARCH_TEST-06 in @ref diag_march_aou. The return address is backed up 
 * from stack and restored when the test is complete to be able to return 
 * from the function while all data in the SRAM used by the application (defined by 
 * @ref DIAG_SRAM_DATA_REGION_LEN) is overwritten.
 * @var diag_sram_march_mode_t:: SRAM_MARCH_PARTIAL
 * 0xBDU - Only a part of the SRAM is tested. In this mode, it is assumed that any 
 * backup and restoration of the data residing in the area under test is handled by
 * the caller, if required.
 */
typedef enum
{
    SRAM_MARCH_STARTUP  = 0xBDU, //0b10111101
    SRAM_MARCH_PARTIAL  = 0xDBU, //0b11011011
} diag_sram_march_mode_t;


/**
 * @ingroup diag_sram_marchc_minus
 * @brief **Software Requirement Reference ID : SW_SRAM_MARCH_TEST_01**\n
 * Implements the March C- algorithm, converted to a Word-Oriented Memory (WOM) test. 
 * See the @ref wom_conversion section for a full explanation of 
 * how the March algorithm works. See the @ref diag_march_aou section for a list of 
 * all the AoUs for this API. 
 * 
 * @warning The March test is destructive, meaning that all data stored in the 
 * SRAM area under test will be permanently overwritten when the test is completed 
 * to ensure full test coverage of the listed Coupling Faults (CFs) between all
 * bits in SRAM under test.
 * 
 * The test is executed with the following steps:
 * - If in @ref SRAM_MARCH_STARTUP mode, back up the return address from stack 
 * - Verify that the input parameters are valid
 * - Run the inter-word test 
 * - Run the intra-word test 
 * - If in @ref SRAM_MARCH_STARTUP mode, restore the return address to stack 
 * - If in @ref SRAM_MARCH_STARTUP mode, store the result of the test in a 
 *   persistent variable
 * - If in @ref SRAM_MARCH_PARTIAL mode, return the result of the test
 * 
 * If an error is detected while executing any of the March elements, the return 
 * address is restored (in @ref SRAM_MARCH_STARTUP mode), before returning as fast
 * as possible.  
 * 
 * The March C- WOM algorithm consists of the following march elements: 
 * 
 * - Start of the inter-word test:       
 *  - M0 up    (w00000000);           
 *  - M1 up    (r00000000,w11111111); 
 *  - M2 up    (r11111111,w00000000); 
 *  - M3 down  (r00000000,w11111111); 
 *  - M4 down  (r11111111,w00000000); 
 * - Start of the intra-word test:       
 *  - M5 down  (r00000000, w01010101);
 *  - M6 up    (r01010101, w10101010);
 *  - M7 down  (r10101010, w01010101);
 *  - M8 up    (r01010101, w00110011);
 *  - M9 down  (r00110011, w11001100);
 *  - M10 up   (r11001100, w00110011);
 *  - M11 down (r00110011, w00001111);
 *  - M12 up   (r00001111, w11110000);
 *  - M13 down (r11110000, w00001111);
 *  - M14 up   (r00001111);
 * 
 * @note Procedural Abstraction (PA) optimization is disabled for this 
 * function. This assembly optimization can adversely affect the March test 
 * by introducing calls to routines that change the content of the stack in SRAM 
 * and can thus cause the March test to fail. This is achieved by using the 
 * `__nopa` directive, which is expanded to `__attribute__((nopa, noinline))`.
 * This additionally ensures that inlined code is not subject to PA.
 * 
 * @param startAddress - Start address of the memory under test
 * @param length - Defines the size of the memory under test
 * @param mode - Selects which mode the function uses, as defined in 
 * @ref diag_sram_march_mode_t
 * 
 * 
 * @return @ref DIAG_PASS,
 * @return @ref DIAG_FAIL,
 * @return @ref DIAG_INVALID_ARG,
 * @return @ref DIAG_UNDEFINED.
 * 
 * @note In @ref SRAM_MARCH_STARTUP mode, discard the return value as 
 * it will always be @ref DIAG_UNDEFINED. Call @ref DIAG_SRAM_MarchGetStartupResult() 
 * in the main function to get the result of the diagnostic in 
 * Start-up mode. In @ref SRAM_MARCH_PARTIAL mode, read the return value directly.
 * 
 * @misradeviation{@advisory,8.7} False positive - The term 
 * "referenced" includes both the call of a function and its definition. A function 
 * which  is defined in one translation unit and called in a different one is
 * compliant with this rule. See the declaration of function R_8_7_2 and its usage 
 * in the R_08_07.h, R_08_07_1.c and R_08_07_1.c files in the MISRA-Example-Suite
 * repository on GitHub.
 */

/* cppcheck-suppress misra-c2012-8.7 */
diag_result_t __nopa DIAG_SRAM_MarchCMinus(register volatile uint8_t* startAddress, 
                                           register uint16_t length, 
                                           register diag_sram_march_mode_t mode);

#if defined (__DOXYGEN__)
/**
 * @ingroup diag_sram_marchc_minus
 * @brief Handles the testing of the entire SRAM used by the 
 * application, as defined by @ref DIAG_SRAM_DATA_REGION_LEN. This API wrapper is intended to be  
 * the first function call in the `.init0` section that runs before entering 
 * `main()` function.
 * 
 * The API additionally handle some of the @ref diag_march_aou by:
 * - Disabling global interrupts (no restoration)
 * - Only perform the test if the WDT reset flag is not set to ensure compatibility
 *   with the WDT diagnostic test. It is the responsibility of the system
 *   integrator to reset the flag in case of an unintended WDT reset in order to
 *   ensure that the March test is run at start-up. 
 * - Sets an alternate main clock frequency defined by 
 *   @ref DIAG_SRAM_MARCH_ALT_CLK_FRQ before running the diagnostic and restores it
 *   back to default when the test is complete, if 
 *   @ref DIAG_SRAM_MARCH_ALT_CLK_FRQ_ENABLED is not zero. 
 * - Calls @ref DIAG_SRAM_MarchCMinus() in @ref SRAM_MARCH_STARTUP mode on the 
 *   entire SRAM used by the application to get full coverage of the listed Fault
 *   Models.
 * 
 * @warning This function is NOT intended to be called from the `main()` function.
 * 
 * @note The function is declared static inline with the `always_inline` attribute 
 * to ensure that no CALL instruction is issued before executing the March test, as 
 * this would cause undefined behaviour as the function would not be able to back 
 * up the correct return address. 
 * @param None.
 * @return Call @ref DIAG_SRAM_MarchGetStartupResult() from the `main()`function 
 * to get the result of the test. 
*/
static inline void DIAG_SRAM_MarchStartup(void); 
#else
static inline void __attribute__((always_inline)) DIAG_SRAM_MarchStartup(void) 
{	
    /*
     * AoU-SRAM_MARCH_TEST-09: 
     * Only perform the test if the Watchdog reset flag is not set. 
     */
    if (RSTCTRL_WDRF_bm != (RSTCTRL.RSTFR & RSTCTRL_WDRF_bm))
    {   
        /*
         * AoU-SRAM_MARCH_TEST-07: 
         * Disable global interrupts. No backup is performed as it should be 
         * disabled by default at start-up. 
         */
        SREG &= (~CPU_I_bm); 
        
        /*
         * AoU-SRAM_MARCH_TEST-05:
         * Set the stack pointer to the last address of the SRAM used by the 
         * application. This assumes compliance with AoU-SRAM_MARCH_TEST-04.
         */
        SP = DIAG_SRAM_END_ADDR;

        /*
         * If enabled, set the alternative main clock frequency before calling 
         * the March test in order to change the execution time. 
         */  
        #if (DIAG_SRAM_MARCH_ALT_CLK_FRQ_ENABLED != 0)
        /* Assumes compliance with AoU-SRAM_MARCH_TEST-11 */
        ccp_write_io((void*) &DIAG_SRAM_MARCH_CLK_FRQ_REG, DIAG_SRAM_MARCH_ALT_CLK_FRQ);
        #endif
        
        /*
         * Ignore the return value of the March test function as it is stored in
         * a persistent variable when calling the function in Startup mode. The 
         * result of the test can be read by calling the MarchGetStartupResult
         * function in main.
         */
        (void) DIAG_SRAM_MarchCMinus((uint8_t*)INTERNAL_SRAM_START, DIAG_SRAM_DATA_REGION_LEN, SRAM_MARCH_STARTUP);
        
        /*
         * If enabled, restore the default main clock frequency before calling 
         * the March test in order to have no side effects upon returning. 
         */ 
        #if (DIAG_SRAM_MARCH_ALT_CLK_FRQ_ENABLED != 0)
        ccp_write_io((void*) &DIAG_SRAM_MARCH_CLK_FRQ_REG, DIAG_SRAM_MARCH_DEFAULT_CLK_FRQ);
        #endif
    }
}
#endif //defined (__DOXYGEN__)

/**
 * @ingroup diag_sram_marchc_minus
 * @brief Returns the result of @ref DIAG_SRAM_MarchCMinus() diagnostic when called with the 
 * @ref SRAM_MARCH_STARTUP mode as the input parameter. This API is intended to be called 
 * from the `main()` function after executing 
 * @ref DIAG_SRAM_MarchStartup() in the start-up code. 
 * @param None.  
 * @return @ref DIAG_PASS,
 * @return @ref DIAG_FAIL,
 * @return @ref DIAG_INVALID_ARG.
*/
diag_result_t  DIAG_SRAM_MarchGetStartupResult(void);


#if (DIAG_SRAM_MARCH_PERIODIC_ENABLED != 0 || defined (__DOXYGEN__))

/**
 * @ingroup diag_sram_marchc_minus
 * @def DIAG_SRAM_TEMP_STACK_SIZE
 * Determines the size of the temporary stack, which in turn is
 * used by @ref DIAG_SRAM_MarchPeriodic() when the current SRAM section under test 
 * overlaps the current stack pointer. The specific value was chosen to ensure that
 * the context of the @ref DIAG_SRAM_MarchPeriodic() function is preserved after
 * calling @ref DIAG_SRAM_MarchCMinus().
 */
#define DIAG_SRAM_TEMP_STACK_SIZE (0x10U)

/**
 * @ingroup diag_sram_marchc_minus
 * @brief Handles nondestructive periodic March testing in 
 * overlapping sections of the SRAM used by the application, defined by 
 * @ref DIAG_SRAM_DATA_REGION_LEN. This API wrapper is intended to be 
 * called periodically to test the next SRAM section as long as
 *  @ref DIAG_SRAM_MARCH_PERIODIC_ENABLED is not zero. Each time the function is called, 
 * the next SRAM section is backed up in a buffer, tested using the @ref DIAG_SRAM_MarchCMinus() 
 * function in  @ref SRAM_MARCH_PARTIAL mode and restored from the buffer before 
 * the result of the test is returned. This is done on all SRAM sections until it
 *  wraps around and starts testing the first SRAM section again. The function also handles 
 * testing the stack area by temporarily moving the stack pointer to a reserved 
 * temporary stack while performing the march test to return safely.
 * Finally, in compliance with AoU-SRAM_MARCH_TEST-07 in @ref diag_march_aou, the
 * Global Interrupt Enable bit is backed up and disabled when entering the function
 * and restored before returning.  
 * 
 * @warning The periodic March test does not give full coverage of the listed fault
 * models. To get full coverage of the listed coupling faults, i.e. detect
 * all possible coupling faults between all bits in the SRAM, the March C minus 
 * algorithm must be executed on the entire SRAM used by the application, which is 
 * done in the @ref DIAG_SRAM_MarchStartup() API. However, as the March algorithm 
 * is destructive, it is not possible to get full fault model coverage periodically
 * unless the application is reset periodically. Thus, this API implements a 
 * nondestructive periodic March test diagnostic with reduced coupling fault 
 * coverage. This approach gives full coupling fault coverage between all bits a
 * section under test, but no coverage of coupling faults between bits in a section
 * under test and the rest of the SRAM used by the application. To somewhat 
 * mitigate this issue, the sections can be configured to overlap each other so 
 * that coupling faults between the overlapping parts of two sections are covered.
 * Since coupling faults are more likely to occur on physically neighbouring bits,
 * it is deemed a sufficient solution to periodically detect SRAM faults. 
 * 
 * @note The size of each SRAM section is determined by the value of 
 * @ref DIAG_SRAM_MARCH_SEC_SIZE and the overlap between sections is given by 
 * @ref DIAG_SRAM_MARCH_SEC_OVERLAP.
 * 
 * @note The Fault Detection Time Interval (`FDTI`) is given by: 
 * `FDTI = N * T, T > t_s` 
 * where `N` is the number of SRAM sections (see @ref DIAG_SRAM_MARCH_SEC_SIZE), 
 * `T` is the period between calls to @ref DIAG_SRAM_MarchPeriodic() and `t_s` is 
 * the execution time of @ref DIAG_SRAM_MarchPeriodic() (which will depend on 
 * @ref DIAG_SRAM_MARCH_SEC_SIZE, optimization level and the main clock frequency).
 * 
 * @note The function reserves two buffers at the start of SRAM for doing backup of
 * an SRAM section and creating a temporary stack when calling the
 * @ref DIAG_SRAM_MarchCMinus() function in @ref SRAM_MARCH_PARTIAL mode when the
 * section under test overlaps the current stack. For this reason, it is required 
 * to add a linker option to move the BSS section to avoid any data collision during
 * compilation. The required linker options are printed as pragma messages during
 * compile time if @ref DIAG_LINKER_OPTION_MSG_ENABLE is not zero. 
 * 
 * @warning The periodic March test assumes that the depth of the application stack 
 * never exceeds a value so that one SRAM section will overlap both the temporary 
 * and current stack pointer. In this scenario, both stacks would be
 * under test and the March test function will not be able to return correctly or 
 * preserve the context of the caller function. Assuming that the stack starts at 
 * @ref DIAG_SRAM_END_ADDR, the maximum stack depth value, stack_max, is given by: \n
 * `tot` = @ref DIAG_SRAM_DATA_REGION_LEN \n
 * `sec` = @ref DIAG_SRAM_MARCH_SEC_SIZE \n
 * `overlap` = @ref DIAG_SRAM_MARCH_SEC_OVERLAP \n
 * `tmp` = @ref DIAG_SRAM_TEMP_STACK_SIZE \n 
 * `stack_max = tot + (sec + tmp)%(sec - overlap) - 2*sec - tmp`, 
 * where `%` is the modulo operator.
 * 
 * @note Procedural Abstraction (PA) optimization is disabled for this 
 * function. This assembly optimization can adversely affect the execution 
 * by introducing calls to routines that change the content 
 * of the stack in the SRAM, and can thus cause the backing up of the stack to fail. 
 * This is achieved by using the `__nopa` directive, 
 * which is expanded to `__attribute__((nopa, noinline))`.
 * This additionally ensures that inlined code is not subject to PA.
 * 
 * @param None. 
 * @return @ref DIAG_PASS,
 * @return @ref DIAG_FAIL,
 * @return @ref DIAG_INVALID_ARG.
*/
diag_result_t __nopa DIAG_SRAM_MarchPeriodic(void);
#endif //(DIAG_SRAM_MARCH_PERIODIC_ENABLED != 0 || defined (__DOXYGEN__))

#endif //DIAG_SRAM_MARCHC_MINUS_H
