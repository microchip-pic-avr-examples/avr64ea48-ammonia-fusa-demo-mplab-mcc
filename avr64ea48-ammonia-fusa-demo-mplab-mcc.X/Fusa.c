#include "Fusa.h"

#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"

#include "Application.h"
#include "GasSensor.h"

#define PASS_STRING "OK\r\n"
#define FAIL_STRING "FAIL\r\n"
#define NOT_FOUND_STRING "NOT FOUND\r\n"

static volatile SystemState sysState = SYS_ERROR;

//Runs a self-test of the system
bool Fusa_runStartupSelfTest(void)
{
    printf("\r\nRunning Self Test\r\n");
    
    //Run the buzzer during self-test
    BUZZER_ENABLE();
    
    //Set the state to INIT
    //If an error occurs, then it will be moved to ERROR
    //If in INIT at the end of this function, then it will be moved to warm-up
    sysState = SYS_INIT;
    
    //Check CPU
    printf("Testing CPU...");
    if (Fusa_testCPU())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }
    
    //Check Memory
    printf("Testing Memory Integrity...");
    if (Fusa_testMemory())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }
    
    //Check Comparator
    printf("Testing Analog Comparator...");
    if (Fusa_testAC())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }
        
    //Check EEPROM for valid constants
    printf("Calibration data...");
    if (GasSensor_isEEPROMValid())
        printf(PASS_STRING);
    else
    {
        printf(NOT_FOUND_STRING);
        //This can fail - user will have to calibrate
    }
    
    //In develop mode, ignore startup errors
#ifdef DEVELOP_MODE
    if (sysState == SYS_ERROR)
    {
        printf("\r\nWARNING: Start-up test failed. Continuing startup...\r\n");
        sysState = SYS_WARMUP;
    }
    
#endif
    
    //Disable the buzzer at the end of self-test
    BUZZER_DISABLE();
    
    //Check to see if any error occurred
    if (sysState == SYS_ERROR)
    {
        printf("Self Test Failed\r\n\r\n");
    }
    else
    {
        printf("Self Test Complete\r\n\r\n");
        sysState = SYS_WARMUP;
    }
    
    return false;
}

//Run a CPU test
bool Fusa_testCPU(void)
{
    return false;
}
    
//Test the comparator
bool Fusa_testAC(void)
{
    /* Test Setup
     * Verify DACREF and CMP are operational by using DAC0's output
     * 
     * 1. Verify DACREF > MIN_ALLOWED
     * 2. Save current system state.
     * 3. Switch to SYS_TEST state.
     * 4. Switch In+ to DAC0 Output (PD6, AINP3)
     * 5. Set DAC0 to a value greater than DACREF
     * 6. Wait for value to stabilize 
     * 7. Check comparator output is HIGH
     * 8. Set DAC0 to 0
     * 9. Wait for value to stabilize
     * 10. Check comparator output is LOW
     * 11. Switch In+ back to Ammonia Sensor (PD4, AINP2)
     * 12. Return to previous state
     */
    
    if (Application_getDACREF() < DACREF_MIN_ALLOWED)
    {
        sysState = SYS_ERROR;
        return false;
    }
    
    //Save current state
    SystemState prevState = sysState;
    
    //Switch to test state
    sysState = SYS_TEST;
    
    //Switch AC0 Input
    Application_connectToDAC();
    
    //DAC0 is 10-bit, DACREF is 8-bit
    
    //Set to DACREF + MARGIN
    int16_t testVal = (Application_getDACREF() << 2) + TEST_MARGIN;
    DAC0_SetOutput(testVal);
    
    //Wait 7us (DAC0 Settling) + 0.15us (AC Response)
    DELAY_microseconds(8);
    
    //Is the signal LOW?
    if (!AC1_Read())
    {
        //AC0 is malfunctioning, change to SYS_ERROR
        sysState = SYS_ERROR;
        return false;
    }
    
    //Set to DACREF - TEST_MARGIN
    testVal = (Application_getDACREF() << 2) - TEST_MARGIN;
    DAC0_SetOutput(testVal);
    
    //Wait 7us (DAC0 Settling) + 0.15us (AC Response)
    DELAY_microseconds(8);
    
    //Is the signal HIGH?
    if (AC1_Read())
    {
        //AC0 is malfunctioning, change to SYS_ERROR
        sysState = SYS_ERROR;
        return false;
    }
    
    //Switch AC back to Sensor
    Application_connectToSensor();
    
    //Restore system state
    sysState = prevState;
    
    return true;
}

//Run a memory self-check
bool Fusa_testMemory(void)
{
    return false;
}
    
//Runs the periodic self-test of the system
void Fusa_runPeriodicSelfCheck(void)
{
    //Clear WDT
    asm("WDR");
    
    //Run state machine
    switch (sysState)
    {
        case SYS_WARMUP:
        {
            //System is in warm-up
            
            //Sensor is now ready
            if (Application_isSensorReady())
            {
                //Ready to begin active monitoring
                sysState = SYS_MONITOR;
            }
            
            break;
        }
        case SYS_MONITOR:
        {
            //System is running
            
            //Did the alarm activate?
            if (GasSensor_isTripped())
            {
                sysState = SYS_ALARM;
            }
                
            break;
        }
        case SYS_ALARM:
        {
            //System alarm is tripped
            
            //Did the alarm go off?
            if (!GasSensor_isTripped())
            {
                sysState = SYS_MONITOR;
            }
            
            break;
        }
        case SYS_ERROR:
        case SYS_INIT:
        case SYS_TEST:
        default:
        {
            //Bad States
            //SYS_INIT should never be possible outside of POR self-test
            //SYS_TEST should never be possible outside of periodic self-tests
            
            //Enter an infinite loop
            Fusa_onSystemFailure();
            break;
        }

    }
}

//Infinite loop for a system failure
void Fusa_onSystemFailure(void)
{
    //Disable interrupts
    cli();
    
    //Variable used for printing the failure message
    uint8_t timeCount = 10;
    
    while (1)
    {
        //Print a UART message every 10 cycles (~8s)
        if (timeCount == 10)
        {
            printf("SYSTEM FAULT\r\n");
            timeCount = 1;
        }
        else
        {
            timeCount++;
        }
        
        //Buzzer pattern
        
        BUZZER_ENABLE();
        LED0_SetHigh();
        DELAY_milliseconds(600);
        LED0_SetLow();
        BUZZER_DISABLE();
        DELAY_milliseconds(200);
        
        //Clear WDT
        asm("WDR");
    }
}

//Returns true if the system is ready, false if alarms should be ignored
bool Fusa_isSystemArmed(void)
{ 
    if ((sysState == SYS_MONITOR) || (sysState == SYS_ALARM))
        return true;   
    return false;
}