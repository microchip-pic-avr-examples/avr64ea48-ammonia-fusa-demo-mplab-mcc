/**
 * © 2023 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms
 * applicable to your use of third party software (including open
 * source software) that may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL,
 * PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED,
 * EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE
 * DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW,
 * MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO
 * THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU
 * HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 *  @file    diag_wdt_startup.c
 *  @brief   Contains API prototypes for the Watchdog Timer start-up test.
 *  @ingroup diag_wdt_startup
 *
 * @note
 * Microchip Technology Inc. has followed development methods required by
 * Functional Safety Standards and performed extensive validation and static
 * testing to ensure that the code operates as intended. Any modification to the
 * code can invalidate the results of Microchip's validation and testing.
 */

#include "diag_wdt_startup.h"
#include "../../diag_common/config/diag_config.h"
#include <avr/wdt.h>
#include <xc.h>

/*Defining OSCCFG for AVR devices*/
#if defined (DIAG_AVR_Ex_DEVICE) || defined (DIAG_AVR_TINYx_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DEVICE_HAS_OSCCFG_FUSE
#endif

#if defined (DIAG_AVR_Ex_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define FUSE_OSC_FRQ_16MHZ (FUSE_OSCHFFRQ_bm)
/* cppcheck-suppress misra-c2012-2.5 */
#define FUSE_OSC_FRQ_20MHZ (0U)
/* cppcheck-suppress misra-c2012-2.5 */
#define FUSE_OSC_FRQ_BM (FUSE_OSCHFFRQ_bm)

#elif defined (DIAG_AVR_TINYx_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define FUSE_OSC_FRQ_16MHZ (FUSE_FREQSEL_0_bm)
/* cppcheck-suppress misra-c2012-2.5 */
#define FUSE_OSC_FRQ_20MHZ (FUSE_FREQSEL_1_bm)
/* cppcheck-suppress misra-c2012-2.5 */
#define FUSE_OSC_FRQ_BM (FUSE_FREQSEL_gm)
#endif

/*
 Use the smallest possible WDT period to decrease the test execution time if the
 WDT is not enabled in the fuses and thus locked.
 */
#if (DIAG_WDT_FUSE_ENABLED == 0U)
#define DIAG_WDT_PERIOD (WDT_PERIOD_8CLK_gc)
#endif

// Convert group configuration number to number of 1KHz clock cycles
#define WDT_PERIOD_CLK ((1U << (DIAG_WDT_PERIOD - 1U)) * 8U)

// Convert from number of 1KHz clock cycles to milliseconds
#define WDT_1KHZ_CLK  ((float)1024U)
#define WDT_PERIOD_MS (((uint32_t)WDT_PERIOD_CLK / WDT_1KHZ_CLK) * 1000U)

/*
 * Calculate the maximum number of timer overflows before it is assumed that the
 * WDT is not able to reset the device, while taking the tolerance into account.
 */
#define WDT_MAX_COUNT_VAL (WDT_PERIOD_MS*(100U + (uint32_t)DIAG_WDT_TOLERANCE_PCT) / (float)10U)

/*
 * Calculate the minimum number of timer overflows before it is assumed that the
 * WDT is not able to reset the device, while taking the tolerance into account.
 */
#define WDT_MIN_COUNT_VAL (WDT_PERIOD_MS*(100U - (uint32_t)DIAG_WDT_TOLERANCE_PCT) / (float)10U)

/*
 * Used to set the overflow on the TCA to 100 us
 */
#define TCA_OVF_PERIOD (10000U)

/*
 * Used for WDR instruction that will need 2 to 3 cycles of the WDT clock in
 * order to be synchronized.
 */
#define WDR_SYNC_DELAY (30U)

/*
 * This enum contains the valid test states for the WDT startup test. Each state
 * has a unique bit pattern with several bit flips required to transition from
 * one state to the other to avoid false results in case of memory corruption.
 */
typedef enum
{
    TEST_UNINITIALIZED = 0b00000000, // Uninitialized state
    TEST_SYSTEM_RESET = 0b01111110, // Test if the WDT can issue resets
    TEST_COUNTER_RESET = 0b10111101, // Test if the WDT counter can be reset
    NO_FAULT_DETECTED = 0b11011011, // For the situation when no fault is found
    FAULT_DETECTED = 0b11100111 // For the situation when a fault is found
} fsm_state_t;


/*
 * Forward deceleration of helper functions used to do time taking and
 * measurements using a Timer/Counter peripheral.
 */
static inline void InitTimer(void);

static void RestartTimer(void);

static void TimerDelay(uint32_t numOverflows);

static inline void TearDownTimer(void);

/*
 * The following variables are declared static volatile with a the __persistent
 * identifier to ensure that the value is not lost between WDT issued system
 * resets. An inverse copy is stored to perform an SRAM Duplication test in case
 * of any corruption.
 */

// Used to track the state of the finite state machine.
static volatile __persistent fsm_state_t testState;
static volatile __persistent fsm_state_t invTestState;

// Used to stores the result of the diagnostic for retrieval in the main function.
static volatile __persistent diag_result_t testResult;
static volatile __persistent diag_result_t invTestResult;

/*
 * Used to store the Reset Flag Register (RSTFR) contents before performing the
 * test, as the test clears the register in order to verify the correct WDT
 * behaviour.
 */
static volatile __persistent uint8_t resetFlagsBackup;
static volatile __persistent uint8_t invResetFlagsBackup;

/*
 * Used to measure the number of Timer overflows that has occurred before the WDT
 * issues a system reset. Cannot store an inverse copy due to the fact that it
 * is not possible to ensure that the inverse copy is updated before the WDT
 * issues a system reset.
 */
static volatile __persistent uint32_t timerOverflowCount;

diag_result_t DIAG_WDT_GetResult(void)
{
    // Perform SRAM duplication test
    if ((testResult ^ invTestResult) != 0xFFU)
    {
        testResult = DIAG_FAIL;
        invTestResult = (uint8_t) (DIAG_FAIL) ^ 0xFFU;
    }

    return testResult;
}

uint8_t DIAG_WDT_GetRSTFRCopy(void)
{
    // Perform SRAM Duplication test
    if ((resetFlagsBackup ^ invResetFlagsBackup) != 0xFFU)
    {
        resetFlagsBackup = DIAG_RSTFR_COPY_INVALID;
        invResetFlagsBackup = (uint8_t) (DIAG_RSTFR_COPY_INVALID) ^ 0xFFU;
    }

    return resetFlagsBackup;
}

/*
 * Configures the WDT period or restarts the WDT counter if the WDT is enabled
 * through FUSES.
 */
static void ConfigureWDT(WDT_PERIOD_t period)
{

    if (DIAG_WDT_FUSE_ENABLED != 0U)
    {
        // Issue WDR instruction to reset the current WDT period
        wdt_reset();
        // Wait for the WDR instruction to take effect
        TimerDelay(WDR_SYNC_DELAY);
    }
    else
    {
        while ((WDT.STATUS & WDT_SYNCBUSY_bm) != 0U)
        {
            // Wait for WDT to sync registers
        }

        //Configure the WDT with the specified period
        ccp_write_io((void*) &(WDT.CTRLA), period);

        while ((WDT.STATUS & WDT_SYNCBUSY_bm) != 0U)
        {
            // Wait for WDT to sync registers
        }
    }
}

/*
 * Test that the WDT is able to issue a system reset when it expires and measure
 * the number of timer periods until the reset occurs. If no system reset occurs
 * within WDT_MAX_COUNT_VAL, the WDT is not able to reset the device in time.
 */
static void TestSystemReset(void)
{
    // Start the WDT (or restart if enabled through FUSES)
    ConfigureWDT(DIAG_WDT_PERIOD);

    timerOverflowCount = 0U;

    RestartTimer();

    while (timerOverflowCount < WDT_MAX_COUNT_VAL)
    {
        timerOverflowCount++;
        while ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_OVF_bm) == 0U)
        {
            // wait for timer overflow
        }

        // Clear the timer overflow flag
        TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
    }
}

/*
 * Test that the WDT counter can be reset correctly such that the WDT does not
 * expire.
 */
static void TestCounterReset(void)
{
    // Start the WDT (or restart if enabled through FUSES)
    ConfigureWDT(DIAG_WDT_PERIOD);

    /*
     * Wait approximately 0.75 * T_WDT - WDR_SYNC_DELAY, where T_WDT is
     * the measured WDT period before a WDT reset (timerOverflowCount).
     * This checks that the WDT does not expire earlier than expected.
     * WDR_SYNC_DELAY needs to be subtracted to avoid that the WDT resets
     * the device during synchronization after resetting the WDT counter.
     */
    uint32_t delay = (((timerOverflowCount * 3U) / 4U) - WDR_SYNC_DELAY);
    TimerDelay(delay);
    wdt_reset();

    /*
     * The WDR instruction will need 2 to 3 cycles of the WDT clock in
     * order to be synchronized.
     */
    TimerDelay(WDR_SYNC_DELAY);

    // Wait approximately 0.75 * T_WDT
    delay = ((timerOverflowCount * 3U) / 4U);
    TimerDelay(delay);
}

void DIAG_WDT_Startup(void)
{
    /*
     * Check if the device reset was due to an intentional WDT reset or another
     * reset source. If another reset source, the diagnostic is initiated by
     * backing up the RSTFR register, clearing it, and setting the testState to
     * TEST_SYSTEM_RESET. The RSTFR register must be cleared in order to know if
     * the intentional WDT reset has taken effect as part of the diagnostic
     * execution.
     */
    bool isOtherReset = ((RSTCTRL.RSTFR & (~RSTCTRL_WDRF_bm)) != 0);
    bool isWDTReset = ((RSTCTRL.RSTFR & RSTCTRL_WDRF_bm) == RSTCTRL_WDRF_bm);

    if (isOtherReset || (isWDTReset && (testState == TEST_UNINITIALIZED)))
    {
        // Back up reset register
        resetFlagsBackup = RSTCTRL.RSTFR;
        invResetFlagsBackup = (uint8_t) (RSTCTRL.RSTFR) ^ 0xFFU;

        /*
         * Clear all reset flags to ensure that the correct reset reason is
         * detected after an intentional WDT reset
         */
        RSTCTRL.RSTFR = (RSTCTRL_UPDIRF_bm | RSTCTRL_SWRF_bm | RSTCTRL_WDRF_bm
                         | RSTCTRL_EXTRF_bm | RSTCTRL_BORF_bm | RSTCTRL_PORF_bm);

        // Initiate the diagnostic state machine
        testState = TEST_SYSTEM_RESET;
        invTestState = (uint8_t) (TEST_SYSTEM_RESET) ^ 0xFFU;
    }

    // Set up the timer for time measurement and delays
    InitTimer();

    // Perform SRAM duplication test on test state variable
    if ((testState ^ invTestState) != 0xFFU)
    {
        testState = FAULT_DETECTED;
        invTestState = (uint8_t) (FAULT_DETECTED) ^ 0xFFU;
    }

    // Diagnostic state machine
    switch (testState)
    {
        case TEST_SYSTEM_RESET:
        {
            // Set the state to the next test step after the WDT reset
            testState = TEST_COUNTER_RESET;
            invTestState = (uint8_t) (TEST_COUNTER_RESET) ^ 0xFFU;

            // Verify that the WDT is able to issue a system reset
            TestSystemReset();

            // Reaching this means that the WDT did not reset the device in time
            testState = FAULT_DETECTED;
            invTestState = (uint8_t) (FAULT_DETECTED) ^ 0xFFU;
            break;
        }
        case TEST_COUNTER_RESET:
        {
            // Assume the test is failing for now
            testState = FAULT_DETECTED;
            invTestState = (uint8_t) (FAULT_DETECTED) ^ 0xFFU;

            /*
             * Abort test if a watchdog reset did not occur in the previous test
             * state as expected.
             */
            if (isWDTReset)
            {
                /*
                 * Clear the WDT reset flag to allow for distinguishing between
                 * an intentional and unintentional WDT system reset.
                 */
                RSTCTRL.RSTFR = RSTCTRL_WDRF_bm;
            }
            else
            {
                break;
            }

            /*
             * Abort test if the overflow counter is smaller than the minimum
             * count value in which the WDT issues a system reset too soon in
             * the previous test state
             */
            if (timerOverflowCount < WDT_MIN_COUNT_VAL)
            {
                break;
            }

            // Verify that the WDT is able to reset the counter
            TestCounterReset();

            /*
             * Exiting the above test function should only occur if the WDT
             * counter reset worked, otherwise there would have been a system
             * reset before this point (this would be executed approximately
             * 1.5 * T_WDT after the WDT was initially set). Note that in that
             * case the testState would still be FAULT_DETECTED. Set next state
             * and wait for the WDT to issue a system reset. That should
             * approximately happen in 0.25*T_WDT.
             */
            testState = NO_FAULT_DETECTED;
            invTestState = (uint8_t) (NO_FAULT_DETECTED) ^ 0xFFU;

            // Wait approximately 0.5*T_WDT until the WDT expires
            uint32_t delay = timerOverflowCount / 2U;
            TimerDelay(delay);

            /*
             * Reaching this point means that the WDT did not issue a system
             * reset after expiring
             */
            testState = FAULT_DETECTED;
            invTestState = (uint8_t) (FAULT_DETECTED) ^ 0xFFU;
            break;
        }
        case NO_FAULT_DETECTED:
        {
            testResult = DIAG_PASS;
            invTestResult = (uint8_t) (DIAG_PASS) ^ 0xFFU;
            break;
        }
        default: //Otherwise, assume error
        {
            testState = FAULT_DETECTED;
            invTestState = (uint8_t) (FAULT_DETECTED) ^ 0xFFU;
        }
    }

    /*
     * Disable the WDT if not locked through fuses or reset the WDT counter if
     * enabled through the fuses.
     */
    ConfigureWDT(WDT_PERIOD_OFF_gc);

    /*
     * If this point is reached and testState is still FAULT_DETECTED, an error
     * should be reported through the testResult software flag
     */
    if (testState == FAULT_DETECTED)
    {
        testResult = DIAG_FAIL;
        invTestResult = (uint8_t) (DIAG_FAIL) ^ 0xFFU;
    }

    /*
     * Set the test state to uninitialized and clear the WDT reset flag. This is
     * done to ensure that the diagnostic is run correctly after the next reset,
     * and to allow for distinguishing between an intentional and unintentional
     * WDT system reset when the diagnostic is run again.
     */
    testState = TEST_UNINITIALIZED;
    invTestState = (uint8_t) (TEST_UNINITIALIZED) ^ 0xFFU;
    RSTCTRL.RSTFR = RSTCTRL_WDRF_bm;

    // Restore timer resource registers to reset values
    TearDownTimer();
}

// Configures the TCA to overflow according to TCA_OVF_PERIOD

static inline void InitTimer(void)
{
    uint32_t defaultMainFreq = 0;

#if defined DIAG_AVR_Dx_DEVICE
    defaultMainFreq = 4000000U;
#elif defined (DEVICE_HAS_OSCCFG_FUSE)
    if ((FUSE.OSCCFG & FUSE_OSC_FRQ_BM) == FUSE_OSC_FRQ_16MHZ)
    {
        defaultMainFreq = 2666666U;
    }
    else if ((FUSE.OSCCFG & FUSE_OSC_FRQ_BM) == FUSE_OSC_FRQ_20MHZ)
    {
        defaultMainFreq = 3333333U;
    }
    else
    {
        defaultMainFreq = 0; // Invalid fuse setting, test will fail
    }
#else
#warning Device not supported.
#endif

    // Set the TCA period value
    TCA0.SINGLE.PER = (uint16_t) (defaultMainFreq / (uint32_t) TCA_OVF_PERIOD);

    // Enable TCA and start counting
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
}

static void RestartTimer(void)
{
    TCA0.SINGLE.CTRLESET = TCA_SINGLE_CMD_RESTART_gc;
    TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

// Configurable delay for 'numOverflows' timer overflows

static void TimerDelay(uint32_t numOverflows)
{
    RestartTimer();
    uint32_t delayCountdown = numOverflows;
    while (delayCountdown != 0U)
    {
        while ((TCA0.SINGLE.INTFLAGS & TCA_SINGLE_OVF_bm) == 0U)
        {
            // Wait for TCA0 to overflow
        }

        // Clear the timer overflow flag
        TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
        delayCountdown--;
    }
}

// Set TCA registers back to reset values

static inline void TearDownTimer(void)
{
    TCA0.SINGLE.PER = 0xFFFFU;
    TCA0.SINGLE.CTRLA = 0x00U;
    TCA0.SINGLE.INTCTRL = 0x00U;
    TCA0.SINGLE.INTFLAGS = 0xFFU;
}