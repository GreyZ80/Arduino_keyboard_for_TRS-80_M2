# Arduino-keyboard-for-TRS-80-M2

## Table of Contents
- [Introduction](#introduction)
- [Functions](#functions)
- [Hardware](#hardware)
- [Putty settings](#putty-settings)
- [Background information](#background-information)
- [Notes](#notes)
- [About me](#about-me)


## Introduction

The TRS-80 Model II keyboard is rather bulky and occupies a lot of desk space. Sometime you even do not have a proper working keyboard.
This project describes a solution where you use Putty running on a Windows or Linux computer with an Arduino as keyboard replacement for a TRS-80 Model II.
Connection between PC and Arduino is by means of a standard USB cable.

&nbsp; &nbsp; &nbsp; <img width="300"  src="https://github.com/user-attachments/assets/38e45ba9-abd1-4610-a2d1-7861c1418159" />

The model 16 uses the same keyboard hardware (keyboard without cable).
This project can also be used for the Models 12, 16B & 6000. These models use a keyboard with cable.
Please note that the pins on the keyboard connector of these computers are different from the Model II !!


## Functions

The following features are embedded in the unit. By changing the Arduino code, customisation can be performed.
* All standard keys are transferred to the Model II.
* Each key typed is echoed to the Putty terminal as well. Also the built-in led on the arduino will light up.
* Arrow keys on the computer keyboard are translated into the Model II keyboard code.
* Backspace is translated to the correct code for the Model II.
* Function keys F1 ~ F8 are translated into the proper codes for function keys of the Model II and Model 12.
* Function key F9 is translated into the BREAK code (03)
* Function key F10 is translated into the HOLD code (00). When pressed, the message [HOLD] will be shown of the Putty screen.

* The BUSY* line coming from the Model II is monitored to detect the presence of a powered on Model II. When the Model II is not connected, or powered down, the led will blink fast.

The BUSY* line coming from the Model II is not used to wait until the Model II is ready to receive a key. The Model II is much faster than the typist. 

When starting up (or after pressing reset on the Arduino) a welcome text will be printed on the Putty screen. This will also show the status of the Model II.
This message can also be triggered by typing Cntrl-t.

During normal operation, the build-in led blinks briefly every 8 second.
When the Model II is not connected or powered off the built-in led blinks fast at 4 Hertz to indicate an abnormal situation.
In this situation the Arduino will still transmit characters using the DIN connection. It does not check the BUSY* line.
This allows checking of the workings with a Tandy computer connected. 

## Hardware

The smallest Arduino I could find was good enough (without additional circuitry). You only essentially need to add a 5 pin 180 degrees DIN female connector.\
Arduine used: Arduino Nano\
Processor: ATmega 168\
These boards are available with different USB connectors. I have used mini USB for my unit.

&nbsp; &nbsp; &nbsp; <img width="300" alt="click to enlarge" src="https://github.com/user-attachments/assets/7eec2c83-32c4-4d01-a011-8550477db702" />
<BR><BR>

>[!CAUTION]
> **As the Arduino receives power via USB from the Windows or Linux PC, the 5 Volt from the Tandy computer must NOT be connected to the Arduino. Doing this anyway might damage the PC, the Arduino and/or the Tandy computer!**


When building the Arduino into a small enclosure you might want to add and extra push button for the reset (temporarely connect to ground), and an additional led with resistor (e.g. 220 ohms) connected from D13 to ground. 


&nbsp; &nbsp; &nbsp; <img width="300"  src="https://github.com/user-attachments/assets/b4b0e0d9-66c4-4043-8b78-8d2a7726f5ef" />
&nbsp; &nbsp; &nbsp; <img width="300"  src="https://github.com/user-attachments/assets/026540a4-2391-4acd-8c78-d64c4d319a53" />

I use a momentary ON switch with embedded led on top of the little enclosure.

The pins of the DIN connector are connected to the Arduino in the following manner:


| Signal | Model II, 16 | Model 12, 16B, 6000 | Arduino Nano | Color |
|--|:---: |:---: |:---: |:---: |
| DATA | 1 | 1 | D4 | purple |
| BUSY* | 2 | 3 | D2 | white |
| Gnd | 3 | 5 | Gnd | black |
| CLOCK | 4 | 2 | D3 | grey |
| +5V | 5 | 4 | NOT USED |

Additional signals:

| Function | Arduino Pin | Color |
|--|:---: |:---: |
| Same as built-in led | D13 | blue |
| Reset line | RST | orange |

## Putty settings

Putty can de downloaded for free from https://www.putty.org/.  
To work correctly, Putty emulation must be set to VT100+ mode. Otherwise the function keys do not work correctly, but stil good enough for quick testing.\
Local echo should be off.

<img width="400" alt="Putty Settings" src="https://github.com/user-attachments/assets/599dc90e-d137-43fb-854f-a443946c9224" />

On a Windows PC, the OS might assign a different serial port to the USB connection to the Arduino. On my system the Windows system manager show the port used. This is normaly port 7, sometimes 6. The Arduino IDE also shows the port under the Tool tab.\
On a Chromebook the Minicom application works nice in the Linux shell.


&nbsp; &nbsp; &nbsp; <img width="297" alt="Windows device manager Arduino on COM7" src="https://github.com/user-attachments/assets/6e96717b-5a24-4594-b198-02c849a8e1fd" />
&nbsp; &nbsp; &nbsp; <img width="573" height="394" alt="Arduini EDI showing Tools setting" src="https://github.com/user-attachments/assets/1d7cc2b4-d7ee-407b-9f83-87ca81849277" />


## Background information

Detailed information on the keyboard interface can be found in the Model II technical reference manual. Essential is the timing relationship between the DATA and CLOCK signals.

<img width="900"  alt="Keyboard timing diagram" src="https://github.com/user-attachments/assets/6c5b35c3-feb5-4467-a511-fb046971d69c" />

Below the signal of the 00 character (HOLD or Ctrl-Space) as send by the Arduino.  
The DATA line (yellow) shows the data (8x low) followed by the stop bit. When not active this line is high (+5 Volt).     
The CLOCK line (blue) shows the 8 clock bits. When not active this line is low (0 Volt).  

&nbsp; &nbsp; &nbsp; <img width="900" src="https://github.com/user-attachments/assets/6b22c48f-cf8b-4f8b-a609-2fec3f41ed4d" />



The exact frequency of the CLOCK signal pulses is not critical. The timing is determined in the software by the value of the QuarterPulse.
The signals are created in 4 parts for each bit where the signal level for the DataBit and ClockBit are set high or low. After sending the data the stop bit is transmitted.
The current code gives bit pulse width of about 500 micro seconds. Transmission of 1 character takes about 10 ms. In reality that is fast enough (max. 100 characters per seconds).

## Notes

- During startup, the Tandy computer might detect one or two spurious keystrokes. Possibly caused by the fact that the Arduino is initialising. I have not been able to fix this.
- It is not absolutely necessary to use Putty. Other software that can communicate with USB as serial port can work. However, many programs I found will only transmit the characters to the Arduino after hitting [enter]. That might not always be practical.
- When the keyboard stalls, you can try to reset it or restart the putty session
- When a single keystroke translates in multiple (weird) characters, check the data line 

## About me

I was fascinated by the TRS-80 Model I as soon as it hit the streets in the Netherlands. As a student of the University of Utrecht I purchased the Model I as a 16K machine.  Printing listings was performed on a Teletype that was bought used from the Philips Nat Lab in Eindhoven. Very noisy but reliable.
Later on my Model 1 was expanded to 64K and received a Dutch (home build) expansion system called the "Utrecht Bus", which used ECB format boards. Functions of the system were floppy disk, serial port and Eprom programmer.
My collection of Tandy computers was then expanded with a Model 100 and a Model 4P which ran Newdos80 in 80x24 character mode.
The bigger Tandy computers were not witin my reach. However, when I was able to purchase a working Model II in 2017 I did not hesitate. Meanwhile that system has been expanded with my **512K RAM board** (designed and build by me) for use by the Z80 and a Gotek drive for easy "floppy disk" use.  
The latest addition to my Model II is the **AM9511 Arithmetic Processing Unit (APU)** described here: https://github.com/GreyZ80/AM9511_for_TRS-80_M2.  
My Arduino experience started in 2014 with the design of an automatic car door unlock when approaching my Volvo 850 carrying a (known) Bluetooth device.

I can be contacted on Discord (as LaserVision) and I do visit the Tandy Assembly events in the USA. When you login to Github, you can also see my e-mail address.
