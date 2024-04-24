#include "SENSOR.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include "EEPROM.h"
#include "application.h"
#include "mcc_generated_files/diagnostics/diag_library/memory/non_volatile/diag_eeprom_crc16.h"

typedef enum {
    GAS_SENSOR_INVALID = 0, GAS_SENSOR_LOW, GAS_SENSOR_HIGH
} gas_sensor_threshold_t;

static float R_S0 = 0.0;
static bool memValid = false;

static gas_sensor_threshold_t sensorThreshold = GAS_SENSOR_INVALID;
static uint8_t alarmHighVal, alarmLowVal;
static volatile uint8_t alarmValidate = 0x00;

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
    float alarmValueHigh = (R_L / (R_L + (R_S0 * ALARM_THRESHOLD_HIGH))) * SENSOR_BIAS_VOLTAGE;
    float alarmValueLow = (R_L / (R_L + (R_S0 * ALARM_THRESHOLD_LOW))) * SENSOR_BIAS_VOLTAGE;
    
    //Volts per bit resolution of DACREF
    const float DACREF_SENSITIVITY = DACREF_VREF / DACREF_BITS;
    
    //DACREF setpoint
    uint16_t setPtHigh = round(alarmValueHigh / DACREF_SENSITIVITY);
    uint16_t setPtLow = round(alarmValueLow / DACREF_SENSITIVITY);
    
    //If bigger than the max allowed
    if (setPtHigh > UINT8_MAX)
    {
        printf("WARNING: setPtHigh, DACREF at maximum.\r\n");
        setPtHigh = 0xFF;
    }
    
    //If bigger than the max allowed
    if (setPtLow > UINT8_MAX)
    {
        printf("WARNING: setPtLow, DACREF at maximum.\r\n");
        setPtLow = 0xFF;
    }
    
#ifdef PRINT_SENSOR_INIT_DATA
    printf("R_S0 = %f\r\n", R_S0);
    printf("Alarm Point High = %f V (DACREF = 0x%x)\r\n", alarmValueHigh, setPtHigh);
    printf("Alarm Point Low = %f V (DACREF = 0x%x)\r\n", alarmValueLow, setPtLow);
#endif
    
    //Store the DAC values
    alarmHighVal = (uint8_t) setPtHigh;
    alarmLowVal = (uint8_t) setPtLow;
    
    //Variable used to verify the values have not been corrupted
    alarmValidate = alarmHighVal ^ alarmLowVal;
    
    //Default to the high threshold
    SENSOR_ThresholdHighSet();
}

//Initialize the constants and parameters for the sensor
void SENSOR_EEPROMInit(void)
{
    _initParameters(SENSOR_ReferenceValueGet());
    memValid = true;
}

//Erases the EEPROM
void SENSOR_EEPROMErase(void)
{
    EEPROM_WordWrite(EEPROM_CKSM_H_ADDR, 0xFFFF);
    EEPROM_WordWrite(EEPROM_REF_VALUE_H_ADDR, 0xFFFF);
}

//Sets the sensor to the low range
void SENSOR_ThresholdLowSet(void)
{
    //Set the new DACREF
    APP_DACREFSet(alarmLowVal);
    sensorThreshold = GAS_SENSOR_LOW;
}
    
//Sets the sensor to the high range
void SENSOR_ThresholdHighSet(void)
{
    //Set the new DACREF
    APP_DACREFSet(alarmHighVal);
    sensorThreshold = GAS_SENSOR_HIGH;
}

//Returns true if the EEPROM is valid
bool SENSOR_IsEEPROMValid(void)
{
    return memValid;
}

//Write the reference value to EEPROM
bool SENSOR_EEPROMWrite(uint16_t refValue)
{
    //Invalidate memory valid flag
    memValid = false;
    
    //Write 0x0000 as a placeholder for the Checksum
    if (!EEPROM_WordWrite(EEPROM_CKSM_H_ADDR, 0x0000))
        return false;
    
    //Write Version ID
    if (!EEPROM_ByteWrite(EEPROM_VERSION_ADDR, EEPROM_VERSION_ID))
        return false;
    
    //Write the Reference Value
    if (!EEPROM_WordWrite(EEPROM_REF_VALUE_H_ADDR, refValue))
        return false;
    
#ifdef FUSA_ENABLE_EEPROM_SIMPLE_CHECKSUM
    //Write the real checksum
    if (!EEPROM_WordWrite(EEPROM_CKSM_H_ADDR, EEPROM_ChecksumCalculate()))
        return false;
#else
    //Write the real CRC checksum
    if (DIAG_EEPROM_CalculateStoreCRC(DIAG_EEPROM_START_ADDR, DIAG_EEPROM_LENGTH,
            DIAG_EEPROM_CRC_STORE_ADDR) != DIAG_PASS)
        return false;
    
    printf("CRC Checksum = 0x%x\r\n", EEPROM_WordRead(EEPROM_CKSM_H_ADDR));
    
    if (DIAG_EEPROM_ValidateCRC(DIAG_EEPROM_START_ADDR, DIAG_EEPROM_LENGTH,
            DIAG_EEPROM_CRC_STORE_ADDR) != DIAG_PASS)
        return false;
    
    printf("EEPROM Verified\r\n");
    
#endif
    
    //Set the memory valid flag
    memValid = true;
    
    return true;
}

//Returns the state of the AC
bool SENSOR_IsTripped(void)
{
    //Above max allowable level
    if (AC1_Read() == GAS_SENSOR_LOGIC_TRIPPED)
    {
        return true;
    }
    
    return false;
}

//This function uses the current sensor output as a reference zero, write it to memory, and sets the AC
bool SENSOR_Calibrate(void)
{
    //Get the current value
    uint16_t result = SENSOR_SampleSensor();

    //Write data to EEPROM
    if (!SENSOR_EEPROMWrite(result))
    {
        printf("An error occurred when writing EEPROM.\r\n");
        return false;
    }
        
    //Compute R_L and DACREF
    _initParameters(result);
        
    //Success!    
    return true;
}

//Verifies the DACREF value is set correctly
diag_result_t SENSOR_SetpointVerify(void)
{
    if (sensorThreshold == GAS_SENSOR_LOW)
    {
        if (APP_DACREFGet() != alarmLowVal)
        {
            return DIAG_FAIL;
        }
        else if (alarmValidate != (alarmLowVal ^ alarmHighVal))
        {
            return DIAG_FAIL;
        }
    }
    else if (sensorThreshold == GAS_SENSOR_HIGH)
    {
        if (APP_DACREFGet() != alarmHighVal)
        {
            return DIAG_FAIL;
        }
        else if (alarmValidate != (alarmLowVal ^ alarmHighVal))
        {
            return DIAG_FAIL;
        }
    }
    
    return DIAG_PASS;
}

//Starts and returns the analog value of the gas sensor
uint16_t SENSOR_SampleSensor(void)
{
    //PD4, AIN4
    return ADC0_GetConversion(ADC_MUXPOS_AIN4_gc);
}

//Returns the stored reference value
uint16_t SENSOR_ReferenceValueGet(void)
{
    return EEPROM_WordRead(EEPROM_REF_VALUE_H_ADDR);
}

//Converts a measurement value into PPM
uint16_t SENSOR_MeasurementConvert(uint16_t measurement)
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
    printf("Sensor Ratio (R_S / R_0) = %f\r\n\r\n", ratio);
#endif
    
    //Constants are from a best-fit plot of the provided sensor data
    //PPM = 0.1282 * result^(-3.833)
    uint16_t result = round(0.1282 * powf(ratio, -3.833));
    
    return result;
}
