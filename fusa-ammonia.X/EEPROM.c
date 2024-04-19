#include "EEPROM.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>

#include "mcc_generated_files/system/system.h"
#include "application.h"

//Writes and verifies a byte to the EEPROM. Returns true if successful
bool EEPROM_ByteWrite(uint16_t address, uint8_t data)
{
    //Check to see if VDD is OK
    //If VLM is 1, then we are below threshold
    if (!APP_VLMStatusGet())
    {
        printf("BOD ERROR\r\n");
        return false;
    }
    
    //Address is out of bounds
    if (address > EEPROM_END)
    {
        printf("ADDR ERROR\r\n");
        return false;
    }
   
    //Check for a pending operation
    while (EEPROM_IsBusy());
    
    //Clear any prev. errors
    NVM_StatusClear();
    
    //Write the byte
    nvm_status_t status = EEPROM_Write(address, data);
    
    //Did an error occur?
    if (status == NVM_ERROR)
    {
        printf("NVM ERROR\r\n");
        return false;
    }
    
    //Wait for write
    while (EEPROM_IsBusy());
    
    //Verify write
    if (EEPROM_Read(address) != data)
    {
        printf("VERIFY ERROR\r\n");
        return false;
    }
    
    return true;
}

//Writes and verifies a 16-bit word to the EEPROM. Returns true if successful
bool EEPROM_WordWrite(uint16_t address, uint16_t data)
{
    //High Byte
    if (!EEPROM_ByteWrite(address, ((data & 0xFF00) >> 8)))
        return false;
    
    //Low Byte
    if (!EEPROM_ByteWrite((address + 1), (data & 0xFF)))
        return false;
    
    return true;
}

//Reads an 8-bit word from the EEPROM
uint8_t EEPROM_ByteRead(uint16_t address)
{
    return EEPROM_Read(address);
}

//Reads a 16-bit word from the EEPROM
uint16_t EEPROM_WordRead(uint16_t address)
{
    uint16_t val;
    
    //Load the high byte
    val = EEPROM_ByteRead(address);
    
    //Shift Data
    val <<= 8;
    
    //Load the low byte
    val |= EEPROM_ByteRead(address + 1);
    
    return val;
}

//Run a checksum of the EEPROM
uint16_t EEPROM_ChecksumCalculate(void)
{
    bool isLoaded = false;
    uint32_t sum = 0;
    uint16_t addWord = 0;
        
    //Sum the bytes as 16-bit words
    for (uint16_t index = 0; index < EEPROM_SIZE; index++)
    {
        addWord |= EEPROM_ByteRead(index);
        
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
