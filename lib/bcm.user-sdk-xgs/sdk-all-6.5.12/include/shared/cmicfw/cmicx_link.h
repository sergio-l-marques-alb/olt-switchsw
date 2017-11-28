/*
 * $Id: cmicx_link.h, rshende Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        cmicx_link.h
 * Purpose:     cmicx m0 linkscan management
 */

#ifndef __CMICX_LINK_H__
#define __CMICX_LINK_H__
#include <bcm/types.h>
#if defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD)
#include <shared/cmicfw/iproc_mbox.h>

#define MAX_LS_RETRY      50
#define LS_HW_INTERVAL    30 /* in msecs */
#define CMICX_LS_ALIVE    1
#define CMICX_LS_NOTALIVE 2

typedef enum ls_msgtype_e {
   LS_HW_CONFIG = 1,
   LS_HW_HEARTBEAT,
   LS_PAUSE,
   LS_CONTINUE,
   LS_LINK_STATUS_CHANGE
} ls_msgtype_t;

typedef struct soc_ls_heartbeat_s {
    uint32 m0_fw_version;
    uint32 host_fw_version;
}__attribute__ ((packed)) soc_ls_heartbeat_t;

#ifndef CMICX_FW_BUILD
typedef int (*soc_iproc_linkscan_sm_t)(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *msg);
int soc_cmicx_linkscan_hw_init(int unit);
int soc_cmicx_linkscan_hw_deinit(int unit);
int soc_cmicx_linkscan_config(int unit, soc_pbmp_t hw_mii_pbm);
int soc_cmicx_linkscan_heartbeat(int unit, soc_ls_heartbeat_t *ls_heartbeat);
int soc_cmicx_linkscan_hw_link_get(int unit, soc_pbmp_t *hw_link);
int soc_cmicx_linkscan_pause(int unit);
int soc_cmicx_linkscan_continue(int unit);
void cmicx_esw_linkscan_hw_interrupt(int unit);
void cmicx_common_linkscan_hw_interrupt(int unit);
int soc_cmicx_linkscan_hw_link_cache_get(int unit, soc_pbmp_t *hw_link);
#endif
#endif /*defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD)*/
#endif /* __CMICX_LINK_H__ */
