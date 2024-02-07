#ifndef GASSENSOR_H
#define	GASSENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
//This is the alarm threshold
//Set to the 50 ppm point on the MQ-137 response curve
#define ALARM_THRESHOLD 0.205
    
//This is the sensor resistance at 0ppm, calculated from the provided datasheet
#define SENSOR_R0 28871.42857
    
//Bias voltage on the sensor
#define SENSOR_BIAS_VOLTAGE 5.0
    
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
    
    //Returns true if the EEPROM is valid
    bool GasSensor_isEEPROMValid(void);
    
    //Write the reference value to EEPROM
    bool GasSensor_writeEEPROM(uint16_t refValue);
    
    //Returns the state of the AC
    bool GasSensor_isTripped(void);
    
    //This function uses the current sensor output as a reference zero, write it to memory, and sets the AC
    bool GasSensor_calibrate(void);
        
    //Starts and returns the analog value of the gas sensor
    uint16_t GasSensor_getCurrentValue(void);
    
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

