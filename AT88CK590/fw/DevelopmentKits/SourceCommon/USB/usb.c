/** \file
 *  \brief Definition of USB sending and receiving data functions.
 *  \author Atmel Crypto Products
 *  \date March 1, 2011
 */

#include <string.h>

#include "usb.h"
#include "usb_drv.h"
#include "usb_device_task.h"
#include "usb_standard_request.h"
#include "conf_usb.h"
#include "usb_task.h"

#include "parser.h"
#include "hardware.h"
#include "ll_timers.h"


// protocol class identifiers
typedef enum
{
  CLASS_GEN,
  CLASS_CM,
  CLASS_CM_CMC
} CLASS_ID;


// byte indexes in response packet
enum
{
  RESPONSE_STATUS_INDEX = 4,
  RESPONSE_LEN_INDEX = 5,
  RESPONSE_PAYLOAD_INDEX = 6
};

uint uiFunctionEntered = 0;
uchar ucLed1State = 0;
uchar ucUsbDataParseState = PACKET_STATE_IDLE;
uchar ucParseDataStart = FALSE;
uchar ucUsbReceiveDataIndex = 0;
uchar payloadSize = 0;
uchar payloadEnd = 0;
uchar packetEnd = 0;



/** \brief Initialize USB device
 */
void Usb_Initialize(void)
{
  ucUsbDataParseState = PACKET_STATE_IDLE;
  Usb_task_initialize();
}


/** \brief State of USB device
 */
void Usb_CheckPacketState()
{
  uchar ucReturn = SUCCESS;

  Usb_device_task();
  Usb_CheckDataReceived();

  if (ucParseDataStart == TRUE)
  {
    uchar packetIndex = 0;

    // Switch LED2 on while executing the API command.
    LED2_ON
    uchar ucSendDataSize = 0;
    ucReturn = UNKNOWN_COMMAND;

    if (pucUsbRxBuffer[INDEX_CLASSID] == CLASS_GEN)
      ucReturn = ParseKitCommands(&pucUsbRxBuffer[INDEX_CMDID],
                                  // For now, assume that all command packets are less than 128 bytes in length.
                                  pucUsbTxBuffer + RESPONSE_PAYLOAD_INDEX,
                                  &ucSendDataSize);
    // Look for CM commands
    else if (((pucUsbRxBuffer[INDEX_CMDID] >= LIB_CM_CMD_START) && (pucUsbRxBuffer[INDEX_CMDID] <= LIB_CM_CMD_END)) ||
      ((pucUsbRxBuffer[INDEX_CMDID] >= LIB_CMD_GET_ENCR_WRITE_DATA) && (pucUsbRxBuffer[INDEX_CMDID] <= LIB_CMD_GET_ENCR_READ_PWD)) ||
      pucUsbRxBuffer[INDEX_CMDID] == LIB_CMD_RESET_PASSWORD ||
      pucUsbRxBuffer[INDEX_CMDID] == LIB_CMD_CM_INIT)
      ucReturn = ParseCmCommands(&pucUsbRxBuffer[INDEX_CMDID],
                                  // For now, assume that all command packets are less than 128 bytes in length.
                                  pucUsbRxBuffer[INDEX_LEN + 1],
                                  pucUsbTxBuffer + RESPONSE_PAYLOAD_INDEX,
                                  &ucSendDataSize);
    // Look for CMC commands
    else if (pucUsbRxBuffer[INDEX_CMDID] >= CMC_CMD_START && pucUsbRxBuffer[INDEX_CMDID] <= CMC_CMD_END)
      ucReturn = ParseCmcCommands(&pucUsbRxBuffer[INDEX_CMDID],
                                  // For now, assume that all command packets are less than 128 bytes in length.
                                  pucUsbRxBuffer[INDEX_LEN + 1],
                                  pucUsbTxBuffer + RESPONSE_PAYLOAD_INDEX,
                                  &ucSendDataSize);
    // Look for Bowline commands
    else if (pucUsbRxBuffer[INDEX_CMDID] >= CMC_BOWLINE_START && pucUsbRxBuffer[INDEX_CMDID] <= CMC_BOWLINE_END)
      ucReturn = ParseBowlineCommands(&pucUsbRxBuffer[INDEX_CMDID],
                                      // For now, assume that all command packets are less than 128 bytes in length.
                                      pucUsbRxBuffer[INDEX_LEN + 1],
                                      pucUsbTxBuffer + RESPONSE_PAYLOAD_INDEX,
                                      &ucSendDataSize);

    pucUsbTxBuffer[packetIndex++] = 'S';
    pucUsbTxBuffer[packetIndex++] = 'O';
    pucUsbTxBuffer[packetIndex++] = 'P';
    pucUsbTxBuffer[packetIndex++] = ucReturn;
    // A return length of less than 256 bytes is assumed.
    pucUsbTxBuffer[packetIndex++] = 0;
    pucUsbTxBuffer[packetIndex++] = ucSendDataSize;
    packetIndex += ucSendDataSize;
    pucUsbTxBuffer[packetIndex++] = 'E';
    pucUsbTxBuffer[packetIndex++] = 'O';
    pucUsbTxBuffer[packetIndex++] = 'P';
    Usb_Send(packetIndex, pucUsbTxBuffer);

    ucParseDataStart = FALSE;

    // Done with command execution. Switch LED2 off.
    LED2_OFF

    // Indicate error condition.
    if (ucReturn)
      LED3_ON
    else
      LED3_OFF
  }
  // Toggle LED1 appr. every second to indicate that everything is operational.
//  if (uiFunctionEntered < (((uint) (F_CPU / 1000000UL)) * 4000))
//    uiFunctionEntered++;
//  else
//  {
//    ToggleLed1();
//    uiFunctionEntered = 0;
//  }
}


/** \brief Check for incoming data
 */
void Usb_CheckDataReceived(void)
{
  if (Is_device_enumerated())             //! Check USB HID is enumerated
  {
    Usb_endpoint_select(EP_HID_OUT);    //! Get Data report from Host
    if (Is_usb_receive_out())
      Usb_ReceivePacket();
  }
}


void Usb_ReceivePacket()
{
  // Take 8 byte data, store it to local buffer
  uchar i, ucLocalReceiveDataBuffer[8];


  for (i = 0; i < 8; i++)
    ucLocalReceiveDataBuffer[i] = Usb_byte_read(); // Fill report

  Usb_receive_out_ack();

  for (i = 0; i < 8 ; i++)
  {
    switch (ucUsbDataParseState)
    {
      case PACKET_STATE_IDLE:
        if (i < INDEX_LEN + 1)
          break;

        // Check for start of packet.
        if (ucLocalReceiveDataBuffer[0] != 'S' ||
            ucLocalReceiveDataBuffer[1] != 'O' ||
            ucLocalReceiveDataBuffer[2] != 'P')
            // Did not receive start of packet.
            return;

        // Check length.
        payloadSize = ucLocalReceiveDataBuffer[INDEX_LEN];
        if (payloadSize != 0)
          // Length is greater than 127.
          return;

        payloadSize = ucLocalReceiveDataBuffer[i];
        payloadEnd = INDEX_PAYLOAD + payloadSize;
        packetEnd = payloadEnd + 3;
        ucUsbReceiveDataIndex = i + 1;
        memcpy(pucUsbRxBuffer, ucLocalReceiveDataBuffer, ucUsbReceiveDataIndex);
        ucUsbDataParseState = PACKET_STATE_TAKE_DATA;
        break;

      case PACKET_STATE_TAKE_DATA:
        if (ucUsbReceiveDataIndex > payloadEnd)
          ucUsbDataParseState = PACKET_STATE_END_OF_VALID_DATA;
        pucUsbRxBuffer[ucUsbReceiveDataIndex++] = ucLocalReceiveDataBuffer[i];
        break;

      case PACKET_STATE_END_OF_VALID_DATA:
        // The last three bytes should be "EOP".
        if (ucUsbReceiveDataIndex < packetEnd)
        {
          pucUsbRxBuffer[ucUsbReceiveDataIndex++] = ucLocalReceiveDataBuffer[i];
          if (ucUsbReceiveDataIndex < packetEnd)
            break;
        }
        if (pucUsbRxBuffer[ucUsbReceiveDataIndex - 3] == 'E' &&
            pucUsbRxBuffer[ucUsbReceiveDataIndex - 2] == 'O' &&
            pucUsbRxBuffer[ucUsbReceiveDataIndex - 1] == 'P')
            // correct end of packet
            ucParseDataStart = TRUE;

        ucUsbReceiveDataIndex = payloadEnd = payloadSize = packetEnd = 0;
        ucUsbDataParseState = PACKET_STATE_IDLE;
        return;
    }
  }
}


/** \brief Send data to USB
 */
// new protocol
void Usb_Send(uchar ucDataLength, puchar pucDataBuffer)
{
  uint uiTimeOut = 100; // 100 ms
  uchar i = 0;
  uchar j = 0;
  uchar ucDataBufferTemp[ucDataLength + 7];

  memset(ucDataBufferTemp, 0, ucDataLength + 7);
  for (i = 0; i < ucDataLength; i++)
    ucDataBufferTemp[i] = pucDataBuffer[i];
  uchar nPackets = ucDataLength / 8;
  uchar nBytesLeft = ucDataLength % 8;
  if (nBytesLeft > 0)
    nPackets++;
  i = 0;
/*
#ifdef UART_LOGGING
  uart_PutString("USB Send : ");
  char len[3];
  len[0] = Nibble_to_ascii(ucDataLength >> 4);
  len[1] = Nibble_to_ascii(ucDataLength);
  len[2] = 0;
  uart_PutString(len);
  uart_PutCrLf();
#endif
*/

  Usb_endpoint_select(EP_HID_IN); //! Ready to send information to host application

  while (nPackets--)
  {
    // Send eight bytes per USB packet.
    for (j = 0; j < 8; j++)
    {
      if (Is_usb_write_enabled())
      {
        Usb_byte_write(ucDataBufferTemp[i++]);
        uiTimeOut = 0;
      }
      else
      {
        Timer_delay_ms(1);
        if (uiTimeOut-- == 0)
        {
          #ifdef UART_LOGGING
            uart_PutString((pschar) "USB Send : Fail\r\n");
          #endif

          return;
        }
      }
    }
    // one USB packet of eight bytes sent -> receive ack
    Usb_in_ready_ack();
    Timer_delay_ms(4);
  }
}


/** \brief Function to convert hexadecimal data to string and to send via USB
 */
/*
void Usb_ConvertDataAndSend(uchar ucDataLength, puchar pucDataBuffer)
{
    ConvertBinaryToHexAscii(ucDataLength, pucDataBuffer);
    Usb_SendRawData(ucDataLength, pucDataBuffer);
}
*/

/** \brief Function to toggle LED1
 */
void ToggleLed1(void)
{
  if (ucLed1State == 0)
  {
    LED1_ON
    ucLed1State = 1;
  }
  else if (ucLed1State == 1)
  {
    LED1_OFF
    ucLed1State = 0;
  }
}
