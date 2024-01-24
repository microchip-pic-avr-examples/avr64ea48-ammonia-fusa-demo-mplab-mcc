#ifndef FUSA_H
#define	FUSA_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
    typedef enum {
        SYS_ERROR = -1, SYS_INIT = 0, SYS_WARMUP, SYS_CALIBRATE, SYS_RUN, SYS_ALARM
    } SystemState;
    
    //Runs a self-test of the system on startup
    bool Fusa_runStartupSelfTest(void);
    
    //Run a CPU test
    bool Fusa_runCPUCheck(void);
    
    //Test the DAC
    bool Fusa_checkDAC(void);
    
    //Test the comparator
    bool Fusa_checkCMP(void);
    
    //Run a memory self-check
    bool Fusa_runMemoryCheck(void);
    
    //Are the calibration constants OK?
    bool Fusa_isCalOK(void);
    
    //Runs the periodic self-test of the system
    void Fusa_runPeriodicSelfCheck(void);
    
    //Infinite loop for a system failure
    void Fusa_onSystemFailure(void);
    

#ifdef	__cplusplus
}
#endif

#endif	/* FUSA_H */

