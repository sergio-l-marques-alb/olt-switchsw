/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  cpu_init.c
*
* @purpose   Provide cpu intialization functions for the BMW CPU.
*
* @component BSP
*
* @create    06/10/2005
*
* @author    Brady Rutherford
*
* @end
*
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "l7_common.h"
#include "platform_cpu.h"
#include "bspcpu_api.h"
#include "sysapi.h"
#include "cfd.h"
#include "bspapi.h"

#include "immap_cpm2.h"
#include "addrmap.h"
#include "pci.h"
#include "debug.h"

volatile cpm2_map_t *PPC8280RegPtr= NULL;//ponteiro para os registos do processador
TAddrMap      PPC8280RegAddrMap;
unsigned long IMMR_ADDR = 0xFF000000;

L7_uchar8 cfd[SERIAL_EEPROM_SIZE];
L7_int32  cfgValid = L7_FALSE;
L7_int32  cpu_card_id;
/**************************************************************************
*
* @purpose  Read the configuration data from the CPU board's Flash and 
*           populate registry
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
L7_RC_t bspCpuInit(void)
{
  L7_int32 rc = L7_SUCCESS;
  int fd;

  cpu_card_id = CARD_CPU_BMW_REV_1_ID;

  /* Inicializar chipselects */    
  if ( (PPC8280RegPtr = AddrAlloc(&PPC8280RegAddrMap, IMMR_ADDR , 0x00100000)) == MAP_FAILED ) {
    printf("\rSTART_UP: Aloc MPC8280 [ERROR]\n\r");
    return(L7_FAILURE);
  }
  sleep(1);
  PCI_Init();
  sleep(1);
  //PCI_Teste();
  if ( AddrFree(&PPC8280RegAddrMap) ) {
    printf("\rSTART_UP: Free MPC8280 [ERROR]\n\r");
    return(L7_FAILURE);
  }

  fd = open("/dev/nvram",O_RDONLY);
  if(fd < 0)
  {
     printf("Unable to open /dev/nvram (not a fatal error)\n");
     return L7_FAILURE;
  }
  if(lseek(fd,L7_VPD_BUFFER_OFFSET,SEEK_SET) < 0)
  {
     printf("Cannot seek to addr in /dev/nvram\n");
     close(fd);
     return L7_FAILURE;
  }
  if(read(fd, (void *)cfd, SERIAL_EEPROM_SIZE) < 0)
  {
     printf("Cannot read from /dev/nvram\n");
     close(fd);
     return L7_FAILURE;
  }
  close(fd);

  rc = sysapiCheckCfDCRC(( L7_ushort16 )( cfd[0] << 8 | cfd[1] ), (L7_uchar8 *)&cfd[2], SERIAL_EEPROM_SIZE-2);
  if ( rc == L7_ERROR )
  {

    printf("\n\nERROR!\nERROR!\nERROR!\nNVRAM CRC check failed. Boot cannot continue!!\nERROR!\nERROR!\nERROR!\n");

    return rc;
  }

  cfgValid = L7_TRUE;

  return(rc);
}

