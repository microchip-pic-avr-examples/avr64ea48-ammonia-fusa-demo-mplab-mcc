#ifndef EEPROM_H
#define	EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
#include <avr/io.h>

//8-bit unsigned integer that indicates the version of the EEPROM mapping
//Used to detect mismatches during development / firmware upgrades
#define EEPROM_VERSION_ID 1

//Address of the EEPROM Version
#define EEPROM_VERSION_ADDR (0 + EEPROM_START)
    
//Address of the Zero-Point / Reference Value
#define EEPROM_REF_VALUE_H_ADDR (EEPROM_VERSION_ADDR + 1)
#define EEPROM_REF_VALUE_L_ADDR (EEPROM_REF_VALUE_H_ADDR + 1)

//Address of the Checksum for the EEPROM
#define EEPROM_CKSM_H_ADDR (EEPROM_SIZE - 2)
#define EEPROM_CKSM_L_ADDR (EEPROM_SIZE - 1)
    
#define EEPROM_CHECKSUM_GOOD 0x0000
    
    //Writes and verifies a byte to the EEPROM. Returns true if successful
    bool Memory_writeEEPROM8(uint16_t addr, uint8_t data);
    
    //Writes and verifies a 16-bit word to the EEPROM. Returns true if successful
    bool Memory_writeEEPROM16(uint16_t addr, uint16_t data);
    
    //Reads an 8-bit word from the EEPROM
    uint8_t Memory_readEEPROM8(uint16_t addr);
    
    //Reads a 16-bit word from the EEPROM
    uint16_t Memory_readEEPROM16(uint16_t addr);
    
    //Calculates the checksum over the EEPROM
    uint16_t Memory_calculateChecksum(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

