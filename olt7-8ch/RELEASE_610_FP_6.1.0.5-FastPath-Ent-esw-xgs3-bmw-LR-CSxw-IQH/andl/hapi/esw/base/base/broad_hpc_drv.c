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
#include "registry.h"
#include "log.h"
#include "unitmgr_api.h"
#include "l7_usl_api.h"
#include "cnfgr.h"
#include "hpc_hw_api.h"
#include "dtl_exports.h"

#include "broad_policy.h"
#include "broad_common.h"
#include "broad_utils.h"
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
#include "bcmx/filter.h"
#include "bcmx/l3.h"

#include "soc/debug.h"
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

static L7_int32  cpu_kbps_rate_limit_per_cos[] = { 512, 512, 512, 512, 512, 0, 0, 0 };

#if  (L7_MAX_CFG_QUEUES_PER_PORT  == 4)
static L7_int32  cpu_pps_rate_limit_per_cos[] = { 100, 100, 0, 0 };
#else
static L7_int32  cpu_pps_rate_limit_per_cos[] = { 1024, 1024, 1024, 1024, 1024, 0, 0, 0 };
#endif

static L7_int32 cpu_xgs2_sw_rate_limit_per_unit = 1500, cpu_raptor_sw_rate_limit_per_unit = 300;
static L7_int32 cpu_default_sw_rate_limit_per_unit = 0;

typedef enum {
  BROAD_CPU_KBPS_RATE_LIMIT = 0,
  BROAD_CPU_PPS_RATE_LIMIT
} BROAD_CPU_RATE_LIMIT_TYPE_t;

void hapiBroadCpuCosqRateSet(int unit, int cosq, int rate, BROAD_CPU_RATE_LIMIT_TYPE_t type);
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

/* Flag indicating that device doesn't have any active stack ports.
** We don't attempt to start discovery when there are no stack ports.
*/
L7_BOOL hpcBroadNoStackPorts = L7_FALSE;

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
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].inUse         = L7_FALSE;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmUnit       = -1;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmCpuUnit    = -1;
          hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].numOfBcmPorts = 0;

          memset(hapiBroadMapDb[l7UnitIndex].bcmCpuUnit[bcmCpuUnitIndex].bcmxLport,
                 -1,
                 HAPI_BROAD_MAX_PORTS_PER_CPU_UNIT * sizeof(bcmx_lport_t));

          hapiBroadMapDb[l7UnitIndex].numOfBcmCpuUnits--;

          if (hapiBroadMapDb[l7UnitIndex].numOfBcmCpuUnits == 0)
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
        if (hpcBroadNoStackPorts == L7_FALSE)
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

  if (~flags & BCMX_PORT_F_VALID)
  {
    return(BCMX_UPORT_INVALID_DEFAULT);
  }
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
void hapiBroadCpuCosqRateSet(int unit, int cosq, int rate, BROAD_CPU_RATE_LIMIT_TYPE_t type)
{
  int  rv = BCM_E_NONE;

  switch(type)
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
#ifdef BCM_TRIUMPH2_SUPPORT
 // PTin added: SOC_IS_VALKYRIE2(unit)
 if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) {
   extern int bcm_tr2_cosq_port_pps_set(int unit, bcm_port_t port,
				       bcm_cos_queue_t cosq, int pps);   
   rv = bcm_tr2_cosq_port_pps_set(unit, CMIC_PORT(unit), cosq, rate);
 } else
#endif
 if (SOC_IS_TR_VL(unit)) {
        rv = _bcm_tr_cosq_port_packet_bandwidth_set(unit,CMIC_PORT(unit), cosq, rate, rate);
     } else {
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

  if (rv != BCM_E_NONE)
  {
      LOG_MSG("In %s:%d, could not set rate limit to %d for cpu port cos %d                                                                               rv %d\n",
            __FUNCTION__, __LINE__, rate, cosq, rv);
  }

  return;
}

/*********************************************************************
*
* @purpose Enable various switch control.
*
* @param   none
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
  // PTin removed
  #if 0
  rv = bcm_switch_control_set(i, bcmSwitchV4L3DstMissToCpu, 1);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchV6L3DstMissToCpu, 1);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* Enable L3 exception to CPU. */
  rv = bcm_switch_control_set(i, bcmSwitchV4L3ErrToCpu, 1);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchV6L3ErrToCpu, 1);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  /* Enabled IP packets with TTL=1 and TTL=0 */
  rv = bcm_switch_control_set(i, bcmSwitchL3SlowpathToCpu, 1);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }

  rv = bcm_switch_control_set(i, bcmSwitchL3UcTtlErrToCpu, 1);
  if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
  {
    return L7_FAILURE;
  }
  #endif

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
  }
  else 
  {
    /* For XGS2 devices, set L3DstMiss and L3Err packets to CPU. 
     * These controls are supported on XGS3 too [compatibility], but
     * could be removed for XGS3 in future.
     */
    // PTin removed
    #if 0
    rv = bcm_switch_control_set(i, bcmSwitchUnknownL3DestToCpu, 1);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
      return L7_FAILURE;

    rv = bcm_switch_control_set(i, bcmSwitchL3HeaderErrToCpu, 1);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
      return L7_FAILURE;
    #endif
  }

  /* Enable source port mismatch to CPU. This was a FIXUP before */
  // PTin removed
  #if 0
  rv = bcm_switch_control_set(i, bcmSwitchIpmcPortMissToCpu, 1);
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

  // PTin added: SOC_IS_VALKYRIE2(unit)
  if (SOC_IS_APOLLO(i) || SOC_IS_TRIUMPH2(i) || SOC_IS_VALKYRIE2(unit)) /* double check that TR2 should be included */
  {
    rv = bcm_switch_control_set(i, bcmSwitchIpmcSameVlanPruning, 0); /* bit is inversed */
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      return L7_FAILURE;
    }
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
        LOG_MSG("hpcHardwareBlockMaskSet: could not set block mask %d\n", rv);
      }
    }
  }
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
  L7_int32 *tmp_cpu_kbps_rate_limit_per_cos; 
  int tmp_num_queues = 0;
  L7_uint32       schedulerMode;


#ifdef L7_STACKING_PACKAGE
  /* change the vlan on which stacking traffic runs */
  (void)atp_cos_vlan_set(HPC_STACKING_COS, HPC_STACKING_VLAN_ID);
  (void)next_hop_cos_set(HPC_STACKING_COS);
  (void)next_hop_vlan_set(HPC_STACKING_VLAN_ID);
  (void)disc_cos_set (HPC_STACKING_COS);
  (void)disc_vlan_set (HPC_STACKING_VLAN_ID);
#endif

  /* change the vlan on which stacking traffic runs */
#ifdef L7_ROBO_SUPPORT
  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
#else
  for (i = 0; i < bde->num_devices(BDE_ALL_DEVICES); i++)
#endif
  {
#ifdef L7_STACKING_PACKAGE
    /* HPC keeps track of link status in order to report stacking port
    ** link changes to the Stack Port Manager.
    */
    rv = bcm_linkscan_register (i,hpcStackPortLinkStatusCallback);
    if (rv != BCM_E_NONE)
    {
      LOG_ERROR (i);
    }
#endif
    rv = bcm_custom_register(i, custom_bcmx_port_handler);
     if (rv != BCM_E_NONE)
     {
       LOG_ERROR (rv);
     }

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
        // PTin added: SOC_IS_VALKYRIE2(unit)
        if (SOC_IS_FB_FX_HX(i) || SOC_IS_TR_VL(i) || SOC_IS_TRIUMPH2(i) || SOC_IS_APOLLO(i) || SOC_IS_VALKYRIE2(i))
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

        rv = bcm_cosq_mapping_set(i, priority, cosq);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_PARAM))
        {
          LOG_ERROR (rv);
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
       rv = bcm_cosq_sched_set (i, 
                                schedulerMode,
                                wrr_default_weights, 
                                0);
       if (rv != BCM_E_NONE) 
       {
         LOG_ERROR (rv);
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
      LOG_ERROR (rv);
    }

    /* Tucana and XGS3 devices support H/W Rate limiting */
    /* For devices that support packet rate limit feature, like Triumph, the
     * rate limits are set below
     */
    if ((SOC_IS_FB_FX_HX(i)) || (SOC_IS_TUCANA(i)) || (SOC_IS_BRADLEY(i)) || 
        (SOC_IS_RAVEN(i)))
    {
      int cos_queue = 0;

      if (!SOC_IS_RAPTOR(i)  && !SOC_IS_HAWKEYE(i)) 
      {
        tmp_cpu_kbps_rate_limit_per_cos = cpu_kbps_rate_limit_per_cos;
        tmp_num_queues = sizeof(cpu_kbps_rate_limit_per_cos) / sizeof(L7_int32);
          
        /* Set the hardware rate-limit of cpu port queues */
        for (cos_queue = 0; cos_queue < tmp_num_queues; cos_queue++)
        {
          if (tmp_cpu_kbps_rate_limit_per_cos[cos_queue] > 0)
          {
            hapiBroadCpuCosqRateSet(i,
                                    cos_queue,tmp_cpu_kbps_rate_limit_per_cos[cos_queue],
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
    if (SOC_IS_HELIX(i) || soc_feature(i, soc_feature_packet_rate_limit))
    {
      int cos_queue = 0;

      for (cos_queue = 0; cos_queue < (sizeof(cpu_pps_rate_limit_per_cos) / sizeof(L7_int32)); cos_queue++)
      {
        if (cpu_pps_rate_limit_per_cos[cos_queue] > 0)
          hapiBroadCpuCosqRateSet(i,
                                  cos_queue,cpu_pps_rate_limit_per_cos[cos_queue],
                                  BROAD_CPU_PPS_RATE_LIMIT);
      }
    }

    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_create(i,HPC_STACKING_VLAN_ID);
    if ((rv < 0) && (rv != BCM_E_EXISTS))
    {
      printk("bcm_vlan_create failed unit %d\n", i);
      LOG_ERROR (rv);
    }

    rv = bcm_vlan_port_add(i,HPC_STACKING_VLAN_ID,PBMP_CMIC(i),ubmp);
    if (rv < 0)
    {
      printk("bcm_vlan_port_add failed unit %d\n", i);
      LOG_ERROR (rv);
    }

    PBMP_E_ITER (i, port)
    {
      if (!SOC_IS_XGS_FABRIC(i))
      {
        if (hpcIsBcmPortStacking (i, port) == L7_FALSE)
        {
          rv = bcm_port_enable_set (i,port , 0);
          if (rv != BCM_E_NONE)
          {
            LOG_ERROR (rv);
          }

          /* Disable learning on the ports. The port security component sets
          ** up learning.
          */
#ifndef L7_ROBO_SUPPORT
#ifdef L7_MACLOCK_PACKAGE
          rv = bcm_port_learn_set (i,port,  0);
#else 
          /* If PML component is not present, we have to enable the learning mode on all ports */
          rv = bcm_port_learn_set (i,port, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
#endif
#endif
          if (rv != BCM_E_NONE)
          {
            LOG_ERROR (rv);
          }

          rv = bcm_port_pfm_set(i, port, BCM_PORT_PFM_MODEC);     // PTin modified
          if (L7_BCMX_OK(rv) != L7_TRUE && rv != BCM_E_UNAVAIL)
          {
            LOG_ERROR (rv);
          }

          /* Set the spanning-tree state for each front-panel port to
          ** disable as bcm_clear sets all the port states to forwarding
          ** in bcm_esw_auth_init 
          */
          rv = bcm_port_stp_set(i, port, BCM_STG_STP_DISABLE);
          if (L7_BCMX_OK(rv) != L7_TRUE && rv != BCM_E_UNAVAIL)
          {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
                    "Failed to set port %d/%d state to disable rv %d\n",
                    i, port, rv);
          }

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
            LOG_ERROR (rv);
          }
          }

          if (SOC_IS_FIREBOLT2(i) || SOC_IS_RAPTOR(i) || SOC_IS_RAVEN(i)) 
          {
             /* Set priority of ARP sent to CPU - applies to ARP replies only */ 
             rv = bcm_switch_control_set(i, bcmSwitchCpuProtoArpPriority, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
             if (rv != BCM_E_NONE)
             {
                LOG_ERROR (rv);
             }
          }
          /* Send L2 static station movement packets to cpu */
          // PTin removed
          #if 0
          rv = bcm_switch_control_set (i, bcmSwitchL2StaticMoveToCpu, 1);
          #endif
        }
        else if (SOC_IS_BRADLEY(i))
        {
          rv = bcm_switch_control_set (i, bcmSwitchCpuProtoBpduPriority, HAPI_BROAD_INGRESS_BPDU_COS);
        }
        // PTin added: SOC_IS_VALKYRIE2(unit)
        else if (SOC_IS_TR_VL(i) || SOC_IS_SCORPION(i) || SOC_IS_TRIUMPH2(i) || SOC_IS_APOLLO(i) || SOC_IS_VALKYRIE2(i))
        {
          bcm_rx_reasons_t reason, no_reason;
          int              internal_priority;

          index = 0;

          BCM_RX_REASON_CLEAR_ALL(reason);
          BCM_RX_REASON_SET(reason, bcmRxReasonL3HeaderError);
          rv = bcm_rx_cosq_mapping_set(i, index, reason, reason, 0, 0, 0, 0, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            LOG_ERROR (rv);
          }
          index++;

          BCM_RX_REASON_CLEAR_ALL(reason);
          BCM_RX_REASON_SET(reason, bcmRxReasonL3Slowpath);
          rv = bcm_rx_cosq_mapping_set(i, index, reason, reason, 0, 0, 0, 0, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            LOG_ERROR (rv);
          }
          index++;

          BCM_RX_REASON_CLEAR_ALL(reason);
          BCM_RX_REASON_SET(reason, bcmRxReasonTtl1);
          rv = bcm_rx_cosq_mapping_set(i, index, reason, reason, 0, 0, 0, 0, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            LOG_ERROR (rv);
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
            LOG_ERROR (rv);
          }
          index++;

          /* Set up a one to one mapping of internal priority to CPU cosq. */
          BCM_RX_REASON_CLEAR_ALL(no_reason);
          for (internal_priority = 0; internal_priority < 8; internal_priority++)
          {
            rv = bcm_rx_cosq_mapping_set(i, index, no_reason, no_reason, internal_priority, 0x07, 0, 0, internal_priority);
            if (rv != BCM_E_NONE)
            {
              LOG_ERROR (rv);
            }
            index++;
          }

          if (SOC_IS_SCORPION(i))
          {
            rv = bcm_linkscan_register(i, hapiBroadMmuLinkscanCb);
            if (rv != BCM_E_NONE)
            {
              LOG_ERROR(rv);
            }
          }
        }
        else
        {
          rv = bcm_switch_control_set (i, bcmSwitchCpuProtocolPrio, HAPI_BROAD_INGRESS_BPDU_COS);
        }

        if (rv != BCM_E_NONE)
        {
          LOG_ERROR (rv);
        }
        // PTin added: SOC_IS_VALKYRIE2(unit)
        if (!SOC_IS_TR_VL(i) && !SOC_IS_SCORPION(i) && !SOC_IS_TRIUMPH2(i) && !SOC_IS_APOLLO(i) && !SOC_IS_VALKYRIE2(i))
        {
          /* This priority is used for packets that are copied to the CPU with a classifier, 
          ** and for IP traffic destined to the CPU due to IP address in the frames or
          ** IP header errors.
          */
          rv = bcm_switch_control_set (i, bcmSwitchCpuDefaultPrio, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
          if (rv != BCM_E_NONE)
          {
            LOG_ERROR (rv);
          }
        }
    } 

    /* Set the MAC/L3DST lookup failure packet priority. */
    (void)bcm_switch_control_set(i, bcmSwitchCpuUnknownPrio, 0);

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
            LOG_ERROR (rv);
          }
        }
        // PTin added: SOC_IS_VALKYRIE2(unit)
        if (!SOC_IS_TR_VL(i) && !SOC_IS_SCORPION(i) && !SOC_IS_TRIUMPH2(i) && !SOC_IS_APOLLO(i) && !SOC_IS_VALKYRIE2(i))
        {
          /* Send unknown SA frames to the CPU with priority 0.
          */
          rv = bcm_switch_control_set (i, bcmSwitchCpuUnknownPrio, 0);
          if (rv != BCM_E_NONE)
          {
            LOG_ERROR (rv);
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
          LOG_ERROR (rv);
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
          LOG_ERROR (rv);
        }
      }
    }
  
    /* Set priorities for the packet sent to CPU on Bradley.
     * It has got one more CPU control register.
     */
    if (SOC_IS_BRADLEY(i))
    {  
      /* Send L2 static station movement packets to cpu */
      // PTin removed
      #if 0
      rv = bcm_switch_control_set (i, bcmSwitchL2StaticMoveToCpu, 1);
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (rv);
      }
      #endif
      /* Set priority of ARP sent to CPU */ 
      rv = bcm_switch_control_set(i, bcmSwitchCpuProtoArpPriority, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (rv);
      }
      /* Set prioirty of IPv4/IPv6 multicast reserved packets sent to CPU.
       * Dont know yet which to send. Sending it to a low priority queue 
       */
      rv = bcm_switch_control_set(i, bcmSwitchCpuProtoIpmcReservedPriority, HAPI_BROAD_INGRESS_LOWEST_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (rv);
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
       if (rv != BCM_E_NONE)
       {
          LOG_ERROR (rv);
       }

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
          LOG_ERROR (rv);
       }

       if (soc_feature(i, soc_feature_l3))
       {
       rv = bcm_switch_control_get(i, bcmSwitchHashL3, &hashControl);
       if (rv != BCM_E_NONE)
       {
          LOG_ERROR (rv);
       }

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
          LOG_ERROR (rv);
       }
    }
    }

    if (SOC_IS_RAPTOR(i) || SOC_IS_HAWKEYE(i)) 
    {
      rv = bcm_switch_control_set(i, bcmSwitchCpuProtoIgmpPriority, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (i);
      }

      rv = bcm_switch_control_set(i, bcmSwitchCpuProtoIpmcReservedPriority, HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (i);
      }

      rv = bcm_switch_control_set(i,bcmSwitchCpuProtoDhcpPriority,HAPI_BROAD_INGRESS_MED_PRIORITY_COS);
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (i);
      }

      // PTin removed
      #if 0
      rv = bcm_switch_control_set(i , bcmSwitchDhcpPktToCpu, 1 );
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (i);
      }


      rv = bcm_switch_control_set(i, bcmSwitchNdPktToCpu, 1);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        LOG_ERROR (rv);
      }

      rv = bcm_switch_control_set(i, bcmSwitchV4ResvdMcPktToCpu, 1);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        LOG_ERROR (rv);
      }
      rv = bcm_switch_control_set(i, bcmSwitchV6ResvdMcPktToCpu, 1);
      if (L7_BCMX_OK(rv) != L7_TRUE)
      {
        LOG_ERROR (rv);
      }
      #endif

      /* For Hawkeye, the hardware default value is 0, so enable it */
      if( SOC_IS_HAWKEYE(i) )
      {
        rv = bcm_switch_control_set(i,bcmSwitchIgmpReservedMcastEnable, 1);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          LOG_ERROR (rv);
        }
        rv = bcm_switch_control_set(i,bcmSwitchMldReservedMcastEnable, 1);
        if (L7_BCMX_OK(rv) != L7_TRUE)
        {
          LOG_ERROR (rv);
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
      LOG_ERROR(rv);
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
      (void)bcm_switch_control_set(i, bcmSwitchModuleType,
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
      LOG_ERROR(i);
    }

    /* Modify flood mask to cpu for various traffic */
    hpcHardwareBlockMaskSet(i);

#ifdef L7_ROUTING_PACKAGE
    rv = bcm_ipmc_enable(i, L7_FALSE);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      LOG_ERROR (rv);
    }
#endif

#ifdef INCLUDE_L3
    /* keep port l3 bits on all the time */
    rv = bcm_l3_enable_set(i, L7_TRUE);
    if ((L7_BCMX_OK(rv) != L7_TRUE) && (rv != BCM_E_UNAVAIL))
    {
      LOG_ERROR (rv);
    }
#endif

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
    // PTin removed
    #if 0
    rv = bcm_switch_control_set(unit, bcmSwitchArpReplyToCpu, 1);
    if (rv != BCM_E_NONE)
    {
      LOG_ERROR(rv);
    }

    rv = bcm_switch_control_set(unit, bcmSwitchDhcpPktToCpu, 1);
    if (rv != BCM_E_NONE)
    {
      LOG_ERROR(rv);
    }
    #endif

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
* @purpose  Resets the Driver routing tables.
*
* @param    void
*                                       
* @returns  L7_SUCCESS successfully reset the driver
* @returns  L7_FAILURE problem occurred while resetting the driver
*
* @comments This function is used by stack units  to clean up
*           the hardware state before accepting new configuration. 
*           due to the way HPC RPC mechanism works, some route and IPMC
*           entries may still be in the hardware after clear-config.
*              
*       
* @end
*********************************************************************/
L7_RC_t hpcHardwareTablesReset (void)
{
  L7_uint32 i;

  for (i = 0; i < soc_ndev; i++)
  {
#ifdef L7_ROUTING_PACKAGE
    (void)bcm_l3_init (i);

     /* L3 in re'inited, set egress mode and ECMP path */
     if(bcm_switch_control_set(i, bcmSwitchL3EgressMode, 1) != BCM_E_NONE)
     {
       LOG_ERROR(0);
     }

     hapiBroadL3MaxEcmpPathsSet(i);
#endif
#ifdef L7_MCAST_PACKAGE
    (void)bcm_ipmc_init (i);
#endif
  }

  return L7_SUCCESS;
}

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
            LOG_ERROR (rv);
          }

          /* Disable learning on the ports. The port security component sets
          ** up learning.
          */
          rv = bcm_port_learn_set (i,port,  0);
          if (rv != BCM_E_NONE)
          {
            LOG_ERROR (rv);
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
        LOG_ERROR (rv);
      }

#if 0
    /* Broadcom recommends not clearing the COS table for the 5670. This table should not 
    ** change during normal operation. Clearing this table may cause the DMA hang on the 5670
    ** for test cases that involve joining two running stacks.
    */
      rv = bcm_cosq_init (i);
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (rv);
      }
#endif

      rv = bcm_mcast_init (i);
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (rv);
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
        LOG_ERROR (rv);
      }
    }
  }

  usl_mac_table_sync_resume();

  /* reset all the USL databases in the driver */
  usl_database_invalidate();

  hpcHardwareDefaultConfigApply ();

  hapiBroadPolicyReInit();   /* call prior to re-adding any policies */

#ifdef L7_STACKING_PACKAGE
  hpcStackPortEnable (L7_TRUE);
  hpcHardwareManagerNeighListReset();
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
   LOG_ERROR(rv);
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
   LOG_ERROR (rv);
 }

 bcmx_lplist_free(&portList);
}

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
        if ((sysapiHpcCardInfoPtr->portInfo[portNo].type == L7_IANA_10G_ETHERNET) &&
            (sysapiHpcCardInfoPtr->portInfo[portNo].connectorType == L7_XAUI))
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

L7_RC_t hpcBroadInit()
{
  L7_uint32                    total_bcom_units, bcom_unit;
  const bcm_sys_board_t       *board_info;
  int                          rc;

  hpcConfigSet();

  total_bcom_units = bde->num_devices(BDE_ALL_DEVICES);

  for (bcom_unit=0; bcom_unit<total_bcom_units; bcom_unit++)
  {
    sysconf_attach(bcom_unit);
  }

  hpcConfigPhySet();

#ifdef L7_ROBO_SUPPORT
  for (bcom_unit=0; bcom_unit< bde->num_devices(BDE_SWITCH_DEVICES) ; bcom_unit++)
#else
  for (bcom_unit=0; bcom_unit<total_bcom_units; bcom_unit++)
#endif
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
    if (hapiBroadMmuConfigModify(bcom_unit) != L7_SUCCESS) /* add cells to CPU COS 4-7 for bursts */
    {
      return (L7_FAILURE);
    }
    
    (void)bcm_switch_control_set(bcom_unit,bcmSwitchL2McastAllRouterPortsAddEnable,0);
  }
  
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
    printk("Could not create port mapping database\n");
    return(L7_FAILURE);
  }

  /* normally done as part of hpcBroadStackInit */
  if (bcmx_uport_create_callback_set(lvl7_uport_create_callback) != BCM_E_NONE)
  {
    printk("Could not register bcmx_uport_create_callback\n");
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
    if (soc_property_port_get(unit, port, spn_CX4_TO_HG, FALSE)) {
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

#define SYSTEM_INIT_CHECK(action, description)                  \
        if ((rv = (action)) < 0) {                              \
            msg = (description);                                \
            goto done;                                          \
        }

int
systemInit(int unit)
{
  soc_port_t            port;
  int                   rv;
  sal_usecs_t           usec;
  char          *msg = NULL;
  pbmp_t                pbmp;
#ifdef BCM_ROBO_SUPPORT
extern int soc_robo_misc_init(int ); 
extern int soc_robo_mmu_init(int );
#endif

#ifdef BCM_ROBO_SUPPORT
  SYSTEM_INIT_CHECK(soc_robo_reset_init(unit), "Device reset");
  SYSTEM_INIT_CHECK(soc_robo_misc_init(unit), "Misc init");
  SYSTEM_INIT_CHECK(soc_robo_mmu_init(unit), "MMU init");
#else
  SYSTEM_INIT_CHECK(soc_reset_init(unit), "Device reset");
  SYSTEM_INIT_CHECK(soc_misc_init(unit), "Misc init");
  SYSTEM_INIT_CHECK(soc_mmu_init(unit), "MMU init");
#endif

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

  SYSTEM_INIT_CHECK(bcm_init(unit), "BCM driver layer init");

  if (soc_property_get_str(unit, spn_BCM_LINKSCAN_PBMP) == NULL) {
  SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
  } else {
  pbmp = soc_property_get_pbmp(unit, spn_BCM_LINKSCAN_PBMP, 0);
  }

  PBMP_ITER(pbmp, port) {

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
  if (msg != NULL) {
  soc_cm_debug(DK_ERR,
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

/*
 * Function:
 *      bcmx_bcm_unit_to_modid
 * Purpose:
 *      Map a bcm_unit to its mod id, if appropriate
 * Parameters:
 *      lport        - The bcm_unit to lookup
 *      modid        - (OUT) Where to put the modid
 * Returns:
 *      BCM_E_XXX
 */

int
hapiBroadBcmUnitToModid(int bcm_unit, int *modid)
{
  bcmx_lport_t lport;
  int unit;
  bcm_port_t port;

  BCMX_FOREACH_LPORT(lport) {
    bcmx_lport_to_unit_port(lport, &unit, &port);
    if (unit == bcm_unit) {
      bcmx_lport_to_modid_port(lport, modid, &port);
      return BCM_E_NONE;
    }
  }

  return BCM_E_NOT_FOUND;
}

static int dapiTraceOverride_g = 0;
static int printingOverride_g = 0;

void hapiBroadDebugBcmTrace(int val)
{
  dapiTraceOverride_g = val;
}

void hapiBroadDebugBcmPrint(int val)
{
  printingOverride_g  = val;
}

int hapiBroadCmPrint(uint32 flags, const char *format, va_list args)
{
  L7_LOG_SEVERITY_t sev = L7_LOG_SEVERITY_DEBUG;
  L7_BOOL   logit = L7_FALSE, printit = L7_FALSE;
  L7_uchar8 buf[LOG_MSG_MAX_MSG_SIZE]; 

  if (flags == 0)
  {
    /* Always treat no flags as an immediate print to the console */
    /* Most of the drivshell functionality relies on 0 flags */
    printit = L7_TRUE;
  }
  else if (flags == ~ (DK_ERR | DK_WARN))
  {
    logit = L7_TRUE;
    printit = printingOverride_g;
    sev = L7_LOG_SEVERITY_DEBUG; 
  }
  else
  {
    if (soc_cm_debug_check(flags))   
    {
      logit = L7_TRUE;
      printit = printingOverride_g;

      if (flags & DK_ERR) 
      {
        sev = L7_LOG_SEVERITY_ERROR ;
      }
      else if (flags & DK_WARN) 
      {
        sev = L7_LOG_SEVERITY_WARNING;
      }
      else 
      { 
        sev = L7_LOG_SEVERITY_DEBUG; 
      }
    }
  }

  if (printit)
  {
    vprintf(format,args);
  }
  else if (logit)
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
        l7_logf(sev, L7_DRIVER_COMPONENT_ID, __FILE__, __LINE__, buf);
      }
    }
  }

  return 0;
}
