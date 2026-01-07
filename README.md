## __USBC-eDP Driver Board__  

![Soldered embedded displayport adapter](docPics/PCB_SHOT.jpg)
This project works to adapt a USB-C DisplayPort alt-mode input into an embedded DisplayPort output. It uses an STM32 to communicate over the USB-C Power delivery protocol, acts as a USB Hub providing one USB 3.0 Superspeed port and 2 USB 2.0 High Speed ports. It also negotiates the highest current available from the source, and internally generates the 12V needed to run the display.  It works by simply routing the same DP signals through to the output, and does no signal processing of it's own. 
## USE  

![Portable monitor built using this driver board](docPics/monitor.jpg)

It should work with most 30pin eDP panels. Since it has no native processing on it's own, the DP generation is set by the computer and panel used. Generally, this allows for up to 1080p panels. Exceptionally large or exceptionally bright panels may not work, as those panels tend to draw a large current on the 12V rail. I highly recommend sticking to panels that draw a continuous current of no more than 800mA for the backlight.

The STM32 provides PWM dimming capability by using the potentiometer on the daughter board. If you want to remove this capability, short pin 1 to pin 2 on J2 (This is a temporary solution, the plan is to in the future specify which brightness control scheme should be used using R25 and R26)

Personally, I am using the Innolux N116BCA-EA1 panel with a 3D printed case published in the Mechanical folder. If you would prefer a 1080p panel, the AUO G116HAN looks like it should also fit the case, however I do not have one to test with. 

#### NOTICE
The USB 2.0 ports are configured to run outside of the USB specification. They claim to be able to provide up to 500mA of current, however trigger overcurrent protection at 250mA. This can be fixed by replacing line 17 in i2C_comms.c with {0x18, 0x20, 0x01}, however the incorrect version can be useful with higher power flash drives that require 200mA, which was the reason I chose to do it this way.
## BUILD INFO

#### PCBs
BOMs for both the main board and secondary board are in the electrical folder, and contain all the parts required to build a fully functional PCB.  The main board is specifically designed to be manufactured using JLC-PCBs JLC04081H-3313 stackup, as the USB lines are impedance matched, and the generated gerber files are designed to be used with JLC. The potentiometer board has no such restrictions, but I recommend using a 0.8mm PCB. Currently there is no pick and place support, so you will need to hand solder the PCBs. The passives should be rated for at least 12V

## Firmware
The firmware is written using the STMCube Ide. To get the PCB to work, you need to compile it using cube IDE, then upload it to the STM32 using either a J-link or STlink. If you have a TagConnect TC2030, there is a pad connector available on the PCB, if not, the pads are big enough you could solder on some leads to them.

## FFC cables
The design uses a Type A, 4pin, 0.5mm pitch FFC cable to connect the main board and pot board. For my design, I used a 10cm long cable, though what length you need may depend on your chosen LCD and layout

You will also require a 30 pin eDP cable. I used a 250mm one for my design.

## Hardware
The mechanical design is published as a set of solidworks files as well as a set of 3MFs. The 3MFs were made for the Prusa MK3S+ with the MMU2, though they will likely work with most slicers and printers that support multi filament printing. They are designed to be printed in PETG with appropriate scaling to account for shrinkage. A couple of the parts also have support blocks with a PLA interface so they should be removable. 

To fully assemble the enclosure, you will need:

- 19x [M2 4.8mm Wide, 1.5mm thick nuts](https://www.mcmaster.com/92513A110/)
- 13x [M2 10mm Countersunk Screw](https://www.mcmaster.com/90236A106/)
- 17x [M2 6mm Countersunk Screw](https://www.mcmaster.com/90236A104/)
- 13x [M2 4mm Countersunk Screw](https://www.mcmaster.com/90236A102/) 
	
## TODO
Future Plans are mostly in the software:
 - USB HID interface for brightness control and notifications
 - Proper current monitoring and active over-current protection control by reducing screen brightness
 - Use R25 and R26 resistors to define which brightness control method is desired.
 - Redesign the enclosure to use metal nuts. I just used some nuts I had on hand that I thought were M2 but are actually probably M2.5