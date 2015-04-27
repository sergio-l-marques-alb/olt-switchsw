/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_prvtmib_api.c
*
* @purpose    DiffServ component Private MIB General Status Group 
*             API implementation.
*
* @component  DiffServ
*
* @comments   none
*
* @create     04/22/2002
*
* @author     rjindal
* @author     gpaussa
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_diffserv_include.h"

/* external references */
extern dsmibGenStatusGroup_t   genStatus;       /* General Status Group */


/*
==========================
==========================
==========================

   GENERAL STATUS GROUP

==========================
==========================
==========================
*/

/*********************************************************************
* @purpose  Get the value of the DiffServ administrative mode  
*
* @param    pMode       @{(output)} Pointer to mode value
*
* @returns  void
*
* @notes    The DiffServ admin mode is maintained in the General Status Group.
*
* @notes    Only outputs a value if the pMode parm is non-null.
*
* @end
*********************************************************************/
void diffServAdminModeGet(L7_uint32* pMode)
{
  /* caller is expected to provide a non-null ptr if it wants a value */
  if (pMode != L7_NULLPTR)
    *pMode = genStatus.adminMode;
}

/*************************************************************************
* @purpose  Set the value of the DiffServ administrative mode
*
* @param    mode        @b{(input)} Mode value (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The DiffServ admin mode is maintained in the General Status Group.
*
* @end
*********************************************************************/
L7_RC_t diffServAdminModeSet(L7_uint32 mode)
{
  L7_uint32     prevMode, prevDataChanged;
  L7_RC_t       rc;

  /* check inputs */
  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  /* if set value is same as current mode, assume nothing more to do here */
  if (mode == genStatus.adminMode)
    return L7_SUCCESS;

  /* save existing settings for restoration after failure */
  prevMode = genStatus.adminMode;
  prevDataChanged = pDiffServCfgData_g->cfgHdr.dataChanged;

  /* handle mode change
   *
   * NOTE: adminMode must be set to L7_ENABLE BEFORE applying enable mode.
   *       adminMode must be set to L7_DISABLE AFTER applying disable mode.
   */
  if (mode == L7_ENABLE)
  {
    genStatus.adminMode = mode;
    pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;
    rc = diffServAdminModeApply(L7_ENABLE);
  }
  else
  {
    rc = diffServAdminModeApply(L7_DISABLE);
    genStatus.adminMode = mode;
    pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }

  /* restore prior setting in the event of a failure
   *
   * NOTE: adminMode is properly set in either case for re-applying the 
   *       previous mode.
   */
  if (rc != L7_SUCCESS)
  {
    (void)diffServAdminModeApply(prevMode);  /* don't change rc here */
    genStatus.adminMode = prevMode;
    pDiffServCfgData_g->cfgHdr.dataChanged = prevDataChanged;
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the specified object from the General Status Group 
*
* @param    oid         @b{(input)}  Object ID
* @param    pValue      @b{(output)} Pointer to the object output value
*  
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServGenObjectGet(L7_DIFFSERV_GEN_STATUS_GROUP_OBJECT_t oid,
                             L7_uint32 *pValue)
{
  L7_uint32     val;

  /* check inputs */
  if ((oid <= L7_DIFFSERV_GEN_STATUS_TABLE_NONE) || 
      (oid >= L7_DIFFSERV_GEN_STATUS_TABLE_TOTAL))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* access appropriate value per object id
   * NOTE: The Policy Perf In and Out table attributes are not defined
   *       in the private MIB, since they shadow the size of the Policy 
   *       Instance table (although these values are maintained
   *       internally by the implementation).
   */
  switch (oid)
  {
  case L7_DIFFSERV_GEN_STATUS_TABLE_CLASS_SIZE:
    val = genStatus.tableSize[DSMIB_TABLE_ID_CLASS];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_CLASS_MAX:
    val = genStatus.tableMax[DSMIB_TABLE_ID_CLASS];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_CLASS_RULE_SIZE:
    val = genStatus.tableSize[DSMIB_TABLE_ID_CLASS_RULE];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_CLASS_RULE_MAX:
    val = genStatus.tableMax[DSMIB_TABLE_ID_CLASS_RULE];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_SIZE:
    val = genStatus.tableSize[DSMIB_TABLE_ID_POLICY];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_MAX:
    val = genStatus.tableMax[DSMIB_TABLE_ID_POLICY];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_INST_SIZE:
    val = genStatus.tableSize[DSMIB_TABLE_ID_POLICY_INST];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_INST_MAX:
    val = genStatus.tableMax[DSMIB_TABLE_ID_POLICY_INST];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_ATTR_SIZE:
    val = genStatus.tableSize[DSMIB_TABLE_ID_POLICY_ATTR];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_POLICY_ATTR_MAX:
    val = genStatus.tableMax[DSMIB_TABLE_ID_POLICY_ATTR];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_SERVICE_SIZE:
    val = genStatus.tableSize[DSMIB_TABLE_ID_SERVICE];
    break;

  case L7_DIFFSERV_GEN_STATUS_TABLE_SERVICE_MAX:
    val = genStatus.tableMax[DSMIB_TABLE_ID_SERVICE];
    break;

  default:
    /* invalid oid */
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  *pValue = val;
  return L7_SUCCESS;
}


/*
========================================
========================================
========================================

   GENERAL STATUS DEBUG SHOW FUNCTION

========================================
========================================
========================================
*/

/* common display string for debug show functions (matches enum defs) */

/*********************************************************************
* @purpose  Display the current General Status Group contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibGenStatusShow(void)
{
  L7_uint32     msgLvlReqd;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ General Status Group:\n\n");

  DIFFSERV_PRT(msgLvlReqd, 
               "Admin mode............................  %s\n",
               (genStatus.adminMode == L7_ENABLE) ? "Enable" : "Disable");

  DIFFSERV_PRT(msgLvlReqd, 
               "Class Table size curr/max.............  %u/%u\n",
               genStatus.tableSize[DSMIB_TABLE_ID_CLASS],
               genStatus.tableMax[DSMIB_TABLE_ID_CLASS]);

  DIFFSERV_PRT(msgLvlReqd, 
               "Class Rule Table size curr/max........  %u/%u\n",
               genStatus.tableSize[DSMIB_TABLE_ID_CLASS_RULE],
               genStatus.tableMax[DSMIB_TABLE_ID_CLASS_RULE]);

  DIFFSERV_PRT(msgLvlReqd, 
               "Policy Table size curr/max............  %u/%u\n",
               genStatus.tableSize[DSMIB_TABLE_ID_POLICY],
               genStatus.tableMax[DSMIB_TABLE_ID_POLICY]);

  DIFFSERV_PRT(msgLvlReqd, 
               "Policy Inst Table size curr/max.......  %u/%u\n",
               genStatus.tableSize[DSMIB_TABLE_ID_POLICY_INST],
               genStatus.tableMax[DSMIB_TABLE_ID_POLICY_INST]);

  DIFFSERV_PRT(msgLvlReqd, 
               "Policy Attr Table size curr/max.......  %u/%u\n",
               genStatus.tableSize[DSMIB_TABLE_ID_POLICY_ATTR],
               genStatus.tableMax[DSMIB_TABLE_ID_POLICY_ATTR]);

  DIFFSERV_PRT(msgLvlReqd, 
               "Policy Perf In Table size curr/max....  %u/%u\n",
               genStatus.tableSize[DSMIB_TABLE_ID_POLICY_PERF_IN],
               genStatus.tableMax[DSMIB_TABLE_ID_POLICY_PERF_IN]);

  DIFFSERV_PRT(msgLvlReqd, 
               "Policy Perf Out Table size curr/max...  %u/%u\n",
               genStatus.tableSize[DSMIB_TABLE_ID_POLICY_PERF_OUT],
               genStatus.tableMax[DSMIB_TABLE_ID_POLICY_PERF_OUT]);

  DIFFSERV_PRT(msgLvlReqd, 
               "Service Table size curr/max...........  %u/%u\n",
               genStatus.tableSize[DSMIB_TABLE_ID_SERVICE],
               genStatus.tableMax[DSMIB_TABLE_ID_SERVICE]);


  DIFFSERV_PRT(msgLvlReqd, "\nAdditional Info:\n\n");

  DIFFSERV_PRT(msgLvlReqd, 
               "Rule per class limit..................  %u\n",
               (L7_uint32)L7_DIFFSERV_RULE_PER_CLASS_LIM);

  DIFFSERV_PRT(msgLvlReqd, 
               "Inst per policy limit.................  %u\n",
               (L7_uint32)L7_DIFFSERV_INST_PER_POLICY_LIM);

  DIFFSERV_PRT(msgLvlReqd, 
               "Attr per instance limit...............  %u\n",
               (L7_uint32)L7_DIFFSERV_ATTR_PER_INST_LIM);


  DIFFSERV_PRT(msgLvlReqd, "\n\n");
}

