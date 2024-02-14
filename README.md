<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# Ammonia Gas Sensor with AVR64EA48

This application is an example ofs how to develop a class B Functional Safety (FuSa) application using the AVR64EA48 family of microcontrollers.

## Warning

**This example is for demonstration purposes only, and is not qualified, tested or verified to meet Class B safety standards.**

## Related Documentation

- [Introduction to Functional Safety](https://mu.microchip.com/introduction-to-functional-safety)
- [Class B Diagnostic Libraries for Functional Safety](https://mu.microchip.com/class-b-diagnostic-libraries-for-functional-safety)
- [MQ137 Ammonia Sensor Product Page](https://www.winsen-sensor.com/product/mq137.html)

## Software Used


- [MPLAB® X IDE 6.15](#)
- [MPLAB XC8 v2.45 or newer](#)
- [MPLAB Code Configurator (MCC)](#)
- AVR-Ex_DFP v2.8.189 or newer

## Hardware Used

- [AVR64EA48 Curiosity Nano Evaluation Kit (EV66E56A)](#)
- [Curiosity Nano Base for Click Boards&trade; (AC164162)](#)
- [Ammonia Click (MIKROE-4151)](https://www.mikroe.com/ammonia-click)
- [(Optional) BUZZ 2 Click (MIKROE-2720)](https://www.mikroe.com/buzz-2-click)
- [2x2 Key Click (MIKROE-2152)](https://www.mikroe.com/2x2-key-click)
- Ammonia Solution (such as household glass cleaner)

## Setup

**TODO: Image**

### Hardware Setup
1. With the power off, insert the AVR64EA48 Curiosity Nano into the Curiosity Nano Base.
2. In Slot 1, insert the Ammonia Click.
2. In Slot 2, insert the Buzz 2 Click, if using.
3. In Slot 3, insert the 2x2 Key Click.
4. Connect the Curiosity Nano to the Computer with a USB cable.
5. Program the Curiosity Nano with the `Free` or `Pro` configuration. (See Program Setup for more information.)
6. Wait for the sensor to warm-up.
7. Once warmed up, press and hold SW0 until the calibration completes.
8. Spray a solution containing ammonia near the sensor or apply the liquid to a towel or cloth and waft it over the element. The sensor should react to the change in environmental conditions within a few seconds.

### Program Configuration Setup

There are 4 configurations of the program inside of the IDE. **The sensor requires a 24 hour warm-up time before becoming stable.** If the microcontroller is power cycled during programming, the timer will restart. For evaluation purposes, only `free` and `pro` configurations should be used as these are the "production" versions. 

For development, only `develop` and `develop_no_cksm` should be used. These development versions do not enforce the 24 hour warm-up time, will power-up even if the system fails self-check, and retain EEPROM values across programming cycles. Note, only the `develop_no_cksm` version is capable of entering debug mode, but will fail the flash checksum on startup. **The user is responsible for ensuring the sensor has warmed up in these modes.**

Please consult the table below to determine which configuration to use. **For initial evaluation purposes, `free` or `pro` is recommended.**

| Configuration | Optimization Level | Debug Mode Capable | 24 Hour Warm-Up Time Enforced? | EEPROM Retained | Checksum Valid
| --- | ---- | ---- | ---- | ---- | ----- 
| free | 1 | No | Yes | No | Yes
| pro | S | No | Yes | No | Yes
| develop | 1 | No | No | Yes | Yes
| develop_no_cksm | 1 | Yes | No | Yes | No

#### Setting Program Configuration

1. Open the project inside of MPLAB X IDE
2. At the top left side of the screen, press the white dropdown box (develop in the image).  
![Program Configuration Dropdown](./images/programConfig.PNG)  
3. Select the desired configuration from the list.

### Data Visualizer Setup

1. In MPLAB X IDE, press the Data Visualizer Icon (below) in the top toolbar.  
![Data Visualizer Icon](./images/dvIcon.PNG)  
2. On the left side of the screen, click the gear icon next to the COM port. The port number may vary.  
![Port Number](./images/portNumber.PNG)  
3. Set the baud rate to 115200. No other changes are needed.
4. Press the play button next to the gear.
5. When prompted, press "Send to Terminal", then close.  

## System States

This application is controlled by a state machine, shown below. 

TODO: State Machine Image

### SYS_INIT

This state is used for initializing the system on Power-on-Reset (POR). The `SYS_INIT` state only is active during the function `Fusa_runStartupSelfTest` and POR. If a startup error occurs, the program switches to `SYS_ERROR`.  

### SYS_WARMUP

This state is active during the sensor warmup. Once an hour, a tick from the Real Timer Clock  (RTC) sets a flag and updates the remaining time. The polled function checks the flag status, clears it, and prints a message for the remaining time.

After 24 hours, the function `Application_isSensorReady` returns `true`, indicating the sensor is ready for use. If the internal EEPROM is valid, then the system switches to the `SYS_MONITOR` state or the `SYS_ALARM` state, depending on the status of the alarm. Otherwise, the system switches to the `SYS_CALIBRATE` state. 

### SYS_CALIBRATE

This state is used to handle the sensor calibration. To properly use the sensor, it is necessary to calibrate the sensor against a known zero point at specified environmental conditions. The user must press and hold SW0 until the calibration completes. If no errors occurred during calibration, the system switches to the `SYS_MONITOR` state.

### SYS_MONITOR

This state is used to monitor the sensor. The analog comparator is pollsed to see if the ammonia value is above the `ALARM_THRESHOLD_HIGH` point. If it is, the system transitions to `SYS_ALARM`. Otherwise, the analog comparator is checked by the function `Fusa_testAC`. During this function, the system temporarily enters the `SYS_SELF_TEST` state, but returns to `SYS_MONITOR` after executing. An error during self-test function will cause the system to enter the `SYS_ERROR` state. Finally, if no other issues have occurred and the Alarm Test button is pressed, then the system will transition to `SYS_ALARM`.

### SYS_SELF_TEST

This state is active during the self-test of the Analog Comparator. During this operation, the AC is disconnected from the sensor, and connected to the output of DAC0. At the end of this self-test, the sensor is reconnected to the AC and returned to the previous state. If this state is ever encountered outside of the self-test function, the system goes to the `SYS_ERROR` state.

### SYS_ALARM

This state is active when the alarm is triggered. In this state, the buzzer sounds and the LED blinks. When the threshold falls below the `ALARM_THRESHOLD_LOW` threshold, the system transitions to `SYS_MONITOR`. AC self-tests, alarm test, and other functions are not available in this mode.

### SYS_ERROR

This state is active if a self-test fails or the state machine enters an unexpected state. The LED blinks and the buzzer sounds in a pattern while printing `SYSTEM FAULT` to the UART. This state is an infinite loop which cannot be escaped. 

## Operation

**Important: The sensor must warm-up for 24 hours before becoming stable.**


## Summary

<!-- Summarize what the example has shown -->
