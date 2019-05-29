import inputs
from time import sleep

def vibL(pad, ev_type, ev_state):
    if ev_state == 1:
        pad.set_vibration(1, 1, 100)
        print("Howdy!\n \t", ev_type, " | ", ev_state)
    
def vibR(pad, ev_type, ev_state):
    if ev_state == 1:
        pad.set_vibration(1, 1, 100)
        print("Howdy!\n \t", ev_type, " | ", ev_state)
# A dictionary that link event type to function

def vibB(pad, ev_type, ev_state):
    if ev_state == 1:
        pad.set_vibration(1, 1, 100)
        print("Howdy!\n \t", ev_type, " | ", ev_state)

# A dictionary that link event type to function
event_dict = {
        'BTN_NORTH' : vibL,
        'BTN_EAST' : vibR,
        'BTN_SOUTH' : vibB
        }

# event loop 


def event_loop(pad, events):
    for event in events:
        print(event.ev_type, event.code, event.state)
        call = event_dict.get(event.code)
        if callable(call):
            try:
                call(pad, event.ev_type, event.state)
            except:
                print("Error");

'''
NOTE

ev_type:
    - Key : boolean
    - Absolute : continuous value 
'''
if __name__ == '__main__':
    pads = inputs.devices.gamepads
     
    if len(pads) == 0:
        raise Exception("Couldn't find any Gamepads!")
    else:
        pad = pads[0]
    
    try:
        while True:
            try:
                event_loop(pad, inputs.get_gamepad())
                #pad.set_vibration(1, 1, 100);
            except inputs.UnknownEventCode:
                pass
            
    except KeyboardInterrupt:
        print("Bye!")
