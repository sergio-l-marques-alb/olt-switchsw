/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  diag.c
*
* @purpose   Diagnostic functions modified from the Broadcom SDK.
*
* @component
*
* @create    08/02/2006
*
* @author    Brady Rutherford
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "log.h"

#include "sal/appl/io.h"
#include "appl/cputrans/next_hop.h"
#include "appl/cputrans/atp.h"
#include "appl/stktask/stktask.h"
#include "appl/stktask/topology.h"
#include "appl/stktask/topo_pkt.h"
#include "appl/stktask/attach.h"


/* start - code from Broadcom SDK5 file : $Id: tksdiag.c,v 1.2.2.35 2003/12/04 02:32:18 dtalayco */
/* lines 1555 - 1575 */
/****************************************************************
 *
 * StackTask thread
 *
 ****************************************************************/

#define TOPO_ATP_FLAGS (ATP_F_NEXT_HOP | ATP_F_REASSEM_BUF | ATP_F_NO_ACK)

static sal_thread_t st_tid = SAL_THREAD_ERROR;

STATIC void
tks_st_thread(void *cookie)
{
  bcm_st_config_t *cfg;
  int rv;

  cfg = (bcm_st_config_t *)cookie;

  rv = topo_pkt_handle_init(TOPO_ATP_FLAGS);
  if (rv < 0) {
        PT_LOG_ERR(LOG_CTX_STARTUP,"WARNING: topo pkt handle init returned %s\n", bcm_errmsg(rv));
        LOG_ERROR (rv);
  }

  if ((rv=bcm_stack_attach_init()) != BCM_E_NONE) {
    PT_LOG_ERR(LOG_CTX_STARTUP,"WARNING: bcm_stack_attach_init returned %s\n", bcm_errmsg(rv));
  }

  rv = bcm_st_start(cfg, TRUE);
  PT_LOG_INFO(LOG_CTX_STARTUP,"st thread exitted with value %d\n", rv);
  st_tid = SAL_THREAD_ERROR;
  sal_thread_exit(rv);
}
/* end - code from Broadcom SDK5 file : $Id: tksdiag.c,v 1.2.2.35 2003/12/04 02:32:18 dtalayco */


#ifndef L7_BCM_SHELL_SUPPORT 
/* The show functions were initially pulled from DIAG code. Previouly, DIAG was not linked in.
 * Now, DIAG code is linked in for platforms which have enough memory. Use the diag version,
 * if linked. Else, this copy gets linked.
 */
/* start - code from Broadcom SDK5 file : $Id: tksdiag.c,v 1.2.2.35 2003/12/04 02:32:18 dtalayco */
/* lines 53 - 139 */
STATIC INLINE void
db_entry_show(const cpudb_entry_t *entry)
{
    const cpudb_stk_port_t *sp_p;
    int		i;
    char	keybuf[CPUDB_KEY_STRING_LEN];
    const cpudb_unit_port_t *sp_base;

    cpudb_key_format(entry->base.key, keybuf, sizeof(keybuf));
    printf("    Key: %s   MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
	   keybuf,
           entry->base.mac[0], entry->base.mac[1], entry->base.mac[2],
           entry->base.mac[3], entry->base.mac[4], entry->base.mac[5]);
    printf("    Flags 0x%x:%s%s%s%s%s%s%s%s%s%s\n",
           entry->flags,
           entry->flags & CPUDB_F_IS_MASTER ? " master" : "",
           entry->flags & CPUDB_F_IS_LOCAL ? " local" : "",
           entry->flags & CPUDB_F_TX_KNOWN ? " tx-known" : "",
           entry->flags & CPUDB_F_SYSID_KNOWN ? " sysid-known" : "",
           entry->flags & CPUDB_F_DEST_KNOWN ? " dest-known" : "",
           entry->flags & CPUDB_F_BASE_INIT_DONE ? " base-init" : "",
           entry->flags & CPUDB_F_GLOBAL_COMPLETE ? " gcomp" : "",
           entry->flags & CPUDB_F_INACTIVE_MARKED ? " inact" : "",
           entry->flags & CPUDB_F_CONFIG_IN ? " cfg in" : "",
           entry->flags & CPUDB_F_LOCAL_COMPLETE ? " lcomp" : "");
    printf("    units %d. dest unit %d. base dest port %d. "
           "dest mod %d dest port %d.\n",
           entry->base.num_units,
           entry->base.dest_unit,
           entry->base.dest_port,
           entry->dest_mod,
           entry->dest_port);
    for (i = 0; i < entry->base.num_units; i++) {
        printf("        unit %d: mod id req %d; mod id pref %d\n",
               i, entry->base.mod_ids_req[i], entry->base.pref_mod_id[i]);
    }
    printf("    master prio %d. slot id %d. discovery seq %d.\n",
           entry->base.master_pri,
           entry->base.slot_id,
           entry->base.dseq_num);
    printf("    dest mod,port %d,%d. tx unit,port %d,%d. num stk %d\n",
           entry->dest_mod,
           entry->dest_port,
           entry->tx_unit,
           entry->tx_port,
           entry->base.num_stk_ports);
    printf("    trans pointers %p.\n", (void *)entry->trans_ptr);
    for (i = 0; i < entry->base.num_units; i++) {
        printf("        unit %d: base mod id %d\n", i, entry->mod_ids[i]);
    }
    for (i = 0; i < entry->base.num_stk_ports; i++) {
        sp_p = &entry->sp_info[i];
        sp_base = &entry->base.stk_ports[i];
        printf("        StkPort %d:  Unit %d. Port %d. "
               "Flags 0x%x:%s%s%s%s%s%s; \n",
               i, sp_base->unit, sp_base->port,
               sp_p->flags,
	       sp_p->flags & CPUDB_SPF_NO_LINK ? " nolink" : "",
	       sp_p->flags & CPUDB_SPF_INACTIVE ? " inactive" : "",
	       sp_p->flags & CPUDB_SPF_DUPLEX ? " duplex" : "",
	       sp_p->flags & CPUDB_SPF_CUT_PORT ? " cut_port" : "",
	       sp_p->flags & CPUDB_SPF_TX_RESOLVED ? " txok" : "",
	       sp_p->flags & CPUDB_SPF_RX_RESOLVED ? " rxok" : "");
        printf("            Weight %u. Base Flags 0x%x:%s.\n",
	       sp_base->weight,
	       sp_base->bflags,
	       sp_base->bflags & CPUDB_UPF_DISABLE_IF_CUT ? " dis_cut" : "");
	cpudb_key_format(sp_p->tx_cpu_key, keybuf, sizeof(keybuf));
	if (CPUDB_KEY_COMPARE(sp_p->tx_cpu_key, sp_p->rx_cpu_key) == 0 &&
	    sp_p->tx_stk_idx == sp_p->rx_stk_idx) {
	    if (sp_p->tx_stk_idx != 0) {
		printf("            TX/RX Key: %s (stkport %d)\n",
		       keybuf, sp_p->tx_stk_idx);
	    } else {
		printf("            TX/RX Key: %s\n",
		       keybuf);
	    }
	} else {
	    if (sp_p->tx_stk_idx != 0) {
		printf("            TX Key: %s (stkport %d)\n",
		       keybuf, sp_p->tx_stk_idx);
	    } else {
		printf("            TX Key: %s\n",
		       keybuf);
	    }
	    cpudb_key_format(sp_p->rx_cpu_key, keybuf, sizeof(keybuf));
	    if (sp_p->rx_stk_idx != 0) {
		printf("            RX Key: %s (stkport %d)\n",
		       keybuf, sp_p->rx_stk_idx);
	    } else {
		printf("            RX Key: %s\n",
		       keybuf);
	    }
	}
    }

    fflush(stdout);
}


/* end - code from Broadcom SDK5 file : $Id: tksdiag.c,v 1.2.2.35 2003/12/04 02:32:18 dtalayco */


/* start - code from Broadcom SDK5 file : $Id: tksdiag.c,v 1.2.2.35 2003/12/04 02:32:18 dtalayco */
/* lines 169 - 203 */
void
db_dump(cpudb_ref_t db_ref)
{
  int   i, count;
  cpudb_entry_t *entry;
  char  keybuf[CPUDB_KEY_STRING_LEN];

  if (cpudb_valid(db_ref))
  {
    count = cpudb_entry_count_get(db_ref);
    printf("  DB is valid and contains %d entries\n", count);
    if (db_ref->local_entry != NULL)
    {
      cpudb_key_format(db_ref->local_entry->base.key,
                       keybuf, sizeof(keybuf));
      printf("    Local Key: %s\n", keybuf);
    }
    if (db_ref->master_entry != NULL)
    {
      cpudb_key_format(db_ref->master_entry->base.key,
                       keybuf, sizeof(keybuf));
      printf("    Master Key: %s\n", keybuf);
    }
    i = 0;
    CPUDB_FOREACH_ENTRY(db_ref, entry) {
      i += 1;
      printf("    Entry %d/%d: ", i, count);
      db_entry_show(entry);
      printf("\n");
    }
    if (i != count)
    {
      printf("WARNING: entry count is %d but %d entries actually found\n",
             count, i);
    }
  }
  else
  {
    printf("DB is not valid\n");
  }

  fflush(stdout);
}

/* Patch for the XGS3 opcode 0 Issue - original code is in tksdiag.c */
/*
 * Function:
 *     mh_opcode0_priority_select
 * Description:
 *     Creates FP rule to redirect incoming HiGig port packets with
 *     MH Opcode 0 to the CPU port.  In addition, it clears the
 *     'copy-to-cpu' in the icontrol_opcode_bitmap registers for the HiGig.
 *
 * Parameters:
 *     unit           - BCM device number
 *     group_priority - Group priority within allowable range
 *                      (see bcm_field_status_get),
 *                      or BCM_FIELD_GROUP_PRIO_ANY to automatically assign a
 *                      priority; each priority value may be used only once
 *     group          - (OUT) New FP Group ID
 *     entry          - (OUT) New FP Entry ID
 * Returns:
 *     BCM_E_NONE     Success
 *     BCM_E_XXX      Failure,
 *                    - NULL pointers
 *                    - FP subsystem is not initialized
 *                    - No more FP resources available
 * Notes:
 *     FP subsystem must be initialized.
 *     This routine should only be executed on (local)
 *         5650x A0, A1
 *         5630x A0, B0
 *         5610x A0
 */

#include <soc/drv.h>
#include <bcm/field.h>
#include <bcm_int/control.h>
#ifdef BCM_FIELD_SUPPORT
int
mh_opcode0_priority_select(int unit,
                           int group_priority,
                           bcm_field_group_t *group,
                           bcm_field_entry_t *entry,
                           int               *obm_reg_changed)
{
    int                 valid_rev = FALSE;
    bcm_port_t          port;
    uint32              obm_reg;
    bcm_field_qset_t    qset;
    uint8               opcode;
    uint8               opcode_mask;
    uint32              action_param0;
    bcm_pbmp_t          in_ports;
    bcm_pbmp_t          in_ports_mask;
    uint16              dev_id;
    uint8               rev_id;


    /* Check device type and revision */
    if ((!BCM_UNIT_VALID(unit)) || (!BCM_IS_LOCAL(unit))) {
        return BCM_E_NONE;
    }

    /* Need to check the Revision ID of the device, not the driver */
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (SOC_IS_FIREBOLT(unit) &&
        ((rev_id == BCM56504_A0_REV_ID) ||
         (rev_id == BCM56504_A1_REV_ID))) {
        valid_rev = TRUE;
    } else if (SOC_IS_HELIX(unit) &&
               ((rev_id == BCM56304_A0_REV_ID) ||
                (rev_id == BCM56304_B0_REV_ID))) {
        valid_rev = TRUE;
    } else if (SOC_IS_FELIX(unit) &&
               (rev_id == BCM56107_A0_REV_ID)) {
        valid_rev = TRUE;
    }
    if (!valid_rev) {  /* No error; not for this device */
        return BCM_E_NONE;
    }


    /* Valid devide type, revision */

    if ((group == NULL) || (entry == NULL)) {
        return BCM_E_PARAM;
    }

    /* Set the HG ingress CPU Opcode map to NOT forward to CPU */
    PBMP_ST_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(READ_ICONTROL_OPCODE_BITMAPr(unit, port,
                                                         &obm_reg));
        soc_reg_field_set(unit, ICONTROL_OPCODE_BITMAPr, &obm_reg, BITMAPf,
                          0x0);
        SOC_IF_ERROR_RETURN(WRITE_ICONTROL_OPCODE_BITMAPr(unit, port,
                                                          obm_reg));
        *obm_reg_changed = TRUE;
    }


    /* Select qualifiers: MHOpcode, Ingress Ports */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMHOpcode);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPorts);

    /* Create group */
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset,
                                               group_priority, group));
    /* Create entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, *group, entry));

    /* Set qualifiers data, mask */
    /* MHOpcode */
    opcode = BCM_FIELD_MHOPCODE_CONTROL;
    opcode_mask = 0x7;   /* Match all bits */
    BCM_IF_ERROR_RETURN(bcm_field_qualify_MHOpcode(unit, *entry,
                                                   opcode, opcode_mask));
    /* Ingress Ports */
    BCM_PBMP_ASSIGN(in_ports, PBMP_ST_ALL(unit));
    BCM_PBMP_ASSIGN(in_ports_mask, PBMP_PORT_ALL(unit));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPorts(unit, *entry,
                                                  in_ports, in_ports_mask));

    /* Set action to redirect to CPU port */
    action_param0 = SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0);
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, *entry,
                                             bcmFieldActionRedirectPbmp,
                                             action_param0, 0));

    return bcm_field_entry_install(unit, *entry);
}


/*
 * Function:
 *     mh_opcode0_priority_clear
 * Description:
 *     It destroys the FP rule for the given group ID and entry ID.
 *     In addition, it sets the 'copy-to-cpu' flag in the icontrol_opcode_bitmap
 *      registers for the HiGig.
 *
 * Parameters:
 *     unit           - BCM device number
 *     group          - FP Group ID
 *     entry          - FP Entry ID
 * Returns:
 *     BCM_E_NONE     Success
 *     BCM_E_XXX      Failure
 * Notes:
 *     This routine is used in conjuction with mh_opcode0_priority_select.
 *     This should only be called on those units where the
 *     mh_opcode0_priority_select was successfully called.  The FP group
 *     and entry IDs should have been saved earlier and passed in
 *     to this routine.
 *
 *
 *     Given the above, the device type and rev will NOT be validated.
 */

int
mh_opcode0_priority_clear(int unit,
                          bcm_field_group_t group,
                          bcm_field_entry_t entry,
                          int obm_reg_changed)
{
    bcm_port_t          port;
    uint32              obm_reg;

    /* Destroy entry */
    if (entry != -1) {
        bcm_field_entry_remove(unit, entry);
        bcm_field_entry_destroy(unit, entry);
    }

    /* Destroy group */
    if (group != -1) {
        bcm_field_group_destroy(unit, group);
    }

    /* Set HG ingress CPU Opcode map to COPY to the CPU */
    if (obm_reg_changed) {
        PBMP_ST_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(READ_ICONTROL_OPCODE_BITMAPr(unit, port,
                                                             &obm_reg));
            soc_reg_field_set(unit, ICONTROL_OPCODE_BITMAPr, &obm_reg, BITMAPf,
                              0x10);
            SOC_IF_ERROR_RETURN(WRITE_ICONTROL_OPCODE_BITMAPr(unit, port,
                                                              obm_reg));
        }
    }

    return BCM_E_NONE;
}
#endif

/* end - code from Broadcom SDK5 file : $Id: tksdiag.c,v 1.2.2.35 2003/12/04 02:32:18 dtalayco */
#endif
