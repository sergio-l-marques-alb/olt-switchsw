#include "commdefs.h"
#include <string.h>
#include "l7_common.h"
#include "boxs_debug.h"
#include "sysapi.h"


L7_uint32 boxsDebugTraceCfg[BOXS_DBG_FLAG_LAST_ENTRY];


void boxsDebugTraceDump(void)
{
    sysapiPrintf(" boxsDebugTraceCfg[BOXS_DBG_FLAG_cpConfigGroup] = %d\n", boxsDebugTraceCfg[BOXS_DBG_FLAG_cpConfigGroup]);
    sysapiPrintf(" boxsDebugTraceCfg[BOXS_DBG_FLAG_cpGeneralGroup] = %d\n", boxsDebugTraceCfg[BOXS_DBG_FLAG_cpGeneralGroup]);
}


void boxsDebugTraceSet(L7_uint32 flag)
{
    switch (flag) 
    {
    case BOXS_DBG_FLAG_cpConfigGroup:
        boxsDebugTraceCfg[BOXS_DBG_FLAG_cpConfigGroup] = L7_TRUE;
        break;
    case BOXS_DBG_FLAG_cpGeneralGroup:
        boxsDebugTraceCfg[BOXS_DBG_FLAG_cpGeneralGroup] = L7_TRUE;
        break;
    default:
        sysapiPrintf(" Invalid Value %d\n", flag);
        break;
    }
    return;
}

void boxsDebugTraceClear(L7_uint32 flag)
{

    if (flag == 0xFF)
    {
        memset((L7_uchar8*)&boxsDebugTraceCfg[0], 0, sizeof(L7_uint32) *BOXS_DBG_FLAG_LAST_ENTRY  );
        return;
    }
    switch (flag) 
    {
    case BOXS_DBG_FLAG_cpConfigGroup:
        boxsDebugTraceCfg[BOXS_DBG_FLAG_cpConfigGroup] = L7_FALSE;
        break;

	case BOXS_DBG_FLAG_cpGeneralGroup:
		boxsDebugTraceCfg[BOXS_DBG_FLAG_cpGeneralGroup] = L7_FALSE;
		break;
    default:
        sysapiPrintf(" Invalid Value %d\n", flag);
        break;
    }
    return;
}
void boxsDebugTraceLinkerAssist(void)
{
    boxsDebugTraceDump();
    boxsDebugTraceSet(0);
    return;
}

