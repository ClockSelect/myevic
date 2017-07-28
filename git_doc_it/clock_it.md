### Clock
![Clock Menu](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/clock_zpsmrvulkdd.png)

* __Set Date/Time__

  Imposta data ed ora.
  La pressione del tasto fire cambia il campo da modificare ed i tasti + e - impostano il valore. Una pressione prolungata del tasto Fire salva le impostazioni correnti.

  Ci sono anche altri modi per impostarli:
  * usando il comando ```evic-usb time``` , se hai installato l'utilità da riga di comando [python-evic](https://github.com/ClockSelect/python-evic "python-evic");
  * usando la funziona di sincronizzazione della [NToolbox](https://github.com/TBXin/NFirmwareEditor/releases) o NFirmwareEditor 6.0+; (controlla la funzione di sincronizzazione nell'icona posizionata nella tray).
  * usando MicroSur's [FWUpdater](https://www.dropbox.com/s/83zd19gu05pl3r6/EvicVTCFont.rar?dl=1) con la funzione "Set current time".
  * usando il firmware updater della Joyetech.  
  
  
  
* __Fmt (date format)__

  Serve a modificare il formato della data tra i modelli selezionabili. (day.month.year | month/day/year | day/month/year | year-month-day)

  * __Size__

    Serve a modificare la grandezza del formato dell'ora (hh:mm:ss | HH:MM)

  * __Dial__

    Serve a cambiare il formato dell'orologio. (A) analogico, (D) digitale. 
  
  * __Clk Adjust__

    Serve a mettere a punto l'orologio permettendo di modificarne i secondi. Il tasto Fire salva le modifiche. Per i possessori della VTwo questa impostazione mette a punto anche la frequenza X32 se fatta in modo opportuno.

  * __Clk Speed__

    *Questa impostazione è inutile su box con RTC. (VTwo/Dual, AIO, Basic, eGrip II)*  
    *Questa impostazione è utile solamente nel caso in cui Light Sleep mode fosse impostata ad "OFF" nel menù Expert.*  
      
    La procedura per regolare il rapporto di velocità dell'orologio è la seguente:
      * Imposta l'orologio tramite il menù date/time oppure con la command-line tramite il comando"```evic-usb time```" .
      * Lascia che la box entri nella sleep mode per alcune ore.
      * Accendi la box e vai nel menù "Clk Speed"; non lasciare che la box entri in modalità sleep, altrimenti dovrai ricominciare tutto il processo.
      * Utilizzando i pulsanti + e -, regola l'orologio che vedi in alto per sincronizzarlo con l'ora reale. I valori si attestano tra 32 e 34000 circa.
      * Una volta fatto ciò, premi il tasto fire per confermare l'operazione.
  
-----

← Previous Page: [Vaping](vaping_it.md) --  Next Page: [Interface](interface_it.md)→
