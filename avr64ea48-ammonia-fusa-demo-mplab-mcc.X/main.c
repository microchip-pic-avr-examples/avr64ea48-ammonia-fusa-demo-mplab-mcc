 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include "Fusa.h"
#include "Application.h"

/*
    Main application
*/

//This is the checksum used for the system memory
//Inserted by the compiler at runtime
const uint32_t flashChecksum = 0x00000000;

int main(void)
{
    SYSTEM_Initialize();
    
    //Start the sensor heater
    HEATER_SetHigh();
    
    printf("AVR64EA48 Ammonia Gas Functional Safety Demo\r\n");
    
#ifdef DEVELOP_MODE
    printf("WARNING: Device is in develop mode. System will power-up if errors occur and skip sensor warm-up period.\r\nDO NOT USE FOR PRODUCTION\r\n");
#endif
    
    //Run system self-test
    Fusa_runStartupSelfTest();
    
    //Interrupt callback for an hour passing
    RTC_SetOVFIsrCallback(Application_onHourTick);
    
    //Interrupt callback for the PIT
    RTC_SetPITIsrCallback(Application_onPITTick);
    
    //Enable interrupts
    sei();
    
    //Clear self-test flag
    Application_clearSelfTestFlag();
    
    while(1)
    {
        //Do we need to self test and clear WDT?
        if (Application_shouldSelfTest())
        {
            //Clear self-test flag
            Application_clearSelfTestFlag();
            
            //Run periodic self-check
            Fusa_runPeriodicSelfCheck();
        }
    }    
}