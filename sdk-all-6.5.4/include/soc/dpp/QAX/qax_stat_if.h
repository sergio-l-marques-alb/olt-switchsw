/*
 * $Id: $
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
 * File: qax_stat_if.h
 */

#ifndef __QAC_STAT_IF_INCLUDED__

#define __QAX_STAT_IF_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_api_stat_if.h>

#define QAX_STAT_IF_REPORT_DESC_THRESHOLD_MAX (0x7F0000)
#define QAX_STAT_IF_REPORT_THRESHOLD_IGNORED_VALUE (0xFFF)
#define QAX_SCRUBBER_TRESHOLDS_MANTISSA_NOF_BITS (8)
#define QAX_SCRUBBER_TRESHOLDS_EXP_NOF_BITS (8)
#define QAX_REPORT_QUEUE_SIZE_MAX (0x7FFF)

#define QUX_REPORT_QUEUE_SIZE_MAX (0x3FFF)

uint32 soc_qax_stat_if_init_set(int unit, ARAD_INIT_STAT_IF *stat_if);
uint32 soc_qax_stat_if_report_info_set(int unit, int nof_sif_ports, ARAD_INIT_STAT_IF *stat_if);
soc_error_t soc_qax_stat_if_queue_range_set(int unit, int core, int is_scrubber, int min_queue, int max_queue);
soc_error_t soc_qax_stat_if_queue_range_get(int unit, int core, int is_scrubber, int* min_queue, int* max_queue);

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /*__QAX_STAT_IF_INCLUDED__*/

