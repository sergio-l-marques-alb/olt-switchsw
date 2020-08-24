/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * IFA_LEAP - In-band Flow Analyzer-Low latency Elastic Accelerator Protocol Embedded Application APP interface
 * Purpose: API to configure IFA_LEAP embedded app interface.
 *
 */
#if defined(INCLUDE_IFA)
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/debug.h>
#include <soc/scache.h>
#include <soc/trident3.h>
#include <soc/tomahawk.h>
#include <soc/profile_mem.h>
#include <soc/uc.h>

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <shared/idxres_fl.h>

#include <bcm/module.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/ifaleap.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw_dispatch.h>

STATIC
void bcm_td3_ifa_leap_linked_list(int unit,
                                  _bcm_ifa_leap_gport_port_queue_map_t  *head)
{
    _bcm_ifa_leap_gport_port_queue_map_t  *temp, *newnode;
    temp = head;
    while(temp) {
        newnode = temp;
        temp = temp->next;
        soc_cm_sfree(unit, newnode);
    }
}

int _bcm_td3_ifa_leap_init(int unit)
{
    uint32        sram_base_addr, sram_base_offset;
    uint32        app_status[2], host_status;
    int           i, rv = BCM_E_NONE;

    for (i = 0; i < BCM_IFA_LEAP_M0_NUM_UCS; i++) {
        /* M0 uc SRAM Base Address */
        sram_base_addr       = soc_iproc_percore_membase_get(unit, i);

        /* SRAM first 256Bytes reserved */
        sram_base_offset     = (sram_base_addr + BASEADDR_OFFSET(unit));

        app_status[0] = soc_uc_mem_read(unit,
                (sram_base_addr + BCM_IFA_LEAP_M0_EAPP_STATUS_OFFSET));
        if (app_status[0] != BCM_IFA_LEAP_M0_EAPP_STATUS) {
            continue;
        }

        app_status[0] = soc_uc_mem_read(unit,
                (sram_base_offset + ifa_leap_offset(app_status)));
        app_status[1] = soc_uc_mem_read(unit,
                (sram_base_offset + ifa_leap_offset(app_status) + 4));
        host_status   = soc_uc_mem_read(unit,
                (sram_base_offset + ifa_leap_offset(host_status)));

        /*
         * Validation check whether M0 is loaded with IFA_LEAP APP
         */
        if ((app_status[0] == BCM_IFA_LEAP_APP_STATUS_1) &&
            (app_status[1] == BCM_IFA_LEAP_APP_STATUS_2) &&
            (host_status & BCM_IFA_LEAP_CONFIG_STATUS_BIT)) {
            /* Counter module settings */
            rv = soc_counter_egr_perq_xmt_ctr_evict_disable(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                          "IFA LEAP(unit %d) counter disable failed %s\n"),
                          unit, bcm_errmsg(rv)));
                return rv;
            }
            SOC_CONTROL(unit)->egr_perq_xmt_ctr_evict_disable_ifa = 1;
            break;
        }
    }
    return rv;
}

int _bcm_td3_ifa_leap_config_set(int unit,
                                 bcm_ifa_leap_config_t *config_data)
{
    uint32                   sram_base_addr, sram_base_offset;
    uint32                   app_status[2], ifa_leap_app = 1;
    int                      time_count, index, rv = BCM_E_NONE, reset;
    int                      phy_port, pipe, mmu_port, local_mmu_port, i;
    bcm_port_t               local_port;
    _bcm_td3_cosq_node_t    *node;
    soc_info_t              *si;
    uint32                   port_pipe_queue_num, host_status, fw_status;
    uint32                   num_sbusdma, bufp[SOC_MAX_MEM_FIELD_WORDS] = {0};
    _bcm_ifa_leap_gport_port_queue_map_t *head, *newnode, *temp;

    if (config_data->num_gports > BCM_IFA_LEAP_MAX_NUM_GPORT) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                        "IFA LEAP app support Maximum 64 Queues\n")));
        return BCM_E_PARAM;
    }

    num_sbusdma = soc_property_get(unit, spn_IFA_LEAP_NUM_SBUS_DMA, 2);
    if ((num_sbusdma != 2) && (num_sbusdma != 4)) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA LEAP app support 4 and 2 SBUS DMA number channels\n")));
        return BCM_E_CONFIG;
    }

    for (i = 0; i < BCM_IFA_LEAP_M0_NUM_UCS; i++) {
        /* M0 uc SRAM Base Address */
        sram_base_addr       = soc_iproc_percore_membase_get(unit, i);

        /* SRAM first 256Bytes reserved */
        sram_base_offset     = (sram_base_addr + BASEADDR_OFFSET(unit));

        app_status[0] = soc_uc_mem_read(unit,
                            (sram_base_addr + BCM_IFA_LEAP_M0_EAPP_STATUS_OFFSET));
        if (app_status[0] != BCM_IFA_LEAP_M0_EAPP_STATUS) {
            continue;
        }

        app_status[0] = soc_uc_mem_read(unit,
                            (sram_base_offset + ifa_leap_offset(app_status)));
        app_status[1] = soc_uc_mem_read(unit,
                            (sram_base_offset + ifa_leap_offset(app_status) + 4));

        /*
         * Validation check whether M0 is loaded with IFA_LEAP APP
         */
        if ((app_status[0] == BCM_IFA_LEAP_APP_STATUS_1) &&
            (app_status[1] == BCM_IFA_LEAP_APP_STATUS_2)) {
            if (!soc_iproc_m0ssq_reset_ucore_get(unit, i, &reset)) {
                if (reset) {
                    soc_uc_mem_write(unit,
                                     (sram_base_addr + BCM_IFA_LEAP_M0_EAPP_STATUS_OFFSET),
                                     (uint32)0);
                    continue;
                }
            } else {
                continue;
            }
            ifa_leap_app = 0;
            break;
        }
    }

    if (ifa_leap_app) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "load IFA LEAP APP M0\n")));
        return BCM_E_UNAVAIL;
    }

    host_status = soc_uc_mem_read(unit,
                        (sram_base_offset + ifa_leap_offset(host_status)));

    if (host_status & BCM_IFA_LEAP_CONFIG_STATUS_BIT) {
        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(host_status)),
                         (uint32)0);

        /* Counter module settings */
        if (SOC_CONTROL(unit)->egr_perq_xmt_ctr_evict_disable_ifa) {
            SOC_CONTROL(unit)->egr_perq_xmt_ctr_evict_disable_ifa = 0;
            rv = soc_counter_egr_perq_xmt_ctr_evict_enable(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                          "IFA LEAP(unit %d) Error: counter enable failed %s\n"),
                          unit, bcm_errmsg(rv)));
                return rv;
            }
        }
        time_count = 0;
        while (1) {
            fw_status  = soc_uc_mem_read(unit,
                               (sram_base_offset + ifa_leap_offset(uc_status)));
            if (!(fw_status & BCM_IFA_LEAP_CONFIG_STATUS_BIT)) {
                break;
            }
            ++time_count;
            if ((time_count * BCM_IFA_LEAP_UC_STATUS_TIMEOUT) >
                BCM_IFA_LEAP_UC_MSG_TIMEOUT) {
                LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                                "IFA LEAP app does not responded\n")));
                return BCM_E_TIMEOUT;
            }
            sal_usleep(BCM_IFA_LEAP_UC_STATUS_TIMEOUT);
        }
    }

    if (!config_data->num_gports) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                        "IFA LEAP APP process stopped\n")));
        return BCM_E_NONE;
    }

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(queue_size_tbl_addr)),
                     (uint32)soc_mem_base(unit, MMU_THDU_UCQ_STATS_XPE0_PIPE0m));
    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(queue_size_tbl_addr) + 4),
                     (uint32)soc_mem_base(unit, MMU_THDU_UCQ_STATS_XPE0_PIPE1m));

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(egr_tx_cnt_tbl_addr)),
                     (uint32)soc_mem_base(unit, EGR_PERQ_XMT_COUNTERS_PIPE0m));
    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(egr_tx_cnt_tbl_addr) + 4),
                     (uint32)soc_mem_base(unit, EGR_PERQ_XMT_COUNTERS_PIPE1m));

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(egr_vlan_xlate_tbl_addr)),
                     (uint32)soc_mem_base(unit, EGR_VLAN_XLATE_2_ECCm));

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(queues_per_pipe)),
                     (uint32)(SOC_TD3_NUM_UC_QUEUES_PER_PIPE - 10));

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(queues_per_port)),
                     (uint32)SOC_TD3_NUM_UCAST_QUEUE_PER_PORT);

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(ports_per_pipe)),
                     (uint32)_TD3_MMU_PORTS_PER_PIPE);

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(num_pipes)),
                     (uint32)_TD3_PIPES_PER_DEV);

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(num_sbus_dma)),
                     (uint32)num_sbusdma);

    head = (_bcm_ifa_leap_gport_port_queue_map_t *)0;
    si = &SOC_INFO(unit);

    for (i = 0; i < config_data->num_gports; i++) {
        if (!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(config_data->gport_list[i]))) {
            bcm_td3_ifa_leap_linked_list(unit, head);
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "%x gport is non unicast queue type"
                      "Only uincast type gport are alone monitored\n"),
                      config_data->gport_list[i]));
            return BCM_E_PARAM;
        }
        newnode = (_bcm_ifa_leap_gport_port_queue_map_t *) soc_cm_salloc(unit,
                                                               sizeof(struct _bcm_ifa_leap_gport_port_queue_map_s),
                                                               "microburst temp linked list");

        if (newnode == NULL) {
            bcm_td3_ifa_leap_linked_list(unit, head);
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "Failed to create local memory\n")));
            return BCM_E_MEMORY;
        }
        sal_memset(newnode, 0, sizeof(struct _bcm_ifa_leap_gport_port_queue_map_s));
        newnode->next=(_bcm_ifa_leap_gport_port_queue_map_t *)0;

        rv = _bcm_td3_cosq_node_get(unit, config_data->gport_list[i], NULL, &local_port,
                                    NULL, &node);

        if ((BCM_E_NONE != rv) || (local_port < 0)) {
            bcm_td3_ifa_leap_linked_list(unit, head);
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "gport %x with wrong local port value\n"),
                      config_data->gport_list[i]));
            return rv;
        }

        rv = soc_port_pipe_get(unit, local_port, &pipe);
        if (BCM_E_NONE != rv) {
            bcm_td3_ifa_leap_linked_list(unit, head);
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "gport %x with wrong pipe\n"),
                      config_data->gport_list[i]));
            return rv;
        }

        newnode->queue_num = node->hw_index % SOC_TD3_NUM_UCAST_QUEUE_PER_PORT;
        if (newnode->queue_num >= SOC_TD3_NUM_UCAST_QUEUE_PER_PORT) {
            bcm_td3_ifa_leap_linked_list(unit, head);
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "gport %x with wrong queue number\n"),
                      config_data->gport_list[i]));
            return BCM_E_PORT;
        }

        rv = bcm_esw_port_gport_get(unit, local_port, &newnode->dgpp);
        if (BCM_E_NONE != rv) {
            bcm_td3_ifa_leap_linked_list(unit, head);
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "gport %x with wrong DGPP\n"),
                      config_data->gport_list[i]));
            return rv;
        }

        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];
        local_mmu_port = mmu_port & SOC_TD3_MMU_PORT_STRIDE;

        newnode->pipe_num =  pipe;
        newnode->gport = config_data->gport_list[i];
        newnode->port_num = local_mmu_port;
        newnode->global_num = (newnode->pipe_num * SOC_TD3_NUM_UC_QUEUES_PER_PIPE) +
                               (local_mmu_port * SOC_TD3_NUM_UCAST_QUEUE_PER_PORT) +
                               newnode->queue_num;

        if (head == NULL || (head)->global_num >= newnode->global_num) {
            newnode->next = head;
            head = newnode;
        } else {
            temp = head;
            while ((temp->next != NULL) &&
                   (temp->next->global_num < newnode->global_num)) {
                temp = temp->next;
            }
            newnode->next = temp->next;
            temp->next = newnode;
        }
    }

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(flag)),
                     (uint32)config_data->flag);

    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(num_gports)),
                     (uint32)config_data->num_gports);
    temp = head;
    i = 0;
    while(temp) {
        port_pipe_queue_num = (((temp->port_num & 0xFFFF) << 16) |
                               ((temp->pipe_num & 0xF) << 12) |
                               (temp->queue_num & 0xFFF));

        index = 0;
        sal_memset(bufp, 0, sizeof(uint32) * SOC_MAX_MEM_FIELD_WORDS);
        bufp[0]   =  (28 << 3);
        bufp[0]  |=  ((temp->dgpp & 0xFFFF) << 8);
        if (config_data->flag == BCM_IFA_LEAP_QUEUE_ID_INT_PRI) {
            bufp[0]  |=  ((config_data->int_pri[i] & 0xF) << 24);
        } else if (config_data->flag == BCM_IFA_LEAP_QUEUE_ID_INT_PRI_CLASS_ID) {
            bufp[0]  |=  ((config_data->int_pri[i] & 0xF) << 24);
            bufp[0]  |=  ((config_data->class_id[i] & 0x3) << 28);
        } else {
            bufp[0]  |=  ((temp->queue_num & 0xF) << 24);
        }


        rv = soc_mem_search(unit, EGR_VLAN_XLATE_2_SINGLEm, MEM_BLOCK_ANY, &index, bufp, bufp, 0);
        if (BCM_E_NONE != rv) {
            bcm_td3_ifa_leap_linked_list(unit, head);
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                      "Key %x VLAN_XLATE Entry does not exist\n"),
                      bufp[0]));
            return rv;
        }

        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(egr_vlan_xlate_tbl_offset) + (i * 4)),
                         (uint32)index);

        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(egr_vlan_xlate_tbl_data) + (i * 16)),
                         (uint32)bufp[0]);
        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(egr_vlan_xlate_tbl_data) + (i * 16) + 4),
                         (uint32)bufp[1]);

        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(egr_vlan_xlate_tbl_data) + (i * 16) + 8),
                         (uint32)bufp[2]);

        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(egr_vlan_xlate_tbl_data) + (i * 16) + 12),
                         (uint32)bufp[3]);

        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(dgpp) + (i * 4)),
                         (uint32)temp->dgpp);
        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(gport) + (i * 4)),
                         (uint32)temp->gport);
        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(port_pipe_queue) + (i * 4)),
                         (uint32)port_pipe_queue_num);
        soc_uc_mem_write(unit,
                         (sram_base_offset + ifa_leap_offset(pri_class_id) +  (i * 4)),
                         (uint32)((config_data->int_pri[i] << 8) | config_data->class_id[i]));
        newnode = temp;
        temp = temp->next;
        soc_cm_sfree(unit, newnode);
        i = i + 1;
    }

    /* Counter module settings */
    rv = soc_counter_egr_perq_xmt_ctr_evict_disable(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA LEAP(unit %d) Error: counter disable failed %s\n"),
                  unit, bcm_errmsg(rv)));
        return rv;
    }
    SOC_CONTROL(unit)->egr_perq_xmt_ctr_evict_disable_ifa = 1;

    host_status |= BCM_IFA_LEAP_CONFIG_STATUS_BIT;
    soc_uc_mem_write(unit,
                     (sram_base_offset + ifa_leap_offset(host_status)),
                     host_status);

    time_count = 0;
    while (1) {
        fw_status  = soc_uc_mem_read(unit,
                                     (sram_base_offset + ifa_leap_offset(uc_status)));
        if (fw_status & BCM_IFA_LEAP_CONFIG_STATUS_BIT) {
            break;
        }
        ++time_count;
        if ((time_count * BCM_IFA_LEAP_UC_STATUS_TIMEOUT) >
            BCM_IFA_LEAP_UC_MSG_TIMEOUT) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                            "IFA LEAP app does not responded\n")));
            return BCM_E_TIMEOUT;
        }
        sal_usleep(BCM_IFA_LEAP_UC_STATUS_TIMEOUT);
    }
    return rv;
}

int _bcm_td3_ifa_leap_config_get(int unit,
                                 bcm_ifa_leap_config_t *config_data)
{
    uint32                   sram_base_addr, sram_base_offset, ifa_leap_app = 1;
    uint32                   app_status[2], host_status, fw_status, tmp_val;
    int                      i, rv = BCM_E_NONE;

    for (i = 0; i < BCM_IFA_LEAP_M0_NUM_UCS; i++) {
        /* M0 uc SRAM Base Address */
        sram_base_addr       = soc_iproc_percore_membase_get(unit, i);

        /* SRAM first 256Bytes reserved */
        sram_base_offset     = (sram_base_addr + BASEADDR_OFFSET(unit));

        app_status[0] = soc_uc_mem_read(unit,
                            (sram_base_addr + BCM_IFA_LEAP_M0_EAPP_STATUS_OFFSET));
        if (app_status[0] != BCM_IFA_LEAP_M0_EAPP_STATUS) {
            continue;
        }

        app_status[0] = soc_uc_mem_read(unit,
                            (sram_base_offset + ifa_leap_offset(app_status)));
        app_status[1] = soc_uc_mem_read(unit,
                            (sram_base_offset + ifa_leap_offset(app_status) + 4));

        /*
         * Validation check whether M0 is loaded with IFA_LEAP APP
         */
        if ((app_status[0] == BCM_IFA_LEAP_APP_STATUS_1) &&
            (app_status[1] == BCM_IFA_LEAP_APP_STATUS_2)) {
            ifa_leap_app = 0;
            break;
        }
    }

    if (ifa_leap_app) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "load IFA LEAP APP M0\n")));
        return BCM_E_UNAVAIL;
    }

    host_status = soc_uc_mem_read(unit,
                        (sram_base_offset + ifa_leap_offset(host_status)));

    fw_status = soc_uc_mem_read(unit,
                        (sram_base_offset + ifa_leap_offset(uc_status)));
    if (!((host_status & BCM_IFA_LEAP_CONFIG_STATUS_BIT) &&
          (fw_status   & BCM_IFA_LEAP_CONFIG_STATUS_BIT))) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                        "IFA LEAP APP configuration does not exist\n")));
        return (BCM_E_NOT_FOUND);
    }

    config_data->flag = (uint8)soc_uc_mem_read(unit,
                     (sram_base_offset + ifa_leap_offset(flag)));

    config_data->num_gports = (uint16)soc_uc_mem_read(unit,
                     (sram_base_offset + ifa_leap_offset(num_gports)));
    for (i = 0; i < config_data->num_gports; i++) {
        config_data->gport_list[i] = soc_uc_mem_read(unit,
                         (sram_base_offset + ifa_leap_offset(gport) + (i * 4)));
        tmp_val = soc_uc_mem_read(unit,
                         (sram_base_offset + ifa_leap_offset(pri_class_id) + (i * 4)));
        config_data->int_pri[i] = (uint16)((tmp_val >> 8) & 0xFFFF);
        config_data->class_id[i] = (uint8)(tmp_val & 0xFF);
    }

    return rv;
}
#else
typedef int bcm_make_iso_compilers_happy;
#endif /* INCLUDE_IFA */
