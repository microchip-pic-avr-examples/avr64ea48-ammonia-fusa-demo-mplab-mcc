#include "Application.h"

#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"

static volatile uint8_t warmupHours = 0;
static volatile bool WDT_ready = false;

//Interrupt for an elapsed hour
void Application_onHourTick(void)
{
    warmupHours++;
    
    //Check to see if sensor is ready
    if (Application_isSensorReady())
    {
        //Disable 1 hour interrupts
        RTC_DisableOVFInterrupt();
    }
}

//Interrupt from the PIT (used for periodic self-test)
void Application_onPITTick(void)
{
    WDT_ready = true;
}

//Returns true if sensor is ready
bool Application_isSensorReady(void)
{
    if (warmupHours >= WARM_UP_HOURS)
    {
        return true;
    }
    return false;
}

//Returns true if ready to self-test
bool Application_shouldSelfTest(void)
{
    return WDT_ready;
}

//Clears the self-test flag
void Application_clearSelfTestFlag(void)
{
    WDT_ready = false;
}