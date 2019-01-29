/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  vpd.c
*
* @purpose   Provide VPD utility functions
*
* @component BSP
*
* @create    06/14/2006
*
* @author    Brady Rutherford
*
* @end
*
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#ifdef _L7_OS_VXWORKS_
#include <fioLib.h>
#endif

#include "l7_common.h"
#include "platform_cpu.h"
#include "bspcpu_api.h"
#include "log_api.h"
#include "cfd.h"
#include "bspapi.h"

extern int sysEnetAddrGet (char *dev, int unit, unsigned char *pMac);
/**************************************************************************
*
* @purpose  Retrieve a CFD field's value
*
* @param    field @b{(input)} the field's value that is wanted
* @param    *pCFD @b{(input)} ptr where CFD contents are to be searched for field
* @param    *pbuffer @b{(output)} where to put field's value if found in CFD data pointed to by *pCFD
*
* @returns  L7_SUCCESS if field is found
* @returns  L7_FAILURE if field is NOT found
*
* @notes    if L7_SUCCESS, *pCFD pts to field
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t getCFDField( L7_uchar8 field,
                     L7_char8 *pCFD,
                     void *pbuffer)
{
  cfd_mapping *p;

  /* pCFD = beginning of CFD data from I2C call  */
  /* field = what the user is looking for        */
  /* pbuffer = where to put data if found in cfd */

  p = (cfd_mapping *)( pCFD + 2 ); /* move past the CRC bytes */

  while ( p->vector != 0x00 )
  {
    if ( p->vector == field )
    {
      switch ( field )
      {
        case CFD_CARD_ID:
          memcpy( pbuffer, (void *)&p->data, 2);
          return(L7_SUCCESS);

        case CFD_CPU_ID:
          memcpy( pbuffer, (void *)&p->data, 4);
          return(L7_SUCCESS);

        case CFD_REVISION_ID:
          memcpy( pbuffer, (void *)&p->data, 2);
          return(L7_SUCCESS);

        case CFD_CARD_BASE_MAC:
          memcpy( pbuffer, (void *)&p->data, 6);
          return(L7_SUCCESS);

        case CFD_NUMBER_MAC_ADDRESSES:
          memcpy( pbuffer, (void *)&p->data, 1);
          return(L7_SUCCESS);

        case CFD_INT_LEVEL:
          memcpy( pbuffer, (void *)&p->data, 1);
          return(L7_SUCCESS);

        case CFD_CPU_TYPE:
          memcpy( pbuffer, (void *)&p->data, 1);
          return(L7_SUCCESS);

        case CFD_SN:
          strcpy( (L7_char8 *)pbuffer, (L7_char8 *)&p->data );
          return(L7_SUCCESS);

        case CFD_SERVICE_TAG:
          strcpy((L7_char8 *)pbuffer, (L7_char8 *)&p->data);
          return(L7_SUCCESS);

        case CFD_TYPE:
          strcpy( (L7_char8 *)pbuffer, (L7_char8 *)&p->data );
          return(L7_SUCCESS);

        case CFD_MODEL:
          strcpy( (L7_char8 *)pbuffer, (L7_char8 *)&p->data );
          return(L7_SUCCESS);

        case CFD_MFGR:
          memcpy( pbuffer, (void *)&p->data, 2);
          return(L7_SUCCESS);

        case CFD_PN:
          strcpy( (L7_char8 *)pbuffer, (L7_char8 *)&p->data );
          return(L7_SUCCESS);

        case CFD_FRU:
          strcpy( (L7_char8 *)pbuffer, (L7_char8 *)&p->data );
          return(L7_SUCCESS);

        case CFD_DESC:
          strcpy( (L7_char8 *)pbuffer, (L7_char8 *)&p->data );
          return(L7_SUCCESS);

        case CFD_MAINT_LVL:
          strcpy( (L7_char8 *)pbuffer, (L7_char8 *)&p->data );
          return(L7_SUCCESS);

        case CFD_PASSWORD:
          strcpy( (L7_char8 *)pbuffer, (L7_char8 *)&p->data );
          return(L7_SUCCESS);

        case CFD_HW_REV:
          strcpy( (L7_char8 *)pbuffer, (L7_char8 *)&p->data );
          return(L7_SUCCESS);

        case CFD_BOARD_ID:
          *((L7_char8 *)pbuffer) = *((L7_char8 *)(&p->data));
          return(L7_SUCCESS);

        case CFD_EXTENDED_VECTOR_CODE:

        default:
          break;
      }
    }
    /* move to next field */
    p = ( cfd_mapping * )( ( L7_uchar8 * )p + p->length );

    /* 
    * Even though a corrupted NVRAM SRAM may pass the CRC check, 
    * protect ourselves from NVRAM SRAM that have garbage in them. 
    */

    if ( ( L7_char8 * )p > ( pCFD + SERIAL_EEPROM_SIZE ) )
    {
      break;
    }
  }
  return(L7_FAILURE);

} /* end : getCFDField */

/**************************************************************************
*
* @purpose  This routine checks and generates a mac address. 
*
* @param    input_mac ptr to mac address in string format
* @param    mac       ptr to hexidecimal mac address
*
* @returns  none
*
* @notes    Duplicated here and in ipl to avoid link errors.
*
* @end
*
*************************************************************************/
static L7_RC_t make_mac (char * input_mac, L7_uchar8 * mac)
{
  L7_int32 i;
  L7_uchar8 c;
  L7_uchar8 temp_mac [12];

  for ( i = 0; i < 12; i++ )
  {
    c = input_mac [i];
    if ( (c >= '0') && (c <= '9') )
      temp_mac[i] = c - '0';
    else if ( (c >= 'a') && (c <= 'f') )
      temp_mac[i] = c - 'a' + 10;
    else if ( (c >= 'A') && (c <= 'F') )
      temp_mac[i] = c - 'A' + 10;
    else return(L7_ERROR);   /* Invalid character */
  }

  for ( i = 0; i < 12; i+= 2 )
  {
    mac[i/2] = (temp_mac[i] * 16) + temp_mac[i+1];
  }
  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  This routine prints the mac address. 
*
* @param    mac_address ptr to mac to print
*
* @returns  none
*
* @notes    Duplicated here and in ipl to avoid link errors. 
*
* @end
*
*************************************************************************/
static void print_mac (L7_uchar8 * mac_address)
{
  L7_int32 i;

  for ( i = 0; i < 6; i++ )
  {
    printf ("%02X", mac_address[i]);
    if ( i < 5 )
      printf (":");
    else
      printf (" ");
  }
}

/**************************************************************************
*
* @purpose  Generate a VPD file
*
* @param    none.
*
* @returns  L7_SUCCESS or L7_FAILURE.
*
* @comments prompts user for the MAC address & serial number.
*
* @end
*
*************************************************************************/
L7_RC_t RuntimeVPDFileGenerate(L7_char8 *pCFD)
{
  L7_int32    j; 
  L7_uchar8   mac_addr[6] = {0,2, 0xbc, 00, 00,0x77}, cmd[2];
  L7_char8    VPDBuffer[SERIAL_EEPROM_SIZE];
  L7_ushort16 crc;
  L7_char8    description[80], sn[80], mac_address_input[20];
  L7_RC_t     rc = L7_SUCCESS;
  L7_uchar8   service_tag[64];

  memset ( VPDBuffer, 0, (size_t)SERIAL_EEPROM_SIZE );
  memset ( description, 0, (size_t)sizeof(description));
  memset ( sn, 0, (size_t)sizeof(sn));
  memset ( service_tag, 0, (size_t)sizeof(service_tag));

  do
  {
    printf ("\n\n"
            "Do you wish to input a MAC address?\n"
            "If you choose no, the currently stored\n"
            "MAC address ( "
           );

#ifdef _L7_OS_VXWORKS_
#if SERVICE_PORT_PRESENT==1
    (void)sysEnetAddrGet (SERVICE_PORT_NAME, (int)(0)  /* not used */, mac_addr);
#endif
#endif
    print_mac(mac_addr);
    printf(") will be used. (y/n): "
          );
    if(0 > read(bspapiConsoleFdGet(), cmd, sizeof(cmd))){}
    if ((cmd[0] == 'n') || (cmd[0] == 'N'))
    {
      break;
    }
    else if ((cmd[0] != 'y') && (cmd[0] != 'Y'))
    {
      printf("\r\n"
             "Invalid input.  Please enter 'y' or 'n'.\r\n"
             "Do you wish to input a MAC address (y/n)? ");
      continue;
    }
    /*
    *
    * Request the MAC address for this switch....
    *
    */
    fflush(stdout);
    printf("\n\n"
           "Enter the six byte starting MAC address (example: 000629328140)\n"
           "-> ");
#ifdef _L7_OS_VXWORKS_
    fioRdString(bspapiConsoleFdGet(), mac_address_input, sizeof(mac_address_input));
#else
    if(NULL == fgets(mac_address_input, (L7_int32)sizeof(mac_address_input), stdin)){}
#endif

    /* Remove the carriage return and or linefeed chars */
    if ( strstr (mac_address_input, "\r") )
      *(strstr (mac_address_input, "\r")) = 0;

    if ( strstr (mac_address_input, "\n") )
      *(strstr (mac_address_input, "\n")) = 0;

    if ( strlen (mac_address_input) != 12 )
    {
      printf ("ERROR: Invalid MAC address length.\n\n");
      continue;
    }
    if ( make_mac (mac_address_input, mac_addr) != L7_SUCCESS )
    {
      printf ("ERROR: Invalid characters in the MAC address.\n\n");
      continue;
    }
    printf ("Entered MAC: ");
    print_mac (mac_addr);
    printf ("\n");
    break;
  } while (1);

  do
  {
    printf("\nEnter a serial number to place in the VPD -> ");

#ifdef _L7_OS_VXWORKS_
    fioRdString(bspapiConsoleFdGet(), sn, sizeof(sn));
#else
    if(NULL == fgets(sn, (L7_int32)sizeof(sn), stdin)){}
#endif

    /* Remove the carriage return and or linefeed chars */
    if ( strstr (sn, "\r") )
      *(strstr (sn, "\r")) = 0;

    if ( strstr (sn, "\n") )
      *(strstr (sn, "\n")) = 0;

    for ( j=0; j < (L7_int32)strlen(sn); j++ )
    {
      if ( !isalnum((L7_int32)sn[j]) )
      {
        printf("\nIllegal char %c in input.\n\n",sn[j]);
        break;
      }
    }
    if ( j == (L7_int32)strlen(sn) )
    {
      break; /* good input */
    }
  } while ( 1 );

#if L7_FEAT_SERVICE_TAG
  do
  {
    printf("\nPlease enter the Service Tag for this unit -> ");

#ifdef _L7_OS_VXWORKS_
    fioRdString(bspapiConsoleFdGet(), service_tag, sizeof(service_tag));
#else
    (void)fgets(service_tag, (L7_int32)sizeof(service_tag), stdin);
#endif

    /* Remove the carriage return and or linefeed chars */
    if (strstr(service_tag, "\r"))
      *(strstr(service_tag, "\r")) = 0;

    if (strstr(service_tag, "\n"))
      *(strstr(service_tag, "\n")) = 0;

    for (j = 0; j < (L7_int32)strlen(service_tag); j++)
    {
      if (!isalnum((L7_int32)service_tag[j]))
      {
        printf("\nIllegal char %c in input.\n\n", service_tag[j]);
        break;
      }
    }
    if (j == (L7_int32)strlen(service_tag))
    {
      break; /* good input */
    }
  } while (1);
#endif

  j = 2; /* Skip the CRC bytes */

  VPDBuffer[j++] = (L7_char8)8; 
  VPDBuffer[j++] = (L7_char8)CFD_CARD_BASE_MAC;
  VPDBuffer[j++] = (L7_char8)mac_addr[0];
  VPDBuffer[j++] = (L7_char8)mac_addr[1]; 
  VPDBuffer[j++] = (L7_char8)mac_addr[2];
  VPDBuffer[j++] = (L7_char8)mac_addr[3];
  VPDBuffer[j++] = (L7_char8)mac_addr[4];
  VPDBuffer[j++] = (L7_char8)mac_addr[5];

  VPDBuffer[j++] = (L7_char8)3;
  VPDBuffer[j++] = (L7_char8)CFD_NUMBER_MAC_ADDRESSES;
  VPDBuffer[j++] = (L7_char8)0x80;

  VPDBuffer[j++] = (L7_char8)(strlen(sn) + 3); /* plus 3 for type field and length (max 64 bytes)  + NULL */
  VPDBuffer[j++] = (L7_char8)CFD_SN; 
  strcpy(&VPDBuffer[j], sn);
  j += strlen(sn);
  VPDBuffer[j++] = (L7_char8)0;

#if L7_FEAT_SERVICE_TAG
  VPDBuffer[j++] = (L7_char8)(strlen(service_tag) + 3); /* plus 3 for type field and length (max 64 bytes)  + NULL */
  VPDBuffer[j++] = (L7_char8)CFD_SERVICE_TAG;
  strcpy(&VPDBuffer[j], service_tag);
  j += strlen(service_tag);
  VPDBuffer[j++] = (L7_char8)0;
#endif

  VPDBuffer[j] = (L7_char8)0xB; /* type field + length field + 8 char password + NULL */
  VPDBuffer[j+1] = (L7_char8)CFD_PASSWORD;
  strcpy(&VPDBuffer[j+2], "debug" );

  VPDBuffer[j+10] = (L7_char8)0;

  GenCrcTable ();
  crc = 0;
  for ( j = 2; j < SERIAL_EEPROM_SIZE; j++ )
  {
    UpdateCRC ((L7_uchar8)VPDBuffer[j], &crc);
  }
  VPDBuffer[0] = (L7_char8)((crc & 0xff00) >> 8);
  VPDBuffer[1] = (L7_char8)(crc & 0xff);

  memcpy(pCFD, VPDBuffer, SERIAL_EEPROM_SIZE);

  return rc;

} /* end: RuntimeVPDFileGenerate */

