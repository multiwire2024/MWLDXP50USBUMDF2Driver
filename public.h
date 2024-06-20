/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//
#ifndef MWL_PUBLIC
#define MWL_PUBLIC
DEFINE_GUID (GUID_DEVINTERFACE_MWLDXP50USBUMDF2Driver,
    0x9558c94f,0x7e57,0x4133,0x8a,0x06,0xd0,0x1e,0x27,0x0c,0x84,0x82);

// {6aedcf8f-7917-417d-ab43-434ce3659b83}
DEFINE_GUID(GUID_CLASS_MWLDXP50USBUMDF2Driver,
    0x911459de, 0x7768, 0x4fee, 0xa8, 0xef, 0x6c, 0x71, 0x57, 0xc6, 0x24, 0x18);

#define IOCTL_INDEX             0x0000


#define IOCTL_MWLUSB_GET_CONFIG_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     IOCTL_INDEX,     \
                                                     METHOD_BUFFERED,         \
                                                     FILE_ANY_ACCESS)

#define IOCTL_MWLUSB_RESET_DEVICE          CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     IOCTL_INDEX + 1, \
                                                     METHOD_BUFFERED,         \
                                                     FILE_ANY_ACCESS)

#define IOCTL_MWLUSB_RESET_PIPE            CTL_CODE(FILE_DEVICE_UNKNOWN,     \
                                                     IOCTL_INDEX + 2, \
                                                     METHOD_BUFFERED,         \
                                                     FILE_ANY_ACCESS)

#define IOCTL_ADAPT_INDEX 0x0000

// Get the driver version
#define IOCTL_ADAPT_GET_DRIVER_VERSION         CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_ADAPT_INDEX, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Send a raw packet to endpoint 0
#define IOCTL_ADAPT_SEND_EP0_CONTROL_TRANSFER CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_ADAPT_INDEX+8, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Reset the pipe
#define IOCTL_ADAPT_RESET_PIPE                CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_ADAPT_INDEX+11, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Set the base of the IOCTL control codes.  This is somewhat of an
// arbitrary base number, so you can change this if you want unique
// IOCTL codes.  You should consult the Windows NT DDK for valid ranges
// of IOCTL index codes before you choose a base index number.
//

#define Ezusb_IOCTL_INDEX  0x0800


#define IOCTL_EZUSB_GET_PIPE_INFO     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+0,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_GET_DEVICE_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+1,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_GET_CONFIGURATION_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+2,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_BULK_OR_INTERRUPT_WRITE     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+3,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_BULK_OR_INTERRUPT_READ      CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+4,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_VENDOR_REQUEST              CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+5,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_GET_CURRENT_CONFIG          CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+6,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_ANCHOR_DOWNLOAD             CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+7,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_RESET  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+12,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_RESETPIPE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+13,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_ABORTPIPE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+15,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_SETINTERFACE  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+16,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_GET_STRING_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+17,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)


//
// Perform an IN transfer over the specified bulk or interrupt pipe.
//
// lpInBuffer: BULK_TRANSFER_CONTROL stucture specifying the pipe number to read from
// nInBufferSize: sizeof(BULK_TRANSFER_CONTROL)
// lpOutBuffer: Buffer to hold data read from the device.  
// nOutputBufferSize: size of lpOutBuffer.  This parameter determines
//    the size of the USB transfer.
// lpBytesReturned: actual number of bytes read
// 
#define IOCTL_EZUSB_BULK_READ             CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+19,\
                                                   METHOD_OUT_DIRECT,  \
                                                   FILE_ANY_ACCESS)

//
// Perform an OUT transfer over the specified bulk or interrupt pipe.
//
// lpInBuffer: BULK_TRANSFER_CONTROL stucture specifying the pipe number to write to
// nInBufferSize: sizeof(BULK_TRANSFER_CONTROL)
// lpOutBuffer: Buffer of data to write to the device
// nOutputBufferSize: size of lpOutBuffer.  This parameter determines
//    the size of the USB transfer.
// lpBytesReturned: actual number of bytes written
// 
#define IOCTL_EZUSB_BULK_WRITE            CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+20,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

//
// The following IOCTL's are defined as using METHOD_DIRECT_IN buffering.
// This means that the output buffer is directly mapped into system
// space and probed for read access by the driver.  This means that it is
// brought into memory if it happens to be paged out to disk.  Even though
// the buffer is only probed for read access, it is safe (probably) to
// write to it as well.  This read/write capability is used for the loopback
// IOCTL's
// 

// TODO Insert Loopback IOCTL's

//
// Retrieve the current USB frame number from the Host Controller
//
// lpInBuffer: NULL
// nInBufferSize: 0
// lpOutBuffer: PULONG to hold current frame number
// nOutputBufferSize: sizeof(PULONG)
// 
#define IOCTL_EZUSB_GET_CURRENT_FRAME_NUMBER  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+21,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)


//
// Performs a vendor or class specific control transfer to EP0.  The contents of
// the input parameter determine the type of request.  See the USB spec
// for more information on class and vendor control transfers.
//
// lpInBuffer: PVENDOR_OR_CLASS_REQUEST_CONTROL
// nInBufferSize: sizeof(VENDOR_OR_CLASS_REQUEST_CONTROL)
// lpOutBuffer: pointer to a buffer if the request involves a data transfer
// nOutputBufferSize: size of the transfer buffer (corresponds to the wLength
//    field of the USB setup packet)
// 
#define IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+22,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)

//
// Retrieves the actual USBD_STATUS code for the most recently failed
// URB.
//
// lpInBuffer: NULL
// nInBufferSize: 0
// lpOutBuffer: PULONG to hold the URB status
// nOutputBufferSize: sizeof(ULONG)
// 

#define IOCTL_EZUSB_GET_LAST_ERROR   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+23,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

//
// Reads from the specified ISO endpoint. (USB IN Transfer)
//
// lpInBuffer: ISO_TRANSFER_CONTROL
// nInBufferSize: sizeof(ISO_TRANSFER_CONTROL)
// lpOutBuffer: buffer to hold data read from the device
// nOutputBufferSize: size of the read buffer.
//
// 
// 

#define IOCTL_EZUSB_ISO_READ          CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+25,\
                                                   METHOD_OUT_DIRECT,  \
                                                   FILE_ANY_ACCESS)

//
// Writes to the specified ISO endpoint. (USB OUT Transfer)
//
// lpInBuffer: ISO_TRANSFER_CONTROL
// nInBufferSize: sizeof(ISO_TRANSFER_CONTROL)
// lpOutBuffer: buffer to hold data to write to the device
// nOutputBufferSize: size of the write buffer.
//
// 
// 

#define IOCTL_EZUSB_ISO_WRITE          CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+26,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)
#if 0
//
// Performs and Anchor Download.
//
// lpInBuffer: PANCHOR_DOWNLOAD_CONTROL
// nInBufferSize: sizeof(ANCHOR_DOWNLOAD_CONTROL)
// lpOutBuffer: pointer to a buffer of data to download to the device
// nOutputBufferSize: size of the transfer buffer
// 
#define IOCTL_EZUSB_ANCHOR_DOWNLOAD   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+27,\
                                                   METHOD_IN_DIRECT,  \
                                                   FILE_ANY_ACCESS)
#endif 
//
// Returns driver version information
//
// lpInBuffer: NULL
// nInBufferSize: 0
// lpOutBuffer: PEZUSB_DRIVER_VERSION
// nOutputBufferSize: sizeof(EZUSB_DRIVER_VERSION)
// 
#define IOCTL_EZUSB_GET_DRIVER_VERSION   CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+29,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_START_ISO_STREAM     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+30,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_STOP_ISO_STREAM     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+31,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_READ_ISO_BUFFER     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+32,\
                                                   METHOD_OUT_DIRECT,  \
                                                   FILE_ANY_ACCESS)

#define IOCTL_EZUSB_SET_FEATURE         CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   Ezusb_IOCTL_INDEX+33,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)

typedef struct _VENDOR_REQUEST_IN 
{
    BYTE    bRequest;
    WORD    wValue;
    WORD    wIndex;
    WORD    wLength;
    BYTE    direction;
    BYTE    bData;
} VENDOR_REQUEST_IN, *PVENDOR_REQUEST_IN;

typedef struct _ANCHOR_DOWNLOAD_CONTROL 
{
   WORD Offset;
} ANCHOR_DOWNLOAD_CONTROL, *PANCHOR_DOWNLOAD_CONTROL;

#define MAX_INTEL_HEX_RECORD_LENGTH 16

typedef struct _INTEL_HEX_RECORD
{
   BYTE  Length;
   WORD  Address;
   BYTE  Type;
   BYTE  Data[MAX_INTEL_HEX_RECORD_LENGTH];
} INTEL_HEX_RECORD, *PINTEL_HEX_RECORD;

typedef struct _SET_INTERFACE_IN
{
   UCHAR interfaceNum;
   UCHAR alternateSetting;
} SET_INTERFACE_IN, *PSET_INTERFACE_IN;

typedef struct _GET_STRING_DESCRIPTOR_IN
{
   UCHAR    Index;
   USHORT   LanguageId;
} GET_STRING_DESCRIPTOR_IN, *PGET_STRING_DESCRIPTOR_IN; 


typedef struct _BULK_TRANSFER_CONTROL
{
    ULONG pipeNum;
} BULK_TRANSFER_CONTROL, * PBULK_TRANSFER_CONTROL;


///////////////////////////////////////////////////////////
//
// control structure for sending vendor or class specific requests
// to the control endpoint.
//
///////////////////////////////////////////////////////////
typedef struct _VENDOR_OR_CLASS_REQUEST_CONTROL
{
   // transfer direction (0=host to device, 1=device to host)
   UCHAR direction;

   // request type (1=class, 2=vendor)
   UCHAR requestType;

   // recipient (0=device,1=interface,2=endpoint,3=other)
   UCHAR recepient;
   //
   // see the USB Specification for an explanation of the
   // following paramaters.
   //
   UCHAR requestTypeReservedBits;
   UCHAR request;
   USHORT value;
   USHORT index;
} VENDOR_OR_CLASS_REQUEST_CONTROL, *PVENDOR_OR_CLASS_REQUEST_CONTROL;


typedef struct _SET_FEATURE_CONTROL
{
    USHORT FeatureSelector;
    USHORT Index;
} SET_FEATURE_CONTROL, * PSET_FEATURE_CONTROL;

typedef struct _EZUSB_DRIVER_VERSION
{
	   WORD     MajorVersion;
	      WORD     MinorVersion;
	         WORD     BuildVersion;
} EZUSB_DRIVER_VERSION, *PEZUSB_DRIVER_VERSION;

#endif
