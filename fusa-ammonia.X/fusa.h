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
    bool FUSA_StartupSelfTestRun(void);
    
    //Run a CPU test
    bool FUSA_CPUTest(void);
    
    //Get the WDT Test Results
    bool FUSA_WDTTest(void);
    
    //Test the comparator
    bool FUSA_ACTest(void);
    
    //Run a memory self-test
    bool FUSA_FlashTest(void);
    
    //Run an SRAM self-test
    bool FUSA_SRAMTest(void);
    
    //Run a checksum of the EEPROM
    bool FUSA_EEPROMTest(void);
    
    //Gets the 32-bit CRC from memory
    uint32_t FUSA_PFMChecksumGet(void);
    
    //Invalidates the EEPROM
    void FUSA_EEPROMInvalidate(void);
    
    //Runs the periodic self-test of the system
    void FUSA_PeriodicSelfCheckRun(void);
    
    //Periodically scans the FLASH
    void FUSA_PeriodicMemoryScanRun(void);
    
    //Infinite loop for a system failure
    void FUSA_HandleSystemFailure(void);
    
    //Activate the alarm
    void FUSA_AlarmActivate(void);
    
    //Deactivate the alarm
    void FUSA_AlarmDeactivate(void);

#ifdef	__cplusplus
}
#endif

#endif	/* FUSA_H */

