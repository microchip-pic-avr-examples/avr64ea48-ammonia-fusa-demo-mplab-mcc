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
 * @file    diag_eeprom_crc16.h
 * @defgroup diag_eeprom_crc16 EEPROM - 16-bit CRC
 * @brief   This file contains prototypes for APIs to calculate, store and validate
 *           16-bit CCITT CRC for a given EEPROM region
 * @brief Fig. 1. Unified Modeling Language (UML) Interface Diagram for EEPROM Test with CRC16: \n
 * @image docbook DIAG_EEPROM_CRC-interfaces.png width=600px
 * @brief Fig. 2. UML Sequence Diagram for EEPROM Test with CRC16: \n
 * @image docbook DIAG_EEPROM_CRC16-sequence.png width=600px
 *
 * @note 
 * Microchip Technology Inc. has followed development methods required by 
 * Functional Safety Standards and performed extensive validation and static
 * testing to ensure that the code operates as intended. Any modification to the 
 * code can invalidate the results of Microchip's validation and testing.
 */

#ifndef DIAG_EEPROM_CRC16_H
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_EEPROM_CRC16_H

#include "../../../diag_common/config/diag_config.h"

/**
 @ingroup diag_eeprom_crc16
 @def CRC_LSB_POS_16BIT
 Used for the LSB position of the CRC store/ref address.
 */
#define CRC_LSB_POS_16BIT            1U

/**
 @ingroup diag_eeprom_crc16
 @def CRC16_CCITT_POLYNOMIAL
 Used for Polynomial of the CRC16 CCITT algorithm.
 */
/* cppcheck-suppress misra-c2012-2.5 */
#define CRC16_CCITT_POLYNOMIAL       0x1021U

/**
 @ingroup diag_eeprom_crc16
 @def CRC16_INITIAL_SEED
 Used for the initial seed for the 16-bit CRC.
 */
#define CRC16_INITIAL_SEED           0xFFFFU

/**
 @ingroup diag_eeprom_crc16
 @brief Calculates 16-bit CRC for a given EEPROM memory region and stores it at
 the address indicated by storeAddress argument.
 @param [in] startAddress - Starting address of EEPROM memory region to be considered for CRC
 @param [in] length - Number of bytes from startAddress to be considered for CRC
 @param [in] storeAddress - The EEPROM address where the calculated CRC will be stored
 @return @ref DIAG_PASS - CRC for given EEPROM region is calculated and stored successfully
 @return @ref DIAG_INVALID_ARG - An invalid argument is passed to the function
 @return @ref DIAG_NVM_STORE_ERROR - An error occurred while storing the calculated CRC in EEPROM
 */
diag_result_t DIAG_EEPROM_CalculateStoreCRC(eeprom_address_t startAddress, uint16_t length, eeprom_address_t storeAddress);

/**
 @ingroup diag_eeprom_crc16
 @brief Validates the CRC of a given EEPROM memory region by re-calculating
 and comparing it with the reference CRC stored by @ref DIAG_EEPROM_CalculateStoreCRC.
 @param [in] startAddress - Starting address of EEPROM memory region to be considered for CRC
 @param [in] length - Number of bytes from startAddress to be considered for CRC
 @param [in] refAddress - The EEPROM address where reference CRC is available
 @return @ref DIAG_PASS - CRC validation for the given EEPROM region is successful \n
 @return @ref DIAG_FAIL - CRC validation for the given EEPROM region is unsuccessful \n
 @return @ref DIAG_INVALID_ARG - An invalid argument is passed to the function \n
 */
diag_result_t DIAG_EEPROM_ValidateCRC(eeprom_address_t startAddress, uint16_t length, eeprom_address_t refAddress);

#endif //DIAG_EEPROM_CRC16_H
