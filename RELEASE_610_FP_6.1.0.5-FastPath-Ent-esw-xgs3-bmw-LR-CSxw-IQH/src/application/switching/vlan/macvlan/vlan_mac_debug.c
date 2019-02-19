/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_debug.c
*
* @purpose   Mac Vlan Debug file
*
* @component vlanMac
*
* @comments 
*
* @create    5/20/2005
*
* @author    tsrikanth
*
* @end
*             
**********************************************************************/

#include "vlan_mac_include.h"

#define VLAN_MAC_DEFAULT_VLAN    1
extern vlanMacCfg_t *vlanMacCfg;
extern macvlanTree_t *macVlanTreeData;

/* Begin Function Declarations: vlan_mac_debug.h */

/*********************************************************************
* @purpose  Dumps the cfg file
*          
* @param    none 
*
* @returns L7_SUCCESS 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t vlanMacCfgDump()
{
    L7_uint32 i;

    for (i = 0; i < L7_VLAN_MAC_MAX_VLANS; i++)
    {
        if (vlanMacCfg->macVlanInfo[i].vlanId != 0)
            sysapiPrintf("(vlanId, (mac):  %d, [(0x%x):(0x%x):(0x%x):(0x%x):(0x%x):(0x%x)]\n",
                         vlanMacCfg->macVlanInfo[i].vlanId,
                         vlanMacCfg->macVlanInfo[i].mac.addr[0],
                         vlanMacCfg->macVlanInfo[i].mac.addr[1],
                         vlanMacCfg->macVlanInfo[i].mac.addr[2],
                         vlanMacCfg->macVlanInfo[i].mac.addr[3],
                         vlanMacCfg->macVlanInfo[i].mac.addr[4],
                         vlanMacCfg->macVlanInfo[i].mac.addr[5]);
    }
    sysapiPrintf("\n");

    return(L7_SUCCESS);
}


/*********************************************************************
* @purpose  Dumps the informational structures
*          
* @param    none 
*
* @returns  L7_SUCCESS 
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t vlanMacInfoDump()
{
   L7_enetMacAddr_t tmpmac ;
   L7_int32 count = -1;
   vlanMacOperData_t *node;
   
   memset((void *)&tmpmac, 0x00, sizeof tmpmac);
   sysapiPrintf("\n");
   sysapiPrintf("MACADDRESS       VLANID\n");
   osapiSemaTake(macVlanTreeData->semId, L7_WAIT_FOREVER);
   count = avlTreeCount(macVlanTreeData);
   while(count != 0)
   {
      node = avlSearchLVL7(macVlanTreeData, &tmpmac, AVL_NEXT);
	  if (node != L7_NULL)
	  {
		sysapiPrintf("%x\t",node->mac);
		sysapiPrintf("%d\n",node->vlanId);
		count--;
	  }
	  else
	  {
		break;
	  }
	  
    }
    osapiSemaGive(macVlanTreeData->semId);
    return L7_SUCCESS;

}

/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/


/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void
vlanMacBuildTestVlanData(vlanMacCfgData_t *pCfg, L7_uint32 seed)
{
#define L7_VLAN_MAC_DEFAULT_MAC(macbytes, seed) do { \
    (macbytes).addr[0] = 0x00; \
    (macbytes).addr[1] = 0x00; \
    (macbytes).addr[2] = 0x00; \
    (macbytes).addr[3] = 0x00; \
    (macbytes).addr[4] = 0x00; \
    (macbytes).addr[5] = 0x00 + seed; \
} while (0)

    pCfg->vlanId = VLAN_MAC_DEFAULT_VLAN;
    L7_VLAN_MAC_DEFAULT_MAC(pCfg->mac, seed);
}

/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void vlanMacBuildTestConfigData(void)
{
    L7_uint32 cfgIndex;

    /*-------------------------------*/
    /* Build Non-Default Config Data */
    /*-------------------------------*/

    /* NOTE FOR WRITING TEST CONFIG ROUTINES!!!!  
       The index of the loop is used as a seed to
       ensure that each array entry is populated
       with a unique value */

    for (cfgIndex = 0; cfgIndex < L7_VLAN_MAC_MAX_VLANS; cfgIndex++)
        vlanMacBuildTestVlanData(&vlanMacCfg->macVlanInfo[cfgIndex], cfgIndex);

    /* End of Component's Test Non-default configuration Data */

    /* Force write of config file */
    vlanMacCfg->cfgHdr.dataChanged = L7_TRUE;

    sysapiPrintf("Built test config data\n");
}

/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void vlanMacConfigDataTestShow(void)
{

    L7_fileHdr_t *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(vlanMacCfg->cfgHdr);
    sysapiCfgFileHeaderDump(pFileHdr);

    /*--------------------------------*/
    /* cfgParms                       */
    /*--------------------------------*/
    vlanMacCfgDump();

    /*-------------------------------*/
    /* Scaling Constants             */
    /*                               */
    /*                               */
    /* NOTE:                         */
    /* Print any limits of arrays    */
    /* or anything else in the       */
    /* confi files which might       */
    /* affect the size of the config */
    /* file.                         */
    /*                               */
    /*-------------------------------*/

    sysapiPrintf("Scaling Constants\n");
    sysapiPrintf("-----------------\n");
    sysapiPrintf("L7_VLAN_MAC_MAX_VLANS - %d\n",L7_VLAN_MAC_MAX_VLANS);

    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("vlanMacCfg->checkSum: %u\n", vlanMacCfg->checkSum);
}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

/* End Function Declarations */
