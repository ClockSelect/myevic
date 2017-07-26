### Clock
![Clock Menu](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/clock_zpsmrvulkdd.png)

* __Set Date/Time__

  Setup date and time.
  Fire change edited field, + and - set value, and long click on fire saves the setting. Since you need to click 2 seconds on the fire button to save, setup the time 2 seconds in advance if you want to achieve best accuracy. I know...

  There is some other ways to setup time:
  * use the ```evic-usb time``` command, if you installed the [python-evic](https://github.com/ClockSelect/python-evic "python-evic") command-line utility;
  * use the Time synchronization function of the [NToolbox](https://github.com/TBXin/NFirmwareEditor/releases) of the NFirmwareEditor 6.0+; (check the time synch feature in the system tray).
  * MicroSur's [FWUpdater](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1) also as a "Set current time" function.
  * Joyetech's official firmware updater also sets date & time when uploading a firmware.  
  
  
  
* __Fmt (date format)__

  Toggle date display format between day.month.year (Western civilized world), month/day/year (US), day/month/year (Eastern civilized world), and year-month-day (Universe) format.

  * __Size__

    Toggle time display format of the digital dial between a small hh:mm:ss and a big HH:MM.

  * __Dial__

    Change the format of the clock display between a hand dial clock (A) and a digital clock with date and time (D). This will change the clock display on the main screen as well as on the screen saver, if the clock is chosen as screen saver.
  
  * __Clk Adjust__

    Adjust clock: for small adjustments to clock time + and - adjust clock by seconds. Fire to save. For VTwo owners, this option also adjusts the X32 frequency accordingly if done properly.

  * __Clk Speed__

    *This setting is useless on RTC-enabled boxes (VTwo/Dual, AIO, Basic, eGrip II)*  
    *This setting is only usefull if you have set the Light Sleep mode "OFF" in the Expert menu. See below the "LSL" setting in the Expert menu description. As long as the Light Sleep mode is "ON" (default setting), there is no real need to adjust the Clock Speed Ratio.*  
  
    Some boxes (VTC-Mini, Cuboid/Mini, Presa 75W, RX75, RX200S, RX2/3) does not have the needed 32kHz crystal soldered on the pcb to regulate the clock speed; so, another time source is used to drive the clock. Since its frequency is quite different, a clock speed ratio is needed to regulate the clock. As long as the box is awake (you're using it), the clock is regulated by the external 12.000MHz crystal, which is an accurate time source. Problem arises when the box enters sleep mode, since the crystal is switched off and the only clock source is the somewhat unreliable 10kHz internal oscillator of the processor. Clock drift mostly occurs when the box is switched off or sleeping.
    
    The procedure to adjust the clock speed ratio is as follow:
      * First, setup time accurately via the date/time menu or the "```evic-usb time```" command line.
      * Let the box enter sleep mode (leave it alone or switch it off) for several hours. I suggest you're doing this before sleeping yourself.
      * Awake the box and go into the "Clk Speed" menu; do not let the box enter sleep mode again in between, or you'll have to redo the whole thing.
      * Using + and - buttons, adjust the time shown on top of the screen to catch up the real time. The number shown in the center of the screen is the clock speed ratio; reasonable values should be around 32~34000.
      * Once you have adjusted the time to the real time, click fire to save the new clock speed ratio. Your clock should now be as accurate as it can. If not, try to repeat the procedure. Accuracy of a few seconds per day can be achieved this way.
  
-----

← Previous Page: [Vaping](vaping_en.md) --  Next Page: [Interface](interface_en.md)→
