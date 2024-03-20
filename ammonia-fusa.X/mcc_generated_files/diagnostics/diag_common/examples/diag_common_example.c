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
 *  @file    diag_common_example.c
 *  @brief   This file contains examples to demonstrate diagnostics API usage
 *
 */

#include <stdint.h>
#include <stdio.h>
#include "diag_common_example.h"
#include "../config/diag_config.h"
#include "../../diag_library/memory/non_volatile/diag_flash_crc16.h"

void DIAG_FLASH_Example(void)
{
    diag_result_t status;
    status = DIAG_FLASH_CalculateStoreCRC(DIAG_FLASH_START_ADDR, DIAG_FLASH_LENGTH, DIAG_FLASH_CRC_STORE_ADDR);
    switch (status)
    {
    case DIAG_PASS:
        printf("\r\nPassed : FLASH CRC Test - CRC calculated and stored successfully\r\n");
        break;
    case DIAG_INVALID_ARG:
        printf("\r\nInvalid Arguments : FLASH CRC Test - Invalid store address or length\r\n");
        break;
    case DIAG_NVM_STORE_ERROR:
        printf("\r\nError : FLASH CRC Test - NVM Controller could not write to storeAddress: 0x%lx\r\n", (unsigned long)DIAG_FLASH_CRC_STORE_ADDR);
        break;
    default:
        printf("\r\nUndefined : FLASH CRC Test\r\n");
        break;
    }

    /*
     * DIAG_FLASH_ValidateCRC() should ideally be called periodically during
     * standard application usage. It is called once here for demonstration purposes.
     */
    status = DIAG_FLASH_ValidateCRC(DIAG_FLASH_START_ADDR, DIAG_FLASH_LENGTH, DIAG_FLASH_CRC_STORE_ADDR);
    switch (status)
    {
    case DIAG_PASS:
        printf("\r\nPassed : FLASH CRC Test\r\n");
        break;
    case DIAG_FAIL:
        printf("\r\nFailed : FLASH CRC Test\r\n");
        break;
    case DIAG_INVALID_ARG:
        printf("\r\nInvalid Arguments : FLASH CRC Test - Invalid store address or length\r\n");
        break;
    default:
        printf("\r\nUndefined : FLASH CRC Test\r\n");
        break;
    }
}

