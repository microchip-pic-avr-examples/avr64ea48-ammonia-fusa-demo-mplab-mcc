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
 *  @file    diag_result_type.h
 *  @brief   This file contains common type definitions for return codes
 *           in the diagnostic library.
 * 
 *  @note 
 *  Microchip Technology Inc. has followed development methods required by 
 *  Functional Safety Standards and performed extensive validation and static
 *  testing to ensure that the code operates as intended. Any modification to the 
 *  code can invalidate the results of Microchip's validation and testing.
 *
 */


#ifndef DIAG_RESULT_TYPE_H
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_RESULT_TYPE_H

/**
 @enum diag_result_t
 @brief Contains return codes for the diagnostic tests.
 @var diag_result_t:: DIAG_PASS
 0x81U - Indicates that no Fault was detected by the diagnostic \n
 @var diag_result_t:: DIAG_FAIL
 0x42U - Indicates that a Fault was detected by the diagnostic \n
 @var diag_result_t:: DIAG_INVALID_ARG
 0x24U - Indicates that the diagnostic was not run successfully due 
 to one or more invalid input arguments when calling the function \n 
 @var diag_result_t:: DIAG_UNDEFINED
 0x7EU - Indicates that the result of the diagnostic is undefined 
 due to incorrect usage of the API \n
 @var diag_result_t:: DIAG_NVM_STORE_ERROR
 0xBDU  - Indicates that the NVM controller reported an error when 
 attempting to write a value to nonvolatile memory \n

 @note The enum values are chosen with a specific bit pattern to not
 overlap with the March diagnostic data backgrounds and avoid 
 accidentally reporting an incorrect result. 
 */
typedef enum
{
    DIAG_PASS               = 0x81U, //0b10000001
    DIAG_FAIL               = 0x42U, //0b01000010
    DIAG_INVALID_ARG        = 0x24U, //0b00100100
    DIAG_UNDEFINED          = 0x7EU, //0b01111110 
    DIAG_NVM_STORE_ERROR    = 0xBDU  //0b10111101
} diag_result_t;

#endif //DIAG_RESULT_TYPE_H