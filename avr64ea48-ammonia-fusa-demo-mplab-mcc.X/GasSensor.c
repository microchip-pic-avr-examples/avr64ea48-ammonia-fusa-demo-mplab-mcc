#include "GasSensor.h"

#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"

//Is the calibration EEPROM valid?
bool GasSensor_isEEPROMValid(void)
{
    
    return false;
}

//Returns the state of the AC
bool GasSensor_isTripped(void)
{
    //Above max allowable level
    if (AC1_Read())
    {
        return true;
    }
    
    //Long-term duration
    //TODO: Long-term value 
    
    return false;
}

//Called whenever a rising edge occurs on the AC
void GasSensor_onAlert(void)
{
    
}