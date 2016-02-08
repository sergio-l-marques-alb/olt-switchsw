/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_cfg.c
*
* @purpose   Protected Port configuration file
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

/* Operational information of the interfaces */
protectedPortIntfInfo_t  *protectedPortIntfInfo = L7_NULLPTR;

extern osapiRWLock_t protectedPortCfgRWLock;

/* Begin Function Declarations: protected_port_cfg.h */

/*********************************************************************
* @purpose  Builds the cfg file with default parameter values
*          
* @param    ver   @{(input)} Software version of Config Data
*
* @returns  void  
*
* @comments 
*
* @end
*********************************************************************/
void protectedPortBuildDefaultConfigData(L7_uint32 ver)
{
    L7_uint32 groupId,intIfNum;

    memset((void *) protectedPortCfg, 0, sizeof(protectedPortCfg_t));

    for(intIfNum = 0; intIfNum<L7_MAX_INTERFACE_COUNT; intIfNum++)
    {
      memset(&protectedPortIntfInfo[intIfNum].acquiredList, 0x00,
           sizeof(AcquiredMask));
    } 

    for (groupId = 0; groupId < L7_PROTECTED_PORT_MAX_GROUPS; groupId++)
    {
       memset(&protectedPortCfg->protectedPortGroup[groupId].protectedPortPortMask, 
              0x00, sizeof(L7_INTF_MASK_t)); 
       strcpy(protectedPortCfg->protectedPortGroup[groupId].protectedPortName,
               FD_PROTECTED_PORT_GROUP_NAME);
    }
    
    strcpy((L7_char8 *) protectedPortCfg->cfgHdr.filename,
           PROTECTED_PORT_CFG_FILENAME);
    protectedPortCfg->cfgHdr.version = ver;
    protectedPortCfg->cfgHdr.componentID = L7_PROTECTED_PORT_COMPONENT_ID;
    protectedPortCfg->cfgHdr.type = L7_CFG_DATA;
    protectedPortCfg->cfgHdr.length = (L7_uint32) sizeof(protectedPortCfg_t);
    protectedPortCfg->cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Build default protected port interface mask config data
*
* @parms    configId   @b{(input)} the config Id to be placed into the cfg config
* @parms    pCfg       @b{(input)}  a pointer to the protected port cfg structure
*
* @returns  none
*
*
* @end
*********************************************************************/

/*********************************************************************
* @purpose  Has the data in the cfg file changed
*          
* @param    none 
*
* @returns  L7_TRUE  
* @returns  L7_FALSE  
*
* @comments 
*
* @end
*********************************************************************/
L7_BOOL protectedPortHasDataChanged()
{
    return protectedPortCfg->cfgHdr.dataChanged;
}
void protectedPortResetDataChanged(void)
{
  protectedPortCfg->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Restores the default configuration
*          
* @param    none 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t protectedPortRestore()
{
    /* Remove */
    protectedPortCfgRemove();
    /* Build defaults */
    protectedPortBuildDefaultConfigData(PROTECTED_PORT_CFG_VER_CURRENT);
    /* Apply defaults */
    protectedPortApplyConfigData();

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Saves the current configuration
*          
* @param    none 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t protectedPortSave()
{
    L7_RC_t rc = L7_SUCCESS;

    if (protectedPortCfg->cfgHdr.dataChanged == L7_TRUE)
    {
        protectedPortCfg->cfgHdr.dataChanged = L7_FALSE;
        protectedPortCfg->checkSum = nvStoreCrc32((L7_char8 *) protectedPortCfg,
                                                  ((L7_uint32) sizeof(protectedPortCfg_t) -
                                                   (L7_uint32) sizeof(protectedPortCfg->checkSum)));

        if ((rc = sysapiCfgFileWrite(L7_PROTECTED_PORT_COMPONENT_ID,
                                     PROTECTED_PORT_CFG_FILENAME, (L7_char8 *) protectedPortCfg,
                                     (L7_int32) sizeof(protectedPortCfg_t))) != L7_SUCCESS)
        {
            rc = L7_ERROR;
			L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_PROTECTED_PORT_COMPONENT_ID, "Protected Port: failed to save "
						 "configuration. This appears when the protected port configuration cannot be saved");
        }
    }
    return rc;
}

/*********************************************************************
* @purpose  Applies the configured saved configuration
*          
* @param    none 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t protectedPortApplyConfigData()
{
    L7_uint32 groupId;
    L7_INTF_MASK_t intfMask;

    if (PROTECTED_PORT_IS_READY == L7_TRUE)
    {
        for (groupId = 0; groupId <L7_PROTECTED_PORT_MAX_GROUPS ; groupId++)
        {
            memcpy(&intfMask,
                   &protectedPortCfg->protectedPortGroup[groupId].protectedPortPortMask,
                   L7_INTF_INDICES);

            /* Any error  logging will take place in underlying layers */
            (void) (protectedPortGroupCreateApply(groupId, intfMask));
        }

    }
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Remove any configured protected port interface mask
*          
* @param    none 
*
* @returns  void
*
* @comments 
*
* @end
*********************************************************************/
void protectedPortCfgRemove()
{
    L7_uint32 groupId, intIfNum;
    L7_INTF_MASK_t intfMask;

    memset(&intfMask, 0, sizeof( intfMask));
    for (groupId = 0; groupId < L7_PROTECTED_PORT_MAX_GROUPS; groupId++)
    {
        memcpy(&intfMask,
               &protectedPortCfg->protectedPortGroup[groupId].protectedPortPortMask,
                L7_INTF_INDICES);
        /* Any error  logging will take place in underlying layers */
        (void) (protectedPortGroupDeleteApply(groupId));

        memset((void *) &protectedPortCfg->protectedPortGroup[groupId].
               protectedPortPortMask, 0x00, sizeof(L7_INTF_MASK_t));
    }

   for(intIfNum = 0; intIfNum<L7_MAX_INTERFACE_COUNT; intIfNum++)
   {
      memset(&protectedPortIntfInfo[intIfNum].acquiredList, 0x00,
           sizeof(AcquiredMask));
   }

   
}

/* End Function Declarations */

