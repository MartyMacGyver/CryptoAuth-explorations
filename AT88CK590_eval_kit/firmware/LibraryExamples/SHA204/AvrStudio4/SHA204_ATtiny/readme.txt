This application is meant to work with a host script / application that talks RS232, 9600 baud, 1 start, 1 stop, 1 parity.
The protocol is <count including this byte><data><two-byte crc>. The current implementation does not calculate or check the CRC but just sends two zeroes.

The host PC has to do the following in order to use a host SHA204 attached to this ATtiny CPU along with a client SHA204 attached to a Crypto development kit:
- Wakeup the client SHA204.
- Send a Wakeup pulse.
- Send a Mac command and store its response.
- Send a Sleep sequence.
- Send the Mac challenge and response in one Tiny protocol packet to the Tiny and receive its response.
- The Tiny response should be the SHA204 response (<count = 4><status = 0 (success) or 1 (fail)><crc msb><crc lsb>). 

An IronPython GUI script is implemented that does the following:
- Finds a Crypto development kit that contains a SHA204 ("Find Microbase" button).
- If found, the SHA204 button in the main window gets enabled.
- After pressing this button, the SHA204 window is shown. It contains a.o. a menu item "Tiny".
- Selecting the menu item "Find" under "Tiny" verifies the RS232 communication on COM1 and instantiates the RS232 communication object. The Tiny firmware version is returned. The STK500 has to be connected through a serial cable between the RS232 SPARE connector and the host PC.
- Selecting "Mac" issues a Mac command for the client SHA204 and receives its response.
- Selecting "CheckMac" sends the challenge / response data from the previous Mac command to the Tiny which should return a four-byte status packet.

The firmware was tested on an ATtiny85 and an ATtiny44 plugged into an STK500. For the host SHA204 a Security Xplained extension board was used.

    ATtiny85:
MCU plugged into STK500 MCU connector SCKT3400D1.
STK500 PB0 - Security Xplained J104 pin 1: SDA
STK500 PB1 - STK500 RS232 SPARE: UART TX
STK500 PB2 - Security Xplained J104 pin 2: SCL
STK500 PB3 - STK500 RS232 SPARE: UART RX
STK500 PB5 - STK500 PE3: debugWire reset

    ATtiny44:
The STK500 does not have a socket for this MCU. Therefore, the MCU has to be wired through a bread-board:

ATtin44 pin# | port   |   STK500   | Security Xplained
       1     | Vcc    | VTG        | J104 10
       2     | XTAL1  | EXPAND1 17 | NC
       3     | XTAL2  | EXPAND1 18 | NC
       4     | RESET  | AUX RST    | NC
       5     | PB2    | NC         | NC
       6     | OC0B   | RS232  RXD | NC
       7     | SDA    | NC         | J104  1
       8     | OC1B   | RS232  TXD | NC
       9     | SCL    | NC         | J104  2
      10     | NC     | NC         | NC
      11     | NC     | NC         | NC
      12     | NC     | NC         | NC
      13     | NC     | NC         | NC
      14     | GND    | GND        | J104  9


For the client, a Rhino Black evalutaion kit was used. A Microbase with a SHA204 daughter card would also serve the purpose.

12/2/2011
Gunter Fuchs at Atmel Crypto Products
719-540-1107

