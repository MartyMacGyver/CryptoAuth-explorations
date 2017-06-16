******************************************************************************
			Work Space and Project Structure
******************************************************************************

The CryptoMemory line of development kits has three members.
1.	Aris+ that support CM / CMC.
2.	Keen+ that supports CRF / CMC.
3.	AT88CK201STK that supports CRF / CMC and runs on the Microbase board.

The fact that these kits share in many parts the same source code
is reflected in the folder structure of this work space.
ArisPlus, KeenPlus, AT88CK201STK, SourceCommon, and SourceCommonCrf reside on 
the same level. ArisPlus and KeenPlus contain make outputs and the make file. 
All the kits share the library source that resides in Libraries\CmCrfLibrary.

Folder hierarchy:
  CryptoToolset
    DevelopmentKits
      ArisPlus
      KeenPlus
      AT88CK201STK
      SourceCommon (USB, UART, Parser)
      SourceCommonCrf (Bowline, SPI, TWI)
    Libraries
      CmCrfLibrary
        CMC
        SRC


******************************************************************************
																		Tools
******************************************************************************

						compiler suite:
WinAVR 20090313

						IDEs:
AVR Studio 4.18, Build 684
