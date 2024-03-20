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
 * @file    diag_eeprom_crc16_lookup.c
 * @brief   Contains the implementation of the APIs to test EEPROM using the
 *          CRC16 CCITT Look-up Table algorithm
 * @ingroup diag_eeprom_crc16
 * @note 
 * Microchip Technology Inc. has followed development methods required by 
 * Functional Safety Standards and performed extensive validation and static
 * testing to ensure that the code operates as intended. Any modification to the 
 * code can invalidate the results of Microchip's validation and testing.
 */

#include "diag_eeprom_crc16.h"
#include "diag_crc16_lookup_table.h"
#include <string.h>

static void DIAG_EEPROM_CalculateCRC(eeprom_address_t startAddress, uint16_t length, uint16_t *crcSeed)
{
    uint16_t i;
    uint8_t readByte;

    for (i = 0U; i < length; i++)
    {
        readByte = EEPROM_Read(startAddress + i);
        readByte ^= *crcSeed >> 8U;
        *crcSeed = READ_DIAG_CRC16Table(readByte) ^ (*crcSeed << 8U);
    }
}

diag_result_t DIAG_EEPROM_CalculateStoreCRC(eeprom_address_t startAddress, uint16_t length, eeprom_address_t storeAddress)
{
    diag_result_t testStatus;
    uint16_t crc = CRC16_INITIAL_SEED;
    bool storeAddrInsideEvaluatedArea = (((storeAddress + CRC_LSB_POS_16BIT) >= startAddress) && (storeAddress < (startAddress + length)));
    bool storeAddrOutsideEEPROM = ((storeAddress + CRC_LSB_POS_16BIT) > EEPROM_END);

    //Check for valid length
    if ((length == 0U) || ((startAddress + length) > (EEPROM_END + 1)))
    {
        testStatus = DIAG_INVALID_ARG;
    }
        //Check if storeAddress is not included in EEPROM region to be scanned for CRC
    else if (storeAddrInsideEvaluatedArea || storeAddrOutsideEEPROM)
    {
        testStatus = DIAG_INVALID_ARG;
    }
    else
    {
        //Calculate CRC-16 CCITT for given EEPROM region
        DIAG_EEPROM_CalculateCRC(startAddress, length, &crc);

        EEPROM_Write(storeAddress + 1U, (uint8_t) crc);
        while (true == EEPROM_IsBusy())
        {
        }
        if (NVM_StatusGet() == NVM_ERROR)
        {
            testStatus = DIAG_NVM_STORE_ERROR;
#if defined(DIAG_AVR_Dx_DEVICE)
            NVM_StatusClear();
#endif
        }
        else
        {
            EEPROM_Write(storeAddress, (uint8_t) (crc >> 8U));
            while (true == EEPROM_IsBusy())
            {
            }
            if (NVM_StatusGet() == NVM_ERROR)
            {
                testStatus = DIAG_NVM_STORE_ERROR;
#if defined(DIAG_AVR_Dx_DEVICE)
                NVM_StatusClear();
#endif
            }
            else
            {
                testStatus = DIAG_PASS;
            }
        }
    }

    return testStatus;
}

diag_result_t DIAG_EEPROM_ValidateCRC(eeprom_address_t startAddress, uint16_t length, eeprom_address_t refAddress)
{
    diag_result_t testStatus;
    uint16_t crc = CRC16_INITIAL_SEED;
    uint16_t refCRC;
    bool refAddrInsideEvaluatedArea = (((refAddress + CRC_LSB_POS_16BIT) >= startAddress) && (refAddress < (startAddress + length)));
    bool refAddrOutsideEEPROM = ((refAddress + CRC_LSB_POS_16BIT) > EEPROM_END);

    //Check for valid length
    if ((length == 0U) || ((startAddress + length) > (EEPROM_END + 1)))
    {
        testStatus = DIAG_INVALID_ARG;
    }

        //Check if refAddress is not included in EEPROM region to be scanned for CRC
    else if (refAddrInsideEvaluatedArea || refAddrOutsideEEPROM)
    {
        testStatus = DIAG_INVALID_ARG;
    }
    else
    {
        //Calculate CRC-16 CCITT for given EEPROM region
        DIAG_EEPROM_CalculateCRC(startAddress, length, &crc);

        //Read the reference CRC
        refCRC = (uint16_t) (
                ((uint16_t) EEPROM_Read(refAddress + 1U)) |
                ((uint16_t) EEPROM_Read(refAddress) << 8U)
                );

        if (refCRC != crc)
        {
            testStatus = DIAG_FAIL;
        }
        else
        {
            testStatus = DIAG_PASS;
        }
    }

    return testStatus;
}
