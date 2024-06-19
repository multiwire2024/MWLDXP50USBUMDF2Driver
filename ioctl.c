
#include "private.h"
#include "Trace.h"
#include "ioctl.tmh"
#define _DEBUG
#include <malloc.h>
#include <crtdbg.h>

VOID
MWLDXP50USBUMDF2DriverEvtIoDeviceControl(
    _In_ WDFQUEUE   Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t     OutputBufferLength,
    _In_ size_t     InputBufferLength,
    _In_ ULONG      IoControlCode
    )
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
    WDFDEVICE          device = WdfIoQueueGetDevice(Queue);
    PDEVICE_CONTEXT    pDevContext = GetDeviceContext(device);
    PVOID              ioBuffer = NULL;
    size_t            bufLength;
    NTSTATUS           status = STATUS_INVALID_DEVICE_REQUEST;
   
   // PFILE_CONTEXT      pFileContext;
    ULONG              length = 0;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    DoTraceMessage(MWLUSBDRIVER_ALL_INFO, "Entered MWLUsb_DispatchDevCtrl control code = %x\n", IoControlCode);
    MWLUsb_DbgPrint(1, ("Entered MWLUsb_DispatchDevCtrl control code = %x\n", IoControlCode));

#ifdef WDF_KERNEL_MODE
    //
    // If your driver is at the top of its driver stack, EvtIoDeviceControl is called
    // at IRQL = PASSIVE_LEVEL.
    //
    _IRQL_limited_to_(PASSIVE_LEVEL);

    PAGED_CODE();
#endif


    switch(IoControlCode) {
    case IOCTL_ADAPT_GET_DRIVER_VERSION:
    {
        VOID* outBuffer = NULL;
        size_t outLength = 0;
        status = WdfRequestRetrieveOutputBuffer(Request, length, &outBuffer, &outLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("EZUSB GET DRIVER VERSION failed\n"));
            break;
        }
        MWLUsb_DbgPrint(1, ("Ezusb Get Driver Version\n"));

        ULONG* version = outBuffer;

        if (outLength >= sizeof(ULONG))
        {
            *version = (MWLUSB_MAJOR_VERSION <<16) | (MWLUSB_MINOR_VERSION);
            length = sizeof(ULONG);
            status = STATUS_SUCCESS;
        }
        else
        {
            status = STATUS_UNSUCCESSFUL;
        }
    }
        break;

    case IOCTL_MWLUSB_RESET_PIPE:
        PVOID inBuffer = NULL;
        size_t inLength = 0;

        MWLUsb_DbgPrint(1, ("MWLUsb Reset Pipe\n"));
        status = WdfRequestRetrieveInputBuffer(Request, length, &inBuffer, &inLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }

        status = MWLDXP50USBDriver_ResetPipe(device, Request, (ULONG*)inBuffer);
        length = 0;
        MWLUsb_DbgPrint(1, ("Reset pipenum %d, status = %x\n", pipe_num, status ));

        break;

#if 0
    case IOCTL_MWLUSB_RESET_DEVICE:
        MWLUsb_DbgPrint(1, ("MWLUsb Reset Device\n"));
        status = ResetDevice(device);
        break;
#endif

    case IOCTL_EZUSB_VENDOR_REQUEST:
        MWLUsb_DbgPrint(1, ("Ezusb Vendor Request\n"));
        status = WdfRequestRetrieveInputBuffer(Request, length, &ioBuffer, &bufLength);
        if (!NT_SUCCESS(status)){
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = WdfRequestRetrieveOutputBuffer(Request, length, &ioBuffer, &bufLength);
        if (!NT_SUCCESS(status)){
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = MWLUsb_VendorRequest (device, (PVENDOR_REQUEST_IN) ioBuffer, bufLength, &bufLength);
 
        MWLUsb_DbgPrint(1, ("Vendor Request returned %d bytes\n", length));
        break;

   case IOCTL_EZUSB_ANCHOR_DOWNLOAD:
   {

        MWLUsb_DbgPrint(1, ("Ezusb Anchor Download\n"));
        
#if 0
#define CHUNKLENGTH 512

      PURB                urb             = NULL;
      int   chunkcount;
      //int   chunklength = 1024;
      char *ioBufferPtr = NULL;
      int i;

      urb = ExAllocatePool(NonPagedPool, 
                          sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

      Ezusb_KdPrint ("IOCTL_Ezusb_ANCHOR_DOWNLOAD\n");
             
      if (urb)
      {
         chunkcount = inputBufferLength / CHUNKLENGTH;
         ioBufferPtr = (char*)ioBuffer;

         for (i=0;i<chunkcount;i++)
         {
            RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

            urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
            urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

            Ezusb_KdPrint ("ioBuffer: %x | inputBufferLength: %d | outputBufferLength: %d\n",
                              ioBuffer, inputBufferLength, outputBufferLength);

            urb->UrbControlVendorClassRequest.TransferBufferLength = CHUNKLENGTH;
            urb->UrbControlVendorClassRequest.TransferBuffer = ioBufferPtr;
            urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
            urb->UrbControlVendorClassRequest.Request = 0xA0;
            urb->UrbControlVendorClassRequest.Value = (USHORT)(i * CHUNKLENGTH);
            urb->UrbControlVendorClassRequest.Index = 0;

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            ioBufferPtr += CHUNKLENGTH;
         }

         ExFreePool(urb);
      }
#else
        status = STATUS_SUCCESS;
#endif
   }

   break;

 
   
   case IOCTL_EZUSB_GET_CURRENT_CONFIG:
      MWLUsb_DbgPrint(1, ("Ezusb Get Current Config\n"));
      status = STATUS_SUCCESS;
      break;

   case IOCTL_EZUSB_GET_CURRENT_FRAME_NUMBER:
   {

        MWLUsb_DbgPrint(1, ("Ezusb Get Current Frame\n"));
        status = WdfRequestRetrieveOutputBuffer(Request, length, &outBuffer, &outLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("Reset Pipe: WdfRequestRetrieveInputBuffer failed\n"));
	    length = 0;
            break;
        }
	status = WdfUsbTargetDeviceRetrieveCurrentFrameNumber(device, (ULONG*)outBuffer);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("Reset Pipe: WdfUsbTargetDeviceRetrieveCurrentFrameNumber failed status = 0x%x\n", status));
	    length = 0;
            break;
        }
        length = sizeof(ULONG);
   }
   break;

   case IOCTL_EZUSB_RESETPIPE:
   {
        MWLUsb_DbgPrint(1, ("Ezusb Reset Pipe\n"));
        status = WdfRequestRetrieveInputBuffer(Request, length, &inBuffer, &inLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("Reset Pipe: WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = MWLDXP50USBDriver_ResetPipe(device, Request, (ULONG*)inBuffer);
        length = 0;
   }

   break;

   case IOCTL_EZUSB_ABORTPIPE:
   {
        MWLUsb_DbgPrint(1, ("Ezusb Abort Pipe\n"));
        status = WdfRequestRetrieveInputBuffer(Request, length, &inBuffer, &inLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("Reset Pipe: WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = MWLDXP50USBDriver_ResetPipe(device, Request, (ULONG*)inBuffer);
        length = 0;
   }
   
   break;

   case IOCTL_EZUSB_GET_PIPE_INFO:
      //
      // inputs  - none
      // outputs - we copy the interface information structure that we have
      //           stored in our device extension area to the output buffer which
      //           will be reflected to the user mode application by the IOS.
      //
#if 0
      length = 0;
      pch = (PUCHAR) ioBuffer;

      if (pdx->Interface)
      {
         RtlCopyMemory(pch+length,
                       (PUCHAR) pdx->Interface,
                       pdx->Interface->Length);

         length += pdx->Interface->Length;
      }


      Irp->IoStatus.Information = length;
      Irp->IoStatus.Status = STATUS_SUCCESS;
#endif
      MWLUsb_DbgPrint(1, ("Ezusb Get Pipe Info\n"));
      break;

   case IOCTL_EZUSB_GET_DEVICE_DESCRIPTOR:
      //
      // inputs  - pointer to a buffer in which to place descriptor data
      // outputs - we put the device descriptor data, if any is returned by the device
      //           in the system buffer and then we set the length inthe Information field
      //           in the Irp, which will then cause the system to copy the buffer back
      //           to the user's buffer
      //

      if (pDevContext) {
  
          length = pDevContext->UsbDeviceDescriptor.bLength;
          status = WdfRequestRetrieveOutputBuffer(Request, length, &ioBuffer, &bufLength);
          if (!NT_SUCCESS(status)){
              MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
              break;
          }
  
          RtlCopyMemory(ioBuffer,
                        &pDevContext->UsbDeviceDescriptor,
                        length);
  
          status = STATUS_SUCCESS;
      }
      else {
          status = STATUS_INVALID_DEVICE_STATE;
      }

      MWLUsb_DbgPrint(1, ("Get Device Descriptor returned %d bytes\n", length));

      break;

   case IOCTL_EZUSB_GET_STRING_DESCRIPTOR:
   {
    
       PGET_STRING_DESCRIPTOR_IN pcontrol = NULL;

       void* outputBuffer = NULL;
       void* inputBuffer = NULL;
       size_t outBufLength = 0;
       size_t inBufLength = 0; 
       WDF_USB_CONTROL_SETUP_PACKET cntl = { 0 };
       WDF_MEMORY_DESCRIPTOR mem = { 0 };

       status = WdfRequestRetrieveOutputBuffer(Request, sizeof(USB_STRING_DESCRIPTOR), &outputBuffer, &outBufLength);
       if (!NT_SUCCESS(status)) {
           MWLUsb_DbgPrint(1, ("GET_STRING_DESC: WdfRequestRetrieveOutputBuffer failed\n"));
           break;
       }

       status = WdfRequestRetrieveInputBuffer(Request, sizeof(GET_STRING_DESCRIPTOR_IN), &inputBuffer, &inBufLength);
       if (!NT_SUCCESS(status)) {
           MWLUsb_DbgPrint(1, ("GET_STRING_DESC: WdfRequestRetrieveInputBuffer failed\n"));
           break;
       }

       pcontrol = inputBuffer;
       MWLUsb_DbgPrint(1, ("Ezusb Get String Desc\n"));
       cntl.Packet.bm.Request.Dir = 1;
       cntl.Packet.bRequest = USB_REQUEST_GET_DESCRIPTOR; // Get Descriptor
       cntl.Packet.wValue.Bytes.HiByte = USB_STRING_DESCRIPTOR_TYPE;
       cntl.Packet.wValue.Bytes.LowByte = pcontrol->Index;
       cntl.Packet.wIndex.Value = pcontrol->LanguageId;
       cntl.Packet.wLength = 255;
   
#ifdef WDF_KERNEL_MODE
       BYTE* transfer_buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, pDevContext->MaximumTransferSize, 'MWLU');
#else
       BYTE* transfer_buffer = malloc(pDevContext->MaximumTransferSize);
#endif
       mem.Type = WdfMemoryDescriptorTypeBuffer;
       mem.u.BufferType.Buffer = transfer_buffer;
       mem.u.BufferType.Length = (ULONG)256;

       ULONG transfer_bytes = 0;
       RtlZeroMemory(transfer_buffer, pDevContext->MaximumTransferSize);
       status = WdfUsbTargetDeviceSendControlTransferSynchronously(pDevContext->WdfUsbTargetDevice, Request, NULL, &cntl, &mem, &transfer_bytes);
       if (!NT_SUCCESS(status)) {
           MWLUsb_DbgPrint(1, ("GET_STRING_DESC: WdUsbTargetDeviceSendControlTransferSynchronously failed status=0x%x\n", status));
#ifdef WDF_KERNEL_MODE
           ExFreePoolWithTag(transfer_buffer, 'MWLU');
#else
           free(transfer_buffer);
#endif
           break;
       }

       length = (ULONG)((transfer_bytes >= outBufLength) ? outBufLength : transfer_bytes);

       RtlCopyMemory((BYTE*)outputBuffer, transfer_buffer, length );

       PUSB_STRING_DESCRIPTOR rtn_desc = (PUSB_STRING_DESCRIPTOR)transfer_buffer;
       UNREFERENCED_PARAMETER(rtn_desc);
       MWLUsb_DbgPrint(1, ("GET_STRING_DESC: WdUsbTargetDeviceSendControlTransferSynchronously returns bDescriptorType = %x bLength = %x\n", rtn_desc->bDescriptorType, rtn_desc->bLength));
#ifdef WDF_KERNEL_MODE
       ExFreePoolWithTag(transfer_buffer, 'MWLU');
#else
       free(transfer_buffer);
#endif

   } 
   break;

      case IOCTL_EZUSB_GET_CONFIGURATION_DESCRIPTOR:
         //
         // inputs  - pointer to a buffer in which to place descriptor data
         // outputs - we put the configuration descriptor data, if any is returned by the device
         //           in the system buffer and then we set the length in the Information field
         //           in the Irp, which will then cause the system to copy the buffer back
         //           to the user's buffer
         //

        MWLUsb_DbgPrint(1, ("Ezusb Get Config Desc\n"));
        if (pDevContext->UsbConfigurationDescriptor) {

            length = pDevContext->UsbConfigurationDescriptor->wTotalLength;

            status = WdfRequestRetrieveOutputBuffer(Request, length, &ioBuffer, &bufLength);
            if (!NT_SUCCESS(status)){
                MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
                break;
            }

            RtlCopyMemory(ioBuffer,
                          pDevContext->UsbConfigurationDescriptor,
                          length);

            status = STATUS_SUCCESS;
        }
        else {
            status = STATUS_INVALID_DEVICE_STATE;
        }

        break;

    case IOCTL_EZUSB_SETINTERFACE:
    {
        MWLUsb_DbgPrint(1, ("Ezusb Set Interface\n"));
 #if 0
      PSET_INTERFACE_IN input = ioBuffer;
      Irp->IoStatus.Status = SetInterface(fdo,
                                          input->interfaceNum,
                                          input->alternateSetting);
      Irp->IoStatus.Status = 0;
#endif
     }
     break;
 
    case IOCTL_EZUSB_RESET:
        MWLUsb_DbgPrint(1, ("Ezusb Reset\n"));
        status = MWLUSBDXP50USBUMDF2Driver_ResetDevice(device);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
        }
	length = 0;
        break;

    case IOCTL_EZUSB_BULK_WRITE:
    {
        // The convention for this deviocontrol is a little unusual, in that the "outputbuffer" is used as the data buffer
        // whether the data is read or written. So outputbuffer is the written data, prepopulated by the call.
        // 
        // We copy the mdl because it appears that wdf will free the mdl when done. We generate the mdl (for certainly less than 64k 
        // in the transfer) because the other parts of the pipeline want it, and will not accept just a pointer.
        //
        // Get the pipe associate with this request.
        //
        MWLUsb_DbgPrint(1, ("Ezusb Bulk Write\n"));
        PVOID inBuffer = NULL;
        PVOID outBuffer = NULL;
        size_t outLength = 0;
        size_t inLength = 0;

        status = WdfRequestRetrieveInputBuffer(Request, length, &inBuffer, &inLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = WdfRequestRetrieveOutputBuffer(Request, length, &outBuffer, &outLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        PBULK_TRANSFER_CONTROL     bulkControl = (PBULK_TRANSFER_CONTROL)inBuffer;
        ULONG pipe_num = bulkControl->pipeNum;
        
        WDFUSBPIPE pipe = WdfUsbInterfaceGetConfiguredPipe(pDevContext->UsbInterface, (BYTE) pipe_num, NULL);
        WDF_USB_PIPE_INFORMATION pipeInfo = { 0 };

        MWLUsb_DbgPrint(1, ("Bulk Write sending %d byte on pipehandle %x contents: \n", outLength, pipe));
        if (pipe == NULL) {
            MWLUsb_DbgPrint(1, ("pipe handle is NULL\n"));
            status = STATUS_INVALID_PARAMETER;
            break;
        }
        WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);
        WdfUsbTargetPipeGetInformation(pipe, &pipeInfo);

        for (int i = 0; i < outLength && i <= 9;) {
            if (outLength - i >= 4) {
                MWLUsb_DbgPrint(1, ("%x %x %x %x ", ((BYTE*)outBuffer)[i], ((BYTE*)outBuffer)[i+1], ((BYTE*)outBuffer)[i+2], ((BYTE*)outBuffer)[i+3]
                    ));
                i += 4;
            }
            else {
                MWLUsb_DbgPrint(1, ("%x ", ((BYTE*)outBuffer)[i]));
                i++;
            }

        }
        MWLUsb_DbgPrint(1, ("\n"));
        if ((WdfUsbPipeTypeBulk == pipeInfo.PipeType) ||
            (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType)) {

            WDF_MEMORY_DESCRIPTOR buffer_desc = { 0 };
            buffer_desc.Type = WdfMemoryDescriptorTypeBuffer;
            buffer_desc.u.BufferType.Buffer = outBuffer;
            buffer_desc.u.BufferType.Length = (ULONG)outLength;
            status = WdfUsbTargetPipeWriteSynchronously(pipe, Request, NULL, &buffer_desc, &length);
            if (!NT_SUCCESS(status)) {
                MWLUsb_DbgPrint(1, ("Urb synchronous send failed requset\n"));
                break;
            }
            MWLUsb_DbgPrint(1, ("Sent %d bytes, status = %x\n", outLength, status ));
        }
        else {
            status = STATUS_INVALID_DEVICE_STATE;
        }
    }  
    break;
    case IOCTL_EZUSB_BULK_READ:
    {
       
        //
        // Get the pipe associate with this request.
        //
        MWLUsb_DbgPrint(1, ("Ezusb Bulk Read\n"));
        // Get the pipe associate with this request.
  //
        MWLUsb_DbgPrint(1, ("Ezusb Bulk Write\n"));
        PVOID inBuffer = NULL;
        PVOID outBuffer = NULL;
        size_t outLength = 0;
        size_t inLength = 0;

        status = WdfRequestRetrieveInputBuffer(Request, length, &inBuffer, &inLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = WdfRequestRetrieveOutputBuffer(Request, length, &outBuffer, &outLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }

        PBULK_TRANSFER_CONTROL     bulkControl = (PBULK_TRANSFER_CONTROL)inBuffer;
        ULONG pipe_num = bulkControl->pipeNum;

        WDFUSBPIPE pipe = WdfUsbInterfaceGetConfiguredPipe(pDevContext->UsbInterface, (BYTE)pipe_num, NULL);
        WDF_USB_PIPE_INFORMATION pipeInfo = { 0 };

        MWLUsb_DbgPrint(1, ("Bulk read receiving %d byte on pipehandle %x index %d\n", outLength, pipe, pipe_num));

        if (pipe == NULL) {
            MWLUsb_DbgPrint(1, ("pipe handle is NULL\n"));
            status = STATUS_INVALID_PARAMETER;
            break;
        }
        WDF_USB_PIPE_INFORMATION_INIT(&pipeInfo);
        WdfUsbTargetPipeGetInformation(pipe, &pipeInfo);

        if ((WdfUsbPipeTypeBulk == pipeInfo.PipeType) ||
            (WdfUsbPipeTypeInterrupt == pipeInfo.PipeType)) {

#ifdef WDF_KERNEL_MODE
            BYTE* transfer_buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, outLength, 'MWLU');
#else
            BYTE* transfer_buffer = malloc(outLength);
#endif

            WDF_MEMORY_DESCRIPTOR buffer_desc = { 0 };
            buffer_desc.Type = WdfMemoryDescriptorTypeBuffer;
            buffer_desc.u.BufferType.Buffer = transfer_buffer;
            buffer_desc.u.BufferType.Length = (ULONG)outLength;
            status = WdfUsbTargetPipeReadSynchronously(pipe, Request, (PWDF_REQUEST_SEND_OPTIONS)NULL, &buffer_desc, &length);
            if (!NT_SUCCESS(status)) {
                MWLUsb_DbgPrint(1, (" synchronous read failed request status = %x\n", status));
#ifdef WDF_KERNEL_MODE
                ExFreePoolWithTag(transfer_buffer, 'MWLU');
#else
                free(transfer_buffer);
#endif
               
#if 0
                if (copyMdl != NULL) {
                    IoFreeMdl(copyMdl);

                    
                }
#endif
                break;
            }
            MWLUsb_DbgPrint(1, ("Ezusb bulk read received %d bytes max buffer = %d bytes\n", length, outLength ));


            RtlCopyMemory(outBuffer, transfer_buffer, length);
#ifdef WDF_KERNEL_MODE
            ExFreePoolWithTag(transfer_buffer, 'MWLU');
#else
            free(transfer_buffer);
#endif
        }
        else {
            status = STATUS_INVALID_DEVICE_STATE;
        }
        
        MWLUsb_DbgPrint(1, ("Bulk Read returned %d bytes\n", length));
        
    }
    break;

    case IOCTL_EZUSB_VENDOR_OR_CLASS_REQUEST: {
        PVOID inBuffer = NULL;
        PVOID outBuffer = NULL;
        size_t outLength = 0;
        size_t inLength = 0;

        MWLUsb_DbgPrint(1, ("Ezusb Vendor or CLass Request\n"));
     
        status = WdfRequestRetrieveInputBuffer(Request, length, &inBuffer, &inLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = WdfRequestRetrieveOutputBuffer(Request, length, &outBuffer, &outLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("WdfRequestRetrieveInputBuffer failed\n"));
            break;
        }
        status = MWLUsb_VendorClassRequest(device, Request, (PVENDOR_OR_CLASS_REQUEST_CONTROL)inBuffer, inLength, outBuffer, &outLength);
        if (!NT_SUCCESS(status)) {
            MWLUsb_DbgPrint(1, ("EZUSB_VENDOR_CLASS_REQUEST failed status = %x\n", status));
            break;
        }
        length = (ULONG)outLength;
    }
    break;

    case IOCTL_EZUSB_GET_LAST_ERROR:

      //
      // make sure the output buffer is ok, and then copy the most recent
      // URB status from the device extension to it
      //
     MWLUsb_DbgPrint(1, ("Ezusb Get Last Error\n"));
#if 0
      if (outputBufferLength >= sizeof(ULONG))
      {
         *((PULONG)ioBuffer) = pdx->LastFailedUrbStatus;
         Irp->IoStatus.Status = STATUS_SUCCESS;
         Irp->IoStatus.Information = sizeof(ULONG);
      }
      else
      {
         Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
      }
#endif
      break;

      case IOCTL_EZUSB_ISO_READ:
      case IOCTL_EZUSB_ISO_WRITE:
        MWLUsb_DbgPrint(1, ("Ezusb ISO Read/Write not supported\n"));
	status = STATUS_INVALID_PARAMETER;
	length = 0;
      break;

      case IOCTL_EZUSB_START_ISO_STREAM:
        MWLUsb_DbgPrint(1, ("Ezusb ISO Start Stream not supported\n"));
	status = STATUS_INVALID_PARAMETER;
	length = 0;
      break;

      case IOCTL_EZUSB_STOP_ISO_STREAM:
        MWLUsb_DbgPrint(1, ("Ezusb ISO Stop Stream not supported\n"));
	status = STATUS_INVALID_PARAMETER;
	length = 0;
      break;

      case IOCTL_EZUSB_READ_ISO_BUFFER:
      {
        MWLUsb_DbgPrint(1, ("Ezusb ISO Read Buffer Not supported\n"));
	status = STATUS_INVALID_PARAMETER;
	length = 0;
      }
      break;

      case IOCTL_EZUSB_GET_DRIVER_VERSION:
      {
          VOID* outBuffer = NULL;
          size_t outLength = 0;
          status = WdfRequestRetrieveOutputBuffer(Request, length, &outBuffer, &outLength);
          if (!NT_SUCCESS(status)) {
              MWLUsb_DbgPrint(1, ("EZUSB GET DRIVER VERSION failed\n"));
              break;
          }
          MWLUsb_DbgPrint(1, ("Ezusb Get Driver Version\n"));

          PEZUSB_DRIVER_VERSION version = (PEZUSB_DRIVER_VERSION) outBuffer;

          if (outLength >= sizeof(EZUSB_DRIVER_VERSION))
          {
              version->MajorVersion = MWLUSB_MAJOR_VERSION;
              version->MinorVersion = MWLUSB_MINOR_VERSION;
              version->BuildVersion = MWLUSB_BUILD_VERSION;
              length = sizeof(EZUSB_DRIVER_VERSION);
              status = STATUS_SUCCESS;
         }
         else
         {
            status = STATUS_UNSUCCESSFUL;
         }
      }
      break;

      case IOCTL_EZUSB_SET_FEATURE:

        MWLUsb_DbgPrint(1, ("Ezusb Set Feature\n"));
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

    default :
        status = STATUS_INVALID_DEVICE_REQUEST;
        MWLUsb_DbgPrint(1, ("Exit MWLUsb_DispatchDevCtrl: Unsupported Ioctl code: %d\n", IoControlCode));
        break;
    }

    WdfRequestCompleteWithInformation(Request, status, length);

    MWLUsb_DbgPrint(3, ("Exit MWLUsb_DispatchDevCtrl\n"));

    return;
}


NTSTATUS
MWLUsb_VendorClassRequest(
    _In_ WDFDEVICE device,
    _In_ WDFREQUEST Request,
    _In_ PVENDOR_OR_CLASS_REQUEST_CONTROL pRequestControl,
    _In_ size_t inBufferLength,
    _Out_ VOID *outBuffer,
    _Out_ size_t* outBufferLength
   )
{
   NTSTATUS                   ntStatus = STATUS_UNSUCCESSFUL;
   PDEVICE_CONTEXT            pDevContext = GetDeviceContext(device);

   WDF_USB_CONTROL_SETUP_PACKET cntl = { 0 };
   WDF_MEMORY_DESCRIPTOR mem = { 0 };

   UNREFERENCED_PARAMETER(outBuffer);
   UNREFERENCED_PARAMETER(inBufferLength);
  

   MWLUsb_DbgPrint(1, ("Ezusb Vendor Class Request\n"));
   DoTraceMessage(MWLUSBDRIVER_ALL_INFO, "Ezusb Vendor or Class Request. \n");
   cntl.Packet.bm.Request.Dir = pRequestControl->direction;
   cntl.Packet.bm.Request.Recipient = pRequestControl->recepient;
   cntl.Packet.bm.Request.Type = pRequestControl->requestType;
   cntl.Packet.bRequest = pRequestControl->request;
   cntl.Packet.wIndex.Value = pRequestControl->index;
   cntl.Packet.wValue.Value = pRequestControl->value;
   cntl.Packet.wLength = sizeof(cntl);
   
#ifdef WDF_KERNEL_MODE
   BYTE* transfer_buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, pDevContext->MaximumTransferSize, 'MWLU');
#else
   BYTE* transfer_buffer = malloc(pDevContext->MaximumTransferSize);
#endif
       mem.Type = WdfMemoryDescriptorTypeBuffer;
       mem.u.BufferType.Buffer = transfer_buffer;
       mem.u.BufferType.Length = (ULONG)512;

       ULONG transfer_bytes = 0;
       ULONG length = 0;
       RtlZeroMemory(transfer_buffer, pDevContext->MaximumTransferSize);
       ntStatus = WdfUsbTargetDeviceSendControlTransferSynchronously(pDevContext->WdfUsbTargetDevice, Request, NULL, &cntl, &mem, &transfer_bytes);
       if (!NT_SUCCESS(ntStatus)) {
           MWLUsb_DbgPrint(1, ("VENDOR_OR_CLASS_REQUEST: WdUsbTargetDeviceSendControlTransferSynchronously failed status=0x%x\n", ntStatus));
         
       }
       else {
           length = (ULONG)((transfer_bytes >= *outBufferLength) ? *outBufferLength : transfer_bytes);

           RtlCopyMemory((BYTE*)outBuffer, transfer_buffer, length);
           MWLUsb_DbgPrint(1, ("VENDOR_OR_CLASS_REQUEST: WdUsbTargetDeviceSendControlTransferSynchronously returns bDescriptorType = %x bLength = %x\n", rtn_desc->bDescriptorType, rtn_desc->bLength));
          
       }
#ifdef WDF_KERNEL_MODE
       ExFreePoolWithTag(transfer_buffer, 'MWLU');
#endif


   *outBufferLength = length;

   return ntStatus;
}


NTSTATUS
MWLUsb_VendorRequest(
    IN WDFDEVICE device,
    IN PVENDOR_REQUEST_IN pVendorRequest,
    IN size_t bufferLength,
    OUT size_t * outbufferLength
    )
{
    NTSTATUS            ntStatus        = STATUS_SUCCESS;
   
    
    MWLUsb_DbgPrint (1, ("Enter Ezusb_VendorRequest - yahoooo\n"));    
#if 0
    urb = ExAllocatePool(NonPagedPool, 
                         sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));
                         
    if (urb)
    {
        RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

        //
        // fill in the URB
        //
        urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

        urb->UrbControlVendorClassRequest.TransferBufferLength = pVendorRequest->wLength;
        urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
        urb->UrbControlVendorClassRequest.Request = pVendorRequest->bRequest;
        urb->UrbControlVendorClassRequest.Value = pVendorRequest->wValue;
        urb->UrbControlVendorClassRequest.Index = pVendorRequest->wIndex;


        //
        // very kludgey.  The idea is: if its an IN then a buffer has been passed
        // in from user mode.  So, use the pointer to the system buffer as the transfer
        // buffer.  If the transfer is an out, then we need to allocate a transfer 
        // buffer.  If the length of the transfer is 1, then put pVendorRequest->bData
        // in the buffer.  Otherwise, fill the buffer with an incrementing byte pattern.
        // yuch
        //
        if (pVendorRequest->direction)
        {
            urb->UrbControlVendorClassRequest.TransferFlags |= USBD_TRANSFER_DIRECTION_IN;
            urb->UrbControlVendorClassRequest.TransferBuffer = pVendorRequest;

        }
        else
        {
            urb->UrbControlVendorClassRequest.TransferFlags = 0;
            buffer = ExAllocatePool(NonPagedPool, pVendorRequest->wLength);

            urb->UrbControlVendorClassRequest.TransferBuffer = buffer;

            if (pVendorRequest->wLength == 1)
            {
                buffer[0] = pVendorRequest->bData;
            }
            else
            {
                int i;
                PUCHAR ptr = buffer;

                for (i=0; i < pVendorRequest->wLength; i++)
                {
                    *ptr = (UCHAR) i;
                    ptr++;
                }
            }
        }

        ntStatus = Ezusb_CallUSBD(fdo, urb);

        //
        // only return a length if this was an IN transaction
        //
        if (pVendorRequest->direction)
        {
            length = urb->UrbControlVendorClassRequest.TransferBufferLength;
        }
        else
        {
            length = 0;
        }

        ExFreePool(urb);
        if (buffer)
            ExFreePool(buffer);
    }
#else
    UNREFERENCED_PARAMETER(device);
    UNREFERENCED_PARAMETER(pVendorRequest);
    UNREFERENCED_PARAMETER(bufferLength);
    UNREFERENCED_PARAMETER(outbufferLength);
#endif
    return ntStatus;
}

NTSTATUS MWLDXP50USBDriver_ResetPipe(
		_In_ device,
		_In_ WDFRequest Request, 
		_In_ ULONG pipeNum)

{   NTSTATUS status = STATUS_INVALID_STATE;
    PDEVICE_CONTEXT    pDevContext = GetDeviceContext(device);

    WDFUSBPIPE pipe = WdfUsbInterfaceGetConfiguredPipe(pDevContext->UsbInterface, (BYTE) pipeNum, NULL);

    MWLUsb_DbgPrint(1, ("Reset pipe %d, pipehandle %x: \n", pipeNum, pipe));
    if (pipe == NULL) {
        MWLUsb_DbgPrint(1, ("pipe handle for pipenum %d is NULL\n", pipeNum));
        status = STATUS_INVALID_PARAMETER;
    }
    status = WdfUsbTargetPipeResetSynchronously(pipe, Request, NULL);
    if (!NT_SUCCESS(status)) {
        MWLUsb_DbgPrint(1, ("Pipe Reset synchronous send failed requset status = 0x%x\n", status));
    }
    return status;
}
    

NTSTATUS
MWLDPX50USBUMDF2Driver_AbortPipe(
		_In_ device,
		_In_ WDFRequest Request, 
		_In_ ULONG pipeNum)
/*++

Routine Description:

   cancel pending transfers for a pipe

Arguments:

Return Value:


--*/
{
   NTSTATUS ntStatus = STATUS_SUCCESS;

   //PURB urb;
   //USBD_VERSION_INFORMATION VersionInformation;

{   NTSTATUS status = STATUS_INVALID_STATE;
    PDEVICE_CONTEXT    pDevContext = GetDeviceContext(device);

    WDFUSBPIPE pipe = WdfUsbInterfaceGetConfiguredPipe(pDevContext->UsbInterface, (BYTE) pipeNum, NULL);

    MWLUsb_DbgPrint(1, ("Abort pipe %d, pipehandle %x: \n", pipeNum, pipe));
    if (pipe == NULL) {
        MWLUsb_DbgPrint(1, ("pipe handle for pipenum %d is NULL\n", pipeNum));
        status = STATUS_INVALID_PARAMETER;
    }
    status = WdfUsbTargetPipeAbortSynchronously(pipe, Request, NULL);
    if (!NT_SUCCESS(status)) {
        MWLUsb_DbgPrint(1, ("Pipe Reset synchronous send failed requset status = 0x%x\n", status));
    }
    return status;
}



NTSTATUS
Ezusb_ResetDevice(
    _In_ WDFDEVICE device
    )
/*++

Routine Description:

    Reset the our parent port

Arguments:

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_CONTEXT pDeviceContext;
     
    PAGED_CODE();
  
    TraceEvents(TRACE_LEVEL_INFORMATION, DBG_IOCTL, "--> ResetDevice\n");
 
    pDeviceContext = GetDeviceContext(Device);
 
    //
    // A NULL timeout indicates an infinite wake
    //
    status = WdfWaitLockAcquire(pDeviceContext->ResetDeviceWaitLock, NULL);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "ResetDevice - could not acquire lock\n");
        return status;
    }
 
    StopAllPipes(pDeviceContext);
     
    status = WdfUsbTargetDeviceResetPortSynchronously(pDeviceContext->UsbDevice);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, DBG_IOCTL, "ResetDevice failed - 0x%x\n", status);
    }
     
    status = StartAllPipes(pDeviceContext);
    if (!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, WDF_DRIVER, "Failed to start all pipes - 0x%x\n", status);
    }
     
    WdfWaitLockRelease(pDeviceContext->ResetDeviceWaitLock);
 
    MWLUsb_DbgPrint(1, ("EZUSB.SYS: enter Ezusb_ResetPort\n"));

    return ntStatus;
}


PUSB_CONFIGURATION_DESCRIPTOR
GetConfigDescriptor(
    _In_ WDFDEVICE device
    )
/*++

Routine Description:

Arguments:



Return Value:

    NT status code

--*/
{
    
    NTSTATUS ntStatus;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = NULL;

    MWLUsb_DbgPrint(1, ("Ezusb.SYS: enter Ezusb_GetConfigDescriptor\n"));
    UNREFERENCED_PARAMETER(device);
    UNREFERENCED_PARAMETER(ntStatus);
#if 0
    pdx = fdo->DeviceExtension;

    urb = ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if (urb) {


        siz = sizeof(USB_CONFIGURATION_DESCRIPTOR)+256;

get_config_descriptor_retry2:

        configurationDescriptor = ExAllocatePool(NonPagedPool,
                                                 siz);

        if (configurationDescriptor) {

            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         configurationDescriptor,
                                         NULL,
                                         siz,
                                         NULL);

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            Ezusb_KdPrint ("Ezusb.SYS: Configuration Descriptor = %x, len %x\n",
                            configurationDescriptor,
                            urb->UrbControlDescriptorRequest.TransferBufferLength);
        } else {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        }

        if (NT_SUCCESS(ntStatus) &&
            (urb->UrbControlDescriptorRequest.TransferBufferLength >= 
             sizeof(USB_CONFIGURATION_DESCRIPTOR)) &&
            (configurationDescriptor->wTotalLength >=
             sizeof(USB_CONFIGURATION_DESCRIPTOR)))
        {
            //
            // The Get Config Descriptor request did not return an error
            // AND at least enough data was transferred to fill a Config
            // Descriptor AND the Config Descriptor wLength is at least the
            // size of a Config Descriptor
            //
            if (configurationDescriptor->wTotalLength > siz)
            {
                //
                // The request buffer is not big enough to hold the
                // entire set of descriptors.  Free the current buffer
                // and retry with a buffer which should be big enough.
                //
                siz = configurationDescriptor->wTotalLength;
                ExFreePool(configurationDescriptor);
                configurationDescriptor = NULL;
                goto get_config_descriptor_retry2;
            }
            else if (configurationDescriptor->wTotalLength >
                     urb->UrbControlDescriptorRequest.TransferBufferLength)
            {
                //
                // The request buffer is greater than or equal to the
                // Config Descriptor wLength, but less data was transferred
                // than wLength.  Return NULL to indicate a device error.
                //
                ExFreePool(configurationDescriptor);
                configurationDescriptor = NULL;
            }
            //
            // else  everything is OK with the Config Descriptor, return it.
            //
        }
        else
        {
            //
            // The Get Config Descriptor request returned an error OR
            // not enough data was transferred to fill a Config Descriptor
            // OR the Config Descriptor wLength is less than the size of
            // a Config Descriptor.  Return NULL to indicate a device error.
            //
            ExFreePool(configurationDescriptor);
            configurationDescriptor = NULL;
        }

        ExFreePool(urb);

    } else {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    Ezusb_KdPrint ("Ezusb.SYS: exit Ezusb_GetConfigDescriptor\n");
#endif
    return configurationDescriptor;
}


#if 0
NTSTATUS
ConfigureDevice(
    _In_ WDFDEVICE device
    )
/*++
Routine Description:
    Configures the USB device via USB-specific device requests and interaction
    with the USB software subsystem.

Arguments:
    fdo - pointer to the device object for this instance of the Ezusb Device

Return Value:
    NT status code
--*/
{
  
   NTSTATUS ntStatus = STATUS_SUCCESS;
  
   UNREFERENCED_PARAMETER(device);
   MWLUsb_DbgPrint(1, ("enter ConfigureDevice\n"));

   //
   // Get the configuration Descriptor
   //
   configurationDescriptor = GetConfigDescriptor(fdo);

   if (!configurationDescriptor)
   {
      Ezusb_KdPrint ("Get Configuration Descriptor Failed\n");
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupConfigureDevice;
   }

#define INTERFACENUMBER 0
#define ALTERNATESETTING 1
   //
   // Get the interface Descriptor for the interface we want
   //
   interfaceDescriptor = USBD_ParseConfigurationDescriptorEx(
                           configurationDescriptor,
                           configurationDescriptor,
                           INTERFACENUMBER,
                           ALTERNATESETTING,
                           -1,
                           -1,
                           -1);
   if (!interfaceDescriptor)
   {
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupConfigureDevice;
   }

   numberOfPipes = interfaceDescriptor->bNumEndpoints;
   
   //
   // Configure the Device, but don't select any interfaces
   //
   urb = USBD_CreateConfigurationRequestEx(configurationDescriptor, NULL);

   if (!urb)
   {
      Ezusb_KdPrint ("USBD_CreateConfigurationRequestEx Failed\n");
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupConfigureDevice;
   }

   ntStatus = Ezusb_CallUSBD(fdo, urb);

   if (NT_SUCCESS(ntStatus))
   {
      pdx->ConfigurationHandle = urb->UrbSelectConfiguration.ConfigurationHandle;
   }
   else
   {
      Ezusb_KdPrint ("Configuration Request Failed\n");
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupConfigureDevice;
   }

CleanupConfigureDevice:

   // Clean up and exit this routine
   if (urb != NULL)
   {
      ExFreePool(urb);
   }

   if (configurationDescriptor != NULL)
   {
      ExFreePool(configurationDescriptor);
   }

    Ezusb_KdPrint ("exit Ezusb_ConfigureDevice (%x)\n", ntStatus);

    return ntStatus;
}
#endif

NTSTATUS
SetInterface(
   _In_ WDFDEVICE device,
   _In_ UCHAR InterfaceNumber,
   _In_ UCHAR AlternateSetting
   )
{

   NTSTATUS ntStatus = STATUS_SUCCESS;

   UNREFERENCED_PARAMETER(device);
   UNREFERENCED_PARAMETER(InterfaceNumber);
   UNREFERENCED_PARAMETER(AlternateSetting);

#if 0
   //
   // Get the configuration Descriptor
   //
   configurationDescriptor = GetConfigDescriptor(fdo);
   if (!configurationDescriptor)
   {
      Ezusb_KdPrint ("Get Configuration Descriptor Failed\n");
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupSetInterface;
   }

   //
   // Get the interface Descriptor for the interface we want
   //
   interfaceDescriptor = USBD_ParseConfigurationDescriptorEx(
                           configurationDescriptor,
                           configurationDescriptor,
                           InterfaceNumber,
                           AlternateSetting,
                           -1,
                           -1,
                           -1);

   if (!interfaceDescriptor)
   {
      ntStatus = STATUS_UNSUCCESSFUL;
      goto CleanupSetInterface;
   }

   numberOfPipes = interfaceDescriptor->bNumEndpoints;
   Ezusb_KdPrint ("numberOfPipes = %d\n", numberOfPipes);


   urbSize = (USHORT)GET_SELECT_INTERFACE_REQUEST_SIZE(numberOfPipes);
   Ezusb_KdPrint ("urbSize = %d\n", urbSize);
   urb = ExAllocatePool(NonPagedPool,urbSize);

   if (!urb)
   {
      ntStatus = STATUS_NO_MEMORY;
      goto CleanupSetInterface;
   }
   
   RtlZeroMemory(urb,urbSize);

   UsbBuildSelectInterfaceRequest(urb,
                                  urbSize,
                                  pdx->ConfigurationHandle,
                                  InterfaceNumber,
                                  AlternateSetting);


   interfaceInformation = &(urb->UrbSelectInterface.Interface);
   interfaceInformation->Length = (USHORT)GET_USBD_INTERFACE_SIZE(numberOfPipes);

   for (i = 0 ;i < numberOfPipes ;i++ )
   {
      interfaceInformation->Pipes[i].MaximumTransferSize = (64*1024) -1;
   }

   ntStatus = Ezusb_CallUSBD(fdo, urb);

   //
   // If that succeeded, then update the Interface structure
   // in the device extension.
   //
   if (NT_SUCCESS(ntStatus))
   {
      for (j=0; j<interfaceInformation->NumberOfPipes; j++)
      {
         PUSBD_PIPE_INFORMATION pipeInformation;

         pipeInformation = &interfaceInformation->Pipes[j];

         Ezusb_KdPrint ("---------\n");
         Ezusb_KdPrint ("PipeType 0x%x\n", pipeInformation->PipeType);
         Ezusb_KdPrint ("EndpointAddress 0x%x\n", pipeInformation->EndpointAddress);
         Ezusb_KdPrint ("MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize);
         Ezusb_KdPrint ("Interval 0x%x\n", pipeInformation->Interval);
         Ezusb_KdPrint ("Handle 0x%x\n", pipeInformation->PipeHandle);
         Ezusb_KdPrint ("MaximumTransferSize 0x%x\n", pipeInformation->MaximumTransferSize);
      }

      if (pdx->Interface)
      {
         ExFreePool(pdx->Interface);
      }

      pdx->Interface = NULL;
      pdx->Interface = ExAllocatePool(NonPagedPool,interfaceInformation->Length);
      if (!pdx->Interface)
      {
         ntStatus = STATUS_NO_MEMORY;
         goto CleanupSetInterface;
      }
      RtlCopyMemory(pdx->Interface, interfaceInformation, interfaceInformation->Length);
   }

CleanupSetInterface:

   // Clean up and exit this routine
   if (urb != NULL)
   {
      ExFreePool(urb);
   }

   if (configurationDescriptor != NULL)
   {
      ExFreePool(configurationDescriptor);
   }
#endif
   return(ntStatus);
   
}   

#define BYTES_PER_LINE 0x10


NTSTATUS Ezusb_8051Reset(
   _In_ WDFDEVICE device,
   _In_ UCHAR resetBit
   )
/*++

Routine Description:
   Uses the ANCHOR LOAD vendor specific command to either set or release the
   8051 reset bit in the EZ-USB chip.

Arguments:
   fdo - pointer to the device object for this instance of an Ezusb Device
   resetBit - 1 sets the 8051 reset bit (holds the 8051 in reset)
              0 clears the 8051 reset bit (8051 starts running)
              
Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise

--*/
{
   NTSTATUS ntStatus = STATUS_SUCCESS;
   UNREFERENCED_PARAMETER(device);
   UNREFERENCED_PARAMETER(resetBit);

#if 0
   PURB urb = NULL;

   urb = ExAllocatePool(NonPagedPool, 
                       sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

   if (urb)
   {

      // toggle the EZ-USB reset bit (harmless on FX2)
      RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

      urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

      urb->UrbControlVendorClassRequest.TransferBufferLength = 1;
      urb->UrbControlVendorClassRequest.TransferBuffer = &resetBit;
      urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
      urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_INTERNAL;
      urb->UrbControlVendorClassRequest.Value = CPUCS_REG_EZUSB;
      urb->UrbControlVendorClassRequest.Index = 0;

      ntStatus = Ezusb_CallUSBD(fdo, urb);

      // toggle the FX2 reset bit (harmless on EZ-USB)
      RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

      urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

      urb->UrbControlVendorClassRequest.TransferBufferLength = 1;
      urb->UrbControlVendorClassRequest.TransferBuffer = &resetBit;
      urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
      urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_INTERNAL;
      urb->UrbControlVendorClassRequest.Value = CPUCS_REG_FX2;
      urb->UrbControlVendorClassRequest.Index = 0;

      ntStatus = Ezusb_CallUSBD(fdo, urb);
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

//
// this is the number of bytes of firmware to download per setup transfer.
// 
#define CHUNK_SIZE 64

NTSTATUS Ezusb_AnchorDownload(
   _In_ WDFDEVICE device,
   _In_ WORD offset,
   _In_ PUCHAR downloadBuffer,
   _In_ ULONG downloadSize
   )
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
   UNREFERENCED_PARAMETER(device);
   UNREFERENCED_PARAMETER(offset);
   UNREFERENCED_PARAMETER(downloadBuffer);
       UNREFERENCED_PARAMETER(downloadSize);
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

NTSTATUS Ezusb_DownloadIntelHex(
   _In_ WDFDEVICE device,
   _In_ PINTEL_HEX_RECORD hexRecord
   )
/*++

Routine Description:
   This function downloads Intel Hex Records to the EZ-USB device.  If any of the hex records
   are destined for external RAM, then the caller must have previously downloaded firmware
   to the device that knows how to download to external RAM (ie. firmware that implements
   the ANCHOR_LOAD_EXTERNAL vendor specific command).

Arguments:
   fdo - pointer to the device object for this instance of an Ezusb Device
   hexRecord - pointer to an array of INTEL_HEX_RECORD structures.  This array
               is terminated by an Intel Hex End record (Type = 1).

Return Value:
   STATUS_SUCCESS if successful,
   STATUS_UNSUCCESSFUL otherwise

--*/
{
   NTSTATUS ntStatus = STATUS_SUCCESS;

   UNREFERENCED_PARAMETER(device);
   UNREFERENCED_PARAMETER(hexRecord);
   UNREFERENCED_PARAMETER(ntStatus);
#if 0
   PURB urb = NULL;
   PINTEL_HEX_RECORD ptr = hexRecord;

   urb = ExAllocatePool(NonPagedPool, 
                       sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

   if (urb)
   {
      //
      // The download must be performed in two passes.  The first pass loads all of the
      // external addresses, and the 2nd pass loads to all of the internal addresses.
      // why?  because downloading to the internal addresses will probably wipe out the firmware
      // running on the device that knows how to receive external ram downloads.
      //
      //
      // First download all the records that go in external ram
      //
      while (ptr->Type == 0)
      {
         if (!INTERNAL_RAM(ptr->Address))
         {
            RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

            urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
            urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;
            urb->UrbControlVendorClassRequest.TransferBufferLength = ptr->Length;
            urb->UrbControlVendorClassRequest.TransferBuffer = ptr->Data;
            urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_EXTERNAL;
            urb->UrbControlVendorClassRequest.Value = ptr->Address;
            urb->UrbControlVendorClassRequest.Index = 0;

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            if (!NT_SUCCESS(ntStatus))
               break;
         }
         ptr++;
      }

      //
      // Now download all of the records that are in internal RAM.  Before starting
      // the download, stop the 8051.
      //
      Ezusb_8051Reset(fdo,1);
      ptr = hexRecord;
      while (ptr->Type == 0)
      {
         if (INTERNAL_RAM(ptr->Address))
         {
            RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

            urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
            urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;
            urb->UrbControlVendorClassRequest.TransferBufferLength = ptr->Length;
            urb->UrbControlVendorClassRequest.TransferBuffer = ptr->Data;
            urb->UrbControlVendorClassRequest.Request = ANCHOR_LOAD_INTERNAL;
            urb->UrbControlVendorClassRequest.Value = ptr->Address;
            urb->UrbControlVendorClassRequest.Index = 0;

            ntStatus = Ezusb_CallUSBD(fdo, urb);

            if (!NT_SUCCESS(ntStatus))
               break;
         }
         ptr++;
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
#if 0
PRING_BUFFER
AllocRingBuffer(
   SIZE_T    Size
   )
{
   PRING_BUFFER   ringBuffer = NULL;

   ringBuffer = ExAllocatePool(NonPagedPool, sizeof(RING_BUFFER));

   if (!ringBuffer)
      return NULL;

   ringBuffer->buffer = ExAllocatePool(NonPagedPool, Size);

   if (!ringBuffer->buffer)
   {
      ExFreePool(ringBuffer);
      return NULL;
   }

   ringBuffer->inPtr = ringBuffer->buffer;
   ringBuffer->outPtr = ringBuffer->buffer;
   ringBuffer->totalSize = Size;
   ringBuffer->currentSize = 0;

   KeInitializeSpinLock(&ringBuffer->spinLock);

   return ringBuffer;
}

VOID
FreeRingBuffer(
   PRING_BUFFER   ringBuffer
   )
{
   ExFreePool(ringBuffer->buffer);
   ExFreePool(ringBuffer);
}

SIZE_T
ReadRingBuffer(
   PRING_BUFFER   ringBuffer,
   PUCHAR         readBuffer,
   SIZE_T         numberOfBytesToRead
   )
/*
   Routine Description:
   This routine reads data from a ring buffer.

   Arguments:
   ringBuffer - pointer to a ring buffer structure
   readBuffer - pointer to a user supplied buffer to transfer data into
   numberOfBytesToRead - number of bytes to read from the ring buffer

   Return Value:
   ULONG - number of bytes read.  May be smaller than requested number of bytes.
*/
{
   SIZE_T   byteCount;
   KIRQL    oldIrql;

   Ezusb_KdPrint( "ReadRingBuffer() enter\n");

   if (numberOfBytesToRead > ringBuffer->totalSize)
      return 0;

   if (ringBuffer->currentSize == 0)
      return 0;

   if ( numberOfBytesToRead > ringBuffer->currentSize )
      byteCount = ringBuffer->currentSize;
   else
      byteCount = numberOfBytesToRead;

   //
   // two cases.  Read either wraps or it doesn't.
   // Handle the non-wrapped case first
   //
   if ((ringBuffer->outPtr + byteCount - 1) < 
       (ringBuffer->buffer + ringBuffer->totalSize))
   {
      Ezusb_KdPrint( "ReadRingBuffer() about to copy a\n");
      RtlCopyMemory(readBuffer, ringBuffer->outPtr, byteCount);
      ringBuffer->outPtr += byteCount;
      if (ringBuffer->outPtr == ringBuffer->buffer + ringBuffer->totalSize)
         ringBuffer->outPtr = ringBuffer->buffer;
   }
   // now handle the wrapped case
   else
   {
      SIZE_T    fragSize;

      Ezusb_KdPrint( "ReadRingBuffer() about to copy b\n");
      // get the first half of the read
      fragSize = ringBuffer->buffer + ringBuffer->totalSize - ringBuffer->outPtr;
      RtlCopyMemory(readBuffer, ringBuffer->outPtr, (SIZE_T)fragSize);

      // now get the rest
      RtlCopyMemory(readBuffer + fragSize, ringBuffer->buffer, byteCount - fragSize);

      ringBuffer->outPtr = ringBuffer->buffer + byteCount - fragSize;
   }
 
   // 
   // update the current size of the ring buffer.  Use spinlock to insure
   // atomic operation.
   //
   KeAcquireSpinLock(&ringBuffer->spinLock, &oldIrql);
   ringBuffer->currentSize -= byteCount;
   KeReleaseSpinLock(&ringBuffer->spinLock, oldIrql);

   Ezusb_KdPrint( "ReadRingBuffer() exit\n");

   return byteCount;
}

SIZE_T
WriteRingBuffer(
   PRING_BUFFER   ringBuffer,
   PUCHAR         writeBuffer,
   SIZE_T          numberOfBytesToWrite
   )
/*
   Routine Description:
   This routine writes data to a ring buffer.  If the requested write size exceeds
   available space in the ring buffer, then the write is rejected.

   Arguments:
   ringBuffer - pointer to a ring buffer structure
   readBuffer - pointer to a user supplied buffer of data to copy to the ring buffer
   numberOfBytesToRead - number of bytes to write to the ring buffer

   Return Value:
   ULONG - number of bytes written.
*/
{
   SIZE_T    byteCount;
   KIRQL    oldIrql;

   if ( numberOfBytesToWrite >
        (ringBuffer->totalSize - ringBuffer->currentSize) )
   {
      Ezusb_KdPrint( "WriteRingBuffer() OVERFLOW\n");
      return 0;
   }

   byteCount = numberOfBytesToWrite;

   //
   // two cases.  Write either wraps or it doesn't.
   // Handle the non-wrapped case first
   //
   if ((ringBuffer->inPtr + byteCount - 1) < 
       (ringBuffer->buffer + ringBuffer->totalSize))
   {
      RtlCopyMemory(ringBuffer->inPtr, writeBuffer, byteCount);
      ringBuffer->inPtr += byteCount;
      if (ringBuffer->inPtr == ringBuffer->buffer + ringBuffer->totalSize)
         ringBuffer->inPtr = ringBuffer->buffer;
   }
   // now handle the wrapped case
   else
   {
      SIZE_T   fragSize;

      // write the first fragment
      fragSize = ringBuffer->buffer + ringBuffer->totalSize - ringBuffer->inPtr;
      RtlCopyMemory(ringBuffer->inPtr, writeBuffer, fragSize);

      // now write the rest
      RtlCopyMemory(ringBuffer->buffer, writeBuffer + fragSize, byteCount - fragSize);

      ringBuffer->inPtr = ringBuffer->buffer + byteCount - fragSize;
   }

   // 
   // update the current size of the ring buffer.  Use spinlock to insure
   // atomic operation.
   //
   KeAcquireSpinLock(&ringBuffer->spinLock, &oldIrql);
   ringBuffer->currentSize += byteCount;
   KeReleaseSpinLock(&ringBuffer->spinLock, oldIrql);

   return byteCount;
}

#endif
BOOLEAN IsFx2(
   _In_ WDFDEVICE device
   )
/*
   Routine Description:
   This routine queries the device via a SETUP command to determine if it is
   an FX2 chip.  In order for this to work, the device must implement the vendor
   specific IN SETUP command 0xAC, which returns 1 if the chip is FX2 and 0
   otherwise.

   Arguments:
   fdo - our device object

   Return Value:
   BOOL - TRUE if FX2, FALSE otherwise
*/
{
   NTSTATUS ntStatus = STATUS_SUCCESS;

   UNREFERENCED_PARAMETER(device);
   UNREFERENCED_PARAMETER(ntStatus);

#if 0
   PURB urb = NULL;
   UCHAR transferBuffer[64];

   urb = ExAllocatePool(NonPagedPool, 
                       sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

   if (urb)
   {
      RtlZeroMemory(urb,sizeof(struct  _URB_CONTROL_VENDOR_OR_CLASS_REQUEST));

      urb->UrbHeader.Length = sizeof(struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST);
      urb->UrbHeader.Function = URB_FUNCTION_VENDOR_DEVICE;

      urb->UrbControlVendorClassRequest.TransferFlags = USBD_TRANSFER_DIRECTION_IN ;
      urb->UrbControlVendorClassRequest.TransferBufferLength = 1;
      urb->UrbControlVendorClassRequest.TransferBuffer = transferBuffer;
      urb->UrbControlVendorClassRequest.TransferBufferMDL = NULL;
      urb->UrbControlVendorClassRequest.Request = ANCHOR_ISFX2;
      urb->UrbControlVendorClassRequest.Value = 0;
      urb->UrbControlVendorClassRequest.Index = 0;

      Ezusb_KdPrint ("**** About to query ISFX2\n");

      ntStatus = Ezusb_CallUSBD(fdo, urb);
   }
   else
   {
      ntStatus = STATUS_NO_MEMORY;
   }

   if (urb)
      ExFreePool(urb);

   if (transferBuffer[0] == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
#else
   return TRUE;
#endif
}


NTSTATUS Ezusb_SetFeature(
   _In_ WDFDEVICE device,
   _In_ PSET_FEATURE_CONTROL setFeatureControl
   )
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
   NTSTATUS            ntStatus        = STATUS_SUCCESS;
  
   UNREFERENCED_PARAMETER(device);
   UNREFERENCED_PARAMETER(setFeatureControl);
   MWLUsb_DbgPrint(1, ("Enter Ezusb_SetFeature\n"));    

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



NTSTATUS
Ezusb_Read_Write(
   IN  WDFDEVICE device,
   IN  WDFREQUEST request
   )
/*++
Routine Description:
    
Arguments:

Return Value:
    NT status code
        STATUS_SUCCESS:                 Read was done successfully
        STATUS_INVALID_PARAMETER_3:     The Endpoint Index does not specify an IN pipe 
        STATUS_NO_MEMORY:               Insufficient data memory was supplied to perform the READ

    This routine fills the status code into the Irp
    
--*/
{
    NTSTATUS                   ntStatus = STATUS_UNSUCCESSFUL;
    UNREFERENCED_PARAMETER(device);
    UNREFERENCED_PARAMETER(request);
#if 0
   PBULK_TRANSFER_CONTROL     bulkControl =
                              (PBULK_TRANSFER_CONTROL)Irp->AssociatedIrp.SystemBuffer;
   ULONG                      bufferLength =
                              irpStack->Parameters.DeviceIoControl.OutputBufferLength;
   PURB                       urb = NULL;
   ULONG                      urbSize = 0;
   ULONG                      transferFlags = 0;
   PUSBD_INTERFACE_INFORMATION interfaceInfo = NULL;
   PUSBD_PIPE_INFORMATION     pipeInfo = NULL;
   USBD_PIPE_HANDLE           pipeHandle = NULL;


   Ezusb_KdPrint("enter Ezusb_Read_Write()\n");
   
   //
   // verify that the selected pipe is valid, and get a handle to it. If anything
   // is wrong, return an error
   //
   interfaceInfo = pdx->Interface;

   if (!interfaceInfo)
   {
      Ezusb_KdPrint("Ezusb_Read_Write() no interface info - Exiting\n");
      return STATUS_UNSUCCESSFUL;
   }
   
   if (bulkControl->pipeNum > interfaceInfo->NumberOfPipes)
   {
      Ezusb_KdPrint("Ezusb_Read_Write() invalid pipe - Exiting\n");
      return STATUS_INVALID_PARAMETER;
   }

   pipeInfo = &(interfaceInfo->Pipes[bulkControl->pipeNum]);

   if (!((pipeInfo->PipeType == UsbdPipeTypeBulk) ||
         (pipeInfo->PipeType == UsbdPipeTypeInterrupt)))
   {
      Ezusb_KdPrint("Ezusb_Read_Write() invalid pipe - Exiting\n");
      return STATUS_INVALID_PARAMETER;
   }

   pipeHandle = pipeInfo->PipeHandle;

   if (!pipeHandle)
   {
      Ezusb_KdPrint("Ezusb_Read_Write() invalid pipe - Exiting\n");
      return STATUS_UNSUCCESSFUL;
   }

   if (bufferLength > pipeInfo->MaximumTransferSize)
   {
      Ezusb_KdPrint("Ezusb_Read_Write() invalid transfer size - Exiting\n");
      return STATUS_INVALID_PARAMETER;
   }

   //
   // allocate and fill in the Usb request (URB)
   //
   urbSize = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);

   urb = ExAllocatePool(NonPagedPool,urbSize);

   if (!urb)
   {
      Ezusb_KdPrint("Ezusb_Read_Write() unable to alloc URB - Exiting\n");
      return STATUS_NO_MEMORY;
   }
   

   transferFlags = USBD_SHORT_TRANSFER_OK;

   //
   // get direction info from the endpoint address
   //
   if (USB_ENDPOINT_DIRECTION_IN(pipeInfo->EndpointAddress))
      transferFlags |= USBD_TRANSFER_DIRECTION_IN;

   UsbBuildInterruptOrBulkTransferRequest(urb,        //ptr to urb
                        (USHORT) urbSize,             //size of urb
                               pipeHandle,                   //usbd pipe handle
                               NULL,                         //TransferBuffer
                               Irp->MdlAddress,              //mdl
                               bufferLength,                 //bufferlength
                               transferFlags,                //flags
                               NULL);                        //link

   //
   // Call the USB Stack.
   //
    ntStatus = Ezusb_CallUSBD(fdo, urb);

   //
   // If the transfer was successful, report the length of the transfer to the
   // caller by setting IoStatus.Information
   //
   if (NT_SUCCESS(ntStatus))
   {
      Irp->IoStatus.Information = urb->UrbBulkOrInterruptTransfer.TransferBufferLength;
      Ezusb_KdPrint("Successfully transfered 0x%x bytes\n",Irp->IoStatus.Information);
   }

   //
   // free the URB
   //
   ExFreePool(urb);
#endif
   return ntStatus;
}


#if 0
void InitLog()
{
    IO_STATUS_BLOCK        ioStatus;
    OBJECT_ATTRIBUTES    objectAttributes;
    UNICODE_STRING      unicodeName;
    NTSTATUS            ntStatus;

    RtlInitUnicodeString( &unicodeName, DEFAULT_LOG_FILE_NAME);
    InitializeObjectAttributes( &objectAttributes, 
        &unicodeName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL);
    
    ntStatus = ZwCreateFile( &m_hLogFile, 
        GENERIC_WRITE | SYNCHRONIZE | FILE_APPEND_DATA, 
        &objectAttributes,
        &ioStatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_WRITE,
        FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0);
}

void LogMessage(PCHAR szFormat, ...)
{
    ULONG Length;
    CHAR messagebuf[256];
    va_list va;
    IO_STATUS_BLOCK  IoStatus;
    // OBJECT_ATTRIBUTES objectAttributes;
    // NTSTATUS status;
    
    //format the string
    va_start(va,szFormat);
    vsprintf(messagebuf,szFormat,va);
    va_end(va);
    
    Length=(ULONG)strlen(messagebuf);
    if(messagebuf[Length-1]=='\n')
    {
        messagebuf[Length-1]='\r';
        strcat(messagebuf,"\n");
        Length++;
    }
    else
    {
        strcat(messagebuf,"\r\n");
        Length+=2;
    }

    ZwWriteFile(
        m_hLogFile,
        NULL,
        NULL,
        NULL,
        &IoStatus,
        messagebuf,
        Length,
        NULL,
        NULL
        );
    
}

void CloseLog()
{
    ZwClose(m_hLogFile);
}

#endif


