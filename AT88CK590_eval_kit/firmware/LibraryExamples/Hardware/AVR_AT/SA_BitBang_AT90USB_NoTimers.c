// ----------------------------------------------------------------------------
//         ATMEL Microcontroller Software Support  -  Colorado Springs, CO -
// ----------------------------------------------------------------------------
// DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
// DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------
/** \file
 *  \brief Functions of CryptoAuthentication Communication Layer That Are Closest to Hardware
 *  \date Jan 13, 2010
 */

#include <stdint.h>
#include <avr/io.h>
#include <string.h>

#include "delay_x.h"
#include "SA_Phys.h"

// ******************************************************************************
// ******************* hardware / system related definitions ********************
// ******************************************************************************

// There should not be any need to modify function bodies when porting this module to other CPUs.

#define MAX_DEVICE_ID         1                             //!< maximum device id

// client
#define SIG1_PORT_DDR      (DDRB)                           //!< direction register for device id 0
#define SIG1_PORT_OUT      (PORTB)                          //!< output port register for device id 0
#define SIG1_PORT_IN       (PINB)                           //!< input port register for device id 0
#define SIG1_BIT           (7)                              //!< bit position of port register for device id 0

// host
#define SIG2_PORT_DDR      (DDRB)                           //!< direction register for device id 1
#define SIG2_PORT_OUT      (PORTB)                          //!< output port register for device id 1
#define SIG2_PORT_IN       (PINB)                           //!< input port register for device id 1
#define SIG2_BIT           (6)                              //!< bit position of port register for device id 1

// debug pin that indicates pulse edge detection. Only enabled if compilation switch _DEBUG is used.
// To debug timing, disable host power (H1 and H2 on CryptoAuth daughter board) and connect logic analyzer
// or storage oscilloscope to the H2 pin that is closer to the H1 header.
#define DEBUG_PORT_DDR     (DDRB)                           //!< direction register for debug pin
#define DEBUG_PORT_OUT     (PORTB)                          //!< output port register for debug pin
#define DEBUG_BIT          (0)                              //!< bit position of debug pin

#ifdef DEBUG_BITBANG
   #define DEBUG_LOW       DEBUG_PORT_OUT &= ~_BV(DEBUG_BIT)   //!< set debug pin low
   #define DEBUG_HIGH      DEBUG_PORT_OUT |= _BV(DEBUG_BIT)    //!< set debug pin high
#else
   #define DEBUG_LOW
   #define DEBUG_HIGH
#endif

// time to drive bits at 230.4 kbps
// Using the value below we measured 4340 ns with logic analyzer (10 ns resolution).
#define PORT_ACCESS_TIME   (630)                                //!< time it takes to toggle the pin in ns
#define BIT_DELAY_1        _delay_ns( 4340 - PORT_ACCESS_TIME)  //!< pulse width for one pulse (start pulse or zero pulse)
#define BIT_DELAY_5        _delay_ns(26042 - PORT_ACCESS_TIME)  //!< time to keep pin high for five pulses plus stop bit (used to bit-bang CryptoAuth 'zero' bit)
#define BIT_DELAY_7        _delay_ns(39063 - PORT_ACCESS_TIME)  //!< time to keep pin high for seven bits plus stop bit (used to bit-bang CryptoAuth 'one' bit)


// One loop iteration for edge detection takes about 0.6 us on this hardware.
// Lets set the timeout value for start pulse detection to the maximum bit time of 86 us.
/** \brief This value is decremented while waiting for the falling edge of a start pulse.
 */
#define START_PULSE_TIME_OUT  (150)

// We measured a loop count of 8 for the start pulse. That means it takes about
// 0.6 us per loop iteration. Maximum time between rising edge of start pulse
// and falling edge of zero pulse is 8.6 us. Therefore, a value of 20 (10 to 12 us)
// gives ample time to detect a zero pulse and also leaves enough time to detect
// the following start pulse.
/** \brief This value is decremented while waiting for the falling edge of a zero pulse.
 */
#define ZERO_PULSE_TIME_OUT   (20)


/** \brief Maps port pins with device identifiers. */
uint8_t devicePin[MAX_DEVICE_ID + 1] = {SIG1_BIT, SIG2_BIT};

/** \brief Maps port direction registers with device identifiers. */
volatile uint8_t *devicePortDdr[MAX_DEVICE_ID + 1] = {&SIG1_PORT_DDR, &SIG2_PORT_DDR};

/** \brief Maps port output registers with device identifiers. */
volatile uint8_t *devicePortOut[MAX_DEVICE_ID + 1] = {&SIG1_PORT_OUT, &SIG2_PORT_OUT};

/** \brief Maps port input registers with device identifiers. */
volatile uint8_t *devicePortIn[MAX_DEVICE_ID + 1] = {&SIG1_PORT_IN, &SIG2_PORT_IN};

/** \brief Variable is set when switching to rx or tx mode for faster pin toggling. */
uint8_t activeDevicePin = SIG1_BIT;

/** \brief Variable is set when switching to tx mode for faster pin toggling. */
volatile uint8_t *activeDevicePortOut;

/** \brief Variable is set when switching to rx mode for faster reading of pin state. */
volatile uint8_t *activeDevicePortIn;

//! declaration of the variable indicating which device to talk to
uint8_t deviceID = 0;

//! device id for client currently in use
uint8_t clientDeviceId = NO_DEVICE_ID;

//! device id for host currently in use
uint8_t hostDeviceId = NO_HOST_ID;

//! array of device information structures
DiscoveryDeviceInfo_t devices[MAX_DEVICE_ID + 1];

// ******************************************************************************
// ******************* end of hardware / system related definitions *************
// ******************************************************************************


/** \brief Gets the maximum device id in the system / number of devices minus one.
 *
 * \return maximum device id
 */
uint8_t SAP_GetMaxDeviceId()
{
   return MAX_DEVICE_ID;
}


/** \brief Gets the device information of the chosen device.
 *
 * \param[in] deviceId index into the "devices" array
 * \return pointer to device information structure
 */
DiscoveryDeviceInfo_t *SAP_GetDeviceInfo(uint8_t deviceId)
{
   if (deviceId > MAX_DEVICE_ID)
      return (void *) 0;

   return &devices[deviceId];
}


/** \brief Gets the type of the currently selected client.
 *
 * \return device type
 */
uint8_t SAP_GetDeviceType()
{
   if (deviceID == NO_DEVICE_ID)
      return SA_DeviceTypeUnknown;

   return devices[deviceID].deviceType;
}


/** \brief Sets the client device id.
 *
 * @param id identifier for client device
 */
int8_t SAP_SetClientDeviceID(uint8_t id)
{
   if (id > MAX_DEVICE_ID && id != NO_DEVICE_ID)
      return SA_BADPARM;

   clientDeviceId = id;

   return SA_SUCCESS;
}


/** \brief Gets the client device id.
 *
 * \param[out] id pointer to id for currently selected client device
 * @return device identifier for client
 */
int8_t SAP_GetClientDeviceID(uint8_t *id)
{
   *id = clientDeviceId;
   return (clientDeviceId > MAX_DEVICE_ID) ? SA_GENFAIL : SA_SUCCESS;
}


/** \brief Sets the host device id.
 *
 * @param id identifier for host device
 */
int8_t SAP_SetHostDeviceID(uint8_t id)
{
   if (id > MAX_DEVICE_ID && id != NO_HOST_ID)
      return SA_BADPARM;

   hostDeviceId = id;

   return SA_SUCCESS;
}


/** \brief Gets the host device id.
 *
 * \param[out] id pointer to id for host device
 * @return device identifier for host
 */
int8_t SAP_GetHostDeviceID(uint8_t *id)
{
   *id = hostDeviceId;
   return (hostDeviceId > MAX_DEVICE_ID) ? SA_GENFAIL : SA_SUCCESS;
}


/** \brief Sets the device id. Communication functions will use this device id.
 *
 *  \param[in] id device id
 *  \return status of the operation
 ****************************************************************
 */
int8_t SAP_SetDeviceID(uint8_t id)
{
   if (id > MAX_DEVICE_ID)
      return SA_BADPARM;

   deviceID = id;

   return SA_SUCCESS;
}


/** \brief Gets the device id currently in use.
 *
 * @param[out] id pointer to device identifier
 * @return device identifier
 */
int8_t SAP_GetDeviceID(uint8_t *id)
{
   *id = deviceID;
   return (deviceID > MAX_DEVICE_ID) ? SA_GENFAIL : SA_SUCCESS;
}


/** \brief 	Configures signal pin as output.
 *  \return void
 ****************************************************************
 */
void SAP_TxMode() {
   // Supply port and pin information about this device.
   activeDevicePin = _BV(devicePin[deviceID]);
   activeDevicePortOut = devicePortOut[deviceID];

   // Set pin as output and high.
   *activeDevicePortOut |= activeDevicePin;
   *devicePortDdr[deviceID] |= activeDevicePin;

   // Wait turn around time minus time we already spent in this function.
   _delay_us(50);
}


/** \brief 	Configures signal pin as input without pull-up.
 *  \return void
 ****************************************************************
 */
void SAP_RxMode() {
   // Supply port and pin information about this device for later use when receiving.
   activeDevicePin = _BV(devicePin[deviceID]);
   activeDevicePortIn = devicePortIn[deviceID];

   // Set pin as input.
   *devicePortDdr[deviceID] &= ~_BV(devicePin[deviceID]);

   // Set debug pin as output.
   DEBUG_PORT_DDR |= _BV(DEBUG_BIT);
   DEBUG_LOW;

   // Wait turn around time minus time we already spent in this function.
   _delay_us(20);
}


/** \brief Not used when not using timers.
 *
 * \param[in] Timeout value to load the timer compare register with
 */
void SAP_SetupIoTimeoutTimer(uint16_t Timeout)
{
}


/** \brief 	Initializes communication time-out timer.
 *  \return void
 ****************************************************************
 */
void SAP_Init() {
   uint8_t i;

   memset((void *) &devices, 0, sizeof(devices));

   for (i = 0; i <= MAX_DEVICE_ID; i++) {
      devices[i].deviceAddress = 0xFF;
      devices[i].deviceId = NO_DEVICE_ID;
   }

   DEBUG_PORT_DDR |= _BV(DEBUG_BIT);
   DEBUG_LOW;
}


/** \brief 	Puts a "one" bit on the signal wire. Inlined.
 *  \return void
 ****************************************************************
 */
void SAP_SendOne() {
   *activeDevicePortOut &= ~activeDevicePin;
   BIT_DELAY_1;
   *activeDevicePortOut |= activeDevicePin;
   BIT_DELAY_7;
}


/** \brief 	Puts a "zero" bit on the signal wire. Inlined.
 *  \return void
 ****************************************************************
 */
void SAP_SendZero() {
   *activeDevicePortOut &= ~activeDevicePin;
   BIT_DELAY_1;
   *activeDevicePortOut |= activeDevicePin;
   BIT_DELAY_1;
   *activeDevicePortOut &= ~activeDevicePin;
   BIT_DELAY_1;
   *activeDevicePortOut |= activeDevicePin;
   BIT_DELAY_5;
}


/** \brief 	Waits until transmission is complete.
 *
 *    For this bit-banging version it is only a dummy function that is
 *    called from the communication layer. For the UART version, this function
 *    checks the tx empty flag.
 *
 *  \return void
 ****************************************************************
 */
void SAP_WaitForXmitComplete() {
}


/** \brief 	Reads a bit from the signal wire.
 *  \param[out]   bitData pointer to bit value
 *  \return status of the operation
 ****************************************************************
 */
int8_t SAP_GetBit(uint8_t *bitData) {
   uint8_t edgeCount = 0;

   // Make sure that the variable below is big enough.
   // Change it to uint16_t if 255 is too small, but be aware that
   // the loop resolution decreases on an 8-bit controller in that case.
   uint8_t timeOutCount = START_PULSE_TIME_OUT;

#ifdef DEBUG_BITBANG
   // Use this variable to measure the number of loop counts per pulse
   // to establish a value for the zero bit detection timeout.
   // For the AT88CK109STK3 (AT90USB1287 at 16 MHz) it was 8 for the start pulse.
   uint8_t startPulseWidth = 0;
#endif

   // Detect start bit.
   // Disable interrupts during detection since we might otherwise miss the edges.
   // We should actually disable interrupts during the entire time a command is sent
   // or a response is received when bit-banging. We should therefore disable interrupts in the
   // SAC_SendAndReceive function. Or we have to make sure that interrupt handlers
   // do not take more than: minimum bit-time - 3 * maximum pulse width - turn-around
   // time between possibly inlined calls to SAP_GetBit
   // (46.2 us - 3 * 8.6 us = 20.4 us, excludes turn-around time).
   //cli();
   DEBUG_HIGH;
   while (--timeOutCount) {
      // Wait for falling edge.
      if (!(*activeDevicePortIn & activeDevicePin))
         break;
   }
   DEBUG_LOW;

   if (!timeOutCount) {
      // Allows to put a break point.
      //asm volatile("nop"::);
      return SA_TIMEOUT;
   }
   DEBUG_HIGH;

   do {
      // Wait for rising edge.
      if (*activeDevicePortIn & activeDevicePin) {
         // For an Atmel microcontroller this might be faster than "edgeCount++".
         edgeCount = 1;
         break;
      }
#ifdef DEBUG_BITBANG
      startPulseWidth++;
#endif
   } while (--timeOutCount);
   DEBUG_LOW;

#ifdef DEBUG_BITBANG
   if (!edgeCount || !startPulseWidth)
#else
   if (!edgeCount)
#endif
   {
      // Allows to put a break point.
      //asm volatile("nop"::);
      return SA_TIMEOUT;
   }

   // Trying to measure the time of start bit and calculating the timeout
   // for zero bit detection is not accurate enough for an 8 MHz 8-bit CPU.
   // So let's just wait the maximum time for the falling edge of a zero bit
   // to arrive after we have detected the rising edge of the start bit.
   timeOutCount = ZERO_PULSE_TIME_OUT;

   // Detect possible edge indicating zero bit.
   DEBUG_HIGH;
   do {
      if (!(*activeDevicePortIn & activeDevicePin)) {
         // For an Atmel microcontroller this might be faster than "edgeCount++".
         edgeCount = 2;
         break;
      }
   } while (timeOutCount--);
   DEBUG_LOW;

   // Wait for rising edge of zero pulse before returning. Otherwise we might interpret
   // its rising edge as the next start pulse.
   if (edgeCount == 2) {
      DEBUG_HIGH;
      do {
         if (*activeDevicePortIn & activeDevicePin)
            break;
      } while (timeOutCount--);
   }
   // Enable interrupts.
   //sei();
   DEBUG_LOW;

   // Normally, we would check the parameter at the beginning of this function,
   // but we have more time at the end.
   if (!bitData)
      return SA_BADPARM;

   // Since we check the validity of the pointer to bitData only now, we
   // assign the detected bit value here.
   *bitData = (edgeCount < 2 ? 0x7F : 0);

   return SA_SUCCESS;
}


/** \brief 	Wakes up the devices connected to the selected pin.
 *  \return void
 ****************************************************************
 */
void SAP_Wakeup() {
   *activeDevicePortOut &= ~activeDevicePin;
   _delay_us(DLY_WAKE_PULSE);
   *activeDevicePortOut |= activeDevicePin;

   // Wait after transition
   _delay_us(DLY_WAKE);
}


/** \brief 	Generates an execution delay. This is needed to keep AVR
 *				library from pulling in (massive!) floating point support
 *				for delays using run-time parameters.
 *  \param  delay execution delay type depending on command being executed
 *  \return void
 ****************************************************************
 */
void SAP_ExecDelay(SA_Delay delay) {

   switch (delay) {
      case SA_DELAY_PARSE:
         _delay_us(DLY_PARSE);
         break;

      case SA_DELAY_EXEC_MAC:
         _delay_us(DLY_PARSE + DLY_EXEC_MAC);
         break;

      case SA_DELAY_EXEC_READ:
         _delay_us(DLY_PARSE + DLY_EXEC_READ);
         break;

      case SA_DELAY_EXEC_FUSE:
         _delay_us(DLY_PARSE + DLY_EXEC_FUSE);
         break;

      case SA_DELAY_EXEC_SECURE:
         _delay_us(DLY_PARSE + DLY_EXEC_SECURE);
         break;

      case SA_DELAY_EXEC_SECURE_HOST:
         _delay_us(DLY_PARSE + DLY_EXEC_SECURE_HOST);
         break;

#ifdef LOW_VOLTAGE
      case SA_DELAY_EXEC_FUSE_LOW_V:
         _delay_ms((uint8_t) DLY_EXEC_FUSE_LOW_V);
         _delay_us(DLY_PARSE);
         break;

      case SA_DELAY_EXEC_SECURE_LOW_V:
         _delay_ms((uint16_t) DLY_EXEC_SECURE_LOW_V);
         _delay_us(DLY_PARSE);
         break;
#endif

      case SA_DELAY_EXEC_PERSON:
         _delay_us(DLY_PARSE + DLY_PERSON);
         break;

      case SA_DELAY_EXEC_HOST0:
         _delay_us(DLY_PARSE + DLY_HOST0);
         break;

      case SA_DELAY_EXEC_HOST1:
         _delay_us(DLY_PARSE + DLY_HOST1);
         break;

      case SA_DELAY_EXEC_HOST2:
         _delay_us(DLY_PARSE + DLY_HOST2);
         break;

      case SA_DELAY_SLEEP_WAKE:
         _delay_us(DLY_SLEEP_WAKE);
         break;

      default:
         break;
   }
}


/** \brief 	Not used.
 ****************************************************************
 */
void SAP_EnableIoTimer() {
}


/** \brief 	Not used.
 *  \return void
 ****************************************************************
 */
void SAP_DisableIoTimer() {
}


