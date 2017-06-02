
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename  sysapi_cfg_migrate_stub.c
*
* @purpose
*
* @component sysapi
*
* @create       8/23/2004
*
* @author       Rama Sasthri, Kristipati
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "nimapi.h"

L7_RC_t sysapiCfgFileUncombinedToCombinedCfgFileConvert (void)
{
  return L7_SUCCESS;
}

void sysapiUncombinedCfgRemove (void)
{
}

void sysapiCfgMigrateDataDump (void)
{
}

L7_RC_t sysapiCfgFileRel4_0Separate (L7_char8 * bigCfn)
{
  return L7_SUCCESS;
}

L7_RC_t sysapiCfgFileRel4_0SlotPortToIntfInfoGet (L7_uint32 slot,
                                                  L7_uint32 port,
                                                  nimConfigID_t * configId,
                                                  L7_uint32 * configIdOffset,
                                                  L7_INTF_TYPES_t * intfType)
{
  return L7_ERROR;
}

L7_RC_t sysapiCfgFileRel4_0IndexToIntfInfoGet (L7_uint32 index,
                                               nimConfigID_t * configId,
                                               L7_uint32 * configIdOffset,
                                               L7_INTF_TYPES_t * intfType)
{
  return L7_ERROR;
}

L7_RC_t sysapiCfgFileRel4_0VlanIntfGet (L7_uint32 mappingIndex, L7_uint32 * vlanId)
{
  return L7_ERROR;
}

void sysapiCfgFileMigrateExtIfNumToUnitSlotPortRel4_0Get (L7_uint32 extIfNum,
                                                          L7_uint32 *unit,
                                                          L7_uint32 *slot,
                                                          L7_uint32 *port)
{
  *unit = *slot = *port = 0;
}
