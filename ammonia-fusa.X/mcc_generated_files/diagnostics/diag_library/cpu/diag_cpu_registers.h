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
 * @file    diag_cpu_registers.h
 * @defgroup diag_cpu_register CPU Registers
 * @brief   Contains the API prototypes for the CPU Registers diagnostic test.
 * 
 * The following is a Unified Modeling Language (UML) interface diagram for CPU Registers: \n
 * @image docbook cpu_register-interface.png width=600px
 * The following is a UML sequence diagram for CPU Registers: \n
 * @image docbook cpu_register-sequence.png width=600px
 * @note 
 * Microchip Technology Inc. has followed development methods required by 
 * Functional Safety Standards and performed extensive validation and static
 * testing to ensure that the code operates as intended. Any modification to the 
 * code can invalidate the results of Microchip's validation and testing.
 */
 
#ifndef DIAG_CPU_REGISTERS_H
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_CPU_REGISTERS_H

#include "../../diag_common/diag_result_type.h"

/**
 @ingroup diag_cpu_register
 @brief Validates CPU registers for stuck bits and neighboring Coupling Faults.
        This diagnostic writes a checkerboard pattern and an inverse checkerboard pattern 
        to the testable bits in the CPU registers (R0-R31), Status Register (SREG) and
        the Stack Pointer (SP). If an error is detected, the registers are restored 
        before the test result is returned.

        The diagnostic is built up in the following order:
        1.  The Global interrupts status is backed up and disabled. 
        2.  The CPU register context is pushed to the stack.
        3.  The Zero Flag of the Status Register is tested to ensure that branching is working correctly. 
        4.  The R16 CPU register is tested as it is used for the rest of the test.
        5.  The remaining CPU registers are tested (R0 to R15 and R17 to R31).
        6.  The SREG is tested, except for the Global Interrupt Enable Bit.
        7.  The SP is backed up, tested and restored.
        8.  The CPU register context is restored from the stack.
        9.  Global interrupts status is restored. 
        10. The test result is returned.
 @param None.
 @return @ref DIAG_PASS - Indicates that no Fault was detected by the CPU Registers diagnosis \n
 @return @ref DIAG_FAIL - Indicates that a Fault was detected by the CPU Registers diagnosis \n
 */
diag_result_t DIAG_CPU_Registers(void);

#endif //DIAG_CPU_REGISTERS_H
