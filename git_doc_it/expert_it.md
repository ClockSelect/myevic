### Expert

Alcune imopstazioni avanzate.  
Gli utenti normali non hanno nulla a che fare con queste impostazioni e possono perfettamente vivere senza sapere che esistano. 
Non consigliamo pertanto alcun tipo di test con queste impostazioni. 
Per tanto decliniamo ogni responsabilità, d'altrone si suppone che l'utente sia maggiorenne.^^

  * __USB__

        Sceglie diverse modalità USB:
     * HID: Modalità operativa normale - impostazione di default. Disabilita qualsiasi device USB oltre l'HID.
       L'HID è sempre attiva anche con le altre due modalità operative. Questa funzione permette la comunicazione tra la box e le utilità firmware.

     * COM: Un'interfaccia com virtuale; utilizzata principalmente per il debug con un terminale COM come putty.
       Può essere utile se stai sviluppando una versione personalizzata del firmware.

     * DSK: Un drive virtuale per scaricare il file del firmware.
     Connettendo la box ad un PC, apparirà un dispositivo di archiviazione con il file MYEVIC.bin .

  * __DBG__
  
    Attiva o disattiva le informazioni di debug. Una volta impostata su ON le informazioni di debug possono essere mostrate/nascoste con 4 pressioni consecutive del tasto fire.

  * __X32__

    Attiva o disattiva l'utilizzo del cristallo X32 della PCB.  
	*Impostazione inutile per box che non hanno il supporto RTC.*

  * __LSL__
  
    Light Sleep mode.  
    *Impostazione inutile per box che hanno il supporto RTC.*  
    Utilizzando (su box senza RTC) quest'impostazione l'accuratezza dell'ora è uguale alle box con supporto RTC (al costo di 50mA/giorno).  
   

  * __NFE__
 
    *Attenzione: se quest'impostazione è abilitata non è possibile utilizzare altri firmware updater.*  
    Abilita o disabilita una parziale compatibilità con il [NFirmwareEditor 5.x](https://github.com/TBXin/NFirmwareEditor/releases) per l'editing delle configurazioni myEvic.  
    Si avrà accesso alle funzioni Monitoring, Screenshot, COM terminal ed al Custom Battery Profile editor.  
 
  * __SHR__
  
    Shunt Resistance (in mΩ). (real resistance in Ohm now)
    *ATTENZIONE: Quest'impostazione è dannosa per la tua box. Giocando con questi parametri si potrebbero causare sovraccarichi che porterebbero il circuito della vostra box alla morte definitiva.*  
    Questa voce permette la modifica del valore dell'atomizzatore relativo alla resistenza del circuito shunt. Questa resistenza è coinvolta in ogni computazione e misurazione relativa alle resistenze/corrente/potenza. 
	Non modificare questo valore se non sai cosa stai facendo.  
    Per resettare il valore di shunt, tieni premuto il tasto Fire per 2 secondi.

  * __UCH__

    USB Battery Charging.  
    *Solo per box Multi-Cell.*  
    Abilita o disabilita la ricarica via USB nonostante resti utilizzabile per altri scopi.

  * __BAT__

    Modello batteria.  
    Puoi specificare il modello di batteria che stai utilizzando tra i vari preset: 25R, 30Q, HG2, HE4, VTC4, VTC5 and VTC6. 
	"GEN" è il profilo generico utilizzato di default che comprende le batterie non presenti in lista e le built-in.  

    Custom Battery:  
    Tenendo premuto il tasto Fire per due secondi, si potrà accedere alla modifica del modello di batteria. 
	La voce verrà mutata in CUS confermando lo stato dell'operazione. 
	La box quindi utilizzerà i valori impostati dall'utente.
	Questi valori possono essere modificati tramite il discharge profile editor di [NFirwareEditor](https://github.com/TBXin/NFirmwareEditor/releases)  ( guarda la documentazione del [NFirwareEditor](https://github.com/TBXin/NFirmwareEditor/releases) per maggiori informazioni).  
    *La compatibilità con NFE deve essere impostata ad ON nel menù Expert.*


  * __BVO__

    Battery Voltage Offset submenu  
    Questa impostazione serve a correggere l'offset dei voltaggi delle batterie.  
    Utilizzando un voltimetro bisogna comparare i voltaggi con quelli della box.e regolarli come si conviene.  
    Il range è da -1.00 a +1.00 Volts per ogni step di 10mV.  
    * Sulle box a singola batteria solo la prima voce (B1) è rilevante.  
    * Sulle box dual battery, solo i primi due settaggi (B1 & B2) sono rilevanti, rispettivamente, per la prima e seconda batteria.
    * Sulle box "miste" (dual e single battery), B1 è utilizzata per la batteria in setup singolo, B2 & B3 per il setup in dual.  
    * Sulle box a tripla batteria B4 è ignorato.
    * Sulle box a quadrupla batteria... beh indovinate.  

-----

← Previous Page: [Interface](interface_it.md) 
