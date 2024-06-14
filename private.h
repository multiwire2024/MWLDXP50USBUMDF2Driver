/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    private.h

Abstract:

    Contains structure definitions and function prototypes private to
    the driver.

Environment:

    Kernel mode

--*/

#pragma warning(disable:4200)  //
#pragma warning(disable:4201)  // nameless struct/union
#pragma warning(disable:4214)  // bit field types other than int

//#include <initguid.h>


#ifdef WDF_KERNEL_MODE
#include <ntddk.h>
#include <ntintsafe.h>
#else
#include <wtypes.h>
#endif
#include "usb.h"
#include "usbdi.h"
#ifdef WDF_KERNEL_MODE
#include "usbdlib.h"
#endif


#pragma warning(default:4200)
#pragma warning(default:4201)
#pragma warning(default:4214)

#include <wdf.h>
#include <wdfusb.h>
#include "public.h"


#ifndef _H
#define _H

#define POOL_TAG (ULONG) 'MWLU'

#define MAX_SUPPORTED_PACKETS_FOR_SUPER_SPEED 1024
#define MAX_SUPPORTED_PACKETS_FOR_HIGH_SPEED 1024
#define MAX_SUPPORTED_PACKETS_FOR_FULL_SPEED 255

#define DISPATCH_LATENCY_IN_MS 10

/*
 * These are the requests (bRequest) that the bootstrap loader is expected
 * to recognize.  The codes are reserved by Cypress, and these values match
 * what EZ-USB hardware, or "Vend_Ax" firmware (2nd stage loader) uses.
 * Cypress' "a3load" is nice because it supports both FX and FX2, although
 * it doesn't have the EEPROM support (subset of "Vend_Ax").
 */

#define FX2_RW_INTERNAL     0xA0            /* hardware implements this one */
#define FX2_RW_EEPROM       0xA2
#define FX2_RW_MEMORY       0xA3
#define FX2_GET_EEPROM_SIZE 0xA5 

#define MWLUSB_MAJOR_VERSION             02
#define MWLUSB_MINOR_VERSION             01
#define MWLUSB_BUILD_VERSION             0001


#if DBG

#define MWLUsb_DbgPrint(level, _x_) \
            if((level) <= DebugLevel) { \
                DbgPrint("MWLUsb: "); DbgPrint _x_; \
            }

#else

#define MWLUsb_DbgPrint(level, _x_)

#endif

#define MAX_FULL_SPEED_PACKET_SIZE   64
#define MAX_HIGH_SPEED_PACKET_SIZE   512
#define MAX_SUPER_SPEED_PACKET_SIZE  1024
#define MAX_STREAM_VALID_PACKET_SIZE   1024
#define REMOTE_WAKEUP_MASK 0x20

#define DEFAULT_REGISTRY_TRANSFER_SIZE 65536

#define IDLE_CAPS_TYPE IdleUsbSelectiveSuspend

#endif
