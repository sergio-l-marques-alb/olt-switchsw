/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy_bcmx.c
*
* This file implements the custom bcmx layer that runs on the master. 
*
* @component hapi
*
* @create    3/18/2005
*
* @author    robp
*
* @end
*
**********************************************************************/

#include "broad_policy_bcmx.h"
#include "bcmx/custom.h"
#include "bcmx/bcmx_int.h"
#include "zlib.h"
#include "broad_utils.h"

/* size of bcm rpc buffer in bytes */
#define BCM_CUSTOM_BYTES_MAX (BCM_CUSTOM_ARGS_MAX*sizeof(uint32))
#define PAYLOAD_BYTES_MAX (BCM_CUSTOM_BYTES_MAX - (sizeof(BROAD_POLICY_CUSTOM_DATA_t) - sizeof(BROAD_POLICY_ENTRY_t)))

/* zlib algorithm requires larger dest buffer larger than source buffer */
#define COMPRESS_BUFFER_BYTES (sizeof(BROAD_POLICY_CUSTOM_DATA_t)*2)

/* use compression by default as it saves messaging overhead */
#define BROAD_CUSTOMX_POLICY_COMPRESSED

#ifdef BROAD_CUSTOMX_POLICY_COMPRESSED
static char *comp_buffer = L7_NULLPTR;

static int _policy_compress_payload(BROAD_POLICY_ENTRY_t *src)
{
    int    err;
    uLongf dstSize;

    if (comp_buffer == L7_NULLPTR)
    {
      comp_buffer = osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(char)*COMPRESS_BUFFER_BYTES);
      if (comp_buffer == L7_NULL) {
          return 0;
      }
    }

    dstSize = COMPRESS_BUFFER_BYTES;

    err = compress((Bytef *)comp_buffer, &dstSize, (Bytef *)src, sizeof(BROAD_POLICY_ENTRY_t));
    if (Z_OK != err)
        return BCM_E_FAIL;

    return dstSize;
}
#endif

int customx_port_policy_create(BROAD_POLICY_t        policyId,
                               BROAD_POLICY_ENTRY_t *policyEntry,
                               bcmx_lport_t          port)
{
    int                         rv = BCM_E_NONE;
    int                         payloadSize;
    uint8                      *payloadPtr;
    uint32                      args[BCM_CUSTOM_ARGS_MAX];
    BROAD_POLICY_CUSTOM_DATA_t *pData;

    pData = (BROAD_POLICY_CUSTOM_DATA_t *)args;

    pData->policyCmd   = BROAD_CUSTOM_POLICY_CREATE;
    pData->policyId    = policyId;
    pData->policyFlags = BROAD_POLICY_FIRST;

#ifdef BROAD_CUSTOMX_POLICY_COMPRESSED
    /* check if compression is required (optional) */
    if (sizeof(BROAD_POLICY_ENTRY_t) > PAYLOAD_BYTES_MAX)
    {
        payloadSize = _policy_compress_payload(policyEntry);
        payloadPtr  = comp_buffer;
        
        if (payloadSize > 0)
            pData->policyFlags |= BROAD_POLICY_COMPRESSED;
        else
            rv = BCM_E_FAIL;
    }
    else
#endif
    {
        payloadSize = sizeof(BROAD_POLICY_ENTRY_t);
        payloadPtr  = (uint8*)policyEntry;

        pData->policyFlags &= ~BROAD_POLICY_COMPRESSED;
    }

    while ((payloadSize > 0) && (BCM_E_NONE == rv))
    {
        /* update policy header and copy data into message */        
        if (payloadSize <= PAYLOAD_BYTES_MAX)
        {
            pData->policySize   = payloadSize;
            pData->policyFlags |= BROAD_POLICY_LAST;
        }
        else
        {
            pData->policySize   = PAYLOAD_BYTES_MAX;
        }

        memcpy(&pData->cmdData, payloadPtr, pData->policySize);

        rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_POLICY_SET_HANDLER, args);
        if (L7_BCMX_OK(rv) == L7_TRUE)
            rv = BCM_E_NONE;

        /* Packet has been sent - next one won't be the first */
        pData->policyFlags &= ~BROAD_POLICY_FIRST;

        payloadSize -= pData->policySize;
        payloadPtr  += pData->policySize;
    }

    return rv;
}

int customx_port_policy_destroy(BROAD_POLICY_t policyId,
                                bcmx_lport_t   port)
{
    int                         rv;
    uint32                      args[BCM_CUSTOM_ARGS_MAX];
    BROAD_POLICY_CUSTOM_DATA_t *pData;

    pData = (BROAD_POLICY_CUSTOM_DATA_t *)args;
    pData->policyCmd   = BROAD_CUSTOM_POLICY_DESTROY;
    pData->policyFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
    pData->policyId    = policyId;
    pData->policySize  = 0;

    rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_POLICY_SET_HANDLER, args);
    if (L7_BCMX_OK(rv) == L7_TRUE)
        rv = BCM_E_NONE;

    return rv;
}

int customx_port_policy_check(BROAD_POLICY_t policyId,
                              bcmx_lport_t   port)
{
    int                         rv;
    uint32                      args[BCM_CUSTOM_ARGS_MAX];
    BROAD_POLICY_CUSTOM_DATA_t *pData;

    pData = (BROAD_POLICY_CUSTOM_DATA_t *)args;
    pData->policyCmd   = BROAD_CUSTOM_POLICY_CHECK;
    pData->policyFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
    pData->policyId    = policyId;
    pData->policySize  = 0;

    rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_POLICY_SET_HANDLER, args);

    return rv;
}

int customx_port_policy_apply(BROAD_POLICY_t policyId,
                              bcmx_lport_t   port)
{
    int                         rv;
    uint32                      args[BCM_CUSTOM_ARGS_MAX];
    BROAD_POLICY_CUSTOM_DATA_t *pData;

    pData = (BROAD_POLICY_CUSTOM_DATA_t *)args;
    pData->policyCmd   = BROAD_CUSTOM_POLICY_APPLY;
    pData->policyFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
    pData->policyId    = policyId;
    pData->policySize  = 0;

    rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_POLICY_SET_HANDLER, args);

    return rv;
}

int customx_port_policy_remove(BROAD_POLICY_t policyId,
                               bcmx_lport_t   port)
{
    int                         rv;
    uint32                      args[BCM_CUSTOM_ARGS_MAX];
    BROAD_POLICY_CUSTOM_DATA_t *pData;

    pData = (BROAD_POLICY_CUSTOM_DATA_t *)args;
    pData->policyCmd   = BROAD_CUSTOM_POLICY_REMOVE;
    pData->policyFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
    pData->policyId    = policyId;
    pData->policySize  = 0;

    rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_POLICY_SET_HANDLER, args);
    if (L7_BCMX_OK(rv) == L7_TRUE)
        rv = BCM_E_NONE;

    return rv;
}

static void _accumulate_stats(L7_uint32 numStats, BROAD_POLICY_STATS_t *stats, uint32 *args)
{
    int       i;
    int       index;
    L7_uint64 tmpVal;

    for (i = 0; i < numStats; i++)
    {         
        index = i * 4;

        /* copy returned values (args) into stats structure */
        if (L7_TRUE == stats[i].meter)
        {
            /* accumulate out-of-profile */
            COMPILER_64_SET(tmpVal, args[index+0], args[index+1]);
            COMPILER_64_ADD_64(stats[i].statMode.meter.out_prof, tmpVal);
            /* accumulate in-profile */
            COMPILER_64_SET(tmpVal, args[index+2], args[index+3]);
            COMPILER_64_ADD_64(stats[i].statMode.meter.in_prof, tmpVal);
        }
        else
        {
            /* accumulate counter */
            COMPILER_64_SET(tmpVal, args[index+2], args[index+3]);
            COMPILER_64_ADD_64(stats[i].statMode.counter.count, tmpVal);
        }
    }
}

int customx_policy_stats_get(BROAD_POLICY_t        policyId,
                             L7_uint32             numStats,
                             BROAD_POLICY_STATS_t *stats)
{
    int     i, bcm_unit, rv;
    uint32  args[BCM_CUSTOM_ARGS_MAX];

    BCMX_UNIT_ITER(bcm_unit, i)
    {
        if( (SOC_UNIT_VALID(bcm_unit))&& (SOC_IS_XGS_FABRIC(bcm_unit)) )
	    {
              continue;
        }
	
        /* get function can only specify type - note replacement of port with policyId */
        rv = bcm_custom_port_get(bcm_unit, policyId, HAPI_BROAD_BCMX_POLICY_GET_HANDLER, args);
        if (BCM_E_NONE == rv)
        {
            _accumulate_stats(numStats, stats, args);
        }
    }

    return BCM_E_NONE;
}

