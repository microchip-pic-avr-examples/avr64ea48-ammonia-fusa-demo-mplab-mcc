#ifndef EEPROM_H
#define	EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <avr/io.h>

//8-bit unsigned integer that indicates the version of the EEPROM mapping
//Used to detect mismatches during development / firmware upgrades
#define EEPROM_VERSION 1

//Address of the EEPROM Version
#define EEPROM_VERSION_ADDR 0
    
//Address of the Zero-Point / Reference Value
#define EEPROM_REF_VALUE_H_ADDR (EEPROM_VERSION_ADDR + 1)
#define EEPROM_REF_VALUE_L_ADDR (EEPROM_REF_VALUE_H_ADDR + 1)

//Address of the CRC16 Checksum for the EEPROM
#define EEPROM_CRC16_H_ADDR (EEPROM_REF_VALUE_L_ADDR + 1)
#define EEPROM_CRC16_L_ADDR (EEPROM_CRC16_H_ADDR + 1)

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

