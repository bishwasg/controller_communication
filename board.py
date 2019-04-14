from time import sleep
from serial import Serial
import serial
from termcolor import colored
import inputs
import sys

def sendSignal(serial, data):
    """
    INPUT: string
    """
    serial.flush() # wait for all the data is written

    # reformat data
    data = "<" + data + ">"
    # encode data
    encoded_data = str.encode(data)
    # send data
    serial.write(encoded_data)
    
    print(colored("SENT:\t ", 'red'), data)
    
def recieveSignal(serial):
    serial.flush() # wait for all the data is written
    # Reading from arduino serial
    msg = serial.readline()
    print(colored("RECIEVED: ", 'blue'), msg)
    return msg

"""
    EVENT HANDELING
"""
def press_X(ser, ev_type, state):
    signal = 'X,0' if state == 0 else 'X,1'
    sendSignal(ser, signal)

def press_A(ser, ev_type, state):
    signal = 'A,0' if state == 0 else 'A,1'
    sendSignal(ser, signal)

def press_B(ser, ev_type, state):
    signal = 'B,0' if state == 0 else 'B,1'
    sendSignal(ser, signal)

def press_Y(ser, ev_type, state):
    signal = 'Y,0' if state == 0 else 'Y,1'
    sendSignal(ser, signal)

def press_RB(ser, ev_type, state):
    signal = 'RB,0' if state == 0 else 'RB,1'
    sendSignal(ser, signal)

event_dict = {'BTN_NORTH' : press_X,
              'BTN_SOUTH' : press_A, 
              'BTN_EAST'  : press_B, 
              'BTN_WEST'  : press_Y, 
              'BTN_TR'    : press_RB, 
            }

def event_loop(ser, events):
    for event in events:
        print(event.ev_type, event.code, event.state)
                
        call = event_dict.get(event.code)
        if callable(call):
            call(ser, event.ev_type, event.state)
            recieveSignal(ser)
"""
-------------
    MAIN
-------------
"""
def main():
    """
        SERIAL CONNECTION
    """
    # Cli arg for different serial port 
    # first arg is the serial port number
    if(len(sys.argv) < 2):
        serial_num = '0'
    else:
        serial_num = sys.argv[1]

    serial_port = '/dev/ttyACM' + serial_num
    
    # Connect to serial
    try:
        ser = serial.Serial(serial_port, 57600, timeout=60)
        print("Serial connected")
    except serial.serialutil.SerialException:
        raise Exception("Invalid Serial Port")

    """
        JOYSTICK CONNECTION
    """
    pads = inputs.devices.gamepads

    if len(pads) == 0:
        raise Exception("Couldn't find any Gamepads!")
    else:
        pad = pads[0]
    
    #sendSignal(ser, "howdy")
    #recieveSignal(ser) 
    try:
        while True:
            try:
                event_loop(ser, inputs.get_gamepad())
            except inputs.UnknownEventCode:
                pass
    except KeyboardInterrupt:
        print("BYEEE!")

   # # ---------------
    #for i in range(4):

    #    sendSignal(ser, 'X-1')
    #    recieveSignal(ser)

    #    sleep(2)

if __name__ == "__main__":
    main()
