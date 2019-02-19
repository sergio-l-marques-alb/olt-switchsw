/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  cpu_utils.c
*
* @purpose   Provide cpu utility functions for the BMW CPU.
*
* @component BSP
*
* @create    04/30/2005
*
* @author    Brady Rutherford
*
* @end
*
*********************************************************************/
#include <stdio.h>
#include <string.h>

#include "l7_common.h"
#include "platform_cpu.h"
#include "registry.h"
#include "sysapi.h"
#include "cfd.h"
#include "readenv.h"  // PTin added

#ifdef _L7_OS_VXWORKS_
#include "platform_proto.h"
#endif

extern L7_uchar8 cfd[SERIAL_EEPROM_SIZE];
extern L7_int32  cfgValid;

/**************************************************************************
*
* @purpose  Read the configuration data from the CPU board's Flash and populate registry
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if CRC is bad
*
* @comments    none.
*
* @end
*
*************************************************************************/
static L7_RC_t bmwPopulateRegistry(void)
{
  L7_int32 rc;
  L7_char8 tempstring[40];
  L7_uint32 icache, dcache;
  L7_uchar8 temp8;

  rc = getCFDField( CFD_CPU_TYPE, ( L7_char8 *)&cfd, ( void * )&temp8);
  if ( rc != L7_SUCCESS )
  {
    temp8 = MPC603; /* assume... */
  }
  switch ( temp8 )
  {
    case MPC850 :
      icache = 2*1024;
      dcache = 1*1024;
      break;
    case MPC860P :
      icache = 16*1024;
      dcache = 8*1024;
      break;
    case MPC860T :
      icache = 4*1024;
      dcache = 4*1024;
      break;
    case MPC855T :
      icache = 4*1024;
      dcache = 4*1024;
      break;
    case MPC603 :
      icache = 16*1024;
      dcache = 16*1024;
      break;
    default:
      break;
  }
  (void)sysapiRegistryPut((L7_uint32) CPU_I_CACHE, (L7_uint32) U32_ENTRY, (void *) &icache);
  (void)sysapiRegistryPut((L7_uint32) CPU_D_CACHE, (L7_uint32) U32_ENTRY, (void *) &dcache);


  if ( getCFDField( CFD_FRU, ( L7_char8 *)&cfd, ( void * )&tempstring[0]) == L7_SUCCESS )
  {
    (void)sysapiRegistryPut((L7_uint32) FRU, (L7_uint32) STR_ENTRY, (void *) &tempstring[0]);
  }
  if ( getCFDField( CFD_DESC, ( L7_char8 *)&cfd, ( void * )&tempstring[0]) == L7_SUCCESS )
  {
    (void)sysapiRegistryPut((L7_uint32) SYSTEM_DESC, (L7_uint32) STR_ENTRY, (void *) &tempstring[0]);
  }

  return(L7_SUCCESS);
}

/**************************************************************************
* @purpose  Read the CPU's serial number. If the serial number cannot be read,
*           do not update serialString.
*
* @param    serialString - pointer to serial number string
*
* @returns  L7_SUCCESS - serial number read.
* @returns  L7_FAILURE - Unable to read the serial number.
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuReadSerialNum(L7_char8 *serialString)
{
  L7_RC_t  rc = L7_SUCCESS;

  // PTin added
  // Open environment variables access
  open_bootenv();
  read_bootenv("serial#",serialString);
  close_bootenv();

  printf("Serial=\"%s\"\r\n",serialString);

//if (cfgValid == L7_TRUE)
//{
//  /* Read box serial number */
//  getCFDField(CFD_SN, cfd, serialString);
//  rc = L7_SUCCESS;
//}
  return(rc);
}

/**************************************************************************
* @purpose  Read the CPU's MAC address. If MAC address cannot be read,
*           do not update macString.
*
* @param    macString - pointer to MAC address string
* @param    macString - pointer to MAC address count
*
* @returns  L7_SUCCESS - MAC address read.
* @returns  L7_FAILURE - Unable to read the MAC address
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuReadMac(L7_char8 *macString, L7_uchar8 *macCount)
{
  L7_RC_t  rc;
  char mac_addr[BOOTENV_MAX]="";
  L7_int addr[6]={0x00,0x01,0x01,0x02,0x02,0x03};

  rc = bmwPopulateRegistry();
#ifdef _L7_OS_LINUX_
  // PTin removed
//if (cfgValid == L7_FALSE)
//{
//  rc = L7_FAILURE;
//}
//else
//{
    /* Read box mac address and mac address count */
    getCFDField( CFD_CARD_BASE_MAC, cfd, macString);
    getCFDField( CFD_NUMBER_MAC_ADDRESSES, cfd, macCount);

    // Open environment variables access
    open_bootenv();
    if (read_bootenv("ethaddr", mac_addr)==0)  {
      if (sscanf(mac_addr,"%x:%x:%x:%x:%x:%x",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5])!=6)  {
        addr[0]=0x00;
        addr[1]=0x01;
        addr[2]=0x01;
        addr[3]=0x02;
        addr[4]=0x02;
        addr[5]=0x03;
      }
    }
    close_bootenv();

    macString[0] = addr[0];
    macString[1] = addr[1];
    macString[2] = addr[2];
    macString[3] = addr[3];
    macString[4] = addr[4];
    macString[5] = addr[5];
    *macCount = 32;

    printf("MAC=%02X:%02X:%02X:%02X:%02X:%02X (%u)\r\n",macString[0],macString[1],macString[2],macString[3],macString[4],macString[5],*macCount);
  //}
#else
  if ( sysEnetAddrGet ("bc", 0, macString) == ERROR )
  {
    printf("\n\nERROR! System MAC address in NVRAM is corrupt!\n\n");
    rc = L7_FAILURE;
  }
  *macCount = 128;
#endif
  return(rc);

}

/*********************************************************************
* @purpose  Sets the stack ID LEDs
*
* @comments Broadcom reference platforms have no stack indicator so
*           this is simply a stub function.
*
* @end
*********************************************************************/
void sysStackLedSet(L7_BOOL master, L7_uint32 unit_number)
{

}

/*********************************************************************
* @purpose  Returns the card index of the slot requested
*
* @param    slotNum @b{(input)} requested slot ID
*
* @returns  card index of slot requested. (-1 if no card present)
*
* @end
*********************************************************************/
int bspCpuReadCardID(L7_uint32 slotNum)
{
  int cardIndex = -1;

  switch (slotNum)
  {
    /* BMW's have one non-pluggable card in slot 0 */
    case 0:
      cardIndex = 0;
      break;

    default:
      break;
   }

   return cardIndex;
}

