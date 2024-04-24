#ifndef APPLICATION_H
#define	APPLICATION_H

#include "mcc_generated_files/system/system.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
//Enables the alarm buzzer
#define BUZZER_ENABLE() do { TCA0_Start(); } while (0)
    
//Disables the alarm buzzer
#define BUZZER_DISABLE() do { TCA0_Stop(); BUZZER_SetLow(); } while (0)

//Number of hours to warmup for
#define WARM_UP_HOURS 24
    
//If defined, the class B library uses a 16-bit CRC to verify EEPROM
//If not defined, a 16-bit checksum is used instead
//#define FUSA_ENABLE_EEPROM_SIMPLE_CHECKSUM
    
//If defined, the class B library will perform a flash scan in HW
//If not defined, the HW CRC will be used instead
//#define FUSA_ENABLE_FLASH_HW_SCAN
    
    //Interrupt for an elapsed hour
    void APP_HourTick(void);
    
    //Interrupt from the PIT (used for periodic self-test)
    void APP_PITTick(void);
    
    //Reset the device
    void APP_Reset(void);
    
    //Returns true if an hour has ticked
    bool APP_HasHourTicked(void);
    
    //Clear the hour tick flag
    void APP_HourTickClear(void);
    
    //Prints hours remaining in warmup
    void APP_RemainingHoursPrint(void);
    
    //Returns true if sensor is ready
    bool APP_IsSensorReady(void);

    //Returns true if ready to self-test
    bool APP_IsReadyForSelfTest(void);
    
    //Clears the self-test flag
    void APP_SelfTestFlashClear(void);
    
    //Connect the comparator to the gas sensor 
    void APP_SensorConnect(void);
    
    //Connect the comparator to the DAC output
    void APP_DACConnect(void);
    
    //Gets the current DACREF on AC1
    uint8_t APP_DACREFGet(void);
    
    //Sets a new DACREF on AC1
    //Disables AC1 interrupt, updates value, waits, then re-enables
    void APP_DACREFSet(uint8_t val);
    
    //Runs a CRC Scan (Blocking)
    //Returns true if successful 
    bool APP_HardwareCRCRun(void);
    
    //Returns the VLM Status
    bool APP_VLMStatusGet(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* APPLICATION_H */

