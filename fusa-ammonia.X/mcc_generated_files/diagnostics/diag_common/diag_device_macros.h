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
 *  @file    diag_device_macros.h
 *  @brief   This common file contains macros that group device specific macros
 *  to account for differences and similarities in device architectures.
 *  @warning This file may contain references to unsupported devices.
 *  See the Diagnostic Library release notes for a list of supported devices.
 *
 *  @note
 *  Microchip Technology Inc. has followed development methods required by
 *  Functional Safety Standards and performed extensive validation and static
 *  testing to ensure that the code operates as intended. Any modification to the
 *  code can invalidate the results of Microchip's validation and testing.
 *
 */

#ifndef DIAG_DEVICE_MACROS_H
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_DEVICE_MACROS_H

//AVR DA Family:

//For all AVR32DAxx devices
#if defined (__AVR_AVR32DA28__) || defined (__AVR_AVR32DA32__) || defined (__AVR_AVR32DA48__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR32DAxx_DEVICE
#endif

//For all AVR64DAxx devices
#if defined (__AVR_AVR64DA28__) || defined (__AVR_AVR64DA32__) || defined (__AVR_AVR64DA48__) || defined (__AVR_AVR64DA64__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR64DAxx_DEVICE
#endif

//For all AVR128DAxx devices
#if defined (__AVR_AVR128DA28__) || defined (__AVR_AVR128DA32__) || defined (__AVR_AVR128DA48__) || defined (__AVR_AVR128DA64__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR128DAxx_DEVICE
#endif

//For all AVR DA Family devices
#if defined (DIAG_AVR32DAxx_DEVICE) || defined (DIAG_AVR64DAxx_DEVICE) || defined (DIAG_AVR128DAxx_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_DA_DEVICE
#endif


//AVR DB Family:

//For all AVR32DBxx devices
#if defined (__AVR_AVR32DB28__) || defined (__AVR_AVR32DB32__) || defined (__AVR_AVR32DB48__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR32DBxx_DEVICE
#endif

//For all AVR64DBxx devices
#if defined (__AVR_AVR64DB28__) || defined (__AVR_AVR64DB32__) || defined (__AVR_AVR64DB48__) || defined (__AVR_AVR64DB64__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR64DBxx_DEVICE
#endif

//For all AVR128DBxx devices
#if defined (__AVR_AVR128DB28__) || defined (__AVR_AVR128DB32__) || defined (__AVR_AVR128DB48__) || defined (__AVR_AVR128DB64__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR128DBxx_DEVICE
#endif

//For all AVR DB Family devices
#if defined (DIAG_AVR32DBxx_DEVICE) || defined (DIAG_AVR64DBxx_DEVICE) || defined (DIAG_AVR128DBxx_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_DB_DEVICE
#endif


//AVR DD Family: 

//For all AVR16DDxx devices
#if defined (__AVR_AVR16DD14__) || defined (__AVR_AVR16DD20__) || defined (__AVR_AVR16DD28__) || defined (__AVR_AVR16DD32__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR16DDxx_DEVICE
#endif

//For all AVR32DDxx devices
#if defined (__AVR_AVR32DD14__) || defined (__AVR_AVR32DD20__) || defined (__AVR_AVR32DD28__) || defined (__AVR_AVR32DD32__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR32DDxx_DEVICE
#endif

//For all AVR64DDxx devices
#if defined (__AVR_AVR64DD14__) || defined (__AVR_AVR64DD20__) || defined (__AVR_AVR64DD28__) || defined (__AVR_AVR64DD32__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR64DDxx_DEVICE
#endif

//For all AVR DD Family devices
#if defined (DIAG_AVR64DDxx_DEVICE) || defined (DIAG_AVR32DDxx_DEVICE) || defined (DIAG_AVR16DDxx_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_DD_DEVICE
#endif

//AVR EA family:
//For all AVR64EAxx devices
//For all AVR16EAxx devices
#if defined (__AVR_AVR16EA28__) || defined (__AVR_AVR16EA32__) || defined (__AVR_AVR16EA48__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR16EAxx_DEVICE
#endif

//For all AVR32EAxx devices
#if defined (__AVR_AVR32EA28__) || defined (__AVR_AVR32EA32__) || defined (__AVR_AVR32EA48__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR32EAxx_DEVICE
#endif

#if defined (__AVR_AVR64EA28__) || defined (__AVR_AVR64EA32__) || defined (__AVR_AVR64EA48__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR64EAxx_DEVICE
#endif

//For all AVR EA devices
#if defined (DIAG_AVR64EAxx_DEVICE) || defined (DIAG_AVR32EAxx_DEVICE) || defined (DIAG_AVR16EAxx_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_EA_DEVICE
#endif

//tinyAVR 0-series:

//For all ATtiny20x devices
#if defined (__AVR_ATtiny202__) || defined (__AVR_ATtiny204__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny20x_DEVICE
#endif

//For all ATtiny40x devices
#if defined (__AVR_ATtiny402__) || defined (__AVR_ATtiny404__) || defined (__AVR_ATtiny406__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny40x_DEVICE
#endif

//For all ATtiny80x devices
#if defined (__AVR_ATtiny804__) || defined (__AVR_ATtiny806__) || defined (__AVR_ATtiny807__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny80x_DEVICE
#endif

//For all ATtiny160x devices
#if defined (__AVR_ATtiny1604__) || defined (__AVR_ATtiny1606__) || defined (__AVR_ATtiny1607__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny160x_DEVICE
#endif

//For all tinyAVR 0-series devices
#if defined (DIAG_ATtiny20x_DEVICE) || defined (DIAG_ATtiny40x_DEVICE) || defined (DIAG_ATtiny80x_DEVICE) || defined (DIAG_ATtiny160x_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_TINY0_DEVICE
#endif


//tinyAVR 1-series:

//For all ATtiny21x devices
#if defined (__AVR_ATtiny212__) || defined (__AVR_ATtiny214__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny21x_DEVICE
#endif

//For all ATtiny41x devices
#if defined (__AVR_ATtiny412__) || defined (__AVR_ATtiny414__) || defined (__AVR_ATtiny416__) || defined (__AVR_ATtiny417__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny41x_DEVICE
#endif

//For all ATtiny81x devices
#if defined (__AVR_ATtiny814__) || defined (__AVR_ATtiny816__) || defined (__AVR_ATtiny817__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny81x_DEVICE
#endif

//For all ATtiny161x devices
#if defined (__AVR_ATtiny1614__) || defined (__AVR_ATtiny1616__) || defined (__AVR_ATtiny1617__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny161x_DEVICE
#endif

//For all ATtiny321x devices
#if defined (__AVR_ATtiny3216__) || defined (__AVR_ATtiny3217__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny321x_DEVICE
#endif

//For all tinyAVR 1-series devices
#if defined (DIAG_ATtiny21x_DEVICE) || defined (DIAG_ATtiny41x_DEVICE) || defined (DIAG_ATtiny81x_DEVICE) || defined (DIAG_ATtiny161x_DEVICE) || defined (DIAG_ATtiny321x_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_TINY1_DEVICE
#endif


//tinyAVR 2 Family:

//For all ATtiny42x devices
#if defined (__AVR_ATtiny424__) || defined (__AVR_ATtiny426__) || defined (__AVR_ATtiny427__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny42x_DEVICE
#endif

//For all ATtiny82x devices
#if defined (__AVR_ATtiny824__) || defined (__AVR_ATtiny826__) || defined (__AVR_ATtiny827__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny82x_DEVICE
#endif

//For all ATtiny162x devices
#if defined (__AVR_ATtiny1624__) || defined (__AVR_ATtiny1626__) || defined (__AVR_ATtiny1627__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny162x_DEVICE
#endif

//For all ATtiny322x devices
#if defined (__AVR_ATtiny3224__) || defined (__AVR_ATtiny3226__) || defined (__AVR_ATtiny3227__)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_ATtiny322x_DEVICE
#endif

//For all tinyAVR 2 Family devices
#if defined (DIAG_ATtiny42x_DEVICE) || defined (DIAG_ATtiny82x_DEVICE) || defined (DIAG_ATtiny162x_DEVICE) || defined (DIAG_ATtiny322x_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_TINY2_DEVICE
#endif


//Grouping families/series:

//For all defined AVR Dx devices
#if defined (DIAG_AVR_DA_DEVICE) || defined (DIAG_AVR_DB_DEVICE) || defined (DIAG_AVR_DD_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_Dx_DEVICE
#endif

//For all defined AVR Ex devices
#if defined (DIAG_AVR_EA_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_Ex_DEVICE
#endif

//For all defined tinyAVR devices
#if defined (DIAG_AVR_TINY0_DEVICE) || defined (DIAG_AVR_TINY1_DEVICE) || defined (DIAG_AVR_TINY2_DEVICE)
/* cppcheck-suppress misra-c2012-2.5 */
#define DIAG_AVR_TINYx_DEVICE
#endif

#endif //DIAG_DEVICE_MACROS_H