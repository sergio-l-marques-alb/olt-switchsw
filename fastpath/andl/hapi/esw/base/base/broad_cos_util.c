/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  broad_cos_util.c
*
* @purpose   This file contains utility functions to manage COS policies.
*
* @component hapi-broad
*
* @comments
*
* @create    12/04/08
*
* @author    colinw
*
* @end
*
**********************************************************************/
#include "broad_common.h"
#include "broad_policy.h"
#include "broad_cos_util.h"
#include "zlib.h"

typedef struct
{
    L7_uchar8      used;
    L7_ushort16    ifCount;
    L7_uint32      hashVal;
    L7_uint32      hashSize;
    BROAD_POLICY_t policyId;
}
BROAD_COS_POLICY_UTIL_ENTRY_t;

static BROAD_COS_POLICY_UTIL_ENTRY_t *broadCosUtilTable = L7_NULL;

static L7_uint32 hapiBroadCosPolicyUtilHash(L7_uchar8 *data, L7_uint32 size)
{
    /* Skip instanceId as it can vary and jump right to the rules. */
    return crc32(0L, data, size);
}

L7_RC_t hapiBroadCosPolicyUtilInit()
{
    int i;

    if (broadCosUtilTable == L7_NULL)
    {
      /* There is a maximum of one COS policy per port. */
      broadCosUtilTable = (BROAD_COS_POLICY_UTIL_ENTRY_t *)osapiMalloc(L7_DRIVER_COMPONENT_ID, L7_MAX_PORT_COUNT * sizeof(BROAD_COS_POLICY_UTIL_ENTRY_t));
      if (broadCosUtilTable == L7_NULL)
      {
        L7_LOG_ERROR(0);
        return L7_FAILURE;
      }
    }
    for (i = 0; i < L7_MAX_PORT_COUNT; i++)
    {
        broadCosUtilTable[i].used = L7_FALSE;
        broadCosUtilTable[i].policyId = BROAD_POLICY_INVALID;
    }

    return L7_SUCCESS;
}

L7_BOOL hapiBroadCosPolicyUtilLookup(L7_uchar8 *data, L7_uint32 size, BROAD_POLICY_t *policy)
{
    int       i;
    L7_uint32 hash;

    *policy = BROAD_POLICY_INVALID;

    hash = hapiBroadCosPolicyUtilHash(data, size);

    for (i = 0; i < L7_MAX_PORT_COUNT; i++)
    {
        if ((L7_TRUE == broadCosUtilTable[i].used) &&
            (hash == broadCosUtilTable[i].hashVal) &&
            (size == broadCosUtilTable[i].hashSize))
        {
            *policy = broadCosUtilTable[i].policyId;
            return L7_TRUE;
        }
    }

    return L7_FALSE;
}

L7_RC_t hapiBroadCosPolicyUtilAdd(L7_uchar8 *data, L7_uint32 size, BROAD_POLICY_t policy)
{
    int            i;
    L7_uint32      hash;
    BROAD_POLICY_t tmpPolicy;

    if (hapiBroadCosPolicyUtilLookup(data, size, &tmpPolicy) == L7_TRUE)
    {
        if (tmpPolicy == policy)
            return L7_SUCCESS;   /* already present */

     /*   L7_LOG_ERROR(policy);  */     /* duplicate hash? */
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "ERROR: policy %d, tmpPolicy %d, size %d, data %d %d %d %d %d %d %d %d."
            " An issue installing the policy due to a possible duplicate hash.",
            policy, tmpPolicy, size, data[0], data[1], data[2], data[3], data[4],data[5],data[6],data[7]);  
    }

    hash = hapiBroadCosPolicyUtilHash(data, size);

    for (i = 0; i < L7_MAX_PORT_COUNT; i++)
    {
        if (L7_FALSE == broadCosUtilTable[i].used)
        {
            broadCosUtilTable[i].used    = L7_TRUE;
            broadCosUtilTable[i].hashVal = hash;
            broadCosUtilTable[i].hashSize= size;
            broadCosUtilTable[i].ifCount = 0;
            broadCosUtilTable[i].policyId= policy;

            return L7_SUCCESS;
        }
    }

    return L7_FAILURE;
}

L7_RC_t hapiBroadCosPolicyUtilApply(BROAD_POLICY_t policy, bcmx_lport_t lport)
{
    int     i;
    L7_RC_t result;

    for (i = 0; i < L7_MAX_PORT_COUNT; i++)
    {
        if ((L7_TRUE == broadCosUtilTable[i].used) && (policy == broadCosUtilTable[i].policyId))
        {
            result = hapiBroadPolicyApplyToIface(policy, lport);
            if (L7_SUCCESS == result)
                broadCosUtilTable[i].ifCount++;

            return result;
        }
    }

    return L7_FAILURE;
}

L7_RC_t hapiBroadCosPolicyUtilRemove(BROAD_POLICY_t policy, bcmx_lport_t lport)
{
    int     i;
    L7_RC_t result = L7_FAILURE;

    for (i = 0; i < L7_MAX_PORT_COUNT; i++)
    {
        if ((L7_TRUE == broadCosUtilTable[i].used) && (policy == broadCosUtilTable[i].policyId))
        {
            result = hapiBroadPolicyRemoveFromIface(policy, lport);
            if (L7_SUCCESS == result)
                broadCosUtilTable[i].ifCount--;
            
            if (broadCosUtilTable[i].ifCount <= 0)
            {
                (void)hapiBroadPolicyDelete(policy);

                broadCosUtilTable[i].used     = L7_FALSE;
                broadCosUtilTable[i].policyId = BROAD_POLICY_INVALID;
            }

            return result;
        }
    }

    return result;
}

L7_RC_t hapiBroadCosPolicyUtilDelete(BROAD_POLICY_t policy)
{
    int     i;
    L7_RC_t result = L7_FAILURE;

    for (i = 0; i < L7_MAX_PORT_COUNT; i++)
    {
        if ((L7_TRUE == broadCosUtilTable[i].used) && (policy == broadCosUtilTable[i].policyId))
        {
            (void)hapiBroadPolicyDelete(policy);

            broadCosUtilTable[i].used     = L7_FALSE;
            broadCosUtilTable[i].policyId = BROAD_POLICY_INVALID;

            return L7_SUCCESS;
        }
    }

    return result;
}

