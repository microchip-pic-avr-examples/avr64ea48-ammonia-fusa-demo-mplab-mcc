#ifndef FUSA_H
#define	FUSA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/system/pins.h"
    
//How many bits above/below the DACREF should we test at 
//Note - DAC0 is 10-bit, DACREF is 8-bit, so 4 bits at DAC0 = 1 bit at DACREF
#define TEST_MARGIN 4
            
//If set, raw ADC values are printed
//#define VIEW_RAW_ADC
    
#define TEST_BUTTON_GetValue T1OUT_GetValue
        
    typedef enum {
        SYS_ERROR = -1, SYS_INIT = 0, SYS_WARMUP, 
        SYS_CALIBRATE, SYS_MONITOR, SYS_SELF_TEST, SYS_ALARM
    } system_state_t;
    
    //Runs a self-test of the system on startup
    bool Fusa_StartupSelfTestRun(void);
    
    //Run a CPU test
    bool Fusa_CPUTest(void);
    
    //Get the WDT Test Results
    bool Fusa_WDTTest(void);
    
    //Test the comparator
    bool Fusa_ACTest(void);
    
    //Run a memory self-test
    bool Fusa_FlashTest(void);
    
    //Run an SRAM self-test
    bool Fusa_SRAMTest(void);
    
    //Run a checksum of the EEPROM
    bool Fusa_EEPROMTest(void);
    
    //Gets the 32-bit CRC from memory
    uint32_t Fusa_PFMChecksumGet(void);
    
    //Invalidates the EEPROM
    void Fusa_EEPROMInvalidate(void);
    
    //Runs the periodic self-test of the system
    void Fusa_PeriodicSelfCheckRun(void);
    
    //Infinite loop for a system failure
    void Fusa_HandleSystemFailure(void);
    
    //Activate the alarm
    void Fusa_AlarmActivate(void);
    
    //Deactivate the alarm
    void Fusa_AlarmDeactivate(void);

#ifdef	__cplusplus
}
#endif

#endif	/* FUSA_H */

