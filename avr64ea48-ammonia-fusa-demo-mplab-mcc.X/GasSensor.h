#ifndef GASSENSOR_H
#define	GASSENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
    
    //Is the calibration EEPROM valid?
    bool GasSensor_isEEPROMValid(void);
    
    //Returns the state of the AC
    bool GasSensor_isTripped(void);
    
    //Called whenever a rising edge occurs on the AC
    void GasSensor_onAlert(void);

#ifdef	__cplusplus
}
#endif

#endif	/* GASSENSOR_H */

