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

/** \file	main.c
 *  \brief 	functions for commands
 *  \author Christopher Gorog
 *  \date 	April 8, 2010
 */

//#include "SA_Phys_Linux.h"
#include <stdint.h>
#include <stdio.h>

#include "Physical.h"
#include "SHA_CommMarshalling.h"
#include "SHA_Status.h"
#include "SHA_Comm.h"
#include "SHA_TimeUtils.h"


// Data definitions
extern SHA_CommParameters	commparms;
SHA_CommParameters*	mainparms = &commparms;

// Prototypes
void PrintResults(int8_t CmdSize);



int main( int argc, char *argv[] )
{

	uint8_t Mode = 0x00;
	uint16_t KeyID = 0xFFFF;
	uint8_t ChallBuf1[] = {	0x00, 0x01,0x02, 0x03,0x04, 0x05,0x06, 0x07,
							0x08, 0x09,0x0A, 0x0B,0x0C, 0x0D,0x0E, 0x0F,
							0x10, 0x11,0x12, 0x13,0x14, 0x15,0x16, 0x17,
							0x18, 0x19,0x1A, 0x1B,0x1C, 0x1D,0x1E, 0x1F	};

	uint8_t ChallBuf2[] = {	0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,
						    0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,
						    0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,
						    0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00	};


	uint8_t *Challenge1 = ChallBuf1;
	uint8_t *Challenge2 = ChallBuf2;


	//while(1)
	{
		//if(SHA_SUCCESS == SHAP_OpenChannel())
		if(SHA_SUCCESS == SHAP_Init())
		{
			SHAC_Wakeup();

			SHAC_Read(0x01, 0x0002);// Status fuses and MfgId fuses
			PrintResults(8);

			SHAC_Read(0x01, 0x0003); // Fuse Serial number value
			PrintResults(8);

			SHAC_Read(0x00, 0x0000); // ROM MfgId and ROM SN
			PrintResults(8);

			SHAC_Read(0x00, 0x0001); // ROM Revision number
			PrintResults(8);

			SHAC_Mac(Mode, KeyID, Challenge1);
			PrintResults(40);

			SHAC_Mac(Mode, KeyID, Challenge2);
			PrintResults(40);

		}
		//SHAP_CloseChannel();
		//loop_delay(50);
      SHAP_Delay(50000);
	}

	return 1;
}

void PrintResults(int8_t CmdSize)
{
	int i;
	//mainparms = SHAC_GetData();
	printf("Send Value: ");
	for(i=0;i< CmdSize;i++)
	{
		printf("%02X ", mainparms->txBuffer[i]);
	}
	printf("\nRecieve Value: ");
	for(i=0;i< mainparms->rxSize;i++)
	{
		printf("%02X ", mainparms->rxBuffer[i]);
	}
	printf("\n\n");
}


int main_hold( int argc, char *argv[] )
{

	uint8_t Mode = 0x00;
	uint16_t KeyID = 0xFFFF;
	uint8_t ChallBuf1[] = {	0x00, 0x01,0x02, 0x03,0x04, 0x05,0x06, 0x07,
							0x08, 0x09,0x0A, 0x0B,0x0C, 0x0D,0x0E, 0x0F,
							0x10, 0x11,0x12, 0x13,0x14, 0x15,0x16, 0x17,
							0x18, 0x19,0x1A, 0x1B,0x1C, 0x1D,0x1E, 0x1F	};

	uint8_t ChallBuf2[] = {	0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,
						    0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,
						    0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,
						    0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00	};


	uint8_t *Challenge1 = ChallBuf1;
	uint8_t *Challenge2 = ChallBuf2;


	//while(1)
	{
		//if(SHA_SUCCESS == SHAP_OpenChannel())
		if(SHA_SUCCESS == SHAP_Init())
		{
			SHAC_Wakeup();

			SHAC_Read(0x01, 0x0002);// Status fuses and MfgId fuses
			PrintResults(8);

			SHAC_Read(0x01, 0x0003); // Fuse Serial number value
			PrintResults(8);

			SHAC_Read(0x00, 0x0000); // ROM MfgId and ROM SN
			PrintResults(8);

			SHAC_Read(0x00, 0x0001); // ROM Revision number
			PrintResults(8);

			SHAC_Mac(Mode, KeyID, Challenge1);
			PrintResults(40);

			SHAC_Mac(Mode, KeyID, Challenge2);
			PrintResults(40);

		}
		//SHAP_CloseChannel();
		//loop_delay(50);
      SHAP_Delay(50000);
	}

	return 1;
}



