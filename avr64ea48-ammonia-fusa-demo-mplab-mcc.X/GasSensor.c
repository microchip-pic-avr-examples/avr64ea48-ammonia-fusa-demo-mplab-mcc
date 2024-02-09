#include "GasSensor.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include "EEPROM.h"
#include "Application.h"

static float R_S0 = 0.0;
static bool memValid = false;

void _initParameters(uint16_t ref)
{
    //Pre-calculate ADC constant for R_L
    //V_S / V_REF * 2^n (n = ADC Resolution)
    const float K = (SENSOR_BIAS_VOLTAGE / ADC_VREF) * ADC_BITS;
    
    //Sensor resistance at 0 ppm (computed from DS)
    const float R_L = LOAD_RESISTANCE;
    
    //Sensor Resistance
    R_S0 = R_L * ((K / ref) - 1);
        
    //Alarm trigger voltage (DACREF)
    float alarmValue = (R_L / (R_L + (R_S0 * ALARM_THRESHOLD))) * SENSOR_BIAS_VOLTAGE;
    
    //Volts per bit resolution of DACREF
    const float DACREF_SENSITIVITY = DACREF_VREF / DACREF_BITS;
    
    //DACREF setpoint
    uint16_t setPt = round(alarmValue / DACREF_SENSITIVITY);
    
    //If bigger than the max allowed
    if (setPt > UINT8_MAX)
    {
        printf("WARNING: Clipping DACREF at maximum.\r\n");
        setPt = 0xFF;
    }
    
#ifdef PRINT_SENSOR_PARAMETERS
    printf("R_S0 = %f\r\n", R_S0);
    printf("Alarm Point = %f V (DACREF = 0x%x)\r\n", alarmValue, setPt);
#endif
    
    //Set the new DACREF
    Application_setDACREF(setPt);
}

//Initialize the constants and parameters for the sensor
void GasSensor_initFromEEPROM(void)
{
    _initParameters(GasSensor_getReferenceValue());
    memValid = true;
}

//Erases the EEPROM
void GasSensor_eraseEEPROM(void)
{
    Memory_writeEEPROM16(EEPROM_CKSM_H_ADDR, 0xFFFF);
    Memory_writeEEPROM8(EEPROM_VERSION_ADDR, 0xFF);
    Memory_writeEEPROM16(EEPROM_REF_VALUE_H_ADDR, 0xFFFF);
}

//Returns true if the EEPROM is valid
bool GasSensor_isEEPROMValid(void)
{
    return memValid;
}

//Write the reference value to EEPROM
bool GasSensor_writeEEPROM(uint16_t refValue)
{
    //Invalidate memory valid flag
    memValid = false;
    
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
    
    return false;
}

//This function uses the current sensor output as a reference zero, write it to memory, and sets the AC
bool GasSensor_calibrate(void)
{
    //Get the current value
    uint16_t result = GasSensor_sampleSensor();

    //Write data to EEPROM
    if (!GasSensor_writeEEPROM(result))
        return false;
        
    //Compute R_L and DACREF
    _initParameters(result);
        
    //Success!    
    return true;
}

//Starts and returns the analog value of the gas sensor
uint16_t GasSensor_sampleSensor(void)
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
    else if (R_S0 <= 0)
    {
        //If the load resistance is not set (error state), return max PPM
        return UINT16_MAX;
    }
    else if (!memValid)
    {
        //EEPROM memory is currently invalid
        return UINT16_MAX;
    }
    
    const float precalc = (ADC_BITS * SENSOR_BIAS_VOLTAGE) / ADC_VREF;
    
    //Sensor Resistance
    float R_S = ((precalc / measurement) - 1) * LOAD_RESISTANCE;
#ifdef PRINT_SENSOR_PARAMETERS
    printf("Sensor Resistance = %f\r\n", R_S);
#endif

    //Compute ratio against R0
    float ratio = R_S / R_S0;
#ifdef PRINT_SENSOR_PARAMETERS
    printf("Sensor Ratio (R_S / R_0) = %f\r\n", ratio);
#endif
    
    //Constants are from a best-fit plot of the provided sensor data
    //PPM = 0.1282 * result^(-3.833)
    uint16_t result = round(0.1282 * powf(ratio, -3.833));
    
    return result;
}

//Called whenever a rising edge occurs on the AC
void GasSensor_onAlert(void)
{
    
}