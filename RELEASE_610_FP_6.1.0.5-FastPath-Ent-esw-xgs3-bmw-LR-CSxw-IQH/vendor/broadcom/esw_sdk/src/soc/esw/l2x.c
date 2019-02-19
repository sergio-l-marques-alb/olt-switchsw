/*
 * $Id: l2x.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
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
 * XGS L2 Table Manipulation API routines.
 *
 * The L2Xm pseudo-table is an aggregate structure supported by
 * hardware.  When an entry is read from it, it is constructed from the
 * L2X_BASEm, L2X_VALIDm, L2X_HITm, and L2X_STATICm tables.
 * NOTE: The L2X pseudo-table is read-only.
 *
 * A low-level L2 shadow table is optionally kept in soc->arlShadow by
 * using a callback to get all inserts/deletes from the l2xmsg task.  It
 * can be disabled by setting the l2xmsg_avl property to 0.
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/ptable.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/tucana.h>

#ifdef BCM_EASYRIDER_SUPPORT
#include <soc/er_cmdmem.h>
#endif

#define DEFAULT_L2DELETE_CHUNKS		(64)	/* 16k entries / 64 = 256 */
            
#ifdef BCM_XGS_SWITCH_SUPPORT

/*
 * While the L2 table is frozen, the L2Xm lock is held.
 *
 * All tasks must obtain the L2Xm lock before modifying the CML bits or
 * age timer registers.
 */

typedef struct l2_freeze_s {
    int                 frozen;
    int                 save_age_sec;
    int                 save_age_ena;
} l2_freeze_t;

typedef struct cml_freeze_s {
    int                 frozen;
    int                 save_cml[SOC_MAX_NUM_PORTS];
    int                 save_cml_move[SOC_MAX_NUM_PORTS];
} cml_freeze_t;

l2_freeze_t l2_freeze_state[SOC_MAX_NUM_DEVICES];
cml_freeze_t cml_freeze_state[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *	soc_l2x_entry_compare_key
 * Purpose:
 *	Comparison function for AVL shadow table operations.  Compares
 *	key portion of entry only.
 * Parameters:
 *	user_data - used to pass StrataSwitch unit #
 *	datum1 - first L2X entry to compare
 *	datum2 - second L2X entry to compare
 * Returns:
 *	datum1 <=> datum2
 */

int
soc_l2x_entry_compare_key(void *user_data,
                          shr_avl_datum_t *datum1,
                          shr_avl_datum_t *datum2)
{
    int		unit = PTR_TO_INT(user_data);

    return soc_mem_compare_key(unit, L2Xm, datum1, datum2);
}

/*
 * Function:
 *	soc_l2x_entry_compare_all
 * Purpose:
 *	Comparison function for AVL shadow table operations.  Compares
 *	entire key+data of entry, but key is still most significant.
 * Parameters:
 *	user_data - used to pass StrataSwitch unit #
 *	datum1 - first L2X entry to compare
 *	datum2 - second L2X entry to compare
 * Returns:
 *	datum1 <=> datum2
 */

int
soc_l2x_entry_compare_all(void *user_data,
                          shr_avl_datum_t *datum1,
                          shr_avl_datum_t *datum2)
{
    int		unit = PTR_TO_INT(user_data);
    int		i;

    i = soc_mem_compare_key(unit, L2Xm, datum1, datum2);

    if (i == 0) {
        i = sal_memcmp(datum1, datum2, sizeof (l2x_base_entry_t));
    }

    return i;
}

/*
 * Function:
 *	soc_l2x_entry_dump
 * Purpose:
 *	Debug dump function for AVL shadow table operations.
 * Parameters:
 *	user_data - used to pass StrataSwitch unit #
 *	datum - L2X entry to dump
 *	extra_data - unused
 * Returns:
 *	SOC_E_XXX
 */

int
soc_l2x_entry_dump(void *user_data, shr_avl_datum_t *datum, void *extra_data)
{
    int		unit = PTR_TO_INT(user_data);

    COMPILER_REFERENCE(extra_data);

    soc_mem_entry_dump(unit, L2Xm, (l2x_entry_t *) datum);
    soc_cm_print("\n");

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2x_shadow_callback
 * Purpose:
 *	Internal callback routine for updating an AVL tree shadow table
 * Parameters:
 *	unit - StrataSwitch unit number.
 *	entry_del - Entry to be deleted or updated, NULL if none.
 *	entry_add - Entry to be inserted or updated, NULL if none.
 *	fn_data - unused.
 * Notes:
 *	Used only if L2X shadow table is enabled.
 */

STATIC void
soc_l2x_shadow_callback(int unit,
                        l2x_entry_t *entry_del,
                        l2x_entry_t *entry_add,
                        void *fn_data)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);

    if (entry_del != NULL) {
        shr_avl_delete(soc->arlShadow, soc_l2x_entry_compare_key,
                       (shr_avl_datum_t *)entry_del);
    }

    if (entry_add != NULL) {
        shr_avl_insert(soc->arlShadow, soc_l2x_entry_compare_key,
                       (shr_avl_datum_t *)entry_add);
    }

    sal_mutex_give(soc->arlShadowMutex);
}

/*
 * Function:
 *	soc_l2x_attach
 * Purpose:
 *	Allocate L2X subsystem resources
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	The L2X tree shadow table is optional and its allocation
 *	is controlled using a property.
 */

int
soc_l2x_attach(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    (void)soc_l2x_detach(unit);

    if (soc_property_get(unit, spn_L2XMSG_AVL,
                         SOC_IS_EASYRIDER(unit) ? FALSE: TRUE)) {
        int		datum_bytes, datum_max;

#ifdef BCM_EASYRIDER_SUPPORT
        if (soc_feature(unit, soc_feature_l2_multiple)) {
            datum_bytes = sizeof (l2_entry_internal_entry_t);
            datum_max =
                soc_mem_index_count(unit, L2_ENTRY_EXTERNALm) +
                soc_mem_index_count(unit, L2_ENTRY_INTERNALm) +
                soc_mem_index_count(unit, L2_ENTRY_OVERFLOWm);
        } else
#endif
        {
            datum_bytes = sizeof (l2x_entry_t);
            datum_max = soc_mem_index_count(unit, L2Xm);
        }

        if (shr_avl_create(&soc->arlShadow,
                           INT_TO_PTR(unit),
                           datum_bytes,
                           datum_max) < 0) {
            return SOC_E_MEMORY;
        }

        if ((soc->arlShadowMutex = sal_mutex_create("asMutex")) == NULL) {
            (void)soc_l2x_detach(unit);
            return SOC_E_MEMORY;
        }

        soc_l2x_register(unit, soc_l2x_shadow_callback, NULL);
    }

    /* Reset l2 freeze structures. */
    sal_memset(l2_freeze_state + unit, 0, sizeof(l2_freeze_t));
    sal_memset(cml_freeze_state + unit, 0, sizeof(cml_freeze_t));


    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2x_detach
 * Purpose:
 *	Deallocate L2X subsystem resources
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_XXX
 */

int
soc_l2x_detach(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    soc_l2x_unregister(unit, soc_l2x_shadow_callback, NULL);

    if (soc->arlShadow != NULL) {
        shr_avl_destroy(soc->arlShadow);
        soc->arlShadow = NULL;
    }

    if (soc->arlShadowMutex != NULL) {
        sal_mutex_destroy(soc->arlShadowMutex);
        soc->arlShadowMutex = NULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2x_init
 * Purpose:
 *	Initialize L2 table subsystem.
 * Parameters:
 *	unit - StrataSwitch unit number.
 * Returns:
 *	SOC_E_xxx
 * Notes:
 *	This routine must not do anything time consuming since it is
 *	called by soc_init().
 */

int
soc_l2x_init(int unit)
{
    /* The L2 table is cleared in soc_draco_misc_init(). */
    return SOC_E_NONE;
}

#if defined(BCM_XGS12_SWITCH_SUPPORT)
/*
 * Function:
 *	soc_l2x_lookup
 * Purpose:
 *	Send an L2 lookup message over the S-Channel and receive the
 *	response.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	key - L2X entry to look up; only MAC+VLAN fields are relevant
 *	result - L2X entry to receive entire found entry
 *	index_ptr (OUT) - If found, receives table index where found
 * Returns:
 *	SOC_E_INTERNAL if retries exceeded or other internal error
 *	SOC_E_NOT_FOUND if the entry is not found.
 *	SOC_E_NONE (0) on success (entry found):
 * Notes:
 *	The S-Channel response contains either a matching L2 entry,
 *	or an entry with a key of all f's if not found.  It is okay
 *	if the result pointer is the same as the key pointer.
 *	Retries the lookup in cases where the particular chip requires it.
 */

int
soc_l2x_lookup(int unit,
               l2x_entry_t *key, l2x_entry_t *result,
               int *index_ptr)
{
    schan_msg_t 	schan_msg;

    schan_msg_clear(&schan_msg);
    schan_msg.arllkup.header.opcode = ARL_LOOKUP_CMD_MSG;
    schan_msg.arllkup.header.srcblk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    schan_msg.arllkup.header.datalen = 8;
    schan_msg.arllkup.address = soc_mem_addr(unit, L2Xm, ARL_BLOCK(unit), 0);

    /* Fill in packet data */

    sal_memcpy(schan_msg.arllkup.data, key, sizeof (schan_msg.arllkup.data));

    /*
     * Write onto S-Channel "ARL lookup" command packet consisting of
     * header word + address + two words of ARL key, and read back header
     * word + 2 words of a special lookup result format.
     */

    SOC_IF_ERROR_RETURN(soc_schan_op(unit, &schan_msg, 4, 3, 1));

    /* Check result */

    if (schan_msg.readresp.header.opcode != READ_MEMORY_ACK_MSG) {
        soc_cm_debug(DK_ERR,
                     "soc_l2x_lookup: "
                     "invalid S-Channel reply, expected READ_MEMORY_ACK:\n");
        soc_schan_dump(unit, &schan_msg, 3);
        return SOC_E_INTERNAL;
    }

    /*
     * Fill in result entry from read data.
     * Note that the lookup does not return the key, so it must be copied.
     *
     * Format of S-Channel response:
     *		readresp.header : Memory Read Ack S-channel header
     *		readresp.data[0]: { 4'h0, hit, static, l2_table_data }
     *		readresp.data[1]: { 18'h0, index_into_L2_table[13:0] }
     */

    if (schan_msg.readresp.data[0] == 0xffffffff &&
        schan_msg.readresp.data[1] == 0xffffffff) {
        *index_ptr = -1;
        return SOC_E_NOT_FOUND;
    }

    /* MACADDR<31:0> */
    result->entry_data[0] = key->entry_data[0];

    /* CPU, COS<2:0>, VLAN<12:0>, MACADDR<47:32> */
    result->entry_data[1] = ((schan_msg.readresp.data[0] << 28) |
                             (key->entry_data[1] & 0x0fffffff));

    /* ZERO<6:0>, VALID=1, HIT, STATIC .. DST_DISCARD */
    result->entry_data[2] = schan_msg.readresp.data[0] >> 4;
    if (SOC_IS_DRACO15(unit)) {
        /* VALID_BIT is bit89 on 5695, not bit88 */
        result->entry_data[2] |= 1 << (89-64);
    } else {
        result->entry_data[2] |= 1 << (88-64);
    }

    *index_ptr = schan_msg.readresp.data[1];

    if (soc_cm_debug_check(DK_SOCMEM)) {
        soc_cm_debug(DK_SOCMEM, "L2 entry lookup: ");
        soc_mem_entry_dump(unit, L2Xm, result);
        soc_cm_debug(DK_SOCMEM, " (index=%d)\n", *index_ptr);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *	_soc_l2x_hit_clear
 * Purpose:
 *	Clear the hit bit on a specified L2 entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number
 *	entry - Entry to operate on
 * Returns:
 *	SOC_E_NONE - success
 *	SOC_E_NOT_FOUND - no such address
 *	SOC_E_XXX - other
 * Notes:
 *	The proper way to clear hit bits is to use soc_l2x_insert()
 *	with the hit bit clear.  This is just a helper routine for
 *	a 5690 problem.
 *
 *	5690 cannot clear hit bits atomically, hence there is a race
 *	condition where if other hit bit(s) got set in the same bucket
 *	between our read and write operations, they could also be
 *	cleared.
 */

int
_soc_l2x_hit_clear(int unit, l2x_entry_t *entry)
{
    l2x_entry_t		result;
    l2x_hit_entry_t	hit_bits;
    int			index, bucket, bit, r;

    if ((r = soc_l2x_lookup(unit, entry, &result, &index)) < 0) {
        return r;
    }

    bucket = index / SOC_L2X_BUCKET_SIZE;
    bit = index % SOC_L2X_BUCKET_SIZE;

    /* Read/modify/write bucket hit bits */

    soc_mem_lock(unit, L2X_HITm);

    if ((r = soc_mem_read(unit, L2X_HITm, MEM_BLOCK_ANY,
                          bucket, &hit_bits)) < 0) {
        soc_mem_unlock(unit, L2X_HITm);
        return r;
    }

    hit_bits.entry_data[0] &= ~(1 << bit);

    if ((r = soc_mem_write(unit, L2X_HITm, MEM_BLOCK_ANY,
                           bucket, &hit_bits)) < 0) {
        soc_mem_unlock(unit, L2X_HITm);
        return r;
    }

    soc_mem_unlock(unit, L2X_HITm);

    return SOC_E_NONE;
}

/*
 * Function:
 *	soc_l2x_insert
 * Purpose:
 *	Insert an entry into the L2X hash table.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	entry - L2X entry to insert
 * Returns:
 *	SOC_E_NONE - success
 *	SOC_E_FULL - hash bucket full
 * Notes:
 *	Uses hardware insertion; sends an L2 INSERT message over the
 *	S-Channel.  The hardware needs the L2Xm entry type.
 *	This affects the L2_ENTRYm table, L2_VALIDm table, L2_STATICm
 *	table, and the L2_HITm table.
 */

int
soc_l2x_insert(int unit, l2x_entry_t *entry)
{
    l2x_entry_t         valid_entry;
    schan_msg_t 	schan_msg;
    int			rv;

    if (soc_cm_debug_check(DK_SOCMEM)) {
        soc_cm_debug(DK_SOCMEM, "Insert table[L2X]: ");
        soc_mem_entry_dump(unit, L2Xm, entry);
        soc_cm_debug(DK_SOCMEM, "\n");
    }

    memcpy(&valid_entry, entry, sizeof(l2x_entry_t));
    soc_L2Xm_field32_set(unit, &valid_entry, VALID_BITf, 1);

#define SOC_L2X_MEM_BYTES 11
    schan_msg_clear(&schan_msg);
    schan_msg.arlins.header.opcode = ARL_INSERT_CMD_MSG;
    schan_msg.arlins.header.srcblk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    schan_msg.arlins.header.datalen = SOC_L2X_MEM_BYTES;
#undef SOC_L2X_MEM_BYTES

    /* Fill in ARL packet data */

    sal_memcpy(schan_msg.arlins.data, &valid_entry, 
               sizeof (schan_msg.arlins.data));

    /* Lock ARL memory to avoid modifying L2 table while frozen */

    soc_mem_lock(unit, L2Xm);

    /* Execute S-Channel operation (header word + 3 DWORDs) */

    rv = soc_schan_op(unit, &schan_msg, 4, 0, 1);

    if (rv == SOC_E_FAIL) {
        soc_cm_debug(DK_SOCMEM, "Insert table[L2X]: hash bucket full\n");
        rv = SOC_E_FULL;
    }

    soc_mem_unlock(unit, L2Xm);

    /* 5690 workaround; see _soc_l2x_hit_clear() for details */

    if (rv >= 0 &&
        soc_feature(unit, soc_feature_l2x_ins_sets_hit) &&
        !soc_L2Xm_field32_get(unit, entry, HIT_BITf)) {
        rv = _soc_l2x_hit_clear(unit, entry);
    }

    return rv;
}

/*
 * Function:
 *	soc_l2x_delete
 * Purpose:
 *	Delete an entry from the L2X hash table.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	entry - L2X entry to delete
 * Returns:
 *	SOC_E_NONE - success
 *	SOC_E_FULL - hash bucket full
 * Notes:
 *	Uses hardware deletion; sends an L2 DELETE message over the
 *	S-Channel.  The hardware needs the L2Xm entry type.
 *	Only the L2_VALIDm table is affected.
 */

int
soc_l2x_delete(int unit, l2x_entry_t *entry)
{
    schan_msg_t 	schan_msg;
    int			rv;

    if (soc_cm_debug_check(DK_SOCMEM)) {
        soc_cm_debug(DK_SOCMEM, "Delete table[L2X]: ");
        soc_mem_entry_dump(unit, L2Xm, entry);
        soc_cm_debug(DK_SOCMEM, "\n");
    }

#define SOC_L2X_MEM_KEY_BYTES 8
    schan_msg_clear(&schan_msg);
    schan_msg.arldel.header.opcode = ARL_DELETE_CMD_MSG;
    schan_msg.arldel.header.srcblk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    schan_msg.arldel.header.datalen = SOC_L2X_MEM_KEY_BYTES;
#undef SOC_L2X_MEM_KEY_BYTES

    /* Fill in packet data */

    sal_memcpy(schan_msg.arldel.data, entry, sizeof (schan_msg.arldel.data));

    /* Lock ARL memory to avoid modifying L2 table while frozen */

    soc_mem_lock(unit, L2Xm);

    /* Execute S-Channel operation (header word + 2 DWORDs) */
    rv = soc_schan_op(unit, &schan_msg, 3, 0, 1);

    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Function:
 *	soc_l2x_delete_all
 * Purpose:
 *	Clear the L2 table by invalidating entries.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	static_too - if TRUE, delete static and non-static entries;
 *		     if FALSE, delete only non-static entries
 * Returns:
 *	SOC_E_XXX
 */

int
soc_l2x_delete_all(int unit, int static_too)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			index_min, index_max, index;
    l2x_valid_entry_t	valid_bits;
    l2x_static_entry_t	static_bits;
    int			rv = SOC_E_NONE;

    soc_mem_lock(unit, L2Xm);

    sal_memset(&valid_bits, 0, sizeof (valid_bits));

    index_min = soc_mem_index_min(unit, L2X_VALIDm);
    index_max = soc_mem_index_max(unit, L2X_VALIDm);

    for (index = index_min; index <= index_max; index++) {
        if (!static_too) {
            if ((rv = soc_mem_read(unit, L2X_VALIDm, MEM_BLOCK_ANY,
                                   index, &valid_bits)) < 0) {
                goto done;
            }

            if ((rv = soc_mem_read(unit, L2X_STATICm, MEM_BLOCK_ANY,
                                   index, &static_bits)) < 0) {
                goto done;
            }

            valid_bits.entry_data[0] &= static_bits.entry_data[0];
        } /* else the valid bits will always be 0 */

        if ((rv = soc_mem_write(unit, L2X_VALIDm, MEM_BLOCK_ALL,
                                index, &valid_bits)) < 0) {
            goto done;
        }
    }

    if (soc->arlShadow != NULL) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }

 done:

    soc_mem_unlock(unit, L2Xm);

    return rv;
}
#endif /* BCM_XGS12_SWITCH_SUPPORT */

/*
 * Function:
 *	_soc_l2x_port_age
 * Purpose:
 *	Use HW port/VLAN "aging" to delete selected L2 entries.
 * Parameters:
 *	unit	     - StrataSwitch unit #
 *	age_rval     - the desired config for the port aging register.
 *	alt_age_reg  - optioanl 2nd port aging register.
 *	alt_age_rval - the desired config for the 2nd port aging register.
 * Returns:
 *	SOC_E_XXX
 * Notes:
 *	Hardware deletion is used.
 *	The chip requires that ARL aging be disabled during this
 *	operation.  An unavoidable side effect is that the hardware
 *	aging timer gets restarted whenever this routine is called.
 */

int
soc_l2x_port_age(int unit, uint32 age_rval, soc_reg_t alt_age_reg,
                 uint32 alt_age_rval)
{
    soc_timeout_t	to;
    int			rv;
    int			timeout_usec;
    int                 complete, alt_complete;
    uint32              alt_addr = 0;

    timeout_usec = soc_property_get(unit,
                                    spn_ARL_CLEAN_TIMEOUT_USEC,
                                    5000000);

    SOC_IF_ERROR_RETURN(soc_l2x_freeze(unit));

    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &age_rval, STARTf, 1);
    soc_reg_field_set(unit, PER_PORT_AGE_CONTROLr, &age_rval, COMPLETEf, 0);
    if ((rv = WRITE_PER_PORT_AGE_CONTROLr(unit, age_rval)) < 0) {
        goto done;
    }
    complete = FALSE;

    alt_complete = TRUE;
    if (alt_age_reg != INVALIDr) {
        soc_reg_field_set(unit, alt_age_reg, &alt_age_rval, STARTf, 1);
        soc_reg_field_set(unit, alt_age_reg, &alt_age_rval, COMPLETEf, 0);
        alt_addr = soc_reg_addr(unit, alt_age_reg, REG_PORT_ANY, 0);
        if ((rv = soc_reg32_write(unit, alt_addr, alt_age_rval)) < 0) {
            goto done;
        }
        alt_complete = FALSE;
    }

    soc_timeout_init(&to, timeout_usec, 0);
    for (;;) {
        if (!complete) {
            rv = READ_PER_PORT_AGE_CONTROLr(unit, &age_rval);
            if (rv >= 0) {
                complete = soc_reg_field_get(unit, PER_PORT_AGE_CONTROLr,
                                             age_rval, COMPLETEf);
            }
        }
        if (!alt_complete) {
            rv = soc_reg32_read(unit, alt_addr, &alt_age_rval);
            if (rv >= 0) {
                alt_complete = soc_reg_field_get(unit, alt_age_reg,
                                                 alt_age_rval, COMPLETEf);
            }
        }

        if (complete && alt_complete) {
            rv = SOC_E_NONE;
            break;
        }

        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            break;
        }
    }

 done:
    SOC_IF_ERROR_RETURN(soc_l2x_thaw(unit));
    return rv;
}

/*
 * Function:
 *  	_soc_l2x_frozen_cml_restore
 * Purpose:
 *  	Helper function to restore port learning mode.
 * Parameters:
 *   	unit - (IN) BCM device number.
 * Returns:
 *	    SOC_E_XXX
 */
int 
_soc_l2x_frozen_cml_restore(int unit)
{
    cml_freeze_t		*f_cml = &cml_freeze_state[unit]; /* Cml freeze state.*/
    soc_port_t		    port;                /* Device port iterator.         */ 
    port_tab_entry_t    pent;                /* Port table hw entry buffer.   */
    int                 cml;                 /* Learn mode for specific port. */ 
    int                 rv = SOC_E_NONE;     /* Operation return status.      */

    /* Take protection mutex. */ 
    soc_mem_lock(unit, PORT_TABm);

    /* If going out of freeze restore sw preserved values to HW. */
    if (f_cml->frozen == 1) {
        PBMP_E_ITER(unit, port) {
            /* Read port table entry. */
            rv = READ_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ANY, port, &pent);
            if (SOC_FAILURE(rv)) {
                break;
            }
            /* Get port current learning mode. */
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                int cml_change = FALSE;
                cml = soc_PORT_TABm_field32_get(unit, &pent, CML_FLAGS_NEWf);
                if (cml != f_cml->save_cml[port]) {
                    soc_PORT_TABm_field32_set(unit, &pent, CML_FLAGS_NEWf,
                                              f_cml->save_cml[port]);
                    cml_change = TRUE;
                }
                cml = soc_PORT_TABm_field32_get(unit, &pent, CML_FLAGS_MOVEf);
                if (cml != f_cml->save_cml_move[port]) {
                    soc_PORT_TABm_field32_set(unit, &pent, CML_FLAGS_MOVEf,
                                              f_cml->save_cml_move[port]);
                    cml_change = TRUE;
                }
                if (cml_change) {
                    rv = WRITE_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ALL, port, &pent);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                }
            } else
#endif /* BCM_TRX_SUPPORT */
            {
                cml = soc_PORT_TABm_field32_get(unit, &pent, CMLf);
                /* Update mode if necessary. */
                if (cml != f_cml->save_cml[port]) {
                    soc_PORT_TABm_field32_set(unit, &pent, CMLf, f_cml->save_cml[port]);
                    rv = WRITE_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ALL, port, &pent);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                }
            }
        } 
    } 

    /* Regardless of status, decrement frozen count. */
    f_cml->frozen--;

    /* Release protection mutex. */ 
    soc_mem_unlock(unit, PORT_TABm);
    return (rv);
}


/*
 * Function:
 *  	_soc_l2x_frozen_cml_save
 * Purpose:
 *  	Helper function to preserve port learning mode in sw structure.
 * Parameters:
 *   	unit - (IN) BCM device number.
 * Returns:
 *	    SOC_E_XXX
 */
int 
_soc_l2x_frozen_cml_save(int unit)
{
    cml_freeze_t		*f_cml = &cml_freeze_state[unit]; /* Cml freeze state.*/
    soc_port_t		    port;                /* Device port iterator.         */ 
    port_tab_entry_t    pent;                /* Port table hw entry buffer.   */
    int                 rv = SOC_E_NONE;     /* Operation return status.      */

    /* Take protection mutex. */ 
    soc_mem_lock(unit, PORT_TABm);

    /* Freezing l2 save l2 learn mode and disable learning. */
    if (f_cml->frozen == 0) {
        PBMP_E_ITER(unit, port) {
            /* Read port table entry. */
            rv = READ_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ANY, port, &pent);
            if (SOC_FAILURE(rv)) {
                break;
            }
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                f_cml->save_cml[port] =
                    soc_PORT_TABm_field32_get(unit, &pent, CML_FLAGS_NEWf);
                f_cml->save_cml_move[port] =
                    soc_PORT_TABm_field32_get(unit, &pent, CML_FLAGS_MOVEf);

                /* Set bits to the equivilant of PVP_CML_FORWARD */
                soc_PORT_TABm_field32_set(unit, &pent, CML_FLAGS_NEWf, 0x0);
                soc_PORT_TABm_field32_set(unit, &pent, CML_FLAGS_MOVEf, 0x0);
                rv = WRITE_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ALL, port, &pent);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            } else
#endif /* BCM_TRX_SUPPORT */
            {
                /* Save current learning mode in sw structure. */
                f_cml->save_cml[port] = soc_PORT_TABm_field32_get(unit, &pent, CMLf);

                /* Disable L2 learning on the port. */
                if ((f_cml->save_cml[port] == PVP_CML_SWITCH) ||
                    (f_cml->save_cml[port] == PVP_CML_CPU_SWITCH)) {
                    soc_PORT_TABm_field32_set(unit, &pent, CMLf, PVP_CML_FORWARD);
                    rv = WRITE_PORT_TAB_MOD_CHK(unit, MEM_BLOCK_ALL, port, &pent);
                    if (SOC_FAILURE(rv)) {
                        break;
                    }
                } 
            }
        }
    } 

    /* Icrement CML frozen indicator. */
    if (SOC_SUCCESS(rv)) {
        f_cml->frozen++;
    }

    soc_mem_unlock(unit, PORT_TABm);	/* PORT_UNLOCK */
    return(rv);  
}



/*
 * Function:
 *	    soc_l2x_freeze
 * Purpose:
 *   	Temporarily quiesce L2 table from all activity (learning, aging)
 * Parameters:
 *	   unit - (IN) BCM device number. 
 * Returns:
 *	   SOC_E_XXX
 * Notes:
 *	Leaves L2Xm locked until corresponding thaw.
 *	PORT_TABm is locked in order to lockout bcm_port calls
 *	bcm_port calls will callout to soc_arl_frozen_cml_set/get
 */

int
soc_l2x_freeze(int unit)
{
    l2_freeze_t		    *f_l2 = &l2_freeze_state[unit];
    int                 rv = SOC_E_NONE;


    /* Check if already frozen. */
    SOC_L2X_MEM_LOCK(unit);
    if (f_l2->frozen > 0) {
        /* Keep count - do nothing. */
        f_l2->frozen++;
        SOC_L2X_MEM_UNLOCK(unit);
        return (SOC_E_NONE);
    }
    SOC_L2X_MEM_UNLOCK(unit);

    /* Preserve ports learning mode & disable learning. */ 
    SOC_IF_ERROR_RETURN(_soc_l2x_frozen_cml_save(unit));

    /*
     * Lock l2x, disable learning and aging.
     */
    SOC_L2X_MEM_LOCK(unit);

    /* Read l2 aging interval. */
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_get(unit,
                                                &f_l2->save_age_sec,
                                                &f_l2->save_age_ena);
    if (SOC_FAILURE(rv)) {
        SOC_L2X_MEM_UNLOCK(unit);
        _soc_l2x_frozen_cml_restore(unit);
        return rv;
    }
    /* If l2 aging is on - disable it. */
    if (f_l2->save_age_ena) {
        rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, 
                                                    f_l2->save_age_sec, 0);
        if (SOC_FAILURE(rv)) {
            SOC_L2X_MEM_UNLOCK(unit);
            _soc_l2x_frozen_cml_restore(unit);
            return rv;
        }
    }
    /* Increment  l2 frozne indicator. */
    f_l2->frozen++;
    return (SOC_E_NONE);
}


/*
 * Function:
 *   	soc_l2x_thaw
 * Purpose:
 *	    Restore normal L2 activity.
 * Parameters:
 *	    unit - (IN) BCM device number.
 * Returns:
 *	    SOC_E_XXX
 */

int
soc_l2x_thaw(int unit)
{
    l2_freeze_t		    *f_l2 = &l2_freeze_state[unit];
    int                 l2rv, cmlrv;

    /* Sanity check to protect from thaw without freeze. */
    SOC_L2X_MEM_LOCK(unit);
    if (f_l2->frozen == 0 ) {
        SOC_L2X_MEM_UNLOCK(unit);
        assert(0);
    }
    SOC_L2X_MEM_UNLOCK(unit);


    /* In case of nested freeze/thaw just decrement reference counter. */
    if (f_l2->frozen > 1) {
        f_l2->frozen--;
        return (SOC_E_NONE);
    }

    /*
     * Last thaw restore L2 learning and aging.
     */
    l2rv = SOC_E_NONE;
    if (f_l2->save_age_ena) {
        l2rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit,
                                                      f_l2->save_age_sec,
                                                      f_l2->save_age_ena);
    }
    /* L2 freeze reference counter decrement. */
    f_l2->frozen--;
    SOC_L2X_MEM_UNLOCK(unit);

    /* Restore port learning mode. */
    cmlrv = _soc_l2x_frozen_cml_restore(unit);

    if (SOC_FAILURE(l2rv)) {
        return l2rv;
    }
    return cmlrv;
}

/*
 * Function:
 *	soc_l2x_frozen_cml_set
 * Purpose:
 *	Update the saved Cpu Managed Learning mode for a port
 *	if the device is frozen.
 * Parameters:
 *	unit		device number
 *	port		port number
 *	cml		Cpu-Managed-Learning mode to update
 *	repl_cml	CML mode to use in port table update
 * Returns:
 *	SOC_E_NONE	frozen cml is updated
 *	SOC_E_FAIL	unit is not frozen
 *
 *  NOTE: 
 *     PORT_TAB must be locked when calling this routine.
 */
int
soc_l2x_frozen_cml_set(int unit, soc_port_t port, int cml, int *repl_cml)
{
    cml_freeze_t	*f_cml = &cml_freeze_state[unit];

    if (f_cml->frozen) {
        f_cml->save_cml[port] = cml;
        f_cml->save_cml_move[port] = cml;
        if (repl_cml != NULL) {
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                /* Return the equivelant of PVP_CML_FORWARD */
                *repl_cml = 0;
            } else
#endif
            {
                *repl_cml = PVP_CML_FORWARD;
            }
        }
        return SOC_E_NONE;
    }
    return SOC_E_FAIL;
}

/*
 * Function:
 *	soc_l2x_frozen_cml_get
 * Purpose:
 *	Get the saved Cpu Managed Learning mode for a port
 *	if the device is frozen.
 * Parameters:
 *	unit		device number
 *	port		port number
 *	cml		(OUT) saved Cpu-Managed-Learning mode
 * Returns:
 *	SOC_E_NONE	frozen cml is returned
 *	SOC_E_FAIL	unit is not frozen
 *
 *  NOTE: 
 *     PORT_TAB must be locked when calling this routine.
 */
int
soc_l2x_frozen_cml_get(int unit, soc_port_t port, int *cml)
{
    cml_freeze_t	*f_cml = &cml_freeze_state[unit];

    if (f_cml->frozen) {
        *cml = f_cml->save_cml[port];
        return SOC_E_NONE;
    }
    return SOC_E_FAIL;
}


#ifdef BCM_FIREBOLT_SUPPORT
static int
soc_fb_l2x_entries(int unit)
{
    int         index_min, index_max, index, total, rv;
    uint32      *ent;
    uint32      *buf = NULL;


    index_min = soc_mem_index_min(unit, L2Xm);
    index_max = soc_mem_index_max(unit, L2Xm);
    buf = soc_cm_salloc(unit,
                        SOC_MEM_TABLE_BYTES(unit, L2Xm),
                        "l2x_entries");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }

    rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                            index_min, index_max, buf);

    if (rv < 0) {
        soc_cm_sfree(unit, buf);
        return rv;
    }

    total = 0;
    ent = buf;
    for (index = 0; index < (index_max - index_min); index++) {
        if (soc_L2Xm_field32_get(unit, ent, VALIDf)) {
            total++;
        }
        ent += soc_mem_entry_words(unit, L2Xm);
    }

    soc_cm_sfree(unit, buf);

    return total;
}
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
static int
_soc_er_l2x_entries_cb(int unit, uint32 *entry, int index,
                       soc_mem_t tmem, void *data)
{
    /* data points to entry count */
    *((int *) data) += 1;

    return SOC_E_NONE;
}

static int
soc_er_l2x_entries(int unit)
{
    int rv, total = 0;

    rv = soc_er_l2_traverse(unit, _soc_er_l2x_entries_cb, &total);
    if (rv < 0) {
        return rv;
    }

    return total;
}

#endif /* BCM_EASYRIDER_SUPPORT */

/*
 * Function:
 *	soc_l2x_entries
 * Purpose:
 *	Return number of valid entries in L2 table.
 * Parameters:
 *	unit - StrataSwitch unit #
 * Returns:
 *	If >= 0, number of entries in L2 table
 *	If < 0, SOC_E_XXX
 * Notes:
 *	Somewhat slow; has to read whole table.
 *	New improved: Now with DMA power.
 */

int
soc_l2x_entries(int unit)
{
    int			index_min, index_max, total, rv;
    l2x_valid_entry_t   *buf, *bufp;
    uint32              bucket_bmap;
    uint32              count, index; 

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return soc_fb_l2x_entries(unit);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        return soc_er_l2x_entries(unit);
    }
#endif /* BCM_EASYRIDER_SUPPORT */

    index_min = soc_mem_index_min(unit, L2X_VALIDm);
    index_max = soc_mem_index_max(unit, L2X_VALIDm);
    count = soc_mem_index_count(unit, L2X_VALIDm);

    /* Allocate dma-able buffer; DMA the valid bits table. */
    buf = soc_cm_salloc(unit,
                        SOC_MEM_TABLE_BYTES(unit, L2X_VALIDm),
                        "soc_l2x_entries");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }

    rv = soc_mem_read_range(unit, L2X_VALIDm, MEM_BLOCK_ANY,
                            index_min, index_max, buf);

    if (rv < 0) {
        soc_cm_sfree(unit, buf);
        return rv;
    }

    total = 0;

    for (index = 0; index < count; index++) {
        bufp = soc_mem_table_idx_to_pointer(unit, L2X_VALIDm,
                                    l2x_valid_entry_t *, buf, index);
        bucket_bmap = soc_mem_field32_get(unit, L2X_VALIDm, bufp, BUCKET_BITMAPf);
        total += _shr_popcount(bucket_bmap & 0xff);
    }

    soc_cm_sfree(unit, buf);

    return total;
}

/*
 * Function:
 *	soc_l2x_hash
 * Purpose:
 *	Return hash bucket into which L2 entry belongs, according to HW
 * Parameters:
 *	unit - StrataSwitch unit #
 *	entry - L2X entry to hash
 * Returns:
 *	If >= 0, hash bucket number
 *	If < 0, SOC_E_XXX
 */

int
soc_l2x_hash(int unit, l2x_entry_t *entry)
{
    hashinput_entry_t		hent;
    uint32			key[SOC_MAX_MEM_WORDS];
    uint32			result;
    sal_mac_addr_t		mac;

    sal_memset(key, 0, sizeof(key));
    soc_L2Xm_mac_addr_get(unit, entry, MAC_ADDRf, mac);
    SAL_MAC_ADDR_TO_UINT32(mac, key);
    key[1] |= soc_L2Xm_field32_get(unit, entry, VLAN_IDf) << 16;

    soc_HASHINPUTm_field32_set(unit, &hent, KEY_TYPEf, XGS_HASH_KEY_TYPE_L2);
    soc_HASHINPUTm_field_set(unit, &hent, KEYf, key);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, HASHINPUTm, MEM_BLOCK_ALL,
                                      0, &hent));

    SOC_IF_ERROR_RETURN(READ_HASH_OUTPUTr(unit, &result));

    return (int)result;
}

/*
 * Function:
 *	soc_l2x_software_hash
 * Purpose:
 *	Return hash bucket into which L2 entry belongs, according to software
 * Parameters:
 *	unit - StrataSwitch unit #
 *	hash_select - Which hash method to use; one of XGS_HASH_xxx
 *	entry - L2X entry to hash
 * Returns:
 *	If >= 0, hash bucket number
 *	If < 0, SOC_E_XXX
 */
int
soc_l2x_software_hash(int unit, int hash_select, l2x_entry_t *entry)
{
    uint8	key[XGS_HASH_KEY_SIZE];

    soc_draco_l2x_base_entry_to_key(unit, entry, key);

    return soc_draco_l2_hash(unit, hash_select, key);
}

#ifdef BCM_FIREBOLT_SUPPORT
/*
 * Function:
 *      soc_fb_l2x_bank_lookup
 * Purpose:
 *      Send an L2 lookup message over the S-Channel and receive the
 *      response.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      banks - For dual hashing, which halves are selected (inverted)
 *      key - L2X entry to look up; only MAC+VLAN fields are relevant
 *      result - L2X entry to receive entire found entry
 *      index_ptr (OUT) - If found, receives table index where found
 * Returns:
 *      SOC_E_INTERNAL if retries exceeded or other internal error
 *      SOC_E_NOT_FOUND if the entry is not found.
 *      SOC_E_NONE (0) on success (entry found):
 * Notes:
 *      The S-Channel response contains either a matching L2 entry,
 *      or an entry with a key of all f's if not found.  It is okay
 *      if the result pointer is the same as the key pointer.
 *      Retries the lookup in cases where the particular chip requires it.
 */

int
soc_fb_l2x_bank_lookup(int unit, uint8 banks,
               l2x_entry_t *key, l2x_entry_t *result,
               int *index_ptr)
{
    schan_msg_t    schan_msg;
    int entry_dw = soc_mem_entry_words(unit, L2Xm);
    int nbits;
    int rv;

    schan_msg_clear(&schan_msg);
    schan_msg.l2x2.header.opcode = L2_LOOKUP_CMD_MSG;
    schan_msg.l2x2.header.srcblk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    schan_msg.l2x2.header.dstblk = SOC_BLOCK2SCH(unit, IPIPE_BLOCK(unit));
    schan_msg.l2x2.header.cos = banks & 0x3;
    schan_msg.l2x2.header.datalen = entry_dw * 4;

    /* Fill in entry data */

    sal_memcpy(schan_msg.l2x2.data, key, sizeof (schan_msg.l2x2.data));

    /*
     * Write onto S-Channel "L2 lookup" command packet consisting of
     * header word + three words of L2 key, and read back header
     * word + 4 words of lookup result. (index of the extry + contents of
     * the entry)
     */

    rv = soc_schan_op(unit, &schan_msg, entry_dw + 1, entry_dw + 2, 1);

    /* Check result */

    if (schan_msg.readresp.header.opcode != L2_LOOKUP_ACK_MSG) {
        soc_cm_debug(DK_ERR,
                     "soc_fb_l2x_bank_lookup: "
                     "invalid S-Channel reply, expected L2_LOOKUP_ACK_MSG:\n");
        soc_schan_dump(unit, &schan_msg, entry_dw + 2);
        return SOC_E_INTERNAL;
    }

    /*
     * Fill in result entry from read data.
     *
     * Format of S-Channel response:
     *          readresp.header : L2 lookup Ack S-channel header
     *          readresp.data[0]: index of the entry in the L2Xm Table
     *          readresp.data[1-3]: entry contents
     * =============================================================
     * | PERR_PBM | MFIFO_FULL | OP_FAIL | Index  | L2x entry data |
     * =============================================================
     */
    nbits = soc_mem_entry_bits(unit, L2Xm) % 32; /* Data Bits in last word */

    if ((schan_msg.readresp.header.cpu) || (rv == SOC_E_FAIL)) {
        *index_ptr = -1;
        if (soc_feature(unit, soc_feature_l2x_parity)) {
            int op_fail_pos;
            op_fail_pos = SOC_L2X_OP_FAIL_POS(unit);
            if ((schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff) {
                uint32 index = (schan_msg.readresp.data[2] >> nbits) &
                               ((1 << (32 - nbits)) - 1);
                index |= (schan_msg.readresp.data[3] << (32 - nbits)) &
                         soc_mem_index_max(unit, L2Xm); /* Assume size of table 2^N */
                soc_cm_debug(DK_ERR,
                    "Lookup table[L2Xm]: Parity Error Index %d Bucket Bitmap 0x%08x\n",
                     index,
                    (schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff );
                return SOC_E_INTERNAL;
            }
        }
        return SOC_E_NOT_FOUND;
    }

    /* MACADDR<31:0> */
    result->entry_data[0] = schan_msg.readresp.data[0];

    /* CPU, PRI<2:0>, VLAN<12:0>, MACADDR<47:32> */
    result->entry_data[1] = schan_msg.readresp.data[1];

    /* HIT SA, HIT DA, VALID, MIRROR, RPE, STATIC_BIT, RESERVED,
     * MAC_BLOCK_INDEX, L3 MODULE_ID, PORT_TGID, SCP, SRC_DISCARD,
     * DST_DISCARD
     */
    result->entry_data[2] = schan_msg.readresp.data[2] & ((1 << nbits) - 1);

    *index_ptr = (schan_msg.readresp.data[2] >> nbits) &
                 ((1 << (32 - nbits)) - 1);
    *index_ptr |= (schan_msg.readresp.data[3] << (32 - nbits));
    *index_ptr &= soc_mem_index_max(unit, L2Xm); /* Assume size of table 2^N */
    if (soc_cm_debug_check(DK_SOCMEM)) {
        soc_cm_debug(DK_SOCMEM, "L2 entry lookup: ");
        soc_mem_entry_dump(unit, L2Xm, result);
        soc_cm_debug(DK_SOCMEM, " (index=%d)\n", *index_ptr);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_fb_l2x_bank_insert
 * Purpose:
 *      Insert an entry into the L2X hash table.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      banks - For dual hashing, which halves are selected (inverted)
 *      entry - L2X entry to insert
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_FULL - hash bucket full
 *      SOC_E_BUSY - modfifo full
 * Notes:
 *      Uses hardware insertion; sends an L2 INSERT message over the
 *      S-Channel.  The hardware needs the L2Xm entry type.
 */

int
soc_fb_l2x_bank_insert(int unit, uint8 banks, l2x_entry_t *entry)
{
    schan_msg_t 	schan_msg;
    int			rv;
    int                 op_fail_pos;
    int entry_dw = soc_mem_entry_words(unit, L2Xm);
    int nbits;

    if (soc_cm_debug_check(DK_SOCMEM)) {
        soc_cm_debug(DK_SOCMEM, "Insert table[L2_ENTRY]: ");
        soc_mem_entry_dump(unit, L2Xm, entry);
        soc_cm_debug(DK_SOCMEM, "\n");
    }

    schan_msg_clear(&schan_msg);
    schan_msg.l2x2.header.opcode = ARL_INSERT_CMD_MSG;
    schan_msg.l2x2.header.srcblk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    schan_msg.l2x2.header.dstblk = SOC_BLOCK2SCH(unit, IPIPE_BLOCK(unit));
    schan_msg.l2x2.header.cos = banks & 0x3;
    schan_msg.l2x2.header.datalen = entry_dw * 4;

    /* Fill in ARL packet data */

    sal_memcpy(schan_msg.l2x2.data, entry, sizeof (schan_msg.l2x2.data));

    /* Execute S-Channel operation (header word + 3 DWORDs) */

    rv = soc_schan_op(unit, &schan_msg, entry_dw + 1, entry_dw + 2, 1);

    if (schan_msg.readresp.header.opcode != ARL_INSERT_DONE_MSG) {
        soc_cm_debug(DK_ERR,
                     "soc_fb_l2x_bank_insert: "
                     "invalid S-Channel reply, expected L2_INSERT_ACK_MSG:\n");
        soc_schan_dump(unit, &schan_msg, 1);
        return SOC_E_INTERNAL;
    }

    /*
     * =============================================================
     * | PERR_PBM | MFIFO_FULL | OP_FAIL | Index  | L2x entry data |
     * =============================================================
     */
    nbits = soc_mem_entry_bits(unit, L2Xm) % 32; /* Data Bits in last word */
    op_fail_pos = SOC_L2X_OP_FAIL_POS(unit);

    /* bit-106 OP_FAIL; bit-107 ModFifo Full */
    if ((schan_msg.readresp.header.cpu) || (rv == SOC_E_FAIL)) {
        if (schan_msg.readresp.data[3] & (1 << op_fail_pos)) {
            soc_cm_debug(DK_SOCMEM,
                "Insert table[L2Xm]: hash bucket full\n");
            rv = SOC_E_FULL;
        } else if (schan_msg.readresp.data[3] & (1 << (op_fail_pos + 1))) {
            soc_cm_debug(DK_SOCMEM,
                "Insert table[L2Xm]: Modfifo full\n");
            rv = SOC_E_BUSY;
        } else if (soc_feature(unit, soc_feature_l2x_parity) &&
                   ((schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff)) {
            uint32 index = (schan_msg.readresp.data[2] >> nbits) &
                           ((1 << (32 - nbits)) - 1);
            index |= (schan_msg.readresp.data[3] << (32 - nbits)) &
                     soc_mem_index_max(unit, L2Xm); /* Assume size of table 2^N */
            soc_cm_debug(DK_ERR,
                "Insert table[L2Xm]: Parity Error Index %d Bucket Bitmap 0x%08x\n",
                 index,
                (schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff );
            rv = SOC_E_INTERNAL;
        } else {
            rv = SOC_E_FAIL;
        }
    }

    return rv;
}

/*
 * Function:
 *	soc_fb_l2x_bank_delete
 * Purpose:
 *	Delete an entry from the L2X hash table.
 * Parameters:
 *	unit - StrataSwitch unit #
 *      banks - For dual hashing, which halves are selected (inverted)
 *	entry - L2X entry to delete
 * Returns:
 *	SOC_E_NONE - success
 *	SOC_E_NOT_FOUND - hash bucket full
 *      SOC_E_BUSY - modfifo full
 * Notes:
 *	Uses hardware deletion; sends an L2 DELETE message over the
 *	S-Channel.  The hardware needs the L2Xm entry type.
 */

int
soc_fb_l2x_bank_delete(int unit, uint8 banks, l2x_entry_t *entry)
{
    schan_msg_t 	schan_msg;
    int			rv;
    int                 op_fail_pos;
    int                 nbits;
    int entry_dw = soc_mem_entry_words(unit, L2Xm);

    if (soc_cm_debug_check(DK_SOCMEM)) {
        soc_cm_debug(DK_SOCMEM, "Delete table[L2Xm]: ");
        soc_mem_entry_dump(unit, L2Xm, entry);
        soc_cm_debug(DK_SOCMEM, "\n");
    }

    schan_msg_clear(&schan_msg);
    schan_msg.l2x2.header.opcode = ARL_DELETE_CMD_MSG;
    schan_msg.l2x2.header.srcblk = SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    schan_msg.l2x2.header.dstblk = SOC_BLOCK2SCH(unit, IPIPE_BLOCK(unit));
    schan_msg.l2x2.header.cos = banks & 0x3;
    schan_msg.l2x2.header.datalen = entry_dw * 4;

    /* Fill in packet data */

    sal_memcpy(schan_msg.l2x2.data, entry, sizeof (schan_msg.l2x2.data));

    /* Execute S-Channel operation (header word + 2 DWORDs) */
    rv = soc_schan_op(unit, &schan_msg, entry_dw + 1, entry_dw + 2, 1);

    if (schan_msg.readresp.header.opcode != ARL_DELETE_DONE_MSG) {
        soc_cm_debug(DK_ERR,
                     "soc_fb_l2x_bank_delete: "
                     "invalid S-Channel reply, expected L2_DELETE_ACK_MSG:\n");
        soc_schan_dump(unit, &schan_msg, 1);
        return SOC_E_INTERNAL;
    }

    /*
     * =============================================================
     * | PERR_PBM | MFIFO_FULL | OP_FAIL | Index  | L2x entry data |
     * =============================================================
     */
    nbits = soc_mem_entry_bits(unit, L2Xm) % 32; /* Data Bits in last word */
    op_fail_pos = SOC_L2X_OP_FAIL_POS(unit);

    /* bit-106 OP_FAIL; bit-107 ModFifo Full */
    if ((schan_msg.readresp.header.cpu) || (rv == SOC_E_FAIL)) {
        if (schan_msg.readresp.data[3] & (1 << op_fail_pos)) {
            soc_cm_debug(DK_SOCMEM,
                "Delete table[L2Xm]: Not found\n");
            rv = SOC_E_NOT_FOUND;
        } else if (schan_msg.readresp.data[3] & (1 << (op_fail_pos + 1))) {
            soc_cm_debug(DK_SOCMEM,
                "Delete table[L2Xm]: Modfifo full\n");
            rv = SOC_E_BUSY;
        } else if (soc_feature(unit, soc_feature_l2x_parity) &&
                   ((schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff)) {
            uint32 index = (schan_msg.readresp.data[2] >> nbits) &
                           ((1 << (32 - nbits)) - 1);
            index |= (schan_msg.readresp.data[3] << (32 - nbits)) &
                     soc_mem_index_max(unit, L2Xm); /* Assume size of table 2^N */
            soc_cm_debug(DK_ERR,
                "Delete table[L2Xm]: Parity Error Index %d Bucket Bitmap 0x%08x\n",
                 index,
                (schan_msg.readresp.data[3] >> (op_fail_pos + 2)) & 0xff );
            rv = SOC_E_INTERNAL;
        } else {
            rv = SOC_E_FAIL;
        }
    }

    return rv;
}

/*
 * Original non-bank versions of the FB L2X table op functions
 */

int
soc_fb_l2x_insert(int unit, l2x_entry_t *entry)
{
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    if (soc_feature(unit, soc_feature_dual_hash)) {
        return
            _soc_mem_dual_hash_insert(unit, L2Xm, COPYNO_ALL, (void *)entry,
                          NULL, SOC_CONTROL(unit)->dual_hash_recurse_depth);
    } else
#endif
    {
        return soc_fb_l2x_bank_insert(unit, 0, entry);
    }
}

int
soc_fb_l2x_delete(int unit, l2x_entry_t *entry)
{
    return soc_fb_l2x_bank_delete(unit, 0, entry);
}

int
soc_fb_l2x_lookup(int unit, l2x_entry_t *key,
                  l2x_entry_t *result, int *index_ptr)
{
    return soc_fb_l2x_bank_lookup(unit, 0, key, result, index_ptr);
}

/*
 * Function:
 *	soc_fb_l2x_delete_all
 * Purpose:
 *	Clear the L2 table by invalidating entries.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	static_too - if TRUE, delete static and non-static entries;
 *		     if FALSE, delete only non-static entries
 * Returns:
 *	SOC_E_XXX
 */

int
soc_fb_l2x_delete_all(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			    index_min, index_max, index, mem_max;
    l2x_entry_t     *l2x_entry;
    int			rv = SOC_E_NONE;
    int             *buffer = NULL;
    int             mem_size, idx;

    index_min = soc_mem_index_min(unit, L2_ENTRY_ONLYm);
    mem_max = soc_mem_index_max(unit, L2_ENTRY_ONLYm);
    mem_size =  DEFAULT_L2DELETE_CHUNKS * sizeof(l2x_entry_t);

    buffer = soc_cm_salloc(unit, mem_size, "L2X_delete");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }

    soc_mem_lock(unit, L2Xm);
    for (idx = index_min; idx < mem_max; idx += DEFAULT_L2DELETE_CHUNKS) {
        index_max = idx + DEFAULT_L2DELETE_CHUNKS - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ANY,
                                     idx, index_max, buffer)) < 0 ) {
            break;
        }
        for (index = 0; index < index_max - idx + 1; index++) {
            l2x_entry =
                soc_mem_table_idx_to_pointer(unit, L2_ENTRY_ONLYm,
                                             l2x_entry_t *, buffer, index);
                sal_memcpy(l2x_entry, soc_mem_entry_null(unit, L2_ENTRY_ONLYm),
                           sizeof(l2x_entry_t));
        }
        if ((rv = soc_mem_write_range(unit, L2_ENTRY_ONLYm, MEM_BLOCK_ALL, 
                                     idx, index_max, buffer)) < 0) {
            break;
        }
    }
    soc_mem_unlock(unit, L2Xm);

    if (soc->arlShadow != NULL) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }
    soc_cm_sfree(unit, buffer);
    

    return rv;
}

#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
/*
 * Function:
 *      soc_er_l2x_lookup
 * Purpose:
 *      Use command memory op to search L2 tables.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      key - L2X entry to look up; only MAC+VLAN fields are relevant
 *      result - L2X entry to receive entire found entry
 *      index_ptr (OUT) - If found, receives table index where found
 *      mem (OUT) - The table in which the entry was found.
 * Returns:
 *      SOC_E_INTERNAL if retries exceeded or other internal error
 *      SOC_E_NOT_FOUND if the entry is not found.
 *      SOC_E_NONE (0) on success (entry found):
 * Notes:
 *      It is okay if the result pointer is the same as the key pointer.
 */

int
soc_er_l2x_lookup(int unit,
                  l2_entry_internal_entry_t *key,
                  l2_entry_internal_entry_t *result,
                  int *index_ptr, soc_mem_t *mem)
{
    soc_mem_cmd_t cmd_info;
    uint32        hash_addr, res_table;
    int           rv;

    sal_memset(&cmd_info, 0, sizeof(cmd_info));
    cmd_info.command = SOC_MEM_CMD_SEARCH;
    cmd_info.entry_data = key;
    rv = soc_mem_cmd_op(unit, L2_ENTRY_INTERNALm, &cmd_info, FALSE);
    if (rv == SOC_E_NOT_FOUND) {
        *index_ptr = -1;
    } else {
        sal_memcpy(result, cmd_info.output_data,
                   soc_mem_entry_words(unit, L2_ENTRY_INTERNALm) *
                   sizeof (uint32));
        hash_addr = cmd_info.addr0;

        res_table = (hash_addr >> SOC_MEM_CMD_HASH_TABLE_SHIFT) & 
            SOC_MEM_CMD_HASH_TABLE_MASK;
        hash_addr &= SOC_MEM_CMD_HASH_ADDR_MASK;
        /* More L2 work needed here */
        /* Adjust HW output */
        if (res_table == SOC_MEM_CMD_HASH_TABLE_L2EXT) {
            *mem = L2_ENTRY_EXTERNALm;
            hash_addr = 
                ((hash_addr & SOC_MEM_CMD_EXT_L2_ADDR_MASK_HI) >> 
                 SOC_MEM_CMD_EXT_L2_ADDR_SHIFT_HI) |
                (hash_addr & SOC_MEM_CMD_EXT_L2_ADDR_MASK_LO);
        } else if (res_table == SOC_MEM_CMD_HASH_TABLE_L2OVR) {
            *mem = L2_ENTRY_OVERFLOWm;
        } else if (res_table == SOC_MEM_CMD_HASH_TABLE_DFLT) {
            *mem = L2_ENTRY_INTERNALm;
        } else {
            soc_cm_debug(DK_WARN, 
                         "soc_er_l2x_lookup, unit %d: "
                         "Unknown L2 table referenced\n",
                         unit);
        }
        *index_ptr = hash_addr & SOC_MEM_CMD_HASH_ADDR_MASK;
    }
    return rv;
}

/*
 * Function:
 *	soc_er_l2x_delete_all
 * Purpose:
 *	Clear the L2 table by invalidating entries.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	static_too - if TRUE, delete static and non-static entries;
 *		     if FALSE, delete only non-static entries
 * Returns:
 *	SOC_E_XXX
 */

int
soc_er_l2x_delete_all(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			rv = SOC_E_NONE;

    soc_mem_lock(unit, L2_ENTRY_INTERNALm);

    if ((rv = soc_er_l2x_delete_by(unit, 0, 0, 0, 0, 0,
                                   SOC_ER_L2_ALL_DEL, TRUE)) < 0) {
        goto done;
    }

    if ((soc->arlShadow != NULL)) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }
    
 done:
    soc_mem_unlock(unit, L2_ENTRY_INTERNALm);


    return rv;
}

/*
 * Function:
 *	soc_er_l2x_delete_by
 * Purpose:
 *	Remove/replace entries with matching parameters from the L2 tables.
 * Parameters:
 *	unit - StrataSwitch unit #
 *	static_too - if TRUE, affect static and non-static entries;
 *		     if FALSE, affect only non-static entries
 * Returns:
 *	SOC_E_XXX
 */

int
soc_er_l2x_delete_by(int unit, uint32 modid, uint32 tgid_port, uint32 vlan,
                     uint32 repl_modid, uint32 repl_tgid_port,
                     uint32 op, int static_too)
{
    soc_mem_cmd_t cmd_info;
    command_memory_hse_entry_t *cmd_entry;

    sal_memset(&cmd_info, 0, sizeof(cmd_info));
    cmd_entry = cmd_info.entry_data = cmd_info.output_data;
    cmd_info.command = SOC_MEM_CMD_PER_PORT_AGING;

    if ((int)modid > SOC_MODID_MAX(unit)){
        return SOC_E_BADID;
    }

    /* Transfer parameters */
    soc_mem_field32_set(unit, COMMAND_MEMORY_HSEm, (uint32 *) cmd_entry,
                        PPA_MODEf, op & SOC_ER_L2_DELETE_BY_MASK);
    soc_mem_field32_set(unit, COMMAND_MEMORY_HSEm, (uint32 *) cmd_entry,
                        VLAN_IDf, vlan);
    soc_mem_field32_set(unit, COMMAND_MEMORY_HSEm, (uint32 *) cmd_entry,
                        MODULE_IDf, modid);
    soc_mem_field32_set(unit, COMMAND_MEMORY_HSEm, (uint32 *) cmd_entry,
                        TGID_PORTf, tgid_port);
    soc_mem_field32_set(unit, COMMAND_MEMORY_HSEm, (uint32 *) cmd_entry,
                        REPLACE_MODULE_IDf, repl_modid);
    soc_mem_field32_set(unit, COMMAND_MEMORY_HSEm, (uint32 *) cmd_entry,
                        REPLACE_PORT_TGIDf, repl_tgid_port);
    soc_mem_field32_set(unit, COMMAND_MEMORY_HSEm, (uint32 *) cmd_entry,
                        EXCL_STATICf, static_too ? 0 : 1);

    soc_mem_cmd_op(unit, L2_ENTRY_INTERNALm, &cmd_info, TRUE);

    /*
     * For APIs using PPA_MODE, hardware does not update the COMMAND_WORD
     * field after performing the operation. So return value for these APIs
     * is always SOC_E_NONE.
     */
    return SOC_E_NONE;
}

STATIC int
soc_er_l2_traverse_mem(int unit, soc_er_l2_cb_fn l2_op_fn, 
                       soc_mem_t tmem, void *fn_data)
{
    int  idx_min, idx_max, jx;
    int  chunksize, nchunks, chunk_bottom, chunk_top, chunk;
    int  mem_count, rv = SOC_E_NONE;
    uint32 *l2_entry, *l2_buf = NULL;

    switch (tmem) {
        case L2_ENTRY_EXTERNALm:
        case L2_ENTRY_INTERNALm:
        case L2_ENTRY_OVERFLOWm:
            break;
        default: 
            return SOC_E_PARAM;
    }

    /* no hardware acceleration */
    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
				 SOC_ER_L2_CHUNKS_DEFAULT);

    /*
     * L2 internal has larger entry size than the other two,
     * so allocate based on its size.
     */
    l2_buf = soc_cm_salloc(unit,
                           WORDS2BYTES(soc_mem_entry_words(unit,
                                       L2_ENTRY_INTERNALm)) * chunksize,
                           "l2x_traverse_entries");
    if (l2_buf == NULL) {
        return SOC_E_MEMORY;
    }

    mem_count = soc_mem_index_count(unit, tmem);
    if (mem_count > 0) {
        idx_min = soc_mem_index_min(unit, tmem);
        idx_max = soc_mem_index_max(unit, tmem);

        nchunks = (mem_count + chunksize - 1) / chunksize;
        for (chunk = 0; chunk < nchunks; chunk++) {
            chunk_bottom = idx_min + (chunk * chunksize);
            chunk_top = chunk_bottom + (chunksize - 1);
            if (idx_max < chunk_top) {
                chunk_top = idx_max;
            }
            rv = soc_mem_read_range(unit, tmem, MEM_BLOCK_ANY,
                                    chunk_bottom, chunk_top, l2_buf);

            for (jx = chunk_bottom; jx <= chunk_top; jx++) {
                l2_entry =
                    soc_mem_table_idx_to_pointer(unit, tmem, uint32 *, l2_buf, 
                                                 jx - chunk_bottom);
                if (!soc_mem_field32_get(unit, tmem, l2_entry, VALID_BITf)) {
                    continue;
                }

                if (!soc_mem_field32_get(unit, tmem, l2_entry, ACTIVEf)) {
                    continue;
                }

                if ((rv = (*l2_op_fn)(unit, l2_entry, jx,
                                      tmem, fn_data)) < 0) {
                    break;
                }
            }

            if (rv < 0) {
                break;
            }
        }
    }

    if (l2_buf) {
        soc_cm_sfree(unit, l2_buf);
    }

    return rv;
}

int
soc_er_l2_traverse(int unit, soc_er_l2_cb_fn l2_op_fn, void *fn_data)
{
    int rv;
    rv = soc_er_l2_traverse_mem(unit, l2_op_fn, 
                                L2_ENTRY_EXTERNALm, fn_data);
    if (rv == SOC_E_NONE) {
        rv = soc_er_l2_traverse_mem(unit, l2_op_fn,
                                    L2_ENTRY_INTERNALm, fn_data);
    }
    if (rv == SOC_E_NONE) {
        rv = soc_er_l2_traverse_mem(unit, l2_op_fn,
                                    L2_ENTRY_OVERFLOWm, fn_data);
    }
    return rv;
}

/*
 * Function:
 *	soc_er_l2x_entries_external
 * Purpose:
 *	Return number of valid entries in ER External L2 table.
 * Parameters:
 *	unit - StrataSwitch unit #
 * Returns:
 *	If >= 0, number of entries in L2 table
 *	If < 0, SOC_E_XXX
 * Notes:
 */

int
soc_er_l2x_entries_external(int unit)
{
    int rv, total = 0;

    rv = soc_er_l2_traverse_mem(unit, _soc_er_l2x_entries_cb, 
                                L2_ENTRY_EXTERNALm, &total);
    if (rv < 0) {
        return rv;
    }

    return total;
}

/*
 * Function:
 *	soc_er_l2x_entries_internal
 * Purpose:
 *	Return number of valid entries in ER Internal L2 table.
 * Parameters:
 *	unit - StrataSwitch unit #
 * Returns:
 *	If >= 0, number of entries in L2 table
 *	If < 0, SOC_E_XXX
 * Notes:
 */

int
soc_er_l2x_entries_internal(int unit)
{
    int rv, total = 0;

    rv = soc_er_l2_traverse_mem(unit, _soc_er_l2x_entries_cb, 
                                L2_ENTRY_INTERNALm, &total);
    if (rv < 0) {
        return rv;
    }

    return total;
}

/*
 * Function:
 *	soc_er_l2x_entries_overflow
 * Purpose:
 *	Return number of valid entries in ER Overflow L2 table.
 * Parameters:
 *	unit - StrataSwitch unit #
 * Returns:
 *	If >= 0, number of entries in L2 table
 *	If < 0, SOC_E_XXX
 * Notes:
 */

int
soc_er_l2x_entries_overflow(int unit)
{
    int rv, total = 0;

    rv = soc_er_l2_traverse_mem(unit, _soc_er_l2x_entries_cb, 
                                L2_ENTRY_OVERFLOWm, &total);
    if (rv < 0) {
        return rv;
    }

    return total;
}

#endif /* BCM_EASYRIDER_SUPPORT */

#endif /* BCM_XGS_SWITCH_SUPPORT */
