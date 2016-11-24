/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename macal_cfg.c
*
* @purpose Management Access Control and Administration List configuration functions
*
* @component Management Access Control and Administration List
*
* @comments none
*
* @create 05/05/2005
*
* @author stamboli
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "commdefs.h"
#include "defaultconfig.h"
#include "nvstoreapi.h"
#include "sysapi.h"
#include "macal.h"

void            *macalQueue      = L7_NULLPTR;
macalCfgData_t  *macalCfgData    = L7_NULLPTR;
/*void            *macalSemaphore  = L7_NULLPTR;*/

/*********************************************************************
* @purpose  Saves Management Access Control and Administration List file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 macalSave(void)
{
  if (macalCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    macalCfgData->cfgHdr.dataChanged = L7_FALSE;

    macalCfgData->checkSum = nvStoreCrc32((L7_char8 *)macalCfgData, 
                                         (sizeof(macalCfgData_t) - sizeof(macalCfgData->checkSum)));

    if (sysapiCfgFileWrite(L7_MGMT_ACAL_COMPONENT_ID, MACAL_CFG_FILENAME,
                           (L7_char8 *)macalCfgData, sizeof(macalCfgData_t)) != L7_SUCCESS)
    {
      LOG_MSG("macalSave: Error on call to sysapiCfgFileWrite file %s\n", MACAL_CFG_FILENAME);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if Management Access Control and Administration List config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL macalHasDataChanged(void)
{
  return macalCfgData->cfgHdr.dataChanged;
}

/*********************************************************************
* @purpose  Apply Management Access Control and Administration List Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t macalApplyConfigData(void)
{
  L7_BOOL bRegister;
  
  bRegister = ((macalCfgData->macalList.activationStatus == L7_ACTIVATED) || (macalCfgData->consoleOnly == L7_TRUE)) \
               ? L7_TRUE : L7_FALSE;

  /* Sysnet register/deregister for Management Access Control and Administration List */
  (void)macalSysnetRegisterDeregister(bRegister);  

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default Management Access Control and Administration List config data  
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
void macalBuildDefaultConfigData(L7_uint32 ver)
{
  memset((void *)macalCfgData, 0, sizeof(macalCfgData_t));

  /* if the MACAL feature is enabled. It should always be true. It's for an SNMP object support. */
  macalCfgData->enabled = L7_TRUE;

  strncpy(macalCfgData->cfgHdr.filename, MACAL_CFG_FILENAME, L7_MAX_FILENAME);

  macalCfgData->cfgHdr.version = ver;

  macalCfgData->cfgHdr.componentID = L7_MGMT_ACAL_COMPONENT_ID;

  macalCfgData->cfgHdr.type = L7_CFG_DATA;

  macalCfgData->cfgHdr.length = sizeof(macalCfgData_t);

  macalCfgData->cfgHdr.dataChanged = L7_FALSE;
  
  (void) macalBuildDefaultRules(ALL_RULES);
  macalCfgData->consoleOnly = L7_FALSE;

  return;
}

/*********************************************************************
* @purpose  Build default Management Access Control and Administration List rules data  
*
* @param    ruleNum   (input) rule number
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
void macalBuildDefaultRules(L7_uint32 ruleNum)
{
  L7_uint32 i;

  if (ruleNum == ALL_RULES) 
  {
    /* Defaults for all the rules */
    for (i = 0; i < (MACAL_MAX_NUM_RULES + 1); i++)
    {  
      macalCfgData->macalList.rule[i].action =  MACAL_ACTION_DENY;
      macalCfgData->macalList.rule[i].activationStatus = L7_DEACTIVATED;
      macalCfgData->macalList.rule[i].configMask = L7_NULL;
      macalCfgData->macalList.rule[i].ifNum = L7_NULL;
      macalCfgData->macalList.rule[i].serviceType = MACAL_PROTOCOL_NONE;
      macalCfgData->macalList.rule[i].srcIp = L7_NULL;
      macalCfgData->macalList.rule[i].srcMask = ~(L7_NULL);  /* 255.255.255.255 */
      macalCfgData->macalList.rule[i].vlanId = L7_NULL;
    }
  }
  else /* Assumption: the ruleNum sent is within array limits */
  {   
    /* Defaults for a specific rule */
    macalCfgData->macalList.rule[ruleNum].action = MACAL_ACTION_DENY;
    macalCfgData->macalList.rule[ruleNum].activationStatus = L7_DEACTIVATED;
    macalCfgData->macalList.rule[ruleNum].configMask = L7_NULL;
    macalCfgData->macalList.rule[ruleNum].ifNum = L7_NULL;
    macalCfgData->macalList.rule[ruleNum].serviceType = MACAL_PROTOCOL_NONE;
    macalCfgData->macalList.rule[ruleNum].srcIp = L7_NULL;
    macalCfgData->macalList.rule[ruleNum].srcMask = ~(L7_NULL);  /* 255.255.255.255 */
    macalCfgData->macalList.rule[ruleNum].vlanId = L7_NULL;
  }
}
