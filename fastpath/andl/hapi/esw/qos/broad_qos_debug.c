/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_qos_debug.c
*
* Purpose: This file contains debug routines for the QOS package
*
* Component: hapi
*
* Comments:
*
* Created by: grantc 7/24/02
*
*********************************************************************/
#include <string.h>

#include "broad_qos.h"
/* PTin removed: SDK 6.3.0 */
#include "ptin_globaldefs.h"
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* No include */
#else
#include "bcm/diffserv.h"
#endif
#include "soc/debug.h"
#include "soc/cm.h"
#include "soc/mcm/allenum.h"
#include "bcmx/cosq.h"
#include "bcmx/port.h"
#ifdef L7_STACKING_PACKAGE
#include "../../../../../src/l7public/api/unitmgr_api.h"
#endif

extern DAPI_t *dapi_g;
extern DAPI_QOS_COS_MAP_TABLE_t  mapTable;

extern int gTrustedMode;
int verifyUnitNum(int unit);

/*********************************************************************
*
* @purpose Initialize the QOS debug routines
*
* @param   void
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadQosDebugInit(void)
{
    /* nothing to do here, this function is just called to link in this
       file */
    return;
}

/*********************************************************************
*
*********************************************************************/

L7_RC_t debugDsStats(L7_uint32 dpid, L7_uint32 cfid)
{
  /* PTin removed: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  /* Nothing done! */
  #else
    L7_uint32                 unit;
    bcm_ds_counters_t         ds_counter;
    L7_uint32                 rv;
    L7_ulong64                stats_in_profile;
    L7_ulong64                stats_out_profile;

    unit = 0;

    rv = bcm_ds_counter_get(unit, dpid, cfid, &ds_counter);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
        soc_cm_debug(DK_ERR, "Error: counter get failed: %s\n", bcm_errmsg(rv));
        return L7_FAILURE;
    }
    hapiBroadStatsConvert(&stats_in_profile, ds_counter.inp_pkt_cntr);
    hapiBroadStatsConvert(&stats_out_profile, ds_counter.outp_pkt_cntr);
    printf("\nIn-profile packets = %u %u\n", stats_in_profile.high, stats_in_profile.low);
    printf("Out-profile packets = %u %u\n", stats_out_profile.high, stats_out_profile.low);
  #endif
    return L7_SUCCESS;
}

/*********************************************************************
*
*********************************************************************/
L7_RC_t hapiBroadDebugDumpCosShaping(L7_uint32 unit,L7_uint32 port)
{
    DAPI_USP_t                usp;
    BROAD_PORT_t               *hapiPortPtr;
    L7_uint32                   kbits_sec;
    L7_uint32                   kbits_burst;

    usp.unit = verifyUnitNum(unit);
    usp.slot = 0;
    usp.port = port; 


    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

    bcmx_port_rate_egress_get( hapiPortPtr->bcmx_lport, &kbits_sec, &kbits_burst);

    printf("COS Egress Rate   Port = %d   Rate = %d Kbps  \n",port,kbits_sec);

    return L7_SUCCESS;

}
/*********************************************************************
*
*********************************************************************/

int hapiBroadDebugDumpCosQueues(int unit, int port)
{
    bcmx_lplist_t               cosq_port_list;
    BROAD_PORT_t               *hapiPortPtr;
    int mode = 0;
    L7_uint32                   weights[8] = {0}; 
    DAPI_USP_t                usp;



    usp.unit = verifyUnitNum(unit);
    usp.slot = 0;
    usp.port = port; 


    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);


    bcmx_lplist_init(&cosq_port_list, L7_MAX_INTERFACE_COUNT, 0);
    bcmx_lplist_last_insert(&cosq_port_list, hapiPortPtr->bcmx_lport);
    bcmx_cosq_port_sched_get(cosq_port_list,&mode, (int *)&weights, 0);
    bcmx_lplist_free(&cosq_port_list);
    printf("COS Queue type = %x\n",mode);

    printf("COS Queue Weights = (0)%d (1)%d (2)%d (3)%d (4)%d (5)%d (6)%d (7)%d\n",
           weights[0],
           weights[1],
           weights[2],
           weights[3],
           weights[4],
           weights[5],
           weights[6],
           weights[7]);


    return L7_SUCCESS;

}

void hapiBroadDebugDumpDscpMap(int unit,int port)
{
    int index;
    DAPI_USP_t                usp;
    BROAD_PORT_t             *hapiPortPtr;
    HAPI_BROAD_QOS_PORT_t    *qosPortPtr;
    int                      mapcp;
    int                      prio;

    usp.unit = verifyUnitNum(unit);
    usp.slot = 0;
    usp.port = port; 

    hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
    qosPortPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    if (qosPortPtr->cos.trustMode != DAPI_QOS_COS_INTF_MODE_TRUST_IPDSCP)
    {
        printf("Port is not in IP DSCP Trust mode \n");
        return;
    }

    printf("DSCP Map from Broadcom Chip- ");
    for (index=0; index<L7_QOS_COS_MAP_NUM_IPDSCP; index++)
    {
        /* result = bcmx_port_dscp_map_get(bcmx_lport_t port, int srccp, int mapcp, int prio);*/
        bcmx_port_dscp_map_get(BCMX_LPORT_ETHER_ALL, index, &mapcp, &prio);

        if ((index%8) == 0)
            printf("\n[%2.2d-%2.2d] ", index, index+7);

        printf("%d ", prio);
    }
    printf("\n");
}

int verifyUnitNum(int unit)
{

#ifdef L7_STACKING_PACKAGE
    int rc;
    L7_uint32 unit_status;


    rc = unitMgrUnitStatusGet (unit, &unit_status);
    if ((rc != L7_SUCCESS) || (unit_status != L7_UNITMGR_UNIT_OK))
    {
        printf("%d is an invalid unit number, using unit 1\n",unit);
        return 1;
    }
    else
        return(unit);;
#else
    return(1);
#endif

}

