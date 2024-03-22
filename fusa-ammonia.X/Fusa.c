#include "Fusa.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"

#include "Application.h"
#include "GasSensor.h"
#include "EEPROM.h"
#include "mcc_generated_files/diagnostics/diag_library/memory/non_volatile/diag_flash_crc32.h"
#include "mcc_generated_files/diagnostics/diag_library/cpu/diag_cpu_registers.h"
#include "mcc_generated_files/diagnostics/diag_library/memory/volatile/diag_sram_marchc_minus.h"
#include "mcc_generated_files/diagnostics/diag_library/wdt/diag_wdt_startup.h"
#include "mcc_generated_files/diagnostics/diag_library/memory/non_volatile/diag_eeprom_crc16.h"

#define PASS_STRING "OK\r\n"
#define FAIL_STRING "FAIL\r\n"

#define CRC_ADDRESS_START 0xFFFC

#ifndef TEST_CHECKSUM
/* This is the checksum used for the system memory
 * CRC-32 at end of Application Memory
 * Inserted by the compiler at runtime
 * Note: __at is in bytes, not words */
extern const uint32_t flashChecksum __at((CRC_ADDRESS_START));

#else
/*
 * This is a checksum used for testing ONLY
 * Little Endian Order - Highest byte is the last byte
 */
static volatile const uint32_t flashChecksum __at((CRC_ADDRESS_START)) = 0x87654321;
#endif

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
    
    if (SW0_GetValue())
    {
        //Erase requested by user
        GasSensor_eraseEEPROM();
                
        printf("Configuration ERASED\r\n");
    }
    
    printf("Flash Memory Checksum = 0x%lx\r\n\r\n", Fusa_getChecksumFromPFM());
    
    //Check CPU
    printf("Testing CPU...");
    if (Fusa_testCPU())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }
    
    //Get the WDT Result
    printf("Testing WDT...");
    if (Fusa_testWDT())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }

    //Get the SRAM Result
    printf("Testing SRAM...");
    if (Fusa_testSRAM())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        sysState = SYS_ERROR;
    }
    
    //Check Memory
    printf("Testing Memory Integrity...");
    if (Fusa_testFlash())
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
    if (Fusa_testEEPROM())
    {        
        //Init R_L constant from EEPROM
        GasSensor_initFromEEPROM();
        
        //EEPROM OK
        printf(PASS_STRING);
    }
    else
    {
        //No valid calibration found in EEPROM
        printf("INVALID\r\n");
    }
        
    DELAY_microseconds(100);
    
    //Restart the RTC
    RTC_Start();

    
    //In develop mode, ignore startup errors and accelerate warm-up
#ifdef DEVELOP_MODE
    //Stop the RTC
    RTC_Stop();
    
    //Min period
    RTC_WritePeriod(1);
    
    //Clear counter
    RTC_WriteCounter(0);
    
    //REQUIRED DELAY
    //TO BE FIXED IN LATER MCC VERSIONS
    DELAY_microseconds(100);
    
    //Restart the RTC
    RTC_Start();
    
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
        printf("Self Test Complete\r\n\r\nBeginning %u hour sensor warmup\r\n", WARM_UP_HOURS);
        sysState = SYS_WARMUP;
    }
    
    return false;
}

//Run a CPU test
bool Fusa_testCPU(void)
{
    if (DIAG_CPU_Registers() == DIAG_PASS)
    {
        return true;
    }
    return false;
}
    
//Get the WDT Test Results
bool Fusa_testWDT(void)
{
    if (DIAG_WDT_GetResult() == DIAG_PASS)
    {
        return true;
    }
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
    
    //Save current state
    SystemState prevState = sysState;
    
    //Switch to test state
    sysState = SYS_SELF_TEST;
    
    //Switch AC0 Input
    Application_connectToDAC();
    
    //DAC0 is 10-bit, DACREF is 8-bit
    
    //Set to DACREF + MARGIN
    int16_t testVal = (Application_getDACREF() << 2) + TEST_MARGIN;
    
    //Limit to a 10-bit number
    if (testVal >= 0x3FF)
    {
        testVal = 0x3FF;
    }
    
    //Set to Alarm state
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
    
    //Set to DACREF - TEST_MARGIN
    testVal = (Application_getDACREF() << 2) - TEST_MARGIN;
    
    //If less than 0, set to 0
    if (testVal < 0)
    {
        testVal = 0;
    }
    
    //Set to low value
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
    
    //Switch AC back to Sensor
    Application_connectToSensor();
    
    //Restore system state
    sysState = prevState;
    
    return true;
}

//Run a memory self-check
bool Fusa_testFlash(void)
{    
#ifdef FUSA_ENABLE_FLASH_SW_SCAN
    if (DIAG_FLASH_ValidateCRC(DIAG_FLASH_START_ADDR, (DIAG_FLASH_CRC_STORE_ADDR), DIAG_FLASH_CRC_STORE_ADDR)
            == DIAG_PASS)
    {
        return true;
    }
    return false;
#else
    return Application_runHWCRC();
#endif
}

//Run an SRAM self-test
bool Fusa_testSRAM(void)
{
    //Did the SRAM test pass?
    if (DIAG_SRAM_MarchGetStartupResult() == DIAG_PASS)
    {
        return true;
    }
    return false;
}

//Run a checksum of the EEPROM
bool Fusa_testEEPROM(void)
{
    /* Test Procedure:
     * 1. Verify the EEPROM Version ID matches expected
     * 2. Verify the EEPROM Checksum
     */
    
    //Verify Version ID
    if (Memory_readEEPROM8(EEPROM_VERSION_ADDR) != EEPROM_VERSION_ID)
    {
        return false;
    }
        
#ifndef FUSA_ENABLE_EEPROM_CRC_CHECK
    //Verify Checksum (Simple)
    if (Memory_calculateChecksum() != EEPROM_CHECKSUM_GOOD)
    {
        return false;
    }
#else
    
    //Verify a CRC Checksum w/ Class B libraries
    if (DIAG_EEPROM_ValidateCRC(0, DIAG_EEPROM_LENGTH,
            DIAG_EEPROM_CRC_STORE_ADDR - DIAG_EEPROM_START_ADDR) != DIAG_PASS)
    {
        return false;
    }
#endif
    
    return true;
}

//Gets the 32-bit CRC from memory
uint32_t Fusa_getChecksumFromPFM(void)
{
    uint32_t addr = CRC_ADDRESS_START;
    
    //Low Byte
    uint32_t sum = (uint32_t) FLASH_Read(addr);
    addr++;
    
    //High Byte
    sum |= ((uint32_t) FLASH_Read(addr)) << 8;
    addr++;
    
    //Upper Byte
    sum |= ((uint32_t) FLASH_Read(addr)) << 16;
    addr++;
    
    //Top Byte
    sum |= ((uint32_t) FLASH_Read(addr)) << 24;
    
    return sum;
}
    
//Invalidates the EEPROM
void Fusa_invalidateEEPROM(void)
{
    Memory_writeEEPROM8(EEPROM_VERSION_ADDR, 0xFF);
    Memory_writeEEPROM16(EEPROM_CKSM_H_ADDR, 0xFFFF);
}

//Runs the periodic self-test of the system
void Fusa_runPeriodicSelfCheck(void)
{    
    //Clear WDT
    asm("WDR");
    
    //Get a new ADC reading from the sensor (blocking!)
    uint16_t meas = GasSensor_sampleSensor();
            
#ifdef VIEW_RAW_ADC
    printf("ADC Result: 0x%x\r\n", meas);
#endif
    
    //Run state machine
    switch (sysState)
    {
        case SYS_WARMUP:
        {
            //System is in warm-up
            
            //Sensor is now ready
            if (Application_isSensorReady())
            {
                printf("\r\nWarmup complete.\r\n");
                
                if (GasSensor_isEEPROMValid())
                {
                    //Ready to begin active monitoring
                    
                    //If the alarm is active, jump to alarm
                    if (GasSensor_isTripped())
                    {
                        //Activate the alarm, and transition to a new state
                        Fusa_activateAlarm();
                    }
                    else
                    {
                        sysState = SYS_MONITOR;
                    }
                }
                else
                {
                    //Calibration Required
                    printf("Calibration data not found. Press SW0 to set new zero-point.\r\n");
                    sysState = SYS_CALIBRATE;
                }
            }
            else
            {
                //Has an hour elapsed?
                if (Application_hasHourTicked())
                {
                    //Clear flag
                    Application_clearHourTick();
                    
                    //Print message
                    Application_printHoursRemaining();
                }
            }
            
            break;
        }
        case SYS_CALIBRATE:
        {
            //Need to calibrate
            
            //If SW0 was pressed
            if (SW0_GetValue())
            {
                //Run calibration
                
                printf("Running calibration.\r\n");
                
                if (GasSensor_calibrate())
                {
                    //No errors
                    
                    printf("Calibration complete. System is now ready.\r\n");
                    
                    
                    //Since calibration just completed, it would be odd to immediately switch to SYS_ALARM
                    //So, it's probably safe to go to SYS_MONITOR
                    sysState = SYS_MONITOR;
                }
                else
                {
                    //Something went wrong
                    
                    printf("Calibration failed to complete.\r\n");
                    sysState = SYS_ERROR;
                }
                
            }
            
            break;
        }
        case SYS_MONITOR:
        {
            //Flash the LED
            LED0_SetHigh();
            DELAY_milliseconds(1);
            LED0_SetLow();
            
            //System is running
            printf("[MONITOR] Estimated Ammonia: %d ppm\r\n", GasSensor_convertToPPM(meas));
            
            //Did the alarm activate?
            if (GasSensor_isTripped())
            {
                //Activate the alarm and transition to a new state
                Fusa_activateAlarm();
                
                printf("Alarm is tripped!\r\n");
            }
            else
            {
                //Run self-test
                if (!Fusa_testAC())
                {
                    printf("AC failed self-check.\r\n");
                    sysState = SYS_ERROR;
                }
                else if (SW0_GetValue())
                {
                    //Re-calibrate
                    printf("Ready to recalibrate. Press SW0 to set new zero-point.\r\n");
                    sysState = SYS_CALIBRATE;
                }
                else if (TEST_BUTTON_GetValue())
                {
                    //No errors, and request
                    Fusa_activateAlarm();
                }
            }
            break;
        }
        case SYS_ALARM:
        {
            //System alarm is tripped
            LED0_Toggle();
            
            printf("[ALARM] Estimated Ammonia: %d ppm\r\n", GasSensor_convertToPPM(meas));
            
            //Did the alarm go off?
            if (!GasSensor_isTripped())
            {
                //Deactivate the alarm and transition to SYS_MONITOR
                Fusa_deactivateAlarm();
                
                printf("Alarm has cleared!\r\n");
            }
            
            break;
        }
        case SYS_ERROR:
        case SYS_INIT:
        case SYS_SELF_TEST:
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

//Activate the alarm
void Fusa_activateAlarm(void)
{
    //Enable the Buzzer
    BUZZER_ENABLE();
    
    //Switch to low threshold - concentration must fall to here to clear
    GasSensor_setThresholdLow();
    
    //Switch to alarm state
    sysState = SYS_ALARM;
}

//Deactivate the alarm
void Fusa_deactivateAlarm(void)
{
    //Disable the buzzer
    BUZZER_DISABLE();
    
    //Switch to high threshold - concentration must rise to here to activate
    GasSensor_setThresholdHigh();
    
    //Switch to monitor state
    sysState = SYS_MONITOR;
}