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
    
    //Returns true if sensor is ready
    bool Application_isSensorReady(void);

    //Returns true if ready to self-test
    bool Application_shouldSelfTest(void);
    
    //Clears the self-test flag
    void Application_clearSelfTestFlag(void);
    
#ifdef	__cplusplus
}
#endif

#endif	/* APPLICATION_H */

