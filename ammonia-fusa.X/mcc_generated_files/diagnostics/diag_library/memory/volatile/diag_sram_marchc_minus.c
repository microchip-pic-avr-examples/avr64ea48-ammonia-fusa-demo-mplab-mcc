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
 * @file    diag_sram_marchc_minus.c
 * @brief   Contains APIs to test SRAM using the March C- algorithm
 *
 * @ingroup diag_sram_marchc_minus
 * @note
 * Microchip Technology Inc. has followed development methods required by 
 * Functional Safety Standards and performed extensive validation and static
 * testing to ensure that the code operates as intended. Any modification to the 
 * code can invalidate the results of Microchip's validation and testing.
 */

#include "diag_sram_marchc_minus.h"


/* 
 * Check for valid config value and add a linker option message if enabled and 
 * such a linker option is required given the value of DIAG_SRAM_DATA_REGION_LEN
 */

/* cppcheck-suppress misra-c2012-20.9 */
#if (DIAG_SRAM_DATA_REGION_LEN > INTERNAL_SRAM_SIZE)
#warning DIAG_SRAM_DATA_REGION_LEN cannot exceed the physical SRAM size.

/* cppcheck-suppress misra-c2012-20.9 */
#elif ((DIAG_LINKER_OPTION_MSG_ENABLE != 0) && (DIAG_SRAM_DATA_REGION_LEN < INTERNAL_SRAM_SIZE))
DIAG_MSG("Required linker option: '--defsym=__DATA_REGION_LENGTH__=<len>' \
where <len>=", DIAG_SRAM_DATA_REGION_LEN, "[calculate sum, no brackets, no suffix]")
DIAG_MSG("Required linker option: '-Wl,--defsym=__stack=<addr>' \
where <addr>=", DIAG_SRAM_END_ADDR, "[calculate sum, no brackets, no suffix]") 
#endif

/*
 * Points to the location of the return address in stack when calling 
 * DIAG_SRAM_MarchCMinus() from DIAG_SRAM_MarchStartup() and assumes compliance 
 * with AoU-SRAM_MARCH_TEST-05. They are used to back up and restore the return 
 * address to be able to return from the function after overwriting the entire
 * SRAM including the stack when DIAG_SRAM_MarchCMinus() is called with the 
 */
#define RETURN_ADDR_STARTUP_LSB (*(volatile uint8_t*)(DIAG_SRAM_END_ADDR))
#define RETURN_ADDR_STARTUP_MSB (*(volatile uint8_t*)(DIAG_SRAM_END_ADDR - 1U))

/*
 * The result of the DIAG_SRAM_MarchCMinus diagnostic, when used as a start-up
 * test, is stored as a global variable to be able to access it in main with the
 * DIAG_SRAM_MarchGetStartupResult API. It is declared __persistent to retain the  
 * result after the WDT diagnostic resets the device.
 */
static volatile __persistent diag_result_t startupTestResult;

/**
 @misradeviation{@advisory,15.1} To adhere to misra-c2012-15.5, a goto 
 statement is used instead of having an additional return statement when 
 detecting invalid input parameters. This is done because the implementation of 
 this function requires the usage of goto statements regardless. This is because
 the diagnostic function needs to return as soon as an error is detected, without 
 having multiple return statements, and some cleanup is neccesary before exiting 
 when using the @ref SRAM_MARCH_STARTUP mode. Avoiding the use of goto would require 
 either creating a separate cleanup function, which would be called whenever an error 
 is detected (which is not possible since this function cannot call another function), 
 or having multiple additional checks which would add overhead, diminish the code 
 readability and require defaulting the test result to @ref DIAG_PASS. The latter is 
 deemed less safe than only assigning the result to @ref DIAG_PASS when all March 
 elements have run and detected no error. Finally, as misra-c2012-15.2 and misra-c2012-15.3 
 (regarding the usage of goto) is adhered to, the deviation is deemed safe.
 */
diag_result_t __nopa DIAG_SRAM_MarchCMinus(register volatile uint8_t* startAddress, register uint16_t length, register diag_sram_march_mode_t mode)
{   
    /*
     * Back up the return address for the case when mode is STARTUP.
     * Assumes compliance with AoU-SRAM_MARCH_TEST-01 to AoU-SRAM_MARCH_TEST-06
     */ 
    register uint8_t retAddrBackupLSB = RETURN_ADDR_STARTUP_LSB;
    register uint8_t retAddrBackupMSB = RETURN_ADDR_STARTUP_MSB;

    register uint8_t dataBackground = 0x0U;
    register uint8_t invDataBackground = 0xFFU;
    
    register uint16_t i;
    
    register diag_result_t testResult;

    //Verify that the input parameters are valid
    bool invalidStartAddress = (startAddress < (uint8_t*) INTERNAL_SRAM_START);
    bool invalidLength = ((&startAddress[length - 1U] > (uint8_t*) DIAG_SRAM_END_ADDR) || (length < 2U));
    bool invalidMode = ((mode != SRAM_MARCH_STARTUP) && (mode != SRAM_MARCH_PARTIAL)); 

    if (invalidStartAddress || invalidLength || invalidMode)
    {
        testResult = DIAG_INVALID_ARG;
        /* cppcheck-suppress misra-c2012-15.1 */
        goto returnInvalidStatus;
    }

    /*
     * Start of inter-word March C Minus test which cover coupling faults between
     * bits in separate words by executing March element M0 to M4
     */

    //M0 up or down (w00000000);
    for (i = 0U; i < length; i++)
    {
        startAddress[i] = dataBackground;
    }

    //M1 up (r00000000,w11111111);
    for (i = 0U; i < length; i++)
    {
        if (startAddress[i] == dataBackground)
        {
            startAddress[i] = invDataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M2 up (r11111111,w00000000);
    for (i = 0U; i < length; i++)
    {
        if (startAddress[i] == invDataBackground)
        {
            startAddress[i] = dataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M3 down (r00000000,w11111111);
    for (i = length; i > 0U; i--)
    {
        if (startAddress[i - 1U] == dataBackground)
        {
            startAddress[i - 1U] = invDataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;

        }
    }

    //M4 down (r11111111,w00000000);
    for (i = length; i > 0U; i--)
    {
        if (startAddress[i - 1U] == invDataBackground)
        {
            startAddress[i - 1U] = dataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;

        }
    }
    
    /*
     * Start of intra-word March C Minus test which cover coupling faults between
     * bits in the same word by executing March element M5 to M14
     */

    //M5 down (r00000000, w01010101);
    invDataBackground = 0x55U; //01010101
    for (i = length; i > 0U; i--)
    {
        if (startAddress[i - 1U] == dataBackground)
        {
            startAddress[i - 1U] = invDataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;

        }
    }

    //M6 up (r01010101, w10101010);
    dataBackground = 0xAAU; //10101010
    for (i = 0U; i < length; i++)
    {
        if (startAddress[i] == invDataBackground)
        {
            startAddress[i] = dataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M7 down (r10101010, w01010101);
    for (i = length; i > 0U; i--)
    {
        if (startAddress[i - 1U] == dataBackground)
        {
            startAddress[i - 1U] = invDataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M8 up (r01010101, w00110011);
    dataBackground = 0x33U; //00110011
    for (i = 0U; i < length; i++)
    {
        if (startAddress[i] == invDataBackground)
        {
            startAddress[i] = dataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M9 down(r00110011, w11001100);
    invDataBackground = 0xCCU; //11001100
    for (i = length; i > 0U; i--)
    {
        if (startAddress[i - 1U] == dataBackground)
        {
            startAddress[i - 1U] = invDataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M10 up (r11001100, w00110011);
    for (i = 0U; i < length; i++)
    {
        if (startAddress[i] == invDataBackground)
        {
            startAddress[i] = dataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M11 down(r00110011, w00001111);
    invDataBackground = 0x0FU; //00001111
    for (i = length; i > 0U; i--)
    {
        if (startAddress[i - 1U] == dataBackground)
        {
            startAddress[i - 1U] = invDataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M12 up (r00001111, w11110000); 
    dataBackground = 0xF0U; //11110000
    for (i = 0U; i < length; i++)
    {
        if (startAddress[i] == invDataBackground)
        {
            startAddress[i] = dataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M13 down(r11110000, w00001111); 
    for (i = length; i > 0U; i--)
    {
        if (startAddress[i - 1U] == dataBackground)
        {
            startAddress[i - 1U] = invDataBackground;
        }
        else
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }

    //M14 up(r00001111);
    for (i = 0U; i < length; i++)
    {
        if (startAddress[i] != invDataBackground)
        {
            testResult = DIAG_FAIL;
            /* cppcheck-suppress misra-c2012-15.1 */
            goto cleanUpAndReturn;
        }
    }
    
    testResult = DIAG_PASS;

    //goto label used if test fails
    cleanUpAndReturn:

    /*
     * In Startup mode, restore the two byte return address to stack, 
     * which is backed up in the retAddrBackup variables, before returning. 
     * This approach assumes compliance with AoU-SRAM_MARCH_TEST-01 to 
     * AoU-SRAM_MARCH_TEST-06.
     * Set the global result variable to testResult so that it can be accessed in
     * the main function and set the return value to DIAG_INVALID_ARG as the 
     * returned value is not supposed to be read when in Startup mode. 
     */
    if (SRAM_MARCH_STARTUP == mode)
    {   
        startupTestResult = testResult; 
        RETURN_ADDR_STARTUP_LSB = retAddrBackupLSB; 
        RETURN_ADDR_STARTUP_MSB = retAddrBackupMSB;
        testResult = DIAG_UNDEFINED; 
    }

    //goto label used if input arguments are invalid 
    returnInvalidStatus:

    return testResult;
}


diag_result_t DIAG_SRAM_MarchGetStartupResult(void)
{
    return startupTestResult;
}


#if (DIAG_SRAM_MARCH_PERIODIC_ENABLED != 0 || defined (__DOXYGEN__))

/*
 * Compiler specific offset that is neccesary to add to an address in data space
 * when using the __at() specifier with an SRAM address
 */
#define DATA_SPACE_OFFSET (0x800000U)

/*
 * Absolute address of the backup buffer (first address in SRAM)
 */
#define BACKUP_BUFFER_ADDR (DATA_SPACE_OFFSET + INTERNAL_SRAM_START)

/*
 * Absolute address of the temporary stack (first address after the backup 
 * buffer), and the maximum size of that stack
 */
#define TEMP_STACK_ADDR (BACKUP_BUFFER_ADDR + DIAG_SRAM_MARCH_SEC_SIZE)
 
#if (DIAG_LINKER_OPTION_MSG_ENABLE != 0)
#define BSS_ADDR (TEMP_STACK_ADDR + DIAG_SRAM_TEMP_STACK_SIZE )
DIAG_MSG("Required linker option: '-Wl, -Tbss=<addr>' \
where <addr>=", BSS_ADDR, "[calculate sum, no brackets, no suffix]") 
#endif //(DIAG_LINKER_OPTION_MSG_ENABLE != 0)

#if (DIAG_SRAM_MARCH_SEC_SIZE < 0x20U)
#warning The current value of DIAG_SRAM_MARCH_SEC_SIZE is too small and must be \
increased to a size larger than 0x20 to get full coverage due to the physical \
layout of the SRAM.
#endif

#if (DIAG_SRAM_MARCH_SEC_OVERLAP != 0U && DIAG_SRAM_MARCH_SEC_OVERLAP < 0x10U)
#warning The current value of DIAG_SRAM_MARCH_SEC_OVERLAP is too small and must \
be increased to a size larger than 0x10 to get full coverage due to the physical \
layout of the SRAM or set to 0 to have no overlap between SRAM sections. 
#endif

/* cppcheck-suppress misra-c2012-20.9 */
#if (DIAG_SRAM_MARCH_SEC_SIZE > ((DIAG_SRAM_DATA_REGION_LEN/2U) - DIAG_SRAM_TEMP_STACK_SIZE ))
#warning The current value of DIAG_SRAM_MARCH_SEC_SIZE is too large and must be \
reduced to a size smaller than \
`DIAG_SRAM_DATA_REGION_LEN/2 - DIAG_SRAM_TEMP_STACK_SIZE`  in order to not \
overwrite the temporary stack while testing the stack.
#endif


/*
 * Find the amount of data that will not overlap in each section and the amount 
 * of data in the beginning of SRAM that will not be tested twice (due to no 
 * overlap) to calculate the number of SRAM sections. The values are also used
 * to check if NUM_SRAM_SEC will be an integer for the given values of
 * DIAG_SRAM_DATA_REGION_LEN, DIAG_SRAM_MARCH_SEC_SIZE and 
 * DIAG_SRAM_MARCH_SEC_OVERLAP in diag_config.h. 
 * These values are also used to handle the copy to buffer logic when the 
 * current section under test is overlapping the buffer.
 */
#define NO_OVERLAP_SRAM_SEC_SIZE (DIAG_SRAM_MARCH_SEC_SIZE - DIAG_SRAM_MARCH_SEC_OVERLAP)
#define NO_OVERLAP_SRAM_DATA_REGION_LEN (DIAG_SRAM_DATA_REGION_LEN - DIAG_SRAM_MARCH_SEC_OVERLAP)
#define NUM_SRAM_SEC (NO_OVERLAP_SRAM_DATA_REGION_LEN / NO_OVERLAP_SRAM_SEC_SIZE)


#if(NO_OVERLAP_SRAM_SEC_SIZE > 0)
/* cppcheck-suppress misra-c2012-20.9 */
#if ((NO_OVERLAP_SRAM_DATA_REGION_LEN % NO_OVERLAP_SRAM_SEC_SIZE) != 0)
#warning The resulting number of SRAM sections is not an integer due \
to the configured value of DIAG_SRAM_MARCH_SEC_OVERLAP relative to the value of \
DIAG_SRAM_MARCH_SEC_SIZE and DIAG_SRAM_DATA_REGION_LEN.
#endif //((NO_OVERLAP_SRAM_DATA_REGION_LEN % NO_OVERLAP_SRAM_SEC_SIZE) != 0)
#else
#warning The overlap between SRAM sections is larger than the SRAM section size \
due to the configured value of DIAG_SRAM_MARCH_SEC_OVERLAP relative to the value \
of DIAG_SRAM_MARCH_SEC_SIZE. 
#endif //(NO_OVERLAP_SRAM_SEC_SIZE > 0)

#if (DIAG_LINKER_OPTION_MSG_ENABLE != 0)
DIAG_MSG("The number of SRAM sections with the current configuration is given by: ",
NUM_SRAM_SEC,"")
#endif //(DIAG_LINKER_OPTION_MSG_ENABLE != 0)


#if (DIAG_LINKER_OPTION_MSG_ENABLE != 0)
#define MAX_STACK_SIZE (DIAG_SRAM_DATA_REGION_LEN \
+ ((DIAG_SRAM_MARCH_SEC_SIZE + DIAG_SRAM_TEMP_STACK_SIZE ) % NO_OVERLAP_SRAM_SEC_SIZE) \
- (2 * DIAG_SRAM_MARCH_SEC_SIZE) - DIAG_SRAM_TEMP_STACK_SIZE )
DIAG_MSG("The maximum stack depth with the current configuration is given by: ",
MAX_STACK_SIZE,"")
#endif //(DIAG_LINKER_OPTION_MSG_ENABLE != 0)

/*
 * If the overlap is greater than 50% of the section size, the number of sections 
 * that overlap with the buffer must be calculated in order to know how much 
 * data needs to be copied from the part of the section that does not overlap 
 * the buffer to the part of the buffer that is not part of the section being 
 * tested.
 * If the overlap is equal to 0% of section size, the number of sections that 
 * overlap with the buffer is 1. 
 * Otherwise, if overlap is less than or equal to 50% of section size, 
 * the number of sections that overlap the buffer is always 2.
 */ 
#if ((DIAG_SRAM_MARCH_SEC_OVERLAP * 2U) > DIAG_SRAM_MARCH_SEC_SIZE)
#define NUM_SRAM_SEC_OVERLAP_BUFFER (DIAG_SRAM_MARCH_SEC_SIZE / NO_OVERLAP_SRAM_SEC_SIZE)
#elif (DIAG_SRAM_MARCH_SEC_OVERLAP == 0U)
#define NUM_SRAM_SEC_OVERLAP_BUFFER 1U 
#else
#define NUM_SRAM_SEC_OVERLAP_BUFFER 2U
#endif //((DIAG_SRAM_MARCH_SEC_OVERLAP * 2U) > DIAG_SRAM_MARCH_SEC_SIZE)


/**
 @misradeviation{@required,10.7} False positive - The macros 
 `NO_OVERLAP_SRAM_SEC_SIZE` and `NUM_SRAM_SEC` are a constants and the 
 expressions in question are thus not a composite expression as defined by 
 MISRA C:2012.

 @misradeviation{@required,11.4} It is necessary to deviate from this rule when
 manipulating the Stack Pointer (SP) register in this context. This is because 
 the SP register is memory mapped as a `volatile uint16_t`. To compare the stack 
 pointer address and a pointer to an address in SRAM, 
 a conversion from a pointer to an integer type is necessary. In this context, 
 the pointer is converted to an `uint16_t` because addresses in data space are 16 
 bits and it is deemed safer to compare two `uint16_t` values than two 
 pointers. The implementation has been tested thorougly with unit testing and 
 manual inspection of the memory through a debugger to verify that the SP 
 register is loaded with the intended value and that the SP is modified at the 
 appropriate time (based on the aforementioned comparison).
 */
diag_result_t __nopa DIAG_SRAM_MarchPeriodic(void)
{   

    /* Reserved memory buffer used to backup the section under test */
    static volatile uint8_t backupBuffer[DIAG_SRAM_MARCH_SEC_SIZE] __at(BACKUP_BUFFER_ADDR);

    /* Reserved memory that is used as temporary stack when testing the stack */
    static volatile uint8_t tempStack[DIAG_SRAM_TEMP_STACK_SIZE] __at(TEMP_STACK_ADDR);

    /*
     * The index of the current SRAM section being tested. It is incremented each
     * time this function is called to test the next section. When all sections 
     * have been tested, it is set back to 0. 
     */
    static volatile uint16_t sectionIndex = 0;

    /* 
     * AoU-SRAM_MARCH_TEST-07: 
     * Save global interrupt enable bit value and disable interrupts globally
     */
    register uint8_t gieStatus_bm = SREG & CPU_I_bm;
    SREG &= (~CPU_I_bm);
    
    /* Back up the sectionIndex in case its store address will be under test */
    register uint16_t currIndex = sectionIndex; 
    
    /* cppcheck-suppress misra-c2012-10.7 */
    register volatile uint8_t* sectionStartAddress = (uint8_t*)(INTERNAL_SRAM_START + (NO_OVERLAP_SRAM_SEC_SIZE * currIndex));
    
    register uint16_t numCopyValues;
    register uint16_t sectionOffset;
    
    /* 
     * Only back up values in the current section that does not overlap with the
     * buffer to the part of the buffer that does not overlap with the current 
     * section
     */
    if(currIndex < NUM_SRAM_SEC_OVERLAP_BUFFER)
    {   
        /* cppcheck-suppress misra-c2012-10.7 */
        numCopyValues = NO_OVERLAP_SRAM_SEC_SIZE * currIndex; 
        sectionOffset = DIAG_SRAM_MARCH_SEC_SIZE - numCopyValues; 
    }
    else 
    {
        numCopyValues = DIAG_SRAM_MARCH_SEC_SIZE; 
        sectionOffset = 0; 
    }
    
    /* Store the data in the section under test in the backup buffer */
    for (uint16_t i = 0U; i < numCopyValues; i++)
    {
        backupBuffer[i] = sectionStartAddress[sectionOffset + i];
    }
    
    /*
     * Back up the stack pointer (SP) for the case when a section overlaps the 
     * stack and load SP register with the start address of the temporary stack.
     * The SP will point to the last address of the reserved tempStack array 
     * since the stack grows towards decreasing memory addresses. 
     */
    register uint16_t backupStackPointer = SP;
    
    /* cppcheck-suppress misra-c2012-11.4 */
    if(((uint16_t)(sectionStartAddress) + DIAG_SRAM_MARCH_SEC_SIZE) > backupStackPointer)
    {   
        /* cppcheck-suppress misra-c2012-11.4 */
        SP = (uint16_t)(&tempStack[DIAG_SRAM_TEMP_STACK_SIZE-1U]); 
    }
    
    register diag_result_t result = DIAG_SRAM_MarchCMinus(sectionStartAddress, DIAG_SRAM_MARCH_SEC_SIZE, SRAM_MARCH_PARTIAL); 
    
    /* Restore the data in the section under test from the backup buffer */
    for (uint16_t i = 0U; i < numCopyValues; i++)
    {
        sectionStartAddress[sectionOffset + i] = backupBuffer[i];
    }
  
    /* cppcheck-suppress misra-c2012-11.4 */
    if(((uint16_t)(sectionStartAddress) + DIAG_SRAM_MARCH_SEC_SIZE) > backupStackPointer)
    {   
        /* Restore the stack pointer if the section tested overlapped stack */
        SP = backupStackPointer;
    }
    
    sectionIndex++;
    /* cppcheck-suppress misra-c2012-10.7 */
    sectionIndex %= NUM_SRAM_SEC;
       
    /* Restore global interrupt enable bit status */
    SREG |= gieStatus_bm;
   
    return result;  
}
#endif //(DIAG_SRAM_MARCH_PERIODIC_ENABLED != 0 || defined (__DOXYGEN__))

