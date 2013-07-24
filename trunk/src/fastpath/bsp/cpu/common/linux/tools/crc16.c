/***************************************************************************
**
**  MODULE : crc16.c
**                     
**  FUNCTION :        
** This file contains routines to perform byte-at-a-time CRC computation.
**
** The program is derived from pascal program written by Donald R. Blake.
**                                                                  
**  CHANGE SUMMARY :                                               
**    date   SID  Init    Action                                  
**   ----------------------------                                
**   22oct93 1.1  ALT     Created file.                         
**                                                            
***************************************************************************/
#define POLYNOMIAL 0xA097  /* Use 0x8005 for CRC16, 0x1021 for SDLC/HDLC */

static unsigned short CrcTable[256]; /* CRC lookup table */

/************************************************************************
** NAME: UpdateCRC
**
** This function updates existing CRC 
**
** INPUT:
**	DataByte - Received data byte
**	
** INPUT/OUTPUT:
**	crc - Current value of CRC.
************************************************************************/
extern void UpdateCRC (unsigned char DataByte, unsigned short *crc)
{
 register unsigned long new_crc;

 new_crc = *crc;
 new_crc = (new_crc << 8) ^ (CrcTable[new_crc >> 8] ^ DataByte);
 *crc = (unsigned short) new_crc;
};


/************************************************************************
** NAME: GenCrcTable
**
** This function computes CRC look-up table.
************************************************************************/
extern void GenCrcTable (void)
{
 unsigned short CrcCode;
 unsigned char DataByte;
 unsigned long  index, BitNo;
 unsigned long  CarryBit, DataBit;

 for (index = 0; index <= 255; index++) {
   CrcCode = 0;
   DataByte = index;

   for (BitNo = 1; BitNo <= 8; BitNo++) {
	CarryBit = ((CrcCode & 0x8000) != 0);
	DataBit = ((DataByte & 0x80) != 0);
	CrcCode = CrcCode << 1;
	if (CarryBit ^ DataBit) CrcCode = CrcCode ^ POLYNOMIAL;
	DataByte = DataByte << 1;
     }; /* End For */
   CrcTable [index] = CrcCode;
  }; /* End for */
};
