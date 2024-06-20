import win32file
import struct
import libusb

class Test_Context:
    def CTL_CODE(self, DeviceType, Function, Method, Access ):
        # define CTL_CODE(
        #      DeviceType, Function, Method, Access ) (((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) )
        # from ntddk.h
        return ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) 


    def __init__(self):
        self.LANGID_ENGLISH = 0x409
        self.FILE_DEVICE_UNKNOWN = 0x22
        self.METHOD_BUFFERED = 0
        self.FILE_ANY_ACCESS = 0
        self.EZUSB_IOCTL_INDEX =  0x0800
        self.EZUSB_IOCTL_GET_DEVICE_DESCRIPTOR = self.CTL_CODE(self.FILE_DEVICE_UNKNOWN, self.EZUSB_IOCTL_INDEX+1, self.METHOD_BUFFERED, self.FILE_ANY_ACCESS)
        self.EZUSB_IOCTL_GET_STRING_DESCRIPTOR = self.CTL_CODE(self.FILE_DEVICE_UNKNOWN, self.EZUSB_IOCTL_INDEX+17, self.METHOD_BUFFERED, self.FILE_ANY_ACCESS)
        self.EZUSB_IOCTL_BULK_READ = self.CTL_CODE(self.FILE_DEVICE_UNKNOWN, self.EZUSB_IOCTL_INDEX+19, self.METHOD_BUFFERED, self.FILE_ANY_ACCESS)
        self.EZUSB_IOCTL_BULK_WRITE = self.CTL_CODE(self.FILE_DEVICE_UNKNOWN, self.EZUSB_IOCTL_INDEX+20, self.METHOD_BUFFERED, self.FILE_ANY_ACCESS)
        self.DXP50Registers = [0x08,  0x43,  0x44,  0x45, 0x46, 0x47, 0x50,  0x51,  0x52,  0x53, 0x54];
        self.DXP50TransferSize = [0,     0,     1,     1,    1,    1,    1,    1,      1,     1,    1]; # actually, data size - 1, but 1 is a short, 0 a byte
        self.CMD_SEND_PARAM = 0x6

    def unpack_device_descriptor(self, buffer):
        rtn_desc = struct.Struct("=BBHBBBBHHHBBBB")
        unpacked = rtn_desc.unpack_from(buffer)
        rslt = libusb.device_descriptor()
        rslt.bLength = unpacked[0]
        rslt.bDescriptorType = unpacked[1]
        rslt.bcdUSB = unpacked[2]
        rslt.bDeviceClass = unpacked[3]
        rslt.bDeviceSubClass = unpacked[4]
        rslt.bDeviceProtocol = unpacked[5]
        rslt.bMaxPacketSize0 = unpacked[6]
        rslt.idVendor = unpacked[7]
        rslt.idProduct = unpacked[8]
        rslt.bcdDevice = unpacked[9]
        rslt.iManufacturer = unpacked[10]
        rslt.iProduct = unpacked[11]
        rslt.iSerialNumber = unpacked[12]
        rslt.bNumConfigurations = unpacked[13]
        return rslt

    def pack_get_string_desc_ioctl( self, string_idx, lang_id, buffer):
        dev_ctl = struct.Struct("=BH")
        dev_ctl.pack_into(buffer, 0, string_idx, lang_id)

    def unpack_string_desc( self, buffer):
        string_len = len(buffer)
        if string_len < 2:
            return None
        string_len -= 2
        str_desc = struct.Struct( "=BB{}s".format(string_len))
        return str_desc.unpack_from(buffer)

    def pack_bulk_transfer( self, pipenum, buffer ):
        bt = struct.Struct("=L")
        bt.pack_into(buffer, 0, pipenum);

    def pack_send_param_command( self, regaddr, regsize, buffer ):
        bt = struct.Struct("=BH")
        header = (regaddr & 0xfff) | (regsize << 13)
        bt.pack_into(buffer, 0, self.CMD_SEND_PARAM, header);


test_context = Test_Context()

def TestFail(test,reason, got, expected):
    print("MWLUsbDriver: Test {} Failed: reason:{} got:{}, expected {}".format(test, reason, got, expected))
    return False

def TestPass(test):
    print("MWLUsbDriver: Test {} Passed".format(test))
    return False


def TestMWLUsb():

    print("Open the IIUSB driver")
    driver_handle = win32file.INVALID_HANDLE_VALUE
    try:
        driver_handle = win32file.CreateFile(r"\\.\IIUSB-0", (win32file.GENERIC_READ | win32file.GENERIC_WRITE),  \
                                     (win32file.FILE_SHARE_READ | win32file.FILE_SHARE_WRITE), None, win32file.OPEN_EXISTING, 0, None)
    except Exception as e:
        print(e)

    if ( driver_handle == win32file.INVALID_HANDLE_VALUE ):
        return TestFail("Create File", "Could not open", "Invalid Handle", "Valid Handle")
    else:
        TestPass("Create File")

    inbuffer = bytearray(65536)
    outbuffer = bytearray(65536)
    rtn_buffer = None
    try:
        rtn_buffer = win32file.DeviceIoControl(driver_handle, test_context.EZUSB_IOCTL_GET_DEVICE_DESCRIPTOR, inbuffer, outbuffer, None)
    except Exception as e:
        print(e)

    if ( rtn_buffer == None ):
        return TestFail("DevIOCOntrol EZUSB_GET_DEVICE_DESCRIPTOR", "null result", "None", "Valid Buffer")
    #
    # Unpack the descriptor then check the feilds. Vendor ID and Product ID are obvious
    dev_desc = test_context.unpack_device_descriptor(rtn_buffer)
    if (dev_desc.idVendor != 0x0C82) or (dev_desc.idProduct != 0x0010):
        return TestFail("DevIOCOntrol EZUSB_GET_DEVICE_DESCRIPTOR", "wrong vendor id", str(dev_desc.idVendor), "0x08C2")

    TestPass("DevIOCOntrol EZUSB_GET_DEVICE_DESCRIPTOR")

    # Now we get a string descriptor. Northstar gets string index 3, which is the serial number. We will get the right indices from the device desc
    inbuffer = bytearray(300)
    outbuffer = bytearray(300)
    test_context.pack_get_string_desc_ioctl(dev_desc.iManufacturer, test_context.LANGID_ENGLISH, inbuffer)
    try:
        rtn_buffer = win32file.DeviceIoControl(driver_handle, test_context.EZUSB_IOCTL_GET_STRING_DESCRIPTOR, inbuffer, outbuffer, None)
    except Exception as e:
        print(e)

    if ( rtn_buffer == None ):
        return TestFail("DevIOCOntrol EZUSB_GET_STRING_DESCRIPTOR", "null result", "None", "Valid Buffer")

    unpacked = test_context.unpack_string_desc(rtn_buffer)
    mfr_name = unpacked[2].decode('utf-16')
    print("Manufacturer = {}".format(mfr_name))
    
    inbuffer = bytearray(300)
    outbuffer = bytearray(300)
    test_context.pack_get_string_desc_ioctl(dev_desc.iProduct, test_context.LANGID_ENGLISH, inbuffer)
    try:
        rtn_buffer = win32file.DeviceIoControl(driver_handle, test_context.EZUSB_IOCTL_GET_STRING_DESCRIPTOR, inbuffer, outbuffer, None)
    except Exception as e:
        print(e)

    if ( rtn_buffer == None ):
        return TestFail("DevIOCOntrol EZUSB_GET_STRING_DESCRIPTOR Product Description", "null result", "None", "Valid Buffer")

    unpacked = test_context.unpack_string_desc(rtn_buffer)
    product_desc = unpacked[2].decode('utf-16')
    print("Product Desc = {}".format(product_desc))

    inbuffer = bytearray(300)
    outbuffer = bytearray(300)
    test_context.pack_get_string_desc_ioctl(dev_desc.iSerialNumber, test_context.LANGID_ENGLISH, inbuffer)
    try:
        rtn_buffer = win32file.DeviceIoControl(driver_handle, test_context.EZUSB_IOCTL_GET_STRING_DESCRIPTOR, inbuffer, outbuffer, None)
    except Exception as e:
        print(e)

    if ( rtn_buffer == None ):
        return TestFail("DevIOCOntrol EZUSB_GET_STRING_DESCRIPTOR Serial Number", "null result", "None", "Valid Buffer")

    unpacked = test_context.unpack_string_desc(rtn_buffer)
    product_desc = unpacked[2].decode('utf-16')
    print("Serial Number = {}".format(product_desc))
    TestPass("DevIOCOntrol EZUSB_GET_STRING_DESCRIPTOR")

    #====== Send commands to the board
    return True # Skip for now

    # For each parameter register, have the board send us the data item value. If the board is inited, we should get non zero.
    # since we cant be sure that the board is inited, we won't fail the test if we get zeros but will flag for investigation
    for i in range(len(test_context.DXP50Registers)):
        inbuffer = bytearray(65536)
        outbuffer = bytearray(65536)
        rtn_buffer = None
        test_context.pack_bulk_transfer(1, inbuffer) # 1 is the out pipe
        test_context.pack_send_param_command(test_context.DXP50Registers[i], test_context.DXP50TransferSize[i])
        try:
            rtn_buffer = win32file.DeviceIoControl(driver_handle, test_context.EZUSB_IOCTL_BULK_WRITE, inbuffer, outbuffer, None)
        except Exception as e:
            print(e)
            break;
    
        if ( rtn_buffer == None ):
            return TestFail("DevIOCOntrol EZUSB_BULK_WRITE", "null result", "None", "Valid Buffer")
    
        inbuffer = bytearray(65536)
        outbuffer = bytearray(65536)
        rtn_buffer = None
        test_context.pack_bulk_transfer(0, inbuffer) # 0 is the in pipe
        try:
            rtn_buffer = win32file.DeviceIoControl(driver_handle, test_context.EZUSB_IOCTL_BULK_WRITE, inbuffer, outbuffer, None)
        except Exception as e:
            print(e)
            break;
    
        if ( rtn_buffer == None ):
            return TestFail("DevIOCOntrol EZUSB_BULK_WRITE", "null result", "None", "Valid Buffer")

    TestPass("DevIOCOntrol EZUSB_BULK_READ/WRITE")

    return True
    
TestMWLUsb()


