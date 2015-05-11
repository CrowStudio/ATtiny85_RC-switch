# RC-switch
Since I have a cheap electrical speed controller (ESC) that does not support reverse speed I had to come up with something that could pole switch my DC-motor and make it happen :-)  
The RC-switch is a ATtiny85 that is triggered by the OrangeRx R615 channel one's (throttle) PWM-signal. When µs is greater than 1520 µs the switch (relay) is turned off, when µs is lesser than 1480 µs the relay is turned on.  

