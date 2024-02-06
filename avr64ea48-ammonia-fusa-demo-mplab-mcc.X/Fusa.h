#ifndef FUSA_H
#define	FUSA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
//How many bits above/below the DACREF should we test at 
#define TEST_MARGIN 16
    
//Minimum DACREF Value in bits. Should be bigger than or equal to TEST_MARGIN
#define DACREF_MIN_ALLOWED 16
        
    typedef enum {
        SYS_ERROR = -1, SYS_INIT = 0, SYS_WARMUP, 
        SYS_CALIBRATE, SYS_MONITOR, SYS_TEST, SYS_ALARM
    } SystemState;
    
    //Runs a self-test of the system on startup
    bool Fusa_runStartupSelfTest(void);
    
    //Run a CPU test
    bool Fusa_testCPU(void);
    
    //Test the comparator
    bool Fusa_testAC(void);
    
    //Run a memory self-check
    bool Fusa_testMemory(void);
    
    //Gets the 32-bit CRC from memory
    uint32_t Fusa_getChecksumFromPFM(void);
    
    //Runs the periodic self-test of the system
    void Fusa_runPeriodicSelfCheck(void);
    
    //Infinite loop for a system failure
    void Fusa_onSystemFailure(void);
    
    //Returns true if the system is ready, false if alarms should be ignored
    bool Fusa_isSystemArmed(void);

#ifdef	__cplusplus
}
#endif

#endif	/* FUSA_H */

