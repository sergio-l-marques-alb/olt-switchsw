/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy_util.c
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

#include "broad_policy.h"
#include "zlib.h"

typedef struct
{
    L7_uchar8      used;
    L7_ushort16    ifCount;
    L7_uint32      hashVal;
    L7_uint32      hashSize;
    BROAD_POLICY_t policyId;
}
BROAD_POLICY_UTIL_ENTRY_t;

static BROAD_POLICY_UTIL_ENTRY_t *broadUtilTable = L7_NULL;

static L7_uint32 hapiBroadPolicyUtilHash(L7_uchar8 *data, L7_uint32 size)
{
    /* Skip instanceId as it can vary and jump right to the rules. */
    return crc32(0L, data, size);
}

L7_RC_t hapiBroadPolicyUtilInit()
{
    int i;

    if (broadUtilTable == L7_NULL)
    {
      broadUtilTable = (BROAD_POLICY_UTIL_ENTRY_t *)osapiMalloc(L7_DRIVER_COMPONENT_ID, BROAD_MAX_POLICIES * sizeof(BROAD_POLICY_UTIL_ENTRY_t));
      if (broadUtilTable == L7_NULL)
      {
        LOG_ERROR(0);
        return L7_FAILURE;
      }
    }
    for (i = 0; i < BROAD_MAX_POLICIES; i++)
    {
        broadUtilTable[i].used = L7_FALSE;
        broadUtilTable[i].policyId = BROAD_POLICY_INVALID;
    }

    return L7_SUCCESS;
}

L7_BOOL hapiBroadPolicyUtilLookup(L7_uchar8 *data, L7_uint32 size, BROAD_POLICY_t *policy)
{
    int       i;
    L7_uint32 hash;

    *policy = BROAD_POLICY_INVALID;

    hash = hapiBroadPolicyUtilHash(data, size);

    for (i = 0; i < BROAD_MAX_POLICIES; i++)
    {
        if ((L7_TRUE == broadUtilTable[i].used) &&
            (hash == broadUtilTable[i].hashVal) &&
            (size == broadUtilTable[i].hashSize))
        {
            *policy = broadUtilTable[i].policyId;
            return L7_TRUE;
        }
    }

    return L7_FALSE;
}

L7_RC_t hapiBroadPolicyUtilAdd(L7_uchar8 *data, L7_uint32 size, BROAD_POLICY_t policy)
{
    int            i;
    L7_uint32      hash;
    BROAD_POLICY_t tmpPolicy;

    if (hapiBroadPolicyUtilLookup(data, size, &tmpPolicy) == L7_TRUE)
    {
        if (tmpPolicy == policy)
            return L7_SUCCESS;   /* already present */

     /*   LOG_ERROR(policy);  */     /* duplicate hash? */
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "ERROR: policy %d, tmpPolicy %d, size %d, data %d %d %d %d %d %d %d %d."
            " An issue installing the policy due to a possible duplicate hash.",
            policy, tmpPolicy, size, data[0], data[1], data[2], data[3], data[4],data[5],data[6],data[7]);  
    }

    hash = hapiBroadPolicyUtilHash(data, size);

    for (i = 0; i < BROAD_MAX_POLICIES; i++)
    {
        if (L7_FALSE == broadUtilTable[i].used)
        {
            broadUtilTable[i].used    = L7_TRUE;
            broadUtilTable[i].hashVal = hash;
            broadUtilTable[i].hashSize= size;
            broadUtilTable[i].ifCount = 0;
            broadUtilTable[i].policyId= policy;

            return L7_SUCCESS;
        }
    }

    return L7_FAILURE;
}

L7_RC_t hapiBroadPolicyUtilApply(BROAD_POLICY_t policy, bcmx_lport_t lport)
{
    int     i;
    L7_RC_t result;

    for (i = 0; i < BROAD_MAX_POLICIES; i++)
    {
        if ((L7_TRUE == broadUtilTable[i].used) && (policy == broadUtilTable[i].policyId))
        {
            if (hapiBroadPolicyIfaceCheck(policy, lport) == L7_ALREADY_CONFIGURED)
            {
                return L7_SUCCESS;
            }

            result = hapiBroadPolicyApplyToIface(policy, lport);
            if (L7_SUCCESS == result)
                broadUtilTable[i].ifCount++;

            return result;
        }
    }

    return L7_FAILURE;
}

L7_RC_t hapiBroadPolicyUtilRemove(BROAD_POLICY_t policy, bcmx_lport_t lport)
{
    int     i;
    L7_RC_t result = L7_FAILURE;

    for (i = 0; i < BROAD_MAX_POLICIES; i++)
    {
        if ((L7_TRUE == broadUtilTable[i].used) && (policy == broadUtilTable[i].policyId))
        {
            result = hapiBroadPolicyRemoveFromIface(policy, lport);
            if (L7_SUCCESS == result)
                broadUtilTable[i].ifCount--;
            
            if (broadUtilTable[i].ifCount <= 0)
            {
                (void)hapiBroadPolicyDelete(policy);

                broadUtilTable[i].used     = L7_FALSE;
                broadUtilTable[i].policyId = BROAD_POLICY_INVALID;
            }

            return result;
        }
    }

    return result;
}

L7_RC_t hapiBroadPolicyUtilDelete(BROAD_POLICY_t policy)
{
    int     i;
    L7_RC_t result = L7_FAILURE;

    for (i = 0; i < BROAD_MAX_POLICIES; i++)
    {
        if ((L7_TRUE == broadUtilTable[i].used) && (policy == broadUtilTable[i].policyId))
        {
            (void)hapiBroadPolicyDelete(policy);

            broadUtilTable[i].used     = L7_FALSE;
            broadUtilTable[i].policyId = BROAD_POLICY_INVALID;

            return L7_SUCCESS;
        }
    }

    return result;
}
