## **myevic**
This is My eVic VTC Mini.

### Compatible hardwares:
* eVic VTC Mini
* eVic VTwo Mini
* eVic VTwo
* Wismec Presa 75W


### Based on:
*Original VTC Mini 3.03 firmware*

### Main Features:
My custom firmware is more or less identical to the official firmware - Just added some missing features.

### Usage:

You may:

- Use [official firmware updater](http://www.joyetech.com/mvr-software/) to upload [bin/myevic_enc.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic_enc.bin)
- Use evic-usb to upload unencrypted [bin/myevic.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin) to your box, using command line:

    ```evic-usb upload -u bin/myevic.bin```
- Use [NFirmwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) to upload any of those files to your box.

### Thanks to:

    * ReservedField
    * Bane
    * TBXin
    * maelstrom2001
    * Daveee10
    * Eugene San
    * MarkyAD



### Main Screen
* Double-click:

    A double fire click switches between standard view and clock screen.
    The clock screen is close to the one viewed on the VTwo box; just better.
    See below to know how to configure clock.

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/mainscreen_zpsvjfqgoub.png)

* Third info line:

    The third info line of the main screen, used to display amps,  puffs or time counters now have a few more options:
    - Battery voltage
    - Atomizer voltage
    - Board temperature
    - Real-time clock
    - Real-time atomizer resistance

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/mainscreen2_zpsclbvvdah.png)

###Menus:
Holding fire and right button a few seconds enters menus.
There's many more to LOGO and Game menus now. See below.

* At any time, you may press simultaneously fire and right button to get out of the menus and return to the main screen.
* When navigating in sub-menus, you may press simultaneously left and right buttons to return to the parent (upper level) menu.

* Screen

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/screenmenu_zpssfqwsuy4.png)

    Screen management menu:

  * Contrast:

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/contrast_zpsjuouc0v4.png)

         Default VTC contrast is around 17%. This screen gives you access to the full contrast range of the display.
  * Protection:

         ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/protec_zpsniaf7bl8.png)

         Configure screen saver timing. After 30 seconds of inactivity, the main screen switches to the screen saver animation (see below) for the duration configured in this menu, after what the box shuts down. Scroll down the menu to the "Off" option to disable box shutdown.

  * Saver:

         ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/saver_zpsrfxibqb5.png)

         Select the screen saver animation. You may choose between None (blank screen), an analog clock or a 3D spinning cube. More to come. Content of this menu will vary from time to time; this document may not always be up to date. Have a look.

* Coils

    * Management:

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/coils_zpsn29ef1h5.png)
     * On each line of the four TC modes:

            Click fire to switch to edit mode on the resistance.
            While the resistance value is highlighted, click + and - to edit its value.
            Setting the resistance to any non-zero value automaticaly locks it.
            Bring the resistance to zero with - to unlock and reset it.
            You may also fire two seconds to zero the resistance.
      * Zero All:

            Zeroes all resistances; also zeroes all "START" mode saved resistance settings.
      * Exit:

            Self explanatory.

    * Preheat:
    
        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/preheat_zpsrpcu0fml.png)
        
        Coil preheat function. Set at wich power and how long you want the coil to be preheated when you fire; after this delay, the atomizer will be powered by your regular setting. Set the delay to zero to switch preheating off. A small "P" icon will appear on the main screen next the the power setting to remind you the preheat function is enabled ("POWER" mode only). This function can only be active in "POWER" and "START" modes; it is ignored in all other modes.

    * TCR Set:
      
        Enters the TCR Set menu. This menu can also be accessed by switching off the box and pressing both Fire and Right regulatory button for 5 seconds.


* Clock
  * Date/Time

        Setup date and time.
        Fire change edited field, + and - set value.

  * Clk Adjust

        Adjust clock: for small adjustments to clock time + and - adjust clock by seconds. Fire to save. For VTwo owners, this option also adjusts the X32 frequency accordingly if done properly.

  * Clk Speed

        For owners of VTC mini (useless for VTwo owners)
        The VTC mini does not have the needed 32kHz crystal soldered on the pcb to regulate the clock speed; so, another time source is used to drive the clock. Since its frequency is quite different, a clock speed ratio is needed to regulate the clock. As long as the box is awake (you're using it), the clock is regulated by the external 12.000MHz crystal, which is an accurate time source. Problem arises when the box enters sleep mode, since the crystal is switched off and the only clock source is the somewhat unreliable 10kHz internal oscillator of the processor. Clock drift mostly occurs when the box is switched off or sleeping.
    
        The procedure to adjust the clock speed ratio is as follow:
        * First, setup time accurately via the date/time menu or the "```evic-usb time```" command line.
        * Let the box enter sleep mode (leave it alone or switch it off) for several hours. I suggest you're doing this before sleeping yourself.
        * Awake the box and go into the "Clk Speed" menu; do not let the box enter sleep mode again in between, or you'll have to redo the whole thing.
        * Using + and - buttons, adjust the time shown on top of the screen to catch up the real time. The number shown in the center of the screen is the clock speed ratio; reasonable values should be around 32~34000.
        * Once you have adjusted the time to the real time, click fire to save the new clock speed ratio. Your clock should now be as accurate as it can. If not, try to repeat the procedure. Accuracy of a few seconds per day can be achieved this way.
    

* Modes

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/modes_zpslphwvqhh.png)

    Configure used/useless modes.
    Click on each menu item to set 'Y' or 'N' if you want to see any given mode proposed while cycling between modes in menu mode.
    Typically, you'll set "START" mode to 'N'. Typically.

* Expert

    Some advanced options:

  * USB:

        Choose between several USB modes:
     * HID: Normal operation mode - factory setting. Disables any other USB device than HID.
       HID is always active, even in the two others modes of operation. This feature permits communication between the box and firmware utilities.

     * COM: A virtual COM interface; mainly used for debugging with a COM terminal such as putty.
       May be usefull if you are developping your own version of the firmware.

     * DSK: A virtual drive to download the firmware file.
     Connect the box to a PC, and a disk device will appear with a "FIRMWARE.BIN" file. This can be read and copied. This file is an encoded firmware file that can be uploaded to another device using evic-usb or the official Joyetech firmware tool.

  * DBG
  
        Enables or disables the debug informations mode. Once the DBG option is set to "ON", Debug informations can be shown/hidden by clicking fire button four times. This option is OFF by default to prevent users to inadvertendly mess up their screen.

  * X32

        Enables or disables usage of the X32 crystal of the PCB. If this setting is "OFF", the firmware won't try to drive the Real-Time Clock with the 32.768kHz crystal. This may solve freezing issues on some malfunctionning boxes (VTwo only).
 
* Miscs
  * LOGO

        Guenuine menu
  * Game

        Flappy bird. Guenuine menu.
  * 3D

        ![3D cube](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/cube_zpsf9apfiun.png)

        Cool 3D spinning cube.
        Come back to this menu and set it to "None" to cancel animation.
        Or reset the box.

* Interface

    ![Interface menu screen](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/interface_zpsliz14bup.png)

  * Batt%

        Display battery charge percentage next to the battery picture on main screen. Resolution is 10% atm.

  * 1Watt

        Enable increment of power setting by plus or minus 1 Watt instead of plus or minus 0.1 Watts.

  * Font

        Change the display font. The font A is the new bold cool-looking font, font B is the guenuine Joyetech's font.


### Build:
Follow [evic-sdk::Readme] (https://github.com/ReservedField/evic-sdk/blob/master/README.md) instructions to:
  * Setup the environment

  * Install python-evic

  * Install and unpack nuvoton-sdk with the following changes:
    * Unpack into this project directory (evic-sdk itself is not required for this project)
    * When unpacking nuSDK unpack only ```Library``` directory. For example unpack ```M451 Series BSP CMSIS V3.01.001/Library``` from ```M451_Series_BSP_CMSIS_V3.01.001.zip``` to ```myevic/nuvoton-sdk/Library```

  * To build invoke: ```EVICSDK=. make```

### Known quirks/issues:
  * Temperature Control on SS316

    Temperature Control on SS316 is different in this firmware when compared to the original one.
    Depending on your setup, you might need to change you regular temperature setting.
    It's often better to use Temperature Control in TCR mode with SS316L, adapting the TCR/Temperature values to the build by a dry cotton test.

    *It's hard to find a consensusal value for the TCR of SS316L.
    Anything from 88 to 100 can be found depending on the source.
    The original value of 120 is obviously way too high.
    steam-engine.org gives a value of 87.9, which seems reliable, therefore the TCR for SS316 was selected as 88 instead of original 120.*
