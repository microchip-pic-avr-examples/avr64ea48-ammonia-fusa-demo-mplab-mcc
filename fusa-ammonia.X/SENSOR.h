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

#ifndef SENSOR_H
#define	SENSOR_H

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
    void SENSOR_EEPROMInit(void);
    
    //Erases the EEPROM
    void SENSOR_EEPROMErase(void);
    
    //Sets the sensor to the low range
    void SENSOR_ThresholdLowSet(void);
    
    //Sets the sensor to the high range
    void SENSOR_ThresholdHighSet(void);
    
    //Returns true if the EEPROM is valid
    bool SENSOR_IsEEPROMValid(void);
    
    //Write the reference value to EEPROM
    bool SENSOR_EEPROMWrite(uint16_t refValue);
    
    //Returns the state of the AC
    bool SENSOR_IsTripped(void);
    
    //This function uses the current sensor output as a reference zero, write it to memory, and sets the AC
    bool SENSOR_Calibrate(void);
    
    //Verifies the DACREF value is set correctly
    diag_result_t SENSOR_SetpointVerify(void);
        
    //Starts and returns the analog value of the gas sensor
    uint16_t SENSOR_SampleSensor(void);
    
    //Returns the stored reference value
    uint16_t SENSOR_ReferenceValueGet(void);
    
    //Converts a measurement value into PPM
    uint16_t SENSOR_MeasurementConvert(uint16_t measurement);

#ifdef	__cplusplus
}
#endif

#endif	/* SENSOR_H */

