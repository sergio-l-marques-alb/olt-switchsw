/** \file oam_classifier_key_select.c
 * $Id$
 *
 * OAM key selection tables (IPPB_OAM_KEY_SEL_MEM, ETPPC_OAM_KEY_TABLE)
 * contorls the keys for OAM-LIF-DB tables (3 accesses for one packet).
 *
 * This file contains the init functions required to fill these tables
 * with the predefined logic to support simple classification and advanced
 * classification for Hierarchical Loss Measurement.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * OAM_ING_KEY_BASE_SELECT definitions for ease of use
 */
    /** NULL KEY */
#define ING_OAM_LIF_NULL  DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_NULL
    /** MOST INNER LIF */
#define ING_OAM_LIF       DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF
    /** THIRD LIF RECOGNIZED WITH LIF OAM-LIF BIT SET */
#define ING_OAM_LIF_3     DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_3
    /** SECOND LIF RECOGNIZED WITH LIF OAM-LIF BIT SET */
#define ING_OAM_LIF_2     DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_2
    /** FIRST LIF (MOST INNER) RECOGNIZED WITH LIF OAM-LIF BIT */
#define ING_OAM_LIF_1     DBAL_ENUM_FVAL_OAM_ING_KEY_BASE_SELECT_OAM_LIF_1

/*
 * OAM_EG_KEY_BASE_SELECT definitions for ease of use
 */
    /** NULL KEY */
#define EG_OAM_LIF_NULL  DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_NULL
    /** MOST INNER LIF */
#define EG_OAM_LIF       DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF
    /** THIRD LIF RECOGNIZED WITH LIF OAM-LIF BIT SET */
#define EG_OAM_LIF_3     DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_3
    /** SECOND LIF RECOGNIZED WITH LIF OAM-LIF BIT SET */
#define EG_OAM_LIF_2     DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_2
    /** FIRST LIF (MOST INNER) RECOGNIZED WITH LIF OAM-LIF BIT */
#define EG_OAM_LIF_1     DBAL_ENUM_FVAL_OAM_EG_KEY_BASE_SELECT_OAM_LIF_1

/*
 * OAM_KEY_PREFIX definitions for ease of use
 */
    /** SOLE / HIGHEST ENTRY */
#define PREFIX_HIGH   DBAL_ENUM_FVAL_OAM_KEY_PREFIX_HIGH
    /** MIDDLE ENTRY */
#define PREFIX_MID    DBAL_ENUM_FVAL_OAM_KEY_PREFIX_MID
    /** LOW ENTRY */
#define PREFIX_LOW    DBAL_ENUM_FVAL_OAM_KEY_PREFIX_LOW
    /** BFD ENTRY */
#define PREFIX_BFD    DBAL_ENUM_FVAL_OAM_KEY_PREFIX_BFD

/*
 * Entry representation for IPPB_OAM_KEY_SEL_MEM
 */
typedef struct oam_key_select_ingress_table_entry_s
{
    /** Key */
    /** Number of recognized LIFs with OAM bit set */
    int nof_valid_lm_lifs;
    /** Is most recognized inner LIF valid for LM */
    int in_lif_valid_for_lm;
    /** Is classification done by Your-Discriminator */
    int your_disc_valid;

    /** Value */
    /** Selected key for OAM-LIF-DB access 1-3 */
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_1;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_2;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_3;
    /** Selected prefix for OAM-LIF-DB access 1-3 */
    int oam_key_prefix_1;
    int oam_key_prefix_2;
    int oam_key_prefix_3;
    int mp_profile_sel;
} oam_key_select_ingress_table_entry_t;

/*
 * Entry representation for ETPPC_OAM_KEY_TABLE
 */
typedef struct oam_key_select_egress_table_entry_s
{
    /** Key */
    /** Number of recognized LIFs with OAM bit set */
    int nof_valid_lm_lifs;
    /** Is packet identified as an OAM packet */
    int packet_is_oam;
    /** Is packet injected */
    int oam_inject;
    /** Is counter pointer valid */
    int counter_pointer_valid;

    /** Value */
    /** Selected key for OAM-LIF-DB access 1-3 */
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_1;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_2;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_3;
    /** Selected prefix for OAM-LIF-DB access 1-3 */
    int oam_key_prefix_1;
    int oam_key_prefix_2;
    int oam_key_prefix_3;
    int mp_profile_sel;
} oam_key_select_egress_table_entry_t;

/*
 * Entry representation for IPPB_OAM_KEY_SEL_MEM
 */
typedef struct oam_key_select_wo_hlm_ingress_table_entry_s
{
    /** Key */
    /** Number of recognized LIFs with OAM bit set (1bit)*/
    int nof_valid_lm_lifs;
    /** Is most recognized inner LIF valid for LM */
    int in_lif_valid_for_lm;
    /** Is classification done by Your-Discriminator */
    int your_disc_valid;

    /** Value */
    /** Selected key for OAM-LIF-DB */
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_1;
    /** Selected prefix for OAM-LIF-DB */
    int oam_key_prefix_1;
    int mp_profile_sel;
} oam_key_select_wo_hlm_ingress_table_entry_t;

/*
 * Entry representation for ETPPC_OAM_KEY_TABLE
 */
typedef struct oam_key_select_wo_hlm_egress_table_entry_s
{
    /** Key */
    /** Number of recognized LIFs with OAM bit set */
    int nof_valid_lm_lifs;
    /** Is packet identified as an OAM packet */
    int packet_is_oam;
    /** Is packet injected */
    int oam_inject;
    /** Is counter pointer valid */
    int counter_pointer_valid;

    /** Value */
    /** Selected key for OAM-LIF-DB */
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_1;
    /** Selected prefix for OAM-LIF-DB */
    int oam_key_prefix_1;
    int mp_profile_sel;
} oam_key_select_wo_hlm_egress_table_entry_t;

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

 /*
  * Global and Static
  * {
  */

/* *INDENT-OFF* */
#ifdef BCM_DNX2_SUPPORT
/* Ingress key select table data */
static const oam_key_select_ingress_table_entry_t oam_key_select_ingress_map[] = {
    /* 
     * No valid LM LIFs at all
     */
    /** standard classification based on LIF */
    {0, 0, 0, ING_OAM_LIF, ING_OAM_LIF_NULL, ING_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    /** standard classification based on Your-Discriminator */
    {0, 0, 1, ING_OAM_LIF, ING_OAM_LIF_NULL, ING_OAM_LIF_NULL, PREFIX_BFD, PREFIX_HIGH, PREFIX_HIGH, 1},
    /*
     * One valid LM LIF - HLM by LIF
     */
    /** standard classification based on most inner LIF and
     *  two same outer LIF counters */
    {1, 0, 0, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_1, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    /*
     * One valid LM LIF - HLM by MDL
     */
    /** standard classification based on Your-Discriminator and
     *  two same outer LIF counters */
    {1, 0, 1, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_1, PREFIX_BFD, PREFIX_HIGH, PREFIX_MID, 1},
    /** standard classification based on most inner LIF and
     *  extra two same LIF counters */
    {1, 1, 0, ING_OAM_LIF, ING_OAM_LIF, ING_OAM_LIF, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    /* 
     * Two valid LM LIF - HLM by LIF
     */
    /** standard classification based on most inner LIF and
     *  two different outer LIFs counters */
    {2, 0, 0, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    /** standard classification based on Your-Discriminator and
     *  two different outer LIFs counters */
    {2, 0, 1, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_1, PREFIX_BFD, PREFIX_HIGH, PREFIX_HIGH, 1},
    /** standard classification based on most inner LIF and
     *  extra two different outer LIFs counters */
    {2, 1, 0, ING_OAM_LIF, ING_OAM_LIF_2, ING_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    /* 
     * Three valid LM LIF - HLM by LIF
     */
    /** No classification on most inner LIF, Behaviour based on
     *  default profile. Three different outer LIFs counters */
    {3, 0, 0, ING_OAM_LIF_1, ING_OAM_LIF_2, ING_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 0},
    /** standard classification based on Your-Discriminator and
     *  two different outer LIFs counters */
    {3, 0, 1, ING_OAM_LIF, ING_OAM_LIF_1, ING_OAM_LIF_2, PREFIX_BFD, PREFIX_HIGH, PREFIX_HIGH, 1},
    /** standard classification based on most inner LIF and
     *  extra two different outer LIFs counters */
    {3, 1, 0, ING_OAM_LIF, ING_OAM_LIF_2, ING_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
};
/* Egress key select table data */
static const oam_key_select_egress_table_entry_t oam_key_select_egress_map[] = {
    /* 
     * No valid LM LIFs at all
     */
    /** standard classification based on LIF */
    {0, 0, 0, 0, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 0, 0, 1, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 0, 1, 0, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 0, 1, 1, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 1, 0, 0, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 1, 0, 1, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 1, 1, 0, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {0, 1, 1, 1, EG_OAM_LIF, EG_OAM_LIF_NULL, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    /*
     * One valid LM LIF - HLM by LIF
     */
    {1, 0, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    /*
     * One valid LM LIF - HLM by MDL
     */
    /** standard classification based on most inner LIF and
     *  two same/different outer LIF counters */
    {1, 0, 0, 0, EG_OAM_LIF_1, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 0, 1, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 0, 1, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 1, 0, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 1, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 1, 1, 0, EG_OAM_LIF_1, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    {1, 1, 1, 1, EG_OAM_LIF_1, EG_OAM_LIF_1, EG_OAM_LIF_1, PREFIX_HIGH, PREFIX_MID, PREFIX_LOW, 1},
    /* 
     * Two valid LM LIF - HLM by LIF
     */
    /** standard classification based on most inner LIF and
     *  two same/different outer LIF counters */
    {2, 0, 0, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 0, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 0, 1, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 0, 1, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 1, 0, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 1, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 1, 1, 0, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {2, 1, 1, 1, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_NULL, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    /* 
     * Three valid LM LIF - HLM by LIF
     */
    /** No classification on most inner LIF, Behaviour based on
     *  default profile. Three different outer LIFs counters */
    {3, 0, 0, 0, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 0, 0, 1, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    /** standard classification based on most inner LIF and
     *  two same/different outer LIF counters */
    {3, 0, 1, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 0, 1, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 1, 0, 0, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 1, 0, 1, EG_OAM_LIF, EG_OAM_LIF_1, EG_OAM_LIF_2, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 1, 1, 0, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
    {3, 1, 1, 1, EG_OAM_LIF_1, EG_OAM_LIF_2, EG_OAM_LIF_3, PREFIX_HIGH, PREFIX_HIGH, PREFIX_HIGH, 1},
};
#endif

/* Ingress key select table data */
/* *INDENT-ON* */

/*
 * }
 */

/*
 * See h file for description
 */
shr_error_e
dnx_oam_key_select_ingress_init(
    int unit)
{
    uint32 entry_handle_id;
    int entry = 0;
    int nof_ingress_key_select_entries = 0;

    /*
     * IPPB_OAM_KEY_SEL_MEM table key fields
     * Used to iterate all combinations and set entry values accordingly
     */
    int nof_valid_lm_lifs = 0;
    int in_lif_valid_for_lm = 0;
    int your_disc_valid = 0;
    /*
     * IPPB_OAM_KEY_SEL_MEM table entry data fields
     * Used to set one table entry
     */
    int oam_key_prefix_1 = 0;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_1 = ING_OAM_LIF_NULL;
#ifdef BCM_DNX2_SUPPORT
    int oam_key_prefix_2 = 0;
    int oam_key_prefix_3 = 0;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_2 = ING_OAM_LIF_NULL;
    dbal_enum_value_field_oam_ing_key_base_select_e oam_key_base_select_3 = ING_OAM_LIF_NULL;
    uint8 oam_hlm_support = 0;
#endif
    int mp_profile_sel = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX2_SUPPORT
    oam_hlm_support = dnx_data_oam.general.feature_get(unit, dnx_data_oam_general_oam_hlm_support);
    if (oam_hlm_support == 1)
    {
        nof_ingress_key_select_entries =
            sizeof(oam_key_select_ingress_map) / sizeof(oam_key_select_ingress_table_entry_t);
    }
    else
#endif
    {
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_OAM_KEY_SELECT, &entry_handle_id));

    /** Iterate all predefined values for ingress key select
     *  table and commit to IPPB_OAM_KEY_SEL_MEM */
    for (entry = 0; entry < nof_ingress_key_select_entries; entry++)
    {
        /** Key */
#ifdef BCM_DNX2_SUPPORT
        if (oam_hlm_support == 1)
        {
            nof_valid_lm_lifs = oam_key_select_ingress_map[entry].nof_valid_lm_lifs;
            in_lif_valid_for_lm = oam_key_select_ingress_map[entry].in_lif_valid_for_lm;
            your_disc_valid = oam_key_select_ingress_map[entry].your_disc_valid;
            /** Value */
            oam_key_base_select_1 = oam_key_select_ingress_map[entry].oam_key_base_select_1;
            oam_key_prefix_1 = oam_key_select_ingress_map[entry].oam_key_prefix_1;
            oam_key_base_select_2 = oam_key_select_ingress_map[entry].oam_key_base_select_2;
            oam_key_base_select_3 = oam_key_select_ingress_map[entry].oam_key_base_select_3;
            oam_key_prefix_2 = oam_key_select_ingress_map[entry].oam_key_prefix_2;
            oam_key_prefix_3 = oam_key_select_ingress_map[entry].oam_key_prefix_3;
            mp_profile_sel = oam_key_select_ingress_map[entry].mp_profile_sel;
        }
        else
#endif
        {
        }

        /** Add the entry */

        /** setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_VALID, your_disc_valid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_VALID_FOR_LM, in_lif_valid_for_lm);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);

        /** setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_1, INST_SINGLE,
                                     oam_key_base_select_1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_1, INST_SINGLE, oam_key_prefix_1);
#ifdef BCM_DNX2_SUPPORT
        if (oam_hlm_support == 1)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_2, INST_SINGLE,
                                         oam_key_base_select_2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_3, INST_SINGLE,
                                         oam_key_base_select_3);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_2, INST_SINGLE,
                                         oam_key_prefix_2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_3, INST_SINGLE,
                                         oam_key_prefix_3);
        }
#endif
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE_SEL, INST_SINGLE, mp_profile_sel);

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_OAM_KEY_SELECT, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_key_select_egress_init(
    int unit)
{
    uint32 entry_handle_id;
    int entry = 0;
    int nof_egress_key_select_entries = 0;

    /*
     * ETPPC_OAM_KEY_TABLE table key fields
     * Used to iterate all combinations and set entry values accordingly
     */
    int packet_is_oam = 0;
    int counter_pointer_valid = 0;
    int nof_valid_lm_lifs = 0;
    int oam_inject = 0;
    /*
     * ETPPC_OAM_KEY_TABLE table entry data fields
     * Used to set one table entry
     */
    int mp_profile_sel = 0;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_1 = EG_OAM_LIF_NULL;
#ifdef BCM_DNX2_SUPPORT
    int oam_key_prefix_1 = 0;
    int oam_key_prefix_2 = 0;
    int oam_key_prefix_3 = 0;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_2 = EG_OAM_LIF_NULL;
    dbal_enum_value_field_oam_eg_key_base_select_e oam_key_base_select_3 = EG_OAM_LIF_NULL;
    uint8 oam_hlm_support = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX2_SUPPORT
    oam_hlm_support = dnx_data_oam.general.feature_get(unit, dnx_data_oam_general_oam_hlm_support);
    if (oam_hlm_support == 1)
    {
        nof_egress_key_select_entries = sizeof(oam_key_select_egress_map) / sizeof(oam_key_select_egress_table_entry_t);
    }
    else
#endif
    {
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_KEY_SELECT, &entry_handle_id));

    /** Iterate all predefined values for egress key select
     *  table and commit to ETPPC_OAM_KEY_TABLE */
    for (entry = 0; entry < nof_egress_key_select_entries; entry++)
    {
        /** Key */
#ifdef BCM_DNX2_SUPPORT
        if (oam_hlm_support == 1)
        {
            oam_inject = oam_key_select_egress_map[entry].oam_inject;
            nof_valid_lm_lifs = oam_key_select_egress_map[entry].nof_valid_lm_lifs;
            counter_pointer_valid = oam_key_select_egress_map[entry].counter_pointer_valid;
            packet_is_oam = oam_key_select_egress_map[entry].packet_is_oam;
            /** Value */
            oam_key_base_select_1 = oam_key_select_egress_map[entry].oam_key_base_select_1;
            oam_key_base_select_2 = oam_key_select_egress_map[entry].oam_key_base_select_2;
            oam_key_base_select_3 = oam_key_select_egress_map[entry].oam_key_base_select_3;
            oam_key_prefix_1 = oam_key_select_egress_map[entry].oam_key_prefix_1;
            oam_key_prefix_2 = oam_key_select_egress_map[entry].oam_key_prefix_2;
            oam_key_prefix_3 = oam_key_select_egress_map[entry].oam_key_prefix_3;
            mp_profile_sel = oam_key_select_egress_map[entry].mp_profile_sel;
        }
        else
#endif
        {
        }

        /** Add the entry */

        /** setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_OAM, packet_is_oam);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_PTR_VALID, counter_pointer_valid);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INJECT, oam_inject);

        /** setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_1, INST_SINGLE,
                                     oam_key_base_select_1);
#ifdef BCM_DNX2_SUPPORT
        if (oam_hlm_support == 1)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_2, INST_SINGLE,
                                         oam_key_base_select_2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_3, INST_SINGLE,
                                         oam_key_base_select_3);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_1, INST_SINGLE,
                                         oam_key_prefix_1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_2, INST_SINGLE,
                                         oam_key_prefix_2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_3, INST_SINGLE,
                                         oam_key_prefix_3);
        }
#endif
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE_SEL, INST_SINGLE, mp_profile_sel);

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_OAM_KEY_SELECT, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_key_select_ingress_get(
    int unit,
    int nof_valid_lm_lifs,
    int in_lif_valid_for_lm,
    int your_disc_valid,
    uint32 *key_pre_1,
    uint32 *key_pre_2,
    uint32 *key_pre_3,
    uint32 *key_base_1,
    uint32 *key_base_2,
    uint32 *key_base_3)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_OAM_KEY_SELECT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_VALID_FOR_LM, in_lif_valid_for_lm);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_YOUR_DISC_VALID, your_disc_valid);

   /** setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_1, INST_SINGLE, key_pre_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_2, INST_SINGLE, key_pre_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_3, INST_SINGLE, key_pre_3);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_1, INST_SINGLE, key_base_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_2, INST_SINGLE, key_base_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ING_KEY_BASE_SELECT_3, INST_SINGLE, key_base_3);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_key_select_egress_get(
    int unit,
    int nof_valid_lm_lifs,
    int counter_ptr_valid,
    int oam_inject,
    int packet_is_oam,
    uint32 *key_pre_1,
    uint32 *key_pre_2,
    uint32 *key_pre_3,
    uint32 *key_base_1,
    uint32 *key_base_2,
    uint32 *key_base_3)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_KEY_SELECT, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_VALID_LM_LIFS, nof_valid_lm_lifs);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INJECT, oam_inject);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_PTR_VALID, counter_ptr_valid);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_OAM, packet_is_oam);

   /** setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_1, INST_SINGLE, key_pre_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_2, INST_SINGLE, key_pre_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX_3, INST_SINGLE, key_pre_3);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_1, INST_SINGLE, key_base_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_2, INST_SINGLE, key_base_2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_EG_KEY_BASE_SELECT_3, INST_SINGLE, key_base_3);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
