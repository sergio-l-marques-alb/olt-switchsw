/*
 * $Id: dnxc_intr.h, v1 16/06/2014 09:55:39 azarrin Exp $
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _DNXC_INTR_H_
#define _DNXC_INTR_H_

/*************
 * INCLUDES  *
 *************/

int soc_dnxc_set_mem_mask(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int all_one);
int soc_dnxc_ser_init(
    int unit);
int soc_dnxc_interrupts_disable(
    int unit);
int soc_dnxc_interrupts_deinit(
    int unit);
int soc_dnxc_interrupts_init(
    int unit);
int soc_dnxc_interrupt_all_enable_set(
    int unit,
    int enable);
int soc_dnxc_interrrupt_event_trigger(
    int unit,
    void *data);
int soc_dnxc_interrupt_event_init(
    int unit);
int soc_dnxc_interrupt_event_deinit(
    int unit);
shr_error_e soc_dnxc_interrupt_counter_clear(
    int unit);

/*************
 * TYPE DEFS *
 *************/

#endif /*_DNXC_INTR_H_*/
