# Auto feeder firmware
*FarmWare - Powered by Arduino*

## Introduction
This is a project I made for my final project, as part of the IOT102 course at FPT University, semester Spring 2024.
As this is only a proof-of-concept build, only the software and general build is available without a 3D model.
However, with a 3D printer and some hot glue, this project can definitely be made to work.

## Features
A machine that monitors feed bowls and water bowls to automatically carry out refills!
- Automatic, unattended refills for as long as supplies last
- Accurate monitoring of amount of remaining food and water
- Intelligent operation: refills are only carried out when called for and when it is safe to do so

## Operation
The system works on a simple open-looped mechanism to (i) control a feedbox and (ii) drive a pump.<br>

The feed bowl state is determined by 2 photoresistors: 1 at the top of the bowl and the other situated at the bottom. When both photoresistors detect light, the system will assume that the bowl is empty, and the feedbox will be opened. When the bottom sensor is obscured, but the top sensor is not, the bowl is in its nominal state. When both sensors are obscured, the system will take that for the bowl being completely full. In both of the latter cases, the feedbox is closed.<br>
Both of these sensors has a sensitivity value in the range of [0, 1] to determine how sensitive they are to changes in lighting conditions. Values closer to 1 means it is easier to be obscured.<br>
Note that to prevent overfill, the top sensor is programmed to override the bottom. *As long as the top sensor is obscured, the feedbox will never open.*<br>

The feedbox will also be forced close if presence is detected near the feed bowl. This is to ensure safety during refills as food can spill out when refilling with animals nearby.<br>

The water pump is controlled by a relay driven by a water level sensor. This sensor has a set lower and upper threshold, as well as a low threshold. Once the water goes below this lower threshold, the pump is turned on. Pumping stops when the max threshold is reached.<br>
The water level sensor has a multiplier value [0, 1] to determine the low threshold. Values closer to 1 push the threshold higher.<br>

After a predetermined time interval (default is 72h) and the system detects no changes on the presence detector sensor, it will assume that it is not in active use, i.e. no animals are present on the premise at all. It will then timeout, requiring a manual restart to be used again. The onboard LED will blink to notify the operator when this happens.

Consult the following flow chart for a visualization of the operation:
![Program flow chart](program_flow.png?raw=true)

## Hardware used
- Microcontroller: Arduino Uno R3
- Presence detector: HY-SRF05 ultrasonic distance sensor
- Feeder box actuator: SG90 micro servo
- Bowl state detector: generic photoresistor
- Water level sensor: SEN18 water level sensor
- Water pump: mini submersible auto-priming pump
- Relay switch: JQC-3FF-S-Z 5v 12a single-channel relay
- Power supply: MB-102 breadboard power supply
- 330 ohm resistors
- Yellow LED

## Build schematics
An *incomplete* build schematic can be found at: [Tinkercad schematic](https://www.tinkercad.com/things/fEtl90w5uD5-project-auto-feeder/editel?returnTo=/dashboard?collection=designs&sharecode=Eop9zU_BRC4AbcI0ykFd5Q39rgGpT1Hto60VDNmFsO0)<br>
This doesn't include all hardware components since Tinkercad has yet to include components used in the water subsystem.
All pinout is included in the firmware file. These values can be modified to adapt to different build requirements.

## Notes
- Set the debug flag `serialDebug` to `true` to enable debugging. Remember to use the correct baud rate.
- After every build and before running, verify sensor calibrations and **disable the debug flag**. Leaving the flag on will make the system runs 4 times as slow in its default configuration.
- For best sensor readings, all sensors must be powered by the 5v rail from the Arduino.
- The servo must be connected to the Arduino rail.
- The pump is to be supplied by an external power source.
- Avoid running the pump dry. Prolonged pumping without water can damage the pump.
- The JQC-3FF-S-Z is a low level trigger relay. As such, its controls are inverted.

## To-do
- Optimize program loop
- Add auditory alert during pumping (optional)
- Generate 3D models (recommended but not required)
- Finish system schematics
