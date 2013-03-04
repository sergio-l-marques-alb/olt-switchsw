/*  This file contains temporary APIs that I will just use until the 
    true functionality is available from other components.  */
   
   
#include "l7_common.h"
#include "cardmgr.h"
#include "cardmgr_api.h"
#include "sysapi_hpc.h"

/* QSCAN -AEV  --- OBSOLETE */
#if 0
L7_uint32 sysapiHpcMaxUnitsGet ()
{
    return 8;
}


L7_uint32 sysapiHpcMaxSlotsGet ()
{
    return 2;
}
#endif

/* QSCAN -AEV  --- OBSOLETE */
#if 0

void cnfgrApiCallback( CNFGR_IN L7_CNFGR_CB_DATA_t *pCbData )
{
    return;
}

L7_uint32 sysapi_lag_slot_get ()
{
    return 7;
}

L7_uint32 sysapi_vlan_router_slot_get()
{
    return 9;
}

L7_top_of_stack_t sysapi_top_of_stack_get()
{
    return L7_MANAGEMENT_UNIT;
}
L7_BOOL sysapi_stack_ready_is ()
{
    return L7_TRUE;
}

L7_uint32 sysapi_unit_number_get ()
{
    return 1;
}

L7_uint32 sysapi_max_slots_per_unit(L7_uint32 unitType)
{
    return 6;
}

sysapiCardDb_t *sysapiHpclCardTypeDbGet(L7_uint32 cardType)
{
    return 0;
}
#endif


/* QSCAN -AEV  --- OBSOLETE */
#if 0
void initCardMgr(L7_uint32 phase)
{
    L7_CNFGR_CMD_DATA_t cmdData;
    L7_RC_t rc;

    cmdData.correlator = 1;
    cmdData.command = L7_CNFGR_CMD_INITIALIZE;
    cmdData.type = L7_CNFGR_RQST;
    
    if (phase == CNFGR_INIT_PHASE_1) 
        cmdData.u.rqstData.request = L7_CNFGR_RQST_I_PHASE1_START;
    else if (phase == CNFGR_INIT_PHASE_2) 
        cmdData.u.rqstData.request = L7_CNFGR_RQST_I_PHASE2_START;
    else if (phase == CNFGR_INIT_PHASE_3) 
        cmdData.u.rqstData.request = L7_CNFGR_RQST_I_PHASE3_START;

    rc = cmgrApiCnfgrCommand(&cmdData);

    /* enter steady state if phase 3 */
    if (phase == CNFGR_INIT_PHASE_3) 
    {
        cmdData.command = L7_CNFGR_CMD_EXECUTE;
        cmdData.type = L7_CNFGR_RQST;
        cmdData.u.rqstData.request = L7_CNFGR_RQST_E_START;

        rc = cmgrApiCnfgrCommand(&cmdData);
    }
    
    return;
}

#endif
