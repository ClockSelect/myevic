### Hardware Compatibili
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
* Altre
  * Vaponaute La Petite Box
  
### Utilizzo:

Il binario del firmware è incluso nella cartella **bin/** di questo progetto.  
Se il tuo unico interesse è installare il firmware, scaricalo qui: [bin/myevic](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin)
  
Una volta scaricato il binario del firmware puoi:

- Usare l'utilità del produttore della tua box per installare il firmware [myevic.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin) come faresti con qualunque aggiornamento ufficiale:  
  
oppure  

- Per gli utilizzatori di linux/Cygwin, si può utilizzare evic-usb per l'installazione del firmware [myevic.bin](https://github.com/ClockSelect/myevic/blob/master/bin/myevic.bin) utilizzando la riga di comando:

    ```evic-usb upload myevic.bin```

o ancora 

- Utilizzare il [NFirmwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) oppure il [FWUpdater](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1) parte del pacchetto [VTCFont](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1).

### A proposito delle box con più batterie 

Al momento, l'interfaccia mostra solamente un'icona della batteria (percentile o voltaggio) come se ci fosse una singola batteria inserita.
I valori mostrati, sono quelli della batteria che si trova nello *stato di carica più basso*.
Un set di batterie dovrebbe essere sempre in equilibrio e bilanciato, qualora non lo fosse verrà mostrato un avvertimento.A battery set should always be kept in equilibrium and a balance warning is issued if the difference between the lowest and the highest voltage exceeds 0.3 Volts.  

-----

Next Page: [Profiles](behaviourchanges_it.md)→
