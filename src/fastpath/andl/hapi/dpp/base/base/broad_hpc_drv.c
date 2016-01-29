/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  broad_hpc_drv.c
*
* @purpose   Technology specific functions and data for the hardware
*            platform control component
*
* @component 
*
* @create    04/14/2003
*
* @author    jeffr 
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "buff_api.h"
#include "sysapi.h"
#include "sysapi_hpc.h"
#include "osapi.h"
#include "osapi_support.h"
#include "simapi.h"
#include "bspapi.h"
#include "registry.h"
#include "log.h"
#include "unitmgr_api.h"
#include "l7_usl_api.h"
#include "l7_usl_bcmx_port.h"
#include "l7_usl_bcm.h"
#include "l7_usl_sm.h"
#include "l7_usl_l2_db.h"
#ifdef L7_ROUTING_PACKAGE
#include "l7_usl_l3_db.h"
#endif
#include "l7_usl_policy_db.h"
#include "cnfgr.h"
#include "hpc_hw_api.h"
#include "dtl_exports.h"

#include "broad_policy.h"
#include "broad_common.h"
#include "broad_l3.h"
#include "broad_l2_std.h"
#include "broad_mmu.h"
#ifdef L7_QOS_PACKAGE
#include "broad_cos.h"
#endif
#include "dapi_db.h"
#include "dapi_debug.h"
#include "dapi_trace.h"
#include "sysbrds.h"

#include "ibde.h"
#include "bcm/vlan.h"
#include "bcm/cosq.h"
#include "soc/cmext.h"
#include "soc/mem.h"
#include "soc/arl.h"
#include "soc/l2x.h"

#include "bcmx/bcmx.h"
#include "bcmx/bcmx_int.h"
#include "bcm_int/rpc/rlink.h"
/* PTin modified: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* No included */
#else
#include "bcmx/filter.h"
#endif
#include "bcmx/l3.h"
#include "bcm/vlan.h"

//#include "soc/debug.h"
#include "soc/drv.h"

#include "appl/cputrans/next_hop.h"
#include "appl/cputrans/atp.h"
#include "appl/discover/disc.h"
#include "appl/diag/sysconf.h"
#include "commdefs.h"

#if defined(INCLUDE_PHY_8706)
#include "soc/phy.h"
#include "soc/phy/xgxs5.h"
#include "soc/phy/phyident.h"
#include "soc/phy/phynull.h"
#include "soc/phy/phyreg.h"
#include "soc/phyreg.h"
#endif

/* PTin added: includes */
#if 1
#include "logger.h" /* PTin added */
#include "ptin_hapi.h"
#define PTIN_TRAP_TO_CPU  0
#endif

#include "shared/shr_bprof.h"
#include "appl/diag/shell.h"


extern void hapiBroadSocFileLoad(char *file_name, L7_BOOL suppressFileNotAvail);

/* default weights for WRR queues 
*/
L7_uint32 wrr_default_weights[] = 
{
  COSQ0_WEIGHT, COSQ1_WEIGHT, COSQ2_WEIGHT, COSQ3_WEIGHT,
  COSQ4_WEIGHT, COSQ5_WEIGHT, COSQ6_WEIGHT, COSQ7_WEIGHT
};

/* Values used to limit packets to the CPU per COS
 *
 * For hardware rate limiting, the values are in Kbps
 * with a granularity of 64 Kbps. There must be a
 * value (0,64,128,...) for each cos.
 *
 * For software rate limiting, the values denote the 
 * maximum number of packets per second (pps) allowed.
 *
 * A value of zero means that the COS is not rate limited
 */

#if  (L7_MAX_CFG_QUEUES_PER_PORT  == 4)
static L7_int32  cpu_pps_rate_limit_per_cos[] = { 100, 100, 0, 0 };
static L7_int32  cpu_warm_start_pps_rate_limit_per_cos[] = { 25, 25, 0, 0 };
static L7_int32  cpu_kbps_rate_limit_per_cos[] = { 128, 128, 0, 0 };
#else
#ifdef L7_STACKING_PACKAGE
static L7_int32  cpu_kbps_rate_limit_per_cos[] = { 512, 512, 512, 512, 512, 0, 0, 0 };
static L7_int32  cpu_warm_start_kbps_rate_limit_per_cos[] = { 64, 64, 512, 512, 512, 0, 0, 0};
static L7_int32  cpu_pps_rate_limit_per_cos[] = { 1024, 1024, 1024, 1024, 1024, 0, 0, 0 };
static L7_int32  cpu_warm_start_pps_rate_limit_per_cos[] = { 128, 128, 1024, 1024, 1024, 0, 0, 0 };
#else
static L7_int32  cpu_kbps_rate_limit_per_cos[] = { 512, 512, 512, 512, 512, 512, 0, 0 };
static L7_int32  cpu_warm_start_kbps_rate_limit_per_cos[] = { 64, 64, 512, 512, 512, 512, 0, 0};
static L7_int32  cpu_pps_rate_limit_per_cos[] = { 1024, 1024, 1024, 1024, 1024, 1024, 0, 0 };
static L7_int32  cpu_warm_start_pps_rate_limit_per_cos[] = { 128, 128, 1024, 1024, 1024, 1024, 0, 0 };
#endif
#endif

static L7_int32 cpu_xgs2_sw_rate_limit_per_unit = 1500, cpu_raptor_sw_rate_limit_per_unit = 300;
static L7_int32 cpu_default_sw_rate_limit_per_unit = 0;

typedef enum {
  BROAD_CPU_KBPS_RATE_LIMIT = 0,
  BROAD_CPU_PPS_RATE_LIMIT
} BROAD_CPU_RATE_LIMIT_TYPE_t;

int hapiBroadCpuCosqRateSet(int unit, int cosq, int rate, BROAD_CPU_RATE_LIMIT_TYPE_t type);
#ifdef L7_ROBO_SUPPORT
static L7_int32 hapiBroadSetRoboDefaultParameters( L7_int32 unit );
#endif

typedef struct
{
  L7_BOOL              inUse;
  L7_int32             bcmUnit;
  L7_int32             bcmCpuUnit;
  L7_int32             numOfBcmPorts;
  bcmx_lport_t        *bcmxLport;

} HAPI_BROAD_MAP_BCM_CPU_UNIT_t;


typedef struct
{
  L7_BOOL                        inUse;
  cpudb_key_t                    cpuKey;
  L7_int32                       numOfBcmCpuUnits;
  HAPI_BROAD_MAP_BCM_CPU_UNIT_t *bcmCpuUnit;

} HAPI_BROAD_MAP_BCM_CPU_t;


static HAPI_BROAD_MAP_BCM_CPU_t *hapiBroadMapDb     = L7_NULLPTR;
static void                     *hapiBroadMapDbSema = L7_NULLPTR;


L7_RC_t hapiBroadMapDbCreate(void)
{
  L7_uint32 l7UnitIndex, bcmCpuUnitIndex;

  hapiBroadMapDbSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);

  /* Malloc the top of the tree one entry per L7 unit */
  hapiBroadMapDb = (HAPI_BROAD_MAP_BCM_CPU_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID, L7_MAX_UNITS_PER_STACK*sizeof(HAPI_BROAD_MAP_BCM_CPU_t));

  if (hapiBroadMapDb == L7_NULLPTR)
    return L7_FAILURE;

  for (l7UnitIndex = 0; l7UnitIndex < L7_MAX_UNITS_PER_STACK; l7UnitIndex++)
  {
    hapiBroadMapDb[l7UnitIndex].inUse = L7_FALSE;
    memset(&hapiBroadMapDb[l7UnitIndex].cpuKey.key,0,sizeof(cpudb_key_t));
    hapiBroadMapDb[l7UnitIndex].numOfBcmCpuUnits = 0;

    /* For each L7 unit malloc max bcm cpu units */
    hapiBroadMapDb[l7UnitIndex].bcmCpuUnit = (HAPI_BROAD_MAP_BCM_CPU_UNIT_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID, HAPI_BROAD_MAX_CPU_UNITS_PER_BOX *
                                                                                         sizeof(HAPI_BROAD_MAP_BCM_CPU_UNIT_t));
    if (hapiBroadMapDb[l7UnitIndex].bcmCpuUnit == L7_NULLPTR)
      return L7_FAILURE;

    for (bcmCpuUnitIndex = 0; bcmCpuUnitIndex < HAPI_BROAD_MAX_CPU_UNITS_PER_BOX; bcmCpuUnitIndex++)
    {

      /* For each bcm cpu unit malloc ports */
      hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse = L7_FALSE;
      hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts = 0;
      hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit = -1;

      hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmxLport = (bcmx_lport_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID, HAPI_BROAD_MAX_PORTS_PER_CPU_UNIT *
                                                                                                     sizeof(bcmx_lport_t));
      if (hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmxLport == L7_NULLPTR)
        return L7_FAILURE;
    }
  }
  osapiSemaGive(hapiBroadMapDbSema);

  return L7_SUCCESS;
}



L7_RC_t hapiBroadMapDbCpuUnitEntryAdd(int unit, cpudb_key_t *cpuKey, int cpuunit)
{
  L7_uint32 l7UnitIndex, bcmCpuUnitIndex;

  osapiSemaTake(hapiBroadMapDbSema,L7_WAIT_FOREVER);

  /* First see if the cpu_entry already exists */
  for (l7UnitIndex=0; l7UnitIndex < L7_MAX_UNITS_PER_STACK; l7UnitIndex++)
  {
    if ((hapiBroadMapDb[l7UnitIndex].inUse  == L7_TRUE) &&
        (memcmp(&hapiBroadMapDb[l7UnitIndex].cpuKey.key,&cpuKey->key,sizeof(cpudb_key_t)) == 0))
    {
      for (bcmCpuUnitIndex = 0; bcmCpuUnitIndex < HAPI_BROAD_MAX_CPU_UNITS_PER_BOX; bcmCpuUnitIndex++)
      {
        if (hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse == L7_FALSE)
        {
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse         = L7_TRUE;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit       = unit;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmCpuUnit    = cpuunit;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts = 0;
          hapiBroadMapDb[l7UnitIndex].numOfBcmCpuUnits++;
          osapiSemaGive(hapiBroadMapDbSema);
          return L7_SUCCESS;
        }
      }
      if (bcmCpuUnitIndex >= HAPI_BROAD_MAX_CPU_UNITS_PER_BOX)
      {
        /* No more entries available */
        osapiSemaGive(hapiBroadMapDbSema);
        return L7_FAILURE;
      }
    }
  }

  /* Does not already exist, search for an available entry */
  for (l7UnitIndex=0; l7UnitIndex < L7_MAX_UNITS_PER_STACK; l7UnitIndex++)
  {
    if (hapiBroadMapDb[l7UnitIndex].inUse == L7_FALSE)
    {
      hapiBroadMapDb[l7UnitIndex].inUse            = L7_TRUE;
      memcpy(&hapiBroadMapDb[l7UnitIndex].cpuKey.key,&cpuKey->key,sizeof(cpudb_key_t));
      hapiBroadMapDb[l7UnitIndex].numOfBcmCpuUnits = 0;

      for (bcmCpuUnitIndex = 0; bcmCpuUnitIndex < HAPI_BROAD_MAX_CPU_UNITS_PER_BOX; bcmCpuUnitIndex++)
      {
        if (hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse == L7_FALSE)
        {
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse         = L7_TRUE;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit       = unit;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmCpuUnit    = cpuunit;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts = 0;
          hapiBroadMapDb[l7UnitIndex].numOfBcmCpuUnits++;
          osapiSemaGive(hapiBroadMapDbSema);
          return L7_SUCCESS;
        }
      }
      if (bcmCpuUnitIndex >= HAPI_BROAD_MAX_CPU_UNITS_PER_BOX)
      {
        /* No more entries available */
        osapiSemaGive(hapiBroadMapDbSema);
        return L7_FAILURE;
      }
    }
  }
  /* No more entries available */
  osapiSemaGive(hapiBroadMapDbSema);
  return L7_FAILURE;
}


L7_RC_t hapiBroadMapDbCpuUnitEntryDel(int unit)
{
  L7_uint32     l7UnitIndex, bcmCpuUnitIndex, localUnitNum;
  L7_BOOL       resetEntry;
  cpudb_entry_t localKey;

  osapiSemaTake(hapiBroadMapDbSema,L7_WAIT_FOREVER);

  for (l7UnitIndex=0; l7UnitIndex < L7_MAX_UNITS_PER_STACK; l7UnitIndex++)
  {
    if (hapiBroadMapDb[l7UnitIndex].inUse == L7_TRUE)
    {
      for (bcmCpuUnitIndex = 0; bcmCpuUnitIndex < HAPI_BROAD_MAX_CPU_UNITS_PER_BOX; bcmCpuUnitIndex++)
      {
        if ((hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse   == L7_TRUE) &&
            (hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit == unit))
        {
          /* Reset all the port info */
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts = 0;

          memset(hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmxLport,
                 -1,
                 HAPI_BROAD_MAX_PORTS_PER_CPU_UNIT * sizeof(bcmx_lport_t));

          resetEntry = L7_TRUE;

          /* Do not remove the local unit entry from Db as it is inserted only once */
          if ((unitMgrNumberGet(&localUnitNum) == L7_SUCCESS) && 
              (unitMgrUnitIdKeyGet(localUnitNum, (L7_enetMacAddr_t *) &localKey) == L7_SUCCESS))
          {
            if (memcmp(&hapiBroadMapDb[l7UnitIndex].cpuKey.key,
                       &(localKey),sizeof(cpudb_key_t)) == 0)
            {
              resetEntry = L7_FALSE;    
            }
          }

          if (resetEntry == L7_TRUE)
          {
            hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse         = L7_FALSE;
            hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit       = -1;
            hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmCpuUnit    = -1;

            hapiBroadMapDb[l7UnitIndex].numOfBcmCpuUnits--;
          }

          if ((hapiBroadMapDb[l7UnitIndex].numOfBcmCpuUnits == 0)  &&
              (resetEntry == L7_TRUE))
              
          {
            memset(&hapiBroadMapDb[l7UnitIndex].cpuKey.key,0,sizeof(cpudb_key_t));
            hapiBroadMapDb[l7UnitIndex].inUse = L7_FALSE;
          }
          osapiSemaGive(hapiBroadMapDbSema);
          return L7_SUCCESS;
        }
      }
    }
  }
  /* Entry Not Found */
  osapiSemaGive(hapiBroadMapDbSema);
  return L7_FAILURE;
}


L7_RC_t hapiBroadMapDbPortEntryAdd(int unit, bcm_port_t port, bcmx_lport_t lport)
{
  L7_uint32 l7UnitIndex, bcmCpuUnitIndex;

  osapiSemaTake(hapiBroadMapDbSema,L7_WAIT_FOREVER);

  for (l7UnitIndex=0; l7UnitIndex < L7_MAX_UNITS_PER_STACK; l7UnitIndex++)
  {
    if (hapiBroadMapDb[l7UnitIndex].inUse == L7_TRUE)
    {
      for (bcmCpuUnitIndex = 0; bcmCpuUnitIndex < HAPI_BROAD_MAX_CPU_UNITS_PER_BOX; bcmCpuUnitIndex++)
      {
        if ((hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse   == L7_TRUE) &&
            (hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit == unit))
        {
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmxLport[port] = lport;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts++;
          osapiSemaGive(hapiBroadMapDbSema);
          return L7_SUCCESS;
        }
      }
    }
  }
  /* Entry Not Found */
  osapiSemaGive(hapiBroadMapDbSema);
  return L7_FAILURE;
}



L7_RC_t hapiBroadMapDbEntryGet(cpudb_key_t *cpuKey, int cpuunit, bcm_port_t port, int *unit, bcmx_lport_t *lport)
{
  L7_uint32 l7UnitIndex, bcmCpuUnitIndex;

  osapiSemaTake(hapiBroadMapDbSema,L7_WAIT_FOREVER);

  /* First see if the cpu_entry already exists */
  for (l7UnitIndex=0; l7UnitIndex < L7_MAX_UNITS_PER_STACK; l7UnitIndex++)
  {
    if ((hapiBroadMapDb[l7UnitIndex].inUse == L7_TRUE) &&
        (memcmp(&hapiBroadMapDb[l7UnitIndex].cpuKey.key,&cpuKey->key,sizeof(cpudb_key_t)) == 0))
    {
      for (bcmCpuUnitIndex = 0; bcmCpuUnitIndex < HAPI_BROAD_MAX_CPU_UNITS_PER_BOX; bcmCpuUnitIndex++)
      {
#ifdef L7_STACKING_PACKAGE
        {
          /* Attach all units for this CPU key.
          */
          if ((hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts == 0) &&
              (hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse == L7_TRUE))
          {
            int rv = BCM_E_NONE;
            int bcm_unit;

              bcm_unit  = hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit;
              osapiSemaGive(hapiBroadMapDbSema);
              rv = bcmx_device_attach(bcm_unit);

              /* If attach failed then detach any ports that may have been attached and return 
              ** an error code.
              */
              if (rv != BCM_E_NONE)
              {
                dapiTraceStackEvent("Bcmx_device_attach failed, rv %d\n", rv);
                hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts = 0;

                memset(hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmxLport,
                       -1,
                       HAPI_BROAD_MAX_PORTS_PER_CPU_UNIT * sizeof(bcmx_lport_t));

                (void)bcmx_device_detach(bcm_unit);
                return L7_FAILURE;
              }

              osapiSemaTake(hapiBroadMapDbSema,L7_WAIT_FOREVER);
          }
        }
#endif /* L7_STACKING_PACKAGE */

        if ((hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse      == L7_TRUE) &&
            (hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmCpuUnit == cpuunit))
        {
          *unit  = hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit;


          *lport = hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmxLport[port];
          osapiSemaGive(hapiBroadMapDbSema);
          return L7_SUCCESS;
        }
      }
    }
  }
  /* Entry Not Found */
  osapiSemaGive(hapiBroadMapDbSema);
  return L7_FAILURE;
}



extern DAPI_t *dapi_g;

bcmx_uport_t lvl7_uport_create_callback(bcmx_lport_t lport, int unit, bcm_port_t port, uint32 flags)
{
  DAPI_USP_t    searchUsp;
  bcmx_uport_t  uport;
  BROAD_PORT_t *hapiPortPtr;

  PT_LOG_TRACE(LOG_CTX_STARTUP,"New port: lport=0x%x unit=%d port=%d flags=0x%x", lport, unit, port, flags);

  if (~flags & BCMX_PORT_F_VALID)
  {
    return(BCMX_UPORT_INVALID_DEFAULT);
  }
  
  /* PTin modified: 40G interfaces have flags=0x01 */
  #if 1
  hapiBroadMapDbPortEntryAdd(unit, port, lport);
  #else
  else if (flags & (BCMX_PORT_F_FE | BCMX_PORT_F_GE | BCMX_PORT_F_XE | BCMX_PORT_F_HG ))
  {
    /* Regular Physical Ports */
    /* HG ports */
    hapiBroadMapDbPortEntryAdd(unit, port, lport);
  }
  else if (flags & BCMX_PORT_F_CPU)
  {
    /* CPU Port */
    hapiBroadMapDbPortEntryAdd(unit, port, lport);
  }
  else
  {
    return(BCMX_UPORT_INVALID_DEFAULT);
  }
  #endif

  /* Check to see if the lport already exists */
  if ((dapi_g != L7_NULLPTR) &&
      (dapi_g->system != L7_NULLPTR))
  {
    for (searchUsp.unit = 0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
    {
      if (dapi_g->unit[searchUsp.unit] != L7_NULLPTR)
      {
        for (searchUsp.slot = 0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
        {
          if ((dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot] != L7_NULLPTR) &&
              (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE) &&
              (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardType == SYSAPI_CARD_TYPE_LINE))
          {
            for (searchUsp.port=0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot;searchUsp.port++)
            {
              hapiPortPtr = HAPI_PORT_GET(&searchUsp,dapi_g);

              if (hapiPortPtr->bcmx_lport == lport)
              {
                /* 
                ** Broadcom stack status bounce without unitmanager noticing
                ** restore the uport mapping 
                */
                HAPI_BROAD_USP_TO_UPORT(&searchUsp,uport);
                return uport;
              }
            }
          }
        }
      }
    }
  }

  /* 
  ** Need to return an invalid lport so that lvl7 can
  ** call bcmx_uport_set later on
  */
  return(BCMX_UPORT_INVALID_DEFAULT);
}

 void hapiBroadDebugBcmxMapDump(void)
{
  L7_uint32  l7UnitIndex, bcmCpuUnitIndex, portIndex, i;
  L7_uchar8 *bytePtr;

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"BCMX Port Map Show");

  osapiSemaTake(hapiBroadMapDbSema,L7_WAIT_FOREVER);

  /* First see if the cpu_entry already exists */
  for (l7UnitIndex=0; l7UnitIndex < L7_MAX_UNITS_PER_STACK; l7UnitIndex++)
  {
    if (hapiBroadMapDb[l7UnitIndex].inUse == L7_TRUE)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n");
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"L7 unit index    %d\n", l7UnitIndex);
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"cpukey           ");

      bytePtr = (L7_uchar8*)&hapiBroadMapDb[l7UnitIndex].cpuKey;

      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%2.2x", bytePtr[0])

      for (i=1;i<sizeof(hapiBroadMapDb[l7UnitIndex].cpuKey);i++)
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,":%2.2x", bytePtr[i]);

      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"num of bcm units %d\n", hapiBroadMapDb[l7UnitIndex].numOfBcmCpuUnits);

      for (bcmCpuUnitIndex = 0; bcmCpuUnitIndex < HAPI_BROAD_MAX_CPU_UNITS_PER_BOX; bcmCpuUnitIndex++)
      {
        if (hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse == L7_TRUE)
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"  bcm cpuunit       %d\n", hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmCpuUnit);
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"  bcm mapped unit   %d\n", hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit);
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"  num of bcm ports  %d\n", hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts);
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"    (bcm port index, bcmx mapped lport)\n");
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"    ");
          for (portIndex=0; portIndex < hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts; portIndex++)
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"(%d,%d)", portIndex, hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmxLport[portIndex]);
          }
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"\n");
        }
      }
    }
  }
  /* Entry Not Found */
  osapiSemaGive(hapiBroadMapDbSema);
}

/*********************************************************************
* @purpose Rate limit the cpu port cos-queues (0-3) in hardware.
*
* @param    int                           unit  - unit number
*           int                           cosq  - internal cos queue id
*           int                           rate  - rate limit in Kbps
*           BROAD_CPU_RATE_LIMIT_TYPE_t   type  - hardware or software
*                                                 rate limiting                          
*
* @returns  none 
*
* @comments We are rate-limiting data-queues (0-3) only.
*       
* @end
*********************************************************************/
int hapiBroadCpuCosqRateSet(int unit, int cosq, int rate, BROAD_CPU_RATE_LIMIT_TYPE_t type)
{
  int  rv = BCM_E_NONE;

  switch (type)
  {
    case BROAD_CPU_KBPS_RATE_LIMIT:
      rv = bcm_cosq_port_bandwidth_set(unit, CMIC_PORT(unit), cosq, 0, rate, 0);
      break;
    case BROAD_CPU_PPS_RATE_LIMIT:

      /* For devices that support packet rate limit feature, like Triumph, set the 
       * packet rate limits for lower cos queues.
       */
      if (soc_feature(unit, soc_feature_packet_rate_limit)) {
#ifdef BCM_TRX_SUPPORT
        extern int _bcm_tr_cosq_port_packet_bandwidth_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq, int pps, int burst);
#if defined (BCM_TRIUMPH2_SUPPORT) || defined (BCM_TRIUMPH3_SUPPORT)
        /* PTin added: new switch 56689 (Valkyrie2) */
        /* PTin added: new switch 5664x (Triumph3) */
        /* PTin removed: new switch 56843 (Trident) */
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit) /*|| SOC_IS_TRIDENT(unit)*/ ||
            SOC_IS_TRIUMPH3(unit))
        {
          extern int bcm_tr2_cosq_port_pps_set(int unit, bcm_port_t port,
                                               bcm_cos_queue_t cosq, int pps);   
          rv = bcm_tr2_cosq_port_pps_set(unit, CMIC_PORT(unit), cosq, rate);
        }
        else
#endif
        /* PTin added: new switch 56843 (Trident) */
        if ((SOC_IS_TR_VL(unit) || SOC_IS_SCORPION(unit)) && !SOC_IS_TRIDENT(unit)) {
          rv = _bcm_tr_cosq_port_packet_bandwidth_set(unit,CMIC_PORT(unit), cosq, rate, rate);
        }
        /* PTin added: new switch 56843 (Trident) */
        else if (SOC_IS_TRIDENT(unit)) {
          extern int bcm_td_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq, int pps);
          rv = bcm_td_cosq_port_pps_set(unit, CMIC_PORT(unit), cosq, rate);
        }
        /* PTin end */
        else {
          rv = BCM_E_FAIL;
        }
#endif
      }
      else
      {
        rv = bcm_rx_cos_rate_set(unit, cosq, rate);
      }
      break;
    default:
      rv = BCM_E_FAIL;
  }

  return rv;
}

/*********************************************************************
*
* @purpose Configure the defaults for RTAG7 hash controls. 
*
* @param   unit - bcm unit number
*
* @returns L7_RC_t result
*
* @notes   RTAG7 is an enhanced hashing mode for Trunks/ECMP. The
*          RTAG7 settings below are used when Trunk/ECMP hashing mode
*          is set to use RTAG=7. The enhanced hashing mode has many
*          configurable options, which makes it cumbersome to expose 
*          them to end-user via UI. The goal here is to come up with
*          default RTAG7 settings.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadRtag7SwitchControlSet(int unit)
{
  int rv;
  int arg;
  bcm_pbmp_t pbmp;
  bcm_port_t bcmPort;
#ifndef L7_STACKING_PACKAGE 
  L7_uchar8  mac[6];
#endif

  /* NOTE: RTAG7 port flow hashing is available on select platforms */
  if (!soc_feature(unit, soc_feature_port_flow_hash))
  {
    return L7_SUCCESS; /* Nothing to be configured */
  }

  /* Set the RTAG7 packet field selection. Only HASH_A mode is used. */

  /* For L2 packets (non-ip), choose the SRC/DST mac, ethtype, VLAN along with
   * source and destination ports
   */
  arg = (BCM_HASH_FIELD_MACSA_HI | BCM_HASH_FIELD_MACSA_MI |
         BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACDA_HI |
         BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_LO |
         BCM_HASH_FIELD_ETHER_TYPE | BCM_HASH_FIELD_VLAN |
         BCM_HASH_FIELD_SRCPORT | BCM_HASH_FIELD_SRCMOD); 

  rv = bcm_switch_control_set(unit, bcmSwitchHashL2Field0, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* For IPv4 packets, choose the five-tuples of IP packet along with
   * source and destination ports
   */
  arg = (BCM_HASH_FIELD_IP4SRC_HI | BCM_HASH_FIELD_IP4SRC_LO |
         BCM_HASH_FIELD_IP4DST_HI | BCM_HASH_FIELD_IP4DST_LO |
         BCM_HASH_FIELD_VLAN | BCM_HASH_FIELD_SRCL4 |
         BCM_HASH_FIELD_DSTL4 | BCM_HASH_FIELD_PROTOCOL |
         BCM_HASH_FIELD_SRCPORT | BCM_HASH_FIELD_SRCMOD); 

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* For IPv6 packets, choose the five-tuples of IP packet along with
   * source and destination port
   */
  arg = (BCM_HASH_FIELD_IP6SRC_HI | BCM_HASH_FIELD_IP6SRC_LO |
         BCM_HASH_FIELD_IP6DST_HI | BCM_HASH_FIELD_IP6DST_LO |
         BCM_HASH_FIELD_VLAN | BCM_HASH_FIELD_SRCL4 |
         BCM_HASH_FIELD_DSTL4 | BCM_HASH_FIELD_NXT_HDR |
         BCM_HASH_FIELD_SRCPORT | BCM_HASH_FIELD_SRCMOD);

  rv = bcm_switch_control_set(unit, bcmSwitchHashIP6Field0, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }
 
  /* Unknown packet types */
  arg = (BCM_HASH_FIELD_SRCMOD | BCM_HASH_FIELD_SRCPORT) ;

  rv = bcm_switch_control_set(unit, bcmSwitchHashHG2UnknownField0, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }


  /* For T2, MIM/MPLS fields can be selected too. */

  /* Now, select the hashing algorithm. 
   * RTAG7 mode can compute 2 sets of hash at the same time. 
   * Configure only HASH_A. HASH_B is not used.
   */
  arg = BCM_HASH_FIELD_CONFIG_CRC16XOR8;
  rv = bcm_switch_control_set(unit, bcmSwitchHashField0Config, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* Now, select which bits of the computed hash are used for modulo operation.
   * There are separate controls for ECMP and Trunks. The goal here is to use
   * same settings for both ECMP and Trunk hashing. There are 2 sets of offsets,
   * configure SET0.
   */
  
  arg = 0; /* Use offset=0, implies lower-8bits of HASH_A value */
  rv = bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0UnicastOffset, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }
  
  /* Use RTAG7 settings for Non-unicast traffic too (when RTAG7 is enabled) */
  arg = 0; /* Use offset=0, implies lower-8bits of HASH_A value */
  rv = bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0NonUnicastOffset, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }
  
  /* Use same hash bits for HG trunks and ECMP too */

  arg = 0; /* Use offset=0, implies lower-8bits of HASH_A value */
  rv = bcm_switch_control_set(unit, bcmSwitchFabricTrunkHashSet0UnicastOffset, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  arg = 0; /* Use offset=0, implies lower-8bits of HASH_A value */
  rv = bcm_switch_control_set(unit, bcmSwitchECMPHashSet0Offset, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* For each port, choose to select SET0 offsets for HASH bits */
  BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
  BCM_PBMP_ITER(pbmp, bcmPort)
  {
    bcm_port_control_set(unit, bcmPort, bcmPortControlTrunkHashSet, 0);
    bcm_port_control_set(unit, bcmPort, bcmPortControlECMPHashSet, 0);
  }

#ifndef L7_STACKING_PACKAGE 
  /* Configure a random seed for the HASH, so that each router will use a
   * different SEED for HASH computation. This helps to neutralize the hash
   * polarization effect to some extent. For stacking packages, each unit 
   * will have different base MAC address, and we need every unit in stack
   * to hash to an identical index (for a given flow). Programming different
   * seeds for different stack units results in traffic replication/loss.
   */ 
  if (bspapiMacAddrGet(mac) == L7_SUCCESS)
  {
    arg = ((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5]);

    sal_srand((L7_uint32)arg);

    arg = sal_rand();
    rv = bcm_switch_control_set(unit, bcmSwitchHashSeed0, arg);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      return L7_FAILURE;
    }
  }
#endif

  /* 
   * Configure ECMP to use RTAG7 by default.
   * For NON-UC traffic, we could use RTAG7 (NON_UCAST_TRUNK_BLOCK_MASK will
   * still be used for port selection, but the hash is calculated by RTAG7).
   * But, there are issues with packets hashing differently in stacking
   * environments (packets on local unit have a different hash than packets
   * arriving on HIGIG on remote units). BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE
   * is thus not used. 
   */

  arg = 0;
  rv = bcm_switch_control_get(unit, bcmSwitchHashControl, &arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  arg |= (BCM_HASH_CONTROL_ECMP_ENHANCE);

  rv = bcm_switch_control_set(unit, bcmSwitchHashControl, arg);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Enable various switch control.
*
* @param   i - bcm unit number
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSwitchControlSet(int i)
{
  int rv, hashControl;


  /* Packets copied to the CPU through the CAP are assigned a high priority */
  rv = bcm_switch_control_set(i, bcmSwitchCpuFpCopyPrio, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchArpReplyDrop, 0);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* Do not enable copyToCpu for Arp request as we get the packets 
     due to flooding of broadcast packets.*/
  rv = bcm_switch_control_set(i, bcmSwitchArpRequestDrop, 0);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* The following L3 controls are specific to XGS3. On XGS2 devices,
   * these controls return BCM_E_UNAVAIL.
   */

  /* Enable L3 DST miss to CPU. */
  rv = bcm_switch_control_set(i, bcmSwitchV4L3DstMissToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchV6L3DstMissToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* Enable L3 exception to CPU. */
  rv = bcm_switch_control_set(i, bcmSwitchV4L3ErrToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchV6L3ErrToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* Enabled IP packets with TTL=1 and TTL=0 */
  rv = bcm_switch_control_set(i, bcmSwitchL3SlowpathToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchL3UcTtlErrToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* Enable L3 MTU fail to CPU. This is required to generate ICMP too
   * big message (provided we enforce L3 MTU in hardware).
   */
  rv = bcm_switch_control_set(i, bcmSwitchL3MtuFailToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  if ((!SOC_IS_DRACO15(i)) && (!SOC_IS_TUCANA(i)) &&(!SOC_IS_LYNX(i)))
  {
    /* Set the ECMP hashControl to include dstip also - for XGS3 only */
    rv = bcm_switch_control_get(i, bcmSwitchHashControl, &hashControl);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      return L7_FAILURE;
    }

    hashControl |=  (BCM_HASH_CONTROL_MULTIPATH_DIP | BCM_HASH_CONTROL_TRUNK_NUC_DST 
                     | BCM_HASH_CONTROL_TRUNK_NUC_SRC | BCM_HASH_CONTROL_TRUNK_UC_SRCPORT);

    /* L4 ports hashControl below is valid for ECMP as well regular UC 
     * trunk load balacing 
     */
    hashControl |=  BCM_HASH_CONTROL_MULTIPATH_L4PORTS;

    rv = bcm_switch_control_set(i, bcmSwitchHashControl, hashControl);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      return L7_FAILURE;
    }

    /* Enable Directed Mirroring on all XGS3 devices 
     * SDK init code defaults to Legacy/Draco 1.5 mirroring mode
     */
    rv = bcm_switch_control_set(i, bcmSwitchDirectedMirroring, 1);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      return L7_FAILURE;
    }

    /* When mirror mode is changed, the mirror module MUST be re-initialized */
    rv = bcm_mirror_init(i);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      return L7_FAILURE;
    }
  }
  else 
  {
    /* For XGS2 devices, set L3DstMiss and L3Err packets to CPU. 
     * These controls are supported on XGS3 too [compatibility], but
     * could be removed for XGS3 in future.
     */
    rv = bcm_switch_control_set(i, bcmSwitchUnknownL3DestToCpu, PTIN_TRAP_TO_CPU);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
	return L7_FAILURE;

    rv = bcm_switch_control_set(i, bcmSwitchL3HeaderErrToCpu, PTIN_TRAP_TO_CPU);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
	return L7_FAILURE;
  }

  /* Enable source port mismatch to CPU. This was a FIXUP before */
  rv = bcm_switch_control_set(i, bcmSwitchIpmcPortMissToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* PTin added: init */
  #if 1
  rv = bcm_switch_control_set(i, bcmSwitchIpmcTtl1ToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }
  rv = bcm_switch_control_set(i, bcmSwitchL3UcastTtl1ToCpu, PTIN_TRAP_TO_CPU);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }
  #endif

  /* Include the 20 byte IFG for egress shaping. Note that we modified the 
     SDK so that the FP meters are not also affected by this function call. */
  rv = bcm_switch_control_set(i, bcmSwitchMeterAdjust, 20);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* Allow both L2 and L3 copies of a packet to go out on a physical 
   * port if it happens to be a member of both the L2 and L3 bitmaps of the 
   * replication group. This is the desired IPMC behavior (by default).
   * See #113166.
   */
  rv = bcm_switch_control_set(i, bcmSwitchIpmcSameVlanPruning, 0); /* bit is inversed */
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  if(hapiBroadRoboCheck() == L7_TRUE)
  {
    bcm_switch_control_set(i, bcmSwitchDosAttackToCpu, 1);
  }

  /* Set sflow parameters */
  rv = bcm_switch_control_set(i, bcmSwitchSampleIngressRandomSeed, 
                              FD_SFLOW_SAMPLE_RANDOM_SEED);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchSampleEgressRandomSeed, 
                              FD_SFLOW_SAMPLE_RANDOM_SEED);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchCpuSamplePrio, 
                              FD_SFLOW_CPU_SAMPLE_PRIORITY);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchSnapNonZeroOui, 1);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* Setup the defaults for RTAG7 enhanced hashing (used for trunks/ECMP) */
  if (hapiBroadRtag7SwitchControlSet(i) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Turn off bcmFieldControlArpAsIp/bcmFieldControlRarpAsIp. Otherwise the 
   * mac-address passed to the VFP is the mac in the ARP header instead
   * of the ethernet header source mac.
   */
  rv = bcm_field_control_set(i, bcmFieldControlArpAsIp, 0);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_field_control_set(i, bcmFieldControlRarpAsIp, 0);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }
  
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Modify flood mask for various traffic to cpu
*
* @param   none
*
* @returns none
*
* @notes   none
*
* @end
*
*********************************************************************/
void hpcHardwareBlockMaskSet(L7_int32 unit)
{
  L7_int32        port;
  L7_uint32       floodBlockFlag = 0;
  L7_int32        rv;

  /* Block Unknown DAs to CPU from all physical ports */
  floodBlockFlag |= BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST;

  /* For Raptor, block unknown multicast traffic to CPU. With out L3 MCAST
   * support, we do not need unknown multicast data coming to CPU
   */
  if (SOC_IS_RAPTOR(unit) || SOC_IS_HAWKEYE(unit))
  {
    floodBlockFlag |= BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST;
  }

  if (!SOC_IS_XGS_FABRIC(unit))
  {
    PBMP_PORT_ITER (unit, port)
    {
#ifdef L7_ROBO_SUPPORT
      if(IS_E_PORT(unit,port))
      {
        continue;
      }  
#endif
      rv = bcm_port_flood_block_set(unit, port, CMIC_PORT(unit), floodBlockFlag);
      if (rv != BCM_E_NONE && rv != BCM_E_UNAVAIL)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                "hpcHardwareBlockMaskSet: could not set block mask %d\n", rv);
      }
    }

    /* Set the flood mask for loopback port */
    PBMP_ALL_ITER (unit, port)
    {
      if (IS_LB_PORT(unit, port))
      {
        rv = bcm_port_flood_block_set(unit, port, CMIC_PORT(unit), floodBlockFlag);
        if (rv != BCM_E_NONE && rv != BCM_E_UNAVAIL)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                  "hpcHardwareBlockMaskSet: could not set block mask %d\n", rv);
        }
          
      }
    }
  }
}

/*********************************************************************
* @purpose  Setup the cpu hardware cosq rate limits
*
* @param    bcm_unit        {(input)} Bcm device to be configured
* @param    num_cosq_queues {(input)} Number of cos queues in the device
* @param    kbps_rate_limit {(input)} Rate limits in kbps
* @param    pps_rate_limit  {(input)} Rate limit in pps
*                                       
* @returns  BCM_E_XX
*
* @comments    
*       
* @end
*********************************************************************/
L7_int32 hpcHardwareCpuCosqHwRateLimitSet(L7_int32 bcm_unit, 
                                          L7_int32 num_cos_queues,
                                          L7_int32 *kbps_rate_limit,
                                          L7_int32 *pps_rate_limit)
{
  int cos_queue = 0, rv = BCM_E_NONE;

  /* Set the kbps rate limits */

  if ((SOC_IS_FB_FX_HX(bcm_unit)) || (SOC_IS_TUCANA(bcm_unit)) || 
      (SOC_IS_BRADLEY(bcm_unit)) || (SOC_IS_RAVEN(bcm_unit)))
  {

    if (!SOC_IS_RAPTOR(bcm_unit)  && !SOC_IS_HAWKEYE(bcm_unit)) 
    {
         
      for (cos_queue = 0; cos_queue < num_cos_queues; cos_queue++)
      {
        if (kbps_rate_limit[cos_queue] > 0)
        {
          rv = hapiBroadCpuCosqRateSet(bcm_unit, cos_queue,
                                       kbps_rate_limit[cos_queue], 
                                       BROAD_CPU_KBPS_RATE_LIMIT);
        }
      }
    }
  } 

  /* On a Helix, turn on software rate-limiting for cpu port queues
   * to take care of the hardware issue described in 38082
   *
   * Additionally, on hardware that supports hardware based packet rate limit 
   */
  if (SOC_IS_HELIX(bcm_unit) || 
      soc_feature(bcm_unit, soc_feature_packet_rate_limit))
  {
    for (cos_queue = 0; cos_queue < num_cos_queues; cos_queue++)
    {
      if (pps_rate_limit[cos_queue] > 0)
      {
        rv = hapiBroadCpuCosqRateSet(bcm_unit, cos_queue,
                                     pps_rate_limit[cos_queue],
                                     BROAD_CPU_PPS_RATE_LIMIT);
      }
    }
  }

  return rv;
}


/*********************************************************************
* @purpose  Setup the cpu hardware cosq rate limits during warm start
*
* @param    warmStartBegin  {(input)} Indicates warm start is beginning/ending.
*                                     L7_TRUE: Warm start begin. Will set
*                                              the rate limit to low.
*                                     L7_FALSE: Warm start end. Restore
*                                               the rate limit to normal.
*                                       
* @returns  none
*
* @comments    
*       
* @end
*********************************************************************/
void hpcHardwareWarmStartCpuCosqHwRateLimitSet(L7_BOOL warmStartBegin)
{
  L7_int32 num_queues = 0, rv = BCM_E_NONE;
  L7_int32 bcm_unit;

  num_queues = sizeof(cpu_kbps_rate_limit_per_cos) / sizeof(L7_int32);

  
  for (bcm_unit = 0; bcm_unit < bde->num_devices(BDE_SWITCH_DEVICES); bcm_unit++)
  {
  
    if (SOC_IS_XGS_FABRIC(bcm_unit))
    {
      continue;
    }

    if (warmStartBegin == L7_TRUE)
    {
      rv = hpcHardwareCpuCosqHwRateLimitSet(bcm_unit, num_queues, 
                                            cpu_warm_start_kbps_rate_limit_per_cos,
                                            cpu_warm_start_pps_rate_limit_per_cos);
    }
    else
    {
      rv = hpcHardwareCpuCosqHwRateLimitSet(bcm_unit, num_queues, 
                                            cpu_kbps_rate_limit_per_cos,
                                            cpu_pps_rate_limit_per_cos);
    }
  }

  if (rv != BCM_E_NONE)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
            "Driver failed to set the Cpu cosq hardware rate limits, error code %d\n",
            rv);  
  }

  return;
}

extern void hapiBroadXgs3Opcode0Patch(int unit,int enable);

/*********************************************************************
* @purpose  Apply default configuration to the hardware.
*           This function is invoked during IPL and every time that 
*           the hardware is reset with bcm_clear().
*
* @param    void
*                                       
* @returns  none
*
* @comments    
*       
* @end
*********************************************************************/
void hpcHardwareDefaultConfigApply(void)
{
  int i;
  int index;
  int rv;
  bcm_pbmp_t ubmp;
  int port;
  bcm_cos_queue_t cosq;
  bcm_cos_t       priority;
  int             rate_limit;
  int             num_queues = 0;
  L7_uint32       schedulerMode;

  /* change the vlan on which stacking traffic runs */
  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
  {
#ifdef L7_STACKING_PACKAGE
    /* HPC keeps track of link status in order to report stacking port
    ** link changes to the Stack Port Manager.
    */
    rv = bcm_linkscan_register (i,hpcStackPortLinkStatusCallback);
    if (rv != BCM_E_NONE)
    {
      L7_LOG_ERROR (i);
    }
#endif
    /* PTin modified: SDK 6.3.0 */
    #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
    rv = bcm_custom_register(i, custom_bcmx_port_handler, (void *) 0);
    #else
    rv = bcm_custom_register(i, custom_bcmx_port_handler);
    #endif
    if (rv != BCM_E_NONE)
    {
      L7_LOG_ERROR (rv);
    }

    if (!SOC_IS_SAND(i))
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        /* Configure a straight mapping from internal priority to cosq. This allows all cosq's to 
        ** be used. We will keep all dot1p / class selcector traffic on the low priority queues
        ** by advertising just 4 queues (2 queues for Strata) and using the FFP.              
        ** Note: that this is a bcmx call that iterates through the units and should be      
        ** independent of whether units are draco/strata but number of cosq depends on whether
        ** unit is draco/strata. We will allow paramter failures for this reason.           
        */
        for (priority = 0; priority < 8; priority++)
        {

  #ifdef L7_STACKING_PACKAGE
          static int xgs_stack_cos_map[] = {0,1,2,3,4,5,6,6};
          /* PTin added: new switch 56689 (Valkyrie2) */
          /* PTin added: new switch 5664x (Triumph3) */
          /* PTin added: new switch 56843 (Trident) */
          if (SOC_IS_FB_FX_HX(i) || SOC_IS_TR_VL(i) || SOC_IS_TRIUMPH2(i) || SOC_IS_APOLLO(i) || SOC_IS_VALKYRIE2(i) || SOC_IS_TRIDENT(i) ||
              SOC_IS_TRIUMPH3(i))
          {
            /* 
             * XGS3 does not suffer from the same problem that caused us to map 7 -> 6
             * in XGS2 devices.  Remarking 7 does not cause the packet to use cos 7 in this
             * scenario
             */
            xgs_stack_cos_map[7] = 7;
          }
          /* In stacking packages the COS queue 7 is reserved for stack control traffic,
          ** so we don't ever want to put external traffic on that queue.
          */
          cosq = xgs_stack_cos_map[priority];
  #else
  #if  (L7_MAX_CFG_QUEUES_PER_PORT  == 4)
          static L7_uchar8 xgs_cos_map[8] = {0,1,2,3,2,3,3,3};
  #else
          static L7_uchar8 xgs_cos_map[8] = {0,1,2,3,4,5,6,7};
  #endif
          cosq = xgs_cos_map [priority];
  #endif

          /* PTin added: debug/test new switch */
          if (SOC_IS_TRIDENT(i))
          {
            PT_LOG_NOTICE(LOG_CTX_MISC, "bcm_cosq_port_mapping_set invoked for priority %u (of %u)",priority,L7_MAX_CFG_QUEUES_PER_PORT);
            PBMP_PORT_ITER (i, port)
            {
              rv = bcm_cosq_port_mapping_set(i, port, priority, cosq);
              if ((rv != BCM_E_NONE) && (rv != BCM_E_PARAM))
              {
                L7_LOG_ERROR (rv);
              }
            }
          }
          else
          {
            rv = bcm_cosq_mapping_set(i, priority, cosq);
            if ((rv != BCM_E_NONE) && (rv != BCM_E_PARAM))
            {
              L7_LOG_ERROR (rv);
            }
          }
        }

         /* Configure all the queues on all ports for weighted round robin w/ weights 1 through 8. 
         ** Note: The Strata has 4 queues per port but the queue behavior for all ports is the
         ** same. The XGS has 8 queues per port and each port can have its own queue       
         ** configuration. The generic config bcm_cosq_init has already been run by this  
         ** point.                                                                       
         */

  #ifdef L7_STACKING_PACKAGE
         wrr_default_weights[7] = 0;
  #endif

  #ifdef L7_QOS_PACKAGE
         schedulerMode = hapiBroadCosQueueWDRRSupported() ? BCM_COSQ_DEFICIT_ROUND_ROBIN : BCM_COSQ_WEIGHTED_ROUND_ROBIN;
  #else
         schedulerMode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
  #endif
         /* PTin added: debug/test new switch */
         if (SOC_IS_TRIDENT(i))
         {
           bcm_pbmp_t pbmp;

           PT_LOG_NOTICE(LOG_CTX_MISC, "bcm_cosq_port_sched_set invoked for all physical ports");
           
           BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(i));
           rv = bcm_cosq_port_sched_set (i,
                                         pbmp,
                                         schedulerMode,
                                         wrr_default_weights,
                                         0);
           if (rv != BCM_E_NONE)
           {
             L7_LOG_ERROR (rv);
           }         
         }
         else
         {
           rv = bcm_cosq_sched_set (i,
                                    schedulerMode,
                                    wrr_default_weights,
                                    0);
           if (rv != BCM_E_NONE)
           {
             L7_LOG_ERROR (rv);
           }
         }
      }


      rate_limit = cpu_default_sw_rate_limit_per_unit;

      /* Set the cpu software rate-limit per unit for XGS2/Raptor only. For all 
      ** the other devices, software rate-limit per unit is turned-off. 
      */
      if ((SOC_IS_DRACO15(i)) || (SOC_IS_LYNX(i)) || (SOC_IS_XGS_FABRIC(i)))
      {
        rate_limit = cpu_xgs2_sw_rate_limit_per_unit;
      }
      else if (SOC_IS_RAPTOR(i)) 
      {
        rate_limit = cpu_raptor_sw_rate_limit_per_unit;
      }


      rv = bcm_rx_rate_set (i, rate_limit);
      if (rv != BCM_E_NONE) 
      {
        L7_LOG_ERROR (rv);
      }

      num_queues = sizeof(cpu_kbps_rate_limit_per_cos) / sizeof(L7_int32);
            
      rv = hpcHardwareCpuCosqHwRateLimitSet(i, num_queues, 
                                            cpu_kbps_rate_limit_per_cos,
                                            cpu_pps_rate_limit_per_cos);
      if (rv != BCM_E_NONE)
      {
        L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID,
                "Driver failed to set the Cpu cosq hardware rate limits, error code %d\n",
                rv);  
      }
    }

    /* Remove Ethernet ports from VLAN 1 */
    rv = bcm_vlan_port_remove(i, 1, PBMP_E_ALL(i));
    if (rv < 0)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP,"bcm_vlan_port_remove failed unit %d\n", i);
      L7_LOG_ERROR (rv);
    }

    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_create(i,HPC_STACKING_VLAN_ID);
    if ((rv < 0) && (rv != BCM_E_EXISTS))
    {
      PT_LOG_ERR(LOG_CTX_STARTUP,"bcm_vlan_create failed unit %d\n", i);
      L7_LOG_ERROR (rv);
    }

    rv = bcm_vlan_port_add(i,HPC_STACKING_VLAN_ID,PBMP_CMIC(i),ubmp);
    if (rv < 0)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP,"bcm_vlan_port_add failed unit %d\n", i);
      L7_LOG_ERROR (rv);
    }

    PBMP_E_ITER (i, port)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        if (hpcIsBcmPortStacking (i, port) == L7_FALSE)
        {
          rv = bcm_port_enable_set (i, port , 0);
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }

          /* Disable learning on the ports. The port security component sets
          ** up learning.
          */
#ifndef L7_ROBO_SUPPORT
/* PTin modified: Dune */
#if 1
          /* Enable the learning mode on all ports */
          rv = bcm_port_learn_set (i,port, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
#else
#ifdef L7_MACLOCK_PACKAGE
          PT_LOG_NOTICE(LOG_CTX_STARTUP,"I am here!");
          rv = bcm_port_learn_set (i,port,  0);
#else 
          PT_LOG_NOTICE(LOG_CTX_STARTUP,"I am here!");
          /* If PML component is not present, we have to enable the learning mode on all ports */
          rv = bcm_port_learn_set (i,port, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
#endif
#endif
#endif
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }

          if (!SOC_IS_SAND(i))
          {
            rv = bcm_port_pfm_set(i, port, BCM_PORT_PFM_MODEC);   /* PTin modified: L2 */
            PT_LOG_NOTICE(LOG_CTX_STARTUP,"bcm_port_pfm_set configuration to mode C: unit=%d,port=%d => rv=%d (%s)", i, port, rv, bcm_errmsg(rv));
            if (L7_BCMX_OK(rv) != L7_TRUE && rv != BCM_E_UNAVAIL)
            {
              L7_LOG_ERROR (rv);
            }
          }

          /* Set the spanning-tree state for each front-panel port to
          ** disable as bcm_clear sets all the port states to forwarding
          ** in bcm_esw_auth_init 
          */
          /* PTin modified: Dune */
          rv = bcm_port_stp_set(i, port, BCM_STG_STP_FORWARD);
          if (L7_BCMX_OK(rv) != L7_TRUE && rv != BCM_E_UNAVAIL)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                    "Failed to set port %d/%d state to disable rv %d\n",
                    i, port, rv);
          }

        }

        /* PTin modified: Dune */
        /* Enable ingress+egress filtering on all the ports */
        rv = bcm_port_vlan_member_set(i, port, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS | BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP);
        if (L7_BCMX_OK(rv) != L7_TRUE && rv != BCM_E_UNAVAIL)
        {
          L7_LOG_ERROR (rv);
        }
      }
    }

    /* Send BPDUs and CPU-MAC to CPU with high priority.
    ** Although we have a filters that set higher priority the 5695 gives preference
    ** to these registers instead of the filter.
    */
    if (SOC_IS_XGS_SWITCH(i))
    {
        if (SOC_IS_FB_FX_HX(i))
        {
          /* For XGS3, we use the CAP to get reserved MAC addresses to the CPU
           * We need to set this to priority lower than BPDU COS so that ARP_REPLIES
           * Don't adversely impact BPDUs, and LACPDUs, also ARP replies shouldn't impact
           * high priority protocols like RIP/BGP/OSPF.
           */
        if (!SOC_IS_RAPTOR(i) && !SOC_IS_RAVEN(i) && !SOC_IS_HAWKEYE(i)) 
        { /* This register is not supported on the  above silicons */
          rv = bcm_switch_control_set (i, bcmSwitchCpuProtocolPrio, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }
          }

          if (SOC_IS_FIREBOLT2(i) || SOC_IS_RAPTOR(i) || SOC_IS_RAVEN(i)) 
          {
             /* Set priority of ARP sent to CPU - applies to ARP replies only */ 
             rv = bcm_switch_control_set(i, bcmSwitchCpuProtoArpPriority, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
             if (rv != BCM_E_NONE)
             {
                L7_LOG_ERROR (rv);
             }
          }
          /* Send L2 static station movement packets to cpu */
          rv = bcm_switch_control_set (i, bcmSwitchL2StaticMoveToCpu, 1);
        }
        else if (SOC_IS_BRADLEY(i))
        {
          rv = bcm_switch_control_set (i, bcmSwitchCpuProtoBpduPriority, HAPI_BROAD_INGRESS_BPDU_COS);
        }
        /* PTin added: new switch 56689 (Valkyrie2) */
        /* PTin added: new switch 5664x (Triumph3) */
        /* PTin removed: new switch 56843 (Trident) */
        else if (SOC_IS_TR_VL(i) || SOC_IS_SCORPION(i) || SOC_IS_TRIUMPH2(i) || SOC_IS_APOLLO(i) || SOC_IS_VALKYRIE2(i) /*|| SOC_IS_TRIDENT(i)*/ ||
                 SOC_IS_TRIUMPH3(i))
        {
          bcm_rx_reasons_t reason, no_reason;
          int              internal_priority;

          index = 0;

          BCM_RX_REASON_CLEAR_ALL(reason);
          BCM_RX_REASON_SET(reason, bcmRxReasonL3HeaderError);
          rv = bcm_rx_cosq_mapping_set(i, index, reason, reason, 0, 0, 0, 0, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }
          index++;

          BCM_RX_REASON_CLEAR_ALL(reason);
          BCM_RX_REASON_SET(reason, bcmRxReasonL3Slowpath);
          rv = bcm_rx_cosq_mapping_set(i, index, reason, reason, 0, 0, 0, 0, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }
          index++;

          BCM_RX_REASON_CLEAR_ALL(reason);
          BCM_RX_REASON_SET(reason, bcmRxReasonTtl1);
          rv = bcm_rx_cosq_mapping_set(i, index, reason, reason, 0, 0, 0, 0, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }
          index++;

          /* Set IGMP/NDP traffic to come on higher COS. We have system policy for IGMP, but
           * for MLD we rely on CPU_CONTROL option. IGMP and MLD are assigned same reason
           * code.
           */
          BCM_RX_REASON_CLEAR_ALL(reason);
          BCM_RX_REASON_SET(reason, bcmRxReasonIgmp);
          rv = bcm_rx_cosq_mapping_set(i, index, reason, reason, 0, 0, 0, 0, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }
          index++;

          /* Set up a one to one mapping of internal priority to CPU cosq. */
          BCM_RX_REASON_CLEAR_ALL(no_reason);
          for (internal_priority = 0; internal_priority < 8; internal_priority++)
          {
            rv = bcm_rx_cosq_mapping_set(i, index, no_reason, no_reason, internal_priority, 0x07, 0, 0, internal_priority);
            if (rv != BCM_E_NONE)
            {
              L7_LOG_ERROR (rv);
            }
            index++;
          }

          if (SOC_IS_SCORPION(i))
          {
            rv = bcm_linkscan_register(i, hapiBroadMmuLinkscanCb);
            if (rv != BCM_E_NONE)
            {
              L7_LOG_ERROR(rv);
            }
          }
        }
        else
        {
          rv = bcm_switch_control_set (i, bcmSwitchCpuProtocolPrio, HAPI_BROAD_INGRESS_BPDU_COS);
        }

        if (rv != BCM_E_NONE)
        {
          L7_LOG_ERROR (rv);
        }

        /* PTin added: new switch 56689 (Valkyrie2) */
        /* PTin added: new switch 5664x (Triumph3) */
        /* PTin added: new switch 56843 (Trident) */
        if (!SOC_IS_TR_VL(i) && !SOC_IS_SCORPION(i) && !SOC_IS_TRIUMPH2(i) && !SOC_IS_APOLLO(i) && !SOC_IS_VALKYRIE2(i) && !SOC_IS_TRIDENT(i) &&
            !SOC_IS_TRIUMPH3(i))
        {
          /* This priority is used for packets that are copied to the CPU with a classifier, 
          ** and for IP traffic destined to the CPU due to IP address in the frames or
          ** IP header errors.
          */
          rv = bcm_switch_control_set (i, bcmSwitchCpuDefaultPrio, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }
        }
    } 

    if (!SOC_IS_SAND(i))
    {
      /* Set the MAC/L3DST lookup failure packet priority. */
      rv = bcm_switch_control_set(i, bcmSwitchCpuUnknownPrio, 0);
    }

    if (hpcSoftwareLearningEnabled() == L7_TRUE)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        /* Disable hardware learning on all front-panel ports.
        */
        PBMP_ALL_ITER (i, port)
        {
          /* Forward unknown SA/DA frames and send to the CPU.
          */
          if (IS_E_PORT(i, port))
          {
            rv = bcm_port_learn_set (i,port , 
                                     (BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD));
          } 
          else
          {
            /* The stacking ports are configured to forward unknown SA frames,
             ** but don't send a copy to the CPU. Otherwise in a large stack the
             ** mgmt unit will get a lot of copies of the packet and CPU-CPU
             ** tunneled packets will sometimes be duplicated.
             */
            rv = bcm_port_learn_set (i,port, (BCM_PORT_LEARN_FWD));
          }

          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }
        }

        /* PTin added: new switch 56689 (Valkyrie2) */
        /* PTin added: new switch 5664x (Triumph3) */
        /* PTin added: new switch 56843 (Trident) */
        if (!SOC_IS_TR_VL(i) && !SOC_IS_SCORPION(i) && !SOC_IS_TRIUMPH2(i) && !SOC_IS_APOLLO(i) && !SOC_IS_VALKYRIE2(i) && !SOC_IS_TRIDENT(i) &&
            !SOC_IS_TRIUMPH3(i) && !SOC_IS_SAND(i))
        {
          /* Send unknown SA frames to the CPU with priority 0.
          */
          rv = bcm_switch_control_set (i, bcmSwitchCpuUnknownPrio, 0);
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }
        }
      }
    }

    /* Some of the devices have additional priorities for SlowPath */
    if (soc_feature(i, soc_feature_cpu_proto_prio))
    {
      int feature;
    
      rv = bcm_switch_control_get(i, bcmSwitchCpuProtoExceptionsPriority,
                                  &feature);
      if (rv == BCM_E_NONE)
      {
        /* Set priority of IPv4/IPv6 packets with TTL=1 to CPU */
        rv = bcm_switch_control_set(i, bcmSwitchCpuProtoExceptionsPriority,
                                    HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
        if (rv != BCM_E_NONE)
        {
          L7_LOG_ERROR (rv);
        }
      }
      /* Set priority of IPv4/IPv6 packets with options or IPv6
       * packets with 0 len. payload/jumbo option to CPU
       */
      rv = bcm_switch_control_get(i, bcmSwitchCpuProtoIpOptionsPriority,
                                  &feature);
      if (rv == BCM_E_NONE)
      {
        rv = bcm_switch_control_set(i, bcmSwitchCpuProtoIpOptionsPriority,
                                    HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
        if (rv != BCM_E_NONE)
        {
          L7_LOG_ERROR (rv);
        }
      }
    }
  
    /* Set priorities for the packet sent to CPU on Bradley.
     * It has got one more CPU control register.
     */
    if (SOC_IS_BRADLEY(i))
    {  
      /* Send L2 static station movement packets to cpu */
      rv = bcm_switch_control_set (i, bcmSwitchL2StaticMoveToCpu, 1);
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (rv);
      }
      /* Set priority of ARP sent to CPU */ 
      rv = bcm_switch_control_set(i, bcmSwitchCpuProtoArpPriority, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (rv);
      }
      /* Set prioirty of IPv4/IPv6 multicast reserved packets sent to CPU.
       * Dont know yet which to send. Sending it to a low priority queue 
       */
      rv = bcm_switch_control_set(i, bcmSwitchCpuProtoIpmcReservedPriority, HAPI_BROAD_INGRESS_LOWEST_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (rv);
      }
    } 

    /* Enable dual hashing on all devices that support this feature 
     * Dual hashing reduces the probability of hash collision and 
     * there is no reason not to use this feature when supported 
     * (hence treat this as default config whereever applicable). 
     */

    if (soc_feature(i, soc_feature_dual_hash))
    {
       int hashControl;

       /* The key is to select the a different hashing algorithm 
        * than L2/L3 hash. (default CRC32L).
        */

       rv = bcm_switch_control_get(i, bcmSwitchHashL2, &hashControl);
       if (rv != BCM_E_NONE && rv != BCM_E_UNAVAIL)     /* PTin modified: new switch 5664x (Triumph3) */
       {
          L7_LOG_ERROR (rv);
       }
       /* PTin added: new switch 5664x (Triumph3): Execute, only if success */
       if (rv == BCM_E_NONE)
       {
         if (hashControl == BCM_HASH_CRC32L)
            rv = bcm_switch_control_set(i, bcmSwitchHashL2Dual, BCM_HASH_CRC32U);
         else if (hashControl == BCM_HASH_CRC32U)
            rv = bcm_switch_control_set(i, bcmSwitchHashL2Dual, BCM_HASH_CRC32L);
         else if (hashControl == BCM_HASH_CRC16L)
            rv = bcm_switch_control_set(i, bcmSwitchHashL2Dual, BCM_HASH_CRC16U);
         else if (hashControl == BCM_HASH_CRC16U)
             rv = bcm_switch_control_set(i, bcmSwitchHashL2Dual, BCM_HASH_CRC16L);
    
         if (rv != BCM_E_NONE)
         {
            L7_LOG_ERROR (rv);
         }
       }
       else
       {
         PT_LOG_WARN(LOG_CTX_HAPI,"Dual Hash was not configured for L2");
       }

       if (soc_feature(i, soc_feature_l3))
       {
         rv = bcm_switch_control_get(i, bcmSwitchHashL3, &hashControl);
         if (rv != BCM_E_NONE && rv != BCM_E_UNAVAIL)   /* PTin modified: new switch 5664x (Triumph3) */
         {
            L7_LOG_ERROR (rv);
         }
         /* PTin added: new switch 5664x (Triumph3) - Execute, only if success */
         if (rv == BCM_E_NONE)
         {
           if (hashControl == BCM_HASH_CRC32L)
              rv = bcm_switch_control_set(i, bcmSwitchHashL3Dual, BCM_HASH_CRC32U);
           else if (hashControl == BCM_HASH_CRC32U)
              rv = bcm_switch_control_set(i, bcmSwitchHashL3Dual, BCM_HASH_CRC32L);
           else if (hashControl == BCM_HASH_CRC16L)
              rv = bcm_switch_control_set(i, bcmSwitchHashL3Dual, BCM_HASH_CRC16U);
           else if (hashControl == BCM_HASH_CRC16U)
              rv = bcm_switch_control_set(i, bcmSwitchHashL3Dual, BCM_HASH_CRC16L);

           if (rv != BCM_E_NONE)
           {
              L7_LOG_ERROR (rv);
           }
         }
         else
         {
           PT_LOG_WARN(LOG_CTX_HAPI,"Dual Hash was not configured for L3");
         }
       }
    }

    /* PTin added: Hash */
    #if 1
    if (ptin_hapi_hash_init() != L7_SUCCESS)
    {
      PT_LOG_NOTICE(LOG_CTX_HAPI,"Error initializing hash procedures");
      L7_LOG_ERROR(BCM_E_INIT);
    }
    #endif

    if (SOC_IS_RAPTOR(i) || SOC_IS_HAWKEYE(i)) 
    {
      rv = bcm_switch_control_set(i, bcmSwitchCpuProtoIgmpPriority, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (i);
      }

      rv = bcm_switch_control_set(i, bcmSwitchCpuProtoIpmcReservedPriority, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (i);
      }

      rv = bcm_switch_control_set(i,bcmSwitchCpuProtoDhcpPriority,HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (i);
      }

      rv = bcm_switch_control_set(i , bcmSwitchDhcpPktToCpu, 1 );
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (i);
      }


      rv = bcm_switch_control_set(i, bcmSwitchNdPktToCpu, 1);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOG_ERROR (rv);
      }

      rv = bcm_switch_control_set(i, bcmSwitchV4ResvdMcPktToCpu, 1);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOG_ERROR (rv);
      }
      rv = bcm_switch_control_set(i, bcmSwitchV6ResvdMcPktToCpu, 1);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        L7_LOG_ERROR (rv);
      }

      /* For Hawkeye, the hardware default value is 0, so enable it */
      if( SOC_IS_HAWKEYE(i) )
      {
        rv = bcm_switch_control_set(i,bcmSwitchIgmpReservedMcastEnable, 1);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          L7_LOG_ERROR (rv);
        }
        rv = bcm_switch_control_set(i,bcmSwitchMldReservedMcastEnable, 1);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          L7_LOG_ERROR (rv);
        }
     
      }
    }
    /* apply the Opcode 0 patch for XGS3 devices if necessary */
    hapiBroadXgs3Opcode0Patch(i,L7_TRUE); 

#ifdef L7_ROBO_SUPPORT 
    if (SOC_IS_ROBO(i))
    {
      hapiBroadSetRoboDefaultParameters(i);
    }
#endif

    if (!SOC_IS_SAND(i))
    {
#ifdef INCLUDE_L3
      /* Enable L3 Egress mode. Must be done before any host/route is 
       * created.
       */
      rv = bcm_switch_control_set(i, bcmSwitchL3EgressMode, 1);
      if (rv != BCM_E_NONE)
      {
        /* Treat this as fatal error, even if rv is E_UNAVAIL
         * HAPI L3 code assumes egress mode is ON!
         */
        L7_LOG_ERROR(rv);
      }
#endif

#ifdef L7_ROUTING_PACKAGE
      /* Set ECMP max paths */
      hapiBroadL3MaxEcmpPathsSet(i);
#endif
   
      /* Set up the non-ucast trunk block mask, so that unknown unicast
       * and multicast traffic gets distributed over a trunk. Enable this
       * where ever supported. 
       */
      for (index = 0; index < BCM_UNITS_MAX; index++)
      {
        /* It is OK if we fail to set this for some reason. Not fatal */
        rv = bcm_switch_control_set(i, bcmSwitchModuleType,
                                    BCM_SWITCH_CONTROL_MOD_TYPE_PACK(index, BCM_SWITCH_MODULE_XGS3));
      }

      /* For TRX devices, ensure that the egress engine doesn't overwrite dot1p value in pkt. */
      if (SOC_IS_TRX(i))
      {
        bcm_pbmp_t pbmp;
        bcm_port_t bcmPort;

        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(i));

        BCM_PBMP_ITER(pbmp, bcmPort)
        {
          bcm_port_control_set(i, bcmPort, bcmPortControlEgressVlanPriUsesPktPri, 1);
        }
      }

      if (hapiBroadSwitchControlSet(i) != L7_SUCCESS)
      {
        L7_LOG_ERROR(i);
      }

      /* Modify flood mask to cpu for various traffic */
      hpcHardwareBlockMaskSet(i);

#ifdef INCLUDE_L3
      /* keep port l3 bits on all the time */
      rv = bcm_l3_enable_set(i, L7_TRUE);
      if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
      {
        L7_LOG_ERROR (rv);
      }
#endif

#ifdef L7_ROUTING_PACKAGE
      rv = bcm_ipmc_enable(i, L7_FALSE);
      if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
      {
        L7_LOG_ERROR (rv);
      }
#endif

      /* Enable IP/Mac based vlan classification */
      rv = bcm_vlan_control_port_set(i, -1, bcmVlanLookupIPEnable, 1);
      if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
      {
        L7_LOG_ERROR (rv);
      }

      rv = bcm_vlan_control_port_set(i, -1, bcmVlanLookupMACEnable, 1);
      if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
      {
        L7_LOG_ERROR (rv);
      }
    }
  }
}

#ifdef L7_ROBO_SUPPORT
/*********************************************************************
* @purpose  Disables DLF,broadcast,multicast flood to the CPU ,passes
*           ARP packets
*
* @param    void
*                                       
* @returns  none
*
* @comments    
*       
* @end
*********************************************************************/
static L7_int32 hapiBroadSetRoboDefaultParameters( L7_int32 unit )
{
  L7_uint32  reg_addr, reg_value, temp;
  L7_int32     reg_len;
  L7_int32     rv;

  temp = 0;

  if( SOC_IS_ROBO53115(unit))
  {
    /*packets not reaching CPU*/

    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, IMP_CTLr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, IMP_CTLr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) 
    {
        return rv;
    }
    /* Broadcast packet */
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
       (unit, IMP_CTLr, &reg_value, RX_BCST_ENf, &temp));

    /* Multicast  packet */
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
       (unit, IMP_CTLr, &reg_value, RX_MCST_ENf, &temp));

    /* Unicast DLF Failure */
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, IMP_CTLr, &reg_value, RX_UCST_ENf, &temp));

    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len)) < 0) 
    {
        return rv;
    }

    /* set Reserved Multicast Untag check to set vlan tag */
    reg_addr = 0x3401;
    reg_value = 0x0a;
    reg_len = 1;
    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len)) < 0)
    {
        return rv;
    }
    /* Since CPU is blocked from receiving BCAST traffic
     * enabling HL_PRTC_CTRL register to trap ARP request/DHCP packets
     */
    rv = bcm_switch_control_set(unit, bcmSwitchArpReplyToCpu, 1);
    if (rv != BCM_E_NONE)
    {
      L7_LOG_ERROR(rv);
    }

    rv = bcm_switch_control_set(unit, bcmSwitchDhcpPktToCpu, 1);
    if (rv != BCM_E_NONE)
    {
      L7_LOG_ERROR(rv);
    }

    /* Enabling tagging with PVID for untagged GMRP/GVRP packets */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, VLAN_CTRL2r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, VLAN_CTRL2r);

    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) 
    {
        return rv;
    }

    temp = 1;
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
       (unit, VLAN_CTRL2r, &reg_value, EN_GMRP_GVRP_UNTAG_MAPf, &temp));
    
    if ((rv = (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len)) < 0) 
    {
      return rv;
    }
#if L7_FEAT_CUSTOM_LED_BLINK
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, LED_FUNC1_CTLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, LED_FUNC1_CTLr);

    reg_value = 0x3041;
    if ((rv = (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len)) < 0) 
    {
      return rv;
    }
#endif

 }
    
  return rv;
}
#endif

/*********************************************************************
* @purpose  Resets the Driver to a known state
*
* @param    void
*                                       
* @returns  L7_SUCCESS successfully reset the driver
* @returns  L7_FAILURE problem occurred while resetting the driver
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareDriverReset(void)
{
  int rv;
  int i;
  int port;
  USL_DB_TYPE_t  dbType;
  L7_uint32      idx;
  L7_BOOL        dbGroups[USL_LAST_DB_GROUP];

#ifdef L7_STACKING_PACKAGE
  /* clear the remote event reporting infrastructure */
  bcm_rlink_clear();
#endif

  /* Disable all front-panel ports before starting bcm_clear
  */
  for (i = 0; i < soc_ndev; i++)
  {
    PBMP_E_ITER (i, port)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        if (hpcIsBcmPortStacking (i, port) == L7_FALSE)
        { 
          rv = bcm_port_enable_set (i,port , 0);
          if (rv != BCM_E_NONE)
          { 
            L7_LOG_ERROR (rv);
          }

          /* Disable learning on the ports. The port security component sets
          ** up learning.
          */
          rv = bcm_port_learn_set (i,port,  0);
          if (rv != BCM_E_NONE)
          {
            L7_LOG_ERROR (rv);
          }
        }
      }
    }
  }



  /* suspend aging and learning while clearing the tables */
  usl_mac_table_sync_suspend();

  /* flush the address tables */
  usl_mac_table_all_flush();

  /* bring all the local devices and their drivers to a reset state */
  for (i = 0; i < soc_ndev; i++)
  {
    if (SOC_IS_XGS_FABRIC(i))
    {
      /* Don't clear the trunk table on the Herc chips. The trunks are set up
      ** during discovery. We will not do rediscovery after bcm_clear().
      **
      ** The following table(s) need to be cleared on the Herc:
      ** VLAN Table.
      ** COS Table.
      ** L2 Mcast Table.
      */

      rv = bcm_vlan_init (i);
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (rv);
      }

     /* Broadcom recommends not clearing the COS table for the 5670. This table should not 
     ** change during normal operation. Clearing this table may cause the DMA hang on the 5670
     ** for test cases that involve joining two running stacks.
     */

      rv = bcm_mcast_init (i);
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (rv);
      }

    } else
    {
      /* bcm_cosq_init has been removed from bcm_esw_clear by a LVL7_FIXUP. This is done
      ** to avoid re-programming of the MMU registers during a bcm_clear as the stack links
      ** remain up and forrward traffic during driver reset.
     */
      rv = bcm_clear(i);
      if (rv != BCM_E_NONE)
      {
        L7_LOG_ERROR (rv);
      }
    }
  }

  usl_mac_table_sync_resume();

  /* Reset the USL state-machine variables */
  usl_sm_reset();

  /* Reset USL Hw Id generation module */
  usl_hw_id_generator_reset();

  /* Clear the trunks from Hw as bcm_clear would not clear the trunks */
  usl_trunk_clear();

  /* reset all the USL databases in the driver */
  dbType = USL_OPERATIONAL_DB | USL_SHADOW_DB;
  for (idx = USL_FIRST_DB_GROUP; idx < USL_LAST_DB_GROUP; idx++)
  {
    dbGroups[idx] = L7_TRUE;    
  }

  usl_database_invalidate(dbType, dbGroups);

  hpcHardwareDefaultConfigApply ();

#ifdef L7_STACKING_PACKAGE
  hpcStackPortEnable (L7_TRUE);
#endif

    /* Re-initialize global structures in the HAPI driver. 
    */
#ifdef L7_MCAST_PACKAGE
  hapiBroadL3McastReInit ();
#endif


#ifdef L7_ROUTING_PACKAGE
  hapiBroadL3ReInit ();
#endif

  hapiBroadL2ReInit();

  return L7_SUCCESS;
} 

/*********************************************************************
* @purpose  Enables ASF mode on all local BCM units.
*
* @param    void
*                                       
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareDriverAsfEnable(void)
{
  L7_RC_t rc = L7_SUCCESS;
  int unit;

  for (unit=0; unit < soc_ndev; unit++)
  {
    if (bcm_switch_control_set(unit, bcmSwitchAlternateStoreForward, 1) != BCM_E_NONE)
    {
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/* PTin TODO: SDK 6.3.0 */
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* None */
#else
/*********************************************************************
* @purpose  Qualifies BCMX filter with non-stack ports.
*
* @param    void
*                                       
* @returns  none
*
* @comments This function is called for BCMX filters that are installed   
* @comments on all ports in the box. For stand-alone boxes and HiGig
* @comments stackable systems the filter is not changed. 
* @comments For the front-panel stacked systems the filter is qualified
* @comments with front panel ports which are not configured for stacking.
*       
* @end
*********************************************************************/
void hpcBcmxFilterStackPortRemove(bcm_filterid_t *bcmx_filter)
{
 bcmx_lplist_t         portList;
 int                   rv;
 DAPI_USP_t                 searchUsp;
 BROAD_PORT_t              *tempHapiPort;

 if (cnfgrIsFeaturePresent (L7_FLEX_STACKING_COMPONENT_ID, 
                           L7_FPS_FEATURE_ID) == L7_FALSE)  
 {
   return;  /* Don't need to qualify the filter with a port list */
 }

 rv = bcmx_lplist_init(&portList,L7_MAX_INTERFACE_COUNT,0);
 if (L7_BCMX_OK(rv) != L7_TRUE)
 {
   L7_LOG_ERROR(rv);
 }

 /* Add all physical ports to the list. Stack ports are excluded by the isValidUsp() test.
 */
 for (searchUsp.unit=0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
 {
   if (dapi_g->unit[searchUsp.unit] == L7_NULLPTR)
     {
       continue;
     }

    for (searchUsp.slot=0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
    {     
      if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot] == L7_NULLPTR)
       {
         continue;
       }

      if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardType == SYSAPI_CARD_TYPE_LINE)
      {
        for (searchUsp.port=0;
            searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot;
            searchUsp.port++)
        {          
          if (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->port == L7_NULLPTR)
          {
            continue;
          }

          /* Note that we don't use the isValidUsp() test in this code because 
          ** the function may be invoked during card insertion, so the 
          ** "cardPresent" test will fail.
          */
          if (spmFpsPortStackingModeCheck (searchUsp.unit, searchUsp.slot, (searchUsp.port + 1)) == L7_TRUE)
          {
            continue;
          }

          tempHapiPort = HAPI_PORT_GET(&searchUsp, dapi_g);

          if (tempHapiPort == L7_NULLPTR)
          {
            continue;
          }

          BCMX_LPLIST_ADD(&portList, tempHapiPort->bcmx_lport);
        }
      }
    }
  }

 rv = bcmx_filter_qualify_ingress (*bcmx_filter, portList);
 if (L7_BCMX_OK(rv) != L7_TRUE)
 {
   L7_LOG_ERROR (rv);
 }

 bcmx_lplist_free(&portList);
}
#endif

/* The following code is a patch to support Ax-B0 XGS3 parts 
 *  It addresses the Higig Problem Opcode 0 issue
 */
#ifdef BCM_FIELD_SUPPORT
extern int mh_opcode0_priority_select(int unit, 
                                      int group_priority,
                                      bcm_field_group_t *group,
                                      bcm_field_entry_t *entry,
                                      int               *obm_reg_changed);
extern void
mh_opcode0_priority_clear(int unit, 
                          bcm_field_group_t group,
                          bcm_field_entry_t entry,
                          int obm_reg_changed);

extern int hapiBroadXgs3HigigPriorityPatch(int unit);
extern int hapiBroadPolicyBcmReservedGroupGet(int unit);

typedef struct mh_opcode0_fp_s {
    bcm_field_group_t   group;
    bcm_field_entry_t   entry;
    int                 obm_reg_changed;
} mh_opcode0_fp_t;

#ifdef BCM_ESW_SUPPORT
static mh_opcode0_fp_t opcode0GroupMap[BCM_MAX_NUM_UNITS];
#endif
#endif

void hapiBroadXgs3Opcode0Patch(int unit,int enable)
{
#if defined(BCM_FIELD_SUPPORT) && defined(BCM_ESW_SUPPORT)
  int priority = hapiBroadPolicyBcmReservedGroupGet(unit);
  static int uninitialized = 1;
  int i;

  /* Check if patch needed */
  if (hapiBroadXgs3HigigPriorityPatch(unit) == 0)
    return;

  if (uninitialized)
  {
    uninitialized = 0;

    for (i = 0; i < BCM_MAX_NUM_UNITS;i++)
    {
      opcode0GroupMap[i].group = -1;
      opcode0GroupMap[i].entry = -1;
      opcode0GroupMap[i].obm_reg_changed = 0;
    }
  }

  if (enable && (opcode0GroupMap[unit].group < 0) && (opcode0GroupMap[unit].entry < 0))
  {
    (void)mh_opcode0_priority_select (unit, priority, 
                                      &opcode0GroupMap[unit].group,
                                      &opcode0GroupMap[unit].entry,
                                      &opcode0GroupMap[unit].obm_reg_changed);
  }
  else if (enable && ((opcode0GroupMap[unit].group >= 0) || (opcode0GroupMap[unit].entry >= 0)))
  {
    mh_opcode0_priority_clear (unit, 
                               opcode0GroupMap[unit].group, 
                               opcode0GroupMap[unit].entry,
                               opcode0GroupMap[unit].obm_reg_changed);
    opcode0GroupMap[unit].group = -1;
    opcode0GroupMap[unit].entry = -1;

    (void)mh_opcode0_priority_select(unit, priority, 
                                     &opcode0GroupMap[unit].group,
                                     &opcode0GroupMap[unit].entry,
                                     &opcode0GroupMap[unit].obm_reg_changed);
  }
  else
  {
    if ((opcode0GroupMap[unit].group >= 0) && (opcode0GroupMap[unit].entry >= 0))
    {
      mh_opcode0_priority_clear(unit,
                                opcode0GroupMap[unit].group,
                                opcode0GroupMap[unit].entry,
                                opcode0GroupMap[unit].obm_reg_changed);
      opcode0GroupMap[unit].group = -1;
      opcode0GroupMap[unit].entry = -1;
    }
  }
#endif
}

/* Ends the patch to support Ax-B0 XGS3 parts 
 *  It addresses the Higig Problem Opcode 0 issue
 */


/* 
 * Needed for back to back configurations
 */
extern void  lvl7_topo_interconnect(pbmp_t  *pbmp_xport_xe);

char * hpcPortNameGet(int cpuUnit, int bcmPort)
{
  return(SOC_PORT_NAME(cpuUnit, bcmPort));
}

int hapiBroadIncludeRCPU(void)
{
#ifdef INCLUDE_RCPU
  return 1;
#else
  return 0;
#endif
}

/*********************************************************************
* @purpose  Tell the Broadcom driver which ports are HG vs XE ports
*
* @param    lclUnitDesc - The descriptor with the info for the board
*                                       
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments these ports are referred to as XPORTS
*       
* @end
*********************************************************************/
L7_RC_t hpcXeHgSetup(void)
{
  pbmp_t        pbmp_xport_xe[BCM_LOCAL_UNITS_MAX];
  L7_char8      pbmp_xport_xe_str[SOC_PBMP_FMT_LEN];
  L7_char8      configString[64];
  int           i;
  int           portIndex;
  int           portNo;
  L7_uint32     slotIndex = 0;

  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;

  PT_LOG_TRACE(LOG_CTX_STARTUP,"Starting hpcXeHgSetup (%u slots)...", L7_MAX_PHYSICAL_SLOTS_PER_UNIT);

  /* clear the bitmaps */
  for (i = 0; i < BCM_LOCAL_UNITS_MAX;i++)
  {
    BCM_PBMP_CLEAR(pbmp_xport_xe[i]);
  }

  for (slotIndex = 0; slotIndex < L7_MAX_PHYSICAL_SLOTS_PER_UNIT; slotIndex++)
  {

    sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(slotIndex));

    /* Search each line card for 10G XAUI interfaces */
    if ((sysapiHpcCardInfoPtr != L7_NULLPTR) &&
        (sysapiHpcCardInfoPtr->type == SYSAPI_CARD_TYPE_LINE))
    {
      dapiCardPtr = sysapiHpcCardInfoPtr->dapiCardInfo;
      hapiSlotMapPtr = dapiCardPtr->slotMap;

      for (portIndex = 0; portIndex < sysapiHpcCardInfoPtr->numOfNiPorts; portIndex++)
      {
        portNo = dapiCardPtr->portMap[portIndex].portNum;

        /* Add each 10G XAUI interface to the bitmap, so they will default to 10G */
        /* Ethernet, if configured for stacking, it will be reconfigured to HiGIG */

        if ((sysapiHpcCardInfoPtr->portInfo[portNo].type == L7_IANA_10G_ETHERNET ||
             sysapiHpcCardInfoPtr->portInfo[portNo].type == L7_IANA_40G_ETHERNET ||
             sysapiHpcCardInfoPtr->portInfo[portNo].type == L7_IANA_100G_ETHERNET) &&
            (sysapiHpcCardInfoPtr->portInfo[portNo].connectorType == L7_XAUI ||
             sysapiHpcCardInfoPtr->portInfo[portNo].connectorType == L7_XLAUI ||
             sysapiHpcCardInfoPtr->portInfo[portNo].connectorType == L7_BACKPLANE_KR))
        {

          /* default to 10G and later remove the entries that are not needed */
          BCM_PBMP_PORT_ADD (pbmp_xport_xe[hapiSlotMapPtr[portNo].bcm_cpuunit],
                                           hapiSlotMapPtr[portNo].bcm_port);
        }
      }
    }
  }

  lvl7_topo_interconnect(pbmp_xport_xe);

#ifdef L7_STACKING_PACKAGE
{
/* check for any stacking related removals */
extern L7_RC_t hpcStackingXeHgSetup(pbmp_t *pbmp_xport_xe);
  hpcStackingXeHgSetup(pbmp_xport_xe);
}
#endif

  /* look for any XE/HG ports that should be XE */
  for (i = 0; i < BCM_LOCAL_UNITS_MAX;i++)
  {
    if (BCM_PBMP_NOT_NULL(pbmp_xport_xe[i]))
    {
      SOC_PBMP_FMT(pbmp_xport_xe[i], pbmp_xport_xe_str);
      sprintf(configString,"pbmp_xport_xe.%d",i);
      sal_config_set(configString,pbmp_xport_xe_str);
    }
  }

  PT_LOG_TRACE(LOG_CTX_STARTUP,"Finished hpcXeHgSetup successfully!");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Tell the Broadcom driver which ports are HL vs GE ports
*
* @param    lclUnitDesc - The descriptor with the info for the board
*                                       
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t hpcGeHlSetup(void)
{
  pbmp_t        pbmp_gport_stack[BCM_LOCAL_UNITS_MAX];
  L7_char8      pbmp_gport_stack_str[SOC_PBMP_FMT_LEN];
  L7_char8      configString[64];
  int           i;

  /* clear the bitmaps */
  for (i = 0; i < BCM_LOCAL_UNITS_MAX;i++)
  {
    BCM_PBMP_CLEAR(pbmp_gport_stack[i]);
  }

#ifdef L7_STACKING_PACKAGE
{
/* check for any stacking related removals */
extern L7_RC_t hpcStackingGeHlSetup(pbmp_t *pbmp_gport_stack);
	hpcStackingGeHlSetup(pbmp_gport_stack);
}
#endif

  for (i = 0; i < soc_ndev; i++)
  {
    SOC_PBMP_FMT(pbmp_gport_stack[i], pbmp_gport_stack_str);
    sprintf(configString,"%s.%d",spn_PBMP_GPORT_STACK, i);
    sal_config_set(configString, pbmp_gport_stack_str);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Stub function to point to SDK assert to log error
*       
* @end
*********************************************************************/
void hapiBroadAssert(const L7_char8 *expr, const L7_char8 *file, L7_uint32 line)
{
   log_error_code (0, (char *) file, line);
}

/* PTin added: workaround for parity errors */
#ifdef BCM_ESW_SUPPORT
#include "soc/trident.h"
int
soc_trident_port_cbl_table_parity_set(int unit, int enable)
{
  uint32 rval;
  soc_reg_t enable_reg = PORT_CBL_TABLE_PARITY_CONTROLr;
  soc_field_t enable_field = PARITY_ENf;

  soc_trident_pipe_select(unit, 0, 0);

  soc_reg32_get(unit, enable_reg, REG_PORT_ANY, 0, &rval);
  soc_reg_field_set(unit, enable_reg, &rval, enable_field, enable);
  soc_reg32_set(unit, enable_reg, REG_PORT_ANY, 0, rval);

//soc_reg32_get(unit, enable_reg, REG_PORT_ANY, 0, &rval);
//PT_LOG_NOTICE(LOG_CTX_STARTUP, "rval = 0x%08X", rval);

  soc_trident_pipe_select(unit, 0, 1);

  soc_reg32_get(unit, enable_reg, REG_PORT_ANY, 0, &rval);
  soc_reg_field_set(unit, enable_reg, &rval, enable_field, enable);
  soc_reg32_set(unit, enable_reg, REG_PORT_ANY, 0, rval);

//soc_reg32_get(unit, enable_reg, REG_PORT_ANY, 0, &rval);
//PT_LOG_NOTICE(LOG_CTX_STARTUP, "rval = 0x%08X", rval);

  soc_trident_pipe_select(unit, 0, 0);

  return SOC_E_NONE;
}
#endif
/* PTin end */


L7_RC_t hpcBroadInit()
{
  L7_uint32                    total_bcom_units, bcom_unit;
  const bcm_sys_board_t       *board_info;
  int                          rc, rv;

  PT_LOG_NOTICE(LOG_CTX_STARTUP,"hpcConfigSet() skipped!");
  //hpcConfigSet();

  total_bcom_units = bde->num_devices(BDE_SWITCH_DEVICES);

#if 1
  var_set_integer("units", total_bcom_units, FALSE, FALSE);

  for (bcom_unit = 0; bcom_unit < total_bcom_units; bcom_unit++)
  {
      PT_LOG_TRACE(LOG_CTX_STARTUP,"bcom_unit=%d", bcm_unit);
      /* coverity[stack_use_callee] */
      /* coverity[stack_use_overflow] */
      rv = sysconf_attach(bcom_unit);
      if (rv < 0)
      {
          PT_LOG_FATAL(LOG_CTX_STARTUP,"ERROR: SOC unit %d attach failed!", bcom_unit);
          L7_LOG_ERROR(0);
      }
  } /* for */

#ifndef NO_SAL_APPL
  char    *script;

  /* Add backdoor for mem tuner to update system configuration */
  soc_mem_config_set = sal_config_set;

  /*
   * If a startup script is given in the boot parameters, attempt to
   * load it.  This script is for general system configurations such
   * as host table additions and NFS mounts.
   */

  if ((script = sal_boot_script()) != NULL) {
    if (sh_rcload_file(-1, NULL, script, FALSE) != CMD_OK) {
      PT_LOG_ERR(LOG_CTX_STARTUP,"ERROR loading boot init script: %s\n", script);
    }
  }

#if 0
  /*
   * If a default init file is given, attempt to load it.
   */
  for (bcom_unit = 0; bcom_unit < total_bcom_units; bcom_unit++) {
      if (soc_attached(SOC_NDEV_IDX2DEV(bcom_unit))) {
          sh_swap_unit_vars(SOC_NDEV_IDX2DEV(bcom_unit));
          if (SOC_IS_RCPU_ONLY(SOC_NDEV_IDX2DEV(bcom_unit))) {
              /* Wait for master unit to establish link */
              sal_sleep(3);
          }
          PT_LOG_TRACE(LOG_CTX_STARTUP,"Going to load rc.soc file...");
          if (sh_rcload_file(SOC_NDEV_IDX2DEV(bcom_unit), NULL, "/usr/local/ptin/sbin/rc.soc", FALSE) != CMD_OK) {
              PT_LOG_ERR(LOG_CTX_STARTUP,"ERROR loading rc script on unit %d\n", SOC_NDEV_IDX2DEV(bcom_unit));
          }
          else
          {
            PT_LOG_TRACE(LOG_CTX_STARTUP,"rc.soc file loaded!");
          }
      }
  }

#if defined(BCM_EA_SUPPORT)
#if defined(BCM_TK371X_SUPPORT)
  if(BCM_E_NONE == soc_ea_do_init(total_bcom_units)){
      for(bcom_unit = 0; bcom_unit < total_bcom_units; bcom_unit++){
          if(soc_attached(SOC_NDEV_IDX2DEV(bcom_unit)) && SOC_IS_TK371X(SOC_NDEV_IDX2DEV(bcom_unit))){
               int rv;
               if ((rv = (bcm_init(SOC_NDEV_IDX2DEV(bcom_unit)))) < 0) { _SHR_ERROR_TRACE(rv);  return; }
          }
      }
  }
#endif
#endif
#endif

  if (total_bcom_units <= 0)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP,"No attached units.\n");
    L7_LOG_ERROR(0);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_STARTUP,"Units attached!");
  }
#endif /* NO_SAL_APPL */

#else
  for (bcom_unit=0; bcom_unit<total_bcom_units; bcom_unit++)
  {
    sysconf_attach(bcom_unit);
  }
#endif

  hpcConfigPhySet();

  for (bcom_unit=0; bcom_unit<total_bcom_units; bcom_unit++)
  {

#ifdef L7_TOOL_VALGRIND
      (void)soc_property_get(bcom_unit, spn_CDMA_TIMEOUT_USEC, 5000000);
      (void)soc_property_get(bcom_unit, spn_TDMA_TIMEOUT_USEC, 5000000);
#endif
    /* This single call is going to initialise all the modules */
    if ((rc = systemInit(bcom_unit)) != BCM_E_NONE)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_ALWAYS,
                     "\n%s %d: In %s call to 'systemInit' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
      return L7_FAILURE;
    }

    if ((rc = hapiBroadMmuInit()) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DRIVER_COMPONENT_ID,"hapiBroadMmuInit returned %d",rc);
    }

    if (hapiBroadMmuConfigModify(bcom_unit) != L7_SUCCESS) /* add cells to CPU COS 4-7 for bursts */
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "%s: MMU configuration has failed.\n",
                    __FUNCTION__);
      return (L7_FAILURE);
    }
    
    if (!SOC_IS_SAND(bcom_unit))
    {
      (void)bcm_switch_control_set(bcom_unit, bcmSwitchL2McastAllRouterPortsAddEnable, 0); 
    }
  }
  
  /* PTin added: workaround for parity errors */
#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
#ifdef BCM_ESW_SUPPORT
  soc_trident_port_cbl_table_parity_set(0, 0);
  PT_LOG_NOTICE(LOG_CTX_STARTUP, "port_cbl_table_parity_set(0)");
#endif
#endif

  /* 
   * Load post-SDK initialization SOC file.
   * This will allow the user to override properties from driver initialization.
   */
  hapiBroadSocFileLoad("sdk-postinit.soc", L7_TRUE);

  /* Initialize the board with stand alone function prior to stack formation */
  board_info = hpcBoardGet();
  if (board_info == L7_NULL) 
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," In %s:%d, could not find the board\n",
	              __FUNCTION__, __LINE__);
    return (L7_FAILURE);
  }

  board_info->sa_init(board_info, 0);

#ifndef L7_STACKING_PACKAGE
  /*  normally done as part of hpcBroadStackInit */
  if (hapiBroadMapDbCreate() == L7_FAILURE)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP,"Could not create port mapping database\n");
    return(L7_FAILURE);
  }

  /* normally done as part of hpcBroadStackInit */
  if (bcmx_uport_create_callback_set(lvl7_uport_create_callback) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP,"Could not register bcmx_uport_create_callback\n");
    return(L7_FAILURE);
  }
#endif

  return(L7_SUCCESS);
}

#if defined(INCLUDE_PHY_8706)
#if L7_FEAT_SF10GBT
#include "soc/phy/phy8481.h"

#define PHY_SF_OUI      0x000a68        /* Solarflare */
#define PHY_SF_SFX7101  0x07


STATIC int
phy_sfx7101_xgxs5_lb_set(int unit, soc_port_t port, int enable)
{
    int         rv;
    phy_ctrl_t *pc;
    uint16      lb_bit;

    rv = SOC_E_NONE;

    pc = INT_PHY_SW_STATE(unit, port);

    if (PHY_SINGLE_LANE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHYXGXS5_BLK1_LANE_CTRL2r(unit, pc,
                                             (enable) ? 0xFF0F : 0xFF00));
        return SOC_E_NONE;
    }

    /*
     * control RX signal detect, so that a cable is not needed for loopback
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS5_RXALL_CTRLr(unit, pc, (enable) ? 0x0010 : 0, 0x0010));

    /*
     * control the actual loopback bit
     */
    lb_bit = XGXS5_IEEE_CTRL_LB_ENA;
    if (IS_XE_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_xgxs_v5)) {
            lb_bit = 1;
        }
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS5_XAUI_MII_CTRLr(unit, pc,
                                        (enable) ? lb_bit : 0, lb_bit));

    return rv;
}

STATIC int
phy_sfx7101_xgxs5_init(int unit, soc_port_t port)
{
    int            preemph, idriver, pdriver;
    int            locked;
    uint16         pll_stat;
    phy_ctrl_t    *pc;
    soc_timeout_t  to;

    pc = INT_PHY_SW_STATE(unit, port);

    PHY_FLAGS_SET(unit, port, PHY_FLAGS_C45|PHY_FLAGS_COPPER);
  
    /* Power on the Core */
    SOC_IF_ERROR_RETURN
        (MODIFY_PHYXGXS5_BLK1_LANE_TESTr(unit, pc, (1 << 10), (1 << 10)));

    /* Disable PLL State Machine */
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS5_PLL_CTRLr(unit, pc, 0x5006));

    /* Turn off slowdn_xor */
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS5_PLL_CAP_CTRLr(unit, pc, 0x0000));

    if (IS_HG_PORT(unit,port)) {
        if (!PHY_SINGLE_LANE_MODE(unit, port)) {
            /*
             * Do this only for regular 10G/12G mode.
             * Independent Lane mode is configured above.
             */
            SOC_IF_ERROR_RETURN         /* Disable LssQ */
                (WRITE_PHYXGXS5_BLK0_CTRLr(unit, pc, 0x292f));
        }
        SOC_IF_ERROR_RETURN         /* Enable DTE mdio reg mapping */
            (WRITE_PHYXGXS5_BLK0_MISC_CTRL1r(unit, pc, 0x0000));
        preemph = 0x8;
        idriver = 0x7;
        pdriver = 0x9;
    } else {
        SOC_IF_ERROR_RETURN         /* Enable PMA/PMD mdio reg mapping */
            (WRITE_PHYXGXS5_BLK0_MISC_CTRL1r(unit, pc, 0x0200));
        preemph = 0xa;
        idriver = 0xf;
        pdriver = 0xf;
    }

    /* Enable PLL state machine */
    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS5_PLL_CTRLr(unit, pc, 0xf01e));

    /*
     * Transform CX4 pin out on the board to HG pinout
     */
    if (soc_property_port_get(unit, port, spn_CX4_TO_HIGIG, FALSE)) {
        /* Swap TX lanes */
        SOC_IF_ERROR_RETURN         /* Enable TX Lane swap */
            (WRITE_PHYXGXS5_BLK2_TX_LANE_SWAPr(unit, pc, 0x80e4));
        /* Flip TX Lane polarity */
        SOC_IF_ERROR_RETURN         /* Flip TX Lane polarity */
            (WRITE_PHYXGXS5_TXALL_ACTRL0r(unit, pc, 0x1020));
    } else {
        /* If CX4 to HG conversion is enabled, do not allow individual lane
         * swapping.
         */
        uint16 lane_map, i;
        uint16 lane, hw_map, chk_map;

        /* Update RX lane map */
        lane_map = soc_property_port_get(unit, port,
                                   spn_XGXS_RX_LANE_MAP, 0x0123) & 0xffff;

        if (lane_map != 0x0123) {
            hw_map  = 0;
            chk_map = 0;
            for (i = 0; i < 4; i++) {
                lane     = (lane_map >> (i * 4)) & 0xf;
                hw_map  |= lane << (i * 2);
                chk_map |= 1 << lane;
            }
            if (chk_map == 0xf) {
                SOC_IF_ERROR_RETURN     /* Enable RX Lane swap */
                   (MODIFY_PHYXGXS5_BLK2_RX_LANE_SWAPr(unit, pc,
                                                    0x8000 | hw_map, 0x80ff));
            } else {
                SOC_DEBUG_PRINT((DK_ERR,
                     "unit %d port %s: Invalid RX lane map 0x%04x.\n",
                     unit, SOC_PORT_NAME(unit, port), lane_map));
            }
        }

        /* Update TX lane map */
        lane_map = soc_property_port_get(unit, port,
                                  spn_XGXS_TX_LANE_MAP, 0x0123) & 0xffff;

        if (lane_map != 0x0123) {
            hw_map  = 0;
            chk_map = 0;
            for (i = 0; i < 4; i++) {
                lane     = (lane_map >> (i * 4)) & 0xf;
                hw_map  |= lane << (i * 2);
                chk_map |= 1 << lane;
            }
            if (chk_map == 0xf) {
                SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
                   (MODIFY_PHYXGXS5_BLK2_TX_LANE_SWAPr(unit, pc,
                                                    0x8000 | hw_map, 0x80ff));
            } else {
                SOC_DEBUG_PRINT((DK_ERR,
                     "unit %d port %s: Invalid TX lane map 0x%04x.\n",
                     unit, SOC_PORT_NAME(unit, port), lane_map));
            }
        }
    }

    /*
     * Flip TX lane polarity
     */
    if (soc_property_port_get(unit, port, "polarity_swap", FALSE)) {
        /* Flip TX Lane polarity */
        SOC_IF_ERROR_RETURN         /* Flip TX Lane polarity */
            (WRITE_PHYXGXS5_TXALL_ACTRL0r(unit, pc, 0x1020));
    }

    /* Allow config setting to override preemphasis, driver current,
     * and pre-driver current.
     */
    preemph = soc_property_port_get(unit, port,
                                    spn_XGXS_PREEMPHASIS, preemph);
    preemph = _shr_bit_rev8(preemph) >> 4;
    idriver = soc_property_port_get(unit, port,
                                    spn_XGXS_DRIVER_CURRENT, idriver);
    idriver = _shr_bit_rev8(idriver) >> 4;
    pdriver = soc_property_port_get(unit, port,
                                    spn_XGXS_PRE_DRIVER_CURRENT, pdriver);
    pdriver = _shr_bit_rev8(pdriver) >> 4;

    SOC_IF_ERROR_RETURN
        (WRITE_PHYXGXS5_TXALL_DRIVERr(unit, pc, ((preemph & 0xf) << 12) |
                                                ((idriver & 0xf) << 8) |
                                                    ((pdriver & 0xf) << 4)));

    /* Wait up to 0.1 sec for TX PLL lock. */
    soc_timeout_init(&to, 100000, 0);

    locked = 0;
    while (!soc_timeout_check(&to)) {
        SOC_IF_ERROR_RETURN
            (READ_PHYXGXS5_PLL_STATr(unit, pc, &pll_stat));
        /* Check pllSeqDone, freqDone, capDone, and ampDone */
        locked = ((pll_stat & (0xf000)) == 0xf000);
        if (locked) {
            break;
        }
    }

    if (!locked) {
        SOC_DEBUG_PRINT((DK_ERR,
            "unit %d port %s: XGXS PLL did not lock PLL_STAT %04x\n",
             unit, SOC_PORT_NAME(unit, port), pll_stat));
    } else {
        SOC_DEBUG_PRINT((DK_PHY | DK_VERBOSE,
                        "unit %d port %s: XGXS PLL locked in %d usec\n",
                        unit, SOC_PORT_NAME(unit, port),
                        soc_timeout_elapsed(&to)));
    }

    if (PHY_EXTERNAL_MODE(unit, port)) {
        if (IS_HG_PORT(unit, port)) {
            uint16 value;

            /* Enable XgxsNoCC if the HiGig port has external PHY.
             * XgxsNoCC enables in-band link signaling, per the 802.3ae
             * standard (|Q| ordered sets).  XgxsNoLssQNoCC is legacy
             * HiG mode, which does not support in-band link signaling.
             * Current XAUI cores support both 802.3ae compliant
             * signaling, as well as backwards compatibility with
             * prior (legacy) protocols.
             */
            SOC_IF_ERROR_RETURN
                (READ_PHYXGXS5_BLK0_CTRLr(unit, pc, &value));
            value &= ~(0x0f00);
            value |= (1 << 8);
            SOC_IF_ERROR_RETURN
                (WRITE_PHYXGXS5_BLK0_CTRLr(unit, pc, value));
        }
    }

    /* clear loopback (just in case) */
    SOC_IF_ERROR_RETURN
        (phy_sfx7101_xgxs5_lb_set(unit, port, FALSE));

    SOC_DEBUG_PRINT((DK_PHY,
                     "phy_sfx7101_xgxs5_init: u=%d p=%d\n",
                     unit, port));
    return SOC_E_NONE;
}

STATIC int
phy_sfx7101_link_get(int unit, soc_port_t port, int *link)
{
   uint16   pma_mii_stat,  pcs_mii_stat;
   phy_ctrl_t *pc;
    
   if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    if (port == 26 || port == 27)
    {
      *link = L7_FALSE;
      pc = EXT_PHY_SW_STATE(unit, port);
      /* PCS link status , DEVAD=3 */
      SOC_IF_ERROR_RETURN
          (READ_PHY8481_PCS_STATr(unit, pc, &pcs_mii_stat));

      /* Line side link status , DEVAD=1 */
      SOC_IF_ERROR_RETURN
          (READ_PHY8481_PMAD_STATr(unit, pc, &pma_mii_stat));

      *link = (pma_mii_stat & pcs_mii_stat & MII_STAT_LA) ? TRUE : FALSE;
    }

    return SOC_E_NONE;
}

STATIC int
phy_sfx7101_enable_set(int unit, soc_port_t port, int enable)
{
    uint16      data;       /* Holder for new value to write to PHY reg */
    uint16      mask;       /* Holder for bit mask to update in PHY reg */
    phy_ctrl_t *pc;

    pc = EXT_PHY_SW_STATE(unit, port);

    data = 0;
    mask = 1U << 0; /* Global PMD transmit disable */
    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
    } else {
        data = 1U << 0;  /* Global PMD transmit disable */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_PHY8481_PMAD_TX_DISABLEr(unit, pc, data, mask));

    return (SOC_E_NONE);
}

/* This code should be moved to sysbrds.c */
static uint8 phy_8706_get_addr(soc_port_t port)
{
    uint8          phy_addr;

    switch(port) {
    case 24:
        phy_addr = 0x52;
        break;
    case 25:
        phy_addr = 0x53;
        break;
    case 26:
        phy_addr = 0x56;
        break;
    case 27:
        phy_addr = 0x57;
        break;
    default:
        phy_addr = 0x0;
        break;
    }

    return phy_addr;
}
static phy_driver_t phy_8706drv_xe_custom;
static phy_driver_t phy_sfx7101_hg_custom;

static int _chk_cust_8706(int unit, soc_port_t port, soc_phy_table_t *my_entry,
             uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
  uint32      id0_addr, id1_addr;
  uint16      phy_addr = phy_8706_get_addr(port); /* logic highly specific to xgs3 arch. */
  
  if ( phy_addr == 0 )
     return FALSE;
  if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
        id0_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                         MII_PHY_ID0_REG);
        id1_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                         MII_PHY_ID1_REG);
    } else {
        id0_addr = MII_PHY_ID0_REG;
        id1_addr = MII_PHY_ID1_REG;
    }

  (void)soc_miimc45_read(unit, phy_addr, SOC_PHY_CLAUSE45_DEVAD(id0_addr),
                             SOC_PHY_CLAUSE45_REGAD(id0_addr), &phy_id0);
  (void)soc_miimc45_read(unit, phy_addr,  SOC_PHY_CLAUSE45_DEVAD(id1_addr),
                             SOC_PHY_CLAUSE45_REGAD(id1_addr), &phy_id1);

   if (PHY_OUI(phy_id0, phy_id1) == PHY_BCM8706_OUI &&
       PHY_MODEL(phy_id0, phy_id1) == PHY_BCM8706_MODEL) 
   {
     pi->phy_id0       = phy_id0;
     pi->phy_id1       = phy_id1;
     pi->phy_addr_int  = phy_addr;
     pi->phy_addr      = phy_addr;
     pi->phy_name      = my_entry->phy_name;
     /* telling power controller that this port is not 10GBASE-T */
     sysapiHpcNotify10GBASETPresence(unit, port, L7_FALSE); 
     sysapiHpcNotifySfpPlusPresence(unit, port);
     return TRUE;
   }
   return FALSE;
}
extern L7_RC_t hpcSF_InitModule(L7_uint32 turnOn);
static int _chk_cust_10GBASET(int unit, soc_port_t port, soc_phy_table_t *my_entry,
             uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
  uint32      id0_addr, id1_addr;
  uint16      phy_addr = 0;

 /* logic highly specific to xgs3 arch. Also make it port < 24 
    if we support 10G module someday in Bay 1 */
  if ( port < 26 ) 
     return FALSE;

  hpcXfpSF10GbTModuleInit(unit);

  SOC_IF_ERROR_RETURN
        (soc_phy_cfg_addr_get(unit,port,0,&phy_addr));

  id0_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                         MII_PHY_ID0_REG);
  id1_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                         MII_PHY_ID1_REG);

  (void)soc_miimc45_read(unit, phy_addr, SOC_PHY_CLAUSE45_DEVAD(id0_addr),
                             SOC_PHY_CLAUSE45_REGAD(id0_addr), &phy_id0);

  (void)soc_miimc45_read(unit, phy_addr,  SOC_PHY_CLAUSE45_DEVAD(id1_addr),
                             SOC_PHY_CLAUSE45_REGAD(id1_addr), &phy_id1);

   if (PHY_OUI(phy_id0, phy_id1)   == PHY_SF_OUI &&
       PHY_MODEL(phy_id0, phy_id1) == PHY_SF_SFX7101) 
   {
     pi->phy_id0       = phy_id0;
     pi->phy_id1       = phy_id1;
     pi->phy_addr_int  = phy_addr;
     pi->phy_addr      = phy_addr;
     pi->phy_name      = my_entry->phy_name;
     sysapiHpcNotify10GBASETPresence(unit, port, L7_TRUE); 
     return TRUE;
   }
   sysapiHpcNotify10GBASETPresence(unit, port, L7_FALSE); 
   return FALSE;
}

static  int cust_8706_settings
(
int unit, soc_port_t port, soc_phy_table_t *my_entry,
soc_phy_info_t *pi, phy_ctrl_t *int_pc
)
{
  uint16  phy_addr = pi->phy_addr;

  if (_phy_id_BCM8706 == my_entry->myNum)
  {
    phy_addr =  phy_8706_get_addr(port);
  }

  SOC_IF_ERROR_RETURN
      (soc_phy_cfg_addr_set(unit,port,0, phy_addr));
  SOC_IF_ERROR_RETURN
      (soc_phy_cfg_addr_set(unit,port,SOC_PHY_INTERNAL, phy_addr));

  PHY_ADDR(unit, port)     = phy_addr;
  PHY_ADDR_INT(unit, port) = phy_addr;

  int_pc->phy_id0   = pi->phy_id0;
  int_pc->phy_id1   = pi->phy_id1;
  int_pc->phy_id    = phy_addr;
  int_pc->phy_oui   = PHY_OUI(int_pc->phy_id0, int_pc->phy_id1);
  int_pc->phy_model = PHY_MODEL(int_pc->phy_id0, int_pc->phy_id1);
  int_pc->phy_rev   = PHY_REV(int_pc->phy_id0, int_pc->phy_id1);

  return BCM_E_NONE;

}

static soc_phy_cust_t socPhyCust8706;

static soc_phy_table_t phy8706_custom_entry =
  {_chk_cust_8706, _phy_id_BCM8706, "BCM8706/8726", &phy_8706drv_xe_custom, &socPhyCust8706};

static soc_phy_table_t phy10GBASET_custom_entry =
  {_chk_cust_10GBASET, _phy_id_numberKnown, "SFX7101", &phy_sfx7101_hg_custom, &socPhyCust8706};
#endif /* #ifdef L7_FEAT_SF10GBT */
#endif /* #if defined(INCLUDE_PHY_8706) */

/*********************************************************************
* Function:
*      system_init
* Purpose:
*      Reset the switch chip, re-initialize, and initialize the BCM API layer.
* Parameters:
*      unit - StrataSwitch unit #.
* Returns:
*      BCM_E_XXX
* Notes:
*      This is customized version of system_init() in vendor SDK. Ideally,
* we want to use the SDK version of init code, but there will always be 
* few things that are different/additional. 
**********************************************************************/

/* PTin modified: logger */
#define SYSTEM_INIT_CHECK(action, description)                  \
        if ((rv = (action)) < 0) {                              \
            PT_LOG_ERR(LOG_CTX_STARTUP,"%s: Error %d!", description, action); \
            msg = (description);                                \
            goto done;                                          \
        }                                                       \
        else                                                    \
        {                                                       \
            PT_LOG_INFO(LOG_CTX_STARTUP,"%s: OK!", description);   \
        }                                                       \

int
systemInit(int unit)
{
  soc_port_t     port;
  sal_usecs_t    usec;
  pbmp_t         pbmp;
  int            rv;
  char          *msg = NULL;
#if 0
#ifdef BCM_ROBO_SUPPORT
extern int soc_robo_misc_init(int ); 
extern int soc_robo_mmu_init(int );
#endif
#endif

#ifdef BCM_ROBO_SUPPORT
  SYSTEM_INIT_CHECK(soc_robo_reset_init(unit), "Device reset");
  //SYSTEM_INIT_CHECK(soc_robo_misc_init(unit), "Misc init");
  //SYSTEM_INIT_CHECK(soc_robo_mmu_init(unit), "MMU init");
#elif defined (BCM_XGS_SUPPORT)
  SYSTEM_INIT_CHECK(soc_reset_init(unit), "Device reset");
  //SYSTEM_INIT_CHECK(soc_misc_init(unit), "Misc init");
  //SYSTEM_INIT_CHECK(soc_mmu_init(unit), "MMU init");
#endif

  PT_LOG_INFO(LOG_CTX_STARTUP,"Loading rc.soc file...");

  /*
   * If a default init file is given, attempt to load it.
   */
  PT_LOG_TRACE(LOG_CTX_STARTUP,"Initializing unit %u...", unit);
  if (soc_attached(SOC_NDEV_IDX2DEV(unit))) {
    sh_swap_unit_vars(SOC_NDEV_IDX2DEV(unit));
    if (SOC_IS_RCPU_ONLY(SOC_NDEV_IDX2DEV(unit)))
    {
      /* Wait for master unit to establish link */
      sal_sleep(3);
    }
    PT_LOG_TRACE(LOG_CTX_STARTUP,"Going to load rc.soc file...");
    if (sh_rcload_file(SOC_NDEV_IDX2DEV(unit), NULL, "/usr/local/ptin/sbin/rc.soc", FALSE) != CMD_OK)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP,"ERROR loading rc script on unit %d\n", SOC_NDEV_IDX2DEV(unit));
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_STARTUP,"rc.soc file loaded!");
    }
  }

  PT_LOG_INFO(LOG_CTX_STARTUP,"rc.soc file loaded!");

#if defined(INCLUDE_PHY_8706)
#if L7_FEAT_SF10GBT
  socPhyCust8706.custPhySettings=cust_8706_settings;

  memset(&phy_8706drv_xe_custom, 0, sizeof (phy_8706drv_xe_custom));
  memcpy(&phy_8706drv_xe_custom, &phy_8706drv_xe, sizeof (phy_8706drv_xe_custom));
  phy_8706drv_xe_custom.pd_an_get = phy_null_an_get;
  phy_8706drv_xe_custom.pd_an_set = phy_null_set;
  soc_phy_add_entry(&phy8706_custom_entry);

  /* Let this be last driver to be registered with SDK, since we 
     are sending the notification to sensor about the presence/absence
     of 10GBASE-T from this driver only about the presence/absence
     of 10GBASE-T from this driver only. But it is not a very efficient 
     method */
  memset(&phy_sfx7101_hg_custom, 0, sizeof (phy_sfx7101_hg_custom));
  memcpy(&phy_sfx7101_hg_custom, &phy_xgxs5_hg, sizeof (phy_sfx7101_hg_custom));
  phy_sfx7101_hg_custom.pd_link_get   = phy_sfx7101_link_get;
  phy_sfx7101_hg_custom.pd_init       = phy_sfx7101_xgxs5_init;
  phy_sfx7101_hg_custom.pd_enable_set = phy_sfx7101_enable_set;
  soc_phy_add_entry(&phy10GBASET_custom_entry);
#endif
#endif /* #if defined(INCLUDE_PHY_8706)  */

#ifdef  BCM_XGS_SWITCH_SUPPORT
 /* The shadow table is needed only in hardware learning mode.
 */
 if (hpcSoftwareLearningEnabled () != L7_TRUE)
 {
   if (soc_feature(unit, soc_feature_arl_hashed)) {
     usec = soc_property_get(unit, spn_L2XMSG_THREAD_USEC, 3000000);
     SYSTEM_INIT_CHECK(soc_l2x_start(unit, 0, usec), "L2X thread init");
   }
 }
#endif  /* BCM_XGS_SWITCH_SUPPORT */

 /* PTin removed */
#if 0
  SYSTEM_INIT_CHECK(bcm_init(unit), "BCM driver layer init");
#endif

  if (soc_property_get_str(unit, spn_BCM_LINKSCAN_PBMP) == NULL)
  {
    SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
  }
  else
  {
    pbmp = soc_property_get_pbmp(unit, spn_BCM_LINKSCAN_PBMP, 0);
  }

  PBMP_ITER(pbmp, port)
  {
    SYSTEM_INIT_CHECK(bcm_linkscan_mode_set(unit, port,
                                            BCM_LINKSCAN_MODE_SW),
                      "Linkscan mode set");
    SYSTEM_INIT_CHECK(bcm_stat_clear(unit, port),
                      "Stat clear");
  }

  usec = soc_property_get(unit, spn_BCM_LINKSCAN_INTERVAL, 250000);

#ifdef L7_STACKING_PACKAGE
  /* Linkscan will be enabled at a later stage */
#else
  SYSTEM_INIT_CHECK(bcm_linkscan_enable_set(unit, usec), "Linkscan enable");
#endif

done:
  if (msg != NULL)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP,
               "system_init: %s failed: %s\n",
               msg, soc_errmsg(rv));
  }

  return BCM_E_NONE;
}

#ifndef _L7_OS_ECOS_
/* Outcall from SDK init code to load precompiled values defined in 
 * the function sal_config_init_defaults() which is automatically built 
 * under the SDK make system from uncommented values in $SDK/rc/config.bcm 
 * using bcm2c.pl script. This generates a source file called platform_defines.c.
 *
 * Just a stub to build OK. We don't use config.bcm to define defaults. For some
 * reason Linux/SAL code has this defined, but Vxworks/SAL code doesn't have this.
 */
void sal_config_init_defaults(void)
{
}

#endif

static int dapiTraceOverride_g = 0;
static int printingOverride_g = 1;

void hapiBroadDebugBcmTrace(int val)
{
  dapiTraceOverride_g = val;
}

void hapiBroadDebugBcmPrint(int val)
{
  printingOverride_g  = val;
}

/* PTin added: debug */
#if 1
int hapiBroadCmPrint_debug = L7_TRUE;

void hapiBroadCmPrint_enable(int enable)
{
  hapiBroadCmPrint_debug = enable;
}
#endif

#if (SDK_VERSION_IS >= SDK_VERSION(6,4,0,0))

uint32 bsl_layer_bmp = 0;
uint32 bsl_source_bmp[(bslSourceCount/32)+1];
uint8  bsl_severity_min = bslSeverityOff;

/**
 * Reset CM LOGGINGs allowed
 * 
 * @author mruas (12/30/2014)
 */
void hapiBroadCmReset(void)
{
  bsl_layer_bmp = 0;
  memset(bsl_source_bmp, 0, sizeof(bsl_source_bmp));
  bsl_severity_min = bslSeverityOff;
}

/**
 * Configure default CM loggings
 * 
 * @author mruas (12/30/2014)
 */
void hapiBroadCmDefaults(void)
{
  hapiBroadCmReset();

  hapiBroadCmLayerSet(bslLayerAppl, L7_TRUE);
  hapiBroadCmLayerSet(bslLayerBcm,  L7_TRUE);
  hapiBroadCmLayerSet(bslLayerBcmx, L7_TRUE);
  hapiBroadCmLayerSet(bslLayerSoc,  L7_TRUE);
  hapiBroadCmLayerSet(bslLayerSys,  L7_TRUE);

  hapiBroadCmSourceSet(bslSourceInit,   L7_TRUE);
  hapiBroadCmSourceSet(bslSourceShell,  L7_TRUE);
  hapiBroadCmSourceSet(bslSourceMii,    L7_TRUE);
  hapiBroadCmSourceSet(bslSourceMiim,   L7_TRUE);
  hapiBroadCmSourceSet(bslSourceMim,    L7_TRUE);
  hapiBroadCmSourceSet(bslSourcePhy,    L7_TRUE);
  hapiBroadCmSourceSet(bslSourcePhymod, L7_TRUE);
  hapiBroadCmSourceSet(bslSourceL2,     L7_TRUE);
  hapiBroadCmSourceSet(bslSourceL2gre,  L7_TRUE);
  hapiBroadCmSourceSet(bslSourceL2table,L7_TRUE);

  hapiBroadCmSeveritySet(bslSeverityInfo);
}

/**
 * Configure Layers to be logged
 * 
 * @author mruas (12/30/2014)
 * 
 * @param layer 
 * @param enable 
 */
void hapiBroadCmLayerSet(int layer, int enable)
{
  if (layer >= bslLayerCount)
  {
    return;
  }

  if (enable)
  {
    bsl_layer_bmp |= (1UL << layer);
  }
  else
  {
    bsl_layer_bmp &= ~(1UL << layer);
  }
}

/**
 * Configure sources to be logged
 * 
 * @author mruas (12/30/2014)
 * 
 * @param source 
 * @param enable 
 */
void hapiBroadCmSourceSet(int source, int enable)
{
  int div, mod;

  if (source >= bslSourceCount)
  {
    return;
  }

  div = source / (sizeof(uint32)*8);
  mod = source % (sizeof(uint32)*8);

  if (enable)
  {
    bsl_source_bmp[div] |= (1UL << mod);
  }
  else
  {
    bsl_source_bmp[div] &= ~(1UL << mod);
  }
}

/**
 * Define minimum severity to be logged
 * 
 * @author mruas (12/30/2014)
 * 
 * @param severity 
 */
void hapiBroadCmSeveritySet(int severity)
{
  if (severity >= bslSeverityCount)
  {
    return;
  }

  bsl_severity_min = severity;
}

/**
 * Callback for checking if logs should be printed
 * 
 * @author mruas (12/30/2014)
 * 
 * @param meta_pack 
 * 
 * @return int 
 */
int hapiBroadCmCheck(bsl_packed_meta_t meta_pack)
{
    int div, mod;
    int severity = BSL_SEVERITY_GET(meta_pack);
    int layer    = BSL_LAYER_GET(meta_pack);
    int source   = BSL_SOURCE_GET(meta_pack);

    /* Always TRUE */
    if (severity <= bslSeverityWarn /*|| layer == bslLayerAppl*/) {
        return 1;
    }

    /* Check configurable conditions */
    /* Layer */
    if ( !(bsl_layer_bmp & (1UL << layer)) )
    {
      return 0;
    }
    /* Source */
    div = source/(sizeof(uint32)*8);
    mod = source%(sizeof(uint32)*8);
    if ( !(bsl_source_bmp[div] & (1UL << mod)) )
    {
      return 0;
    }
    /* Severity */
    if (severity > bsl_severity_min)
    {
      return 0;
    }

    return 1;
}

/**
 * Callback for LOGs printing
 * 
 * @author mruas (12/30/2014)
 * 
 * @param meta_data 
 * @param format 
 * @param args 
 * 
 * @return int 
 */
int hapiBroadCmPrint(bsl_meta_t *meta_data, const char *format, va_list args)
{
  L7_LOG_SEVERITY_t sev = L7_LOG_SEVERITY_DEBUG;
  L7_BOOL   logit = L7_FALSE, printit = L7_FALSE;
  L7_uchar8 buf[LOG_MSG_MAX_MSG_SIZE];
  log_severity_t ptin_log_sev = LOG_SEV_PRINT;

  if( (meta_data == L7_NULLPTR) )
  {
    /* Always treat no meta data as an immediate print to the console */
    logit = L7_FALSE;
    printit = L7_TRUE;
  }
  else if (meta_data->layer == bslLayerAppl && meta_data->source == bslSourceShell)
  {
    /* Always print APPL layer to stdout */
    logit = L7_FALSE;
    printit = L7_TRUE;
  }
  else if (meta_data->severity <= bslSeverityFatal)
  {
    logit = L7_TRUE;
    printit = printingOverride_g;
    sev = L7_LOG_SEVERITY_EMERGENCY; 
    ptin_log_sev = LOG_SEV_FATAL;
  }
  else if (meta_data->severity <= bslSeverityError)
  {
    logit = L7_TRUE;
    printit = printingOverride_g;
    sev = L7_LOG_SEVERITY_ERROR; 
    ptin_log_sev = LOG_SEV_ERROR;
  }
  else if (meta_data->severity <= bslSeverityWarn)
  {
    logit = L7_TRUE;
    printit = printingOverride_g;
    sev = L7_LOG_SEVERITY_WARNING;
    ptin_log_sev = LOG_SEV_WARNING;
  }
  else if (bsl_check(meta_data->layer, meta_data->source, meta_data->severity, meta_data->unit))
  {
    logit = L7_TRUE;
    printit = printingOverride_g;

    if (meta_data->severity <= bslSeverityInfo)
    {
      sev = L7_LOG_SEVERITY_INFO;
      ptin_log_sev = LOG_SEV_INFO;
    }
    else if (meta_data->severity <= bslSeverityVerbose)
    {
      sev = L7_LOG_SEVERITY_INFO;
      ptin_log_sev = LOG_SEV_VERBOSE;
    }
    else
    {
      sev = L7_LOG_SEVERITY_DEBUG;
      ptin_log_sev = LOG_SEV_DEBUG;
    }
  }

  if (printit)
  {
    vprintf(format,args);
  }
  else if (logit && hapiBroadCmPrint_debug)   /* PTin modified: Logs */
  {
    /* 
     * only allow the write to happen to either syslog or dapiTrace 
     * in order to reduce time.
     */
    int rc = 0;

    rc = osapiVsnprintf(buf, sizeof (buf), format, args);
  
    if (rc <= 0)
    {
      return 0;  
    }
    else
    {  
      if ( rc >= sizeof(buf)) 
      {
        rc = sizeof(buf) - 1;
        /* make sure that the string is terminated */
        buf[rc] =  '\0';
      }

      /* get rid of new lines */
      if (buf[rc-1] == '\n') buf[rc-1] = '\0';

      if (dapiTraceOverride_g)
      {
        /* This will log it to the dapiTraceBuffer instead */
        dapiTraceGeneric(buf);
      }
      else
      {
        logger_print(LOG_CTX_SDK, ptin_log_sev, meta_data->file, meta_data->func, meta_data->line, "%s", buf);
      }
    }
  }

  return 0;
}

#else

int hapiBroadCmPrint(uint32 flags, const char *format, va_list args)
{
  L7_LOG_SEVERITY_t sev = L7_LOG_SEVERITY_DEBUG;
  L7_BOOL   logit = L7_FALSE, printit = L7_FALSE;
  L7_uchar8 buf[LOG_MSG_MAX_MSG_SIZE];
  log_severity_t ptin_log_sev = LOG_SEV_PRINT;

  if( (flags == 0) )
  {
    /* Always treat no flags as an immediate print to the console */
    /* Most of the drivshell functionality relies on 0 flags */
    printit = L7_TRUE;
  }
  else if (flags & DK_ERR)
  {
    logit = L7_TRUE;
    printit = printingOverride_g;
    sev = L7_LOG_SEVERITY_ERROR; 
    ptin_log_sev = LOG_SEV_ERROR;
  }
  else if (flags & DK_WARN)
  {
    logit = L7_TRUE;
    printit = printingOverride_g;
    sev = L7_LOG_SEVERITY_WARNING;
    ptin_log_sev = LOG_SEV_WARNING;
  }
  #if 0
  else if (flags & DK_PHY)
  {
    logit = L7_TRUE;
    printit = printingOverride_g;
    sev = L7_LOG_SEVERITY_INFO;
    ptin_log_sev = LOG_SEV_INFO;
  }
  #endif
  else
  {
    if (soc_cm_debug_check(flags))
    {
      logit = L7_TRUE;
      printit = printingOverride_g;

      sev = L7_LOG_SEVERITY_DEBUG;
      ptin_log_sev = LOG_SEV_DEBUG;
    }
  }

  if (printit)
  {
    vprintf(format,args);
  }
  else if (logit && hapiBroadCmPrint_debug)   /* PTin modified: Logs */
  {
    /* 
     * only allow the write to happen to either syslog or dapiTrace 
     * in order to reduce time.
     */
    int rc = 0;

    rc = osapiVsnprintf(buf, sizeof (buf), format, args);
  
    if (rc <= 0)
    {
      return 0;  
    }
    else
    {  
      if ( rc >= sizeof(buf)) 
      {
        rc = sizeof(buf) - 1;
        /* make sure that the string is terminated */
        buf[rc] =  '\0';
      }

      /* get rid of new lines */
      if (buf[rc-1] == '\n') buf[rc-1] = '\0';

      if (dapiTraceOverride_g)
      {
        /* This will log it to the dapiTraceBuffer instead */
        dapiTraceGeneric(buf);
      }
      else
      {
        //l7_logf(sev, L7_DRIVER_COMPONENT_ID, __FILE__, __LINE__, buf);
        logger_print(LOG_CTX_SDK, ptin_log_sev, NULL, NULL, 0, "(0x%08x) %s", flags, buf);
      }
    }
  }

  return 0;
}

#endif

