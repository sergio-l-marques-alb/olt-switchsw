/*
 * $Id: eav_srp_conndb.h,v 1.1 2011/04/18 17:10:55 mruas Exp $
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
 *
 * Ethernet AV stream reservation protocol connection database
 */
#ifndef EAV_CONNDB_H
#define EAV_CONNDB_H

#include <bcm/types.h>
#include <appl/diag/eav/eav_srp.h>
#include <appl/diag/eav/eav_list.h>

enum { EAV_SRP_WAITING_FOR_RESERVATION, EAV_SRP_RECEIVED };


typedef struct srp_reservation_max_packet_s {
  uint8 dmac[6];
  uint8 smac[6];
  uint32 vlantag;
  uint16 etype;
  
  srp_rsv_header_t rsv;
  srp_resource_requirement_t rr;
  srp_reservation_status_t rstat;
  uint8 reservationHopCount;
  srp_reservation_hop_status_t hops[16];
} __attribute__ ((__packed__)) srp_reservation_max_packet_t;

typedef struct srp_reservation_no_hop_packet_s {
  uint8 dmac[6];
  uint8 smac[6];
  uint32 vlantag;
  uint16 etype;
  
  srp_rsv_header_t rsv;
  srp_resource_requirement_t rr;
  srp_reservation_status_t rstat;
  uint8 reservationHopCount;
} __attribute__ ((__packed__)) srp_reservation_no_hop_packet_t;


typedef struct _srp_entry_s {
  struct list_head list;
  bcm_pbmp_t regpbmp;
  uint16 resvport;
  uint16 state;

  int32 counter;
  uint16 regCount[ EAV_MAX_NUMBER_OF_PORTS ];
  int16 resvTimeCount;

  srp_registration_t reginfo;

  srp_reservation_max_packet_t resv;

  uint16 vlan;
  uint16 resv0;
} _srp_entry_t;

#define _MAXSRPSTREAMS  128

typedef struct _srp_entries_db_s {
  _srp_entry_t entries[_MAXSRPSTREAMS];
  struct list_head free;
  struct list_head used;
  int nused, total;
} _srp_entries_db_t;

void _srp_entries_db_init( _srp_entries_db_t *pdb );
_srp_entry_t * _srp_entries_db_search( _srp_entries_db_t *pdb, unsigned char *pstreamID );
_srp_entry_t * _srp_entries_db_insert( _srp_entries_db_t *pdb, srp_registration_t *psrpreg, uint8 srcport );
int _srp_entries_db_dump_info( _srp_entries_db_t *pdb );
int _srp_entries_db_age_out( _srp_entries_db_t *pdb, int count );
int _srp_entries_db_registration_query( int unit, _srp_entries_db_t *pdb, bcm_mac_t *cpu_mac );

#endif
