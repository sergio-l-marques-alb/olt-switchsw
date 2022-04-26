/*----------------------------------------------------------------------
 * $Id: jupiter_cfg_seq.h, port-mod team Exp $
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : jupiter_cfg_seq.h
 * Description: c functions implementing Tier1s for Jupiter Serdes Driver
 *---------------------------------------------------------------------*/
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  $Id$
*/

#ifndef JUPITER_CFG_SEQ_H 
#define JUPITER_CFG_SEQ_H 

#define JUP_TXFIR_STATUS_POLL_TIMEOUT 5 /* micro-seconds */

int jupiter_pmd_access_clk_switch (phymod_access_t *sa__, uint32_t switch_to_refclk);

int jupiter_pmd_link_training_setup (phymod_access_t *sa__);

int jupiter_ucode_load_verify (phymod_access_t *sa__);

int jupiter_ucode_pmi_load (phymod_access_t *sa__, uint32_t csr_only);
#endif /* JUPITER_CFG_SEQ_H */

