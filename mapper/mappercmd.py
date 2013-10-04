
import usb.core
import time
import serial

class BraduinoUSBCommunicator:

    def __init__(self):
        print "Your UART cable better be plugged into the USB 2.0 port, or else kittens will cry"
        self.HELLO = 0
        self.SET_VALS = 1
        self.GET_VALS = 2
        self.PRINT_VALS = 3
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def hello(self):
        try:
            self.dev.ctrl_transfer(0x40, self.HELLO)
        except usb.core.USBError:
            print "Could not send HELLO vendor request."

    def set_vals(self, val1, val2):
        try:
            self.dev.ctrl_transfer(0x40, self.SET_VALS, int(val1), int(val2))
        except usb.core.USBError as e:
            print e
            print "Could not send SET_VALS vendor request."
            raise e

    def get_vals(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.GET_VALS, 0, 0, 4)
        except usb.core.USBError:
            print "Could not send GET_VALS vendor request."
        else:
            return [int(ret[0])+int(ret[1])*256, int(ret[2])+int(ret[3])*256]

    def print_vals(self):
        try:
            self.dev.ctrl_transfer(0x40, self.PRINT_VALS)
        except usb.core.USBError:
            print "Could not send PRINT_VALS vendor request."

    def move_servos(self,pos1,pos2):
        #do some data validation, then send the servo positions to the PIC
        if pos1 < 0 or pos1 > 1:
            raise ValueError,"pos1 must be between 0 and 1"
        elif pos2 < 0 or pos2 > 1:
            raise ValueError,"pos2 must be between 0 and 1"
        else:
            val1 = int(pos1 * (2**16-1))
            val2 = int(pos2 * (2**16-1))
            print "val1 = %d and val2=%d" % (val1,val2)
            print "in binary, val1 = %s and val2 = %s" % (bin(val1)[1:],bin(val2)[1:])
            self.set_vals(val1,val2)
            time.sleep(0.05) #don't overwhelm the PIC/servos

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyUSB0',19200,timeout=0)
    h = BraduinoUSBCommunicator()  
    #scanning code
    r = 200
    for i in range(1,r,2):
        for j in range(1,r):
            pos1 = float(i)/r
            pos2 = float(j)/r
            h.move_servos(pos1,pos2)
        for k in range(r,1,-1):
            pos1 = float(i+1)/r
            pos2 = float(k)/r
            h.move_servos(pos1,pos2)
