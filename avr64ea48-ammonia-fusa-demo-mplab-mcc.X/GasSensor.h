#ifndef GASSENSOR_H
#define	GASSENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
//Prints the measured sensor parameters
#define PRINT_SENSOR_PARAMETERS
    
//This is the alarm threshold
//Set to the 50 ppm point on the MQ-137 response curve
#define ALARM_THRESHOLD 0.205
    
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
#define SENSOR_ALARM_R0 (SENSOR_R0 * ALARM_THRESHOLD)
    
//Logic for the gas sensor
#define GAS_SENSOR_LOGIC_TRIPPED false
#define GAS_SENSOR_LOGIC_NOT_TRIPPED true
    
    //Initialize the constants and parameters for the sensor
    void GasSensor_initFromEEPROM(void);
    
    //Erases the EEPROM
    void GasSensor_eraseEEPROM(void);
    
    //Returns true if the EEPROM is valid
    bool GasSensor_isEEPROMValid(void);
    
    //Write the reference value to EEPROM
    bool GasSensor_writeEEPROM(uint16_t refValue);
    
    //Returns the state of the AC
    bool GasSensor_isTripped(void);
    
    //This function uses the current sensor output as a reference zero, write it to memory, and sets the AC
    bool GasSensor_calibrate(void);
        
    //Starts and returns the analog value of the gas sensor
    uint16_t GasSensor_sampleSensor(void);
    
    //Returns the stored reference value
    uint16_t GasSensor_getReferenceValue(void);
    
    //Converts a measurement value into PPM
    uint16_t GasSensor_convertToPPM(uint16_t measurement);
    
    //Called whenever a rising edge occurs on the AC
    void GasSensor_onAlert(void);

#ifdef	__cplusplus
}
#endif

#endif	/* GASSENSOR_H */

