/*
 * $Id: iproc_cmicx_link.c$
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose: CMICx link scan module
 */
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/cmic.h>
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#include <shared/cmicfw/iproc_fwconfig.h>
#include <shared/cmicfw/cmicx_link.h>
#include <shared/cmicfw/m0_ver.h>

int soc_iproc_linkscan_process_message(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *msg);

/*
 * Function:
 *      _soc_iproc_l2p_pbmp_update
 * Purpose:
 *      Convert given port bitmap from logical to physical port numbering
 * Parameters:
 *      unit number
 *      port bitmap based on logical port numbering
 *      port bitmap converted to physical port numbering
 * Returns:
 *      none
 */
STATIC
void _soc_iproc_l2p_pbmp_update(int unit, soc_pbmp_t logical_pbmp, soc_pbmp_t *phy_pbmp)
{
    soc_port_t port;
    int phy_port = 0;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    SOC_PBMP_ITER(logical_pbmp, port) {
        phy_port = si->port_l2p_mapping[port];
        SOC_PBMP_PORT_ADD(*phy_pbmp, phy_port);
    }

    return;
}

/*
 * Function:
 *      _soc_iproc_p2l_pbmp_update
 * Purpose:
 *      Convert given port bitmap from physical to logical port numbering
 * Parameters:
 *      unit number
 *      port bitmap based on physical port numbering
 *      port bitmap converted to logical port numbering
 * Returns:
 *      none
 */
STATIC
void _soc_iproc_p2l_pbmp_update(int unit, soc_pbmp_t phy_pbmp, soc_pbmp_t *logical_pbmp)
{
    soc_port_t phy_port;
    int port = 0;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    SOC_PBMP_ITER(phy_pbmp, phy_port) {
        port = si->port_p2l_mapping[phy_port];
        SOC_PBMP_PORT_ADD(*logical_pbmp, port);
    }

    return;
}

/*
 * Function:
 *      _soc_iproc_optimized_cpu_port_to_pbmp_add
 * Purpose:
 *      Add cpu port to given port bitmap and adjust rest of ports accordingly
 *      This function is needed since port bitmap returned by FW does not take
 *      into account cpu port at bit 0 which is required by rest of SDK code
 *      CPU port is not tracked by HW linkscan FW.
 * Parameters:
 *      unit number
 *      port bitmap to be modified to contain cpu port
 * Returns:
 *      none
 */
STATIC
void _soc_iproc_optimized_cpu_port_to_pbmp_add(int unit, soc_pbmp_t *dev_pbmp)
{
    uint32 tmp1 = 0, tmp2 = 0, idx = 0;
    uint32 reg1, reg2;

    reg1 = soc_cmicx_miim_phy_reg_read(unit, idx);

    /* For 1st word, save bit 31 before left shift to add cpu port */
    tmp1 = (reg1 & 0x80000000) >> 31;
    SOC_PBMP_WORD_SET(*dev_pbmp, 0, (reg1 << 1));

    for(idx = 1; idx < _SHR_PBMP_WORD_MAX; idx++) {
        /* Set saved bit 31 from previous to bit 0 of next word */
        reg2 = soc_cmicx_miim_phy_reg_read(unit, idx);
        tmp2 = (reg2 << 1) | tmp1;
        /* Add updated word to pbmp to be returned */
        SOC_PBMP_WORD_SET(*dev_pbmp, idx, tmp2);
        /* Save bit 31 from current word to be used in next iteration */
        reg1 = soc_cmicx_miim_phy_reg_read(unit, idx);
        tmp1 = (reg1 & 0x80000000) >> 31;
    }

    return;
}

/*
 * Function:
 *      soc_cmicx_linkscan_hw_link_cache_get
 * Purpose:
 *      Get the current HW link status from linkscan cached copy
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_NONE
 */
int soc_cmicx_linkscan_hw_link_cache_get(int unit, soc_pbmp_t *hw_link)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int idx;

    for(idx = 0; idx < _SHR_PBMP_WORD_MAX; idx++) {
        SOC_PBMP_WORD_SET(*hw_link, idx, SOC_PBMP_WORD_GET(soc->cmicx_link_stat, idx));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_link_status_update
 * Purpose:
 *      Notify the link status change update from Cortex-M0 to linkscan thread
 * Parameters:
 *      unit number
 *      Linkscan message from cortex-M0
 * Returns:
 *      SOC_E_NONE
 */
int soc_iproc_link_status_update(int unit, soc_iproc_mbox_msg_t *msg)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    soc_pbmp_t phy_pbmp;
    int idx = 0;

    SOC_PBMP_CLEAR(phy_pbmp);

    /* Copy message payload */
    for (idx = 0; idx < _SHR_PBMP_WORD_MAX; idx++) {
        SOC_PBMP_WORD_SET(phy_pbmp, idx, msg->data[idx]);
    }

    /* Convert physical pbmp to logical pbmp */
    _soc_iproc_p2l_pbmp_update(unit, phy_pbmp, &soc->cmicx_link_stat);

#if defined (BCM_ESW_SUPPORT)
    if (SOC_IS_ESW(unit)) {
        /* Inform linkscan thread */
        cmicx_esw_linkscan_hw_interrupt(unit);
    }
#endif

#if defined (BCM_DNXF_SUPPORT)
    if (SOC_IS_DNXF(unit)) {
        /* Inform linkscan thread */
        cmicx_common_linkscan_hw_interrupt(unit);
    }
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_linkscan_process_message
 * Purpose:
 *      Linkscan application handler to handle messages in running state.
 * Parameters:
 *      unit number
 *      Iproc mbox info
 *      Linkscan message from cortex-M0
 * Returns:
 *      SOC_E_xxx
 */
int soc_iproc_linkscan_process_message(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *msg)
{
    int rv = 0;

    if (NULL == msg) {
        return SOC_E_PARAM;
    }

    switch(msg->id) {
        case LS_LINK_STATUS_CHANGE:
            return soc_iproc_link_status_update(chan->unit, msg);
        default:
            LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(chan->unit, "Invalid M0 message id\n")));
            rv = IPROC_MBOX_ERR_INVCMD;
            break;
    }

    return rv;
}

/*
 * Function:
 *      soc_iproc_linkscan_msg_handler
 * Purpose:
 *      Linkscan message handler to handle messages from Cortex-M0 linkscan application.
 * Parameters:
 *      Registered parameter
 * Returns:
 *      SOC_E_xxx
 */
int soc_iproc_linkscan_msg_handler(void *param)
{
    int rv = 0, respidx;
    soc_iproc_mbox_msg_t *msg;
    soc_iproc_mbox_info_t *rxchan = (soc_iproc_mbox_info_t *)param;

    if (rxchan == NULL)
        return SOC_E_PARAM;

    while(!rv) {
        respidx = soc_iproc_data_recv(rxchan, &msg);
        if (respidx < 0) {
            rv = respidx;
            break;
        }

        if ((!IS_IPROC_RESP_READY(msg)) & (!IS_ASYNC_IPROC_STATUS(msg))) {
            soc_iproc_free(msg);
            break;
        } else {
            rv = soc_iproc_linkscan_process_message(rxchan, msg);
            LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(rxchan->unit, " Msg processed %d\n"), msg->id));
            soc_iproc_free(msg);
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_cmicx_linkscan_hw_init
 * Purpose:
 *      Initialize linkscan mbox and handler.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_hw_init(int unit)
{
    int rv = 0;
    soc_control_t   *soc = SOC_CONTROL(unit);

    if (!soc->iproc_m0ls_init_done) {
        SOC_IF_ERROR_RETURN(soc->ls_mbox_id = soc_iproc_mbox_alloc(unit, U0_LINKSCAN_APP));
        soc->ls_txmbox= &soc->iproc_mbox_info[soc->ls_mbox_id][IPROC_MBOX_TYPE_TX];
        soc->ls_rxmbox= &soc->iproc_mbox_info[soc->ls_mbox_id][IPROC_MBOX_TYPE_RX];
        rv = soc_iproc_mbox_handler_register(unit, soc->ls_mbox_id, soc_iproc_linkscan_msg_handler, (void *) soc->ls_rxmbox);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Linkscan Mbox handler register failed\n")));
            soc_iproc_mbox_free(unit, soc->ls_mbox_id);
            return rv;
        }

        soc->iproc_m0ls_init_done = 1;
    }

    return rv;
}

/*
 * Function:
 *      soc_cmicx_linkscan_hw_deinit
 * Purpose:
 *      Cleanup linkscan mbox and handler.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_hw_deinit(int unit)
{
    int rv = 0;
    soc_control_t   *soc = SOC_CONTROL(unit);

    if (soc->iproc_m0ls_init_done) {
        soc_iproc_mbox_free(unit, soc->ls_mbox_id);
        soc->iproc_m0ls_init_done = 0;
    }

    return rv;
}

/*
 * Function:
 *      soc_cmicx_linkscan_msg_id_process
 * Purpose:
 *      Send message to Cortex-M0 linkscan application.
 * Parameters:
 *      unit number
 *      msg_id to be processed
 * Returns:
 *      SOC_E_xxx
 */
static int soc_cmicx_linkscan_msg_id_process(int unit, int msg_id)
{
    soc_iproc_mbox_msg_t *msg, *resp;
    int rv;

    if (!SOC_CONTROL(unit)->iproc_mbox_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox init not done\n")));
        return SOC_E_NONE;
    }

    msg = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(uint32));
    if (msg == NULL)
        return SOC_E_MEMORY;

    resp = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(uint32));
    if (resp == NULL) {
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    msg->id = msg_id;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = 1;
    rv = soc_iproc_data_send_wait(SOC_CONTROL(unit)->ls_txmbox, msg, resp);
    if (rv == IPROC_MSG_SUCCESS) {
        if (IS_IPROC_RESP_READY(resp)) {
            if (IS_IPROC_RESP_SUCCESS(resp)) {
                rv = SOC_E_NONE;
            } else {
                rv = resp->data[0];
            }
        }
    }

    soc_iproc_free(msg);
    soc_iproc_free(resp);

    return rv;
}

/*
 * Function:
 *      soc_cmicx_linkscan_pause
 * Purpose:
 *      Send LS_PAUSE message to Cortex-M0 linkscan application.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_pause(int unit)
{
    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan pause\n")));

    return soc_cmicx_linkscan_msg_id_process(unit, LS_PAUSE);
}

/*
 * Function:
 *      soc_cmicx_linkscan_continue
 * Purpose:
 *      Send LS_CONTINUE message to Cortex-M0 linkscan application.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_continue(int unit)
{
    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan continue\n")));

    return soc_cmicx_linkscan_msg_id_process(unit, LS_CONTINUE);
}

/*
 * Function:
 *       soc_cmicx_linkscan_heartbeat
 * Purpose:
 *       Send LS_HW_HEARTBEAT message to Cortex-M0 linkscan application.
 * Parameters:
 *       unit number
 * Returns:
 *       SOC_E_XXX
 */
int soc_cmicx_linkscan_heartbeat(int unit, soc_ls_heartbeat_t *ls_heartbeat)
{
    soc_iproc_mbox_msg_t *msg, *resp;
    int rv;

    if (!SOC_CONTROL(unit)->iproc_mbox_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox init not done\n")));
        return SOC_E_NONE;
    }

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan heartbeat\n")));

    sal_memset(ls_heartbeat, 0, sizeof(soc_ls_heartbeat_t));

    ls_heartbeat->host_fw_version = (HOST_FW_VER_MAJOR << 16 | HOST_FW_VER_MINOR);

    msg = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_ls_heartbeat_t));
    if (msg == NULL)
        return SOC_E_MEMORY;

    resp = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_ls_heartbeat_t));
    if (resp == NULL) {
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    msg->id = LS_HW_HEARTBEAT;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = sizeof(soc_ls_heartbeat_t) / sizeof(uint32);
    if (sizeof(soc_ls_heartbeat_t) % sizeof(uint32)) {
        msg->size++;
    }

    /* Copy message data */
    sal_memcpy((void *)msg->data, (void *)ls_heartbeat, sizeof(soc_ls_heartbeat_t));

    rv = soc_iproc_data_send_wait(SOC_CONTROL(unit)->ls_txmbox, msg, resp);
    if (rv == IPROC_MSG_SUCCESS) {
        if (IS_IPROC_RESP_READY(resp)) {
            if (IS_IPROC_RESP_SUCCESS(resp)) {
                /* Copy response data */
                sal_memcpy((void *)ls_heartbeat, (void *)resp->data, sizeof(soc_ls_heartbeat_t));
                rv = SOC_E_NONE;
            } else {
                rv = resp->data[0];
            }
        }
    }

    soc_iproc_free(msg);
    soc_iproc_free(resp);

    return rv;
}

/*
 * Function:
 *      soc_cmicx_linkscan_hw_link_get
 * Purpose:
 *      Obtain actual device link status from miim registers
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    int rv;
    soc_pbmp_t phy_hw_pbmp;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan hw link get\n")));

    /* Add CPU port to port bitmap since M0 FW responds without it */
    _soc_iproc_optimized_cpu_port_to_pbmp_add(unit, &phy_hw_pbmp);

    /* Convert physical to logical port numbering */
     _soc_iproc_p2l_pbmp_update(unit, phy_hw_pbmp, hw_link);

    rv = SOC_E_NONE;
    return rv;
}

/*
 * Function:
 *       soc_cmicx_linkscan_config
 * Purpose:
 *       Send LS_HW_CONFIG message to Cortex-M0 linkscan application.
 * Parameters:
 *       unit number
 *       hw_mii_pbm - Port bit map of ports to scan with MIIM registers
 * Returns:
 *       SOC_E_XXX
 */
int soc_cmicx_linkscan_config(int unit, soc_pbmp_t hw_mii_pbm)
{
    soc_iproc_mbox_msg_t *msg, *resp;
    int rv;
    soc_pbmp_t empty_pbmp, phy_mii_pbm;
    int idx = 0;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan config\n")));

    if (!SOC_CONTROL(unit)->iproc_mbox_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox init not done\n")));
        return SOC_E_NONE;
    }

    SOC_PBMP_CLEAR(empty_pbmp);
    if (SOC_PBMP_EQ(hw_mii_pbm, empty_pbmp)) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "HW pbmp is empty..skip\n")));
        return SOC_E_NONE;
    }

    msg = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_pbmp_t));
    if (msg == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "HW Linkscan Mbox msg alloc failed\n")));
        return SOC_E_MEMORY;
    }

    resp = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_pbmp_t));
    if (resp == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "HW Linkscan Mbox resp alloc failed\n")));
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    msg->id = LS_HW_CONFIG;
    msg->flags = IPROC_SYNC_MSG  |IPROC_RESP_REQUIRED;
    msg->size = sizeof(soc_pbmp_t) / sizeof(uint32);
    if (sizeof(soc_pbmp_t) % sizeof(uint32)) {
        msg->size++;
    }

    SOC_PBMP_CLEAR(phy_mii_pbm);

    /* Convert incoming logical pbmp to physical pbmp */
    _soc_iproc_l2p_pbmp_update(unit, hw_mii_pbm, &phy_mii_pbm);

    /* Copy message payload */
    for (idx = 0; idx < _SHR_PBMP_WORD_MAX; idx++) {
        msg->data[idx] = SOC_PBMP_WORD_GET(phy_mii_pbm, idx);
    }

    /* Send message to M0 */
    rv = soc_iproc_data_send_wait(SOC_CONTROL(unit)->ls_txmbox, msg, resp);
    if (rv == IPROC_MSG_SUCCESS) {
        if (IS_IPROC_RESP_READY(resp)) {
            if (IS_IPROC_RESP_SUCCESS(resp)) {
                rv = SOC_E_NONE;
                /* Update linkscan thread with link status for configured port */
                soc_iproc_link_status_update(unit, resp);
            } else {
                rv = resp->data[0];
            }
        }
    }

    soc_iproc_free(msg);
    soc_iproc_free(resp);

    return rv;
}
#endif /*BCM_CMICX_SUPPORT*/
#endif /*BCM_ESW_SUPPORT*/
