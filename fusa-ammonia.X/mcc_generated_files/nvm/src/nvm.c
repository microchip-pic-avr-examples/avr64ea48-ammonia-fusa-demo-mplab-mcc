/**
 *
 * @file nvm.c
 *
 * @ingroup nvm_driver
 *
 * @brief This file contains the implementation for the NVM driver.
 *
 * @version NVM Driver Version 2.1.2
 */
/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <avr/pgmspace.h>
#include "../../system/ccp.h"
#include "../nvm.h"


static inline void NVM_NoOperation_Command(void)
{
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NOOP_gc);
}

static inline void FLASH_SpmWriteWord(flash_address_t address, uint16_t data)
{
    __asm__ __volatile__(
                         "push r0\n\t" /* back up R0*/\
                         "movw r0, %A1\n\t" /* update R0,R1 pair with word*/\
                         "movw r30, %A0\n\t" /* update R30,R31 pair with address[Byte1,Byte0]*/\
                         "spm \n\t" /* spm */\
                         "clr r1\n\t" /* R1 is always assumed to be zero by the compiler. Resetting R1 to zero*/\
                         "pop r0 \n\t" /* restore R0*/\
                         :                                                      \
                         : "r" ((uint32_t)(address)), "r" ((uint16_t)(data))  \
                         : "r30", "r31" /* Clobber R30, R31 to indicate they are used here*/
                         );
}

void NVM_Initialize(void)
{    
    //APPCODEWP enabled; APPDATAWP disabled; BOOTRP disabled; EEWP disabled; FLMAP SECTION0; FLMAPLOCK disabled; 
    ccp_write_io((void*)&NVMCTRL.CTRLB, 0x1);
}
nvm_status_t NVM_StatusGet(void)
{
    return (((NVMCTRL.STATUS & NVMCTRL_ERROR_gm) != 0) ? NVM_ERROR : NVM_OK);
}

void NVM_StatusClear(void)
{
    NVMCTRL.STATUS &= ~NVMCTRL_ERROR_gm;
}


flash_data_t FLASH_Read(flash_address_t address)
{
    return pgm_read_byte_near(address);
}

nvm_status_t FLASH_RowWrite(flash_address_t address, flash_data_t *dataBuffer)
{
    uint16_t dataWord;
    NVM_NoOperation_Command();
    //Write data to the page buffer, one word at a time
    for (uint16_t i = 0; i < PROGMEM_PAGE_SIZE; i = i + 2U)
    {
        //Access wordwise data from buffer
        dataWord = (uint16_t)(dataBuffer[i] | (dataBuffer[i + 1U] << 8U));
        FLASH_SpmWriteWord(address + (uint32_t)i, dataWord);
    }
    // Write the flash page
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_FLPW_gc);
    return (((NVMCTRL.STATUS & NVMCTRL_ERROR_gm) != 0) ? NVM_ERROR : NVM_OK);
}

nvm_status_t FLASH_PageErase(flash_address_t address)
{
    NVM_NoOperation_Command();
    //Dummy write to start erase operation
    FLASH_SpmWriteWord(address, 0);
	// Erase the flash page
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_FLPER_gc);
    return (((NVMCTRL.STATUS & NVMCTRL_ERROR_gm) != 0) ? NVM_ERROR : NVM_OK);
}

bool FLASH_IsBusy(void)
{
    return (NVMCTRL.STATUS & NVMCTRL_FLBUSY_bm);
}

flash_address_t FLASH_PageAddressGet(flash_address_t address)
{
    return (flash_address_t) (address & ((PROGMEM_SIZE - 1) ^ (PROGMEM_PAGE_SIZE - 1)));
}

flash_address_t FLASH_PageOffsetGet(flash_address_t address)
{
    return (flash_address_t) (address & (PROGMEM_PAGE_SIZE - 1));
}


eeprom_data_t EEPROM_Read(eeprom_address_t address)
{	
	return *(eeprom_data_t *)(address);
	
}

nvm_status_t EEPROM_Write(eeprom_address_t address, eeprom_data_t data)
{
        NVM_NoOperation_Command();
		// Write byte to page buffer 
		*(eeprom_data_t *)(address) = data;
		
		// Erase byte and program it with desired value 
		_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_EEPERW_gc); 
		return (((NVMCTRL.STATUS & NVMCTRL_ERROR_gm) != 0) ? NVM_ERROR : NVM_OK);
}

bool EEPROM_IsBusy(void)
{
    return (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
}



