/* -*- Mode:c++; c-style:k&r; c-basic-offset:2; indent-tabs-mode: nil; -*- */
/* vi:set expandtab cindent shiftwidth=2 cinoptions=\:0l1(0t0g0: */
/*
 * $Id: g2p3_cnt.c,v 1.8 Broadcom SDK $
 *
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
 * soc_sbx_g2p3_counters : Guadalupe2k Counters
 */


#include <shared/bsl.h>

#include <soc/sbx/g2p3/g2p3_int.h>

sbStatus_t
soc_sbx_g2p3_counters_ingress_read(soc_sbx_g2p3_state_t *pFe,
                         uint32 ulIndex,
                         soc_sbx_g2p3_count_t *pCount,
                         uint32 ulEntries,
                         uint8 bSync)
{
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_ingctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }
  
  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64*)pCount,
                                    bSync, /* sync */
                                    0); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_ingress_clear(soc_sbx_g2p3_state_t *pFe,
                          uint32 ulIndex,
                          uint32 ulEntries,
                          uint8 bSync)
{
  soc_sbx_g2p3_count_t dummy;
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_ingctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64 *) &dummy,
                                    bSync, /* sync */
                                    1); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_qos_rule_read( soc_sbx_g2p3_state_t *pFe,
                           uint32 ulIndex,
                           soc_sbx_g2p3_count_t *pCount,
                           uint32 ulEntries,
                           uint8 bSync)
{
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_irtctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64*)pCount,
                                    bSync, /* sync */
                                    0); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_qos_rule_clear( soc_sbx_g2p3_state_t *pFe,
                            uint32 ulIndex,
                            uint32 ulEntries,
                            uint8 bSync)
{
  soc_sbx_g2p3_count_t dummy;
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_irtctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64 *) &dummy,
                                    bSync, /* sync */
                                    1); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_egr_rule_read( soc_sbx_g2p3_state_t *pFe,
                           uint32 ulIndex,
                           soc_sbx_g2p3_count_t *pCount,
                           uint32 ulEntries,
                           uint8 bSync)
{
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_ertctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64*)pCount,
                                    bSync, /* sync */
                                    0); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_egr_rule_clear( soc_sbx_g2p3_state_t *pFe,
                            uint32 ulIndex,
                            uint32 ulEntries,
                            uint8 bSync)
{
  soc_sbx_g2p3_count_t dummy;
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_ertctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64 *) &dummy,
                                    bSync, /* sync */
                                    1); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_egress_read(soc_sbx_g2p3_state_t *pFe,
                         uint32 ulIndex,
                         soc_sbx_g2p3_count_t *pCount,
                         uint32 ulEntries,
                         uint8 bSync)
{
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_egrctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64*)pCount,
                                    bSync, /* sync */
                                    0); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_egress_clear(soc_sbx_g2p3_state_t *pFe,
                          uint32 ulIndex,
                          uint32 ulEntries,
                          uint8 bSync)
{
  soc_sbx_g2p3_count_t dummy;
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_egrctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64 *) &dummy,
                                    bSync, /* sync */
                                    1); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_exception_read(soc_sbx_g2p3_state_t *pFe,
                         uint32 ulIndex,
                         soc_sbx_g2p3_count_t *pCount,
                         uint32 ulEntries,
                         uint8 bSync)
{
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_excctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64*)pCount,
                                    bSync, /* sync */
                                    0); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_exception_clear(soc_sbx_g2p3_state_t *pFe,
                                      uint32 ulIndex,
                                      uint32 ulEntries,
                                      uint8 bSync)
{
  soc_sbx_g2p3_count_t dummy;
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_excctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64 *) &dummy,
                                    bSync, /* sync */
                                    1); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_drop_read(soc_sbx_g2p3_state_t *pFe,
                         uint32 ulIndex,
                         soc_sbx_g2p3_count_t *pCount,
                         uint32 ulEntries,
                         uint8 bSync)
{
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_pdctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64*)pCount,
                                    bSync, /* sync */
                                    0); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_drop_clear(soc_sbx_g2p3_state_t *pFe,
                                      uint32 ulIndex,
                                      uint32 ulEntries,
                                      uint8 bSync)
{
  soc_sbx_g2p3_count_t dummy;
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_pdctr_counter_params_get(pFe->unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  return soc_sbx_g2p3_counters_read(pFe,
                                    cp.bank,
                                    ulIndex,
                                    ulEntries,
                                    (uint64 *) &dummy,
                                    bSync, /* sync */
                                    1); /* clear */
}

sbStatus_t
soc_sbx_g2p3_counters_oam_read(uint32 unit,
                               uint32 ulIndex,
                               soc_sbx_g2p3_oam_count_t *pCount,
                               uint8 bClearOnRead)
{
  soc_sbx_g2p3_counter_value_t bytePacketCount;
  soc_sbx_g2p3_counter_params_t cp;
  int rv;

  rv = soc_sbx_g2p3_oamctr_counter_params_get(unit, &cp);
  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }
  
  rv = soc_sbx_g2p3_oamrxtxctr_get(unit,
                                   bClearOnRead,
                                   ulIndex,
                                   &bytePacketCount);

  if (rv != SOC_E_NONE) {
    return SB_OTHER_ERR_CODE;
  }

  /* convert bytes/packet to rx/tx */
  pCount->rx = bytePacketCount.bytes;
  pCount->tx = bytePacketCount.packets;

  return rv;
}

sbStatus_t
soc_sbx_g2p3_counters_oam_clear(uint32 unit,
                                uint32 ulIndex,
                                uint32 ulEntries)
{
  soc_sbx_g2p3_counter_value_t dummy;
  soc_sbx_g2p3_counter_params_t cp;
  int rv, i, status = SOC_E_NONE;
  uint32 ulEnd = ulIndex + ulEntries;

  rv = soc_sbx_g2p3_oamctr_counter_params_get(unit, &cp);
  if (rv != SOC_E_NONE) {
        status = SB_OTHER_ERR_CODE;
  }

  for (i=ulIndex; i < ulEnd; i++) {
    rv = soc_sbx_g2p3_oamrxtxctr_get(unit,
                                     1, /* clear */
                                     ulIndex,
                                     &dummy);
    if (rv != SOC_E_NONE) {
      status = SB_OTHER_ERR_CODE;
    }
  }

  return status;
}

/**/

sbStatus_t
soc_sbx_g2p3_counters_read(soc_sbx_g2p3_state_t *pFe,
                   int eSegment,
                   uint32 ulOffset,
                   uint32 ulEntries,
                   uint64 *ullData,
                   uint8 bSync,
                   uint8 bClear)
{
  uint32 ulCntsPerGroup = 1;
  sbStatus_t sts;


  ulOffset *= ulCntsPerGroup;

  sts = sbFe2000CmuCounterRead(pFe->pCmuMgr,
                         (uint32)eSegment,
                         ulOffset,
                         ulEntries * ulCntsPerGroup,
                         ullData,
                         bSync,
                         bClear);

  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("cdp 0x%x 0x%x 0x%x 0x%x data 0x%x%08x 0x%x 0x%x\n"),eSegment, ulOffset, ulEntries,  ulCntsPerGroup,  COMPILER_64_HI(*ullData), COMPILER_64_LO(*ullData),  bSync,   bClear));

  return sts;
}

/*-----------------------------------------------------------------------------
 * @fn soc_sbx_g2p3_counter_segment_enable()
 *
 * @brief Enable or disable a segment
 *
 * @param pFe       - Driver control structure
 * @param ulSegment - Segment Id (0 -> 31)
 * @param bEnable   - Enable = 1, Disable = 0
 *
 * @return error code, SB_OK on success
 *----------------------------------------------------------------------------*/
sbStatus_t
soc_sbx_g2p3_counter_segment_enable(soc_sbx_g2p3_state_t *pFe,
                           uint32 ulSegment,
                           uint8 bEnable)
{
  return sbFe2000CmuSegmentEnable(pFe->pCmuMgr, ulSegment, bEnable);
}

/*-----------------------------------------------------------------------------
 * @fn soc_sbx_g2p3_counter_segment_flush()
 *
 * @brief Enable or disable flushing (Forced Counter Ejection) on a specified
 *        segment. When this is enabled, the counter manager will cycle through
 *        a segment and serially eject counters for handling. When it reaches
 *        the end of a segment, it will wrap and continue.
 *
 * @param pFe        - Driver control structure
 * @param ulSegment  - Segment to perform flushing on
 * @param bEnable    - Enable or disable ejection on this segment
 * @param ulRateData - Configure rate of counter ejection.
 *
 * @return error code, SB_OK on success
 *-----------------------------------------------------------------------------*/
sbStatus_t
soc_sbx_g2p3_counter_segment_flush(soc_sbx_g2p3_state_t *pFe,
                          uint32 ulSegment,
                          uint8 bEnable,
                          uint32 ulRateData)
{
  return sbFe2000CmuSegmentFlush(pFe->pCmuMgr, ulSegment, bEnable, ulRateData);
}

/*-----------------------------------------------------------------------------
 * @fn soc_sbx_g2p3_counter_segment_flush_status()
 *
 * @brief Return current index in flush progress for a segment. If no flush is
 *        is in progress, return BUSY error code.
 *
 * @param pFe       - Driver control structure
 * @param ulSegment - Segment performing flushing
 * @param ulCntrId  - Current location in segment space
 *
 * @return error code, SB_OK on success
 *-----------------------------------------------------------------------------*/
sbStatus_t
soc_sbx_g2p3_counter_segment_flush_status(soc_sbx_g2p3_state_t *pFe,
                                uint32 ulSegment,
                                uint32 *ulCntrId)
{
  return sbFe2000CmuSegmentFlushStatus(pFe->pCmuMgr, ulSegment, ulCntrId);
}

/*-----------------------------------------------------------------------------
 * @fn soc_sbx_g2p3_counter_force_flush()
 *
 * @brief Request a manual flush of a specified counter or counters to the host
 *        for handling. Currently, the hardware only supports ranges (n -> 0)
 *        but we should be adding support for individual counters.
 *
 * @param pFe       - Driver control structure
 * @param ulSegment - Segment to perform the forced flush on
 * @param ulOffset  - Offset within that segment
 * @param ulSize    - Number of entries to flush
 *
 * @return error code, SB_OK on success
 *-----------------------------------------------------------------------------*/
sbStatus_t
soc_sbx_g2p3_counter_force_flush(soc_sbx_g2p3_state_t *pFe,
                        uint32 ulSegment,
                        uint32 ulOffset,
                        uint32 ulSize)
{
  return sbFe2000CmuForceFlush(pFe->pCmuMgr, ulSegment, ulOffset, ulSize);
}

/*-----------------------------------------------------------------------------
 * @fn soc_sbx_g2p3_oamrxtxctr_get_ext()
 *
 * @brief This function calls the UTG generated oamctr_get function and
 *        uses the Byte/Packet counts to compute the RX/TX counts.  It will
 *        keep the same structure that holds byte/packet counts but the
 *        contents of that structure will be RX & TX packet counts.  The
 *        soc_sbx_g2p3_counter_value_t structure can later be cast as
 *        soc_sbx_g2p3_oam_count_s for RX/TX counts.
 *
 * @param unit      - Unit number
 * @param clear     - 1 to clear on read, 0 to accumulate
 * @param cnum      - Counter index within segment
 * @param v         - Pointer to structure containing RX & TX counts
 *
 * @return error code, SB_OK on success
 *-----------------------------------------------------------------------------*/
sbStatus_t
soc_sbx_g2p3_oamrxtxctr_get_ext (int unit,
                                 int clear,
                                 int cnum,
                                 soc_sbx_g2p3_counter_value_t *v) {
  sbStatus_t rv = SB_OTHER_ERR_CODE;
  uint64 rx, tx, tmp;

  if (v != NULL) {
    rv = soc_sbx_g2p3_oamctr_get(unit, clear, cnum, v);
    if (rv == SOC_E_NONE) {

      /* convert bytes/packet to rx/tx
       *  TX = Bytes & 0x1FFF
       *  RX = Bytes >> 13
       *  Packets = RX + TX
       */

      /* zero the temp variables */
      COMPILER_64_ZERO(rx);
      COMPILER_64_ZERO(tx);

      /* TX = bytes & 0x1FFF*/
      COMPILER_64_ADD_64(tx, v->bytes);
      COMPILER_64_SET(tmp, 0, 0x1FFF);
      COMPILER_64_AND(tx, tmp);

      /* RX = bytes >> 13 */
      COMPILER_64_ADD_64(rx, v->bytes);
      COMPILER_64_SHR(rx, 13);

      /* Sanity check.  RX + TX must equal Packets */
      COMPILER_64_SUB_64(v->packets, rx);
      COMPILER_64_SUB_64(v->packets, tx);
      if (!(COMPILER_64_IS_ZERO(v->packets))) {
        return SB_OTHER_ERR_CODE;
      }

      /* update the v structure */
      COMPILER_64_ZERO(v->bytes);
      COMPILER_64_ZERO(v->packets);

      /* (bytes, packets) <=> (rx, tx) */
      COMPILER_64_ADD_64(v->bytes, rx);
      COMPILER_64_ADD_64(v->packets, tx);
    }
  }

  return rv;
}
