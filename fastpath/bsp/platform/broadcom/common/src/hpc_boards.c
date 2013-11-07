/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  hpc_boards.c
*
* @purpose   Initialize storage for use in hpc board database.
*
* @component hpc
*
* @create    05/23/2008
*
* @author    bradyr 
* @end
*
*********************************************************************/


#include <stdio.h>
#include <soc/drv.h>
#include <soc/cm.h>

#include <bcm/error.h>

#include "sysbrds.h"
#include "bcm/stack.h"
#include "registry.h"
#include <appl/stktask/topo_brd.h>

/*
 * Board specific initialization code
 * used by the board definitions below.
 */
#include "ledlink.h"
#include "l7_common.h"
#include "hpc_db.h"
#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm_int/esw/rcpu.h>
#include <bcm/l2.h>
#include "broad_hpc_db.h"
#include "sal/appl/config.h"

bcm_sys_board_t bcm_sys_boards[] = {
#include "hpc_boards.h"
};

/* Ptin added: init */
#include "logger.h"

/* Default ID function */
int
bcm_sys_id_defl(const bcm_sys_board_t *brd)
{
    int i;
    uint16 devid;

    if (soc_ndev != brd->num_units) {
        return FALSE;
    }

    for (i = 0; i < soc_ndev; i++) {
	soc_cm_get_id(i, &devid, NULL);

        if (brd->dev_id[i] !=
            devid) {
            return FALSE;
        }
    }

    return TRUE;
}


/* Default pre-stacking function */
int
bcm_sys_pre_stack_defl(const bcm_sys_board_t *brd)
{
    return BCM_E_NONE;
}

int
bcm_sys_sa_init_defl(const bcm_sys_board_t* brd, int base)
{
    return 0;
}


/* Board Discovery */
const bcm_sys_board_t*
hpcBoardGet(void)
{
    int i;
    for(i = 0; i < sizeof(bcm_sys_boards) / sizeof(bcm_sys_boards[0]); i++) {
	if(bcm_sys_boards[i].brd_id(bcm_sys_boards + i) == 1) {
	    /* We are this board */
	    return bcm_sys_boards + i;
	}
    }
    /* board not found */
    return NULL;
}

/* Find a board by id */
bcm_sys_board_t*
hpcBoardFind(int id)
{
    int i;
    for(i = 0; i < sizeof(bcm_sys_boards) / sizeof(bcm_sys_boards[0]); i++) {
	if(bcm_sys_boards[i].sys_brd_id == id) {
	    /* This is the board we're looking for */
	    return bcm_sys_boards + i;
	}
    }
    /* board not found */
    return NULL;
}

/*********************************************************************
* @purpose  Common PHY configuration
*
* @param    
*                                       
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments these ports are referred to as XPORTS
*       
* @end
*********************************************************************/
extern char * hpcPortNameGet(int cpuUnit, int bcmPort);
L7_RC_t hpcConfigPhySet(void)
{
  L7_char8      configString[64];
  int           portIndex;
  int           portNo;
  L7_uint32     slotIndex = 0;

  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;

  for (slotIndex = 0; slotIndex < L7_MAX_PHYSICAL_SLOTS_PER_UNIT; slotIndex++)
  {

    sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(slotIndex));

    /* Search each line card for SFP capable interfaces */
    if ((sysapiHpcCardInfoPtr != L7_NULLPTR) &&
        (sysapiHpcCardInfoPtr->type == SYSAPI_CARD_TYPE_LINE))
    {
      dapiCardPtr = sysapiHpcCardInfoPtr->dapiCardInfo;
      hapiSlotMapPtr = dapiCardPtr->slotMap;
      for (portIndex = 0; portIndex < sysapiHpcCardInfoPtr->numOfNiPorts; portIndex++)
      {
        portNo = dapiCardPtr->portMap[portIndex].portNum;

        /* Perform any board specific configuration */
        hpcConfigBoardPhySet(portNo, sysapiHpcCardInfoPtr->cardTypeId,
                             hapiSlotMapPtr);

        /*If this port is SFP capable, configure the phy for support */
        if (sysapiHpcCardInfoPtr->portInfo[portNo].phyCapabilities & 
            L7_PHY_CAP_PORTSPEED_SFP)
        {

          sprintf(configString,"phy_5464S_%s.%d",
                  hpcPortNameGet( hapiSlotMapPtr[portNo].bcm_cpuunit,
                  hapiSlotMapPtr[portNo].bcm_port),
                  hapiSlotMapPtr[portNo].bcm_cpuunit);
          sal_config_set(configString,"0x1");

          if (sysapiHpcCardInfoPtr->portInfo[portNo].phyCapabilities & 
              L7_PHY_CAP_PORTSPEED_SFP_DETECT)
          {
            sprintf(configString,"phy_fiber_detect_%s.%d",
                    hpcPortNameGet( hapiSlotMapPtr[portNo].bcm_cpuunit,
                    hapiSlotMapPtr[portNo].bcm_port),
                    hapiSlotMapPtr[portNo].bcm_cpuunit);
            sal_config_set(configString,"-4");
          } 
        }
        hpcConfigBoardPhyPostSet(portNo, sysapiHpcCardInfoPtr->cardTypeId,hapiSlotMapPtr,
                                         sysapiHpcCardInfoPtr->portInfo[portNo].phyCapabilities);
      }
    }
  }
  
  HPC_BROAD_PHY_INIT(); /* phy settings depending on box */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Common driver configuration.
*
* @param    
*                                       
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @end
*********************************************************************/
L7_RC_t hpcConfigSet()
{
  L7_RC_t rc;

  /* Disable seeing help, memory lists, or register lists in the BCM 
     CLI (drivshell). No need to reset the box if these don't work. */
  /* PTin modified: first changes => 0 to 1 */
  (void) sal_config_set(spn_MEMLIST_ENABLE, "1");
  (void) sal_config_set(spn_REGLIST_ENABLE, "1");
  (void) sal_config_set(spn_HELP_CLI_ENABLE, "1");

#ifdef _L7_OS_LINUX_
  /* Use polling on Linux. Was a fixup before */
  if (sal_config_set(spn_SCHAN_INTR_ENABLE, "1") != 0) LOG_ERROR(33); /* PTin modified: 0->1 */
  if (sal_config_set(spn_MIIM_INTR_ENABLE, "1") != 0) LOG_ERROR(33);  /* PTin modified: 0->1 */
#ifdef LVL7_KEYSTONE
  if (sal_config_set(spn_TSLAM_INTR_ENABLE, "0") != 0) LOG_ERROR(33);
#endif
#ifdef LVL7_RAPTOR
  if (sal_config_set(spn_TDMA_INTR_ENABLE, "0") != 0) LOG_ERROR(33);
#endif
#else
  /* SDK does defaults to interrupt mode */
  if (sal_config_set(spn_SCHAN_INTR_ENABLE, "1") != 0) LOG_ERROR(33);
  if (sal_config_set(spn_MIIM_INTR_ENABLE, "1") != 0) LOG_ERROR(33);
#endif
#ifdef PC_LINUX_HOST  
  sysapiPrintf("Setting salconfig for PC_LINUX_HOST\n");
  if (sal_config_set(spn_L2XMSG_THREAD_USEC,"0") != 0) LOG_ERROR(33);
  if (sal_config_set(spn_BCM_LINKSCAN_INTERVAL,"0") != 0) LOG_ERROR(33);
  if (sal_config_set(spn_BCM_STAT_INTERVAL,"60000000") != 0) LOG_ERROR(33);
#endif

  /* Set property to skip initialization of BPDU addresses in L2 User table.
   * For BPDUs, the system policy redirects them to CPU (local or remote).
   */
  if (sal_config_set(spn_SKIP_L2_USER_ENTRY,"1") != 0) LOG_ERROR(33); 

  /* Ptin removed: init */
  #if 1
  LOG_NOTICE(LOG_CTX_STARTUP,"hpcXeHgSetup/hpcGeHlSetup will not be executed here!");
  #else
  /* setup the xe/hg combo ports */
  hpcXeHgSetup();
  hpcGeHlSetup();
  #endif

#ifdef L7_STACKING_PACKAGE
  if (sal_config_set(spn_L2XMSG_MODE,"0") != 0) LOG_ERROR(33);
#endif

  if (sal_config_set(spn_BCM_NUM_COS,"8") != 0) LOG_ERROR(33);

  /* Turn-off dual modid feature */
  if (sal_config_set(spn_MODULE_64PORTS,"1") != 0) LOG_ERROR(33);

  /* PTin modified: log error on failure! */
  rc = hpcConfigBoardSet();
  if (rc != L7_SUCCESS)
     LOG_ERROR(33);

  /* Ptin added: init */
  #if 1
  LOG_NOTICE(LOG_CTX_STARTUP,"hpcXeHgSetup/hpcGeHlSetup will be executed now!");
  /* setup the xe/hg combo ports */
  hpcXeHgSetup();
  hpcGeHlSetup();
  #endif

  return(L7_SUCCESS);
}

