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
#define BUZZER_DISABLE() do { TCA0_Stop(); } while (0)

//Number of hours to warmup for
#define WARM_UP_HOURS 24
    
    //Interrupt for an elapsed hour
    void Application_onHourTick(void);
    
    //Interrupt from the PIT (used for periodic self-test)
    void Application_onPITTick(void);
    
    //Reset the device
    void Application_reset(void);
    
    //Returns true if an hour has ticked
    bool Application_hasHourTicked(void);
    
    //Clear the hour tick flag
    void Application_clearHourTick(void);
    
    //Prints hours remaining in warmup
    void Application_printHoursRemaining(void);
    
    //Returns true if sensor is ready
    bool Application_isSensorReady(void);

    //Returns true if ready to self-test
    bool Application_shouldSelfTest(void);
    
    //Clears the self-test flag
    void Application_clearSelfTestFlag(void);
    
    //Connect the comparator to the gas sensor 
    void Application_connectToSensor(void);
    
    //Connect the comparator to the DAC output
    void Application_connectToDAC(void);
    
    //Gets the current DACREF on AC1
    uint8_t Application_getDACREF(void);
    
    //Sets a new DACREF on AC1
    //Disables AC1 interrupt, updates value, waits, then re-enables
    void Application_setDACREF(uint8_t val);
    
    //Runs a CRC Scan (Blocking)
    //Returns true if successful 
    bool Application_runCRC(void);
    
    //Returns the VLM Status
    bool Application_isVoltageOK(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* APPLICATION_H */

