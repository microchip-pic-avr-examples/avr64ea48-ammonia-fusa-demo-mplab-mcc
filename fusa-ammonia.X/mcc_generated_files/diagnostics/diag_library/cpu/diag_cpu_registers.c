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
 * @file    diag_cpu_registers.c
 * @ingroup diag_cpu_register 
 * @brief   Contains the API implementation for the CPU Registers diagnostic test.
 * @note 
 * Microchip Technology Inc. has followed development methods required by 
 * Functional Safety Standards and performed extensive validation and static
 * testing to ensure that the code operates as intended. Any modification to the 
 * code can invalidate the results of Microchip's validation and testing.
 */

#include "diag_cpu_registers.h"
#include <xc.h>

/* Checkerboard patterns used to detect stuck bits */
#define CHECKERBOARD_PATTERN        (0xAAU) //0b10101010
#define CHECKERBOARD_PATTERN_INV    (0x55U) //0b01010101

/* Checkerboard pattern for the SREG excluding the Global Interrupt Enabled bit*/
#define SREG_PATTERN (CHECKERBOARD_PATTERN & (~CPU_I_bm))
#define SREG_PATTERN_INV (CHECKERBOARD_PATTERN_INV & (~CPU_I_bm))

/* Find the 8 most significant bits of the SRAM start address */
#define INTERNAL_SRAM_START_8MSB (INTERNAL_SRAM_START >> 8U) 

/* Find the 8 most significant bits of the SRAM end address */
#define INTERNAL_SRAM_END_8MSB (INTERNAL_SRAM_END >> 8U) 

/*
 * Subtract the MSBs of the end address from the start address to find which bits 
 * are configurable in the SPH register, since some bits will always be 0 or 1 
 * (i.e. non-configurable) depending on the SRAM location in the memory map
 */
#define SPH_CONFIGURABLE_BITS_BM (INTERNAL_SRAM_END_8MSB - INTERNAL_SRAM_START_8MSB) 

/*
 * Applies a checkerboard bit-pattern (0xAA) to the configurable bits of the SPH 
 * register. Because the configurable bits of SPH are dependent on the size of 
 * SRAM, it is necessary to add the start address to compensate for any 
 * non-configurable bits when reading and writing to the SPH register. 
 */
#define SPH_PATTERN ((SPH_CONFIGURABLE_BITS_BM & CHECKERBOARD_PATTERN) + INTERNAL_SRAM_START_8MSB)

/*
 * Applies the inverse checkerboard bit-pattern (0x55) to the configurable bits 
 * of the SPH register. Because the configurable bits of SPH are dependent on the
 * size of SRAM, it is necessary to add the start address to compensate for any 
 * non-configurable bits when reading and writing to the SPH register. 
 */
#define SPH_PATTERN_INV ((SPH_CONFIGURABLE_BITS_BM & CHECKERBOARD_PATTERN_INV) + INTERNAL_SRAM_START_8MSB)

/* Find the 8 least significant bits of the SRAM start address */
#define INTERNAL_SRAM_START_8LSB (INTERNAL_SRAM_START & 0xFFU) 

/* Find the 8 least significant bits of the SRAM end address */
#define INTERNAL_SRAM_END_8LSB (INTERNAL_SRAM_END & 0xFFU)

/*
 * Subtract the MSBs of the end address from the start address to find which bits 
 * are configurable in the SPL register, since some bits may always be 0 or 1 
 * (i.e. non-configurable) depending on the SRAM location in the memory map
 */
#define SPL_CONFIGURABLE_BITS_BM (INTERNAL_SRAM_END_8LSB - INTERNAL_SRAM_START_8LSB)

/*
 * Applies a checkerboard bit-pattern (0xAA) to the configurable bits of the SPL 
 * register. Because the configurable bits of SPL are dependent on the size of 
 * SRAM, it is necessary to add the start address to compensate for any 
 * non-configurable bits when reading and writing to the SPL register. 
 */ 
#define SPL_PATTERN ((SPL_CONFIGURABLE_BITS_BM & CHECKERBOARD_PATTERN) + INTERNAL_SRAM_START_8LSB)

/*
 * Applies the inverse checkerboard bit-pattern (0x55) to the configurable bits 
 * of the SPL register. Because the configurable bits of SPL are dependent on the
 * size of SRAM, it is necessary to add the start address to compensate for any 
 * non-configurable bits when reading and writing to the SPL register.
 */ 
#define SPL_PATTERN_INV ((SPL_CONFIGURABLE_BITS_BM & CHECKERBOARD_PATTERN_INV) + INTERNAL_SRAM_START_8LSB)


/*
 * This C Macro function uses extended inline assembly inside a macro function 
 * to load an immediate value from a C macro constant to the CPU register R16.
 */
#define LDI_R16_MACRO_VALUE(value) \
 __asm__("ldi  r16, %[val] \n\t" \
          : /* No output operands */\
          : [val] "i" ((uint8_t)(value))) // Substitute val with 'i'mmediate integer constant

/*
 * This C Macro function uses extended inline assembly inside a macro function 
 * to compare a C macro value with a CPU register R16.
 */
#define CPI_R16_MACRO_VALUE(value) \
 __asm__("cpi  r16, %[val] \n\t" \
          : /* No output operands */\
          : [val] "i" ((uint8_t)(value))) // Substitute val with 'i'mmediate integer constant

/* 
 * The following function is forced to compile with zero optimization (-O0) to 
 * prevent the compiler from assuming values loaded in CPU registers at the 
 * beginning of the function will remain there until it decides to overwrite them.
 * Since the compiler cannot know about inline assembly instructions overwriting 
 * registers, when it sees a write to the same return variable, testResult, it
 * will reuse the register with the already loaded value the second time around
 * on higher optimization levels. However, this would result in the diagnostic 
 * always returning DIAG_FAIL since the function pushes all the registers to 
 * stack and overwrites them during execution of the test. 
 */
diag_result_t __attribute__((optimize("-O0"))) DIAG_CPU_Registers(void)
{   
    /* 
     * Declare the return variable volatile to ensure that the compiler actually 
     * creates writes to the variable in SRAM and static to avoid creating a 
     * local variable stored in registers that is overwritten with the register
     * test below.
     */
    static volatile diag_result_t testResult;
    
    /* Save global interrupt enable bit value for restoration */
    volatile uint8_t gieStatus_bm = SREG & CPU_I_bm;

    /* Disable global interrupts during this test */ 
    SREG &= (~CPU_I_bm);

    /* 
     * Assume the test will fail and only set to it to pass if after completion
     * of the diagnostic tests.
     */ 
    testResult = DIAG_FAIL;

    /* Push register context to stack */
    asm("push r0");
    asm("push r1");
    asm("push r2");
    asm("push r3");
    asm("push r4");
    asm("push r5");
    asm("push r6");
    asm("push r7");
    asm("push r8");
    asm("push r9");
    asm("push r10");
    asm("push r11");
    asm("push r12");
    asm("push r13");
    asm("push r14");
    asm("push r15");
    asm("push r16");
    asm("push r17");
    asm("push r18");
    asm("push r19");
    asm("push r20");
    asm("push r21");
    asm("push r22");
    asm("push r23");
    asm("push r24");
    asm("push r25");
    asm("push r26");
    asm("push r27");
    asm("push r28");
    asm("push r29");
    asm("push r30");
    asm("push r31");
    
    /* 
     * Test Status Register (SREG) Zero Flag (Z-bit). 
     * This is necessary since the rest of the tests use branching that depend on
     * the Zero Flag working correctly to report faults. This is achieved by 
     * using R16 to set the Zero Flag bit in SREG to 0 and call a branch 
     * instruction without a compare instruction. If the Zero Flag bit is 
     * stuck-at 1, the branching instruction is run and and the function returns.
     * Next, the Zero Flag is set to 1 and call a branch instruction without 
     * compare, if the Zero Flag is stuck-at 0, the branching instruction is run 
     * and the and the function returns. This assumes that R16 is free from 
     * faults, as this diagnostic can only test for single point faults. 
     */ 
    LDI_R16_MACRO_VALUE(0U);
    asm("out __SREG__, r16");   //Write 0 to the Zero Flag bit in SREG
    asm("breq jmp_to_return");  //Check if Zero Flag is stuck-at 1

    LDI_R16_MACRO_VALUE(CPU_Z_bm);
    asm("out __SREG__, r16");   //Write 1 to the Zero Flag bit in SREG
    asm("brne jmp_to_return");  //Check if Zero Flag is stuck-at 0

    
    /* 
     * Test R16 with Checkerboard patterns. 
     * R16 is chosen over R0 - R15 to enable the usage of Compare Immediate (CPI)
     * instructions and so that R16 can be used in the rest of the tests later. 
     */
    LDI_R16_MACRO_VALUE(CHECKERBOARD_PATTERN_INV);
    CPI_R16_MACRO_VALUE(CHECKERBOARD_PATTERN_INV);
    asm("brne jmp_to_return");
    LDI_R16_MACRO_VALUE(CHECKERBOARD_PATTERN);
    CPI_R16_MACRO_VALUE(CHECKERBOARD_PATTERN);
    asm("brne jmp_to_return");
    
    
    /* 
     * Test remaining CPU registers using R16 to write a checkerboard and 
     * inverse checkerboard pattern. R16 already contains CHECKERBOARD_PATTERN
     * from above.  
     */
    asm("loop: ");
    /* Load pattern into R17 use the Copy Register Word (MOVW) instruction */
    asm("mov r17, r16"); 
    
    asm("movw r0, r16");
    asm("movw r2, r16");
    asm("movw r4, r16");
    asm("movw r6, r16");
    asm("movw r8, r16");
    asm("movw r10, r16");
    asm("movw r12, r16");
    asm("movw r14, r16");
    /* Skip R16 + R17 as they already contain the pattern */
    asm("movw r18, r16");
    asm("movw r20, r16");
    asm("movw r22, r16");
    asm("movw r24, r16");
    asm("movw r26, r16");
    asm("movw r28, r16");
    asm("movw r30, r16");
    
    /* Read back checkerboard pattern on all registers using R16 */
    asm("cp r0,r16");
    asm("cpc r1,r16");
    asm("cpc r2,r16");
    asm("cpc r3,r16");
    asm("cpc r4,r16");
    asm("cpc r5,r16");
    asm("cpc r6,r16");
    asm("cpc r7,r16");
    asm("cpc r8,r16");
    asm("cpc r9,r16");
    asm("cpc r10,r16");
    asm("cpc r11,r16");
    asm("cpc r12,r16");
    asm("cpc r13,r16");
    asm("cpc r14,r16");
    asm("cpc r15,r16");
    /* Skip R16 as it is used for the compare */
    asm("cpc r17,r16");
    asm("cpc r18,r16");
    asm("cpc r19,r16");
    asm("cpc r20,r16");
    asm("cpc r21,r16");
    asm("cpc r22,r16");
    asm("cpc r23,r16");
    asm("cpc r24,r16");
    asm("cpc r25,r16");
    asm("cpc r26,r16");
    asm("cpc r27,r16");
    asm("cpc r28,r16");
    asm("cpc r29,r16");
    asm("cpc r30,r16");
    asm("cpc r31,r16");
    asm("brne jmp_to_return");
    
    asm("inc r16");                 //Increment R16 for loop logic
    CPI_R16_MACRO_VALUE((CHECKERBOARD_PATTERN_INV + 1U));
    asm("breq test_sreg");          // Go to next test  
    LDI_R16_MACRO_VALUE(CHECKERBOARD_PATTERN_INV);
    asm("rjmp loop"); // Repeat test with inverse checkerboard pattern
    
    
     /* 
     * Fault handler for jumping to return since the operand of BRNE and BREQ
     * instructions are limited to increasing the program counter by max 63. 
     */
    asm("jmp_to_return: ");
    asm("rjmp return");
    
    
    /* Test the Status Register (SREG) with checkerboard patterns. */ 
    asm("test_sreg: ");
    LDI_R16_MACRO_VALUE(SREG_PATTERN);
    asm("out __SREG__, r16");   //write pattern to SREG
    asm("in r16, __SREG__");    //Read back pattern from SREG
    CPI_R16_MACRO_VALUE(SREG_PATTERN);
    asm("brne return");

    LDI_R16_MACRO_VALUE(SREG_PATTERN_INV);
    asm("out __SREG__, r16");   //Write inverse pattern to SREG
    asm("in r16, __SREG__");    //Read back inverse pattern from SREG
    CPI_R16_MACRO_VALUE(SREG_PATTERN_INV);
    asm("brne return");
    

    /* Test the Stack Pointer (SP) */
    asm("in r0, __SP_L__"); // Backup Stack Pointer Low (SPL) value
    asm("in r1, __SP_H__"); // Backup Stack Pointer High (SPH) value
    
    /* Write pattern to SPL and SPH */
    LDI_R16_MACRO_VALUE(SPL_PATTERN);
    asm("out __SP_L__, r16");
    LDI_R16_MACRO_VALUE(SPH_PATTERN);
    asm("out __SP_H__, r16");
    
    /* Compare SPL with pattern */
    asm("in r16, __SP_L__");
    CPI_R16_MACRO_VALUE(SPL_PATTERN);
    asm("brne restore_sp_and_return");
    
    /* Compare SPH with pattern */
    asm("in r16, __SP_H__");
    CPI_R16_MACRO_VALUE(SPH_PATTERN);
    asm("brne restore_sp_and_return");
    
    /* Write inverse pattern to SPL and SPH */
    LDI_R16_MACRO_VALUE(SPL_PATTERN_INV);
    asm("out __SP_L__, r16");
    LDI_R16_MACRO_VALUE(SPH_PATTERN_INV);
    asm("out __SP_H__, r16"); 
    
    /* Compare SPL with inverse pattern */
    asm("in r16, __SP_L__");
    CPI_R16_MACRO_VALUE(SPL_PATTERN_INV);
    asm("brne restore_sp_and_return");
    
    /* Compare SPH with inverse pattern */
    asm("in r16, __SP_H__");
    CPI_R16_MACRO_VALUE(SPH_PATTERN_INV);
    asm("brne restore_sp_and_return");
    
    
    /* Reaching this point indicates that all tests have passed */
    testResult = DIAG_PASS;
    
    
    /* Restore the SP */
    asm("restore_sp_and_return: ");
    asm("out __SP_L__, r0");                      
    asm("out __SP_H__, r1");
    
    /* Restore register context from stack */
    asm("return: ");
    asm("pop r31");
    asm("pop r30");
    asm("pop r29");
    asm("pop r28");
    asm("pop r27");
    asm("pop r26");
    asm("pop r25");
    asm("pop r24");
    asm("pop r23");
    asm("pop r22");
    asm("pop r21");
    asm("pop r20");
    asm("pop r19");
    asm("pop r18");
    asm("pop r17");
    asm("pop r16");
    asm("pop r15");
    asm("pop r14");
    asm("pop r13");
    asm("pop r12");
    asm("pop r11");
    asm("pop r10");
    asm("pop r9");
    asm("pop r8");
    asm("pop r7");
    asm("pop r6");
    asm("pop r5");
    asm("pop r4");
    asm("pop r3");
    asm("pop r2");
    asm("pop r1");
    asm("pop r0");

    /* Restore global interrupt enable bit status */
    SREG |= gieStatus_bm;

	return testResult;
}