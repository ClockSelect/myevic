### Build
Follow [evic-sdk::Readme](https://github.com/ReservedField/evic-sdk/blob/master/README.md) instructions to:

  * Setup the environment

  * Install python-evic

  * Install and unpack nuvoton-sdk with the following changes:
    * Unpack into this project directory (evic-sdk itself is not required for this project)
    * When unpacking nuSDK unpack only ```Library``` directory. For example unpack ```M451 Series BSP CMSIS V3.01.001/Library``` from ```M451_Series_BSP_CMSIS_V3.01.001.zip``` to ```myevic/nuvoton-sdk/Library```

  * To build invoke: ```EVICSDK=. make```
-----

← Previous Page: [Behaviour Changes](behaviourchanges_en.md) --  Next Page: [Profiles](profiles_en.md)→
