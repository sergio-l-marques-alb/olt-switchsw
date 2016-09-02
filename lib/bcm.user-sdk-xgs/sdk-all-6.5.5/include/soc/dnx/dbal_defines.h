/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 */

#ifndef _DBAL_DEFINES_INCLUDED__
#define _DBAL_DEFINES_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal_defines_fields.h>
#include <soc/dnx/dbal_defines_tables.h>
#include <soc/dnx/dbal_defines_hw_entities.h>


typedef enum
{
    DBAL_PHYSICAL_TABLE_NONE,
    DBAL_PHYSICAL_TABLE_TCAM,
    DBAL_PHYSICAL_TABLE_LPM,
    DBAL_PHYSICAL_TABLE_ISEM_1,
    DBAL_PHYSICAL_TABLE_INLIF_1,
    DBAL_PHYSICAL_TABLE_IVSI,
    DBAL_PHYSICAL_TABLE_ISEM_2,
    DBAL_PHYSICAL_TABLE_INLIF_2,
    DBAL_PHYSICAL_TABLE_ISEM_3,
    DBAL_PHYSICAL_TABLE_INLIF_3,
    DBAL_PHYSICAL_TABLE_LEM,
    DBAL_PHYSICAL_TABLE_IOEM_0,
    DBAL_PHYSICAL_TABLE_IOEM_1,
    DBAL_PHYSICAL_TABLE_MAP,
    DBAL_PHYSICAL_TABLE_FEC_1,
    DBAL_PHYSICAL_TABLE_FEC_2,
    DBAL_PHYSICAL_TABLE_FEC_3,
    DBAL_PHYSICAL_TABLE_MC_ID,
	DBAL_PHYSICAL_TABLE_GLEM_0,
    DBAL_PHYSICAL_TABLE_GLEM_1,
    DBAL_PHYSICAL_TABLE_LL_1,
    DBAL_PHYSICAL_TABLE_LL_2,
    DBAL_PHYSICAL_TABLE_LL_3,
    DBAL_PHYSICAL_TABLE_LL_4,
    DBAL_PHYSICAL_TABLE_EEDB_1,
    DBAL_PHYSICAL_TABLE_EEDB_2,
    DBAL_PHYSICAL_TABLE_EEDB_3,
    DBAL_PHYSICAL_TABLE_EEDB_4,
    DBAL_PHYSICAL_TABLE_EOEM_0,
    DBAL_PHYSICAL_TABLE_EOEM_1,
    DBAL_PHYSICAL_TABLE_ESEM,
    DBAL_PHYSICAL_TABLE_EVSI,
    DBAL_PHYSICAL_TABLE_EXEM_1,
    DBAL_PHYSICAL_TABLE_EXEM_2,
    DBAL_PHYSICAL_TABLE_EXEM_3,
    DBAL_PHYSICAL_TABLE_EXEM_4,
    DBAL_PHYSICAL_TABLE_RMEP,

    DBAL_NOF_PHYSICAL_TABLES

}dbal_physical_tables_e;

#endif/*_DBAL_DEFINES_INCLUDED__*/

