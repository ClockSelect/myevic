### Compatible hardwares
* Joyetech
  * eVic VTC Mini
  * eVic VTC Dual
  * eVic VTwo Mini
  * eVic VTwo
  * eVic AIO
  * eVic Basic
  * eGrip II / Light
  * Cuboid
  * Cuboid Mini
  * Cuboid 200
* Wismec
  * Presa TC75W
  * Presa TC100W
  * Reuleaux RX75
  * Reuleaux RX200S
  * Reuleaux RX2/3
  * Reuleaux RXmini
  * Reuleaux RX300
  * Sinuous P228
* Others
  * Vaponaute La Petite Box
  
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

### About Multi-cells boxes

At the moment, the interface only display one battery icon, percentage or voltage, just like there where only a single battery in the box (this may change in the future).  
The displayed values are those of the battery that is in the *lowest state-of-charge* of the set.  
A battery set should always be kept in equilibrium and a balance warning is issued if the difference between the lowest and the highest voltage exceeds 0.3 Volts.  
So as long as you have no warning (a blinking "balance" on the battery icon or an "imbalanced batteries" screen warning), you know that all batteries are within 300mV of the first.

-----

Next Page: [Behaviour Changes](behaviourchanges_en.md)→
