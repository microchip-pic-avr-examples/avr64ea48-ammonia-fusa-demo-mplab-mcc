#include "EEPROM.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>

#include "mcc_generated_files/nvm/nvm.h"

//Writes and verifies a byte to the EEPROM. Returns true if successful
bool Memory_writeEEPROM8(uint16_t addr, uint8_t data)
{
    //Address is out of bounds
    if (addr >= EEPROM_SIZE)
    {
        return false;
    }
    
        //Write the byte
    nvm_status_t status = EEPROM_Write(addr, data);
    
    //Did an error occur?
    if (status == NVM_ERROR)
    {
        return false;
    }
    
    //Wait for write
    while (EEPROM_IsBusy());
    
    //Verify write
    if (EEPROM_Read(addr) != data)
    {
        return false;
    }
    
    return true;
}

//Writes and verifies a 16-bit word to the EEPROM. Returns true if successful
bool Memory_writeEEPROM16(uint16_t addr, uint16_t data)
{
    //High Byte
    if (!Memory_writeEEPROM8(addr, ((data & 0xFF00) >> 8)))
        return false;
    
    //Low Byte
    if (!Memory_writeEEPROM8((addr + 1), (data & 0xFF)))
        return false;
    
    return true;
}

//Reads an 8-bit word from the EEPROM
uint8_t Memory_readEEPROM8(uint16_t addr)
{
    return EEPROM_Read(addr);
}

//Reads a 16-bit word from the EEPROM
uint16_t Memory_readEEPROM16(uint16_t addr)
{
    uint16_t val;
    
    //Load the high byte
    val = Memory_readEEPROM8(addr);
    
    //Shift Data
    val <<= 8;
    
    //Load the low byte
    val |= Memory_readEEPROM8(addr + 1);
    
    return val;
}

//Run a checksum of the EEPROM
uint16_t Memory_calculateChecksum(void)
{
    bool isLoaded = false;
    uint32_t sum = 0;
    uint16_t addWord = 0;
        
    //Sum the bytes as 16-bit words
    for (uint16_t index = 0; index < EEPROM_SIZE; index++)
    {
        addWord |= Memory_readEEPROM8(index);
        
        if (isLoaded)
        {
            //Full word is loaded
            sum += addWord;
            
            //If we have a carryover, wrap the bit
            if (sum > UINT16_MAX)
            {
                //Mask the bit
                sum &= UINT16_MAX;
                
                //Add one
                sum += 1;
            }
            
            //Clear word
            addWord = 0;
        }
        
        isLoaded = !isLoaded;
        
        //Shift data
        addWord <<= 8;
    }
    
    //Odd number of bytes; add the last byte
    if (isLoaded)
    {
        //Full word is loaded
        sum += addWord;

        //If we have a carryover, wrap the bit
        if (sum > UINT16_MAX)
        {
            //Mask the bit
            sum &= UINT16_MAX;

            //Add one
            sum += 1;
        }
    }
    
    //Take the ones complement of the final value, and crop the remaining bits
    return ((~sum) & UINT16_MAX);
}
