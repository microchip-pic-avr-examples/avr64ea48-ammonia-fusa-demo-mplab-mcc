#include "fusa.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"

#include "application.h"
#include "SENSOR.h"
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

static volatile system_state_t sysState = SYS_ERROR;
static volatile system_state_t sysStateCheck = SYS_ERROR;

//Sets the system state
void FUSA_SystemStateSet(system_state_t state)
{
    sysState = state;
    sysStateCheck = state;
}

//Returns DIAG_FAIL if unable to verify the system state
diag_result_t FUSA_SystemStateVerify(void)
{
    if (sysState != sysStateCheck)
    {
        return DIAG_FAIL;
    }
    return DIAG_PASS;
}

//Runs a self-test of the system
bool FUSA_StartupSelfTestRun(void)
{
    printf("\r\nRunning Self Test\r\n");
    
    //Run the buzzer during self-test
    BUZZER_ENABLE();
    
    //Set the state to INIT
    //If an error occurs, then it will be moved to ERROR
    //If in INIT at the end of this function, then it will be moved to warm-up
    FUSA_SystemStateSet(SYS_INIT);
    
    if (SW0_GetValue())
    {
        //Erase requested by user
        SENSOR_EEPROMErase();
                
        printf("Configuration ERASED\r\n");
    }
    
    printf("Flash Memory Checksum = 0x%lx\r\n\r\n", FUSA_PFMChecksumGet());
    
    //Check CPU
    printf("Testing CPU...");
    if (FUSA_CPUTest())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        FUSA_SystemStateSet(SYS_ERROR);
    }
    
    //Get the WDT Result
    printf("Testing WDT...");
    if (FUSA_WDTTest())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        FUSA_SystemStateSet(SYS_ERROR);
    }

    //Get the SRAM Result
    printf("Testing SRAM...");
    if (FUSA_SRAMTest())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        FUSA_SystemStateSet(SYS_ERROR);
    }
    
    //Check Memory
    printf("Testing Memory Integrity...");
    if (FUSA_FlashTest())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        FUSA_SystemStateSet(SYS_ERROR);
    }
    
    //Check Comparator
    printf("Testing Analog Comparator...");
    if (FUSA_ACTest())
        printf(PASS_STRING);
    else
    {
        printf(FAIL_STRING);
        FUSA_SystemStateSet(SYS_ERROR);
    }
        
    //Check EEPROM for valid constants
    printf("Calibration data...");
    if (FUSA_EEPROMTest())
    {        
        //Init R_L constant from EEPROM
        SENSOR_EEPROMInit();
        
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
        FUSA_SystemStateSet(SYS_WARMUP);
    }
#endif
    
    //Disable the buzzer at the end of self-test
    BUZZER_DISABLE();
    
    //Check to see if any error occurred
    if (sysState == SYS_ERROR)
    {
        printf("Self Test Failed\r\n\r\n");
        FUSA_HandleSystemFailure();
    }
    else
    {
        printf("Self Test Complete\r\n\r\nBeginning %u hour sensor warmup\r\n", WARM_UP_HOURS);
        FUSA_SystemStateSet(SYS_WARMUP);
    }
    
    return false;
}

//Run a CPU test
bool FUSA_CPUTest(void)
{
    if (DIAG_CPU_Registers() == DIAG_PASS)
    {
        return true;
    }
    return false;
}
    
//Get the WDT Test Results
bool FUSA_WDTTest(void)
{
    if (DIAG_WDT_GetResult() == DIAG_PASS)
    {
        return true;
    }
    return false;
}

//Test the comparator
bool FUSA_ACTest(void)
{
    /* Test Setup
     * Verify DACREF and CMP are operational by using DAC0's output
     * 
     * 1. Save current system state.
     * 2. Set state to SYS_SELF_TEST.
     * 3. Switch In+ to DAC0 Output (PD6, AINP3)
     * 4. Set DAC0 to a value greater than DACREF
     * 5. Wait for value to stabilize 
     * 6. Check comparator output is HIGH
     * 7. Set DAC0 to a value lower than DACREF
     * 8. Wait for value to stabilize
     * 9. Check comparator output is LOW
     * 10. Switch In+ back to Ammonia Sensor (PD4, AINP2)
     * 11. Return to previous state
     */
    
    //Save current state
    system_state_t prevState = sysState;
    
    //Switch to test state
    FUSA_SystemStateSet(SYS_SELF_TEST);
    
    //Switch AC0 Input
    APP_DACConnect();
    
    //DAC0 is 10-bit, DACREF is 8-bit
    
    //Set to DACREF + MARGIN
    int16_t testVal = (APP_DACREFGet() << 2) + TEST_MARGIN;
    
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
        FUSA_SystemStateSet(SYS_ERROR);
        return false;
    }
    
    //Set to DACREF - TEST_MARGIN
    testVal = (APP_DACREFGet() << 2) - TEST_MARGIN;
    
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
        FUSA_SystemStateSet(SYS_ERROR);
        return false;
    }
    
    //Switch AC back to Sensor
    APP_SensorConnect();
    
    //Restore system state
    FUSA_SystemStateSet(prevState);
    
    return true;
}

//Run a memory self-check
bool FUSA_FlashTest(void)
{    
#ifndef FUSA_ENABLE_FLASH_HW_SCAN
    //Class B Library Mode
    if (DIAG_FLASH_ValidateCRC(DIAG_FLASH_START_ADDR, (DIAG_FLASH_CRC_STORE_ADDR), DIAG_FLASH_CRC_STORE_ADDR)
            == DIAG_PASS)
    {
        return true;
    }
    return false;
#else
    //Hardware Mode
    return APP_HardwareCRCRun();
#endif
}

//Run an SRAM self-test
bool FUSA_SRAMTest(void)
{
    //Did the SRAM test pass?
    if (DIAG_SRAM_MarchGetStartupResult() == DIAG_PASS)
    {
        return true;
    }
    return false;
}

//Run a checksum of the EEPROM
bool FUSA_EEPROMTest(void)
{
    /* Test Procedure:
     * 1. Verify the EEPROM Version ID matches expected
     * 2. Verify the EEPROM Checksum
     */
    
    //Verify Version ID
    if (EEPROM_ByteRead(EEPROM_VERSION_ADDR) != EEPROM_VERSION_ID)
    {
        return false;
    }
        
#ifdef FUSA_ENABLE_EEPROM_SIMPLE_CHECKSUM
    //Verify Checksum (Simple)
    if (EEPROM_ChecksumCalculate() != EEPROM_CHECKSUM_GOOD)
    {
        return false;
    }
#else
    
    //Verify a CRC Checksum w/ Class B libraries
    if (DIAG_EEPROM_ValidateCRC(DIAG_EEPROM_START_ADDR, DIAG_EEPROM_LENGTH,
            DIAG_EEPROM_CRC_STORE_ADDR) != DIAG_PASS)
    {
        return false;
    }
#endif
    
    return true;
}

//Gets the 32-bit CRC from memory
uint32_t FUSA_PFMChecksumGet(void)
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
void FUSA_EEPROMInvalidate(void)
{
    EEPROM_ByteWrite(EEPROM_VERSION_ADDR, 0xFF);
    EEPROM_WordWrite(EEPROM_CKSM_H_ADDR, 0xFFFF);
}

//Runs the periodic self-test of the system
void FUSA_PeriodicSelfCheckRun(void)
{    
    static bool prevButtonState = false;
    bool isPressed = false;
    
    //Clear WDT
    asm("WDR");
    
    //Get a new ADC reading from the sensor (blocking!)
    uint16_t meas = SENSOR_SampleSensor();
            
#ifdef VIEW_RAW_ADC
    printf("ADC Result: 0x%x\r\n", meas);
#endif
    
    //Test SRAM
    if (DIAG_SRAM_MarchPeriodic() != DIAG_PASS)
    {
        printf("SRAM Failed Self-Test\r\n");
        FUSA_SystemStateSet(SYS_ERROR);
    }
    
    //Verify the State Machine Variable
    if (FUSA_SystemStateVerify() != DIAG_PASS)
    {
        printf("State Machine RAM Error\r\n");
        FUSA_SystemStateSet(SYS_ERROR);
    }
    
    //Verify the DACREF Value
    if (SENSOR_SetpointVerify() != DIAG_PASS)
    {
        printf("DACREF Register Error\r\n");
        FUSA_SystemStateSet(SYS_ERROR);
    }
    
    if (!FUSA_CPUTest())
    {
        printf("CPU Failure\r\n");
        FUSA_SystemStateSet(SYS_ERROR);
    }
    
    //Simple one-shot button handler
    if (SW0_GetValue())
    {
        //Button is pressed - was it pressed since the last check
        if (!prevButtonState)
        {
            //Recently pressed
            isPressed = true;
        }
    }
    
    //Update state
    prevButtonState = SW0_GetValue();
    
    //Run state machine
    switch (sysState)
    {
        case SYS_WARMUP:
        {
            //System is in warm-up
            
            //Sensor is now ready
            if (APP_IsSensorReady())
            {
                printf("\r\nWarmup complete.\r\n");
                
                if (SENSOR_IsEEPROMValid())
                {
                    //Ready to begin active monitoring
                    
                    //If the alarm is active, jump to alarm
                    if (SENSOR_IsTripped())
                    {
                        //Activate the alarm, and transition to a new state
                        FUSA_AlarmActivate();
                    }
                    else
                    {
                        FUSA_SystemStateSet(SYS_MONITOR);
                    }
                }
                else
                {
                    //Calibration Required
                    printf("Calibration data not found. Press SW0 to set new zero-point.\r\n");
                    FUSA_SystemStateSet(SYS_CALIBRATE);
                }
            }
            else
            {
                //Has an hour elapsed?
                if (APP_HasHourTicked())
                {
                    //Print message
                    APP_RemainingHoursPrint();
                }
            }
            
            break;
        }
        case SYS_CALIBRATE:
        {
            //Need to calibrate
            
            //If SW0 was pressed
            if (isPressed)
            {
                //Run calibration
                
                printf("Running calibration.\r\n");
                
                if (SENSOR_Calibrate())
                {
                    //No errors
                    
                    printf("Calibration complete. System is now ready.\r\n");
                    
                    
                    //Since calibration just completed, it would be odd to immediately switch to SYS_ALARM
                    //So, it's probably safe to go to SYS_MONITOR
                    FUSA_SystemStateSet(SYS_MONITOR);
                }
                else
                {
                    //Something went wrong
                    
                    printf("Calibration failed to complete.\r\n");
                    FUSA_SystemStateSet(SYS_ERROR);
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
            printf("[MONITOR] Estimated Ammonia: %d ppm\r\n", SENSOR_MeasurementConvert(meas));
            
            //Did the alarm activate?
            if (SENSOR_IsTripped())
            {
                //Activate the alarm and transition to a new state
                FUSA_AlarmActivate();
                
                printf("Alarm is tripped!\r\n");
            }
            else
            {
                //Run self-test
                if (!FUSA_ACTest())
                {
                    printf("AC failed self-check.\r\n");
                    FUSA_SystemStateSet(SYS_ERROR);
                }
                else if (isPressed)
                {
                    //Re-calibrate
                    printf("Ready to recalibrate. Press SW0 to set new zero-point.\r\n");
                    FUSA_SystemStateSet(SYS_CALIBRATE);
                }
                else if (TEST_BUTTON_GetValue())
                {
                    //No errors, and request
                    FUSA_AlarmActivate();
                }
            }
            break;
        }
        case SYS_ALARM:
        {
            //System alarm is tripped
            LED0_Toggle();
            
            printf("[ALARM] Estimated Ammonia: %d ppm\r\n", SENSOR_MeasurementConvert(meas));
            
            //Did the alarm go off?
            if (!SENSOR_IsTripped())
            {
                //Deactivate the alarm and transition to SYS_MONITOR
                FUSA_AlarmDeactivate();
                
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
            FUSA_HandleSystemFailure();
            break;
        }

    }
}

//Periodically scans the FLASH
void FUSA_PeriodicMemoryScanRun(void)
{
    if (!FUSA_FlashTest())
    {
        //Faulty FLASH
        printf("FLASH has failed self test\r\n");
        
#ifndef DEVELOP_MODE
        FUSA_SystemStateSet(SYS_ERROR);
        FUSA_HandleSystemFailure();
#endif
    }
    else if (sysState == SYS_MONITOR)
    {
        //Verify EEPROM if in the run state
        if (!FUSA_EEPROMTest())
        {
            printf("EERPOM has failed self test\r\n");
            
            FUSA_SystemStateSet(SYS_CALIBRATE);
        }
    }
}

//Infinite loop for a system failure
void FUSA_HandleSystemFailure(void)
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
void FUSA_AlarmActivate(void)
{
    //Enable the Buzzer
    BUZZER_ENABLE();
    
    //Switch to low threshold - concentration must fall to here to clear
    SENSOR_ThresholdLowSet();
    
    //Switch to alarm state
    FUSA_SystemStateSet(SYS_ALARM);
}

//Deactivate the alarm
void FUSA_AlarmDeactivate(void)
{
    //Disable the buzzer
    BUZZER_DISABLE();
    
    //Switch to high threshold - concentration must rise to here to activate
    SENSOR_ThresholdHighSet();
    
    //Switch to monitor state
    FUSA_SystemStateSet(SYS_MONITOR);
}