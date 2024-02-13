#include "Fusa.h"

#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"

#include "Application.h"
#include "GasSensor.h"
#include "EEPROM.h"

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

void FLASH_WriteFlashBlock(flash_address_t flash_address, uint8_t *data, size_t size)
{
    volatile flash_address_t flashStartPageAddress;
    volatile uint16_t flashAddressOffset;
    volatile flash_data_t flashWriteData[PROGMEM_PAGE_SIZE];

    //Get the starting address of the page containing the given address
    flashStartPageAddress = FLASH_PageAddressGet(flash_address);

    //Read entire row
    for (flashAddressOffset = 0; flashAddressOffset < PROGMEM_PAGE_SIZE; flashAddressOffset++)
    {
        flashWriteData[flashAddressOffset] = FLASH_Read(flashStartPageAddress + flashAddressOffset);
    }

    //Get offset from the starting address of the page
    flashAddressOffset = FLASH_PageOffsetGet(flash_address);

    //Update data of required size
    for (uint8_t i=0; i<size; i++)
    {
        flashWriteData[flashAddressOffset + i] = *(data+ i);
    }

    //Wait for any pending operations
    while (FLASH_IsBusy());
    
    //Erase the entire Flash page
    if (FLASH_PageErase(flashStartPageAddress) == NVM_ERROR)
    {
        printf("NVM Erase Failure\r\n");
    }
    
    //Wait for Flash Erase to Complete
    while (FLASH_IsBusy());
    
    NVM_StatusClear();

    //Write data to the Flash row
    if (FLASH_RowWrite(flashStartPageAddress, flashWriteData))
    {
        printf("NVM Write Failure\r\n");
    }
    
    NVM_StatusClear();
    
    //Wait for Flash Write to Complete
    while (FLASH_IsBusy());
}

//Runs a self-test of the system
bool Fusa_runStartupSelfTest(void)
{
    printf("\r\nFlash Memory Checksum = 0x%lx\r\n", Fusa_getChecksumFromPFM());
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
    if (SW0_GetValue())
    {
        //Erase requested by user
        GasSensor_eraseEEPROM();
        
        printf("ERASED\r\n");
    }
    else if (Fusa_testEEPROM())
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
    
    //Save current state
    SystemState prevState = sysState;
    
    //Switch to test state
    sysState = SYS_TEST;
    
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
bool Fusa_testMemory(void)
{    
    uint32_t sum = Fusa_getChecksumFromPFM() ^ 0xFFFFFFFF;
    
    uint8_t b[4];
    b[3] = (uint8_t) ((sum & 0xFF000000) >> 24);
    b[2] = (uint8_t) ((sum & 0x00FF0000) >> 16);
    b[1] = (uint8_t) ((sum & 0x0000FF00) >> 8);
    b[0] = (uint8_t) ((sum & 0x000000FF));
    
    FLASH_WriteFlashBlock(CRC_ADDRESS_START, b, 4);
    
    printf("New Checksum from PFM: 0x%lx\r\n", Fusa_getChecksumFromPFM());
    
    return Application_runCRC();
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
        
    //Verify Checksum
    if (Memory_calculateChecksum() != EEPROM_CHECKSUM_GOOD)
    {
        return false;
    }
    
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
                    sysState = SYS_MONITOR;
                }
                else
                {
                    //Calibration Required
                    printf("Calibration data not found. Press SW0 to set new zero-point.\r\n");
                    sysState = SYS_CALIBRATE;
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
                    
                    //If the alarm is active, jump to alarm
                    if (GasSensor_isTripped())
                    {
                        sysState = SYS_ALARM;
                    }
                    else
                    {
                        sysState = SYS_MONITOR;
                    }
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
            //Blink the LED
            LED0_Toggle();
            
            //System is running
            printf("[MONITOR] Estimated Ammonia: %d ppm\r\n", GasSensor_convertToPPM(meas));
            
            //Did the alarm activate?
            if (GasSensor_isTripped())
            {
                printf("Alarm is tripped!\r\n");
                sysState = SYS_ALARM;
            }
            else
            {
                //Run self-test
                if (!Fusa_testAC())
                {
                    printf("AC failed self-check.\r\n");
                    sysState = SYS_ERROR;
                }
            }
                
            break;
        }
        case SYS_ALARM:
        {
            //System alarm is tripped
            
            printf("[ALARM] Estimated Ammonia: %d ppm\r\n", GasSensor_convertToPPM(meas));
            
            //Did the alarm go off?
            if (!GasSensor_isTripped())
            {
                printf("Alarm has cleared!\r\n");
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