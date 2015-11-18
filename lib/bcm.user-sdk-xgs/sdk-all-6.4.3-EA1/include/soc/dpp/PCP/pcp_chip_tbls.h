/* $Id: pcp_chip_tbls.h,v 1.7 Broadcom SDK $
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
*/

#ifndef __SOC_PCP_CHIP_TBLS_INCLUDED__
/* { */
#define __SOC_PCP_CHIP_TBLS_INCLUDED__


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
 
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>

#include <soc/dpp/PCP/pcp_chip_defines.h>
#include <soc/dpp/PCP/pcp_framework.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* block index and mask definitions { */
#define PCP_ECI_MASK    (PCP_ECI_ID << SOC_SAND_MODULE_SHIFT)

#define PCP_ELK_MASK    (PCP_ELK_ID << SOC_SAND_MODULE_SHIFT)

#define PCP_OAM_MASK    (PCP_OAM_ID << SOC_SAND_MODULE_SHIFT)

#define PCP_STS_MASK    (PCP_STS_ID << SOC_SAND_MODULE_SHIFT)


/* block index and mask definitions } */

#define PCP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED              (2)

/* } */

/*************
 *  MACROS   *
 *************/

#define PCP_WIDTH_BITS_TO_WIDTH_BYTES(width_bits)                          \
          SOC_SAND_DIV_ROUND_UP(width_bits, SOC_SAND_NOF_BITS_IN_UINT32) * sizeof(uint32)

/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  uint32       base;       /* table start address */
  uint32       size;       /* table size (number of lines) */
  uint16       width_bits; /* the width of each line in bits */
} __ATTRIBUTE_PACKED__ PCP_TBL_ADDR;

/* Blocks definition { */

  /* Qdr:                                                           */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x00000000 */

   /* Qdr_data:                                                    */
   /* range: 17:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD qdr_data;

} __ATTRIBUTE_PACKED__ PCP_ECI_QDR_TBL;


  /* Rld1:                                                          */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x00800000 */

   /* Rld1_data:                                                   */
   /* range: 71:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD rld1_data;

} __ATTRIBUTE_PACKED__ PCP_ECI_RLD1_TBL;


  /* Rld2:                                                          */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x01000000 */

   /* Rld2_data:                                                   */
   /* range: 71:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD rld3_data;

} __ATTRIBUTE_PACKED__ PCP_ECI_RLD3_TBL;


/* Rld3:                                                          */
typedef struct
{
  PCP_TBL_ADDR addr;   /* 0x01000000 */

  /* Rld2_data:                                                   */
  /* range: 71:0, access type: RW, default value: 0x0             */
  PCP_TBL_FIELD rld2_data;

} __ATTRIBUTE_PACKED__ PCP_ECI_RLD2_TBL;

  /* Fid Counter Profile Db: VSI limit counters profile             */
  /* database.                                                      */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x00000000 */

   /* Limit: Limit on the number of entries in the MACT            */
   /* range: 12:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD limit;

   /* InterruptEn: if set, an interrupt will flash on limit        */
   /* reach                                                        */
   /* range: 13:13, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD interrupt_en;

   /* LimitEn: if set, the limit check is enabled for this         */
   /* profile.                                                     */
   /* range: 14:14, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD limit_en;

   /* IsLlFid: the event self is mapped to this value              */
   /* range: 15:15, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD is_ll_fid;

} __ATTRIBUTE_PACKED__ PCP_ELK_FID_COUNTER_PROFILE_DB_TBL;


  /* Fid Counter Db: VSI limit counters database.                   */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x00800000 */

   /* EntryCount: Holds the current number of entries with         */
   /* FID=index in the MACT                                        */
   /* range: 12:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD entry_count;

   /* ProfilePointer: Pointer to one of 8 possible profiles        */
   /* range: 15:13, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD profile_pointer;

} __ATTRIBUTE_PACKED__ PCP_ELK_FID_COUNTER_DB_TBL;


  /* Sys Port Is Mine: Port is mine database                        */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x01000000 */

   /* PortIsMine:                                                  */
   /* range: 0:0, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD port_is_mine;

} __ATTRIBUTE_PACKED__ PCP_ELK_SYS_PORT_IS_MINE_TBL;


  /* Aging Cfg Table: age configuration database                    */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x01800000 */

   /* AgingCfgInfoDeleteEntry: if set, Delete entry from MACT              */
   /* range: 0:0, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD aging_cfg_info_delete_entry[PCP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

   /* AgingCfgInfoCreateAgedOutEvent: if set, Create 'Aged-out Event'      */
   /* range: 1:1, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD aging_cfg_info_create_aged_out_event[PCP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

   /* AgingCfgInfoCreateRefreshEvent: if set, Create 'Refresh Event'       */
   /* range: 2:2, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD aging_cfg_info_create_refresh_event[PCP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];

} __ATTRIBUTE_PACKED__ PCP_ELK_AGING_CFG_TABLE_TBL;


  /* Flush DB: age configuration database                           */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x02000000 */

   /* CompareValid:                                                */
   /* range: 0:0, access type: UNDEF, default value: 0x0           */
   PCP_TBL_FIELD compare_valid;

   /* Key_fid:                                                     */
   /* range: 15:1, access type: UNDEF, default value: 0x0          */
   PCP_TBL_FIELD key_fid;

   /* key-mask:                                                    */
   /* range: 30:16, access type: UNDEF, default value: 0x0         */
   PCP_TBL_FIELD key_mask;

   /* payload:                                                     */
   /* range: 71:31, access type: UNDEF, default value: 0x0         */
   PCP_TBL_FIELD payload;

   /* payload-mask:                                                */
   /* range: 112:72, access type: UNDEF, default value: 0x0        */
   PCP_TBL_FIELD payload_mask;

   /* drop:                                                        */
   /* range: 113:113, access type: UNDEF, default value: 0x0       */
   PCP_TBL_FIELD drop;

   /* NewPayload:                                                  */
   /* range: 154:114, access type: UNDEF, default value: 0x0       */
   PCP_TBL_FIELD new_payload;

   /* NewPayloadMask:                                              */
   /* range: 195:155, access type: UNDEF, default value: 0x0       */
   PCP_TBL_FIELD new_payload_mask;

} __ATTRIBUTE_PACKED__ PCP_ELK_FLUSH_DB_TBL;

  /* DspEventTable: Determines destination of the event read.      */
typedef struct
{
  
   PCP_TBL_ADDR addr;   /* 0x02800000 */

   /* to_lpbck: write entry to the DSPR FIFO for the loopback */
   /* range: 0:0, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD to_lpbck;

   /* to_fifo2: write fifo to the DPSG FIFO2 for DSP2 */
   /* range: 1:1, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD to_fifo2;

   /* to_fifo1: write fifo to the DPSG FIFO2 for DSP2 */
   /* range: 2:2, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD to_fifo1;   
} __ATTRIBUTE_PACKED__ PCP_ELK_DSP_EVENT_TABLE_TBL;

  /* PR2 TCDP: Map \{cos profile(2), pr(3)\} to TD,DP               */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x00000000 */

   /* TC: TC to put in ITMH                                        */
   /* range: 2:0, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD tc;

   /* DP: DP to put in ITMH                                        */
   /* range: 4:3, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD dp;

} __ATTRIBUTE_PACKED__ PCP_OAM_PR2_TCDP_TBL;


  /* Ext MEP index DB: MEP_index_DB used to map AC and level        */
  /* to MEP_DB. Key=\{Up/down(1),AC(14),level(3)\}                  */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x00080000 */

   /* mep_db_ptr: Index to MEP DB                                  */
   /* range: 11:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD mep_db_ptr;

   /* accelerate: 1' accelerated MEP; '0' non-accelerated MEP      */
   /* range: 12:12, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD accelerate;

} __ATTRIBUTE_PACKED__ PCP_OAM_EXT_MEP_INDEX_DB_TBL;


  /* Ext MEP DB: External memory ExtMEP_DB. Note: the address       */
  /* is irrelevant, the data is divided into four 18 bit            */
  /* units, each in different bank, msb at lower address            */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x000c0000 */

   /* out_ac: in down push out_ac in EEP Ext                       */
   /* range: 13:0, access type: UNDEF, default value: 0x0          */
   PCP_TBL_FIELD out_ac;

   /* MEPID: lsb of MEPID of TX packet                             */
   /* range: 26:14, access type: UNDEF, default value: 0x0         */
   PCP_TBL_FIELD mepid;

   /* VID_vld: push VID to TX packet                               */
   /* range: 27:27, access type: UNDEF, default value: 0x0         */
   PCP_TBL_FIELD vid_vld;

   /* VID: VID of TX packet                                        */
   /* range: 39:28, access type: UNDEF, default value: 0x0         */
   PCP_TBL_FIELD vid;

   /* PTC_Header: User defined outer most header                   */
   /* range: 71:40, access type: UNDEF, default value: 0x0         */
   PCP_TBL_FIELD ptc_header;

} __ATTRIBUTE_PACKED__ PCP_OAM_EXT_MEP_DB_TBL;


  /* MEP DB: MEP_DB used for CFM TX and check integrity of          */
  /* CFM RX                                                         */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x00800000 */

   /* CCM_interval: Transmission interval described in ITU         */
   /* Y.1731                                                       */
   /* range: 2:0, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD ccm_interval;

   /* MAID: MEP association ID                                     */
   /* range: 18:3, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD maid;

   /* ccmTxStart: CCM start phase for scheduler                    */
   /* range: 28:19, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD ccm_tx_start;

   /* DM_enable: Enable TX and RX of DM messages                   */
   /* range: 29:29, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD dm_enable;

   /* DM_DB_index: DM table index                                  */
   /* range: 36:30, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD dm_db_index;

   /* LM_enable: Loss measurement over CCM is enabled on this      */
   /* MEP                                                          */
   /* range: 37:37, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD lm_enable;

   /* LMM_enable: LMM is enabled on this MEP                       */
   /* range: 38:38, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD lmm_enable;

   /* LMR_enable: for lmm farend idetification                       */
   /* range: 39:39, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD lmr_enable;

   /* LM_DB_index: LM table index                                  */
   /* range: 46:40, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD lm_db_index;

   /* destination: system destination (system port / MC ID /       */
   /* Flow ID)                                                     */
   /* range: 62:47, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD destination;

   /* CCM pr: priority of TX packet                                */
   /* range: 65:63, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD ccm_pr;

   /* up1down0: up/down MEP                                        */
   /* range: 66:66, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD up1down0;

   /* SA_lsb: lsb bits of the MAC SA                               */
   /* range: 73:67, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD sa_lsb;

   /* RDI_indicator: RDI                                           */
   /* range: 74:74, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD rdi_indicator;

   /* cos profile: COS profile for TC, DP mapping                  */
   /* range: 76:75, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD cos_profile;

   /* MD_level: MD level of TX packet                              */
   /* range: 79:77, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD md_level;

} __ATTRIBUTE_PACKED__ PCP_OAM_MEP_DB_TBL;


  /* RMEP DB: Remote MEP database. Updated by incoming              */
  /* packet. Used by scanner to detect timeout condition            */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x01000000 */

   /* CCM_interval: Transmission interval described in ITU         */
   /* Y.1731                                                       */
   /* range: 2:0, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD ccm_interval;

   /* CCM_defect: timeout                                          */
   /* range: 3:3, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD ccm_defect;

   /* rcvd_RDI: received RDI from RX packet                        */
   /* range: 4:4, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD rcvd_rdi;

} __ATTRIBUTE_PACKED__ PCP_OAM_RMEP_DB_TBL;


  /* RMEP HASH 0 DB: Table that maps the hash code to RMEP DB       */
  /* pointer                                                        */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x01800000 */

   /* rmepDbPtr0: Pointer to RMEP DB                               */
   /* range: 12:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD rmep_db_ptr0;

   /* verifier0: Verifier field to verify that the hash belong     */
   /* to our key                                                   */
   /* range: 24:13, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD verifier0;

   /* validHash0: Entry valid bit                                  */
   /* range: 25:25, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD valid_hash0;

} __ATTRIBUTE_PACKED__ PCP_OAM_RMEP_HASH_0_DB_TBL;


  /* RMEP HASH 1 DB: Table that map the hash code to RMEP DB        */
  /* pointer                                                        */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x02000000 */

   /* rmepDbPtr1: Pointer to RMEP DB                               */
   /* range: 12:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD rmep_db_ptr1;

   /* verifier1: Verifier field to verify that the hash belong     */
   /* to our key                                                   */
   /* range: 24:13, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD verifier1;

   /* validHash1: Entry valid bit                                  */
   /* range: 25:25, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD valid_hash1;

} __ATTRIBUTE_PACKED__ PCP_OAM_RMEP_HASH_1_DB_TBL;


  /* LMDB CMN: Internal Loss Measurement DB                         */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x02800000 */

   /* TxFCfC: Carries the value of TxFCb field in the last CCM     */
   /* received from the peer MEP. Updated whenever CCM LM          */
   /* packet arrive from the peer MEP                              */
   /* range: 31:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD tx_fcf_c;

   /* TxFCfP: Carries the value of TxFCf[c] when the last CCM      */
   /* received from the peer MEP. Updated whenever CCM LM          */
   /* packet arrive from the peer MEP                              */
   /* range: 63:32, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD tx_fcf_p;

   /* RxFCbPeerC: Carries the value of RxFCb field in the last     */
   /* CCM received from the peer MEP. Updated whenever CCM LM      */
   /* packet arrive from the peer MEP                              */
   /* range: 95:64, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD rx_fcb_peer_c;

   /* RxFCbPeerP: Carries the value of RxFCb.peer[c] when the      */
   /* last CCM received from the peer MEP. Updated whenever        */
   /* CCM LM packet arrive from the peer MEP                       */
   /* range: 127:96, access type: RW, default value: 0x0           */
   PCP_TBL_FIELD rx_fcb_peer_p;

   /* RxFCbC: Latch RxFCb when the last CCM received from the      */
   /* peer MEP. Updated whenever CCM LM packet arrive from the     */
   /* peer MEP                                                     */
   /* range: 159:128, access type: RW, default value: 0x0          */
   PCP_TBL_FIELD rx_fcb_c;

   /* RxFCbP: Latch RxFCb[c] when the last CCM received from       */
   /* the peer MEP. Updated whenever CCM LM packet arrive from     */
   /* the peer MEP                                                 */
   /* range: 191:160, access type: RW, default value: 0x0          */
   PCP_TBL_FIELD rx_fcb_p;

   /* TxFCbPeerC: Carries the value of TxFCf field in the last     */
   /* CCM received from the peer MEP. Updated whenever CCM LM      */
   /* packet arrive from the peer MEP                              */
   /* range: 223:192, access type: RW, default value: 0x0          */
   PCP_TBL_FIELD tx_fcb_peer_c;

   /* TxFCbPeerP: Carries the value of TxFCf.peer[c] when the      */
   /* last CCM received from the peer MEP. Updated whenever        */
   /* CCM LM packet arrive from the peer MEP                       */
   /* range: 255:224, access type: RW, default value: 0x0          */
   PCP_TBL_FIELD tx_fcb_peer_p;

   /* RxFCb_indx_msb: pointer to RX counters in statistics DB      */
   /* range: 266:256, access type: RW, default value: 0x0          */
   PCP_TBL_FIELD rx_fcb_indx_msb;

   /* TxFCf_indx_msb: pointer to TX counters in statistics DB      */
   /* range: 277:267, access type: RW, default value: 0x0          */
   PCP_TBL_FIELD tx_fcf_indx_msb;

   /* LMM DA: LMM destination MAC address; used when OAMP          */
   /* injects LMM                                                  */
   /* range: 325:278, access type: RW, default value: 0x0          */
   PCP_TBL_FIELD lmm_da;

} __ATTRIBUTE_PACKED__ PCP_OAM_LMDB_CMN_TBL;


  /* LMDB tx: Internal Loss Measurement DB                          */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x03000000 */

   /* LMM_interval: LMM transmission interval described (same      */
   /* options as CCM)                                              */
   /* range: 2:0, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD lmm_interval;

   /* LM pr: priority of TX packet                                 */
   /* range: 5:3, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD lm_pr;

} __ATTRIBUTE_PACKED__ PCP_OAM_LMDB_TX_TBL;


  /* DMDB tx: Internal Delay Measurement DB for scheduled           */
  /* generation of DMM packets                                      */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x03800000 */

   /* DmInterval: Transmission interval descruned in ITU           */
   /* Y.1731                                                       */
   /* range: 2:0, access type: RW, default value: 0x0              */
   PCP_TBL_FIELD dm_interval;

   /* DmDa: DMM destination MAC address; used when OAMP            */
   /* injects DMM                                                  */
   /* range: 50:3, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD dm_da;

   /* DM pr: priority of TX packet                                 */
   /* range: 53:51, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD dm_pr;

} __ATTRIBUTE_PACKED__ PCP_OAM_DMDB_TX_TBL;


  /* DMDB rx: Internal Delay Measurement DB measured delay          */
  /* updated when DMR packet arrives                                */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x04000000 */

   /* DmDelay: Delay value computed on the last received DMR       */
   /* range: 19:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD dm_delay;

   /* DmDelayValid: 0' - indicates the value in the delay          */
   /* field is not valid; '1'- indicates the value in the          */
   /* delay field is valid (i.e. it was updated by a valid DMR     */
   /* arrived to this MEP)                                         */
   /* range: 20:20, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD dm_delay_valid;

} __ATTRIBUTE_PACKED__ PCP_OAM_DMDB_RX_TBL;


  /* Ing Counters: Ingress Counters table                           */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x00000000 */

   /* Packets: Number of packets for this record                   */
   /* range: 35:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD packets;

   /* Octets: Number of octets for this record                     */
   /* range: 79:36, access type: RW, default value: 0x0            */
   PCP_TBL_FIELD octets;

} __ATTRIBUTE_PACKED__ PCP_STS_ING_COUNTERS_TBL;


  /* Egr Counters: Egress Counters table                            */
typedef struct
{
   PCP_TBL_ADDR addr;   /* 0x00004000 */

   /* Packets: Number of packets for this record                   */
   /* range: 35:0, access type: RW, default value: 0x0             */
   PCP_TBL_FIELD packets;

} __ATTRIBUTE_PACKED__ PCP_STS_EGR_COUNTERS_TBL;


/* Block definition: ECI 	 */
typedef struct
{
  PCP_ECI_QDR_TBL qdr_tbl;

  PCP_ECI_RLD1_TBL rld1_tbl;

  PCP_ECI_RLD2_TBL rld2_tbl;

  PCP_ECI_RLD3_TBL rld3_tbl;

} __ATTRIBUTE_PACKED__ PCP_ECI_TBLS;

/* Block definition: ELK 	 */
typedef struct
{
  PCP_ELK_FID_COUNTER_PROFILE_DB_TBL fid_counter_profile_db_tbl;

  PCP_ELK_FID_COUNTER_DB_TBL fid_counter_db_tbl;

  PCP_ELK_SYS_PORT_IS_MINE_TBL sys_port_is_mine_tbl;

  PCP_ELK_AGING_CFG_TABLE_TBL aging_cfg_table_tbl;

  PCP_ELK_FLUSH_DB_TBL flush_db_tbl;

  PCP_ELK_DSP_EVENT_TABLE_TBL dsp_event_table_tbl;

} __ATTRIBUTE_PACKED__ PCP_ELK_TBLS;

/* Block definition: OAM 	 */
typedef struct
{
  PCP_OAM_PR2_TCDP_TBL pr2_tcdp_tbl;

  PCP_OAM_EXT_MEP_INDEX_DB_TBL ext_mep_index_db_tbl;

  PCP_OAM_EXT_MEP_DB_TBL ext_mep_db_tbl;

  PCP_OAM_MEP_DB_TBL mep_db_tbl;

  PCP_OAM_RMEP_DB_TBL rmep_db_tbl;

  PCP_OAM_RMEP_HASH_0_DB_TBL rmep_hash_0_db_tbl;

  PCP_OAM_RMEP_HASH_1_DB_TBL rmep_hash_1_db_tbl;

  PCP_OAM_LMDB_CMN_TBL lmdb_cmn_tbl;

  PCP_OAM_LMDB_TX_TBL lmdb_tx_tbl;

  PCP_OAM_DMDB_TX_TBL dmdb_tx_tbl;

  PCP_OAM_DMDB_RX_TBL dmdb_rx_tbl;

} __ATTRIBUTE_PACKED__ PCP_OAM_TBLS;

/* Block definition: STS 	 */
typedef struct
{
  PCP_STS_ING_COUNTERS_TBL ing_counters_tbl;

  PCP_STS_EGR_COUNTERS_TBL egr_counters_tbl;

} __ATTRIBUTE_PACKED__ PCP_STS_TBLS;
  /* Blocks definition } */

  typedef struct
{
  PCP_ECI_TBLS eci;
  PCP_ELK_TBLS elk;
  PCP_OAM_TBLS oam;
  PCP_STS_TBLS sts;
} __ATTRIBUTE_PACKED__ PCP_TBLS;
/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

typedef enum
{

  PCP_TBLS_GET = PCP_PROC_DESC_BASE_CHIP_TBLS_FIRST,
  PCP_TBLS_INIT,

  /*
   * Last element. Do no touch.
   */
  PCP_CHIP_TBLS_PROCEDURE_DESC_LAST
} PCP_CHIP_TBLS_PROCEDURE_DESC;

typedef enum
{
  PCP_TBLS_NOT_INITIALIZED_ERR = PCP_ERR_DESC_BASE_CHIP_TBLS_FIRST,
 
  /*
   * Last element. Do no touch.
   */
  PCP_CHIP_TBLS_ERR_LAST
} PCP_CHIP_TBLS_ERR;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
/*****************************************************
*NAME
* pcp_tbls_get
*TYPE:
*  PROC
*FUNCTION:
*  Get a pointer to tables database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT PCP_TBLS  **pcp_tbls - pointer to pcp
*                           tables database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *pcp_tbls.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    pcp_is_tbl_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  pcp_tbls_get(
    SOC_SAND_OUT PCP_TBLS  **pcp_tbls
  );

/*****************************************************
*NAME
* pcp_tbls
*TYPE:
*  PROC
*FUNCTION:
*  Return a pointer to tables database.
*  The database is per chip-version. Without Error Checking
*INPUT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    *pcp_tbls.
*REMARKS:
*    No checking is performed whether the DB is initialized
*SEE ALSO:
*****************************************************/
PCP_TBLS*
  pcp_tbls(void);

/*****************************************************
*NAME
*  pcp_tbls_init
*TYPE:
*  PROC
*FUNCTION:
*  Dynamically allocates and initializes Pcp tables database.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*   Calling upon already initialized tables will do nothing
*   User must make sure that this function is called after a semaphore was taken
*SEE ALSO:
*****************************************************/
uint32
  pcp_tbls_init(void);

CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_chip_tbls_get_procs_ptr(void);

CONST SOC_ERROR_DESC_ELEMENT*
  pcp_chip_tbls_get_errs_ptr(void);
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_CHIP_TBLS_INCLUDED__*/
#endif

