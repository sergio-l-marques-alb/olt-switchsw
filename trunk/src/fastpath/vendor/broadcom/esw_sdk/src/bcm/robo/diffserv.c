/*
 * $Id: diffserv.c 1.9 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * File:	diffserv.c
 * Purpose:	API for diffserv applications who don't want to use filter and
 *              meter APIs.
 *
 * Note : Not for RoboSwitch currently.
 */

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/diffserv.h>

int 
bcm_robo_ds_classifier_create(int unit, int dpid,
                                    bcm_ds_clfr_t *clfr,
                                    bcm_ds_inprofile_actn_t *inp_actn,
                                    bcm_ds_outprofile_actn_t *outp_actn,
                                    bcm_ds_nomatch_actn_t *nm_actn,
                                    int *cfid)
{
	return BCM_E_UNAVAIL;
}
int 
bcm_robo_ds_classifier_id_create(int unit, int dpid,
                                       bcm_ds_clfr_t *clfr,
                                       bcm_ds_inprofile_actn_t *inp_actn,
                                       bcm_ds_outprofile_actn_t *outp_actn,
                                       bcm_ds_nomatch_actn_t *nm_actn,
                                       int cfid)
{
	return BCM_E_UNAVAIL;
}

int 
bcm_robo_ds_classifier_update(int unit, int dpid, int cfid,
                                    uint32 flags,
                                    bcm_ds_inprofile_actn_t *inp_actn,
                                    bcm_ds_outprofile_actn_t *outp_actn)
{
	return BCM_E_UNAVAIL;
}

int 
bcm_robo_ds_classifier_delete(int unit, int dpid, int cfid)
{
	return BCM_E_UNAVAIL;
}

int 
bcm_robo_ds_init(int unit)
{
	return BCM_E_UNAVAIL;
}
int 
bcm_robo_ds_datapath_create(int unit, uint32 flags, bcm_pbmp_t ports,
                                  int *dpid)
{
	return BCM_E_UNAVAIL;
}

int
bcm_robo_ds_classifier_create_id(int unit, int dpid,
                            bcm_ds_clfr_t *clfr,
                            bcm_ds_inprofile_actn_t *inp_actn,
                            bcm_ds_outprofile_actn_t *outp_actn,
                            bcm_ds_nomatch_actn_t *nm_actn,
                            int cfid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_ds_datapath_create_id(int unit, uint32 flags, bcm_pbmp_t ports, int dpid)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_ds_datapath_id_create(int unit, uint32 flags, bcm_pbmp_t ports,
                                     int dpid)
{
	return BCM_E_UNAVAIL;
}

int 
bcm_robo_ds_datapath_delete(int unit, int dpid)
{
	return BCM_E_UNAVAIL;
}

int 
bcm_robo_ds_datapath_install(int unit, int dpid)
{
	return BCM_E_UNAVAIL;
}
	
int 
bcm_robo_ds_scheduler_add(int unit, int dpid,
                                bcm_ds_scheduler_t *scheduler)
{
	return BCM_E_UNAVAIL;
}
                                
int 
bcm_robo_ds_counter_get(int unit, int dpid, int cfid,
                              bcm_ds_counters_t *counter)
{
	return BCM_E_UNAVAIL;
}
                              
int
bcm_robo_ds_rate_get(int unit,
                     int dpid,
                     int cfid,
                     uint32 *kbits_sec,
                     uint32 *kbits_burst)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_ds_dpid_cfid_bind(int unit, void *mibid, int dpid, int cfid)
{
	return BCM_E_UNAVAIL;
}
	
int 
bcm_robo_ds_dpid_cfid_unbind(int unit, void *mibid, int dpid, int cfid)
{
	return BCM_E_UNAVAIL;
}
	
int 
bcm_robo_ds_dpid_cfid_get(int unit, void *mibid, int dpid, int *cfid)
{
	return BCM_E_UNAVAIL;
}

int
bcm_robo_ds_classifier_get(int unit, int dpid, int cfid,
                          bcm_ds_clfr_t *clfr, 
                          bcm_ds_inprofile_actn_t *inp_actn, 
                          bcm_ds_outprofile_actn_t *outp_actn, 
                          bcm_ds_nomatch_actn_t *nm_actn)
{
    return BCM_E_UNAVAIL;
}

int 
bcm_robo_ds_classifier_traverse(int unit, int dpid, 
                               bcm_ds_classifier_traverse_cb cb, 
                               void *user_data) 
{
    return BCM_E_UNAVAIL;
}
