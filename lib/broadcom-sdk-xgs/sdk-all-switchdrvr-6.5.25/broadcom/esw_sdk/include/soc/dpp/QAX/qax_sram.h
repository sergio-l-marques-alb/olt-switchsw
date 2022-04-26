/*
 * $Id: qax_sram.h Exp $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _QAX_SRAM_H
#define _QAX_SRAM_H

/*
 * Includes
 */

/*
 * TypeDefs
 */

/*
 * Defines
 */

/*
 * Functions
 */

int soc_qax_sram_conf_set(int unit);
int soc_qax_sram_drop_threshold_set(int unit, int priority, uint32 value);
int soc_qax_sram_drop_threshold_get(int unit, int priority, uint32 *value);
int soc_qax_sram_drop_reassembly_context_profile_map_set(int unit, int core, uint32 reassembly_context, uint32 profile_id);
int soc_qax_sram_drop_reassembly_context_profile_map_get(int unit, int core, uint32 reassembly_context, uint32 *profile_id);
int soc_qax_sram_drop_profile_priority_map_set(int unit, uint32 profile_id, uint32 tc, uint32 dp, uint32 priority);
int soc_qax_sram_drop_profile_priority_map_get(int unit, uint32 profile_id, uint32 tc, uint32 dp, uint32 *priority);


#endif /* _QAX_SRAM_H */
