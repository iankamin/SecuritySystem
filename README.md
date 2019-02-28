# Security System (Team Clouseau)
This is an alarm system that alerts the surrounding area when an intruder through a passageway.

An individual walks up to the numeric keypad and enters the code to enter, each keypress vibrates the keypad. Once the correct code is entered a green indicator led lights and the individual is allowed to pass. If an incorrect or no code is entered the red led remains on. If the individual passes the doorway while in this state the buzzer pulses. The correct code has to then be entered to turn the buzzer off and put the system in the unlocked state.

Once unlocked, a new code can be entered to go into the locked state. This is now the new code that is needed to unlock the system. In addition, the code is saved to EEPROM so that on restart or shutdown the passcode is retained.

## Hardware
![keypad]()
The microcontroller is housed in a plastic casing with the keypad and LEDs mounted outside. This is powered via 9v battery. The IR transmitter is powered by another 9v battery on the other side of the passageway. This was done to allow us to separate the IR break sensors further.
