/**
 *  (c) 2023 Microchip Technology Inc. and its subsidiaries.
 *
 *  Subject to your compliance with these terms, you may use Microchip software
 *  and any derivatives exclusively with Microchip products. You're responsible
 *  for complying with 3rd party license terms applicable to your use of 3rd
 *  party software (including open source software) that may accompany Microchip
 *  software.
 *
 *  SOFTWARE IS "AS IS." NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
 *  APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 *  INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 *  WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
 *  HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
 *  THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
 *  CLAIMS RELATED TO THE SOFTWARE WILL NOT EXCEED AMOUNT OF FEES, IF ANY,
 *  YOU PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 *  @file    diag_config.h
 *  @brief   This file contains macros to hold the user settings for diagnostics library
 *
 */
 
#ifndef DIAG_CONFIG_H
#define DIAG_CONFIG_H

#include "../diag_device_macros.h"
#include "../diag_result_type.h"
#include "../../../nvm/nvm.h"
#include "../../../system/utils/compiler.h"
#include "../../../system/ccp.h"

/**
 * @def DIAG_LINKER_OPTION_MSG_ENABLE
 * @brief If not set to zero, this macro enables the use of @ref DIAG_MSG in
 * the diagnostic code. 
 */
#define DIAG_LINKER_OPTION_MSG_ENABLE (1U)

#if (DIAG_LINKER_OPTION_MSG_ENABLE != 0)
/**
 @def DIAG_MSG(msg,val,info)
 @brief Used to add compiler messages with macro values and any additional info,
 such as required linker options given the value of relevant config macros. 
 
 @misradeviation{@advisory,20.10} It is generally agreed that the hash and 
 double-hash preprocessor operands should not be used when such macros will 
 affect the behaviour of the compiled code. However, since the these operands are
 only used to print messages during compilation, in this case, it is deemed safe. 
 Finally, while it is generally not possible to always predict the result of a 
 macro expansion using these operands, the library requires the usage of a 
 specific compiler version in which the functionality is deterministic and 
 thoroughly tested. 
 */
/* cppcheck-suppress misra-c2012-20.10 */
#define DIAG_PRAGMA(x) _Pragma (#x)
/* cppcheck-suppress misra-c2012-20.10 */
#define DIAG_MSG_EXPAND(s0,x,s1) DIAG_PRAGMA(message ( s0 #x s1))
#define DIAG_MSG(msg,val,info) DIAG_MSG_EXPAND(msg,val, info)
#endif //(DIAG_LINKER_OPTION_MSG_ENABLE != 0)

/** 
 * @ingroup diag_wdt_startup
 * @def DIAG_WDT_FUSE_ENABLED
 * Set to a value greater than zero if the WDT is enabled through the fuses. 
 * If enabled, ensure the @ref DIAG_WDT_PERIOD macro is set equal to 
 * the corresponding value in the PERIOD bit field in the FUSE.WDTCFG fuse.
 * This is required because enabling the WDT through the fuses also sets the 
 * LOCK bit in the WDT.STATUS register at boot time, disabling the possibility
 * to reconfigure the WDT period during the diagnostic execution.  
 */
#define DIAG_WDT_FUSE_ENABLED (0U)

#if (DIAG_WDT_FUSE_ENABLED != 0U)
/** 
 * @ingroup diag_wdt_startup
 * @def DIAG_WDT_PERIOD
 * Set equal to the corresponding value in the PERIOD bit field in the 
 * FUSE.WDTCFG fuse, if not equal to zero.
 */ 
#define DIAG_WDT_PERIOD (WDT_PERIOD_OFF_gc)
#endif

/** 
 * @ingroup diag_wdt_startup
 * @def DIAG_WDT_TOLERANCE_PCT
 * Configures the tolerance used in the WDT time-out threshold measurements as 
 * a percentage. 
 * 
 * The WDT runs on a 1.024 kHz clock sourced from the internal Ultra Low-Power
 * Oscillator (OSC32K). Since this oscillator is less accurate than other 
 * oscillators featured on the device, the exact WDT time-out period may vary 
 * between devices, requiring a threshold tolerance. Refer to the tolerance of 
 * the OSC32K in the electrical characteristics in the device data sheet and the 
 * tolerance of the main clock used during the diagnostic (as this is used as the
 * input clock for the timer used for measurement) to calculate the appropriate 
 * tolerance.
 */ 
#define DIAG_WDT_TOLERANCE_PCT (30U)

#define DIAG_FLASH_START_ADDR 0x0U
#define DIAG_FLASH_LENGTH 32766U
#define DIAG_FLASH_CRC_STORE_ADDR 0xfffcU

#define DIAG_EEPROM_START_ADDR 0x1400U
#define DIAG_EEPROM_LENGTH 255U
#define DIAG_EEPROM_CRC_STORE_ADDR 0x15feU

/**
 @ingroup diag_sram_marchc_minus
 @def DIAG_SRAM_DATA_REGION_LEN
 This macro is used to handle a decrease of the size of the SRAM available to the 
 application in order to reduce the worst case execution time (WCET) of 
 @ref DIAG_SRAM_MarchStartup() and/or @ref DIAG_SRAM_MarchCMinus() in 
 @ref SRAM_MARCH_STARTUP mode and reduce the fault detection interval (FDTI) of 
 the aforementioned diagnostics in addition to @ref DIAG_SRAM_MarchPeriodic(). 

 To do this, go to \n
 Project Properties -> XC8 Linker -> Option categories: Additional options \n
 and insert the following linker option in the additional linker options field: \n
 `-Wl,--defsym=__DATA_REGION_LENGTH__=<reduced SRAM size value>`.
 
 Moreover, the start of the stack must additionally be configured to point at 
 the new end address of the SRAM used by the application by using the 
 following linker option (this will take effect after `.init0` but before 
 entering the `main()` function): \n 
 `-Wl,--defsym=__stack=<new end address>`.
 
 Finally, the @ref DIAG_SRAM_DATA_REGION_LEN macro must be set to the new 
 reduced SRAM size value to comply with AoU-SRAM_MARCH_TEST-04 in 
 @ref diag_march_aou.
 
 As an example, to reduce the SRAM size of an AVR128DA48 from default 0x4000 
 (derived from the the `INTERNAL_SRAM_SIZE` macro value in the ioavr128da48.h)
 to 0x2000, the new end address will be: \n
 `INTERNAL_SRAM_START(= 0x4000) + 0x2000 - 1 = 0x5FFF`. \n
 Thus, the following linker option should be set: \n
 `-Wl,--defsym=__DATA_REGION_LENGTH__=0x2000 -Wl,--defsym=__stack=0x5FFF` \n
 and the @ref DIAG_SRAM_DATA_REGION_LEN macro should be updated to: \n
 `DIAG_SRAM_DATA_REGION_LEN (0x2000U)`.
 */ 
#define DIAG_SRAM_DATA_REGION_LEN (0x1800U)

/**
 @ingroup diag_sram_marchc_minus
 @def DIAG_SRAM_MARCH_ALT_CLK_FRQ_ENABLED
 This macro decides if an alternative main clock frequency should be used when
 calling @ref DIAG_SRAM_MarchCMinus in @ref DIAG_SRAM_MarchStartup(). If the 
 macro is not zero, it will enable the use of an alternative main clock frequency
 given by @ref DIAG_SRAM_MARCH_ALT_CLK_FRQ. The main clock frequency will be set 
 back to the default/reset value before returning from 
 @ref DIAG_SRAM_MarchStartup(). Setting the macro to zero will result in using 
 the default/reset frequency during execution of the SRAM march test at startup.
*/
#define DIAG_SRAM_MARCH_ALT_CLK_FRQ_ENABLED (0U)

#if defined (__DOXYGEN__)
/**
 @ingroup diag_sram_marchc_minus
 @def DIAG_SRAM_MARCH_ALT_CLK_FRQ
 This macro decides the main clock frequency used when calling 
 @ref DIAG_SRAM_MarchStartup() if @ref DIAG_SRAM_MARCH_ALT_CLK_FRQ_ENABLED is 
 not zero. The main clock frequency will be set back to default upon completing 
 the startup test. If @ref DIAG_SRAM_MARCH_ALT_CLK_FRQ_ENABLED is set to zero, 
 this macro has no effect and the default main clock frequency will be used 
 during the startup test. Using a higher main clock frequency than the default 
 value will reduce the worst case execution time (WCET) of 
 @ref DIAG_SRAM_MarchStartup().

 Depending on the device, the frequency can be set by defining the macro value 
 as any `CLKCTRL_FRQSEL_t` group configuration (e.g. `CLKCTRL_FRQSEL_24M_gc`) 
 or `CLKCTRL_PDIV_t` group configuration (e.g `CLKCTRL_PDIV_2X_gc`). 
 The former option, if available for the device in use (such as the AVR DA 
 family of devices), can set the frequency directly regardless of the supply 
 voltage to the device. The latter option divides the main clock source 
 frequency (which can depend on FUSE settings), if the PEN bit is also set, 
 with the specified value. Thus, when configuring with the latter option, 
 ensure that `CLKCTRL_PEN_bm` is bitwise ORed with the `CLKCTRL_PDIV_t` group 
 configuration, e.g: \n
 `#define DIAG_SRAM_MARCH_CLK_FRQ  (CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm)` \n
 If the PEN bit is not set, the test will run with no main clock prescaler 
 and thus at full main clock source speed. 
 
 @warning The system integrator shall ensure that this frequency does not exceed 
 the maximum specified frequency with the used device supply voltage and 
 specified temperature range as outlined in AoU-SRAM_MARCH_TEST-11 in 
 @ref diag_march_aou.

 @misradeviation{@advisory,2.5} False positive - the tool is not able to 
 recognize that @ref DIAG_SRAM_MARCH_DEFAULT_CLK_FRQ is used as an argument when 
 calling ccp_write_io() in @ref DIAG_SRAM_MarchStartup().
 */
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_SRAM_MARCH_ALT_CLK_FRQ
#else
#define DIAG_SRAM_MARCH_ALT_CLK_FRQ  (CLKCTRL_PDIV_DIV2_gc | CLKCTRL_PEN_bm)
#endif

/**
 @ingroup diag_sram_marchc_minus
 This macro enables the use of the @ref DIAG_SRAM_MarchPeriodic() API, if not 
 set to zero. 
 */
#define DIAG_SRAM_MARCH_PERIODIC_ENABLED (1U)

#if (DIAG_SRAM_MARCH_PERIODIC_ENABLED != 0)
/**
 @ingroup diag_sram_marchc_minus
 @def DIAG_SRAM_MARCH_SEC_SIZE
 This macro determines the size of each SRAM section tested when calling 
 @ref DIAG_SRAM_MarchPeriodic(). Due to the physical layout of the SRAM on the 
 device, the smallest possible value for this macro is 0x20 and it is recommended
 that the value is divisible by 0x10 for the best possible coverage. The value 
 must be configured such that the resulting number of SRAM sections is an 
 integer, given the value of @ref DIAG_SRAM_MARCH_SEC_OVERLAP and 
 @ref DIAG_SRAM_DATA_REGION_LEN and the value must not exceed 
 DIAG_SRAM_DATA_REGION_LEN/2U - @ref DIAG_SRAM_TEMP_STACK_SIZE. \n
 Tip: Using values of 2^n for @ref DIAG_SRAM_DATA_REGION_LEN and
 @ref DIAG_SRAM_MARCH_SEC_SIZE simplifies the process of selecting appropriate
 values. Increasing the size of each SRAM section will increase the coupling 
 fault coverage at the cost of longer execution time of the diagnostic and an 
 increased size of the backup buffer in SRAM.

 @note The number of SRAM sections, `N`, is given by: \n
 `size_total` = @ref DIAG_SRAM_DATA_REGION_LEN \n
 `size_section` = @ref DIAG_SRAM_MARCH_SEC_SIZE \n
 `size_overlap` = @ref DIAG_SRAM_MARCH_SEC_OVERLAP \n 
 `N = (size_total - size_overlap)/(size_section - size_overlap)`
 */
#define DIAG_SRAM_MARCH_SEC_SIZE (0x100U)

/**
 @ingroup diag_sram_marchc_minus
 @def DIAG_SRAM_MARCH_SEC_OVERLAP
 This macro determines the overlap between each SRAM section when calling 
 @ref DIAG_SRAM_MarchPeriodic(). Due to the physical layout of the SRAM on the 
 device, the smallest possible value for this macro is 0x10 and it is recommended
 that the value is divisible by 0x10 to get the best possible coverage, unless no
 overlap is required in which the value can be set to zero. Using no overlap is 
 not recommended as coupling faults between SRAM Sections will not be detected.
 Moreover, the value must be configured such that 
 @ref DIAG_SRAM_DATA_REGION_LEN - @ref DIAG_SRAM_MARCH_SEC_OVERLAP is divisible 
 by @ref DIAG_SRAM_MARCH_SEC_SIZE - @ref DIAG_SRAM_MARCH_SEC_OVERLAP and it must
 be smaller than @ref DIAG_SRAM_MARCH_SEC_SIZE. Increasing the overlap size will 
 increase the coupling fault coverage at the cost of an increase in the number of
 SRAM sections which in turn increases the total number of times the API must be 
 called before the entire SRAM is tested once, increasing the the FDTI.

 For each address in the SRAM used by the application, coupling faults between 
 that address and at least the @ref DIAG_SRAM_MARCH_SEC_OVERLAP closest addresses
 in each direction are covered by the @ref DIAG_SRAM_MarchPeriodic() diagnostic. 
 For most addresses with most values of @ref DIAG_SRAM_MARCH_SEC_OVERLAP and 
 @ref DIAG_SRAM_MARCH_SEC_SIZE, the number of neighbouring addresses covered is 
 larger than @ref DIAG_SRAM_MARCH_SEC_OVERLAP in one or both directions.

 As an example, using a section size of 0x100 and 75% overlap so that
 @ref DIAG_SRAM_MARCH_SEC_SIZE = 0x100 = 256 and 
 @ref DIAG_SRAM_MARCH_SEC_OVERLAP = 0xC0 = 192, has the effect that for each
 address in the SRAM used by the application, coupling faults between that 
 address and at least the 192 closests addresses in each direction are covered. 
 */
#define DIAG_SRAM_MARCH_SEC_OVERLAP (0x0U)
#endif //(DIAG_SRAM_MARCH_PERIODIC_ENABLE != 0)

#endif //DIAG_CONFIG_H