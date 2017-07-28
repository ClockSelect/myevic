### Build
Segui [evic-sdk::Readme](https://github.com/ReservedField/evic-sdk/blob/master/README.md) per le istruzioni riguardanti:

  * L'impostazione dell'environment.

  * Installazione di python-evic

  * Installazione e spacchettamento del nuvoton-sdk con i cambiamenti sotto riportati:
	* Spacchetta nella cartella del progetto (evic-sdk non è richiesto per questo progetto)
    * Quando si spacchetta il nuSDK, estrai solo la cartella ```Library```. Esempio:```M451 Series BSP CMSIS V3.01.001/Library``` da```M451_Series_BSP_CMSIS_V3.01.001.zip``` in ```myevic/nuvoton-sdk/Library```

  * Per dare via alla build invoca il comando: ```EVICSDK=. make```
-----

← Previous Page: [Behaviour Changes](behaviourchanges_it.md) --  Next Page: [Profiles](profiles_it.md)→
