/*

FROM:

	//! SA_Read output overlay
	typedef struct _CA_Read_out
	{
		uint8_t		count;
		uint8_t		data[4];
		uint16_t		CRC;

	} CA_Read_out;


TO:

	//! SA_Read output overlay
	typedef struct _CA_Read_out
	{
		uint8_t		count;
		uint8_t		ReadData[4];
		uint16_t		CRC;

	} CA_Read_out;



FROM:

	typedef struct _HAHost0Out
	{
		uint8_t		count;
		uint8_t		data[1];
		uint16_t		CRC;

	} HAHost0Out;

TO:

	typedef struct _HAHost0Out
	{
		uint8_t		count;
		uint8_t		HostData[1];
		uint16_t		CRC;

	} HAHost0Out;








*/
