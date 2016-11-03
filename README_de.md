## **myevic**
Dies ist die myeVic Custom Firmware.

### Kompatible Hardware:
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


### Basierend auf:
*Original VTC Mini 3.03 Firmware*

### Hauptfunktionen:
Das Erscheinungsbild der myevic Firmware ist mehr oder weniger identisch mit der originalen Firmware, so dass du nicht verloren gehen solltest.
Viele neue Funktionen und Optionen werden über das Menüsystem zur Verfügung gestellt; Ich empfehle, diese vorliegende Dokumentation vollständig zu lesen.

### Nutzung:

Die binäre Firmware-Datei ist im Unterverzeichnis **bin/** des Projekts enthalten.
Wenn Du nur an der Installation und Verwendung der Firmware interessiert sind, laden sie dir hier herunter: [bin/myevic](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin)
  
Sobald du die Firmware-Binärdatei heruntergeladen hast, kannst du:

- sie mit dem Firmware-Updater des Herstellers [myevic.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin) Wie bei einem normalen Firmware-Update von Joyetech oder Wismec auf die Box laden:
  - Lade das Firmware-Updater-Paket vom Hersteller herunter [Wismec](http://www.wismec.com/software/) oder [Joyetech](http://www.joyetech.com/mvr-software/), Je nach Marke und Modell deiner Box achte darauf, die richtige Software für die richtige Box deiner Marke und deines Modells zu wählen.
  - Entpacke das Update-Paket und starte die UpdateFirmware.exe (Joyetech) oder UpdateWismec.exe (Wismec).
  - Schließe deine Box per USB an 
    Vergewissere dich, dass du ein USB-Datenkabel verwenden. Einige billige Kabel sind nur zum laden geeignet. Wenn du dir nicht sicher bist, verwende das mitgelieferte Kabel. Danach sollte das Update-Fenster das Modell und die Version Ihrer Box anzeigen.
  - Klicke den "Update" Knopf; Ein Dateiauswahldialog wird geöffnet. Wähle dort die "myevic.bin" Datei.
  - Warte bis der Aktualisierungsprozess abgeschlossen ist.  
    Wenn etwas schiefgeht, versuche es erneut. Das Update schlägt manchmal fehl; Dies sollte kein Problem sein.
  - Genieße die Firmware.  
  
ODER  

- Für Linux/Cygwin Nutzer, verwende evic-usb um die [myevic.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin) auf deine Box zu laden, verwende den Befehl:

    ```evic-usb upload myevic.bin```

ODER  

- Verwende den [NFirmwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) oder den [FWUpdater](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1) welcher ein Teil vom [VTCFont](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1) Paket ist, um die [myevic.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin) auf deine Box zu laden.

### Danke an:

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

    Und vielen anderen, die dieses Projekt unterstützen und sich Zeit nehmen Fehler zu melden oder diese zu beheben.



----------
### Veränderungen ###
Es wurden einige Änderungen am Gesamtverhalten der Firmware im Vergleich zum Original vorgenommen. Dieser Absatz sollte von jedem neuen Benutzer gelesen werden, um zu vermeiden das es sich um einen "Bug" handeln könnte.

* Stealth Mode:  
  Der "Stealth Modus" wurde auf einige weitere Bildschirme erweitert.
  Der Akku Ladebildschirm erscheint im "Stealth Modus" nun nicht mehr; nur für ein paar Sekunden, wenn das USB Kabel angeschlossen wurde, um zu zeigen das alles richtig angeschlossen wurde.
  Der "Key Lock/UnLock" Screen wechselt zu einem Schwarzen Bildschirm, wenn die Tasten gesperrt sind.
  Der "Stealth On/Off" Screen wechselt zu einem Schwarzen Bildschirm, wenn der "Stealth Modus" an ist.

* Rechte (+) und Linke (-) Taste:
  Beim editieren einiger Werte, wird die Geschwindigkeit stufenweise erhöht Das drücken dieser Tasten weckt das Gerät nicht länger aus dem Sleep Modus auf, ausgenommen dieses wurde im Interface Menü so konfiguriert (siehe unten). Dies macht das Gerät Taschenfreundlicher.

* Temperatur Konfiguration im TEMP Modus:
  Die Temperatur wechselt nicht länger zwischen °C und °F; die Temperatureinheit wird im Interface Menü eingestellt. Standard ist °C.
  Die Temperatur Einstellung kann in ±1 °C oder ±5 °F erfolgen, wenn diese im Interface Menü (siehe unten) eingestellt ist.

* Feuertaste Mehrfach-Klick:
  Schnelles drücken (2, 3- oder 4-mal) auf die Feuertaste kann einen gewissen Effekt erzielen - siehe unten in der Interface Menü Beschreibung um diese zu sehen und wie sie einzustellen (oder zu deaktivieren) sind.
  10-maliges drücken um die Platinen Temperatur, oder 20-maliges um die Firmware Version anzeigen zu lassen, setzt nicht mehr voraus aus die Box ausgeschaltet ist.

* Coils Memory:
  Im POWER Modus, merkt sich die Box nun die Power Einstellungen für jeden gegebenen Wiederstand, bis zu 10 Verschiedene (so wie auch im SMART Modus). Dies bedeutet, wenn der Tanks gewechselt wird, setzt die Box automatisch die Leistung auf den vorher einstellten Wert für die Verwendete Coil.
  Coils können im Widerstand um ±10% variieren um noch erkannt zu werden, zwei unterschiedliche Coils innerhalb der 10% werden nicht unterschieden. Der Coil Speicher kann im Menüeintrag "Coils >> Manage >> Zero All" Menüeintrag (siehe unten) gelöscht werden.

* Bypass Modus:
  Die Leistung wird nun im oberen Display Bereich anstelle der Volt dargestellt, denn diese Information ist wichtiger als die Dampf Qualität. Die Echtzeit Ausgabe der Ausgangspannung ist stets als Option in der 3. Zeile (VOUT) verfügbar.
  Die vorhersagbaren Leistungen und Spannungen sind vor allem auch auf Multi-Akku-Boxen genauer. Es werden einige Züge benötigt, um die maximale Genauigkeit zu erreichen, damit die Box Zeit hat um das Batterieverhalten unter hoher Last abzuschätzen.

* Logo:
  Die myevic Firmware akzeptiert Logo Größen von 64 Pixels breite und von 40 bis 63 Pixel Höhe. Große Logos können teilweise Zeilen der Informationen in der Bildschirmmitte verdecken; Es liegt an dir das richtige Design und die richtige Größe zu finden damit das Display ordentlich aussieht.

* Menüs:
  Der Menü Zugriff erfolgt durch Drücken der Feuertaste und Rechten Taste, dieses wurde komplett überarbeitet. Siehe unten für die neue Menü Beschreibung.

----------
### Über Multi-Akku-Boxen


Im Moment stellt das Interface nur ein Akkusymbol da, entweder mit oder ohne Prozent/Spannung, als wenn nur ein Akku im Gerät wäre (diese könnte sich in Zukunft ändern).
Die dargestellten Werte sind die des Akkus mit dem *niedrigsten Ladezustand* aus dem Akku Set.
Ein Akku Set sollte immer Gleichgewicht sein. Eine Warnmeldung erscheint sobald ein Unterschied der Ladespannung zwischen der kleinsten und höchsten Spannung 0.3 Volt überschreitet.
Solange du keine Warnung siehst (ein blinkendes "balance" im Akkusymbol oder eine "imbalanced batteries" Warnmeldung), weißt du das die Spannung alle Akkus innerhalb von 300mV ist.


----------


### Haupt Display
* Doppel-Klick:

    Ein doppeltes klicken mit dem Feuertaster wechselt zwischen der Standard Ansicht und der Uhr Ansicht.
    Die Uhr Ansicht ist sehr nahe an der Ansicht der VTwo Box angelehnt; nur mMn besser.
    Um die Uhr einzustellen bitte unten im "Clock Menü" nachsehen.

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/mainscreen_zpsuoh0cthd.png) ![](http://i345.photobucket.com/albums/p374/ClockSelect/mainscreen3_zpsfunjeoct.png)

* Dritte Info-Zeile:

    Die 3. Info-Zeile auf den Haupt Display welche genutzt wird, um Ampere Puffs (Züge) oder Zugzeit anzuzeigen, kann jetzt noch mehr:

    - Verdampfer Spannung
    - Akku Spannung
    - Board Temperatur
    - Uhrzeit
    - Echtzeit Widerstand
    - Geschätzter Liquid Verbrauch in ml oder ml/Tag *(experimentell)*

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/mainscreen2_zpsclbvvdah.png)


----------


###Menüs:
Feuer und rechts zeitgleich für ein paar Sekunden drücken um das Menü zu öffnen.
Hier ist jetzt einiges mehr als nur das LOGO und Game Menü. Siehe unten.

* Es ist zu jeder Zeit möglich, das Menü durch gleichzeitiges drücken von Feuer und rechts zu verlassen und zum Haupt Display zurück zu kehren.
* Innerhalb der Untermenüs ist es möglich, durch gleichzeitiges drücken der linken und rechten Taste eine Ebene im Menü zurück zu gehen.

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/menus_zpsb8gftzok.png)  
Main Menü


----------


* Screen

    ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/screenmenu_zpsph9b8ono.png)

    Screen Management Menü:

  * Contrast:

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/contrast_zpsjuouc0v4.png)

         Die Standard Helligkeit der VTC liegt bei ungefähr 17%. Dieser Menüpunkt gibt dir Zugriff auf die Helligkeit des Displays.
 
 * Protection:

         ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/protec_zpsx0jd4aok.png)

         Konfiguriere die Zeit für den Bildschirmschoner und das Dimmen des Displays. Nach der eingestellten Zeit in der "Main" Option, (Standard sind 30 Sekunden) bei Inaktivität, wechselt das Display zur Screensaver Animation (siehe unten) für die in der "Saver" Option eingestellte Dauer, danach schaltet die Box ab.
         Wenn die "Saver" auf "Off" eingestellt ist, schaltet das Display nach der in der "Main" Option eingestellten Zeit ab und nach 3 Minuten schaltet die Box ab.

  * Saver:

         ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/saver_zpsdkyhj1mz.png)

         Wähle die Screensaver Animation. Du hast haben die Wahl zwischen "None" (schwarzes Display), der Uhr (Analog oder Digital je nach Einstellung im "Clock" Menü), 3D Objekten, einem Logo, einer Qix-like Animation, oder dem Snow Screen.
         Es können weitere folgen. Der Inhalt dieses Menüs kann sich von Zeit zu Zeit ändern; dieses Dokument kann nicht immer aktuell sein. Have a look.
         Wenn der 3D Screensaver aktiv ist, ist es möglich mit + und - das animierte Objekt zu wechseln.
         Über den Snow Screensaver: wähle diesen Screensaver, wenn du merkst das einige Pixel im Display eingebrannt sind; es ist normal bei Boxen welche einige Monate alt sind. Dieser kann dabei helfen die Pixel wiederherzustellen. Sieh dir in diesem Fall auch die "Invert" Option an.

  * Logo

         Das Logo Untermenü wird verwendet um ein eigenes Logo ein oder auszuschalten und um festzulegen wo dies angezeigt werden soll. Das Logo kann oben im Display (im Power/Temp Bereich) oder in der Mitte im Bereich der ersten zwei Informations Zeilen dargestellt werden.

  * Invert  
  
         ![](http://i345.photobucket.com/albums/p374/ClockSelect/invert_zpsyowemzqu.png)  

         Wechselt die Darstellung von weiß auf schwarz oder schwarz auf weiß.  
         Der Hintergedanke neben der Tatsache, dass es Cool Aussieht ist, dass es auch sinnvoll für die Box ist, wenn einige Pixel bereits im Display eingebrannt sind. Du kannst diese Option zusammen mit dem "Snow" Screensaver verwenden um die Qualität deines Displays wieder zu verbessern (siehe "Snow" im Screen -> Saver Untermenü).

  * Miscs  
      Untermenü
      * Game
      
            Flappy Bird. Original Menü.

      * Led  

            eGrip II LED Farbeinstellung.

      * 3D

            ![3D cube](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/cube_zpsf9apfiun.png)

            Coole drehende 3D Objekte.  
            Zurück ins Menü gehen und "None" auswählen um die Animation zu beenden.  
            Oder die Box zurücksetzten.  


----------

* Coils

    * Management:

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/coils_zpsn29ef1h5.png)
     * In jeder Teile des vier TC Modi:

            Drücke Feuer um den Widerstand zu bearbeiten.  
            Wenn der Widerstand markiert ist, drücke (+) und (-) um ihn zu bearbeiten.  
            Wenn du Feuer drückst, ohne den Widerstand zu bearbeiten, wird dieser gesperrt/entsperrt.  
            Bearbeiten des Widerstandes zu einem >0 Wert sperrt diesen automatisch.  
            Wird der Wiederstand mit (-) auf 0 gesetzt, wird dieser zurückgesetzt und entsperrt.  
               
            Du kannst ebenso auf einem der vier Widerstände Feuer für 2 Sekunden gedrückt halten um diesen zurück zu setzten; sollte der ausgewählte Widerstand zu einem verwendeten Modus gehören, wird dieser sofort vom Verdampfer neu ausgelesen und der neue Wert gesperrt.  
              
            Beim aufrufen des Coils Management Menüs, wird der entsprechende Widerstand für den aktuellen Modus (sofern verwendet) automatisch ausgewählt.
            
      * Zero All:

            Alle Widerstände löschen; setzt ebenfalls den für "SMART" und "POWER" gespeicherten Widerstand zurück.
            
       * Check: Yes/No
       
            *Wenn du glaubst, diese Funktion in einer anderen Situation als eine wirklich aufwändigen Wicklung mit niedrigem TCR-Wert (SS und so) verwenden zu müssen, hast du sicherlich ein Problem mit deiner Wicklung. Die Verwendung dieser Funktion kann riskant sein und wird daher nicht empfohlen. Überprüfe bitte zuerst deine Wicklung.*
  
            In den TC-Modi prüft die Box die Wicklung auf Eignung des Widerstandes beim ersten Einschalten (d.h. Jedes Mal, wenn der Verdampfer aufgeschraubt wird oder nach dem reset der Box). Der Test wird in einer Weise durchgeführt, der aufwändige Wicklungen mit einem niedrigem TCR-Wert durchfallen lassen kann (bei aufwändigen Wicklungen z.B. dual twisted clapton oder ähnlichen, mit mehr als etwa 1,7 Gramm Metall). Wenn eine Wicklung den ersten Test nicht besteht, erzwingt die Box den POWER Modus.

            Wenn du weißt das deine aufwändige Wicklung perfekt und für TC geeignet ist, wähle zuerst den paasenden Modus für deine Wicklung, gehe dann in dieses Menü und wähle "No" aus. Die Box wird jetzt den ersten für diese Wicklung überspringen.  
            Dieser Test wird wieder aktiviert sobald der Verdampfer entfert oder die Box zürück gesetzt wird.

            *Diesen Test deaktiviert zu lassen kann dich in ungewollte Situationen bringen. Nutze ihn vorsichtig.*    

    * TCR Set:

        ![](https://www.dropbox.com/s/n09iy9nu57jnv18/tcrset.png?dl=1)  

        TCR Set Menü.  
        *Dieses Menü ist ebenfalls bei ausgeschalteter Box durch 5 Sekunden langes drücken von Feuer und rechts (+) verfügbar.*  
        
        Zusätzlich zu den drei normalen TCR Modi (M1, M2 & M3), sind auch die Standard Werte der Box für TEMP NI, TI und SS editierbar. Um einen dieser drei voreingestellten Werte zu bearbeiten:
        * Wähle den zu bearbeitenden Wert aus,
        * Drücke lange Feuer um den Standard (DEF) Wert zu entsperren; ein annehmbarer Wert wird als Standard zum Bearbeiten verwendet,
        * Bearbeite den TCR Wert.
        * Langes Feuern stellt den Standard Wert (DEF) bei Bedarf wieder her.


----------
* Vaping

    * Preheat:
    
        ![TCR Set Menu](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/preheat_zpsrpcu0fml.png)
        
        Coil Preheat Funktion. Hier können Sie einstellen, wie lange die Coil und mit welcher Temperatur/ Watt Leistung vorheizen soll, wenn Sie die Feuertaste betätigen. Nach diesem Vorgang erhitzt der Coil wieder mit der Standard Einstellung. Setzen Sie die Einstellung auf “Null” um die Preheat Funktion wieder abzuschalten. Ein kleines "P" auf dem Hauptbildschirm erinnert Sie daran, dass die Preheat Funktion abgeschaltet ist. (Nur im "POWER" Modus). Diese Funktion kann nur im "POWER" und "SMART" Modus aktiviert werden; In allen anderen Modi wird diese ignoriert.

    * Modes

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/modes_zpslphwvqhh.png)

        Konfiguriere die Modi, die in Gebrauch sind oder nicht gebraucht werden.
        Klicke auf ein Menü Item, um es auf “Y” oder ”N” zu stellen, wenn du sehen willst, welche Vorschläge dir gegeben werden.

        Im Beispiel wollen wir den "SMART" Modus auf „N“ stellen. 

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
    
         Einstellungen der automatischen Abschaltungen bei zu langem drücken der Feuertaste. Standard sind 10.0 Sekunden.
         Einstellbar zwischen 2 und 15 Sekunden.

    * Vaped

         Wähle die Einheit in der dein Liquidverbrauch (in der dritten Zeile auf dem Haupt Display) angezeigt wird. Du kannst zwischen gesamt Verbrauch (ml) oder Verbrauchsgeschwindigkeit (ml/day) wählen. Für ml/d musst du für die korrekte Funktion Datum und Uhrzeit der RTC eingestellt haben.

    * ml/kJ

         Die Geschwindigkeit, bei der das Liquid in Abhängigkeit der Wicklung und der aufgebrachten Energie verdampft wird. Der Wert ist in Milliliter pro Kilojoules angeben. Der vorgeschlagene Wert von 360 ist für eine Standard-30W-Wicklung genau. Möglicherweise muss dieser passend für dein Setup angepasst werden.  


----------


* Clock

    ![Clock Menu](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/clock_zpsmrvulkdd.png)
  * Set Date/Time

        Einstellung von Datum/Uhrzeit
        Mit der Feuertaste das einzustellende Feld wählen, + und – zum Einstellen. Langes drücken der Feuertaste speichert die Einstellungen.
        Wenn die Feuertaste länger als 2 Sekunden gedrückt werden muss, gibt es noch andere Möglichkeiten.

        Hier ein paar Beispiele, wie die Uhr noch eingestellt werden kann:
        * Nutze den ```evic-usb time``` Befehl, wenn du die [python-evic](https://github.com/ClockSelect/python-evic "python-evic") installiert hast.     
        * Konfig. Editor vom [NFirmwareEditor 5.0](https://github.com/TBXin/NFirmwareEditor/releases); Öffne den Konfig. Editor und klicke auf "Upload Settings" um die richtige Zeit einzustellen.
        * MicroSur's [FWUpdater](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1) kann auch die richtige Uhrzeit einstellen.
 
        * Joyetech's offizieller Firmware Updater setzt die korrekte Zeit bei Installation einer neuen Firmware automatisch.
 
            
  * Fmt (Datumsformat)
        Ändert die Anzeige des Datumsformat Tag/Monat/Jahr(Standarteinstellung), Monat/Tag/Jahr ist das US Format.
 
  * Size
        Time Display Format stellt die Größe der anzuzeigenden Uhrzeit.
 
  * Dial
        Ändert die Anzeigeart der Uhr. Entweder “Armbanduhr” Format (A) oder digital Uhr mit Datum (D). Diese Einstellung ändert die Anzeige der Uhr sowohl auf dem Hauptbildschirm als auch im Screensaver, wenn die Uhr als Screensaver eingestellt wurde.
 
  * Clk Adjust
        Anpassen der Uhr: Für kleinere Änderungen der Uhrzeit nutze die + und – Tasten der Box. Feuertaste zum Speichern.
        Für Besitzer einer VTwo Box, diese Option stellt auch die” X32 Frequenz“ dementsprechend ein.
 
  * Clk Speed
 
        *Diese Funktion ist nur für Boxen mit RTC verfügbar. (VTwo/Dual, AIO, Basic, eGrip II)*
        *Diese Einstellung macht nur Sinn, wenn der Sleep Mode der Beleuchtung im Expertenmodus abgeschaltet wurde.
        Siehe auch "LSL" Einstellung in der Beschreibung des Experten Menüs. Steht der Light Sleep Mode auf „ON" (Standarteinstellung), gibt es eigentlich keinen Grund die „Clock Speed Ratio“ einzustellen.*
        
        Manche Boxen (VTC-Mini, Cuboid/Mini, Presa 75W, RX75, RX200S, RX2/3) haben nicht die 32kHz Funktion im PCB um die Geschwindigkeit der Uhr einzustellen; Solange diese Boxen in Gebrauch sind, regelt sich die Uhrzeit automatisch über den 12.000MHz Kern.
Probleme können auftreten, wenn die Box sich im Sleepmode befindet, dann schaltet sich der Kern ab und die Uhr bekommt nur noch ungenaue Werte vom internen 10kHz Oszillator des Prozessors.
Die Uhr wird erst ungenau, wenn die Box ausgeschaltet oder sich im Sleepmode befindet.
  
         Die Vorgehensweise um den „Clock speed ratio“ einzustellen, ist folgender:
             * Zuerst, stellt ihr Datum/Uhrzeit richtig über das Menü der Box ein oder nutzt die Kommandozeile von „evic-usb time“.
             * Lasst die Box in den Sleepmode gehen oder schaltet sie einfach für ein paar Stunden aus. Am besten macht ihr das, bevor ihr schlafen geht.
             * Schaltet die Box wieder ein und geht in das "Clk Speed" Menü; lasst die Box nicht wieder in den Sleepmode gehen, sonst müsst ihr wieder von vorne beginnen.
             * Nutzt die + und - Knöpfe, bis die Anzeige “Time” sich oben am Bildschirmrand befindet. Die Zahl die jetzt mittig im Display steht ist die gemessene „clock speed ratio“. Sie müsste irgendwo zwischen 32~34000 liegen.
             * Wenn ihr die richtige Zeit eingestellt habt, drückt die Feuertaste um die neue „clock speed ratio“ zu speichern. Eure Uhr sollte nun wieder richtig laufen. Falls nicht, wiederholt die Schritte. Wenn sie ein paar Sekunden vor oder nachgeht, ignoriert das einfach.
 
----------
 
* Interface
 
    ![Interface menu screen](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/interface_zpshqw8kg89.png)
 
  * BatV% Off / % / V
  
         Zeigt den Batteriestatus in Prozent an oder wechselt zur einfachen Batterieanzeige.
 
  * 1Watt On/Off
 
         Einstellung der zu erhöhenden Watt Zahl zwischen ±1.0 Watt und ±0.1 Watt.
 
  * 1C5F On/Off
 
         Einstellung des Temperaturbereiches zwischen ±1°C /±5°F und ±5°C oder ±10°F
 
  * Wake < > On/Off
 
         Aktivieren/Deaktivieren des Sleepmode durch Drücken der + oder – Taste.
 
  * Font A/B
          
         Ändert die Schriftart. Es stehen zwei Schriftarten zur Auswahl; Wähle die aus, die dir am besten gefällt. Die Wahl liegt ganz bei dir. Manche finden die 2.Schriftart auf div. Boxen besser lesbar.
 
  * Temp °C/°F
 
         Einmalige Einstellung des Temp. Bereiches zwischen °C und °F. Niemand springt zwischen den Bereichen hin oder her. Des Weiteren, ist Fahrenheit kein Standard. Vielleicht werde ich das bei Zeiten noch ändern.
 
  * PPwr On/Off
 
         *Nur im TEMP Modus* 

         Ein/ausschalten des bevorzugten Power Modus. Im TC Modus, ändert sich nur die Anzeige von Power und Temperatur. Der Powermodus steht oben im Display und ist direkt editierbar über die + und – Tasten sowie im Powermodus, und die Temperatur steht auf einer der 3 Infozeilen in der Mitte des Displays. Die Temp. lässt sich nur im Menü ändern.
 
         Diese Funktion ähnelt der Preheat Funktion im PWR Modus sobald du deine korrekte Dampftemperatur eingestellt hast (verbrenne nicht deine trockene Watte), die Freude steigt mit dem Wärmeanstieg. Diese Funktion macht das anpassen wesentlich einfacher.
 
  * Clicks
 
        ![Clicks Menu](http://i345.photobucket.com/albums/p374/ClockSelect/clicks_zpsoqy6ngvh.png)
            
        Das Click Untermenü erlaubt es dir einzustellen, was passieren soll wenn du den Feuerknopf mehrfach drückst. Konfiguriere z.B. Doppel, dreifach oder vierfache Klicks auf der Feuertaste.
 
        Mögliche Aktionen sind:
         * Nichts
         * Zugang zum Editier Modus.
         * Umschalten des Displays von Infozeilen zur Uhr.
         * Umschalten in den "Priority Power" Modus siehe dazu "PPwr On/Off".
         * Wechseln in den nächsten Dampfmodus ( TC -> POWER -> BYPASS -> ... )
         * Box Ein /-oder ausschalten.
 
----------
 
 
* Expert

    *Funktionen für Fortgeschrittene.*
    
        
  * USB:
 
     Auswahl zwischen verschiedenen USB Modi.
     * HID: Normaler Modus - Werkseinstellung. Deaktiviert alle USB Geräte außer HID.
       HID ist immer aktiv, auch in den anderen Modi. Diese Funktion erlaubt die Kommunikation zwischen Box und Update Software.
 
     * COM: Ein virtuelles COM Interface; stellt die Kommunikation zwischen Box und einem COM Interface her, wie z.B. Putty.
       Sehr hilfreich, wenn man seine eigene Version dieser Firmware erstellen möchte.
 
     * DSK: Ein virtuelles Laufwerk, um die Firmware der Box runterzuladen.
       Schließe die Box an den PC an und ein Laufwerk wird angezeigt mit dem Inhalt "MYEVIC.BIN". Diese kann gelesen oder kopiert werden. Diese Datei ist eine enkodierte Version der Firmware und kann auf eine andere Box hochgeladen werden, die evic-usb oder das offizielle Joyetech/Wismec Firmware Tool nutzt.
 
  * DBG
 
        Aktiviert oder deaktiviert den Debug Informations Modus. Wenn die DBG Option auf „ON" steht, können Debug Informationen durch mehrmaliges drücken der Feuertaste angezeigt oder ausgeblendet werden. Diese Funktion ist normalerweise deaktiviert. Sie ist für die Leute, die keine eigene Firmware erstellen wollen uninteressant.
 
  * X32
 
        Aktiviert oder deaktiviert die Nutzung des X32 Kerns des PCB. 
        Wenn diese Funktion deaktiviert ist, bekommt die Firmware nicht die richtige Uhrzeit über den 32.768kHz Kern. Dies kann in manchen Fällen zum Absturz der Box führen. Diese Funktion wird automatisch beim Reset wieder aktiviert.
        Wenn die Box nach einem Neustart nicht in der Lage ist mit dem X32 den RTC anzusprechen, schaltet sich diese Funktion automatisch in den “OFF” Modus und das “Light Sleep” Feature wird aktiviert. (siehe auch “LSL”). 
 
        *Dieses Feature ist sinnvoll für Boxen die keinen X32 Kern besitzen. Dies sind alle Boxen, die von Werk aus nicht den RTC Modus haben. Wie z.B.: VTC-Mini, Cuboid/Mini, Presa 75W und RX series.* 
 
  * LSL
 
        Light Sleep mode. 
        *Diese Einstellung kann auf Boxen mit X32 Kern nicht genutzt werden und steht dauerhaft auf “OFF”. (VTwo/Dual, AIO, Basic, eGrip II) Auf allen anderen Boxen steht die Einstellung auf “ON”*
        Bei Boxen wo die Echtzeit durch aktivieren des “Light Sleep” Modus nur emuliert wird (wie bei der VTC-Mini), holen sich diese Boxen anstatt die Echtzeit über den externen 12.000MHz Kern stattdessen diese über den interne LIRC Oszillator, wenn diese in den Sleep Mode geht. Dies führt dazu, dass die Uhr genauer ist und Einstellung über die Clock Speed ratio entfallen, auch eine bessere Akkuleistung erreicht man dadurch (meist nur 50 mAh/Tag). 
        Nutzt man diese Einstellung, läuft die Uhr genauso gut wie auf echten RTC Boxen.
 
  * NFE
 
        *Achtung: Du kannst den Joyetech Firmware Updater nicht nutzen, solange diese Funktion aktiviert ist. Es führt zu Problemen mit anderen Firmware Management Systemen.*
        Aktiviere oder deaktiviere eine Teil Kompatibilität über den [NFirmwareEditor 5.0+](https://github.com/TBXin/NFirmwareEditor/releases) oder den myEvic Konfig. Editor.
        Diese Option ist werkseitig deaktiviert; setze sie auf "ON" wenn du vorhast, NFE Entwickler Features zu nutzen. Du bekommst dann Zugang zur Überwachung, Screenshots und dem COM Terminal.
 
  * SHR
  
        Shunt Resistance (in mΩ). 
        *Warnung: Dieser Punkt ist für deine Box gefährlich. Wenn du nicht weißt, was du tust, kann deine Box hierdurch irreparable Schäden bekommen.*
        Hierüber kannst du den Widerstand des Verdampfers einstellen.
        Dieser Widerstand wird in allen Bereichen der FW genutzt.
        Dieser sollte nur geändert werden, wenn du genau weißt was du tust.
        Um Einstellungen zurückzusetzen, gehe in das “SHR” Menü und drücke 2 Sekunden lang die Feuertaste.
 
  * UCH
 
        Aufladen der Akkus via USB. 
        *Die gilt nur für Mehr-Akku Boxern. Diese Funktion hat keine Auswirkung auf Boxen mit nur einem Akku.*
        Aktiviert oder deaktiviert das laden via USB. Der USB Port bleibt für alles andere wie z.B. Updates usw. aktiviert.
        Wenn du ein externes Ladegerät nutzt und deine Akkus etwas schützen willst, ist es ratsam die USB Ladefunktion zu deaktivieren, um fehlerhaftes oder ungleichmäßiges laden zu verhindern.
 
  * BAT
 
        Batterie Model. 
        Du kannst Marke und Model deiner Akkus einstellen: 25R, 30Q, HG2, HE4, VTC4, VTC5 und VTC6. "GEN" ist eine Standarteinstellung und sollte bei allen anderen Akku Modellen genutzt werden.
        Zu Beginn nutzt die Firmware die Einstellung “GEN” um die Akkus so gut es geht zu schonen.
 
  * BVO
 
        Batteriespannungs Untermenü 
        Zum Korrigieren der Batteriepsannung. Je nach Box kann die Batteriespannung variieren. Dies hat den Nachteil, dass manche Boxen zu früh oder zu spät abschalten.
        Nutze einen externen Spannungsprüfer um die genaue Spannung mit der, die auf der Box angezeigt wird zu vergleichen. Danach kannst du dann in diesem Menüpunkt die Spannung der Akkus einstellen.
        Der Messbereich liegt zwischen -300 bis +300 Millivolt in 10mV Schritten. 
        * Auf Boxen mit nur einem Akku ist nur die erste Einstellung (B1) interessant, B2 und B3 kann ignoriert werden.
        * Auf einer Box mit Zwei Akkus sind die ersten beiden Einstellungen (B1 & B2) in Nutzung, für z.B. den ersten und den Zweiten. Hierüber kannst du die Spannung die du über den Spannungsmesser gemessen hast nachregulieren. B3 kann ignoriert werden. 
        * Auf einer Box mit ein oder zwei Akkubetrieb, wird B1 nur im ein Akku Betrieb genutzt und B2 & B3 im Mehr Akku Betrieb.
        * Auf Boxen mit drei Akku Betrieb könnt ihr es vielleicht schon denken.
 
 
----------
 
### Build:
Folgt den Anweisungen [evic-sdk::Readme](https://github.com/ReservedField/evic-sdk/blob/master/README.md) :
 
  * Setup the environment
 
  * Installiert python-evic
 
  * Entpackt und installiert nuvoton-sdk mit den folgenden Änderungen:
    * Entpackt es in diesen Projekt Ordner (evic-sdk selbst wird bei diesem Projekt nicht gebraucht)
    * Wenn ihr nuvotonSDK entpackt, braucht ihr nur den Ordner ```Library```. Z.b. entpackt ihr nur den Ordner ```M451 Series BSP CMSIS V3.01.001/Library``` aus dem ```M451_Series_BSP_CMSIS_V3.01.001.zip``` nach ```myevic/nuvoton-sdk/Library```
 
  * Dann führt folgenden Befehl aus: ```EVICSDK=. make```
 
----------
 
### Bekannte Probleme:
 
  ![](http://imgs.xkcd.com/comics/fixing_problems.png)
 
Probleme entwickeln sich schnell; Es ist schwer alles immer up to date zu halten 
Siehe auch [geöffnete Seite für Probleme](https://github.com/ClockSelect/myevic/issues) auf GitHub, um immer up to date zu bleiben.
