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
    } SystemState;
    
    //Runs a self-test of the system on startup
    bool Fusa_runStartupSelfTest(void);
    
    //Verifies the checksum has been XORed
    //Required for CRC-32
    bool Fusa_prepareChecksum(void);
    
    //Run a CPU test
    bool Fusa_testCPU(void);
    
    //Test the comparator
    bool Fusa_testAC(void);
    
    //Run a memory self-test
    bool Fusa_testFlash(void);
    
    //Run an SRAM self-test
    bool Fusa_testSRAM(void);
    
    //Run a checksum of the EEPROM
    bool Fusa_testEEPROM(void);
    
    //Gets the 32-bit CRC from memory
    uint32_t Fusa_getChecksumFromPFM(void);
    
    //Invalidates the EEPROM
    void Fusa_invalidateEEPROM(void);
    
    //Runs the periodic self-test of the system
    void Fusa_runPeriodicSelfCheck(void);
    
    //Infinite loop for a system failure
    void Fusa_onSystemFailure(void);
    
    //Activate the alarm
    void Fusa_activateAlarm(void);
    
    //Deactivate the alarm
    void Fusa_deactivateAlarm(void);

#ifdef	__cplusplus
}
#endif

#endif	/* FUSA_H */

