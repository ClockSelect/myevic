### Expert

Some advanced options.  
Normal users should barely have anything to do with those options, and can live perfectly well without ever knowing they exist. 
I do not recommend "testing" those options just to "see what it does". Deny responsability of everything etc. 
You're supposed to have 18+, after all^^

  * __USB__

        Choose between several USB modes:
     * HID: Normal operation mode - factory setting. Disables any other USB device than HID.
       HID is always active, even in the two others modes of operation. This feature permits communication between the box and firmware utilities.

     * COM: A virtual COM interface; mainly used for debugging with a COM terminal such as putty.
       May be usefull if you are developping your own version of the firmware.

     * DSK: A virtual drive to download the firmware file.
     Connect the box to a PC, and a disk device will appear with a "MYEVIC.BIN" file. This can be read and copied. This file is an encoded firmware file that can be uploaded to another device using evic-usb or the official Joyetech/Wismec firmware tool.

  * __DBG__
  
    Enables or disables the debug informations mode. Once the DBG option is set to "ON", Debug informations can be shown/hidden by clicking fire button four times. This option is OFF by default to prevent users to inadvertendly mess up their screen. It's of no interest if you're not developping your own version of the firmware.

  * __X32__

    Enables or disables usage of the X32 crystal of the PCB.  
    If this setting is "OFF", the firmware won't try to drive the Real-Time Clock with the 32.768kHz crystal. This may solve freezing issues on some malfunctionning boxes. This setting will be active at next reset.  
    After reset, if the box cannot use the X32 to drive the RTC, this option will be set back to "OFF" and the Light Sleep feature will be enabled (see "LSL" below).  
    *This option is useless and forced to "OFF" on boxes known not to have an X32 crystal. Those are all boxes for which the manufacturer did not enable the RTC feature, i.e: VTC-Mini, Cuboid/Mini, Presa 75W and RX series.*  

  * __LSL__
  
    Light Sleep mode.  
    *This setting is useless and forced to "OFF" on boxes with a X32 crystal (VTwo/Dual, AIO, Basic, eGrip II), and defaults to "ON" on other boxes.*  
    On boxes where the Real-Time Clock is emulated (like the VTC-Mini), by setting the Light Sleep mode "ON", the box continues to drive the Real-Time Clock with the external 12.000MHz Crystal instead of the internal LIRC oscillator when entering sleep mode. This makes the Clock far more accurate and eliminates the need for the Clock Speed ratio, at the cost of a greater battery consumption (estimated less than 50 mAh/day).  
    Using this setting, Clock accuracy is identical to those of real RTC boxes.

  * __NFE__
 
    *Warning: You can't use Joyetech's firmware updater while this option is "ON". It may aloso cause issues with third-party firmware management systems.*  
    Enables or disables partial compatibility with the [NFirmwareEditor 5.x](https://github.com/TBXin/NFirmwareEditor/releases) myEvic Configuration Editor.  
    This option is disabled by default; set it to "ON" if you plan to use developpement features of the NFE. You'll have access to Monitoring, Screenshot, COM terminal, and Custom Battery Profile editor.  
    Since compatibility is only partial at the moment, editing parameters other than Battery Profile will have no effect.  
 
  * __SHR__
  
    Shunt Resistance (in mΩ). (real resistance in Ohm now)
    *Warning: This item is dangerous to your box. Messing with this parameter may cause overcurrent in the atomizer circuitry and lead to definitive box failure.*  
    This item let you edit the value of the Atomizer's circuit shunt resistance. This resistance is involved in every resistance/current/power measures and computation in the whole firmware. This value should only be changed if you know exactly what you are doing.  
    To reset the shunt value, select the SHR menu item and press the fire button during 2 seconds. This will revert the value to the default hardware setting.

  * __UCH__

    USB Battery Charging.  
    *Multi-cell boxes only. This option has no effect on single-cell boxes.*  
    Enables or disables battery charging via the USB port. The USB port is still usable for all other purposes: firmware management, debugging, etc.  
    If you have an external battery charger and want to take care of your batteries, it is recommended to switch off the USB charging feature to avoid potentialy unbalanced charges, or unadapted charge currents at end of charge.

  * __BAT__

    Battery model.  
    You can specify your battery brand and model among several ones: 25R, 30Q, HG2, HE4, VTC4, VTC5 and VTC6. "GEN" is the generic battery used by default and should be used for any other model of battery or for built-in battery.  

    Custom Battery:  
    By pressing the Fire button during two seconds while editing this option, the battery model changes to "CUS" (Custom Battery). The box will use the user-defined battery discharge curve and settings. Those informations can be edited using [NFirwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) discharge profile editor (see [NFirwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) documentation for more information).  
    *The NFE compatibility mode must be set to ON in the expert menu for NFE to recognize the box as compatible.*

    At the moment, battery model information is used by the firmware to compute a more accurate state-of-charge than the generic setting, and to avoid battery stress by limiting max Amp draw. Max Amps by battery model is based on Mooch's stress tests and are fairly reliable.  

    *On boxes with built-in battery (AIO, Basic, eGrip II, Cuboid Mini), this option should be kept on the "GEN" setting unless you've taken your box apart and manualy replaced the internal battery pack by something else.*  

  * __BVO__

    Battery Voltage Offset submenu  
    Corrective offset value of the battery voltage. Depending on your box, the displayed battery voltage may be off by a few tens of millivolts. It's usually not a concern, but it may make the box locking the vape too early (wasting some battery capacity) or too late (box resets due to low voltage when firing).  
    Use an external accurate voltmeter (your battery charger may do the job) to compare the displayed voltages on the box to the actual battery voltages, then adjust the displayed voltages with this item.  
    Range is -1.00 to +1.00 Volts by step of 10mV.  
    * On a single-cell box, only the first setting (B1) is significant. B2 and B3 are ignored.  
    * On a fixed dual-cells box, the two first settings (B1 & B2) are used, respectively, for the first and the second battery (depending on the box, you'll have to determine wich one wich with your voltmeter). B3 is ignored.  
    * On a mixed single/dual-cells box, B1 is used for the lone battery in single-cell setting, and B2 & B3 for the two cells in a dual-cell setting.  
    * On a triple-cell box, B4 is ignored.  
    * On a quad-cell box, I let you guess.  

-----

← Previous Page: [Interface](interface_en.md) 
