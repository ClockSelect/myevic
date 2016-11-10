## **myevic**
This is myevic Custom Firmware.

### Compatible hardwares:
* eVic VTC Mini
* eVic VTC Dual
* eVic VTwo Mini
* eVic VTwo
* eVic AIO
* eVic Basic
* eGrip II / Light
* Cuboid
* Cuboid Mini
* Wismec Presa TC75W
* Wismec Presa TC100W
* Wismec Reuleaux RX75
* Wismec Reuleaux RX200S
* Wismec Reuleaux RX2/3


### Based on:
*Original VTC Mini 3.03 firmware*

### Main Features:
The look-and-feel of the myevic firmware is more-or-less identical to the original firmware, so you shouldn't be lost.  
Many new features and options are made available through the menu system; I strongly encourage you to fully read the present documentation.

### Usage:

The binary firmware file is included in the **bin/** subdirectory of the project.  
If you are only interested in installing and using the firmware, download it here : [bin/myevic](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin)
  
Once you've downloaded the firmware binary, you may:

- Use the manufacturer's firmware updater to upload [myevic.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin) like you would with a usual firmware update from Joyetech or Wismec:
  - Download the firmware updater package from the manufacturer [Wismec](http://www.wismec.com/software/) or [Joyetech](http://www.joyetech.com/mvr-software/), depending of the brand and model of your box. Be sure to pick the right software for the right box brand and model.
  - Unzip the update package and launch the UpdateFirmware.exe (Joyetech) or UpdateWismec.exe (Wismec) found in the package.
  - Plug your box into your PC via USB  
    Be sure you use a data-capable USB cable. Some cheap cables coming with some boxes only ensures box charging. If unsure, use the cable that came with your box. Once done, the updater window should show the model and version of your box.
  - Click the "Update" button; a file selection dialog will open. Select the "myevic.bin" file.
  - Wait for the update process to complete.  
    If something goes wrong, retry to update. Update fails sometimes; this shouldn't be a concern.
  - Enjoy.  
  
OR  

- For linux/Cygwin users, use evic-usb to upload [myevic.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin) to your box, using command line:

    ```evic-usb upload myevic.bin```

OR  

- Use  [NFirmwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) or the [FWUpdater](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1) part of the [VTCFont](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1) package to upload [myevic.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin) to your box.

### Thanks to:

    * ReservedField
    * Bane
    * TBXin
    * maelstrom2001
    * Daveee10
    * Eugene San
    * MarkyAD
    * bluenazgul
    * Sharky1980
    * gdb
    
    And many others, supporting this project and taking the time to open or help solving issues.



----------
### Changes in behavior ###
Some changes has been made to the overall behavior of the firmware from the original. This paragraph may be read by any new user not to think something is a "bug".

* Stealth mode:  
  The stealth mode has been extended to some more screens.  
  The battery charging screen does no longer show up in stealth mode; only for a few seconds when plugging in the USB cable, just to show it's been correctly plugged.  
  The Key Lock/UnLock screen reverts to black screen if keys are locked.  
  The Stealth On/Off screen goes to black when setting stealth On.

* Right (+) and Left (-) buttons:  
  While editing some values, the speed of change has been made more progressive before reaching the insane terminal velocity.  
  Pressing those buttons does no longer wake up the box from screen saving, except if configured to do so in the Interface menu (see below). This makes the box more pocket-proof.

* Temperature setting in TEMP mode:  
  The temperature no longer wraps-around by switching between °C and °F; the temperature unit has to be chosen once and for all in the Interface menu. Default is °C.  
  The temperature setting can vary by +/1 °C or +/-5 °F when edited if configured to do so in the Interface menu (see below).

* Fire button Multi-Clicks:  
  Clicking rapidly 2, 3 or 4 times on the fire button may now have some effect - see below in the Interface menu description to see those effects and how to set (or unset) them up.  
  Clicking 10 times for board temperature, or 20 times for version screen does no longer require to switch off the box.  

* Coils memory:  
  In POWER mode, the box now reminds of the power setting for any given resistance, up to 10 different (just like in SMART mode). Thus, when swapping between your tanks, the box will automatically set up the power setting you previously entered for that particular coil.  
  Coils may vary by +/-10% in resistance to still be recognized, and two different coils within 10% won't be distinguished.
  The coils memory can be erased in the Coils >> Manage >> Zero All menu item (see below).

* Bypass mode:  
  Power is now displayed on top of screen instead of voltage, as it's a more relevant information regarding vaping quality. Real-time output voltage information is still available as a third-line option (VOUT).  
  Predicted power and voltage are also more accurate, especially on multi-cells boxes. It needs a few puffs to reach max accuracy, time for the box to estimate battery behavior under high load.

* Logo:  
  The myevic firmware accepts logos sizes of 64 pixels wide and from 40 to 63 lines high. Tall logo may partially recover lines of information in the center of the screen; it's up to you to choose the correct design and dimensions so that the display looks fine.

* Menus:  
  Menus accessed by pressing Fire and Right button have been completely revamped. See below for new menus description.

----------
### About Multi-cells boxes

At the moment, the interface only display one battery icon, percentage or voltage, just like there where only a single battery in the box (this may change in the future).  
The displayed values are those of the battery that is in the *lowest state-of-charge* of the set.  
A battery set should always be kept in equilibrium and a balance warning is issued if the difference between the lowest and the highest voltage exceeds 0.3 Volts.  
So as long as you have no warning (a blinking "balance" on the battery icon or an "imbalanced batteries" screen warning), you know that all batteries are within 300mV of the first.

----------

### Main Screen
* Double-click:

    A double fire click switches between standard view and clock screen.  
    The clock screen is close to the one viewed on the VTwo box; just better, imo.  
    See below the Clock menu to know how to configure clock.

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/mainscreen_zpsuoh0cthd.png) ![](http://i345.photobucket.com/albums/p374/ClockSelect/mainscreen3_zpsfunjeoct.png)

* Third info line:

    The third info line of the main screen, used to display amps,  puffs or time counters now have a few more options:
    - Atomizer voltage
    - Battery voltage
    - Board temperature
    - Real-time clock
    - Real-time atomizer resistance
    - Estimate of vape velocity in ml or ml/day *(experimental)*

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/mainscreen2_zpsclbvvdah.png)

----------

###Menus:
Holding fire and right button a few seconds enters menus.
There's many more to LOGO and Game menus now. See below.

* At any time, you may press simultaneously fire and right button to get out of the menus and return to the main screen.
* When navigating in sub-menus, you may press simultaneously left and right buttons for one second to return to the parent (upper level) menu.

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/menus_zpsb8gftzok.png)  
Main menu screen

----------

* Screen

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/screenmenu_zpsph9b8ono.png)

    Screen management menu:

  * Contrast:

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/contrast_zpsjuouc0v4.png)

         Default VTC contrast is around 17%. This screen gives you access to the full contrast range of the display.
  * Protection:

         ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/protec_zpsx0jd4aok.png)

         Configure main screen dim timeout and screen saver timing. After the time setup with the "Main" option, (default 30 seconds) of inactivity, the main screen switches to the screen saver animation (see below) for the duration configured with the "Saver" option, after what the box shuts down.  
         If the "Saver" item is set to "Off", the box will dim after "Main" second, then switch to a black screen for 3 minutes before being shut down.

  * Saver:

         ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/saver_zpsdkyhj1mz.png)

         Select the screen saver animation. You may choose between None (blank screen), the clock (analog or digital depending on the Clock menu setting), 3D spinning objects, the logo, a Qix-like animation, or the snow screen. More to come. Content of this menu will vary from time to time; this document may not always be up to date. Have a look.  
         When the 3D screen saver is active, the + and - buttons change the animated object.  
         About the snow screen saver: choose this screen saver if your box experiment burnt pixels; it's a common issue on boxes a few months to a year old, and this screen may help restore the pixels. Se also the Invert option that may serve the same purpose.

  * Logo

         The Logo submenu is used to toggle display of the custom logo On of Off, and to choose where to display it. Logo may be displayed on the top of the screen (in place of the power/temp setting zone) or in the middle of the screen in place of the two first information lines.

  * Invert  
  
         ![](http://i345.photobucket.com/albums/p374/ClockSelect/invert_zpsyowemzqu.png)  

         Toggle display mode between white on black and black on white.  
         Beyond the fact you may find this option fancy and cool-looking, it may have its usefulness if your box experiment burnt pixels on the always-white zones. You may use this option in combination with the snow screen saver to restore the quality of your display (see the snow screen saver above in the Screen>Saver submenu section).

  * Miscs  
      Sub-menu
      * Game

            Flappy bird. Guenuine menu.

      * Led  

            eGrip II / eVic AIO LED Color setting.

      * 3D

            ![3D cube](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/cube_zpsf9apfiun.png)

            Cool 3D spinning shapes.  
            Come back to this menu and set it to "None" to cancel animation.  
            Or reset the box.  


----------

* Coils

    * Management:

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/coils_zpsn29ef1h5.png)
     * On each line of the four TC modes:

            Click fire to toggle edit mode on the resistance.  
            While the resistance value is highlighted, click (+) and (-) to edit its value.  
            If you click fire again without editing the resistance value, this will toggle its locked/unlocked status.  
            Editing the resistance to any non-zero value automaticaly locks it.  
            Setting the resistance to zero with (-) will unlock and reset it.  
               
            You may also fire two seconds on one of the four resistances to reset its value; if the selected resistance corresponds to the current vaping mode, this will force an immediate reading of the atomizer, setting and lock of the new value.  
              
            When entering the Coils Management menu, the resistance corresponfing to the current vaping mode (if applicable) will be pre-selected.

      * Zero All:

            Zeroes all resistances; also zeroes all "SMART" and "POWER" modes saved resistance settings.

      * Check: Yes/No

            *If you feel you need to use this feature in any other situation than a really heavy build with low TCR value (SS and such), you surely have an issue with your coil. Using this feature may be risky and therefore is not recommended. Check your build first.*  

            In TC modes, the box checks for suitability of the resistance the first time it is fired (i.e. each time the atomizer in screwed on or after box reset). The test is done in a way that can make heavy builds with low TCR value fail the test (really heavy, like dual twisted clapton or such; more than around 1.7 grams of metal). If a coil fails the first fire test, the box will enforce the power mode.  
            If you know your heavy build is well done and suitable for temp control, you may set this option to "No". The box will bypass the first fire test and let you vape in the selected temp control mode.  

            *Keeping the coil test disabled may get you into unpleasant situations. Use wisely.*

    * TCR Set:

        ![](https://www.dropbox.com/s/n09iy9nu57jnv18/tcrset.png?dl=1)  

        TCR Set menu.  
        *This menu is also accessible by switching off the box and pressing both Fire and Right regulatory button (+) for 5 seconds.*  
        
        In addition to the three usual TCR modes (M1, M2 & M3), the box standard presets for TEMP NI, TI and SS are editable. To edit one of the three preset modes:
        * Select the mode you want to edit,
        * Long fire to unlock the default (DEF) behavior; a reasonable value is proposed as a default starting value for edition,
        * Edit the TCR value.
        * Long fire to restore the default (DEF) behavior if needed.


----------
* Vaping  
 
    ![Vaping menu](https://www.dropbox.com/s/5nafopmyhl6m8as/vaping2.png?dl=1)  

    * Preheat:
    
        ![TCR Set Menu](https://www.dropbox.com/s/d1ncqa9heec6grx/preheat1.png?dl=1)
        
        Coil preheat function.  
        *This function can only be active in "POWER" and "SMART" modes; it is ignored in all other modes.*  
        Set at wich power ( **Pwr** ) and how long ( **Time** ) you want the coil to be preheated when you fire; after this delay, the atomizer will be powered by your regular setting. Set the **Time** to zero to switch preheating off.  
        The **Unit** may be absolute Watts or percents of the main power setting.   
        The **Delay** allows to configure a dead time after fire, during which the preheating function will be disabled. This dead time goes from zero (Off) to 3 minutes. The delay runs from the end of fire. Use this feature if you use high inertia (heavy) builds to prevent over-heating when chain-vaping.  
        A small **P** icon will appear on the main screen next to the the power setting to remind you the preheat function is enabled ("POWER" mode only). A blinking **P** indicates the dead time period.  
        *If you configured a **PPwr** action in the multi-clicks configuration (see **Interface** menu), this action will pop-up this menu when in "POWER" or "SMART" mode.*  

    * Modes

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/modes_zpslphwvqhh.png)

         Configure used/useless modes.  
         Click on each menu item to set 'Y' or 'N' if you want to see any given mode proposed while cycling between modes in menu mode.  
         Typically, you'll set "SMART" mode to 'N'. Typically.

    * Algo
    
         *Temperature Control only*  
         TC Algorithms submenu  

         ![Algo submenu](https://www.dropbox.com/s/ecy9oqapftnfwh3/algo.png?dl=1)  

         * Algo  
         Choose betwen several algorithms for temperature control.
           * Off: use the default standard agorythm from stock firmware.
           * Sweet: much like the default, but smoothes out oscillations in regulation phase.
           * Boost: speeds up temperature ramp-up and reduces oscillations, but with the risk of overshooting the target temperature if not configured properly. See the Boost option below.
           * PID: A standard PID algorithm
         
             There is no "best" algorithm; it will essentialy depend on your setup and on your personnal taste. Test and experiment to see what fits best to you. The device monitor of the [NFirmwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) is a great tool to observe the behavior of the algorithms with different setups and parameters.  
             More algorithms may be implemented with time. Stay tuned.  

      * Boost
    
             A parameter controling the Boost algorithm (see above).  
             Set the limit, in percentage of the target temperature, at wich the Boost algorithm will stop speeding up the temperature ramp-up phase. The more the value, the faster the ramp-up time, but with a higher risk of overshooting the target temperature. High values are best with heavy resistances such as clapton builds, lower are prefered with light resistances such as single high gauge wire. Also, choosing the right power/wattage setup (not too high nor too low) will help prevent overshooting while keeping a fast ramp-up. You'll have to experiment to see what fits the best for your setup and personal taste.  
             A long fire resets the parameter to its default value.

      * P, I, D
      
            The 3 parameters controlling the Proportional, Integral and Derivative part of the PID algorithm. You should familiarize yourself with the PID algorithm before playing with those parameters. Many papers exist over the internet on the subject.  
            A long fire resets the parameter to its default value.  
            Default values are (600,850,0). They are general-purpose values that should work in most cases, but may not be perfect and need to be tuned for any particular setup.  
            Algorithm is implemented in its independant form.  
            Sample time (50Hz) is scaled to one for parameters uniformity.  
            Units: **P** is expressed in mW/°C, **I** in mW/°C/s, **D** in mW.s/°C.

    * Prot.

         Set the long fire protection time. Default is 10.0s.  
         Admissible values are 2.0s to 15.0s.

    * Vaped

         Choose the unit in wich the liquid consumption (on the 3rd info line of the main screen) is dislayed. You can choose between raw total ml, or consumption speed in ml/day. You must have setup the date and time of the RTC for the ml/d to behave correctly.

    * ml/kJ

         The speed at wich the liquid is vaporized in function of the energy dissipated by the coil. The value is in milliliters per KiloJoules. The proposed value of 360 is rtaher accurate for a standard 30W factory coil. It may need to be adjusted depending on your setup.  


----------

* Clock

    ![Clock Menu](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/clock_zpsmrvulkdd.png)
  * Set Date/Time

        Setup date and time.
        Fire change edited field, + and - set value, and long click on fire saves the setting. Since you need to click 2 seconds on the fire button to save, setup the time 2 seconds in advance if you want to achieve best accuracy. I know...

        There is some other ways to setup time:
        * use the ```evic-usb time``` command, if you installed the [python-evic](https://github.com/ClockSelect/python-evic "python-evic") command-line utility;
        * use the Configuration Editor of the [NFirmwareEditor 5.0](https://github.com/TBXin/NFirmwareEditor/releases); open the Configuration Editor and click "Upload settings" to upload the current time.
        * MicroSur's [FWUpdater](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1) also as a "Set current time" function.

        * Joyetech's official firmware updater also sets date & time when uploading a firmware.  

        
  * Fmt (date format)

        Toggle date display format between day.month.year (Western civilized world), month/day/year (US), day/month/year (Eastern civilized world), and year-month-day (Universe) format.

  * Size

        Toggle time display format of the digital dial between a small hh:mm:ss and a big HH:MM.

  * Dial

        Change the format of the clock display between a hand dial clock (A) and a digital clock with date and time (D). This will change the clock display on the main screen as well as on the screen saver, if the clock is chosen as screen saver.
  
  * Clk Adjust

        Adjust clock: for small adjustments to clock time + and - adjust clock by seconds. Fire to save. For VTwo owners, this option also adjusts the X32 frequency accordingly if done properly.

  * Clk Speed

        *This setting is useless on RTC-enabled boxes (VTwo/Dual, AIO, Basic, eGrip II)*  
        *This setting is only usefull if you have set the Light Sleep mode "OFF" in the Expert menu. See below the "LSL" setting in the Expert menu description. As long as the Light Sleep mode is "ON" (default setting), there is no real need to adjust the Clock Speed Ratio.*  
        Some boxes (VTC-Mini, Cuboid/Mini, Presa 75W, RX75, RX200S, RX2/3) does not have the needed 32kHz crystal soldered on the pcb to regulate the clock speed; so, another time source is used to drive the clock. Since its frequency is quite different, a clock speed ratio is needed to regulate the clock. As long as the box is awake (you're using it), the clock is regulated by the external 12.000MHz crystal, which is an accurate time source. Problem arises when the box enters sleep mode, since the crystal is switched off and the only clock source is the somewhat unreliable 10kHz internal oscillator of the processor. Clock drift mostly occurs when the box is switched off or sleeping.
    
        The procedure to adjust the clock speed ratio is as follow:
        * First, setup time accurately via the date/time menu or the "```evic-usb time```" command line.
        * Let the box enter sleep mode (leave it alone or switch it off) for several hours. I suggest you're doing this before sleeping yourself.
        * Awake the box and go into the "Clk Speed" menu; do not let the box enter sleep mode again in between, or you'll have to redo the whole thing.
        * Using + and - buttons, adjust the time shown on top of the screen to catch up the real time. The number shown in the center of the screen is the clock speed ratio; reasonable values should be around 32~34000.
        * Once you have adjusted the time to the real time, click fire to save the new clock speed ratio. Your clock should now be as accurate as it can. If not, try to repeat the procedure. Accuracy of a few seconds per day can be achieved this way.
  
----------

* Interface

    ![Interface menu screen](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/interface_zpshqw8kg89.png)

  * BatV% Off / % / V

        Display battery charge percentage or voltage next to the battery picture on main screen.

  * 1Watt On/Off

        Enable increment of power setting by plus ±1.0 Watt instead of ±0.1 Watts.

  * 1C5F On/Off

        Enable increment of temperature setting by ±1°C and ±5°F instead of ±5°C and ±10°F

  * Wake < > On/Off

        Enable/Disable box wake up from sleep mode when pressing the + or - button.

  * Font A/B

        Change the display font. You have two fonts available; choose what pleases you more. Choice is purely aesthetic. One may find the bolder font B more readable on some displays.

  * Temp °C/°F

        Choose once and for all your temperature unit, between °C and °F. Nobody needs to switch back and forth between standards. Moreover, Farenheit is not even a standard. Maybe I'll replace it by Kelvins one day. Changing temperature unit by wrapping around the temperature has been disabled.

  * PPwr On/Off
  
        *TEMP Mode Only*  
        Toggle Priority Power mode On/Off. In temperature control modes, this switches the power and the temperature display; the power is displayed at top of screen and is directly editable by the + and - keys just like in Power mode, and the temperature is displayed on the first of the 3 informations lines in the center of the screen. Temperature remains editable in Edit mode.  
        The purpose of this function is near the preheat function in PWR mode; once you have set your correct vape temperature (not burning your dry cotton), your comfort come from the heat rise speed. This feature let you adjust this parameter more easily.

  * Clicks
  
        ![Clicks Menu](http://i345.photobucket.com/albums/p374/ClockSelect/clicks_zpsoqy6ngvh.png)
        
        The Clicks submenu allows you to configure the action associated to a Fire button multi-clic. Configurable multi-clicks are double, triple and quadruple clicks.

        Possible actions are:
        * Nothing
        * Enter edit mode
        * Toggle display of info lines / clock on the main display
        * Toggle the "Priority Power" mode (basically, switches temperature and power on the main screen display in TC mode, see above "PPwr On/Off" doc), *OR* enters the Preheat menu if box is in POWER mode.
        * Switch to next vaping mode ( TC -> POWER -> BYPASS -> ... )
        * Switch box On or Off

        As a security feature, if none of the action are configured to be the "Edit" mode, the firmware will redefine the triple click action to "Edit" at next reset.

----------


* Expert

    Some advanced options.  
    Normal users should barely have anything to do with those options, and can live perfectly well without ever knowing they exist. I do not recommend "testing" those options just to "see what it does". Deny responsability of everything etc. You're supposed to have 18+, after all^^

  * USB:

        Choose between several USB modes:
     * HID: Normal operation mode - factory setting. Disables any other USB device than HID.
       HID is always active, even in the two others modes of operation. This feature permits communication between the box and firmware utilities.

     * COM: A virtual COM interface; mainly used for debugging with a COM terminal such as putty.
       May be usefull if you are developping your own version of the firmware.

     * DSK: A virtual drive to download the firmware file.
     Connect the box to a PC, and a disk device will appear with a "MYEVIC.BIN" file. This can be read and copied. This file is an encoded firmware file that can be uploaded to another device using evic-usb or the official Joyetech/Wismec firmware tool.

  * DBG
  
        Enables or disables the debug informations mode. Once the DBG option is set to "ON", Debug informations can be shown/hidden by clicking fire button four times. This option is OFF by default to prevent users to inadvertendly mess up their screen. It's of no interest if you're not developping your own version of the firmware.

  * X32

        Enables or disables usage of the X32 crystal of the PCB.  
        If this setting is "OFF", the firmware won't try to drive the Real-Time Clock with the 32.768kHz crystal. This may solve freezing issues on some malfunctionning boxes. This setting will be active at next reset.  
        After reset, if the box cannot use the X32 to drive the RTC, this option will be set back to "OFF" and the Light Sleep feature will be enabled (see "LSL" below).  
        *This option is useless and forced to "OFF" on boxes known not to have an X32 crystal. Those are all boxes for which the manufacturer did not enable the RTC feature, i.e: VTC-Mini, Cuboid/Mini, Presa 75W and RX series.*  

  * LSL
  
        Light Sleep mode.  
        *This setting is useless and forced to "OFF" on boxes with a X32 crystal (VTwo/Dual, AIO, Basic, eGrip II), and defaults to "ON" on other boxes.*  
        On boxes where the Real-Time Clock is emulated (like the VTC-Mini), by setting the Light Sleep mode "ON", the box continues to drive the Real-Time Clock with the external 12.000MHz Crystal instead of the internal LIRC oscillator when entering sleep mode. This makes the Clock far more accurate and eliminates the need for the Clock Speed ratio, at the cost of a greater battery consumption (estimated less than 50 mAh/day).  
        Using this setting, Clock accuracy is identical to those of real RTC boxes.

  * NFE
 
        *Warning: You can't use Joyetech's firmware updater while this option is "ON". It may aloso cause issues with third-party firmware management systems.*  
        Enables or disables partial compatibility with the [NFirmwareEditor 5.0+](https://github.com/TBXin/NFirmwareEditor/releases) myEvic Configuration Editor.  
        This option is disabled by default; set it to "ON" if you plan to use developpement features of the NFE. You'll have access to monitoring, screenshot, and COM terminal.  
 
  * SHR
  
        Shunt Resistance (in mΩ).  
        *Warning: This item is dangerous to your box. Messing with this parameter may cause overcurrent in the atomizer circuitry and lead to definitive box failure.*  
        This item let you edit the value of the Atomizer's circuit shunt resistance. This resistance is involved in every resistance/current/power measures and computation in the whole firmware. This value should only be changed if you know exactly what you are doing.  
        To reset the shunt value, select the SHR menu item and press the fire button during 2 seconds. This will revert the value to the default hardware setting.

  * UCH

        USB Battery Charging.  
        *Multi-cell boxes only. This option has no effect on single-cell boxes.*  
        Enables or disables battery charging via the USB port. The USB port is still usable for all other purposes: firmware management, debugging, etc.  
        If you have an external battery charger and want to take care of your batteries, it is recommended to switch off the USB charging feature to avoid potentialy unbalanced charges, or unadapted charge currents at end of charge.

  * BAT

        Battery model.  
        You can specify your battery brand and model among several ones: 25R, 30Q, HG2, HE4, VTC4, VTC5 and VTC6. "GEN" is the generic battery used by default and should be used for any other model of battery or for built-in battery.  

        Custom Battery:  
        By pressing the Fire button during two seconds while editing this option, the battery model changes to "CUS" (Custom Battery). The box will use the user-defined battery discharge curve and settings. Those informations can be edited using [NFirwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) discharge profile editor (see [NFirwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) documentation for more information).  
        *The NFE compatibility mode must be set to ON in the expert menu for NFE to recognize the box as compatible.*

        At the moment, battery model information is used by the firmware to compute a more accurate state-of-charge than the generic setting, and to avoid battery stress by limiting max Amp draw. Max Amps by battery model is based on Mooch's stress tests and are fairly reliable.  

        *On boxes with built-in battery (AIO, Basic, eGrip II, Cuboid Mini), this option should be kept on the "GEN" setting unless you've taken your box apart and manualy replaced the internal battery pack by something else.*  

  * BVO

        Battery Voltage Offset submenu  
        Corrective offset value of the battery voltage. Depending on your box, the displayed battery voltage may be off by a few tens of millivolts. It's usually not a concern, but it may make the box locking the vape too early (wasting some battery capacity) or too late (box resets due to low voltage when firing).  
        Use an external accurate voltmeter (your battery charger may do the job) to compare the displayed voltages on the box to the actual battery voltages, then adjust the displayed voltages with this item.  
        Range is -300 to +300 milliVolts by step of 10mV.  
        * On a single-cell box, only the first setting (B1) is significant. B2 and B3 are ignored.  
        * On a fixed dual-cells box, the two first settings (B1 & B2) are used, respectively, for the first and the second battery (depending on the box, you'll have to determine wich one wich with your voltmeter). B3 is ignored.  
        * On a mixed single/dual-cells box, B1 is used for the lone battery in single-cell setting, and B2 & B3 for the two cells in a dual-cell setting.  
        * On a triple-cell box, I let you guess.  


----------

### Build:
Follow [evic-sdk::Readme](https://github.com/ReservedField/evic-sdk/blob/master/README.md) instructions to:

  * Setup the environment

  * Install python-evic

  * Install and unpack nuvoton-sdk with the following changes:
    * Unpack into this project directory (evic-sdk itself is not required for this project)
    * When unpacking nuSDK unpack only ```Library``` directory. For example unpack ```M451 Series BSP CMSIS V3.01.001/Library``` from ```M451_Series_BSP_CMSIS_V3.01.001.zip``` to ```myevic/nuvoton-sdk/Library```

  * To build invoke: ```EVICSDK=. make```

----------

### Known quirks/issues:

  ![](http://imgs.xkcd.com/comics/fixing_problems.png)

Issues evolve fast; it's hard to maintain this section up to date.  
See the [opened issues page](https://github.com/ClockSelect/myevic/issues) on github to be kept informed.
