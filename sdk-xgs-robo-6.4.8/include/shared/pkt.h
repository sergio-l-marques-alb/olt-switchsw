/*
 * $Id: multicast.h,v 1.0 Broadcom SDK $
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
 * This file contains multicast definitions.
 */

#ifndef _SHR_PKT_H
#define _SHR_PKT_H
#include <shared/types.h>
#include <shared/pbmp.h>
#include <shared/rx.h>
#include <shared/port.h>



#define _SHR_PKT_NOF_DNX_HEADERS 9
#define _SHR_PKT_DNX_RAW_SIZE_MAX    20  


/* Pkt DNX types. */
typedef enum  {
    _SHR_PKT_DNX_TYPE_PTCH1 = 0, /* PTCH-1 Header. */
    _SHR_PKT_DNX_TYPE_PTCH2 = 1, /* PTCH-2 Header. */
    _SHR_PKT_DNX_TYPE_ITMH = 2,  /* ITMH Header. */
    _SHR_PKT_DNX_TYPE_FTMH = 3,  /* FMTH Header. */
    _SHR_PKT_DNX_TYPE_PPH = 4,   /* PPH Header. */
    _SHR_PKT_DNX_TYPE_OTSH = 5,  /* OAM-TS Header (OTSH). */
    _SHR_PKT_DNX_TYPE_OTMH = 6,  /* OTMH Header. */
    _SHR_PKT_DNX_TYPE_RAW = 7    /* Raw Header. */
} _shr_pkt_dnx_type_t;
 
 
/* PTCH1 */
/* itmh dest type. */
typedef enum  {
    _SHR_PKT_DNX_ITMH_DEST_TYPE_MULTICAST = 0, /* ITMH destination type is multicast. */
    _SHR_PKT_DNX_ITMH_DEST_TYPE_FLOW = 1,      /* ITMH destination type is flow. */
    _SHR_PKT_DNX_ITMH_DEST_TYPE_INGRESS_SHAPING_FLOW = 2, /* ITMH destination type is ingress
                                           shaping flow. */
    _SHR_PKT_DNX_ITMH_DEST_TYPE_VPORT = 3,     /* ITMH destination type is out lif. */
    _SHR_PKT_DNX_ITMH_DEST_TYPE_SYSTEM_PORT = 4 /* ITMH destination type is system port. */
} _shr_pkt_dnx_itmh_dest_type_t;


/* ftmh action type. */
typedef enum  {
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_FORWARD = 0, /* TM action is forward */
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_SNOOP = 1,   /* TM action is snoop */
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_INBOUND_MIRROR = 2, /* TM action is inbound mirror. */
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_OUTBOUND_MIRROR = 3 /* TM action is outbound mirror. */
} _shr_pkt_dnx_ftmh_action_type_t;
  
/* pph fhei type */
typedef enum  {
    _SHR_PKT_DNX_PPH_FHEI_TYPE_BRIDGE = 0, /* PPH FHEI Bridge */
    _SHR_PKT_DNX_PPH_FHEI_TYPE_TRAP = 1,   /* PPH FHEI Trap */
    _SHR_PKT_DNX_PPH_FHEI_TYPE_IP = 2,     /* PPH FHEI Ip */
    _SHR_PKT_DNX_PPH_FHEI_TYPE_MPLS = 3,   /* PPH FHEI Mpls */
    _SHR_PKT_DNX_PPH_FHEI_TYPE_TRILL = 4   /* PPH FHEI Trill */
} _shr_pkt_dnx_pph_fhei_type_t;

/* otsh type */
typedef enum  {
    _SHR_PKT_DNX_OTSH_TYPE_OAM = 0,       /* OAM-TS type is OAM */
    _SHR_PKT_DNX_OTSH_TYPE_L588v2 = 1,    /* OAM-TS type is 1588v2 */
    _SHR_PKT_DNX_OTSH_TYPE_LATENCY = 2    /* OAM-TS type is packet latency */
} _shr_pkt_dnx_otsh_type_t;

/* otsh oam sutype */
typedef enum  {
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_NONE = 0,    /* None */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_LM = 1,      /* Loss Measurement (LM) */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_DM1588 = 2,  /* Delay Measurement (DM) - 1588 ToD */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_DM_NTP = 3,   /* Delay Measurement (DM) - NTP ToD */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_OAM_DEFAULT = 4, /* Default OAM type */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_LOOPBACK = 5, /* Loopback */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_ECN = 7      /* ECN */
} _shr_pkt_dnx_otsh_oam_subtype_t;

  
#endif /* _SHR_PKT_H */
