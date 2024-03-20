#include "Application.h"

#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"

static volatile uint8_t warmupHours = 0;
static volatile bool WDT_ready = false;
static volatile bool hasHourTicked = false;


//Interrupt for an elapsed hour
void Application_onHourTick(void)
{
    //Increment hours count
    warmupHours++;
    
    hasHourTicked = true;
    
//    //Check to see if sensor is ready
//    if (Application_isSensorReady())
//    {
//        //Disable 1 hour interrupts
//        RTC_DisableOVFInterrupt();
//    }
}

//Interrupt from the PIT (used for periodic self-test)
void Application_onPITTick(void)
{
    WDT_ready = true;
}

//Reset the device
void Application_reset(void)
{
    ccp_write_io((void*) &RSTCTRL.SWRR, RSTCTRL_SWRE_bm);
}

//Returns true if an hour has ticked
bool Application_hasHourTicked(void)
{
    return hasHourTicked;
}

//Clear the hour tick flag
void Application_clearHourTick(void)
{
    hasHourTicked = false;
}

//Prints hours remaining in warmup
void Application_printHoursRemaining(void)
{
    printf("Warmup time remaining: %d / %d hrs\r\n", warmupHours, WARM_UP_HOURS);
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

//Connect the comparator to the gas sensor 
void Application_connectToSensor(void)
{
    //Clear the MUXPOS bits
    AC1.MUXCTRL &= ~(AC_MUXPOS_gm);
    
    //AN1 (sensor) is on PD4, AINP2
    AC1.MUXCTRL |= AC_MUXPOS_AINP2_gc;
}

//Connect the comparator to the DAC output
void Application_connectToDAC(void)
{
    //Clear the MUXPOS bits
    AC1.MUXCTRL &= ~(AC_MUXPOS_gm);
    
    //DAC0 is on PD6, AINP3
    AC1.MUXCTRL |= AC_MUXPOS_AINP3_gc;
}

//Gets the current DACREF on AC1
uint8_t Application_getDACREF(void)
{
    return AC1.DACREF;
}

//Sets a new DACREF on AC1
void Application_setDACREF(uint8_t val)
{
    //Disable Interrupts
    AC1.INTCTRL &= ~AC_CMP_bm;
    
    //Update DACREF for AC1
    AC1_DACRefValueSet(val);
    
    //Wait a few microseconds...
    DELAY_microseconds(10);
    
    //Clear ISR Flag
    AC1.STATUS |= AC_CMPIF_bm;
    
    //Re-enable Interrupts
    AC1.INTCTRL |= AC_CMP_bm;
}

//Runs a CRC Scan (Blocking)
//Returns true if successful 
bool Application_runHWCRC(void)
{
    //Reset the CRC
    CRCSCAN.CTRLA |= CRCSCAN_RESET_bm;
    NOP();
    
    //Enable the CRC
    CRCSCAN.CTRLA |= CRCSCAN_ENABLE_bm;
    
    //Wait for busy to clear
    while (CRCSCAN.STATUS & CRCSCAN_BUSY_bm);
    
    //Get OK bit
    return (bool) ((CRCSCAN.STATUS & CRCSCAN_OK_bm) >> CRCSCAN_OK_bp);
}

//Returns the VLM Status
bool Application_isVoltageOK(void)
{
    //If VLMS is 1, then we're below the threshold
    if (BOD.STATUS & BOD_VLMS_bm)
    {
        return false;
    }
    return true;
}