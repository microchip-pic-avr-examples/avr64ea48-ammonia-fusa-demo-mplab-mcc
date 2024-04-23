#ifndef GASSENSOR_H
#define	GASSENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>

#include "mcc_generated_files/diagnostics/diag_common/diag_result_type.h"
    
//Prints the measured sensor parameters
#define PRINT_SENSOR_PARAMETERS
    
//Prints the sensor constants
//#define PRINT_SENSOR_INIT_DATA
    
//This is the alarm HIGH threshold
//Set to the 50 ppm point on the MQ-137 response curve
#define ALARM_THRESHOLD_HIGH 0.205
    
//This is the alarm LOW threshold
//Set to the 30 ppm point on the MQ-137 response curve
#define ALARM_THRESHOLD_LOW 0.240
    
//This is the load resistance
#define LOAD_RESISTANCE 100.0
    
//Bias voltage on the sensor
#define SENSOR_BIAS_VOLTAGE 5.0
    
//DACREF Parameters
#define DACREF_VREF 2.048
#define DACREF_BITS 256
    
//ADC Parameters
#define ADC_VREF 2.048    
#define ADC_BITS 4096

//This is the sensor resistance at the alarm point
#define SENSOR_ALARM_R0 (SENSOR_R0 * ALARM_THRESHOLD_HIGH)
    
//Logic for the gas sensor
#define GAS_SENSOR_LOGIC_TRIPPED false
#define GAS_SENSOR_LOGIC_NOT_TRIPPED true
    
    //Initialize the constants and parameters for the sensor
    void GasSensor_EEPROMInit(void);
    
    //Erases the EEPROM
    void GasSensor_EEPROMErase(void);
    
    //Sets the sensor to the low range
    void GasSensor_ThresholdLowSet(void);
    
    //Sets the sensor to the high range
    void GasSensor_ThresholdHighSet(void);
    
    //Returns true if the EEPROM is valid
    bool GasSensor_IsEEPROMValid(void);
    
    //Write the reference value to EEPROM
    bool GasSensor_EEPROMWrite(uint16_t refValue);
    
    //Returns the state of the AC
    bool GasSensor_IsTripped(void);
    
    //This function uses the current sensor output as a reference zero, write it to memory, and sets the AC
    bool GasSensor_Calibrate(void);
    
    //Verifies the DACREF value is set correctly
    diag_result_t GasSensor_SetpointVerify(void);
        
    //Starts and returns the analog value of the gas sensor
    uint16_t GasSensor_SampleSensor(void);
    
    //Returns the stored reference value
    uint16_t GasSensor_ReferenceValueGet(void);
    
    //Converts a measurement value into PPM
    uint16_t GasSensor_MeasurementConvert(uint16_t measurement);

#ifdef	__cplusplus
}
#endif

#endif	/* GASSENSOR_H */

