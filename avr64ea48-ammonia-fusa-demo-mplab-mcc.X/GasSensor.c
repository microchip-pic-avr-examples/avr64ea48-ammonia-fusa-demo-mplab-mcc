#include "GasSensor.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include "EEPROM.h"
#include "Application.h"

static volatile float R_L = 0.0;

bool _writeEEPROM8(uint16_t addr, uint8_t val)
{
    //Write the byte
    nvm_status_t status = EEPROM_Write(addr, val);
    
    //Did an error occur?
    if (status == NVM_ERROR)
    {
        return false;
    }
    
    //Wait for write
    while (EEPROM_IsBusy());
    
    //Verify write
    if (EEPROM_Read(addr) != val)
    {
        return false;
    }
    
    return true;
}

bool _writeEEPROM16(uint16_t addr, uint16_t val)
{
    //High Byte
    if (!_writeEEPROM8(addr, ((val & 0xFF00) >> 8)))
        return false;
    
    //Low Byte
    if (!_writeEEPROM8((addr + 1), (val & 0xFF)))
        return false;
    
    return true;
}

uint8_t _readEEPROM8(uint16_t addr)
{
    return EEPROM_Read(addr);
}

uint16_t _readEEPROM16(uint16_t addr)
{
    uint16_t val;
    
    //Load the high byte
    val = _readEEPROM8(addr);
    
    //Shift Data
    val <<= 8;
    
    //Load the low byte
    val |= _readEEPROM8(addr + 1);
    
    return val;
}

//Is the calibration EEPROM valid?
bool GasSensor_isEEPROMValid(void)
{
    
    return false;
}

//Write the reference value to EEPROM
bool GasSensor_writeEEPROM(uint16_t refValue)
{
    //Write the EEPROM VERSION ID
    if (!_writeEEPROM8(EEPROM_VERSION_ADDR, EEPROM_VERSION))
        return false;

    //Write the Reference Value
    if (!_writeEEPROM16(EEPROM_REF_VALUE_H_ADDR, refValue))
        return false;
    
    //Write 0x0000 as a placeholder for the CRC Value
    if (!_writeEEPROM16(EEPROM_CRC16_H_ADDR, 0x0000))
        return false;
        
    //TODO: CRC Calculation + Write!
    
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
//    if (!GasSensor_writeEEPROM(result))
//        return false;
        
    //Pre-calculate ADC constant for R_L
    //V_S / V_REF * 2^n (n = ADC Resolution)
    const float K = (SENSOR_BIAS_VOLTAGE / ADC_VREF) * ADC_BITS;
    
    //Sensor resistance at 0 ppm (computed from DS)
    const float R0 = SENSOR_R0;
    
    //Load Resistance
    R_L = R0 / ((K / result) - 1);
    printf("Load Resistance = %f\r\n", R_L);
    
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

//Returns the current reference value
uint16_t GasSensor_getReferenceValue(void)
{
    return _readEEPROM16(EEPROM_REF_VALUE_H_ADDR);
}

//Converts a measurement value into PPM
uint16_t GasSensor_convertToPPM(uint16_t measurement)
{
    uint16_t result = UINT16_MAX;
    
    //Check for bad conditions
    if (measurement == 0)
    {
        //If the measurement is 0, return max PPM
        return UINT8_MAX;
    }
    else if (R_L <= 0)
    {
        //If the load resistance is not set (error state), return max PPM
        return UINT8_MAX;
    }
    
    const float precalc = (ADC_BITS * SENSOR_BIAS_VOLTAGE) / ADC_VREF;
    
    //Sensor Resistance
//    volatile float R_S = ((precalc / result) - 1) * R_L;
    volatile float R_S = (precalc / (float) (measurement));
    R_S -= 1.0;
    R_S *= R_L;
    printf("Sensor Resistance = %f\r\n", R_S);

    //Compute ratio against R0
    float ratio = R_S / SENSOR_R0;
    printf("Sensor Ratio (R_S / R_0) = %f\r\n", ratio);
    
    //Constants are from a best-fit plot of the provided sensor data
    //PPM = 0.1282 * result^(-3.833)
    result = round(0.1282 * powf(ratio, -3.833));
    
    return result;
}

//Called whenever a rising edge occurs on the AC
void GasSensor_onAlert(void)
{
    
}