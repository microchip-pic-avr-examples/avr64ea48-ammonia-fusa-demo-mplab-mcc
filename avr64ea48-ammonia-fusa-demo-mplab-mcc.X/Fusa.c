#include "Fusa.h"

#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include "Application.h"

#define PASS_STRING "OK\r\n"
#define FAIL_STRING "FAIL\r\n"
#define NOT_FOUND_STRING "NOT FOUND\r\n"

static volatile SystemState sysState = SYS_ERROR;

//Runs a self-test of the system
bool Fusa_runStartupSelfTest(void)
{
    printf("\r\nRunning Self Test\r\n");
    
    //Set the state to INIT
    //If an error occurs, then it will be moved to ERROR
    //If in INIT at the end of this function, then it will be moved to warm-up
    sysState = SYS_INIT;
    
    //Check CPU
    printf("Testing CPU...");
    if (Fusa_runCPUCheck())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }
    
    //Check Memory
    printf("Testing Memory Integrity...");
    if (Fusa_runMemoryCheck())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }
    
    //Check DAC
    printf("Testing DAC...");
    if (Fusa_checkDAC())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }
    
    //Check Comparator
    printf("Testing Analog Comparator...");
    if (Fusa_checkCMP())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }
        
    //Check EEPROM for valid constants
    printf("Calibration data...");
    if (Fusa_isCalOK())
        printf(PASS_STRING);
    else
    {
        printf(NOT_FOUND_STRING);
        //This can fail - user will have to calibrate
    }
    
    //TODO: REMOVE THESE LINES
    printf("STARTUP ERROR DISABLED\r\n");
    sysState = SYS_WARMUP;
    
    if (sysState == SYS_ERROR)
    {
        printf("Self Test Failed\r\n\r\n");
    }
    else
    {
        printf("Self Test Passed\r\n\r\n");
        sysState = SYS_WARMUP;
    }
    
    return false;
}

//Run a CPU test
bool Fusa_runCPUCheck(void)
{
    return false;
}

//Test the DAC
bool Fusa_checkDAC(void)
{
    return false;
}
    
//Test the comparator
bool Fusa_checkCMP(void)
{
    /* Test Setup
     * Verify DACREF is operational by using DAC0's output
     * This should cause the comparator to go HIGH
     * After verifying, return it to the Ammonia sensor
     */
    
    
    return false;
}

//Run a memory self-check
bool Fusa_runMemoryCheck(void)
{
    return false;
}
    
//Are the calibration constants OK?
bool Fusa_isCalOK(void)
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
                sysState = SYS_RUN;
            }
            
            break;
        }
        case SYS_RUN:
        {
            //System is running
            break;
        }
        case SYS_ALARM:
        {
            //System alarm is tripped
            break;
        }
        case SYS_ERROR:
        case SYS_INIT:
        default:
        {
            //Bad States
            //SYS_INIT should never be possible outside of self-test function
            
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