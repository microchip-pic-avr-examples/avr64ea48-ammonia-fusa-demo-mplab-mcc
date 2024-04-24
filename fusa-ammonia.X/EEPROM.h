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
    bool EEPROM_ByteWrite(uint16_t address, uint8_t data);
    
    //Writes and verifies a 16-bit word to the EEPROM. Returns true if successful
    bool EEPROM_WordWrite(uint16_t address, uint16_t data);
    
    //Reads an 8-bit word from the EEPROM
    uint8_t EEPROM_ByteRead(uint16_t address);
    
    //Reads a 16-bit word from the EEPROM
    uint16_t EEPROM_WordRead(uint16_t address);
    
    //Calculates the checksum over the EEPROM
    uint16_t EEPROM_ChecksumCalculate(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

