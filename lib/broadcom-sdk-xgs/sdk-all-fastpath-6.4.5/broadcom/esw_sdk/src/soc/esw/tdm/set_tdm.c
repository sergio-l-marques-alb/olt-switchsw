/*
 * $Id: tdm_main.c.$
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
 * All Rights Reserved.$
 *
 * File:        set_tdm.c
 * Purpose:     TDM algorithm
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
	#include <sdk_drv.h>
#else
	#include <soc/tdm/core/tdm_top.h>
	#include <soc/drv.h>
#endif

#ifndef _TDM_STANDALONE
tdm_mod_t
*SOC_SEL_TDM(tdm_soc_t *chip)
{
	tdm_mod_t *_tdm;
	uint16 dev_id;
	uint16 drv_dev_id;
	uint8 rev_id;
	uint8 drv_rev_id;
	
	_tdm = TDM_ALLOC(sizeof(tdm_mod_t),"TDM constructor allocation");
	if (!_tdm) {
		return NULL;
	}
	_tdm->_chip_data.soc_pkg = (*chip);
	
	soc_cm_get_id( _tdm->_chip_data.soc_pkg.unit, &dev_id, &rev_id );
	soc_cm_get_id_driver( dev_id, rev_id, &drv_dev_id, &drv_rev_id );

	switch (dev_id&0xfff0) {
		case (BCM56850_DEVICE_ID&0xfff0):
		case (BCM56860_DEVICE_ID&0xfff0):
		case (BCM56830_DEVICE_ID&0xfff0):
			{
#if ( defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) )
				int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
					&tdm_core_init,
					&tdm_core_post,
					&tdm_td2p_vmap_alloc,
					&tdm_core_vbs_scheduler,
					&tdm_chip_td2p_shim__core_vbs_scheduler_ovs,
					&tdm_core_null, /* TDM_CORE_EXEC__EXTRACT */
					&tdm_core_null, /* TDM_CORE_EXEC__FILTER */
					&tdm_core_acc_alloc,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_td2p_check_ethernet,
					&tdm_pick_vec,
					&tdm_td2p_which_tsc
				};
				int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
					&tdm_td2p_init,
					&tdm_td2p_pmap_transcription,
					&tdm_td2p_lls_wrapper,
					&tdm_td2p_vbs_wrapper,
					&tdm_td2p_filter_chain,
					&tdm_td2p_parse_mmu_tdm_tbl,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_td2p_free,
					&tdm_td2p_corereq,
					&tdm_td2p_post
				};
				TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
				TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
			}
			break;
		case (BCM56960_DEVICE_ID&0xfff0):
			{
#ifdef BCM_TOMAHAWK_SUPPORT
				int (*core_exec[TDM_EXEC_CORE_SIZE])( tdm_mod_t* ) = {
					&tdm_core_init,
					&tdm_core_post,
					&tdm_th_vmap_alloc,
					&tdm_core_vbs_scheduler,
					&tdm_chip_th_shim__core_vbs_scheduler_ovs,
					&tdm_core_null, /* TDM_CORE_EXEC__EXTRACT */
					&tdm_core_null, /* TDM_CORE_EXEC__FILTER */
					&tdm_core_acc_alloc,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_th_check_ethernet,
					&tdm_pick_vec,
					&tdm_th_which_tsc
				};
				int (*chip_exec[TDM_EXEC_CHIP_SIZE])( tdm_mod_t* ) = {
					&tdm_th_init,
					&tdm_th_pmap_transcription,
					&tdm_th_scheduler_wrap,
					&tdm_core_null,
					&tdm_th_filter_chain,
					&tdm_th_parse_tdm_tbl,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_core_null,
					&tdm_th_corereq,
					&tdm_th_post
				};
				TDM_COPY(_tdm->_core_exec,core_exec,sizeof(_tdm->_core_exec));
				TDM_COPY(_tdm->_chip_exec,chip_exec,sizeof(_tdm->_chip_exec));
#endif
			}
			break;
		default:
			TDM_FREE(_tdm);
			TDM_ERROR1("Unrecognized device ID %0x for TDM scheduling algorithm.\n",dev_id);
			return NULL;
	}
	
	return _tdm;
}
#endif


tdm_mod_t
*_soc_set_tdm_tbl( tdm_mod_t *_tdm )
{
	int index, tdm_ver_chk[8];
	
	if (!_tdm) {
		return NULL;
	}
	TDM_BIG_BAR
	TDM_PRINT0("TDM: Release version: ");
	_soc_tdm_ver(tdm_ver_chk);
	TDM_PRINT2("%d%d",tdm_ver_chk[0],tdm_ver_chk[1]);
	for (index=2; index<8; index+=2) {
		TDM_PRINT2(".%d%d",tdm_ver_chk[index],tdm_ver_chk[index+1]);
	}
	TDM_PRINT0("\n"); TDM_SML_BAR
	
	/* Path virtualized API starting in chip executive */
	return ((_tdm->_chip_exec[TDM_CHIP_EXEC__INIT](_tdm))==PASS)?(_tdm):(NULL);
}
