/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  sysapi_support.c
*
* @purpose   Basic support/debug infrastructure
*
* @component sysapi_support
*
* @create    08/03/2006
*
* @author    wjacobs
* @end
*
*********************************************************************/
#include <string.h>                /* for memcpy() etc... */
#include <stdarg.h>                /* for va_start, etc... */
#include "l7_common.h"
#include "l7_product.h"
#include "registry.h"
#include "osapi.h"
#include "log_api.h"
#include "log.h"
#include "log_ext.h"
#include "sysapi.h"
#include "cnfgr.h"
#include "default_cnfgr.h"
#include "nvstoreapi.h"
#include "async_event_api.h"
#include "statsapi.h"
#include "sysapi_hpc.h"
#include "l7_cnfgr_api.h"
#include "dim.h"
#include "zlib.h"
#include "sysapi_util.h"
#include "sysnet_api.h"
#include "osapi.h"
#include "compdefs.h"

L7_RC_t sysapiSupportCfgFileWrite(L7_COMPONENT_IDS_t component_id, L7_char8 *filename,
                                  L7_char8 *buffer, L7_uint32 nbytes)
{
  return L7_SUCCESS;
}

L7_RC_t sysapiSupportCfgFileGet( L7_COMPONENT_IDS_t component_id, L7_char8 *fileName,
                                L7_char8 *buffer, L7_uint32 bufferSize,
                                L7_uint32 *checkSum, L7_uint32 version,
                                void (*defaultBuild)(L7_uint32),
                                void (*migrateBuild)(L7_uint32, L7_uint32, L7_char8 *))
{
  defaultBuild(version);
  return L7_SUCCESS;
}

L7_RC_t supportDebugSave(void)
{
  return L7_SUCCESS;
}

L7_RC_t supportDebugCategoryDump(SUPPORT_CATEGORIES_t category)
{
  return L7_SUCCESS;
}

void sysapiDebugRegisteredSysInfoDump(void)
{
}

L7_BOOL supportDebugCategoryIsPresent(SUPPORT_CATEGORIES_t category,
                                      L7_COMPONENT_IDS_t componentId)
{
  return L7_FALSE;
}

L7_uint32 sysapiDebugSupportDisplayModeGet(void)
{
  return L7_DISABLE;
}

void supportDebugClear(void)
{
}

L7_BOOL supportDebugCategoryRegistrationExists(void)
{
return L7_FALSE;
}

L7_RC_t sysapiSupportCfgFileSysInit(void)
{
  return L7_SUCCESS;
}

L7_RC_t sysapiDebugSupportInit(void)
{
  return L7_SUCCESS;
}

