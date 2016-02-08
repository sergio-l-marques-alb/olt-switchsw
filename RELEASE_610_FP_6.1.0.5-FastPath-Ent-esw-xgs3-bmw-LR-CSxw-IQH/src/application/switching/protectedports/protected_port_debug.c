/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_debug.c
*
* @purpose   Protected Port Debug file
*
* @component protectedPort
*
* @comments 
*
* @create    6/6/2005
*
* @author    ryadagiri 
*
* @end
*             
**********************************************************************/

#include "protected_port_include.h"

extern protectedPortCfg_t *protectedPortCfg;


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
void protectedPortBuildTestData(protectedPortGroup_t * pCfg, 
                                    L7_uint32 intIfNum)
{
    L7_uint32 maskOffset;

    if (nimConfigIdMaskOffsetGet(intIfNum, &maskOffset) != L7_SUCCESS)
        return;

    L7_INTF_SETMASKBIT(pCfg->protectedPortPortMask, maskOffset);
    return;
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
void protectedPortBuildTestConfigData(void)
{
    L7_uint32 cfgIndex;

    /*-------------------------------*/
    /* Build Non-Default Config Data */
    /*-------------------------------*/

    /* NOTE FOR WRITING TEST CONFIG ROUTINES!!!!  
       The index of the loop is used as a seed to
       ensure that each array entry is populated
       with a unique value */

    for (cfgIndex = 0; cfgIndex < L7_PROTECTED_PORT_MAX_GROUPS; cfgIndex++)
        protectedPortBuildTestData(&protectedPortCfg->
                                       protectedPortGroup[cfgIndex], cfgIndex+1);

    /* End of Component's Test Non-default configuration Data */

    /* Force write of config file */
    protectedPortCfg->cfgHdr.dataChanged = L7_TRUE;

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
void protectedPortConfigDataTestShow(void)
{

    L7_fileHdr_t *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(protectedPortCfg->cfgHdr);
    sysapiCfgFileHeaderDump(pFileHdr);

    /*--------------------------------*/
    /* cfgParms                       */
    /*--------------------------------*/
    protectedPortCfgDump();

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


    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("protectedPortCfg->checkSum: %u\n",
                 protectedPortCfg->checkSum);
}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

/*********************************************************************
* @purpose  Dumps the cfg file
*
* @param    none
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t protectedPortCfgDump()
{
    L7_uint32 intIfNum, extIfNum, groupId;

    for (groupId = 0; groupId < L7_PROTECTED_PORT_MAX_GROUPS; groupId++)  
    {
        sysapiPrintf("GroupId Name: %s\n", protectedPortCfg->protectedPortGroup->protectedPortName);
        for (intIfNum = 1; intIfNum <= L7_MAX_INTERFACE_COUNT; intIfNum++)
        {
            if (L7_INTF_ISMASKBITSET(protectedPortCfg->
                       protectedPortGroup[groupId].protectedPortPortMask, intIfNum))
            {
                if (nimGetIntfIfIndex(intIfNum, &extIfNum) == L7_SUCCESS)
                {
                    sysapiPrintf("intIfNum:  %d (0x%x)\n", extIfNum, extIfNum);
                }
            }
        }
        sysapiPrintf( "\n");
    }
    sysapiPrintf( "\n");

    return L7_SUCCESS;
}

void protectedPortDebugBitMaskDump(L7_INTF_MASK_t tempmask)
{
        int index;
        int i;
        char tmp;

        for (index = 0; index <L7_INTF_INDICES ; index++)
        {
            if (tempmask.value[index]) {
                tmp = tempmask.value[index];
                for (i = 0; i < 8; i++) if ((tmp << i) & 0x80)
                        sysapiPrintf("pos (%d), is set to 1.\n", index*8+i+1);
            }
        }
}

void protectedPortDebugInfoShow ()
{
  L7_uint32 intIfNum;
  L7_uint32 index ;
  L7_ushort16 i ;
  char tmp;
  for ( intIfNum = 0 ; intIfNum < L7_INTF_INDICES ; intIfNum++)
  {
    for ( index = 0; index <COMPONENT_INDICES; index++)
    {
      if(protectedPortIntfInfo[intIfNum].acquiredList.value[index])
      {
        tmp = protectedPortIntfInfo[intIfNum].acquiredList.value[index];
        for( i = 0 ; i < 8; i++) if ((tmp << 1) & 0x80)
                 sysapiPrintf("pos (%d), is set to 1.\n", index*8+i+1);
      }
    }
  } 
}

void protectedPortMapDebugHelp(void)
{
  sysapiPrintf ("\nList of protectedPortMapDebug... functions ->\n\n");

  sysapiPrintf ("protectedPortDebugInfoShow() - Display operational information\n");
  sysapiPrintf ("protectedPortCfgDump() - Display current config information\n");
}


/* End Function Declarations */
