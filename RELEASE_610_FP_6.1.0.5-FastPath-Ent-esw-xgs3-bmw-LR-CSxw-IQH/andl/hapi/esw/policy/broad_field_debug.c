/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_field_debug.c
*
* This file implements XGS III field processor test code.
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


#include "broad_common.h"
#include <bcm/field.h>
#include <bcmx/field.h>
#include "sal/core/libc.h"

#define LOG_QOS_ERROR(rv)  {printf("ERROR: File: %s, Line %d, Result %d\n", \
                                   __FILE__, __LINE__, rv);}

static int global_prio = 0;

L7_RC_t fieldInit()
{
    return L7_SUCCESS;
}

L7_RC_t fieldCreate()
{
    bcm_field_qset_t  qualSet;
    bcm_field_group_t aclId;
    bcm_field_entry_t entry;
    int               rv;

    /* Create qset for use by standard L3 ACL. */
    BCM_FIELD_QSET_INIT(qualSet);
    BCM_FIELD_QSET_ADD(qualSet,bcmFieldQualifySrcIp);
    BCM_FIELD_QSET_ADD(qualSet,bcmFieldQualifyInPorts);

    rv = bcmx_field_group_create(qualSet, global_prio, &aclId);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
        LOG_QOS_ERROR(rv);
        return L7_ERROR;
    }

    rv = bcmx_field_entry_create(aclId, &entry);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
        LOG_QOS_ERROR(rv);
        return L7_ERROR;
    }

    printf("Created ACL %d with entry %d\n", (int)aclId, entry);

    global_prio++;

    return L7_SUCCESS;
}

L7_RC_t fieldPort(bcm_field_entry_t entry, int matchAll)
{
    int        rv;
    bcm_pbmp_t port_list;
    bcm_pbmp_t port_mask;

    BCM_PBMP_CLEAR(port_list);
    BCM_PBMP_PORT_SET(port_list, 0);
    if (matchAll)
        port_mask = PBMP_E_ALL(0);   /* 1 means match on port bitmap */
    else
        BCM_PBMP_CLEAR(port_mask);   /* 0 means don't care so match globally */

    rv = bcm_field_qualify_InPorts(0, entry, port_list, port_mask);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
        LOG_QOS_ERROR(rv);
        return L7_ERROR;
    }

    printf("Added port\n");

    return L7_SUCCESS;
}

L7_RC_t fieldSrcIp(bcm_field_entry_t entry, bcm_ip_t ipAddr, bcm_ip_t ipMask)
{
    int    rv;

    rv = bcmx_field_qualify_SrcIp(entry, ipAddr, ipMask);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
        LOG_QOS_ERROR(rv);
        return L7_ERROR;
    }

    return L7_SUCCESS;
}

L7_RC_t fieldActionDrop(bcm_field_entry_t entry)
{
    int    rv;
    uint32 nil = 0;

    rv = bcmx_field_action_add(entry, bcmFieldActionDrop, nil, nil);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
        LOG_QOS_ERROR(rv);
        return L7_ERROR;
    }

    printf("Action set to DROP\n");

    return L7_SUCCESS;
}

L7_RC_t fieldInstall(bcm_field_entry_t entry)
{
    int rv;

    rv = bcmx_field_entry_install(entry);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
        LOG_QOS_ERROR(rv);
        return L7_ERROR;
    }

    printf("Installed Entry %d\n", (int)entry);

    return L7_SUCCESS;
}

L7_RC_t fieldRemove(bcm_field_entry_t entry)
{
    int rv;

    rv = bcmx_field_entry_remove(entry);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
        LOG_QOS_ERROR(rv);
        return L7_ERROR;
    }

    printf("Removed Entry %d\n", (int)entry);

    return L7_SUCCESS;
}

L7_RC_t fieldDump(int unit, bcm_field_group_t group, bcm_field_entry_t entry)
{
    bcm_field_status_t status;

    printf("\nUNIT: %d\n\n", unit);

    bcm_field_status_get(unit, &status);
    printf("Total groups : %d\n", status.group_total);
    printf("Unused groups:  %d\n", status.group_free);
    printf("\n\n");

#ifdef DEBUG
    bcm_field_group_dump(unit, group);
    bcm_field_entry_dump(unit, entry);
#endif

    return L7_SUCCESS;
}

