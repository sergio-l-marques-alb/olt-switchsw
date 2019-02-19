/*
 * $Id: saber2.h,v 1.34 Broadcom SDK $
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
 * File:        sat.h
 * Purpose:     Function declarations for SAT bcm internal functions
 */

#ifndef _BCM_INT_SAT_H_
#define _BCM_INT_SAT_H_


/* SAT UP-MEP OLP header type/subtype */
#define BCM_SAT_UPMEP_OLP_HDR_TYPE                   0x002E

/* SAT Down-MEP OLP header type/subtype */
#define BCM_SAT_DOWNMEP_OLP_HDR_TYPE                 0x000E


struct _bcm_sat_hash_data_s {
    int             in_use;           /* Endpoint usage status.  */
    uint16          sglp;             /* Source GLP port */              
    uint16          dglp;             /* Destination GLP port */
	bcm_sat_endpoint_info_t  ep_info; /* End point information */
};  


typedef struct _bcm_sat_hash_data_s _bcm_sat_hash_data_t;  
 

/*
 * Typedef:
 *     _bcm_sat_control_t
 * Purpose:
 *     SAT module control structure. One structure for each XGS device.
 */
struct _bcm_sat_control_s {
    int                         init;               /* TRUE if SAT module has    */
                                                    /* been initialized.         */
    sal_mutex_t                 sat_lock;           /* Protection mutex          */
    uint32                      upsamp_ep_count;    /* Max number of UP-MEP      */
                                                    /* endpoints supported.      */
    uint32                      downsamp_ep_count;  /* Max number of DOWN_MEP    */
                                                    /* endpoints supported.      */
    shr_idxres_list_handle_t    upsamp_mep_pool;    /* SAT UP-MEP endpoint indices  */
                                                    /* pool.                        */
    shr_idxres_list_handle_t    downsamp_mep_pool;  /* SAT UP-MEP endpoint indices  */
	                                                /* pool.                        */
    shr_htb_hash_table_t        upsamp_htbl;        /* SAT UP-MEP endpoint hash table.      */
	_bcm_sat_hash_data_t       *upsamp_hash_data;   /* Pointer to UP-MEP SAT hash data memory. */
    shr_htb_hash_table_t        downsamp_htbl;      /* SAT Down-MEP endpoint hash table.      */
    _bcm_sat_hash_data_t       *downsamp_hash_data; /* Pointer to Down-MEP SAT hash data memory. */
};

typedef struct _bcm_sat_control_s _bcm_sat_control_t;

#endif  /* !_BCM_INT_SAT_H_ */
