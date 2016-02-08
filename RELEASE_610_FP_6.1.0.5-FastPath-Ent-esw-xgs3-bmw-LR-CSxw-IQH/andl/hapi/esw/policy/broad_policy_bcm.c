/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy_bcm.c
*
* This file implements the custom bcm layer that runs on all units. 
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


#include "broad_policy_bcm.h"
#include "broad_group_bcm.h"
#include "broad_filter_bcm.h"
#include "broad_cfp_bcm.h"
#include "zlib.h"
#include "ibde.h"

#define DECOMPRESS_BUFFER_BYTES (sizeof(BROAD_POLICY_CUSTOM_DATA_t))

static char src_buffer[DECOMPRESS_BUFFER_BYTES];
static char load_buffer[DECOMPRESS_BUFFER_BYTES];
static int  load_offset = 0;

/* Custom BCM Functions */

/* uncompress src_buffer into load_buffer and update policy size */
static int _policy_uncompress_buf(BROAD_POLICY_CUSTOM_DATA_t *src)
{
    int    err;
    uLongf dst_size;

    dst_size = sizeof(load_buffer);

    err = uncompress((Bytef *)load_buffer, &dst_size,
                     (Bytef *)src_buffer, load_offset);
    if (Z_OK != err)
        return BCM_E_FAIL;

    src->policySize = dst_size;

    return BCM_E_NONE;
}

int custom_policy_init()
{
    int rv;
    int i;
    L7_BOOL isRobo = L7_FALSE;
    L7_BOOL xgs3 = L7_FALSE;
    
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      if (SOC_IS_XGS3_SWITCH(i))
      {
        xgs3 = L7_TRUE;
      }
      if (SOC_IS_ROBO(i))
      {
        isRobo = L7_TRUE;
      }

    }

    if (isRobo == L7_TRUE)
    {
      rv = l7_bcm_cfp_policy_init();
    }
    else if (xgs3 == L7_FALSE)
    { 
      rv = l7_bcm_policy_xgs2_init();
    }
    else
    {
      rv = l7_bcm_policy_init();
    }

    return rv;
}

int custom_policy_set_handler(int unit, bcm_port_t port, int setget, uint32 *args)
{
    int                         rv = BCM_E_NONE;
    BROAD_POLICY_CUSTOM_DATA_t *policyHdr;
    BROAD_POLICY_ENTRY_t       *policyData;
    int                         newPolicySize;

    policyHdr = (BROAD_POLICY_CUSTOM_DATA_t *)args;

    /* Reassemble fragmented policy */

    if (policyHdr->policyFlags & BROAD_POLICY_FIRST)
        load_offset = 0;

    newPolicySize = load_offset + policyHdr->policySize;
    if( newPolicySize > DECOMPRESS_BUFFER_BYTES )
    {
      /* insufficient space to load fragment */
      return BCM_E_MEMORY;
    }

    /* If the policy needs to be uncompressed, then copy the data into src buffer */
    /* otherwise, just send it directly into policyData */

    policyData = (policyHdr->policyFlags & BROAD_POLICY_COMPRESSED)
      ? (BROAD_POLICY_ENTRY_t*) &src_buffer[load_offset]
      : (BROAD_POLICY_ENTRY_t*) &load_buffer[load_offset];

    memcpy(policyData, &policyHdr->cmdData, policyHdr->policySize);
    load_offset = newPolicySize;

    if ( ! (policyHdr->policyFlags & BROAD_POLICY_LAST) ) 
    {
      /* wait for more data */
      return BCM_E_NONE;
    }

    if (policyHdr->policyFlags & BROAD_POLICY_COMPRESSED)
    {
        /* uncompress src_buffer into load_buffer and update policy size */
        rv = _policy_uncompress_buf(policyHdr);
        if (BCM_E_NONE != rv)
            return rv;
    }
    else
    {
        /* policy already in load_buffer; update policy size */
        policyHdr->policySize = load_offset;
    }

    policyData = (BROAD_POLICY_ENTRY_t*)load_buffer;

    if (!SOC_IS_XGS3_SWITCH(unit) && !(SOC_IS_ROBO(unit)))
    { 
      switch(policyHdr->policyCmd)
      {
      case BROAD_CUSTOM_POLICY_CREATE:
          rv = l7_bcm_policy_xgs2_create(unit, policyHdr->policyId, policyData);
          break;
      case BROAD_CUSTOM_POLICY_DESTROY:
          rv = l7_bcm_policy_xgs2_destroy(unit, policyHdr->policyId);
          break;
      case BROAD_CUSTOM_POLICY_APPLY:
          rv = l7_bcm_policy_xgs2_apply(unit, policyHdr->policyId, port);
          break;
      case BROAD_CUSTOM_POLICY_APPLY_ALL:
          rv = l7_bcm_policy_xgs2_apply_all(unit, policyHdr->policyId);
          break;
      case BROAD_CUSTOM_POLICY_REMOVE:
          rv = l7_bcm_policy_xgs2_remove(unit, policyHdr->policyId, port);
          break;
      case BROAD_CUSTOM_POLICY_REMOVE_ALL:
          rv = l7_bcm_policy_xgs2_remove_all(unit, policyHdr->policyId);
          break;
      case BROAD_CUSTOM_POLICY_STATS:
          rv = l7_bcm_policy_xgs2_stats(unit, policyHdr->policyId, args);
          break;
      default:
          rv = BCM_E_PARAM;
          break;
      }
    }
    else  if (SOC_IS_XGS3_SWITCH(unit))
    {
      switch(policyHdr->policyCmd)
      {
      case BROAD_CUSTOM_POLICY_CREATE:
          rv = l7_bcm_policy_create(unit, policyHdr->policyId, policyData);
          break;
      case BROAD_CUSTOM_POLICY_DESTROY:
          rv = l7_bcm_policy_destroy(unit, policyHdr->policyId);
          break;
      case BROAD_CUSTOM_POLICY_CHECK:
          rv = l7_bcm_policy_check(unit, policyHdr->policyId, port);
          break;
      case BROAD_CUSTOM_POLICY_APPLY:
          rv = l7_bcm_policy_apply(unit, policyHdr->policyId, port);
          break;
      case BROAD_CUSTOM_POLICY_APPLY_ALL:
          rv = l7_bcm_policy_apply_all(unit, policyHdr->policyId);
          break;
      case BROAD_CUSTOM_POLICY_REMOVE:
          rv = l7_bcm_policy_remove(unit, policyHdr->policyId, port);
          break;
      case BROAD_CUSTOM_POLICY_REMOVE_ALL:
          rv = l7_bcm_policy_remove_all(unit, policyHdr->policyId);
          break;
      case BROAD_CUSTOM_POLICY_STATS:
          rv = l7_bcm_policy_stats(unit, policyHdr->policyId, args);
          break;
      default:
          rv = BCM_E_PARAM;
          break;
      }
    }
    else
    {
      switch(policyHdr->policyCmd)
      {
      case BROAD_CUSTOM_POLICY_CREATE:
          rv = l7_bcm_cfp_policy_create(unit, policyHdr->policyId, policyData);
          break;
      case BROAD_CUSTOM_POLICY_DESTROY:
          rv = l7_bcm_cfp_policy_destroy(unit, policyHdr->policyId);
          break;
      case BROAD_CUSTOM_POLICY_CHECK:
          rv = l7_bcm_cfp_policy_check(unit, policyHdr->policyId, port);
          break;
      case BROAD_CUSTOM_POLICY_APPLY:
          rv = l7_bcm_cfp_policy_apply(unit, policyHdr->policyId, port);
          break;
      case BROAD_CUSTOM_POLICY_APPLY_ALL:
          rv = l7_bcm_cfp_policy_apply_all(unit, policyHdr->policyId);
          break;
      case BROAD_CUSTOM_POLICY_REMOVE:
          rv = l7_bcm_cfp_policy_remove(unit, policyHdr->policyId, port);
          break;
      case BROAD_CUSTOM_POLICY_REMOVE_ALL:
          rv = l7_bcm_cfp_policy_remove_all(unit, policyHdr->policyId);
          break;
      case BROAD_CUSTOM_POLICY_STATS:
          rv = l7_bcm_cfp_policy_stats(unit, policyHdr->policyId, args);
          break;
      default:
          rv = BCM_E_PARAM;
          break;
      }
    }

    return rv;
}

int custom_policy_get_handler(int unit, bcm_port_t port, int setget, uint32 *args)
{
    int rv;
    BROAD_POLICY_t policyId;

    if (SOC_IS_XGS_FABRIC(unit))
    {
       return BCM_E_NOT_FOUND;
    }
    
    /* no way to specify params to GET functions so using port */
    policyId = port;

    if (!SOC_IS_XGS3_SWITCH(unit) && !(SOC_IS_ROBO(unit)))
    { 
      /* only GET function supported is stats query */
      rv = l7_bcm_policy_xgs2_stats(unit, policyId, args);
    }
    else if (SOC_IS_XGS3_SWITCH(unit))
    {
    /* only GET function supported is stats query */
    rv = l7_bcm_policy_stats(unit, policyId, args);
    }

    else
    {
    /* only GET function supported is stats query */
    rv = l7_bcm_cfp_policy_stats(unit, policyId, args);
    }
    
    return rv;
}

