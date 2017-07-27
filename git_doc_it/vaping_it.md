### Vaping  
 
 
 ![Vaping menu](https://www.dropbox.com/s/5nafopmyhl6m8as/vaping2.png?dl=1)  

* __Preheat__
    
 ![TCR Set Menu](https://www.dropbox.com/s/d1ncqa9heec6grx/preheat1.png?dl=1)
        
   __Coil preheat function.__  
        
 *Questa funzione può essere attivata solo nella modalità "POWER" ed in quella "SMART"; 
        Imposta a quale wattaggio ( **Pwr** ) e per quanto tempo ( **Time** ) si vuole che la coil venga pre-riscaldata quando premi il tasto fire; dopo questo ritardo, l'atomizzatore sarà poi alimentato con le tue regolari impostazioni. Imposta il **Time** a zero per disattivare la funzione di preheating.  
        La **Unit** in cui potrai impostare il preriscaldamento sono: Watt or percentuali.   
        Il **Delay** permette di configurare un tempo morto dopo il tasto fire, durante il quale la funzione di preheating sarà disabilitata. Questo tempo morto può essere impostato a zero (off) a 3 minuti. Il ritardo ha luogo a pressione rimossa dal tasto Fire. Usa questa funzione se hai un'alta inerzia nella tua build (coil pesanti) per prevenire l'over-heating quando si esegue del chain-vaping. Il **Delay** si applica anche alle power curves.  
        Una piccola **P** apparirà sulla schermata principale vicino alle impostazioni dei watt per ricordarvi della funzione attiva. A blinking **P** indicates the dead time period.  
        *Se hai configurato un'azione relativa alla "PPwr" alla pressione multipla del tasto fire, verrà mostrato questo menù nella POWER/SMART mode.*  

   * __Modes__

        ![](http://i345.photobucket.com/albums/p374/ClockSelect/eVic/modes_zpslphwvqhh.png)

 Configura le modalità che usi oppure no..  
         Puoi cliccare su ogni elemento del menù affinché tu possa selezionare l'opzione Y o N per rendere una modalità visibile o non. 

   * __Algo__
    
*Solo in Temperature Control *  
         Sotto menù per gli algoritmi in TC 

![Algo submenu](https://www.dropbox.com/s/ecy9oqapftnfwh3/algo.png?dl=1)  

* Algo  
         Scegli tra i diversi algoritmi:
           * Off: utilizza l'algoritmo di default (lo stesso è presente sul firmware ufficiale)
           * Sweet: simile a quello di default, ma ammorbidisce le oscillazioni in fase di regolazione.
           * Boost: aumenta la velocità con la quale la temperatura sale riducendo le oscillazioni, ma con il rischio di andare oltre il target configurato di temperature se non settato correttamente. 
           * PID: Algoritmo PID standard
         
Per definizione non c'è un algoritmo migliore. Dipende semplicemente dalle vostre build e dal vostro gusto.  

* __Boost__
    
 Un parametro che controlla l'algoritmo Boost.   
        Imposta il limite, in percentuale, al quale l'algoritmo boost smetterà di velocizzare la fase dell'incremento di temperatura.
        Una lunga pressione del tasto fire resetta il parametro al suo valore di default. 

* __P, I, D__
      
 I 3 parametri controllano la parte Proporzionale, Integrale e Derivativa dell'algoritmo PID. 
        Una lunga pressione del tasto fire resetta il parametro al suo valore di default.   
        I valori di defult sono (600,850,0).  
        L'algoritmo è implementato nella forma indipendente.  
        Il tempo di campionamento (50Hz) è scalato a uno per uniformità dei parametri.  
        Unità: **P** è espresso in mW/°C, **I** in mW/°C/s, **D** in mW.s/°C.

* __Curve__

 *Solo in modalità POWER/SMART.*        
        Abilita, resetta e/o modifica la curva di potenza dell'atomizzatore.  
        Quando la power curve è attivata, avrà priorità sulla funzione di preheating; una piccola C verrà mostrata per confermare l'operatività della curva.
        I valori ammessi nella power curve sono in percentuale (da 0 a 200%). L'intera power curve sarà in scala ai valori precedentemente impostati (watt). The default power curve (after reset) is a flat 100% curve, so will have no sensible effect.  
        L'impostazione del **Delay** previene che la power curve applichi una potenza superiore al 100%.  
        Possono essere definite diverse power cure per ogni profilo di configurazione.  

* __Prot.__

 Imposta la durata massima di un tiro.  
        I valori ammissibili sono tra 2.0s e 15.0s.

* __Vaped__

Sceglie l'unità nella quale viene mostrata il consumo di liquido. Si può scegliere tra un  Totale ml oppure ml/day. 

* __ml/kJ__

Indica la velocità con cui il liquido viene vaporizzato in funzione dell'energia dissipata dalla coil. Il valore è in millimetri per KiloJoules. Il valore proposto di 360 è accurato per una coil standard da 30W.

-----

← Previous Page: [Coils](coils_it.md) --  Next Page: [Clock](clock_it.md)→
