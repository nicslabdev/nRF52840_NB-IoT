# nRF52840_NB-IoT_AT_Terminal

This application stablish a bypass connection between USB UART and Arduino serial pins UART, i.e. D0 and D1 pins.

This is used in order to manage NB-IoT module directly through AT commands.

This code has been developed for Nordic Semiconductor nRF52840 DK and NB-IoT Arduino shield. 

Tested NB-IoT shileds: AVNET BG96.

## Requeriments

This project uses:
-   nRF5 SDK version 15.3.0
-   nRF52840 PDK (PCA10056)

## Get started

You can find a hex folder with the **precompiled application** for PCA10056.

In order to stablish serial connection with your PC, you can use PUTTY if you are a Windows user or GNU Screen if you are a Linux user.

**PUTTY connection parameters:**

    Baud rate: 115.200
    8 data bits
    1 stop bit
    No parity
    HW flow control: None

**GNU screen**

    sudo screen /dev/ttyACM0 115200
    
Where */dev/ttyACM0* is the nRF52840 device. You can know what is the path of your nRF52840 connecting it and executing *dmesg* on UNIX terminal.

## Compiling the applications

If you want to compile the project, you can use SEGGER Embedded Studio for ARM. Put the downloaded folder into 
	
	\nRF5_SDK_15.3.0\apps\