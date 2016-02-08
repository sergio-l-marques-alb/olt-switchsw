/*
 * $Id: bcm-core-symbols.h,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 */

#include <bcm/debug.h>
#include <bcm/types.h>
#include <bcm/async.h>
#include <bcm/auth.h>
#include <bcm/bcmi2c.h>
#include <bcm/cosq.h>
#include <bcm/custom.h>
#include <bcm/diffserv.h>
#include <bcm/dmux.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/filter.h>
#include <bcm/htls.h>
#include <bcm/igmp.h>
#include <bcm/init.h>
#include <bcm/ipmc.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/link.h>
#include <bcm/mcast.h>
#include <bcm/meter.h>
#include <bcm/mirror.h>
#include <bcm/module.h>
#include <bcm/mpls.h>
#include <bcm/pkt.h>
#include <bcm/port.h>
#include <bcm/proxy.h>
#include <bcm/rate.h>
#include <bcm/rx.h>
#include <bcm/stack.h>
#include <bcm/stat.h>
#include <bcm/stg.h>
#include <bcm/switch.h>
#include <bcm/topo.h>
#include <bcm/trunk.h>
#include <bcm/tx.h>
#include <bcm/vlan.h>

#ifdef INCLUDE_MACSEC
#include <bcm_int/common/macsec_cmn.h>
#endif /* INCLUDE_MACSEC */

#include <sal/core/dpc.h>
#include <appl/diag/system.h>

int kconfig_get_next(char **name, char **value);
char *kconfig_get(const char *name);
int kconfig_set(char *name, char *value);
char *strtok(char *s, const char *delim) { return strsep(&s,delim); }

/* Export all BCM API symbols */
#include <bcm_export.h>

#include <soc/dport.h>

EXPORT_SYMBOL(soc_dport_to_port);
EXPORT_SYMBOL(soc_dport_from_port);
EXPORT_SYMBOL(soc_dport_from_dport_idx);

#include <bcm_int/api_xlate_port.h>

#ifdef INCLUDE_BCM_API_XLATE_PORT

EXPORT_SYMBOL(_bcm_api_xlate_port_a2p);
EXPORT_SYMBOL(_bcm_api_xlate_port_p2a);
EXPORT_SYMBOL(_bcm_api_xlate_port_pbmp_a2p);
EXPORT_SYMBOL(_bcm_api_xlate_port_pbmp_p2a);

#endif /* INCLUDE_BCM_API_XLATE_PORT */

#ifdef BCM_ESW_SUPPORT
#include <soc/triumph.h>
#include <soc/easyrider.h>
#include <soc/i2c.h>
#include <soc/hercules.h>
#include <soc/l3x.h>
#include <soc/xaui.h>
#include <soc/mcm/driver.h>
#include <soc/er_cmdmem.h>
#include <soc/higig.h>
#include <soc/phyctrl.h>

extern int soc_i2c_max127_iterations;
extern int _rlink_nexthop;
extern int  bcm_rlink_start(void);
extern int  bcm_rlink_stop(void);
extern int  bcm_rpc_start(void);
extern int  bcm_rpc_stop(void);
extern void bcm_rpc_dump(void);

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/strata.h>
#include <bcm_int/esw/draco.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/easyrider.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/ipfix.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/control.h>
extern int _rpc_nexthop;
#endif /* BCM_ESW_SUPPORT */ 

#ifdef BCM_SBX_SUPPORT
#include <bcm_int/control.h>
#endif /* def BCM_SBX_SUPPORT */

#ifdef BCM_ROBO_SUPPORT
#include <soc/mcm/robo/driver.h>
extern int _bcm_robo_auth_sec_mode_set(int unit, bcm_port_t port, int mode,
                 int mac_num);
extern void 
_bcm_robo_l2_from_arl(int unit, bcm_l2_addr_t *l2addr, l2_arl_sw_entry_t *arl_entry);
extern void _robo_field_qset_dump(char *prefix, bcm_field_qset_t qset, char* suffix);
#include <bcm_int/control.h>
#include <bcm_int/robo/field.h>
#include <bcm_int/robo/rx.h>
#include <bcm_int/robo/port.h>
#include <bcm_int/robo/trunk.h>
#endif /* BCM_ROBO_SUPPORT */


/* 
 * Export system-related symbols and symbols
 * required by the diag shell .
 * ESW & ROBO both defined
 */

#ifdef BROADCOM_DEBUG
EXPORT_SYMBOL(bcm_debug_print);
EXPORT_SYMBOL(bcm_debug_level);
EXPORT_SYMBOL(bcm_debug_names);
#endif /* BROADCOM_DEBUG */
EXPORT_SYMBOL(bcore_assert_set_default);
EXPORT_SYMBOL(bcore_debug_register);
EXPORT_SYMBOL(bcore_debug_unregister);
EXPORT_SYMBOL(bde);
EXPORT_SYMBOL(_build_date);
EXPORT_SYMBOL(_build_datestamp);
EXPORT_SYMBOL(_build_host);
EXPORT_SYMBOL(_build_release);
EXPORT_SYMBOL(_build_tree);
EXPORT_SYMBOL(_build_user);
EXPORT_SYMBOL(kconfig_get);
EXPORT_SYMBOL(kconfig_get_next);
EXPORT_SYMBOL(kconfig_set);
EXPORT_SYMBOL(phy_port_info);
#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
EXPORT_SYMBOL(sal_alloc_resource_usage_get);
EXPORT_SYMBOL(sal_mutex_resource_usage_get);
EXPORT_SYMBOL(sal_sem_resource_usage_get);
EXPORT_SYMBOL(sal_thread_resource_usage_get);
#endif /* INCLUDE_BCM_SAL_PROFILE */
#endif /* BROADCOM_DEBUG */
EXPORT_SYMBOL(sal_alloc);
EXPORT_SYMBOL(sal_assert_set);
EXPORT_SYMBOL(sal_boot_flags_get);
EXPORT_SYMBOL(sal_boot_flags_set);
EXPORT_SYMBOL(sal_boot_script);
EXPORT_SYMBOL(sal_core_init);
EXPORT_SYMBOL(sal_ctoi);
EXPORT_SYMBOL(sal_dpc_term);
#ifdef sal_free
/* Remove remap from <shared/alloc.h> */
#undef sal_free
#endif
EXPORT_SYMBOL(sal_free);
EXPORT_SYMBOL(sal_free_safe);
EXPORT_SYMBOL(sal_int_context);
#if defined(memcpy)
EXPORT_SYMBOL(sal_memcpy_wrapper);
#endif
EXPORT_SYMBOL(sal_memcmp);
EXPORT_SYMBOL(sal_mutex_create);
EXPORT_SYMBOL(sal_mutex_destroy);
EXPORT_SYMBOL(sal_mutex_give);
EXPORT_SYMBOL(sal_mutex_take);
EXPORT_SYMBOL(sal_os_name);
EXPORT_SYMBOL(sal_sem_create);
EXPORT_SYMBOL(sal_sem_destroy);
EXPORT_SYMBOL(sal_sem_give);
EXPORT_SYMBOL(sal_sem_take);
EXPORT_SYMBOL(sal_sleep);
EXPORT_SYMBOL(sal_snprintf);
EXPORT_SYMBOL(sal_spl);
EXPORT_SYMBOL(sal_splhi);
EXPORT_SYMBOL(sal_sprintf);
EXPORT_SYMBOL(sal_strdup);
EXPORT_SYMBOL(sal_thread_create);
EXPORT_SYMBOL(sal_thread_destroy);
EXPORT_SYMBOL(sal_thread_exit);
EXPORT_SYMBOL(sal_thread_main_get);
EXPORT_SYMBOL(sal_thread_main_set);
EXPORT_SYMBOL(sal_thread_name);
EXPORT_SYMBOL(sal_thread_self);
EXPORT_SYMBOL(sal_time);
EXPORT_SYMBOL(sal_time_usecs);
EXPORT_SYMBOL(sal_udelay);
EXPORT_SYMBOL(sal_usleep);
EXPORT_SYMBOL(shr_avl_count);
EXPORT_SYMBOL(shr_avl_delete);
EXPORT_SYMBOL(shr_avl_delete_all);
EXPORT_SYMBOL(shr_avl_insert);
EXPORT_SYMBOL(shr_avl_lookup);
EXPORT_SYMBOL(shr_avl_traverse);
EXPORT_SYMBOL(_sal_assert);
EXPORT_SYMBOL(_shr_atof_exp10);
EXPORT_SYMBOL(_shr_crc32);
EXPORT_SYMBOL(_shr_ctoa);
EXPORT_SYMBOL(_shr_ctoi);
EXPORT_SYMBOL(_shr_errmsg);
EXPORT_SYMBOL(_shr_pbmp_decode);
EXPORT_SYMBOL(_shr_pbmp_format);
EXPORT_SYMBOL(_shr_popcount);
EXPORT_SYMBOL(_shr_swap32);
EXPORT_SYMBOL(_shr_swap16);
EXPORT_SYMBOL(_soc_mac_all_ones);
EXPORT_SYMBOL(_soc_mac_all_zeroes);
EXPORT_SYMBOL(soc_property_get);
EXPORT_SYMBOL(soc_property_get_pbmp);
EXPORT_SYMBOL(soc_property_get_str);
EXPORT_SYMBOL(soc_property_port_get);
EXPORT_SYMBOL(soc_phy_addr_int_of_port);
EXPORT_SYMBOL(soc_phy_addr_of_port);
EXPORT_SYMBOL(soc_phy_an_timeout_get);
EXPORT_SYMBOL(soc_phy_id0reg_get);
EXPORT_SYMBOL(soc_phy_id1reg_get);
EXPORT_SYMBOL(soc_phy_is_c45_miim);
EXPORT_SYMBOL(soc_phy_name_get);
EXPORT_SYMBOL(soc_cm_debug);
EXPORT_SYMBOL(soc_cm_debug_names);
EXPORT_SYMBOL(soc_cm_device);
EXPORT_SYMBOL(soc_cm_display_known_devices);
EXPORT_SYMBOL(soc_cm_get_id);
EXPORT_SYMBOL(soc_cm_p2l);
EXPORT_SYMBOL(soc_cm_l2p);
EXPORT_SYMBOL(soc_cm_sflush);
EXPORT_SYMBOL(soc_cm_sinval);
EXPORT_SYMBOL(soc_cm_print);
EXPORT_SYMBOL(soc_cm_salloc);
EXPORT_SYMBOL(soc_cm_sfree);
EXPORT_SYMBOL(soc_cm_device_create_id);
EXPORT_SYMBOL(soc_cm_device_destroy);
EXPORT_SYMBOL(soc_cm_device_supported);
#ifdef BROADCOM_DEBUG
EXPORT_SYMBOL(soc_debug_level);
EXPORT_SYMBOL(soc_debug_names);
#endif /* BROADCOM_DEBUG */
EXPORT_SYMBOL(soc_dev_name);
EXPORT_SYMBOL(soc_block_names);
EXPORT_SYMBOL(soc_block_port_names);
EXPORT_SYMBOL(soc_block_name_lookup_ext); 
EXPORT_SYMBOL(soc_misc_init);
EXPORT_SYMBOL(soc_feature_name);
EXPORT_SYMBOL(soc_regtypenames);
EXPORT_SYMBOL(soc_control);
EXPORT_SYMBOL(soc_miimc45_read);
EXPORT_SYMBOL(soc_miimc45_write);
EXPORT_SYMBOL(soc_chip_type_names);
EXPORT_SYMBOL(soc_all_ndev);
EXPORT_SYMBOL(soc_ndev);
EXPORT_SYMBOL(soc_attached);
EXPORT_SYMBOL(soc_timeout_check);
EXPORT_SYMBOL(soc_timeout_init);
EXPORT_SYMBOL(bcm_control);
EXPORT_SYMBOL(system_init);
EXPORT_SYMBOL(soc_miim_read);
EXPORT_SYMBOL(soc_miim_write);
EXPORT_SYMBOL(soc_mem_datamask_get);

/* 
 * Export system-related symbols and symbols
 * required by the diag shell .
 * ESW specific 
 */
 
#ifdef BCM_ESW_SUPPORT
#ifdef BCM_RPC_SUPPORT
EXPORT_SYMBOL(bcm_rlink_stop);
EXPORT_SYMBOL(bcm_rlink_start);
EXPORT_SYMBOL(bcm_rpc_dump);
EXPORT_SYMBOL(bcm_rpc_start);
EXPORT_SYMBOL(bcm_rpc_stop);
#endif /* BCM_RPC_SUPPORT */
EXPORT_SYMBOL(soc_lynx_l3_ecmp_hash);
EXPORT_SYMBOL(soc_mem_field_valid);

#ifdef  BCM_EASYRIDER_SUPPORT
EXPORT_SYMBOL(_bcm_er_l2_from_l2x);
#endif  /* BCM_EASYRIDER_SUPPORT */
EXPORT_SYMBOL(_bcm_fb_l2_from_l2x);

EXPORT_SYMBOL(_bcm_esw_gport_resolve); 
EXPORT_SYMBOL(_bcm_esw_modid_is_local); 
EXPORT_SYMBOL(_bcm_esw_l2_gport_parse); 
#ifdef BCM_FIELD_SUPPORT
EXPORT_SYMBOL(_bcm_field_setup_post_ethertype_udf);
#ifdef BROADCOM_DEBUG
EXPORT_SYMBOL(_field_qset_dump);
#endif /* BROADCOM_DEBUG */
#endif  /* BCM_FIELD_SUPPORT */
EXPORT_SYMBOL(_bcm_l2_from_l2x);
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
EXPORT_SYMBOL(_bcm_tr_l2_from_l2x);
#endif /*BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */
EXPORT_SYMBOL(bcm_ipfix_dump_export_entry);
EXPORT_SYMBOL(bcm_ipfix_export_fifo_control);
#ifdef BCM_RPC_SUPPORT
EXPORT_SYMBOL(_rlink_nexthop);
EXPORT_SYMBOL(_rpc_nexthop);
#endif  /* BCM_RPC_SUPPORT */
#ifdef  BCM_XGS_SWITCH_SUPPORT
EXPORT_SYMBOL(_soc_mem_cmp_l3x_sync);
#endif  /* BCM_XGS_SWITCH_SUPPORT */
#ifdef BCM_RPC_SUPPORT
EXPORT_SYMBOL(cpudb_key_format);
EXPORT_SYMBOL(cpudb_key_parse);
#endif  /* BCM_RPC_SUPPORT */
EXPORT_SYMBOL(mbcm_driver);
EXPORT_SYMBOL(soc_anyreg_read);
EXPORT_SYMBOL(soc_anyreg_write);
EXPORT_SYMBOL(soc_base_driver_table);
EXPORT_SYMBOL(soc_bist);
EXPORT_SYMBOL(soc_chip_dump);
EXPORT_SYMBOL(soc_counter_get);
EXPORT_SYMBOL(soc_counter_get_rate);
EXPORT_SYMBOL(soc_counter_idx_get);
EXPORT_SYMBOL(soc_counter_set32_by_port);
EXPORT_SYMBOL(soc_counter_start);
EXPORT_SYMBOL(soc_counter_stop);
EXPORT_SYMBOL(soc_counter_sync);
EXPORT_SYMBOL(soc_dma_abort_dv);
EXPORT_SYMBOL(soc_dma_chan_config);
EXPORT_SYMBOL(soc_dma_desc_add);
EXPORT_SYMBOL(soc_dma_desc_end_packet);
EXPORT_SYMBOL(soc_dma_dump_dv);
EXPORT_SYMBOL(soc_dma_dump_pkt);
EXPORT_SYMBOL(soc_dma_dv_alloc);
EXPORT_SYMBOL(soc_dma_dv_free);
EXPORT_SYMBOL(soc_dma_dv_reset);
EXPORT_SYMBOL(soc_dma_ether_dump);
EXPORT_SYMBOL(soc_dma_higig_dump);
EXPORT_SYMBOL(soc_dma_init);
EXPORT_SYMBOL(soc_dma_rom_dcb_alloc);
EXPORT_SYMBOL(soc_dma_rom_dcb_free);
EXPORT_SYMBOL(soc_dma_rom_detach);
EXPORT_SYMBOL(soc_dma_rom_init);
EXPORT_SYMBOL(soc_dma_rom_rx_poll);
EXPORT_SYMBOL(soc_dma_rom_tx_poll);
EXPORT_SYMBOL(soc_dma_rom_tx_start);
EXPORT_SYMBOL(soc_dma_start);
EXPORT_SYMBOL(soc_dma_wait);
EXPORT_SYMBOL(soc_draco_hash_get);
EXPORT_SYMBOL(soc_draco_hash_set);
EXPORT_SYMBOL(soc_draco_l2_hash);
EXPORT_SYMBOL(soc_draco_l2x_param_to_key);
EXPORT_SYMBOL(soc_draco_l3_ecmp_hash);
EXPORT_SYMBOL(soc_draco_l3_hash);
EXPORT_SYMBOL(soc_draco_l3x_param_to_key);
#ifdef BROADCOM_DEBUG
EXPORT_SYMBOL(soc_dump);
#endif /* BROADCOM_DEBUG */
#ifdef  BCM_EASYRIDER_SUPPORT
EXPORT_SYMBOL(soc_easyrider_ext_sram_enable_set);
EXPORT_SYMBOL(soc_easyrider_ext_sram_op);
EXPORT_SYMBOL(soc_er_celldata_chunk_read);
EXPORT_SYMBOL(soc_er_celldata_chunk_to_slice);
EXPORT_SYMBOL(soc_er_celldata_chunk_write);
EXPORT_SYMBOL(soc_er_celldata_columns_to_slice);
EXPORT_SYMBOL(soc_er_celldata_slice_to_chunk);
EXPORT_SYMBOL(soc_er_celldata_slice_to_columns);
EXPORT_SYMBOL(soc_er_dual_dimensional_array_alloc);
EXPORT_SYMBOL(soc_er_dual_dimensional_array_free);
EXPORT_SYMBOL(soc_er_fp_ext_read);
EXPORT_SYMBOL(soc_er_fp_ext_write);
EXPORT_SYMBOL(soc_er_fp_tcam_ext_read);
EXPORT_SYMBOL(soc_er_fp_tcam_ext_write);
EXPORT_SYMBOL(soc_er_l2_hash);
EXPORT_SYMBOL(soc_er_l2_traverse);
EXPORT_SYMBOL(soc_er_l3v4_base_entry_to_key);
EXPORT_SYMBOL(soc_er_l3v4_hash);
EXPORT_SYMBOL(soc_er_l3v6_base_entry_to_key);
EXPORT_SYMBOL(soc_er_l3v6_hash);
#endif  /* BCM_EASYRIDER_SUPPORT */
#ifdef  BCM_FIREBOLT_SUPPORT
EXPORT_SYMBOL(soc_fb_l2_hash);
EXPORT_SYMBOL(soc_fb_l2x_bank_insert);
EXPORT_SYMBOL(soc_fb_l2x_bank_delete);
EXPORT_SYMBOL(soc_fb_l2x_bank_lookup);
EXPORT_SYMBOL(soc_fb_l3x_bank_delete);
EXPORT_SYMBOL(soc_fb_l3x_bank_insert);
EXPORT_SYMBOL(soc_fb_l3x_bank_lookup);
EXPORT_SYMBOL(soc_fb_l3x_base_entry_to_key);
#endif  /* BCM_FIREBOLT_SUPPORT */
EXPORT_SYMBOL(soc_fb_l3_hash);

#if !defined(SOC_NO_NAMES)
EXPORT_SYMBOL(soc_fieldnames);
#endif /* !defined(SOC_NO_NAMES) */
#ifdef BCM_HERCULES_SUPPORT
EXPORT_SYMBOL(soc_hercules15_mmu_limits_config);
EXPORT_SYMBOL(soc_hercules_mem_read_word);
EXPORT_SYMBOL(soc_hercules_mem_write_word);
EXPORT_SYMBOL(soc_hercules_mmu_limits_config);
#endif  /* BCM_HERCULES_SUPPORT */
#ifdef BCM_XGS_SUPPORT
EXPORT_SYMBOL(soc_higig_dump);
EXPORT_SYMBOL(soc_higig_field_set);
#endif /* BCM_XGS_SUPPORT */
#ifdef INCLUDE_I2C
EXPORT_SYMBOL(soc_i2c_attach);
EXPORT_SYMBOL(soc_i2c_clear_log);
EXPORT_SYMBOL(soc_i2c_device);
EXPORT_SYMBOL(soc_i2c_device_count);
EXPORT_SYMBOL(soc_i2c_devname);
EXPORT_SYMBOL(soc_i2c_is_attached);
EXPORT_SYMBOL(soc_i2c_lm75_monitor);
EXPORT_SYMBOL(soc_i2c_lm75_temperature_show);
EXPORT_SYMBOL(soc_i2c_max127_iterations);
EXPORT_SYMBOL(soc_i2c_probe);
EXPORT_SYMBOL(soc_i2c_reset);
EXPORT_SYMBOL(soc_i2c_show);
EXPORT_SYMBOL(soc_i2c_show_log);
EXPORT_SYMBOL(soc_i2c_show_speeds);
EXPORT_SYMBOL(soc_i2c_device_present);
EXPORT_SYMBOL(soc_i2c_saddr_to_string);
#endif /* INCLUDE_I2C */
EXPORT_SYMBOL(soc_detach);
EXPORT_SYMBOL(soc_init);
EXPORT_SYMBOL(soc_intr_disable);
EXPORT_SYMBOL(soc_intr_enable);
#ifdef BCM_XGS_SWITCH_SUPPORT
EXPORT_SYMBOL(soc_l2x_entry_compare_key);
EXPORT_SYMBOL(soc_l2x_entry_dump);
EXPORT_SYMBOL(soc_l2x_software_hash);
EXPORT_SYMBOL(soc_l2x_start);
EXPORT_SYMBOL(soc_l2x_stop);
EXPORT_SYMBOL(soc_l3x_software_hash);
#endif  /* BCM_XGS_SWITCH_SUPPORT */
EXPORT_SYMBOL(_soc_mem_entry_null_zeroes);
EXPORT_SYMBOL(soc_mem_addr);
EXPORT_SYMBOL(soc_mem_cache_get);
EXPORT_SYMBOL(soc_mem_cache_set);
EXPORT_SYMBOL(soc_mem_cfap_init);
EXPORT_SYMBOL(soc_mem_clear);
EXPORT_SYMBOL(soc_mem_debug_get);
EXPORT_SYMBOL(soc_mem_debug_set);
EXPORT_SYMBOL(soc_mem_delete);
EXPORT_SYMBOL(soc_mem_delete_index);
#if !defined(SOC_NO_DESC)
EXPORT_SYMBOL(soc_mem_desc);
#endif /* !defined(SOC_NO_DESC) */
EXPORT_SYMBOL(soc_mem_dmaable);
EXPORT_SYMBOL(soc_mem_entries);
EXPORT_SYMBOL(soc_mem_entry_dump);
EXPORT_SYMBOL(soc_mem_entry_dump_if_changed);
EXPORT_SYMBOL(soc_mem_field32_get);
EXPORT_SYMBOL(soc_mem_field32_set);
EXPORT_SYMBOL(soc_mem_field_get);
EXPORT_SYMBOL(soc_mem_field_length);
EXPORT_SYMBOL(soc_mem_field_set);
EXPORT_SYMBOL(soc_mem_generic_delete);
EXPORT_SYMBOL(soc_mem_generic_insert);
EXPORT_SYMBOL(soc_mem_generic_lookup);
EXPORT_SYMBOL(soc_mem_index_last);
EXPORT_SYMBOL(soc_mem_insert);
EXPORT_SYMBOL(soc_mem_ip6_addr_set);
EXPORT_SYMBOL(soc_mem_iterate);
#ifdef BCM_HERCULES_SUPPORT
EXPORT_SYMBOL(soc_mem_lla_init);
#endif /* BCM_HERCULES_SUPPORT */
EXPORT_SYMBOL(soc_mem_pbmp_field_get);
EXPORT_SYMBOL(soc_mem_pbmp_field_set);
EXPORT_SYMBOL(soc_mem_pop);
EXPORT_SYMBOL(soc_mem_push);
EXPORT_SYMBOL(soc_mem_read);
EXPORT_SYMBOL(soc_mem_read_range);
#ifdef INCLUDE_MEM_SCAN
EXPORT_SYMBOL(soc_mem_scan_running);
EXPORT_SYMBOL(soc_mem_scan_start);
EXPORT_SYMBOL(soc_mem_scan_stop);
#endif /* INCLUDE_MEM_SCAN */
EXPORT_SYMBOL(soc_mem_search);
#ifdef BCM_EASYRIDER_SUPPORT
EXPORT_SYMBOL(soc_mem_tcam_op);
#endif  /* BCM_EASYRIDER_SUPPORT */
#if !defined(SOC_NO_NAMES)
EXPORT_SYMBOL(soc_mem_name);
EXPORT_SYMBOL(soc_mem_ufalias);
EXPORT_SYMBOL(soc_mem_ufname);
#endif /* !defined(SOC_NO_NAMES) */
EXPORT_SYMBOL(soc_mem_snoop_register);
EXPORT_SYMBOL(soc_mem_snoop_unregister);
EXPORT_SYMBOL(soc_mem_write);
EXPORT_SYMBOL(soc_mem_write_range);
EXPORT_SYMBOL(soc_mmu_init);
EXPORT_SYMBOL(soc_pci_off2name);
EXPORT_SYMBOL(soc_persist);
EXPORT_SYMBOL(soc_port_cmap_get);
EXPORT_SYMBOL(soc_port_ability_to_mode);
EXPORT_SYMBOL(soc_port_mode_to_ability);
EXPORT_SYMBOL(soc_reg32_read);
EXPORT_SYMBOL(soc_reg32_write);
EXPORT_SYMBOL(soc_reg64_field_get);
EXPORT_SYMBOL(soc_reg64_field_set);
EXPORT_SYMBOL(soc_reg64_read);
EXPORT_SYMBOL(soc_reg_addr);
#if !defined(SOC_NO_ALIAS)
EXPORT_SYMBOL(soc_reg_alias);
#endif /* !defined(SOC_NO_ALIAS) */
#if !defined(SOC_NO_DESC)
EXPORT_SYMBOL(soc_reg_desc);
#endif /* !defined(SOC_NO_DESC) */
EXPORT_SYMBOL(soc_reg_field_get);
EXPORT_SYMBOL(soc_reg_field_set);
EXPORT_SYMBOL(soc_reg_iterate);
#if !defined(SOC_NO_NAMES)
EXPORT_SYMBOL(soc_reg_name);
#endif /* !defined(SOC_NO_NAMES) */
EXPORT_SYMBOL(soc_reg_sprint_addr);
EXPORT_SYMBOL(soc_regaddrinfo_get);
EXPORT_SYMBOL(soc_regaddrlist_alloc);
EXPORT_SYMBOL(soc_regaddrlist_free);
EXPORT_SYMBOL(soc_reset);
EXPORT_SYMBOL(soc_reset_init);
EXPORT_SYMBOL(soc_schan_op);
EXPORT_SYMBOL(soc_tcam_init);
EXPORT_SYMBOL(soc_tcam_get_info);
EXPORT_SYMBOL(soc_tcam_mem_index_to_raw_index);
EXPORT_SYMBOL(soc_tcam_part_index_to_mem_index);
EXPORT_SYMBOL(soc_tcam_read_dbreg);
EXPORT_SYMBOL(soc_tcam_read_entry);
EXPORT_SYMBOL(soc_tcam_read_ima);
EXPORT_SYMBOL(soc_tcam_search_entry);
EXPORT_SYMBOL(soc_tcam_set_valid);
EXPORT_SYMBOL(soc_tcam_write_dbreg);
EXPORT_SYMBOL(soc_tcam_write_entry);
EXPORT_SYMBOL(soc_tcam_write_ib);
EXPORT_SYMBOL(soc_tcam_write_ima);
#ifdef BCM_TRIUMPH_SUPPORT
EXPORT_SYMBOL(soc_tr_l2x_hash);
EXPORT_SYMBOL(soc_tr_l2x_param_to_key);
EXPORT_SYMBOL(soc_triumph_ext_sram_enable_set);
EXPORT_SYMBOL(soc_triumph_ext_sram_bist_setup);
EXPORT_SYMBOL(soc_triumph_ext_sram_op);
EXPORT_SYMBOL(soc_triumph_tcam_search_bist);
#endif /* BCM_TRIUMPH_SUPPORT */
EXPORT_SYMBOL(soc_tucana_l3_ecmp_hash);
EXPORT_SYMBOL(soc_xaui_config_get);
EXPORT_SYMBOL(soc_xaui_config_set);
EXPORT_SYMBOL(soc_xaui_rxbert_bit_err_count_get);
EXPORT_SYMBOL(soc_xaui_rxbert_byte_count_get);
EXPORT_SYMBOL(soc_xaui_rxbert_byte_err_count_get);
EXPORT_SYMBOL(soc_xaui_rxbert_enable);
EXPORT_SYMBOL(soc_xaui_rxbert_pkt_count_get);
EXPORT_SYMBOL(soc_xaui_rxbert_pkt_err_count_get);
EXPORT_SYMBOL(soc_xaui_txbert_byte_count_get);
EXPORT_SYMBOL(soc_xaui_txbert_enable);
EXPORT_SYMBOL(soc_xaui_txbert_pkt_count_get);
EXPORT_SYMBOL(soc_phy_list_get);
#ifdef BCM_XGS3_SWITCH_SUPPORT
EXPORT_SYMBOL(soc_xgs3_l3_ecmp_hash);
#endif /* BCM_XGS3_SWITCH_SUPPORT */
#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_LIB_CPUDB
EXPORT_SYMBOL(tks_debug_level);
EXPORT_SYMBOL(tks_debug_names);
#endif /* INCLUDE_LIB_CPUDB */
#endif /* BROADCOM_DEBUG */

#endif /* BCM_ESW_SUPPORT */ 

/* 
 * Export system-related symbols and symbols
 * required by the diag shell .
 * ROBO specific 
 */

#ifdef BCM_ROBO_SUPPORT
EXPORT_SYMBOL(soc_robo_base_driver_table);
EXPORT_SYMBOL(soc_robo_dma_desc_add);
EXPORT_SYMBOL(soc_robo_counter_idx_get);
EXPORT_SYMBOL(soc_robo_dma_dump_dv);
EXPORT_SYMBOL(soc_robo_anyreg_write);
EXPORT_SYMBOL(soc_robo_dma_dv_free);
EXPORT_SYMBOL(soc_robo_port_cmap_get);
EXPORT_SYMBOL(soc_robo_counter_set32_by_port);
EXPORT_SYMBOL(soc_robo_mem_ufname);
EXPORT_SYMBOL(soc_robo_reg_name);
EXPORT_SYMBOL(soc_robo_dma_dump_pkt);
EXPORT_SYMBOL(soc_robo_mem_ufalias);
EXPORT_SYMBOL(soc_robo_reset_init);
EXPORT_SYMBOL(soc_robo_regaddrlist_free);
EXPORT_SYMBOL(soc_robo_dma_start);
EXPORT_SYMBOL(soc_robo_counter_get_rate);
EXPORT_SYMBOL(soc_robo_mem_debug_get);
EXPORT_SYMBOL(soc_robo_counter_start);
EXPORT_SYMBOL(soc_robo_anyreg_read);
EXPORT_SYMBOL(soc_robo_mem_name);
EXPORT_SYMBOL(soc_robo_reg_desc);
EXPORT_SYMBOL(soc_robo_regaddrlist_alloc);
EXPORT_SYMBOL(soc_robo_reg_iterate);
EXPORT_SYMBOL(soc_robo_chip_dump);
EXPORT_SYMBOL(soc_robo_reg_sprint_addr);
EXPORT_SYMBOL(soc_robo_mem_desc);
EXPORT_SYMBOL(soc_robo_regaddrinfo_get);
EXPORT_SYMBOL(soc_robo_fieldnames);
EXPORT_SYMBOL(soc_robo_reg_alias);
EXPORT_SYMBOL(soc_robo_dump);
EXPORT_SYMBOL(soc_robo_dma_ether_dump);
EXPORT_SYMBOL(soc_robo_dma_dv_alloc);
EXPORT_SYMBOL(soc_robo_counter_stop);
EXPORT_SYMBOL(soc_robo_counter_get);
EXPORT_SYMBOL(bcm_attach_check);
EXPORT_SYMBOL(_bcm_robo_auth_sec_mode_set);
EXPORT_SYMBOL(bcm_mcast_addr_t_init);
EXPORT_SYMBOL(bcm_module_name);
EXPORT_SYMBOL(bcm_find);
EXPORT_SYMBOL(_robo_field_qset_dump);
EXPORT_SYMBOL(bcm_rx_debug);
EXPORT_SYMBOL(bcm_l2_addr_t_init);
EXPORT_SYMBOL(bcm_robo_port_phy_power_mode_set);
EXPORT_SYMBOL(bcm_attach);
EXPORT_SYMBOL(bcm_robo_port_phy_power_mode_get);
EXPORT_SYMBOL(_bcm_robo_l2_from_arl);
EXPORT_SYMBOL(bcm_detach);
EXPORT_SYMBOL(bcm5324_trunk_patch_linkscan);
EXPORT_SYMBOL(soc_robo_counter_prev_get);
EXPORT_SYMBOL(soc_robo_counter_prev_set);
EXPORT_SYMBOL(bcm_l2_cache_addr_t_init);
EXPORT_SYMBOL(_robo_field_thread_stop);
#endif /* BCM_ROBO_SUPPORT */

#ifdef INCLUDE_MACSEC
EXPORT_SYMBOL(bcm_common_macsec_config_print);
#endif /* INCLUDE_MACSEC */
