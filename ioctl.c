/*++

Copyright (c) Multiwire Laboratories Ltd.  All rights reserved.

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

    Kernel mode or User Mode

--*/

#include "private.h"
#include "device.h"
#include <malloc.h>

#include "Trace.h"

#include "ioctl.tmh"

VOID
MWLDXP50USBUMDF2DriverEvtIoDeviceControl (_In_ WDFQUEUE   Queue,
                                          _In_ WDFREQUEST Request,
                                          _In_ size_t     OutputBufferLength,
                                          _In_ size_t     InputBufferLength,
                                          _In_ ULONG      IoControlCode)
/*++

Routine Description:

    This event is called when the framework receives IRP_MJ_DEVICE_CONTROL
    requests from the system.

Arguments:

    Queue - Handle to the framework queue object that is associated
            with the I/O request.
    Request - Handle to a framework request object.

    OutputBufferLength - length of the request's output buffer,
                        if an output buffer is available.
    InputBufferLength - length of the request's input buffer,
                        if an input buffer is available.

    IoControlCode - the driver-defined or system-defined I/O control code
                    (IOCTL) that is associated with the request.
Return Value:

    VOID

--*/
{
    WDFDEVICE       device      = WdfIoQueueGetDevice (Queue);
    PDEVICE_CONTEXT pDevContext = GetDeviceContext (device);
    NTSTATUS        status      = STATUS_INVALID_DEVICE_REQUEST;

    ULONG length = 0;

    UNREFERENCED_PARAMETER (OutputBufferLength);
    UNREFERENCED_PARAMETER (InputBufferLength);

    DoTraceMessage (MWLDXP50USBUMDF2Driver_ALL_INFO,
                    "Entered MWLUsb_DispatchDevCtrl control code = %x\n",
                    IoControlCode);
    MWLUsb_DbgPrint (3, ("Entered MWLUsb_DispatchDevCtrl control code = %x\n",
                         IoControlCode));

#ifdef WDF_KERNEL_MODE
    //
    // If your driver is at the top of its driver stack, EvtIoDeviceControl is
    // called at IRQL = PASSIVE_LEVEL.
    //
    _IRQL_limited_to_ (PASSIVE_LEVEL);

    PAGED_CODE ();
#endif

    switch (IoControlCode) {
    case IOCTL_ADAPT_GET_DRIVER_VERSION: {
        VOID  *outBuffer = NULL;
        size_t outLength = 0;
        status = WdfRequestRetrieveOutputBuffer (Request, length, &outBuffer,
                                                 &outLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("EZUSB GET DRIVER VERSION failed\n"));
            break;
        }
        MWLUsb_DbgPrint (3, ("Ezusb Get Driver Version\n"));

        ULONG *version = outBuffer;

        if (outLength >= sizeof (ULONG)) {
            *version = (MWLUSB_MAJOR_VERSION << 16) | (MWLUSB_MINOR_VERSION);
            length   = sizeof (ULONG);
            status   = STATUS_SUCCESS;
        } else {
            status = STATUS_UNSUCCESSFUL;
        }
    } break;

    case IOCTL_MWLUSB_RESET_PIPE: {
        PVOID  inBuffer = NULL;
        size_t inLength = 0;

        MWLUsb_DbgPrint (3, ("MWLUsb Reset Pipe\n"));
        status = WdfRequestRetrieveInputBuffer (Request, length, &inBuffer,
                                                &inLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
    } break;

    case IOCTL_MWLUSB_RESET_DEVICE:
        MWLUsb_DbgPrint (3, ("MWLUsb Reset Device\n"));
        status = MWLDXP50USBUMDF2Driver_ResetDevice (device, Request);
        length = 0;
        break;

    case IOCTL_EZUSB_VENDOR_REQUEST: {
        PVOID  inBuffer  = NULL;
        size_t inLength  = 0;
        PVOID  outBuffer = NULL;
        size_t outLength = 0;

        MWLUsb_DbgPrint (3, ("Ezusb Vendor Request\n"));
        status = WdfRequestRetrieveInputBuffer (Request, length, &inBuffer,
                                                &inLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = WdfRequestRetrieveOutputBuffer (Request, length, &outBuffer,
                                                 &outLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = MWLUsb_VendorRequest (device, Request, (PVENDOR_REQUEST_IN)inBuffer,
                                    inLength, outBuffer, outLength, &inLength);

        MWLUsb_DbgPrint (3, ("Vendor Request returned %d bytes\n", length));
        break;
    }

    case IOCTL_EZUSB_ANCHOR_DOWNLOAD: {

        MWLUsb_DbgPrint (3, ("Ezusb Anchor Download\n"));

        length = 0;
        status = STATUS_NOT_IMPLEMENTED;
    }

    break;

    case IOCTL_EZUSB_GET_CURRENT_CONFIG:
        MWLUsb_DbgPrint (3, ("Ezusb Get Current Config\n"));
        status = STATUS_SUCCESS;
        break;

    case IOCTL_EZUSB_GET_CURRENT_FRAME_NUMBER: {
        PVOID  outBuffer = NULL;
        size_t outLength = 0;

        MWLUsb_DbgPrint (3, ("Ezusb Get Current Frame\n"));
        status = WdfRequestRetrieveOutputBuffer (Request, length, &outBuffer,
                                                 &outLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed status = %x\n", status));
            break;
        }
#if defined(KERNEL_MODE)
        status = WdfUsbTargetDeviceRetrieveCurrentFrameNumber (device, (PULONG)outBuffer);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfUsbTargetDeviceRetrieCurrentFrameNumber failed status = %x\n", status));
            break;
        }
#else
        // UMDF does not support
        (*(PULONG)outBuffer) = 0;
#endif
        length = sizeof(ULONG);

    } break;

    case IOCTL_EZUSB_RESETPIPE: {
        VOID  *inBuffer = NULL;
        size_t inLength = 0;

        MWLUsb_DbgPrint (1, ("Ezusb Reset Pipe\n"));
        status = WdfRequestRetrieveInputBuffer (Request, length, &inBuffer,
                                                &inLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("Reset Pipe: WdfRequestRetrieveInputBuffer "
                                 "failed\n"));
            break;
        }

        if (inBuffer != NULL) {
            status = MWLDXP50USBUMDF2Driver_ResetPipe (device, Request,
                                                       *(ULONG *)inBuffer);
        } else {
            status = STATUS_INVALID_PARAMETER;
        }
    }

    break;

    case IOCTL_EZUSB_ABORTPIPE: {
        VOID  *inBuffer = NULL;
        size_t inLength = 0;

        MWLUsb_DbgPrint (1, ("Ezusb Abort Pipe\n"));
        status = WdfRequestRetrieveInputBuffer (Request, length, &inBuffer,
                                                &inLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("Reset Pipe: WdfRequestRetrieveInputBuffer "
                                 "failed\n"));
            break;
        }

        if (inBuffer != NULL) {
            status = MWLDXP50USBUMDF2Driver_AbortPipe (device, Request,
                                                       *(ULONG *)inBuffer);
        } else {
            status = STATUS_INVALID_PARAMETER;
        }
    }

    break;

    case IOCTL_EZUSB_GET_PIPE_INFO: {
        //
        // inputs  - none
        // outputs - we copy the interface information structure that we
        // have
        //           stored in our device extension area to the output
        //           buffer which will be reflected to the user mode
        //           application by the IOS.
        //
        //
        PVOID  outBuffer = NULL;
        size_t outLength = 0;

        USB_INTERFACE_DESCRIPTOR interface_desc = {0};
        ULONG num_pipe_descs = 0;

        status = WdfRequestRetrieveOutputBuffer (Request, length,
                                                  &outBuffer, &outLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("GET_PIPE_INFO: WdfRetrieveOutputBuffer "
                                 "failed status=0x%x\n",
                                 status));
        break;
        }

        num_pipe_descs = WdfUsbInterfaceGetNumConfiguredPipes( pDevContext->UsbInterface );

        PUSBD_INTERFACE_INFORMATION pusbdinfo = NULL;
        WDFMEMORY   usbdinfo_mem = NULL;
        size_t usbdinfosize = (ULONG)(sizeof(USBD_INTERFACE_INFORMATION)+(sizeof(USBD_PIPE_INFORMATION)*num_pipe_descs));

        WdfUsbInterfaceGetDescriptor( pDevContext->UsbInterface, 0, &interface_desc);

        status = WdfMemoryCreate (
            /*[in, optional]  PWDF_OBJECT_ATTRIBUTES */ NULL,
            /* [in]            POOL_TYPE             */ NonPagedPool,
            /* [in, optional]  ULONG                 */ 'MWLU',
            /* [in]            size_t                */ usbdinfosize,
            /* [out]           WDFMEMORY             */ &usbdinfo_mem,
            /* [out, optional] PVOID                 */ &pusbdinfo);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("GET_PIPE_INFO: WdfMemoryCreate "
                                 "failed status=0x%x\n",
                                 status));
            break;
        }

        pusbdinfo->Length = interface_desc.bLength;
        pusbdinfo->InterfaceNumber = interface_desc.bInterfaceNumber;
        pusbdinfo->AlternateSetting = interface_desc.bAlternateSetting;
        pusbdinfo->Class           = interface_desc.bInterfaceClass;
        pusbdinfo->SubClass        = interface_desc.bInterfaceSubClass;
        pusbdinfo->Protocol        = interface_desc.bInterfaceProtocol;
        pusbdinfo->NumberOfPipes   = interface_desc.bNumEndpoints;
    
        for (ULONG endpt_idx = 0; endpt_idx < pusbdinfo->NumberOfPipes; endpt_idx++ ) {
            WDF_USB_PIPE_INFORMATION pipe_info = {0};

            WdfUsbInterfaceGetEndpointInformation(
                               pDevContext->UsbInterface, /* [in]      WDFUSBINTERFACE           UsbInterface, */
                               0,                         /* [in]      UCHAR                     SettingIndex, */
                               (UCHAR)endpt_idx,          /* [in]      UCHAR                     EndpointIndex,*/
                               &pipe_info                 /* [in, out] PWDF_USB_PIPE_INFORMATION EndpointInfo  */
                         );

            pusbdinfo->Pipes[endpt_idx].MaximumPacketSize = (USHORT)pipe_info.MaximumPacketSize;
            pusbdinfo->Pipes[endpt_idx].EndpointAddress   = pipe_info.EndpointAddress;
            pusbdinfo->Pipes[endpt_idx].Interval          = pipe_info.Interval;
            pusbdinfo->Pipes[endpt_idx].PipeType          = pipe_info.PipeType;
            pusbdinfo->Pipes[endpt_idx].PipeHandle          = 0; // Cant repro in wdf
            pusbdinfo->Pipes[endpt_idx].MaximumTransferSize = pipe_info.MaximumTransferSize;
            pusbdinfo->Pipes[endpt_idx].PipeFlags           = 0; // Cant repro in wdf
        }

        length = (ULONG)((outLength > usbdinfosize)? usbdinfosize : outLength);
        RtlCopyMemory(outBuffer, pusbdinfo, length);

        MWLUsb_DbgPrint (1, ("Ezusb Get Pipe Info\n"));
    }
    break;

    case IOCTL_EZUSB_GET_DEVICE_DESCRIPTOR: {
        VOID  *outBuffer = NULL;
        size_t outLength = 0;

        //
        // inputs  - pointer to a buffer in which to place descriptor
        // data outputs - we put the device descriptor data, if any is
        // returned by the device
        //           in the system buffer and then we set the length
        //           inthe Information field in the Irp, which will
        //           then cause the system to copy the buffer back to
        //           the user's buffer
        //
        if (pDevContext) {

            length = pDevContext->UsbDeviceDescriptor.bLength;
            status = WdfRequestRetrieveOutputBuffer (Request, length,
                                                     &outBuffer, &outLength);
            if (!NT_SUCCESS (status)) {
                MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputB"
                                     "uffer failed\n"));
                break;
            }

            RtlCopyMemory (outBuffer, &pDevContext->UsbDeviceDescriptor,
                           outLength);

            status = STATUS_SUCCESS;
        } else {
            status = STATUS_INVALID_DEVICE_STATE;
        }
        MWLUsb_DbgPrint (
            1, ("Get Device Descriptor returned %d bytes\n", length));
    } break;

    case IOCTL_EZUSB_GET_STRING_DESCRIPTOR: {

        PGET_STRING_DESCRIPTOR_IN pcontrol = NULL;

        void                        *outputBuffer    = NULL;
        void                        *inputBuffer     = NULL;
        size_t                       outBufLength    = 0;
        size_t                       inBufLength     = 0;
        WDF_USB_CONTROL_SETUP_PACKET cntl            = { 0 };
        WDF_MEMORY_DESCRIPTOR        mem_desc        = { 0 };
        BYTE                        *transfer_buffer = NULL;
        WDFMEMORY                    transfer_mem    = NULL;

        status = WdfRequestRetrieveOutputBuffer (
            Request, sizeof (USB_STRING_DESCRIPTOR), &outputBuffer,
            &outBufLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("GET_STRING_DESC: "
                                 "WdfRequestRetrieveOutputBuffer failed\n"));
            break;
        }

        status = WdfRequestRetrieveInputBuffer (
            Request, sizeof (GET_STRING_DESCRIPTOR_IN), &inputBuffer,
            &inBufLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("GET_STRING_DESC: "
                                 "WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }

        pcontrol = inputBuffer;
        MWLUsb_DbgPrint (1, ("Ezusb Get String Desc\n"));
        cntl.Packet.bm.Request.Dir = 1;
        cntl.Packet.bRequest = USB_REQUEST_GET_DESCRIPTOR; // Get Descriptor
        cntl.Packet.wValue.Bytes.HiByte  = USB_STRING_DESCRIPTOR_TYPE;
        cntl.Packet.wValue.Bytes.LowByte = pcontrol->Index;
        cntl.Packet.wIndex.Value         = pcontrol->LanguageId;
        cntl.Packet.wLength              = 255;

        status = WdfMemoryCreate (
            /*[in, optional]  PWDF_OBJECT_ATTRIBUTES */ NULL,
            /* [in]            POOL_TYPE             */ NonPagedPool,
            /* [in, optional]  ULONG                 */ 'MWLU',
            /* [in]            size_t                */ 512,
            /* [out]           WDFMEMORY             */ &transfer_mem,
            /* [out, optional] PVOID                 */
            &transfer_buffer);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("GET_STRING_DESC: WdfMemoryCreate "
                                 "failed status=0x%x\n",
                                 status));
            break;
        }

        mem_desc.Type                = WdfMemoryDescriptorTypeBuffer;
        mem_desc.u.BufferType.Buffer = transfer_buffer;
        mem_desc.u.BufferType.Length = (ULONG)512;

        ULONG transfer_bytes = 0;
        RtlZeroMemory (transfer_buffer, pDevContext->MaximumTransferSize);
        status = WdfUsbTargetDeviceSendControlTransferSynchronously (
            pDevContext->WdfUsbTargetDevice, Request, NULL, &cntl, &mem_desc,
            &transfer_bytes);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("GET_STRING_DESC: "
                                 "WdUsbTargetDeviceSendControlTran"
                                 "sferSynchronously "
                                 "failed status=0x%x\n",
                                 status));
            break;
        }

        length = (ULONG)((transfer_bytes >= outBufLength) ? outBufLength
                                                          : transfer_bytes);

        RtlCopyMemory ((BYTE *)outputBuffer, transfer_buffer, length);
        PUSB_STRING_DESCRIPTOR rtn_desc
            = (PUSB_STRING_DESCRIPTOR)transfer_buffer;
        UNREFERENCED_PARAMETER (rtn_desc);
        MWLUsb_DbgPrint (1,
                         ("GET_STRING_DESC: "
                          "WdUsbTargetDeviceSendControlTransferSynchronously "
                          "returns bDescriptorType = %x bLength = %x\n",
                          rtn_desc->bDescriptorType, rtn_desc->bLength));
    } break;

    case IOCTL_EZUSB_GET_CONFIGURATION_DESCRIPTOR: {
        void  *outBuffer = NULL;
        size_t outLength = 0;
        //
        // inputs  - pointer to a buffer in which to place descriptor
        // data outputs - we put the configuration descriptor data, if
        // any is returned by the device
        //           in the system buffer and then we set the length in
        //           the Information field in the Irp, which will then
        //           cause the system to copy the buffer back to the
        //           user's buffer
        //

        MWLUsb_DbgPrint (1, ("Ezusb Get Config Desc\n"));
        if (pDevContext->UsbConfigurationDescriptor) {

            length = pDevContext->UsbConfigurationDescriptor->wTotalLength;

            status = WdfRequestRetrieveOutputBuffer (Request, length,
                                                     &outBuffer, &outLength);
            if (!NT_SUCCESS (status)) {
                MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputB"
                                     "uffer failed\n"));
                break;
            }

            RtlCopyMemory (outBuffer, pDevContext->UsbConfigurationDescriptor,
                           outLength);

            status = STATUS_SUCCESS;
        } else {
            status = STATUS_INVALID_DEVICE_STATE;
        }
    } break;

    case IOCTL_EZUSB_SETINTERFACE: {
        MWLUsb_DbgPrint (1, ("Ezusb Set Interface\n"));
#if 0
      PSET_INTERFACE_IN input = ioBuffer;
      Irp->IoStatus.Status = SetInterface(fdo,
                                          input->interfaceNum,
                                          input->alternateSetting);
      Irp->IoStatus.Status = 0;
#endif
    } break;

    case IOCTL_EZUSB_RESET:
        MWLUsb_DbgPrint (1, ("Ezusb Reset\n"));
        status = MWLDXP50USBUMDF2Driver_ResetDevice (device, Request);
        break;

    case IOCTL_EZUSB_BULK_WRITE: {
        // The convention for this deviocontrol is a little unusual, in
        // that the "outputbuffer" is used as the data buffer whether
        // the data is read or written. So outputbuffer is the written
        // data, prepopulated by the call.
        //
        // We copy the mdl because it appears that wdf will free the
        // mdl when done. We generate the mdl (for certainly less than
        // 64k in the transfer) because the other parts of the pipeline
        // want it, and will not accept just a pointer.
        //
        // Get the pipe associate with this request.
        //
        MWLUsb_DbgPrint (1, ("Ezusb Bulk Write\n"));
        PVOID  inBuffer  = NULL;
        PVOID  outBuffer = NULL;
        size_t outLength = 0;
        size_t inLength  = 0;
        status = WdfRequestRetrieveInputBuffer (Request, length, &inBuffer,
                                                &inLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = WdfRequestRetrieveOutputBuffer (Request, length, &outBuffer,
                                                 &outLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        PBULK_TRANSFER_CONTROL bulkControl = (PBULK_TRANSFER_CONTROL)inBuffer;
        ULONG                  pipe_num    = bulkControl->pipeNum;

        WDFUSBPIPE pipe = WdfUsbInterfaceGetConfiguredPipe (
            pDevContext->UsbInterface, (BYTE)pipe_num, NULL);
        WDF_USB_PIPE_INFORMATION pipeInfo = { 0 };

        MWLUsb_DbgPrint (1, ("Bulk Write sending %d byte on "
                             "pipehandle %x contents: \n",
                             outLength, pipe));
        if (pipe == NULL) {
            MWLUsb_DbgPrint (1, ("pipe handle is NULL\n"));
            status = STATUS_INVALID_PARAMETER;
            break;
        }
        WDF_USB_PIPE_INFORMATION_INIT (&pipeInfo);
        WdfUsbTargetPipeGetInformation (pipe, &pipeInfo);

        for (int i = 0; i < outLength && i <= 9;) {
            if (outLength - i >= 4) {
                MWLUsb_DbgPrint (1, ("%x %x %x %x ", ((BYTE *)outBuffer)[i],
                                     ((BYTE *)outBuffer)[i + 1],
                                     ((BYTE *)outBuffer)[i + 2],
                                     ((BYTE *)outBuffer)[i + 3]));
                i += 4;
            } else {
                MWLUsb_DbgPrint (1, ("%x ", ((BYTE *)outBuffer)[i]));
                i++;
            }
        }

        MWLUsb_DbgPrint (1, ("\n"));
        if ((WdfUsbPipeTypeBulk == pipeInfo.PipeType)
            || (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType)) {

            WDF_MEMORY_DESCRIPTOR buffer_desc = { 0 };
            buffer_desc.Type                  = WdfMemoryDescriptorTypeBuffer;
            buffer_desc.u.BufferType.Buffer   = outBuffer;
            buffer_desc.u.BufferType.Length   = (ULONG)outLength;
            status = WdfUsbTargetPipeWriteSynchronously (
                pipe, Request, NULL, &buffer_desc, &length);
            if (!NT_SUCCESS (status)) {
                MWLUsb_DbgPrint (1, ("Urb synchronous send failed requset\n"));
                break;
            }
            MWLUsb_DbgPrint (
                1, ("Sent %d bytes, status = %x\n", outLength, status));
        } else {
            status = STATUS_INVALID_DEVICE_STATE;
        }
    } break;
    case IOCTL_EZUSB_BULK_READ: {

        //
        // Get the pipe associated with this request.
        //
        MWLUsb_DbgPrint (1, ("Ezusb Bulk Read\n"));

        PVOID  inBuffer  = NULL;
        PVOID  outBuffer = NULL;
        size_t outLength = 0;
        size_t inLength  = 0;

        status = WdfRequestRetrieveInputBuffer (Request, length, &inBuffer,
                                                &inLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = WdfRequestRetrieveOutputBuffer (Request, length, &outBuffer,
                                                 &outLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }

        PBULK_TRANSFER_CONTROL bulkControl = (PBULK_TRANSFER_CONTROL)inBuffer;
        ULONG                  pipe_num    = bulkControl->pipeNum;

        WDFUSBPIPE pipe = WdfUsbInterfaceGetConfiguredPipe (
            pDevContext->UsbInterface, (BYTE)pipe_num, NULL);
        WDF_USB_PIPE_INFORMATION pipeInfo = { 0 };

        MWLUsb_DbgPrint (
            1, ("Bulk read receiving %d byte on pipehandle %x index %d\n",
                outLength, pipe, pipe_num));

        if (pipe == NULL) {
            MWLUsb_DbgPrint (1, ("pipe handle is NULL\n"));
            status = STATUS_INVALID_PARAMETER;
            break;
        }
        WDF_USB_PIPE_INFORMATION_INIT (&pipeInfo);
        WdfUsbTargetPipeGetInformation (pipe, &pipeInfo);

        if ((WdfUsbPipeTypeBulk == pipeInfo.PipeType)
            || (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType)) {

            BYTE     *transfer_buffer = NULL;
            WDFMEMORY transfer_mem    = NULL;
            status                    = WdfMemoryCreate (
                /*[in, optional]  PWDF_OBJECT_ATTRIBUTES */ NULL,
                /* [in]            POOL_TYPE             */ NonPagedPool,
                /* [in, optional]  ULONG                 */ 'MWLU',
                /* [in]            size_t                */ 0x10000,
                /* [out]           WDFMEMORY             */ &transfer_mem,
                /* [out, optional] PVOID                 */ &transfer_buffer);
            if (!NT_SUCCESS (status)) {
                MWLUsb_DbgPrint (1, ("Bulk Read: WdfMemoryCreate "
                                     "transfer buffer "
                                     "failed status=0x%x\n",
                                     status));
                break;
            }

            ULONG     packet_size   = pipeInfo.MaximumPacketSize;
            BYTE     *packet_buffer = NULL;
            WDFMEMORY packet_mem    = NULL;
            status                  = WdfMemoryCreate (
                /*[in, optional]  PWDF_OBJECT_ATTRIBUTES */ NULL,
                /* [in]            POOL_TYPE             */ NonPagedPool,
                /* [in, optional]  ULONG                 */ 'MWLU',
                /* [in]            size_t                */ 0x10000,
                /* [out]           WDFMEMORY             */ &packet_mem,
                /* [out, optional] PVOID                 */ &packet_buffer);
            if (!NT_SUCCESS (status)) {
                MWLUsb_DbgPrint (1, ("Bulk Read: WdfMemoryCreate "
                                     "packet buffer "
                                     "failed status=0x%x\n",
                                     status));
                break;
            }

            WDF_MEMORY_DESCRIPTOR buffer_desc = { 0 };
            buffer_desc.Type                  = WdfMemoryDescriptorTypeBuffer;
            buffer_desc.u.BufferType.Buffer   = packet_buffer;
            buffer_desc.u.BufferType.Length   = packet_size;

            ULONG to_go           = (ULONG)outLength;
            ULONG transfer_length = 0;
            length                = 0;

            do {
                transfer_length = 0;
                status          = WdfUsbTargetPipeReadSynchronously (
                    pipe, Request, (PWDF_REQUEST_SEND_OPTIONS)NULL,
                    &buffer_desc, &transfer_length);
                if (!NT_SUCCESS (status)) {
                    MWLUsb_DbgPrint (1, (" synchronous read returned "
                                         "status = %x",
                                         status));
                    break;
                }
                RtlCopyMemory (transfer_buffer + length, packet_buffer,
                               transfer_length);
                length += transfer_length;
                to_go -= transfer_length;
                MWLUsb_DbgPrint (1, (" synchronous read returned "
                                     "transfer_length = %d, "
                                     "length = %d to_go = %d status = %x\n",
                                     transfer_length, length, to_go, status));
            } while (transfer_length == packet_size && to_go > 0
                     && NT_SUCCESS (status));

            if (!NT_SUCCESS (status)) {
                MWLUsb_DbgPrint (1, (" synchronous read failed "
                                     "request status = %x\n",
                                     status));
                if (length > 0) {
                    MWLUsb_DbgPrint (1, ("Ezusb bulk read received %d "
                                         "bytes "
                                         "max buffer = %d bytes\n",
                                         length, outLength));
                    RtlCopyMemory (outBuffer, transfer_buffer,
                                   length > outLength ? outLength : length);
                }
                break;
            }
            MWLUsb_DbgPrint (1, ("Ezusb bulk read received %d "
                                 "bytes max buffer = %d bytes\n",
                                 length, outLength));

            RtlCopyMemory (outBuffer, transfer_buffer,
                           length > outLength ? outLength : length);

            MWLUsb_DbgPrint (1, ("Bulk Read returned %d bytes\n", length));
        } else {
            status = STATUS_INVALID_DEVICE_STATE;
        }
    } break;

    case IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST: {
        PVOID  inBuffer  = NULL;
        PVOID  outBuffer = NULL;
        size_t outLength = 0;
        size_t inLength  = 0;

        MWLUsb_DbgPrint (1, ("Ezusb Vendor or CLass Request\n"));

        status = WdfRequestRetrieveInputBuffer (Request, length, &inBuffer,
                                                &inLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = WdfRequestRetrieveOutputBuffer (Request, length, &outBuffer,
                                                 &outLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = MWLUsb_VendorClassRequest (
            device, Request, (PVENDOR_OR_CLASS_REQUEST_CONTROL)inBuffer,
            inLength, outBuffer, &outLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (
                1,
                ("EZUSB_VENDOR_CLASS_REQUEST failed status = %x\n", status));
            break;
        }
        length = (ULONG)outLength;
    } break;

    case IOCTL_EZUSB_GET_LAST_ERROR:
        // The app never uses this, so we don't support it.
	// Since we do all of our requests synchronously, 
	// the status returned from the operation is valid, so
	// this wouldn't provide any info the NTSTATUS doesn't already
        MWLUsb_DbgPrint (1, ("Ezusb Get Last Error\n"));
	status = STATUS_NOT_IMPLEMENTED;
	length = 0;
        break;

    case IOCTL_EZUSB_ISO_READ:
    case IOCTL_EZUSB_ISO_WRITE:
        MWLUsb_DbgPrint (1, ("Ezusb ISO Read/Write\n"));

	// The DXP50 does not do Isoch or extra streams, so we don't implement this

        length = 0;
        status = STATUS_NOT_IMPLEMENTED;
        break;

    case IOCTL_EZUSB_START_ISO_STREAM:
        MWLUsb_DbgPrint (1, ("Ezusb ISO Start Stream\n"));
	// The DXP50 does not do Isoch or extra streams, so we don't implement this
        length = 0;
        status = STATUS_NOT_IMPLEMENTED;
        break;

    case IOCTL_EZUSB_STOP_ISO_STREAM:
        MWLUsb_DbgPrint (1, ("Ezusb ISO Stop Stream\n"));
	// The DXP50 does not do Isoch or extra streams, so we don't implement this
        length = 0;
        status = STATUS_NOT_IMPLEMENTED;
        break;

    case IOCTL_EZUSB_READ_ISO_BUFFER: {
        MWLUsb_DbgPrint (1, ("Ezusb ISO Read Buffer -- not supported\n"));
	// The DXP50 does not do Isoch or extra streams, so we don't implement this
        status = STATUS_NOT_IMPLEMENTED;
        length = 0;
    } break;

    case IOCTL_EZUSB_GET_DRIVER_VERSION: {
        VOID  *outBuffer = NULL;
        size_t outLength = 0;
        status = WdfRequestRetrieveOutputBuffer (Request, length, &outBuffer,
                                                 &outLength);
        if (!NT_SUCCESS (status)) {
            MWLUsb_DbgPrint (1, ("EZUSB GET DRIVER VERSION failed\n"));
            break;
        }
        MWLUsb_DbgPrint (3, ("Ezusb Get Driver Version\n"));

        PEZUSB_DRIVER_VERSION version = (PEZUSB_DRIVER_VERSION)outBuffer;

        if (outLength >= sizeof (EZUSB_DRIVER_VERSION)) {

            version->MajorVersion = MWLUSB_MAJOR_VERSION;
            version->MinorVersion = MWLUSB_MINOR_VERSION;
            version->BuildVersion = MWLUSB_BUILD_VERSION;
            length                = sizeof (EZUSB_DRIVER_VERSION);
            status                = STATUS_SUCCESS;
        } else {
            status = STATUS_UNSUCCESSFUL;
        }
    } break;

    case IOCTL_EZUSB_SET_FEATURE:

        MWLUsb_DbgPrint (3, ("Ezusb Set Feature\n"));
#if 0
      {
         //
         // verify the input params
         //
         if (inputBufferLength != sizeof(SET_FEATURE_CONTROL))
         {
            Irp->IoStatus.Information = 0; 
            Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            break;
         }

         Ezusb_SetFeature(fdo,(PSET_FEATURE_CONTROL)Irp->AssociatedIrp.SystemBuffer);
         Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
      }
#endif
        break;

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        MWLUsb_DbgPrint (
            1, ("Exit MWLUsb_DispatchDevCtrl: Unsupported Ioctl code: %x\n",
                IoControlCode));
        break;
    }
    MWLUsb_DbgPrint (
        3,
        ("MWLUsb_DispatchDevCtrl complete request, status=%x, length = %d\n",
         status, length));
    WdfRequestCompleteWithInformation (Request, status, length);

    MWLUsb_DbgPrint (3, ("Exit MWLUsb_DispatchDevCtrl\n"));

    return;
}

NTSTATUS
MWLUsb_VendorClassRequest (IN WDFDEVICE device, 
                           IN WDFREQUEST Request,
                           IN PVENDOR_OR_CLASS_REQUEST_CONTROL pRequestControl,
                           IN size_t inBufferLength, 
                           OUT VOID *outBuffer,
                           OUT size_t *outBufferLength)
{
    NTSTATUS        ntStatus    = STATUS_UNSUCCESSFUL;
    PDEVICE_CONTEXT pDevContext = GetDeviceContext (device);

    WDF_USB_CONTROL_SETUP_PACKET cntl = { 0 };
    WDF_MEMORY_DESCRIPTOR        mem  = { 0 };
    BYTE *transfer_buffer = NULL;
    ULONG transfer_length = 0;

    UNREFERENCED_PARAMETER (outBuffer);
    UNREFERENCED_PARAMETER (inBufferLength);
    ULONG length = 0;

    MWLUsb_DbgPrint (3, ("Ezusb Vendor Class Request\n"));
    DoTraceMessage (MWLDXP50USBUMDF2Driver_ALL_INFO,
                    "Ezusb Vendor or Class Request. \n");
    cntl.Packet.bm.Request.Dir       = pRequestControl->direction;
    cntl.Packet.bm.Request.Recipient = pRequestControl->recepient;
    cntl.Packet.bm.Request.Type      = pRequestControl->requestType;
    cntl.Packet.bRequest             = pRequestControl->request;
    cntl.Packet.wIndex.Value         = pRequestControl->index;
    cntl.Packet.wValue.Value         = pRequestControl->value;
    cntl.Packet.wLength              = sizeof (cntl);

    if (pRequestControl->direction == 1) {
        transfer_buffer = (BYTE *)outBuffer;
        transfer_length = (ULONG)*outBufferLength;
    }
    else {
        transfer_buffer = (BYTE*)(pRequestControl+1);
        transfer_length = (ULONG)(inBufferLength-sizeof(*pRequestControl));
    }

    mem.Type                = WdfMemoryDescriptorTypeBuffer;
    mem.u.BufferType.Buffer = transfer_buffer;
    mem.u.BufferType.Length = transfer_length;

    ULONG transfer_bytes = 0;
    RtlZeroMemory (transfer_buffer, pDevContext->MaximumTransferSize);
    ntStatus = WdfUsbTargetDeviceSendControlTransferSynchronously (
        pDevContext->WdfUsbTargetDevice, Request, NULL, &cntl, &mem,
        &transfer_bytes);
    if (!NT_SUCCESS (ntStatus)) {
        MWLUsb_DbgPrint (1,
                         ("VENDOR_CLASS_REQUEST: "
                          "WdUsbTargetDeviceSendControlTransferSynchronously "
                          "failed status=0x%x\n",
                          ntStatus));
    } else {
        length = (ULONG)((transfer_bytes >= *outBufferLength) ? *outBufferLength
                                                           : transfer_bytes);
    }

    *outBufferLength = length;

    return ntStatus;
}

NTSTATUS
MWLUsb_VendorRequest (IN WDFDEVICE          device,
                      IN WDFREQUEST         Request,
                      IN PVENDOR_REQUEST_IN pVendorRequest,
                      IN size_t inBufferLength, IN PVOID outputBuffer,
                      IN size_t outputBufferLength, OUT size_t *bytesReceived)
{
    PDEVICE_CONTEXT pDevContext = GetDeviceContext (device);
    NTSTATUS status = STATUS_SUCCESS;
    WDF_USB_CONTROL_SETUP_PACKET cntl = {0};
    BYTE   *transfer_buffer = NULL;
    ULONG   transfer_length = 0;
    WDF_MEMORY_DESCRIPTOR        mem_desc        = { 0 };

    WDF_USB_CONTROL_SETUP_PACKET_INIT_VENDOR( &cntl, 
                                            (pVendorRequest->direction?BmRequestDeviceToHost:BmRequestHostToDevice),
                                            BmRequestToDevice,    // Not specified. We are surmising
                                            pVendorRequest->bRequest,
                                            pVendorRequest->wValue,
                                            pVendorRequest->wIndex);

    // In
    if (pVendorRequest->direction == 1) {
        transfer_buffer = (BYTE *)outputBuffer;
        transfer_length = (ULONG)outputBufferLength;
    }
    else {
        transfer_buffer = &pVendorRequest->bData;
        transfer_length = (ULONG)(inBufferLength-sizeof(*pVendorRequest));
    }

    mem_desc.Type                = WdfMemoryDescriptorTypeBuffer;
    mem_desc.u.BufferType.Buffer = transfer_buffer;
    mem_desc.u.BufferType.Length = (ULONG)transfer_length;

    *bytesReceived = 0;

    ULONG transfer_bytes = 0;
    RtlZeroMemory (transfer_buffer, pDevContext->MaximumTransferSize);
    status = WdfUsbTargetDeviceSendControlTransferSynchronously (
        pDevContext->WdfUsbTargetDevice, Request, NULL, &cntl, &mem_desc,
        &transfer_bytes);
    if (!NT_SUCCESS (status)) {
        MWLUsb_DbgPrint (1, ("VENDOR_REQUEST: "
                             "WdUsbTargetDeviceSendControlTransferSynchronously "
                             "failed status=0x%x\n",
                             status));
        return status;
    }

    *bytesReceived = transfer_bytes;
    return status;
}

NTSTATUS
MWLDXP50USBUMDF2Driver_ResetPipe (IN WDFDEVICE device, IN WDFREQUEST Request,
                                  ULONG pipeNum)

{
    NTSTATUS        status      = STATUS_INVALID_PARAMETER;
    PDEVICE_CONTEXT pDevContext = GetDeviceContext (device);

    WDFUSBPIPE pipe = WdfUsbInterfaceGetConfiguredPipe (
        pDevContext->UsbInterface, (BYTE)pipeNum, NULL);

    MWLUsb_DbgPrint (3,
                     ("Reset pipe, pipehandle %x index %d\n", pipe, pipeNum));

    if (pipe == NULL) {
        MWLUsb_DbgPrint (1, ("pipe handle is NULL\n"));
        status = STATUS_INVALID_PARAMETER;
        return status;
    }
    status = WdfUsbTargetPipeResetSynchronously (pipe, Request, NULL);
    if (!NT_SUCCESS (status)) {
        MWLUsb_DbgPrint (1, ("Reset Pipe: WdUsbTargetPipeResetSynchronously "
                             "failed status=0x%x\n",
                             status));
    }

    return status;
}

NTSTATUS
MWLDXP50USBUMDF2Driver_AbortPipe (IN WDFDEVICE device, IN WDFREQUEST Request,
                                  ULONG pipeNum)
/*++

Routine Description:

   cancel pending transfers for a pipe

Arguments:
    device - current WDF Device Object. Souce of device context and pipe.
    Request - current WDFRequest - passed to wdf

Return Value:


--*/
{
    NTSTATUS        status      = STATUS_INVALID_PARAMETER;
    PDEVICE_CONTEXT pDevContext = GetDeviceContext (device);

    WDFUSBPIPE pipe = WdfUsbInterfaceGetConfiguredPipe (
        pDevContext->UsbInterface, (BYTE)pipeNum, NULL);

    MWLUsb_DbgPrint (3,
                     ("Abort pipe, pipehandle %x index %d\n", pipe, pipeNum));

    if (pipe == NULL) {
        MWLUsb_DbgPrint (1, ("pipe handle is NULL\n"));
        status = STATUS_INVALID_PARAMETER;
        return status;
    }
    status = WdfUsbTargetPipeAbortSynchronously (pipe, Request, NULL);
    if (!NT_SUCCESS (status)) {
        MWLUsb_DbgPrint (1, ("Reset Pipe: WdUsbTargetPipeResetSynchronously "
                             "failed status=0x%x\n",
                             status));
    }

    return status;
}


NTSTATUS
MWLDXP50USBUMDF2Driver_ResetDevice (_In_ WDFDEVICE device, WDFREQUEST Request)
/*++

Routine Description:

    Reset the device port

Arguments:

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER (device);
    UNREFERENCED_PARAMETER (Request);
    MWLUsb_DbgPrint (3, ("MWLDXP50USBUMDF2Driver: enter Ezusb_ResetPort\n"));
    MWLUsb_DbgPrint (
        1, ("MWLDXP50USBUMDF2Driver: Ezusb_ResetPort (%x)\n", status));

    return status;
}


#define BYTES_PER_LINE 0x10

//
// this is the number of bytes of firmware to download per setup transfer.
//
#define CHUNK_SIZE 64

NTSTATUS
Ezusb_AnchorDownload (_In_ WDFDEVICE device, _In_ WORD offset,
                      _In_ PUCHAR downloadBuffer, _In_ ULONG downloadSize)
/*++

Routine Description:
   Uses the ANCHOR LOAD vendor specific command to download code to the EZ-USB
   device.  The actual code is stored as data within the driver binary in the
   global 'firmware' which is an EZUSB_FIRMWARE struct included in the file
   firmware.c.

Arguments:
   fdo - pointer to the device object for this instance of an Ezusb Device
   downloadBuffer - pointer to the firmware image
   downloadSize - total size (bytes) of the firmware image to download

Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    UNREFERENCED_PARAMETER (device);
    UNREFERENCED_PARAMETER (offset);
    UNREFERENCED_PARAMETER (downloadBuffer);
    UNREFERENCED_PARAMETER (downloadSize);
#if 0
   PURB urb = NULL;
   int i;
   int chunkCount;
   PUCHAR ptr = downloadBuffer;

   urb = ExAllocatePool(NonPagedPool, 
                       sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

   if (urb)
   {
      chunkCount = ((downloadSize + CHUNK_SIZE - 1) / CHUNK_SIZE);
      //
      // The download will be split into CHUNK_SIZE pieces and
      // downloaded with multiple setup transfers.  For the Rev B parts
      // CHUNK_SIZE should not exceed 64 bytes, as larger transfers can
      // result in data corruption when other USB devices are present.
      //
      for (i = 0; i < chunkCount; i++)
      {
         RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

         urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
         urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

         urb->UrbControlVendorClassRequest.TransferBufferLength =
            ((i == (chunkCount - 1)) &&  (downloadSize % CHUNK_SIZE)) ?
            (downloadSize % CHUNK_SIZE) :
            CHUNK_SIZE;

         urb->UrbControlVendorClassRequest.TransferBuffer = ptr;
         urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
         urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_INTERNAL;
         urb->UrbControlVendorClassRequest.Value = (USHORT)((i * CHUNK_SIZE) + offset);
         urb->UrbControlVendorClassRequest.Index = 0;

         ntStatus = Ezusb_CallUSBD(fdo, urb);

         if (!NT_SUCCESS(ntStatus))
            break;

         ptr += CHUNK_SIZE;
      }
   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;
   }

   if (urb)
      ExFreePool(urb);
#endif
    return ntStatus;
}

NTSTATUS
Ezusb_SetFeature (_In_ WDFDEVICE            device,
                  _In_ PSET_FEATURE_CONTROL setFeatureControl)
/*
   Routine Description:
   This routine performs a Set Feature control transfer

   Arguments:
   fdo - our device object
   setFeatureControl - a data structure that contains the arguments for the
   set featire command

   Return Value:
   NTSTATUS
*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER (device);
    UNREFERENCED_PARAMETER (setFeatureControl);
    MWLUsb_DbgPrint (3, ("Enter Ezusb_SetFeature\n"));

#if 0
   urb = ExAllocatePool(NonPagedPool, 
                      sizeof(struct _URB_CONTROL_FEATURE_REQUEST));
                   
   if (urb)
   {
      RtlZeroMemory(urb,sizeof(struct _URB_CONTROL_FEATURE_REQUEST));

      urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_FEATURE_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_SET_FEATURE_TO_DEVICE;

      urb->UrbControlFeatureRequest.FeatureSelector = setFeatureControl->FeatureSelector;
      urb->UrbControlFeatureRequest.Index = setFeatureControl->Index;
                                                                  
      ntStatus = Ezusb_CallUSBD(fdo, urb);

   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;        
   }        

   Ezusb_KdPrint ("Leaving Ezusb_SetFeature\n");
#endif
    return ntStatus;
}
