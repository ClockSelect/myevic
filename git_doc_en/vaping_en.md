### Vaping  
 
 
 ![Vaping menu](https://www.dropbox.com/s/5nafopmyhl6m8as/vaping2.png?dl=1)  

* __Preheat__
    
 ![TCR Set Menu](https://www.dropbox.com/s/d1ncqa9heec6grx/preheat1.png?dl=1)
        
   __Coil preheat function.__  
        
 *This function can only be active in "POWER" and "SMART" modes; it is ignored in all other modes.*  
        Set at which power ( **Pwr** ) and how long ( **Time** ) you want the coil to be preheated when you fire; after this delay, the atomizer will be powered by your regular setting. Set the **Time** to zero to switch preheating off.  
        The **Unit** may be absolute Watts or percents of the main power setting.   
        The **Delay** allows to configure a dead time after fire, during which the preheating function will be disabled. This dead time goes from zero (Off) to 3 minutes. The delay runs from the end of fire. Use this feature if you use high inertia (heavy) builds to prevent over-heating when chain-vaping. The **Delay** also apply on power curves, preventing power to fire over 100% of the preset power within the given time.  
        A small **P** icon will appear on the main screen next to the the power setting to remind you the preheat function is enabled ("POWER" mode only). A blinking **P** indicates the dead time period.  
        *If you configured a "PPwr" action in the multi-clicks configuration (see **Interface** menu), this action will pop-up this menu when in "POWER" or "SMART" mode.*  

   * __Modes__

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/modes_zpslphwvqhh.png)

 Configure used/useless modes.  
         Click on each menu item to set 'Y' or 'N' if you want to see any given mode proposed while cycling between modes in menu mode.  
         Typically, you'll set "SMART" mode to 'N'. Typically.

   * __Algo__
    
*Temperature Control only*  
         TC Algorithms submenu  

![Algo submenu](https://www.dropbox.com/s/ecy9oqapftnfwh3/algo.png?dl=1)  

* Algo  
         Choose betwen several algorithms for temperature control.
           * Off: use the default standard agorithm from stock firmware.
           * Sweet: much like the default, but smoothes out oscillations in regulation phase.
           * Boost: speeds up temperature ramp-up and reduces oscillations, but with the risk of overshooting the target temperature if not configured properly. See the Boost option below.
           * PID: A standard PID algorithm
         
There is no "best" algorithm; it will essentialy depend on your setup and on your personnal taste. Test and experiment to see what fits best to you. The device monitor of the [NFirmwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) is a great tool to observe the behavior of the algorithms with different setups and parameters.  
More algorithms may be implemented with time. Stay tuned.  

* __Boost__
    
 A parameter controling the Boost algorithm (see above).  
        Set the limit, in percentage of the target temperature, at wich the Boost algorithm will stop speeding up the temperature ramp-up phase. The more the value, the faster the ramp-up time, but with a higher risk of overshooting the target temperature. High values are best with heavy resistances such as clapton builds, lower are prefered with light resistances such as single high gauge wire. Also, choosing the right power/wattage setup (not too high nor too low) will help prevent overshooting while keeping a fast ramp-up. You'll have to experiment to see what fits the best for your setup and personal taste.  
        A long fire resets the parameter to its default value.

* __P, I, D__
      
 The 3 parameters controlling the Proportional, Integral and Derivative part of the PID algorithm. You should familiarize yourself with the PID algorithm before playing with those parameters. Many papers exist over the internet on the subject.  
        A long fire resets the parameter to its default value.  
        Default values are (600,850,0). They are general-purpose values that should work in most cases, but may not be perfect and need to be tuned for any particular setup.  
        Algorithm is implemented in its independant form.  
        Sample time (50Hz) is scaled to one for parameters uniformity.  
        Units: **P** is expressed in mW/°C, **I** in mW/°C/s, **D** in mW.s/°C.

* __Curve__

 *POWER/SMART mode only.*        
        Enable, Reset and/or Edit the atomizer power curve.  
        When power curve is enabled, it take precedence over preheating; a little "C" next to the power setting on main screen indicates the power curve is enabled.  
        The values of the power curve are percentages (from 0 to 200%) of the main power setting. The whole power curve will thus scale with the main power setting. The default power curve (after reset) is a flat 100% curve, so will have no sensible effect.  
        The **Delay** setting prevents the power curve to apply a power above 100% during some time after the end of the last puff. See the **Vaping>Preheat** menu documentation for more informations.  
        You may have one different power curve per configuration profile.  

* __Prot.__

 Set the long fire protection time. Default is 10.0s.  
        Admissible values are 2.0s to 15.0s.

* __Vaped__

Choose the unit in wich the liquid consumption (on the 3rd info line of the main screen) is dislayed. You can choose between raw total ml, or consumption speed in ml/day. You must have setup the date and time of the RTC for the ml/d to behave correctly.

* __ml/kJ__

The speed at wich the liquid is vaporized in function of the energy dissipated by the coil. The value is in milliliters per KiloJoules. The proposed value of 360 is rtaher accurate for a standard 30W factory coil. It may need to be adjusted depending on your setup.  

-----

← Previous Page: [Coils](coils_en.md) --  Next Page: [Clock](clock_en.md)→
