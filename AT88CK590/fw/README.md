#Overview

The directory contains the source code for the firmware of the AT88CK490,AT88CK590, AT88CKECCROOT and AT88CKECCSIGNER
USB Dongles.  The development was done using Atmel Studio Framework (6.2).
In addtion to the firmware source code multiple project examples have been provided.  For additional information on the Atmel Cryptokits please go to [http://www.atmel.com/cryptokits](http://www.atmel.com/cryptokits)



 
###Development Kits

The development kits directories contains the firmware for the above mentioned USB dongles.  Note that the firmware is common between these four kits other than the actual kit names.  

The USB interface is implemented as a Human Interface Device for the USB Dongles.   As a reference a USB CDC driver implementation has also been provided. 

```
Primary Project Names: 
..\AT88CK590\DevelopmentKits\AT88CK590\CombinedLibraries\AtmelStudio6\CombinedLibraries\CombinedLibrariesHid.cproj
..\AT88CK590\DevelopmentKits\AT88CK590\CombinedLibraries\AtmelStudio6\CombinedLibraries\CombinedLibrariesCdc.cproj
```

###Libraries
The "Libraries" directories has the code for the SHA204,AES132 and ECC108.  It is recommended that you check the Atmel website for updated libraries for use with Atmel CryptoAuthentication products.


###LibraryExamples
The "LibraryExamples" directories has code for various example products.


### Compiling for the different USB Dongles
Changing the way the program compiles to assign the different Kit names is done by using defines.  The available defines are found in the combined_discover.h
```
Location of combined_discover.h
..\AT88CK590\DevelopmentKits\AT88CK490\CombinedLibraries\KitModules\combined_discover.h
```

###Firmware Upgrades
The latest version of the firmware is located in the "FirmwareFiles" directory.  All of the USB dongles are upgraded using the ATMEL FLIP utility.  Information on flip can be found at:  [http://www.atmel.com/tools/FLIP.aspx](http://www.atmel.com/tools/FLIP.aspx)