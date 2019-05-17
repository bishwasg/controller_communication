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
    #serial.flush() # wait for all the data is written
    # Reading from arduino serial
    while(serial.in_waiting):
        msg = serial.readline()
        print(colored("RECIEVED: ", 'blue'), msg)
        return msg
    return 0

'''
    DATA PROCESSING
'''
def jst_normalize(data):
    # jst min: 2^5 128 max: 2^15 32758
    val = 0 if abs(data) < 150 else int((255/32736)*data)
    return val

"""
    EVENT HANDELING
"""
# Digital Signal
def press_X(ser, ev_type, state):
    signal = 'X,0' if state == 0 else 'X,1'
    sendSignal(ser, signal)

def press_A(ser, ev_type, state):
    signal = 'A,' + str(state)
    sendSignal(ser, signal)

def press_B(ser, ev_type, state):
    signal = 'B,' + str(state)
    sendSignal(ser, signal)

def press_Y(ser, ev_type, state):
    signal = 'Y,' + str(state)
    sendSignal(ser, signal)

def press_RB(ser, ev_type, state):
    signal = 'RB,' + str(state)
    sendSignal(ser, signal)

def press_LB(ser, ev_type, state):
    signal = 'LB,' + str(state)
    sendSignal(ser, signal)

def press_DPX(ser, ev_type, state):
    # D-pad X axis
    signal = 'DPX,' + str(state)
    sendSignal(ser, signal)

def press_DPY(ser, ev_type, state):
    # D-pad Y axis
    signal = 'DPY,' + str(state)
    sendSignal(ser, signal)

# Analog Signal
def press_RT(ser, en_type, state):
    signal = 'RT,' + str(state)
    sendSignal(ser, signal)

def press_LT(ser, en_type, state):
    signal = 'LT,' + str(state)
    sendSignal(ser, signal)

def press_LAX(ser, en_type, state):
    # left analog stick x
    state = jst_normalize(state)
    signal = 'LAX,' + str(state)
    sendSignal(ser, signal)

def press_LAY(ser, en_type, state):
    # left analog stick y
    state = jst_normalize(state)
    signal = 'LAY,' + str(state)
    sendSignal(ser, signal)

def press_RAX(ser, en_type, state):
    # right analog stick x
    state = jst_normalize(state)
    signal = 'RAX,' + str(state)
    sendSignal(ser, signal)

def press_RAY(ser, en_type, state):
    # right analog stick y
    state = jst_normalize(state)
    signal = 'RAY,' + str(state)
    sendSignal(ser, signal)

event_dict = {'BTN_NORTH' : press_X,
              'BTN_SOUTH' : press_A, 
              'BTN_EAST'  : press_B, 
              'BTN_WEST'  : press_Y, 
              'BTN_TR'    : press_RB, 
              'BTN_TL'    : press_LB, 
              'ABS_RZ'    : press_RT, 
              'ABS_Z'     : press_LT, 
              'ABS_HAT0X' : press_DPX, 
              'ABS_HAT0Y' : press_DPY, 
              'ABS_X'     : press_LAX, 
              'ABS_Y'     : press_LAY, 
              'ABS_RX'    : press_RAX, 
              'ABS_RY'    : press_RAY, 
            }

def event_loop(ser, events):
    msg = recieveSignal(ser)
    # print(msg)
    for event in events:
        #print(event.ev_type, event.code, event.state)
                
        call = event_dict.get(event.code)
        if callable(call):
            call(ser, event.ev_type, event.state)
            #recieveSignal(ser)
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


if __name__ == "__main__":
    main()
