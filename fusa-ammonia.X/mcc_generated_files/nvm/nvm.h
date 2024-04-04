/**
 * 
 * @file nvm.h
 *
 * @defgroup nvm_driver  Non-Volatile Memory
 *
 * @brief This file contains API prototypes and other data types for the Non-Volatile Memory (NVM) driver.
 *
 * @version NVM Driver Version 2.1.2
 */

#ifndef NVM_H
#define NVM_H

#include <string.h>
#include "../system/utils/compiler.h"

/**
 * @ingroup nvm_driver
 * @brief Data types for the NVM address and data.
 */

/**
 * @ingroup nvm_driver
 * @brief Data type for the Flash data.
 */
 typedef uint8_t flash_data_t;
 /**
 * @ingroup nvm_driver
 * @brief Data type for the Flash address.
 */
typedef uint16_t flash_address_t;

/**
 * @ingroup nvm_driver
 * @brief Data type for the EEPROM data.
 */
typedef uint8_t eeprom_data_t;
/**
 * @ingroup nvm_driver
 * @brief Data type for the EEPROM address.
 */
typedef uint16_t eeprom_address_t;

/**
 * @ingroup nvm_driver
 * @def NRWW_SECTION
 * This is the macro used to define nrww flash segment.
 * In order to keep all Flash function in a nrww segment, \n
 * User must use NRWW_SECTION attribute in its declaration.
 * e.g. If user wants to keep Flash_Read api in rww segment,
 *  - NRWW_SECTION flash_data_t FLASH_Read(flash_address_t address);
 * User must allocate desired address to <b>.nrww</b> named section using appropriate linker setting. \n
 * e.g. If user wants to keep Flash functions at 0x1000, \n
 * - For XC8: \n
 *   -# Go to Project Properties -> XC8 Global Options -> XC8 Linker -> Linker Additional Options \n
 *   -# Add "-Wl,--section-start=.nrww=0x1000" \n
 */
#define NRWW_SECTION __attribute__((noinline, used, section(".nrww")))

/**
 * @ingroup nvm_driver
 * @def RWW_SECTION
 * This is the macro used to define rww flash segment.
 * In order to keep Flash function in a rww segment, \n
 * User must use RWW_SECTION attribute in its declaration.
 * e.g. If user wants to keep Flash_Read api in rww segment,
 *  - RWW_SECTION flash_data_t FLASH_Read(flash_address_t address);
 * Also User must allocate desired address to <b>.rww</b> named section using appropriate linker setting. \n
 * e.g. If user wants to keep Flash functions at 0x2000, \n
 * - For XC8: \n
 *   -# Go to Project Properties -> XC8 Global Options -> XC8 Linker -> Linker Additional Options \n
 *   -# Add "-Wl,--section-start=.rww=0x2000" \n
 */
#define RWW_SECTION __attribute__((noinline, used, section(".rww")))


/**
 * @ingroup nvm_driver
 * @enum nvm_status_t
 * @brief Contains the return codes for the NVM driver APIs.
 */
typedef enum {
    NVM_OK, /**<0 - The NVM operation succeeded*/
    NVM_ERROR, /**<1 - The NVM operation failed*/
    NVM_BUSY  /**<2 - The NVM Write operation is ongoing*/
} nvm_status_t;

/**
 * @ingroup nvm_driver
 * @brief Initializes the NVM driver.
 * @param None.
 * @return None.
 */
NRWW_SECTION
void NVM_Initialize(void);

/**
 * @ingroup nvm_driver
 * @brief Returns the status of the last NVM operation.
 * @param None.
 * @retval NVM_OK - The NVM operation succeeded
 * @retval NVM_ERROR - The NVM operation failed
 */
NRWW_SECTION
nvm_status_t NVM_StatusGet(void);

/**
 * @ingroup nvm_driver
 * @brief Clears the status of the last NVM operation.
 * @param None.
 * @retval None.
 */
NRWW_SECTION
void NVM_StatusClear(void);


/**
 * @ingroup nvm_driver
 * @brief Reads a byte from the given Flash address.
 * @pre NVM must be initialized with @ref NVM_Initialize() before calling this API.
 * @param [in] address - Address of the Flash location where data read from.
 * @return Byte read from the given Flash address.
 */
NRWW_SECTION
flash_data_t FLASH_Read(flash_address_t address);

/**
 * @ingroup nvm_driver
 * @brief Writes one entire Flash row/page from the given starting address of the row (the first byte location). 
 *        The size of the input buffer must be one Flash row and the address must be aligned with the row boundary.
 *        Use @ref FLASH_PageAddressGet() to obtain the starting address of the row.
 * @pre Erase Flash row before calling this function.
 * @param [in] address - Starting address of the Flash row to be written.
 * @param [in] *dataBuffer - Pointer to a buffer which holds the data to be written.
 * @return Status of the Flash row Write operation as described in @ref nvm_status_t.
 */
NRWW_SECTION
nvm_status_t FLASH_RowWrite(flash_address_t address, flash_data_t *dataBuffer);

/**
 * @ingroup nvm_driver
 * @brief Erases one Flash page containing the given address.
 * @pre NVM must be initialized with @ref NVM_Initialize() before calling this API.
 * @param [in] address - Starting address of the Flash page to be erased.
 * @return Status of the Flash Page Erase operation as described in the @ref nvm_status_t.
 */
NRWW_SECTION
nvm_status_t FLASH_PageErase(flash_address_t address);

/**
 * @ingroup nvm_driver
 * @brief Checks if the Flash is busy.
 * @pre NVM must be initialized with @ref NVM_Initialize() before calling this API.
 * @param None.
 * @retval True - The Flash operation is being performed.
 * @retval False - The Flash operation is not being performed.
 */
NRWW_SECTION
bool FLASH_IsBusy(void);

/**
 * @ingroup nvm_driver
 * @brief Returns the starting address of the page (the first byte location) containing the given Flash address.
 * @param [in] address - Flash address for which the page starting address will be obtained.
 * @return Starting address of the page containing the given Flash address.
 */
flash_address_t FLASH_PageAddressGet(flash_address_t address);

/**
 * @ingroup nvm_driver
 * @brief Returns the offset from the starting address of the page (the first byte location).
 * @param [in] address - Flash address for which the offset from the starting address of the page will be obtained.
 * @return Offset of the given address from the starting address of the page.
 */
NRWW_SECTION
flash_address_t FLASH_PageOffsetGet(flash_address_t address);

//Below macros are added to provide backward compatibility. These will be deprecated in the future versions.
#define FLASH_ErasePageAddressGet FLASH_PageAddressGet
#define FLASH_ErasePageOffsetGet FLASH_PageOffsetGet



/**
 * @ingroup nvm_driver
 * @brief Reads one byte from the given EEPROM address.
 * @pre NVM must be initialized with @ref NVM_Initialize() before calling this API.
 * @param [in] address - Address of the EEPROM location to be read.
 * @return Byte read from the given EEPROM address.
 */
eeprom_data_t EEPROM_Read(eeprom_address_t address);

/**
 * @ingroup nvm_driver
 * @brief Writes one byte to the given EEPROM address. The EEPROM busy status must be checked using the @ref EEPROM_IsBusy() API to know if the Write operation is completed.
 * Use the @ref NVM_StatusGet() API to see the result of the Write operation.
 * @param [in] address - Address of the EEPROM location to be written.
 * @param [in] data - Byte to be written to the given EEPROM location.
 * @return None.
 */
nvm_status_t EEPROM_Write(eeprom_address_t address, eeprom_data_t data);

/**
 * @ingroup nvm_driver
 * @brief Checks if the EEPROM is busy.
 * @pre NVM must be initialized with @ref NVM_Initialize() before calling this API.
 * @param None.
 * @retval True - The EEPROM operation is being performed.
 * @retval False - The EEPROM operation is not being performed.
 */
bool EEPROM_IsBusy(void);


#endif  /* NVM_H */
