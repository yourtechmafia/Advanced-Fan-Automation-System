# Advanced-Fan-Automation-System

This system adds multiple functionalities to a traditional fan. Compatible with Arduino or any compatible Atmel microcontroller.
It: 
* Has a temperature sensor (LM35) that monitors room temperature.
* Can regulate fan speed based on room temperature.
* Can save fan speed in events of power outage.
* Has an overvoltage protection system.
* Four buttons for speed and settings configuration.
* Current and voltage metering system.

Code is well commented for easier editing.

Here are the list of modules used in this project:

* Arduino Uno (ATMega328P) or any other compatible Atmel microcontroller.
* 4 tact switch for selecting speeds.
* PT2272-M4 wireless remote (transmitter and reciever module).
* LCD 1602 (16 x 2 LCD) for data display.
* ACS712 current module.
* Resistive Voltage Divider Analog circuit for analog voltage measurement.
