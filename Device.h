/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    User-mode Driver Framework 2

--*/

#include "public.h"
#include "private.h"

EXTERN_C_START

typedef struct _DEVICE_CONTEXT {
    USB_DEVICE_DESCRIPTOR           UsbDeviceDescriptor;
    PUSB_CONFIGURATION_DESCRIPTOR   UsbConfigurationDescriptor;
    WDFUSBDEVICE                    WdfUsbTargetDevice;
    WDFWAITLOCK                     ResetDeviceWaitLock;
    ULONG                           WaitWakeEnable;
    BOOLEAN                         IsDeviceHighSpeed;
    BOOLEAN                         IsDeviceSuperSpeed;
    WDFUSBINTERFACE                 UsbInterface;
    UCHAR                           SelectedAlternateSetting;
    UCHAR                           NumberConfiguredPipes;
    ULONG                           MaximumTransferSize;
    BOOLEAN                         IsStaticStreamsSupported;
    USHORT                          NumberOfStreamsSupportedByController;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext)

//
// This context is associated with every open handle.
//
typedef struct _FILE_CONTEXT {

    WDFUSBPIPE Pipe;

} FILE_CONTEXT, *PFILE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FILE_CONTEXT, GetFileContext)

#if (NTDDI_VERSION >= NTDDI_WIN8)

typedef struct _MWLUSB_STREAM_INFO {

    // Number of enabled streams on this pipe
    ULONG NumberOfStreams;

    // Array of stream information structures representing streams on this pipe
    PUSBD_STREAM_INFORMATION StreamList;

} MWLUSB_STREAM_INFO, *PMWLUSB_STREAM_INFO;

#endif

//
// This context is associated with every pipe handle. In this sample,
// it used for isoch transfers.
//
typedef struct _PIPE_CONTEXT {

    ULONG NextFrameNumber;

    ULONG   TransferSizePerMicroframe;

    ULONG   TransferSizePerFrame;

    BOOLEAN  StreamConfigured;

#if (NTDDI_VERSION >= NTDDI_WIN8)
    MWLUSB_STREAM_INFO    StreamInfo;
#endif

} PIPE_CONTEXT, *PPIPE_CONTEXT;


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(PIPE_CONTEXT, GetPipeContext)


//
// This context is associated with every request received by the driver
// from the app.
//
typedef struct _REQUEST_CONTEXT {

    WDFMEMORY         UrbMemory;
    PMDL              Mdl;
    ULONG             Length;         // remaining to xfer
    ULONG             Numxfer;
    ULONG_PTR         VirtualAddress; // va for next segment of xfer.
    BOOLEAN           Read; // TRUE if Read
} REQUEST_CONTEXT, * PREQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_CONTEXT, GetRequestContext)

typedef struct _WORKITEM_CONTEXT {
    WDFDEVICE       Device;
    WDFUSBPIPE      Pipe;
} WORKITEM_CONTEXT, *PWORKITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WORKITEM_CONTEXT, GetWorkItemContext)
extern ULONG DebugLevel;

DRIVER_INITIALIZE DriverEntry;

EVT_WDF_DRIVER_DEVICE_ADD MWLUsb_EvtDeviceAdd;
EVT_WDF_DEVICE_PREPARE_HARDWARE MWLUsb_EvtDevicePrepareHardware;

EVT_WDF_OBJECT_CONTEXT_CLEANUP MWLUsb_EvtDeviceContextCleanup;

#if (NTDDI_VERSION >= NTDDI_WIN8)
EVT_WDF_OBJECT_CONTEXT_CLEANUP  MWLUsb_EvtPipeContextCleanup;
#endif

EVT_WDF_DEVICE_FILE_CREATE MWLUsb_EvtDeviceFileCreate;

EVT_WDF_IO_QUEUE_IO_READ MWLUsb_EvtIoRead;
EVT_WDF_IO_QUEUE_IO_WRITE MWLUsb_EvtIoWrite;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL MWLUsb_EvtIoDeviceControl;

EVT_WDF_REQUEST_COMPLETION_ROUTINE MWLUsb_EvtReadWriteCompletion;
EVT_WDF_REQUEST_COMPLETION_ROUTINE MWLUsb_EvtIsoRequestCompletionRoutine;

EVT_WDF_IO_QUEUE_IO_STOP MWLUsb_EvtIoStop;

EVT_WDF_IO_QUEUE_STATE   MWLUsb_EvtIoQueueReadyNotification;

EVT_WDF_WORKITEM MWLUsb_EvtReadWriteWorkItem;

WDFUSBPIPE
GetPipeFromName(
    _In_ PDEVICE_CONTEXT DeviceContext,
    _In_ PUNICODE_STRING FileName
    );

VOID
ReadWriteIsochEndPoints(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ ULONG Length,
    _In_ WDF_REQUEST_TYPE RequestType
    );

VOID
ReadWriteBulkEndPoints(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ ULONG Length,
    _In_ WDF_REQUEST_TYPE RequestType
    );

NTSTATUS
ResetPipe(
    _In_ WDFUSBPIPE             Pipe
    );

NTSTATUS
ResetDevice(
    _In_ WDFDEVICE Device
    );

NTSTATUS
ReadAndSelectDescriptors(
    _In_ WDFDEVICE Device
    );

NTSTATUS
ConfigureDevice(
    _In_ WDFDEVICE Device
    );

NTSTATUS
SelectInterfaces(
    _In_ WDFDEVICE Device
    );

NTSTATUS
SetPowerPolicy(
        _In_ WDFDEVICE Device
    );

NTSTATUS
AbortPipes(
    _In_ WDFDEVICE Device
    );


NTSTATUS
QueuePassiveLevelCallback(
    _In_ WDFDEVICE    Device,
    _In_ WDFUSBPIPE   Pipe
    );

VOID
PerformIsochTransfer(
    _In_ PDEVICE_CONTEXT  DeviceContext,
    _In_ WDFREQUEST       Request,
    _In_ ULONG            TotalLength
    );

VOID
DbgPrintRWContext(
    PREQUEST_CONTEXT                 rwContext
    );

NTSTATUS
ReadFdoRegistryKeyValue(
    _In_  WDFDRIVER   Driver,
    _In_ LPWSTR       Name,
    _Out_ PULONG      Value
    );

NTSTATUS
InitializePipeContextForHighSpeedDevice(
    _In_ WDFUSBPIPE Pipe
    );

NTSTATUS
InitializePipeContextForFullSpeedDevice(
    _In_ WDFUSBPIPE Pipe
    );

NTSTATUS
RetrieveDeviceInformation(
    _In_ WDFDEVICE Device
    );


USBD_STATUS
MWLUsb_ValidateConfigurationDescriptor(
    _In_reads_bytes_(BufferLength) PUSB_CONFIGURATION_DESCRIPTOR ConfigDesc,
    _In_ ULONG BufferLength,
    _Inout_ PUCHAR *Offset
    );


NTSTATUS
GetStackCapability(
    _In_  WDFDEVICE   Device,
    _In_  const GUID* CapabilityType,
    _In_ ULONG       OutputBufferLength,
    _When_(OutputBufferLength == 0, _Pre_null_)
    _When_(OutputBufferLength != 0, _Out_writes_bytes_(OutputBufferLength))
        PUCHAR      OutputBuffer
    );

#if (NTDDI_VERSION >= NTDDI_WIN8)
NTSTATUS
InitializePipeContextForSuperSpeedDevice(
    _In_ PDEVICE_CONTEXT    DeviceContext,
    _In_ WDFUSBPIPE         Pipe
    );

NTSTATUS
InitializePipeContextForSuperSpeedIsochPipe(
    _In_ PDEVICE_CONTEXT    DeviceContext,
    _In_ UCHAR              InterfaceNumber,
    _In_ WDFUSBPIPE         Pipe
    );

PUSB_ENDPOINT_DESCRIPTOR
GetEndpointDescriptorForEndpointAddress(
    _In_ PDEVICE_CONTEXT  DeviceContext,
    _In_ UCHAR            InterfaceNumber,
    _In_ UCHAR            EndpointAddress,
    _Out_ PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR *ppEndpointCompanionDescriptor
    );

NTSTATUS
InitializePipeContextForSuperSpeedBulkPipe(
    _In_ PDEVICE_CONTEXT    DeviceContext,
    _In_ UCHAR              InterfaceNumber,
    _In_ WDFUSBPIPE         Pipe
    );

USBD_PIPE_HANDLE
GetStreamPipeHandleFromBulkPipe(
    _In_ WDFUSBPIPE Pipe
    );

VOID
ConfigureStreamPipeHandleForRequest(
    _In_ WDFREQUEST       Request,
    _In_ WDFUSBPIPE       Pipe
    );

#endif

ULONG
GetMaxPacketSize(
    _In_ WDFUSBPIPE        Pipe,
    _In_ PDEVICE_CONTEXT   DeviceContext
);

NTSTATUS
MWLUsb_VendorRequest(
    _In_ WDFDEVICE device,
    _In_ PVENDOR_REQUEST_IN pVendorRequest,
    _In_ size_t inBufLength,
    _In_ PVOID outLength,
    _In_ size_t outBufferLength,
    _Out_ size_t *outbufferLength
    );



NTSTATUS
MWLUsb_VendorClassRequest(
    _In_ WDFDEVICE device,
    _In_ WDFREQUEST Request,
    _In_ PVENDOR_OR_CLASS_REQUEST_CONTROL pRequestControl,
    _In_ size_t bufferLength,
    _Out_ VOID* outBuffer,
    _Out_ size_t* outBufferLength
);

NTSTATUS
MWLDXP50USBUMDF2Driver_ResetPipe(
   IN WDFDEVICE device,
   IN WDFREQUEST Request,
   ULONG PipeNum
   );

NTSTATUS
MWLDXP50USBUMDF2Driver_AbortPipe(
    IN WDFDEVICE device,
    IN WDFREQUEST Request,
    IN ULONG PipeNum
    );

ULONG
MWLDXP50USBUMDF2Driver_GetCurrentFrameNumber(
    IN WDFDEVICE device,
    IN WDFREQUEST Request
    );

NTSTATUS
MWLDXP50USBUMDF2Driver_ResetDevice(
    IN WDFDEVICE device,
    IN WDFREQUEST Request
    );

ULONG
Ezusb_DownloadTest(
    IN WDFDEVICE device,
    IN PVENDOR_REQUEST_IN pVendorRequest
    );

PUSB_CONFIGURATION_DESCRIPTOR
GetConfigDescriptor(
    IN WDFDEVICE device
    );


NTSTATUS Ezusb_8051Reset(
   IN WDFDEVICE device,
   UCHAR resetBit
   );

NTSTATUS Ezusb_AnchorDownload(
   IN WDFDEVICE device,
   WORD offset,
   PUCHAR downloadBuffer,
   ULONG downloadSize
   );

NTSTATUS Ezusb_DownloadIntelHex(
   IN WDFDEVICE device,
   PINTEL_HEX_RECORD hexRecord
   );


//
// Function to initialize the device's queues and callbacks
//
NTSTATUS
MWLDXP50USBUMDF2DriverCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

//
// Function to select the device's USB configuration and get a WDFUSBDEVICE
// handle
//
EVT_WDF_DEVICE_PREPARE_HARDWARE MWLDXP50USBUMDF2DriverEvtDevicePrepareHardware;

EXTERN_C_END
