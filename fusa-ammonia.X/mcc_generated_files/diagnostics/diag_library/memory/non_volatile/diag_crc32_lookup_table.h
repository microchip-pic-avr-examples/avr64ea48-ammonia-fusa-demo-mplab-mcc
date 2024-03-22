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
 *  @file    diag_crc32_lookup_table.h
 *  @brief   Contains the prototype of the API to read the look-up table contents 
 *           for the 32-bit CRC algorithm.
 * 
 *  @note 
 *  Microchip Technology Inc. has followed development methods required by 
 *  Functional Safety Standards and performed extensive validation and static
 *  testing to ensure that the code operates as intended. Any modification to the 
 *  code can invalidate the results of Microchip's validation and testing.
 *
 */
 
#ifndef DIAG_CRC32_LOOKUP_TABLE_H
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_CRC32_LOOKUP_TABLE_H

uint32_t READ_DIAG_CRC32Table(uint8_t readByte);

#endif //DIAG_CRC32_LOOKUP_TABLE_H
