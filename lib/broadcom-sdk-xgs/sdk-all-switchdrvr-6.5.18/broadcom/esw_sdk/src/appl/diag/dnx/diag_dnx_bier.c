/**
 * \file diag_dnx_bier.c
 *
 * bier management shell commands.
 */

#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDES
 * {
 */
/*
 * shared
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
/*
 * appl
 */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/*
 * soc
 */
/*
 * bcm
 */
#include <bcm/multicast.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/bier/bier.h>
/*
 * }
 */

extern int bcm_dnx_multicast_bfr_id_add(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 bfr_id,
    bcm_multicast_replication_t replication);
extern int bcm_dnx_multicast_bfr_id_delete(
    int unit,
    uint32 flags,
    bcm_multicast_t group,
    uint32 bfr_id,
    bcm_multicast_replication_t replication);

static shr_error_e
bier_add_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_multicast_t group;
    bcm_multicast_replication_t replication;
    uint32 is_ingress;
    uint32 bfr_id;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&replication, 0x0, sizeof(bcm_multicast_replication_t));
    SH_SAND_GET_INT32("MC_Group", group);
    SH_SAND_GET_UINT32("BFR_Id", bfr_id);
    SH_SAND_GET_UINT32("INGress", is_ingress);
    SH_SAND_GET_INT32("Port", replication.port);
    SH_SAND_GET_INT32("ENCap", replication.encap1);
    flags = is_ingress ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP;
    SHR_IF_ERR_EXIT(bcm_dnx_multicast_bfr_id_add(unit, flags, group, bfr_id, replication));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
bier_delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_multicast_t group;
    bcm_multicast_replication_t replication;
    uint32 is_ingress;
    uint32 bfr_id;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&replication, 0x0, sizeof(bcm_multicast_replication_t));
    SH_SAND_GET_INT32("MC_Group", group);
    SH_SAND_GET_UINT32("BFR_Id", bfr_id);
    SH_SAND_GET_UINT32("INGress", is_ingress);
    SH_SAND_GET_INT32("Port", replication.port);
    SH_SAND_GET_INT32("ENCap", replication.encap1);
    flags = is_ingress ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP;
    SHR_IF_ERR_EXIT(bcm_dnx_multicast_bfr_id_delete(unit, flags, group, bfr_id, replication));

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

/**
 * \brief - Man page for 'bier mode' shell command
 */
static sh_sand_man_t bier_add_man = {
    .brief = "delete bfr id",
};

/**
 * \brief - Man page for 'bier mode' shell command
 */
static sh_sand_man_t bier_delete_man = {
    .brief = "delete bfr id",
};
/**
 * \brief - Options list for 'bier mode' shell command
 */
static sh_sand_option_t bier_add_delete_args[] = {
   /* Name      | Type                | Description          | Default     */
    {"MC_Group",      SAL_FIELD_TYPE_INT32, "Multicast Group ID", NULL},
    {"INGress",      SAL_FIELD_TYPE_UINT32, "Is Ingress", NULL},
    {"BFR_Id",      SAL_FIELD_TYPE_UINT32, "BFR id", NULL},
    {"Port",      SAL_FIELD_TYPE_INT32, "Port to add", NULL},
    {"ENCap",      SAL_FIELD_TYPE_INT32, "Encaps id to add", NULL},
    {NULL}      /* End of options list - must be last. */
};

sh_sand_man_t sh_dnx_bier_man = {
    .brief = "bier Control commands",
};

sh_sand_cmd_t sh_dnx_bier_cmds[] = {
  /* Name       | Leaf Action         | Node | Options for Leaf     | Usage                */
    {"add",     bier_add_cmd,         NULL,  bier_add_delete_args,    &bier_add_man,    NULL, NULL, SH_CMD_SKIP_EXEC},
    {"delete",  bier_delete_cmd,      NULL,  bier_add_delete_args,    &bier_delete_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {NULL}
};
/* *INDENT-ON* */
