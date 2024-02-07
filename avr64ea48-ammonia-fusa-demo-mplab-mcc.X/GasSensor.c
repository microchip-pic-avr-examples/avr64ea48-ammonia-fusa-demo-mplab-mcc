#include "GasSensor.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include "EEPROM.h"
#include "Application.h"

static float R_L = 0.0;
static bool memValid = false;

void _initRL(uint16_t ref)
{
    //Pre-calculate ADC constant for R_L
    //V_S / V_REF * 2^n (n = ADC Resolution)
    const float K = (SENSOR_BIAS_VOLTAGE / ADC_VREF) * ADC_BITS;
    
    //Sensor resistance at 0 ppm (computed from DS)
    const float R0 = SENSOR_R0;
    
    //Load Resistance
    R_L = R0 / ((K / ref) - 1);
}

//Initialize the constants and parameters for the sensor
void GasSensor_initFromEEPROM(void)
{
    _initRL(GasSensor_getReferenceValue());
    memValid = true;
}

//Returns true if the EEPROM is valid
bool GasSensor_isEEPROMValid(void)
{
    return memValid;
}

//Write the reference value to EEPROM
bool GasSensor_writeEEPROM(uint16_t refValue)
{
    //Write 0x0000 as a placeholder for the Checksum
    if (!Memory_writeEEPROM16(EEPROM_CKSM_H_ADDR, 0x0000))
        return false;
    
    //Write the EEPROM VERSION ID
    if (!Memory_writeEEPROM8(EEPROM_VERSION_ADDR, EEPROM_VERSION_ID))
        return false;

    //Write the Reference Value
    if (!Memory_writeEEPROM16(EEPROM_REF_VALUE_H_ADDR, refValue))
        return false;
    
    //Write real checksum
    if (!Memory_writeEEPROM16(EEPROM_CKSM_H_ADDR, Memory_calculateChecksum()))
        return false;
    
    //Set the memory valid flag
    memValid = true;
    
    return true;
}

//Returns the state of the AC
bool GasSensor_isTripped(void)
{
    //Above max allowable level
    if (AC1_Read() == GAS_SENSOR_LOGIC_TRIPPED)
    {
        return true;
    }
    
    //Long-term duration
    //TODO: Long-term value 
    
    return false;
}

//This function uses the current sensor output as a reference zero, write it to memory, and sets the AC
bool GasSensor_calibrate(void)
{
    //Get the current value
    uint16_t result = GasSensor_getCurrentValue();

    //Write data to EEPROM
    if (!GasSensor_writeEEPROM(result))
        return false;
        
    //Compute R_L
    _initRL(result);
    
    //Find the threshold
    float fraction = R_L / (R_L + SENSOR_ALARM_R0);
    uint16_t setPt = round(fraction * UINT8_MAX);
    
    //If bigger than the max allowed
    if (setPt > UINT8_MAX)
    {
        setPt = 0xFF;
    }
    
    //Set the new DACREF
    Application_setDACREF(setPt);
    
    //Success!    
    return true;
}

//Starts and returns the analog value of the gas sensor
uint16_t GasSensor_getCurrentValue(void)
{
    //PD4, AIN4
    return ADC0_GetConversion(ADC_MUXPOS_AIN4_gc);
}

//Returns the stored reference value
uint16_t GasSensor_getReferenceValue(void)
{
    return Memory_readEEPROM16(EEPROM_REF_VALUE_H_ADDR);
}

//Converts a measurement value into PPM
uint16_t GasSensor_convertToPPM(uint16_t measurement)
{
    //Check for bad conditions
    if (measurement == 0)
    {
        //If the measurement is 0, return max PPM
        return UINT16_MAX;
    }
    else if (R_L <= 0)
    {
        //If the load resistance is not set (error state), return max PPM
        return UINT16_MAX;
    }
    
    const float precalc = (ADC_BITS * SENSOR_BIAS_VOLTAGE) / ADC_VREF;
    
    //Sensor Resistance
    float R_S = ((precalc / measurement) - 1) * R_L;
//    float R_S = (precalc / (float) (measurement));
//    R_S -= 1.0;
//    R_S *= R_L;
    printf("Sensor Resistance = %f\r\n", R_S);

    //Compute ratio against R0
    float ratio = R_S / SENSOR_R0;
    printf("Sensor Ratio (R_S / R_0) = %f\r\n", ratio);
    
    //Constants are from a best-fit plot of the provided sensor data
    //PPM = 0.1282 * result^(-3.833)
    uint16_t result = round(0.1282 * powf(ratio, -3.833));
    
    return result;
}

//Called whenever a rising edge occurs on the AC
void GasSensor_onAlert(void)
{
    
}