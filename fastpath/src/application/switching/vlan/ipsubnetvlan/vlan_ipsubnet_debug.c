/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_debug.c
*
* @purpose   IP Subnet Vlan Debug file
*
* @component vlanIpSubnet
*
* @comments 
*
* @create    5/5/2005
*
* @author    tsrikanth
*
* @end
*             
**********************************************************************/

#include "vlan_ipsubnet_include.h"

#define VLAN_IPSUBNET_DEFAULT_VLAN    1
extern vlanIpSubnetCfg_t *vlanIpSubnetCfg;

/* Begin Function Declarations: vlan_ipsubnet_debug.h */

/*********************************************************************
* @purpose  Dumps the cfg file
*          
* @param    none 
*
* @returns  L7_SUCCESS
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t vlanIpSubnetCfgDump()
{
    L7_uint32 i;

    for (i = 0; i < L7_VLAN_IPSUBNET_MAX_VLANS; i++)
    {
        if (vlanIpSubnetCfg->vlanIpSubnetCfgData[i].ipSubnet != 0)
            sysapiPrintf("(vlanId, (subnet), (netmask):  %d, (0x%x), (0x%x\n",
                         vlanIpSubnetCfg->vlanIpSubnetCfgData[i].vlanId,
                         vlanIpSubnetCfg->vlanIpSubnetCfgData[i].ipSubnet,
                         vlanIpSubnetCfg->vlanIpSubnetCfgData[i].netMask);
    }
    sysapiPrintf("\n");

    return (L7_SUCCESS);
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
L7_RC_t vlanIpSubnetInfoDump()
{
   L7_uchar8 searchKey[VLAN_IPSUBNET_KEYSIZE] = {0};
   
   L7_int32 count = -1;
   vlanIpSubnetOperData_t *node;

    sysapiPrintf("\n");
    sysapiPrintf(" SUBNET     NETMASK     VLANID\n");
    osapiSemaTake(vlanIpSubnetTreeData->semId, L7_WAIT_FOREVER);
    count = avlTreeCount(vlanIpSubnetTreeData);
    while(count != 0) 
    {
      node = avlSearchLVL7(vlanIpSubnetTreeData, searchKey, AVL_NEXT);
      
	  if (node != L7_NULL)
	  {
      sysapiPrintf("%x\t",node->ipSubnet);
      sysapiPrintf("%x\t",node->netMask);
      sysapiPrintf("%d\n",node->vlanId);
      count--;
      memcpy(&searchKey[0], &node->ipSubnet, 4);
      memcpy(&searchKey[4], &node->netMask, 4);
      
	  }
	  else
	  {
		break;
	  }
    }
    osapiSemaGive(vlanIpSubnetTreeData->semId);
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
vlanIpSubnetBuildTestVlanData(vlanIpSubnetCfgData_t *pCfg, L7_uint32 seed)
{
    char defaultIpSubnet[16];

    pCfg->vlanId = VLAN_IPSUBNET_DEFAULT_VLAN + seed;
    sprintf(defaultIpSubnet, "%d.%d.%d.%d", 192, 168, seed, 0);
    pCfg->ipSubnet = osapiInet_addr(defaultIpSubnet);
    pCfg->netMask = osapiInet_addr("255.255.255.0");
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
void vlanIpSubnetBuildTestConfigData(void)
{
    L7_uint32 cfgIndex;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

    /* NOTE FOR WRITING TEST CONFIG ROUTINES!!!!  
       The index of the loop is used as a seed to
       ensure that each array entry is populated
       with a unique value */

    for (cfgIndex = 0; cfgIndex < L7_VLAN_IPSUBNET_MAX_VLANS; cfgIndex++)
        vlanIpSubnetBuildTestVlanData(&vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex], cfgIndex);

    /* End of Component's Test Non-default configuration Data */

    /* Force write of config file */
    vlanIpSubnetCfg->cfgHdr.dataChanged = L7_TRUE;

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
void
vlanIpSubnetConfigDataTestShow(void)
{

    L7_fileHdr_t *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(vlanIpSubnetCfg->cfgHdr);
    sysapiCfgFileHeaderDump(pFileHdr);

   /*--------------------------------*/
    /* cfgParms                       */
   /*--------------------------------*/
    vlanIpSubnetCfgDump();

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
    sysapiPrintf("L7_IPSUBNET_MAX_VLANS - %d\n",L7_VLAN_IPSUBNET_MAX_VLANS);

    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("vlanIpSubnetCfg->checkSum: %u\n", vlanIpSubnetCfg->checkSum);
}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

/* End Function Declarations */
