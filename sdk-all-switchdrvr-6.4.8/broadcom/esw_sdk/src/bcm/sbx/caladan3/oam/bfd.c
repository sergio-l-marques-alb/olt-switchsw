/*
 * $Id: bfd.c,v 1.28 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * CALADAN3 OAM API
 */

#if defined(INCLUDE_SBX_BFD)

#include <bcm/error.h>
#include <bcm/bfd.h>
#include <bcm_int/sbx/caladan3/oam/bfd.h>
#include <bcm_int/sbx_dispatch.h>

int
bcm_caladan3_bfd_init(int unit)
{
    /* Currently, the OAM init function handles both oam & bfd. */
    return BCM_E_UNAVAIL;
}

int
bcm_caladan3_bfd_detach(int unit)
{
    /* Currently, the OAM detach function handles both oam & bfd. */
    return BCM_E_UNAVAIL;
}


/*
 *   Function
 *      bcm_caladan3_bfd_endpoint_get
 *   Purpose
 *      Retrieve an oam endpoint with the given endpoint id
 *   Parameters
 *       unit           = BCM device number
 *       endpoint       = endpoint ID to retrieve
 *       endpoint_info  = storage location for found endpoint
 *   Returns
 *       BCM_E_*
 *  Notes:
 */
int
bcm_caladan3_bfd_endpoint_get(int unit, bcm_bfd_endpoint_t endpoint, 
                            bcm_bfd_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_UNAVAIL;
    return rv;
}


/*
 *   Function
 *      bcm_caladan3_bfd_endpoint_create
 *   Purpose
 *      Create a bfd endpoint and commit to hardware
 *   Parameters
 *       unit           = BCM device number
 *       endpoint_info  = description of endpoint to create
 *   Returns
 *       BCM_E_*
 *  Notes:
 *
 *  BFD over LSP over trunk notes:
 *    1/ There is no gport associated with a LSP. We get to know if there is 
 *       a trunk underneath the LSP when we fetch the tunnel egress info
 *       None of the code under BCM_GPORT_IS_TRUNK is applicable in this case.
 *
 *    2/ Application can optionally specify a port within the trunk group 
 *       for BFD traffic using the trunk_index variable of the endpoint_info struct. 
 *       If incorrectly specified this defaults to index 0
 *
 *    3/ The selected port determines the QID to be used for BFD, this is 
 *       setup in _oam_egr_path_update. The endpoint->gport is internally 
 *       setup to reflect the MODPORT of the port over which the bfd traffic 
 *       has to be sent
 *
 *    4/ We also enqueue the lsp endpoint for callback processing, and rely 
 *       on the oam callback for trunks. 
 */
int
bcm_caladan3_bfd_endpoint_create(int unit, 
                               bcm_bfd_endpoint_info_t *endpoint_info)
{
    int rv=BCM_E_UNAVAIL;
    return rv;
}


/*
 *   Function
 *      bcm_caladan3_bfd_endpoint_destroy
 *   Purpose
 *      Destroy a bfd endpoint and all allocated resources  with the given 
 *      endpoint id
 *   Parameters
 *       unit           = BCM device number
 *       endpoint       = endpoint ID to destroy
 *   Returns
 *       BCM_E_*
 *  Notes:
 *  BFD over LSP over trunk notes:
 *    1/ There is no gport associated with a LSP. We get to know if there is 
 *       a trunk underneath the LSP when we fetch the tunnel egress info
 *       None of the code under BCM_GPORT_IS_TRUNK is applicable in this case.
 *
 *    2/ Application can optionally specify a port within the trunk group 
 *       for BFD traffic using the trunk_index variable of the endpoint_info
 *       If incorrectly specified this defaults to index 0
 *
 *    3/ The selected port determines the QID to be used for BFD, this is 
 *       setup in _oam_egr_path_update. The endpoint->gport is internally 
 *       setup to reflect the MODPORT of the port over which the bfd traffic 
 *       has to be sent
 *
 *    4/ We also make sure that the endpoint is removed from the trunk_list
 *
 */
int
bcm_caladan3_bfd_endpoint_destroy(int unit, bcm_bfd_endpoint_t endpoint)
{
    return BCM_E_UNAVAIL;
}

/*
 *   Function
 *      bcm_caladan3_bfd_endpoint_destroy_all
 *   Purpose
 *      Destroy all bfd endpoints associated with the given unit
 *   Parameters
 *       unit         = BCM device number
 *   Returns
 *       BCM_E_*
 *  Notes:
 */
int
bcm_caladan3_bfd_endpoint_destroy_all(int unit)
{
    return BCM_E_UNAVAIL;
}


/* the following functions not currently supported */

/* Register a callback for handling BFD events */
int bcm_caladan3_bfd_event_register(
    int unit, 
    bcm_bfd_event_types_t event_types, 
    bcm_bfd_event_cb cb, 
    void *user_data) {
    return BCM_E_UNAVAIL;
}

/* Unregister a callback for handling BFD events */
int bcm_caladan3_bfd_event_unregister(
    int unit, 
    bcm_bfd_event_types_t event_types, 
    bcm_bfd_event_cb cb) {
    return BCM_E_UNAVAIL;
}

/* 
 * Poll an BFD endpoint object.  Valid only for BFD sessions in Demand
 * Mode
 */
int bcm_caladan3_bfd_endpoint_poll(
    int unit, 
    bcm_bfd_endpoint_t endpoint) {
    return BCM_E_UNAVAIL;
}

/* Set SHA1 authentication entry */
int bcm_caladan3_bfd_auth_sha1_set(
    int unit, 
    int index, 
    bcm_bfd_auth_sha1_t *sha1) {
    return BCM_E_UNAVAIL;
}

/* Get SHA1 authentication entry */
int bcm_caladan3_bfd_auth_sha1_get(
    int unit, 
    int index, 
    bcm_bfd_auth_sha1_t *sha1) {
    return BCM_E_UNAVAIL;
}

/* Set Simple Password authentication entry */
int bcm_caladan3_bfd_auth_simple_password_set(
    int unit, 
    int index, 
    bcm_bfd_auth_simple_password_t *sp) {
    return BCM_E_UNAVAIL;
}

/* Get Simple Password authentication entry */
int bcm_caladan3_bfd_auth_simple_password_get(
    int unit, 
    int index, 
    bcm_bfd_auth_simple_password_t *sp) {
    return BCM_E_UNAVAIL;
}

int bcm_caladan3_bfd_endpoint_stat_get(
    int unit, 
    bcm_bfd_endpoint_t endpoint, 
    bcm_bfd_endpoint_stat_t *ctr_info, 
    uint8 clear) {
    return BCM_E_UNAVAIL;
}



#else   /* INCLUDE_SBX_BFD */
int bcm_caladan3_oam_bfd_not_empty;

#if defined(INCLUDE_BFD)

#include <bcm/error.h>
#include <bcm/oam.h>
#include <bcm_int/sbx_dispatch.h>

int
bcm_caladan3_bfd_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}
int
bcm_caladan3_bfd_endpoint_create(
    int unit,
    bcm_bfd_endpoint_info_t *endpoint_info)
{
    return BCM_E_UNAVAIL;
}
int
bcm_caladan3_bfd_endpoint_destroy(
    int unit,
    bcm_bfd_endpoint_t endpoint)
{
    return BCM_E_UNAVAIL;
}
int
bcm_caladan3_bfd_endpoint_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
int
bcm_caladan3_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_bfd_endpoint_info_t *endpoint_info)
{
    return BCM_E_UNAVAIL;
}
int
bcm_caladan3_bfd_endpoint_poll(
    int unit,
    bcm_bfd_endpoint_t endpoint)
{
    return BCM_E_UNAVAIL;
}
int
bcm_caladan3_bfd_endpoint_stat_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_bfd_endpoint_stat_t *ctr_info,
    uint8 clear)
{
    return BCM_E_UNAVAIL;
}
int
bcm_caladan3_bfd_event_register(
    int unit,
    bcm_bfd_event_types_t event_types,
    bcm_bfd_event_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
int
bcm_caladan3_bfd_event_unregister(
    int unit,
    bcm_bfd_event_types_t event_types,
    bcm_bfd_event_cb cb)
{
    return BCM_E_UNAVAIL;
}
int
bcm_caladan3_bfd_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
int 
bcm_caladan3_bfd_auth_sha1_set(
    int unit,
    int index,
    bcm_bfd_auth_sha1_t *sha1) {
    return BCM_E_UNAVAIL;
}
int 
bcm_caladan3_bfd_auth_sha1_get(
    int unit,
    int index,
    bcm_bfd_auth_sha1_t *sha1) {
    return BCM_E_UNAVAIL;
}
int 
bcm_caladan3_bfd_auth_simple_password_set(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t *sp) {
    return BCM_E_UNAVAIL;
}
int 
bcm_caladan3_bfd_auth_simple_password_get(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t *sp) {
    return BCM_E_UNAVAIL;
}

#endif

#endif  /* INCLUDE_SBX_BFD */
