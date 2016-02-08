/* 
 * $Id: mem.h,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        mem.h
 * Purpose:     Include file for memory access functions: ARL, L2X, L3X.
 */

#ifndef _PCID_MEM_H
#define _PCID_MEM_H

#include "pcid.h"
#include <sys/types.h>
#include <soc/mcm/memregs.h>


extern int soc_internal_arl_cnt(pcid_info_t *pcid_info, int copyno, int cnt);
extern int soc_internal_arl_init(pcid_info_t *pcid_info);
extern int soc_internal_arl_ins(pcid_info_t *pcid_info, arl_entry_t *entry);
extern int soc_internal_arl_del(pcid_info_t *pcid_info, arl_entry_t *entry);
extern int soc_internal_arl_lkup(pcid_info_t *pcid_info, int copyno,
                                 arl_entry_t *entry_lookup,
                                 arl_entry_t *entry_result);


#ifdef	BCM_XGS_SWITCH_SUPPORT
extern int soc_internal_l2x_read(pcid_info_t *pcid_info, uint32 addr,
                                 l2x_entry_t *entry);

extern int soc_internal_l2x_write(pcid_info_t *pcid_info, uint32 addr, 
                                  l2x_entry_t *entry);
extern int soc_internal_l2x_lkup(pcid_info_t *pcid_info, 
                                 l2x_entry_t *entry_lookup, uint32 *result);
extern int soc_internal_l2x_init(pcid_info_t * pcid_info) ;
extern int soc_internal_l2x_del(pcid_info_t *pcid_info, l2x_entry_t *entry);
extern int soc_internal_l2x_ins(pcid_info_t *pcid_info, l2x_entry_t *entry);

extern int soc_internal_l3x_read(pcid_info_t *pcid_info, uint32 addr, 
                                 l3x_entry_t *entry);
extern int soc_internal_l3x_write(pcid_info_t *pcid_info, uint32 addr, 
                                  l3x_entry_t *entry);
extern int soc_internal_l3x_init(pcid_info_t * pcid_info) ;

extern int soc_internal_manual_hash(pcid_info_t *pcid_info, uint32 *data);
extern int soc_internal_l3x_del(pcid_info_t *pcid_info, l3x_entry_t *entry);
extern int soc_internal_l3x_ins(pcid_info_t *pcid_info, l3x_entry_t *entry);
#endif	/* BCM_XGS_SWITCH_SUPPORT */
#ifdef	BCM_FIREBOLT_SUPPORT
extern int soc_internal_l2x2_entry_ins(pcid_info_t *pcid_info, uint8 banks,
                                       l2x_entry_t *entry,
                                        uint32 *result);
extern int soc_internal_l2x2_entry_del(pcid_info_t *pcid_info, uint8 banks,
                                       l2x_entry_t *entry,
                                        uint32 *result);
extern int soc_internal_l2x2_entry_lkup(pcid_info_t * pcid_info, uint8 banks,
                                        l2x_entry_t *entry_lookup,
                                        uint32 *result);
extern int soc_internal_l3x2_read(pcid_info_t * pcid_info, soc_mem_t mem,
                                  uint32 addr, uint32 *entry);
extern int soc_internal_l3x2_write(pcid_info_t * pcid_info, soc_mem_t mem,
                                   uint32 addr, uint32 *entry);
extern int soc_internal_l3x2_entry_ins(pcid_info_t *pcid_info, uint8 banks,
                                       l3_entry_ipv6_multicast_entry_t *entry);
extern int soc_internal_l3x2_entry_del(pcid_info_t *pcid_info, uint8 banks,
                                       l3_entry_ipv6_multicast_entry_t *entry);
extern int soc_internal_l3x2_entry_lkup(pcid_info_t * pcid_info, uint8 banks,
                                        l3_entry_ipv6_multicast_entry_t *entry,
                                        uint32 *result);
#endif	/* BCM_FIREBOLT_SUPPORT */
#ifdef	BCM_EASYRIDER_SUPPORT
extern int soc_internal_l2_er_entry_ins(pcid_info_t *pcid_info,
                                        uint32 *entry);
extern int soc_internal_l2_er_entry_del(pcid_info_t *pcid_info,
                                        uint32 *entry);
extern int soc_internal_l2_er_entry_lkup(pcid_info_t * pcid_info,
                                         uint32 *entry);
extern int soc_internal_l3_er_entry_ins(pcid_info_t *pcid_info,
                                        uint32 *entry, int l3v6);
extern int soc_internal_l3_er_entry_del(pcid_info_t *pcid_info,
                                        uint32 *entry, int l3v6);
extern int soc_internal_l3_er_entry_lkup(pcid_info_t * pcid_info,
                                         uint32 *entry, int l3v6);
extern int soc_internal_mcmd_write(pcid_info_t * pcid_info,
                                   soc_mem_t mem, uint32 *data, int cmd);
#endif	/* BCM_EASYRIDER_SUPPORT */
#endif	/* _PCID_MEM_H */
