/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $Id$
 * chenr 08/08/2018
 *
 * kbp flexible mode application reference
 * flexible mode allows the user to access KBP driver directly instead of using DNX APIs
 * enable feature by SOC - custom_feature_ext_flexible_mode=1
 *
 * flexible_mode_ref_app() - main function, includes getting all kbp info from SDK, add customer config and set kbp APIs direcly.
 * flexible_mode_acl_add() - add acl fields to master key.
 * bcm_switch_kbp_info_get() -get all kbp device pointer from SDK.
 * bcm_switch_kbp_apptype_info_get() - get apptype info per apptype
 * flexible_mode_kbp_configuration() -
 *  (a) build master key from Fwd fields and ACL fields.
 *  (b) add new DBs to kbp.
 *  (c) add Fwd/RPF (from SDK) DBs to kbp.
 *  (d) install instruction.
 * kbp_device_lock() - perfrom device lock.
 * add_em_kbp_entry() - add exact match entry to kbp.
 * add_acl_kbp_entry() - add acl entry to kbp.
 */

#if defined(INCLUDE_KBP)

/*************
 * INCLUDES  *
 *************/

#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <include/bcm/field.h>
#include <include/appl/diag/dpp/kbp.h>
#include <shared/swstate/access/sw_state_access.h>

extern genericTblMgrAradAppData *AradAppData[SOC_SAND_MAX_DEVICE];

/*************
* FUNCTIONS *
*************/

/* add acls to master key */
int flexible_mode_acl_add(int unit)
{
    bcm_field_group_config_t group;
    int presel_id = 0;
    int presel_flags = 0;
    int res = 0;

    /* create preselector id */
    res = bcm_field_presel_create(unit, &presel_id);
    if(res)
    {
        printf("Error, bcm_field_presel_create failed\n");
        return  res;
    }

    /* set preselectors value */
    bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageExternal);
    bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldAppTypeIp4Ucast);

    bcm_field_group_config_t_init(&group);

    /* add preselectors to preselector set for first field-group */
    BCM_FIELD_PRESEL_ADD(group.preselset, presel_id);

    group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |BCM_FIELD_GROUP_CREATE_WITH_ID;
    group.mode = bcmFieldGroupModeAuto;
    group.group = 80;
    group.priority = 40;

    /* add qualifiers for first field-group */
    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageExternal);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyIntPriority);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyL4SrcPort);

    /* add actions for first field-group */
    BCM_FIELD_ASET_INIT(group.aset);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionExternalValue0Set);

    /* create first field-group */
    res = bcm_field_group_config_create(unit, &group);
    if(res)
    {
        printf("Error, bcm_field_group_config_create failed\n");
        return  res;
    }

    bcm_field_group_config_t_init(&group);

    /* add preselectors to preselector set for second field-group */
    BCM_FIELD_PRESEL_ADD(group.preselset, presel_id);

    group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |BCM_FIELD_GROUP_CREATE_WITH_ID;
    group.mode = bcmFieldGroupModeAuto;
    group.group = 82;
    group.priority = 42;

    /* add qualifiers for second field-group */
    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageExternal);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyColor);

    /* add actions for second field-group */
    BCM_FIELD_ASET_INIT(group.aset);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionExternalValue1Set);

    /* create second field-group */
    res = bcm_field_group_config_create(unit, &group);
    if(res)
    {
        printf("Error, bcm_field_group_config_create failed\n");
        return  res;
    }

    return res;
}

/* get kbp pointer from SDK */
int bcm_petra_switch_kbp_info_get(int unit, bcm_core_t core, bcm_switch_kbp_info_t *kbp_info)
{
    int res = 0;

    /* check custom_feature_ext_flexible_mode soc is enabled */
    if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
    {
         printf("Error, custom_feature_ext_flexible_mode must be set\n");
         res = 1; /* indicate error */
         return res;
    }

    /* get kbp device pointer */
    kbp_info->device_p = (int*)(AradAppData[unit]->device_p[core]);

    return res;
}

/* get apptype info per apptype */
int bcm_petra_switch_kbp_apptype_info_get(int unit, bcm_core_t core, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t *apptype_info)
{
    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};
    ARAD_PMF_CE pgm_ce;
    SOC_PPC_FP_DATABASE_STAGE stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP;
    ARAD_PMF_DB_INFO pmf_db_info;
    bcm_switch_kbp_master_key_segment_info_t acl_segments[BCM_MAX_NOF_SEGMENTS];
    uint32 ltr_index = 0;
    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);
    uint32 pgm_idx;
    uint32 cycle_idx;
    uint32 lsb_msb;
    uint32 ce_id;
    uint32 ce_idx;
    bcm_field_AppType_t apptype_check = 0;
    int apptype_validitiy_error_check = 0;
    int srch_ndx = 0;
    int res = 0;

    /* initiailze valid, dbs and number of segments */
    for(srch_ndx = 0; srch_ndx < bcmFieldAppTypeCount; srch_ndx++)
    {
        apptype_info[srch_ndx].is_valid = 0;
        apptype_info[srch_ndx].fwd_db = NULL;
        apptype_info[srch_ndx].dummy_db = NULL;
        apptype_info[srch_ndx].rpf_db = NULL;
        apptype_info[srch_ndx].master_key_nof_segments = 0;
    }

    /* run over all KBP ltrs */
    for(ltr_index = 0; ltr_index < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; ltr_index++)
    {
        /* get ltr config info */
        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, ltr_index, &ltr_config_info);
        if(res)
        {
            printf("Error, could not get device ltr info\n");
            return  res;
        }

        /* if ltr is valid, update kbp_info structure */
        if (ltr_config_info.valid)
        {
            /* get apptype */
            res = _bcm_dpp_field_app_type_ppd_to_bcm(unit, ltr_config_info.opcode, &apptype_check);
            if(res)
            {
                printf("Error, _bcm_dpp_field_app_type_ppd_to_bcm failed\n");
                return  res;
            }

            /* check apptype sent to bcm_petra_switch_kbp_apptype_info_get() is corresponding to apptype_check retrieved from ltr_config_info.opcode */
            if (apptype != apptype_check) 
            {
                continue;
            }

            /* update ltr valid */
            apptype_info[apptype].is_valid = ltr_config_info.valid;

            /* update ltr index */
            apptype_info[apptype].ltr_index = ltr_index;

            /* update opcode */
            apptype_info[apptype].opcode = ltr_config_info.opcode;

            /* update instruction pointer */
            apptype_info[apptype].inst_p = (int*)ltr_config_info.inst_p;

            /* update master key number of segments */
            apptype_info[apptype].master_key_nof_segments = ltr_config_info.master_key_fields.nof_key_segments;

            /* update master key segments attributes */
            for(srch_ndx = 0; srch_ndx < apptype_info[apptype].master_key_nof_segments; srch_ndx++)
            {
                /* update segemnt name */
                sal_strncpy(((char*)apptype_info[apptype].master_key_segment[srch_ndx].segment_name), ltr_config_info.master_key_fields.key_segment[srch_ndx].name,
                           sizeof(apptype_info[apptype].master_key_segment[srch_ndx].segment_name));

                /* update segemnt size in bytes */
                apptype_info[apptype].master_key_segment[srch_ndx].segment_size_bits = (ltr_config_info.master_key_fields.key_segment[srch_ndx].nof_bytes)*8;

                /* update segemnt type */
                apptype_info[apptype].master_key_segment[srch_ndx].segment_type = ltr_config_info.master_key_fields.key_segment[srch_ndx].type;
            }

            /* update fwd db, rpf db and dummy db for each apptype */
            for(srch_ndx = 0; srch_ndx < num_of_kbp_searches; srch_ndx++)
            {
                if (SHR_BITGET(&ltr_config_info.parallel_srches_bmp, srch_ndx))
                {
                    /* fw db */
                    if (srch_ndx == 0)
                    {
                        /* update fwd db */
                        apptype_info[apptype].fwd_db = (int*)AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.db_p;

                        /* update ad fwd db */
                        apptype_info[apptype].fwd_ad_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.ad_db_p);
                    }
                    /* dummy db */
                    else if (srch_ndx == 1)
                    {
                        /* update dummy db */
                        apptype_info[apptype].dummy_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.db_p);

                        /* update ad dummy db */
                        apptype_info[apptype].dummy_ad_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.ad_db_p);
                    }
                    /* rpf db */
                    else if (srch_ndx == 2)
                    {
                        /* update rpf db */
                        apptype_info[apptype].rpf_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.db_p);

                        /* update ad rpf db */
                        apptype_info[apptype].rpf_ad_db = (int*)(AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.ad_db_p);
                    }
                }
            }
        }
    }

    /* get acl qualifers info - qualifier type, qualifier size and qualifier name */
    /* run over all flp programs */
    for(pgm_idx = 0; pgm_idx < ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX; pgm_idx++)
    {
        /* initialize srch_ndx */
        srch_ndx = -1;

        /* run over number of cycles */
        for(cycle_idx = 0; cycle_idx < ARAD_PMF_LOW_LEVEL_NOF_CYCLES; cycle_idx++)
        {
            ce_id = 0;

            /* check apptype sent to bcm_petra_switch_kbp_apptype_info_get() is corresponding to apptype_check retrieved from pgm_idx */
            if (apptype_validitiy_error_check)
            {
                break;
            }

            for(lsb_msb = 0; lsb_msb < ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB; lsb_msb++)
            {
                /* check apptype sent to bcm_petra_switch_kbp_apptype_info_get() is corresponding to apptype_check retrieved from pgm_idx */
                if (apptype_validitiy_error_check)
                {
                    break;
                }

                for(ce_idx = 0; ce_idx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB; ce_idx++)
                {
                    ce_id = (SOC_PPC_FP_KEY_BIT_TYPE_LSB == lsb_msb) ?
                    (ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB - ce_idx - 1) :
                    (ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB * 2 - ce_idx - 1);

                    sal_memset(&pgm_ce, 0x0, sizeof(ARAD_PMF_CE));

                    /* get acl key qualifers info */
                    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(unit, stage, pgm_idx, cycle_idx, ce_id, &pgm_ce);

                    if (pgm_ce.is_used)
                    {
                        sal_memset(&pmf_db_info, 0x0, sizeof(ARAD_PMF_DB_INFO));
                        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.get(unit, stage, pgm_ce.db_id, &pmf_db_info);


                        if(SHR_BITGET(pmf_db_info.progs, pgm_idx) > 0)
                        {
                            /* update srch_ndx */
                            ++srch_ndx;

                            /* get apptype */
                            res = _bcm_dpp_field_app_type_ppd_to_bcm(unit, pgm_idx, &apptype_check);

                            /* check apptype sent to bcm_petra_switch_kbp_apptype_info_get() is corresponding to apptype_check retrieved from pgm_idx */
                            if (apptype != apptype_check)
                            {
                                apptype_validitiy_error_check = 1;
                                break;
                            }

                            /* get qualifer name */
                            sal_strncpy(((char*)acl_segments[srch_ndx].segment_name), SOC_PPC_FP_QUAL_TYPE_to_string(pgm_ce.qual_type),
                                       sizeof(acl_segments[srch_ndx].segment_name));

                            /* get qualifer number of bytes */
                            acl_segments[srch_ndx].segment_size_bits = pgm_ce.msb + pgm_ce.msb_padding + 1;

                            /* set qualifer type */
                            acl_segments[srch_ndx].segment_type = KBP_KEY_FIELD_TERNARY;

                            /* update number of segments */
                            ++apptype_info[apptype].master_key_nof_segments;
                        }
                    }
                }
            }
        }

        /* check apptype sent to bcm_petra_switch_kbp_apptype_info_get() is corresponding to apptype_check retrieved from pgm_idx */
        if (apptype_validitiy_error_check) 
        {
            /* reset apptype_validitiy_error_check for next pgm_idx */
            apptype_validitiy_error_check = 0;
            continue;
        }

        /* update kbp info with acl segments */
        for(; srch_ndx >= 0; --srch_ndx)
        {
            /* get qualifer name */
            sal_strncpy(((char*)apptype_info[apptype].master_key_segment[apptype_info[apptype].master_key_nof_segments-srch_ndx-1].segment_name), 
                        ((char*)acl_segments[srch_ndx].segment_name), 
                        sizeof(apptype_info[apptype].master_key_segment[apptype_info[apptype].master_key_nof_segments-srch_ndx-1].segment_name));

            /* get qualifer number of bytes */
            apptype_info[apptype].master_key_segment[apptype_info[apptype].master_key_nof_segments-srch_ndx-1].segment_size_bits = acl_segments[srch_ndx].segment_size_bits;

            /* set qualifer type */
            apptype_info[apptype].master_key_segment[apptype_info[apptype].master_key_nof_segments-srch_ndx-1].segment_type = acl_segments[srch_ndx].segment_type;
        }
    }

    return res;
}

/* build master key for all instructions */
int build_kbp_master_key(int unit, bcm_core_t core, bcm_switch_kbp_info_t* kbp_info, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t* apptype_info)
{
    struct kbp_key *master_key;
    int total_segments_size_bits = 0;
    int segment = 0;
    int reminder_bits = 0;
    int srch_indx = 0;
    int res = 0;

    /* initialize kbp master key */
    res = kbp_key_init((struct kbp_device*)(kbp_info->device_p), &master_key);
    if(res)
    {
        printf("Error, kbp_key_init for master_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize total_segments_size_bits per apptype to 0 */
    total_segments_size_bits = 0;

    /* run over all segments */
    for (segment = 0; segment < apptype_info[apptype].master_key_nof_segments; ++segment)
    {
        /* if one of the segments is above 32b, it is divded to 2 segments. both segments will have the same name.
           in order to add both, need to change the name of the segment */
        for (srch_indx = segment + 1; srch_indx < apptype_info[apptype].master_key_nof_segments; ++srch_indx)
        {
            if (sal_strcmp(((char*)apptype_info[apptype].master_key_segment[segment].segment_name),((char*)apptype_info[apptype].master_key_segment[srch_indx].segment_name)) == 0)
            {
                sal_strncat(((char*)apptype_info[apptype].master_key_segment[segment].segment_name), "1", 1);
            }
        }

        /* if the qualifer is not byte align, do not add this sigment. will add it when segemnt is byte align in next iteration.
           aligned segment will be original one and zero padding. for example dp will be 2b dp then 6b zeros */
        if (apptype_info[apptype].master_key_segment[segment].segment_size_bits%8 != 0)
        {
            apptype_info[apptype].master_key_segment[segment+1].segment_size_bits += apptype_info[apptype].master_key_segment[segment].segment_size_bits;
            sal_strncpy(((char*)apptype_info[apptype].master_key_segment[segment+1].segment_name), 
                        ((char*)apptype_info[apptype].master_key_segment[segment].segment_name), 
                        sizeof(apptype_info[apptype].master_key_segment[segment].segment_name));
            continue;
        }

        /* add fields to kbp master key */
        res = kbp_key_add_field(master_key, ((char*)apptype_info[apptype].master_key_segment[segment].segment_name),
                                apptype_info[apptype].master_key_segment[segment].segment_size_bits,
                                apptype_info[apptype].master_key_segment[segment].segment_type);
        if (res)
        {
            printf("Error, [kbp master key] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
            return  res;
        }

        /* update total segments size */
        total_segments_size_bits = total_segments_size_bits + apptype_info[apptype].master_key_segment[segment].segment_size_bits;
    }

    /* need to padd master key to 80b multipication alignment
       if total number size of segments is not 80b align, add field to align to 80b multiply */
    if ((total_segments_size_bits)%BCM_ALIGN_MASTER_KEY != 0)
    {
        reminder_bits = BCM_ALIGN_MASTER_KEY - (total_segments_size_bits)%BCM_ALIGN_MASTER_KEY;
        res = kbp_key_add_field(master_key, "PADD_80b_multiply", reminder_bits, KBP_KEY_FIELD_HOLE);
        if (res)
        {
            printf("Error, [kbp master key] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }

    /* update total segments size */
    apptype_info[apptype].total_segments_size = total_segments_size_bits + reminder_bits;

    /* set master key */
    res = kbp_instruction_set_key((kbp_instruction_t*)(apptype_info[apptype].inst_p), master_key);
    if(res)
    {
        printf("Error, kbp_instruction_set_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return  res;
}

/* add fw/rpf/dummy dbs to all instructions */
int add_kbp_db(bcm_switch_kbp_info_t kbp_info, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t* apptype_info)
{
    int res = 0;

    /* check if fw db is valid */
    if (apptype_info[apptype].fwd_db)
    {
        /* add fwd db to instruction */
        res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[apptype].inst_p), (struct kbp_db*)(apptype_info[apptype].fwd_db), 0);
        if(res)
        {
            printf("Error, [fwd_db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }

    /* check if dummy db is valid */
    if (apptype_info[apptype].dummy_db)
    {
        /* add dummy db to instruction */
        res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[apptype].inst_p), (struct kbp_db*)(apptype_info[apptype].dummy_db), 1);
        if(res)
        {
            printf("Error, [dummy_db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }

    /* check if rpf db is valid */
    if (apptype_info[apptype].rpf_db)
    {
        /* add rpf db to instruction */
        res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[apptype].inst_p), (struct kbp_db*)(apptype_info[apptype].rpf_db), 2);
        if(res)
        {
            printf("Error, [rpf_db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }

    return  res;
}

/* add new em db */
int add_kbp_new_em_db(bcm_core_t core, bcm_switch_kbp_info_t* kbp_info, bcm_switch_kbp_apptype_info_t* apptype_info, struct kbp_db **kbp_db_em, struct kbp_ad_db **ad_db_em)
{
    struct kbp_key *key_em;
    int db_id_em = 1;
    int db_size = 1024;
    int res = 0;

    /* initialize em db for customer specific configurations */
    res = kbp_db_init((struct kbp_device*)(kbp_info->device_p), KBP_DB_EM, db_id_em, db_size, kbp_db_em);
    if(res)
    {
        printf("Error, [em db] kbp_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize kbp em key */
    res = kbp_key_init((struct kbp_device*)(kbp_info->device_p), &key_em);
    if(res)
    {
        printf("Error, [key_em] kbp_key_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add DIP(32b) field to em kbp key - key format: DIP(32b) */
    res = kbp_key_add_field(key_em, "DIP", 32, KBP_KEY_FIELD_EM);
    if(res)
    {
        printf("Error, [em kbp key - DIP] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add em key to em kbp db */
    res = kbp_db_set_key(*kbp_db_em, key_em);
    if(res)
    {

        printf("Error, [em key] kbp_db_set_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize the em ad db */
    res = kbp_ad_db_init((struct kbp_device*)(kbp_info->device_p), db_id_em, db_size, NLM_TBL_ADLEN_24B, ad_db_em);
    if(res)
    {

        printf("Error, [em ad db] kbp_ad_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate the em ad db with em kbp db lookup */
    res = kbp_db_set_ad(*kbp_db_em, *ad_db_em);
    if(res)
    {

        printf("Error, [em ad] kbp_db_set_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* adding new db to the IPv4 unicast instruction */
    res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[bcmFieldAppTypeIp4Ucast].inst_p), *kbp_db_em, db_id_em);
    if(res)
    {
        printf("Error, [em db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return  res;
}

int add_kbp_new_ternary_db(bcm_core_t core, bcm_switch_kbp_info_t* kbp_info, bcm_switch_kbp_apptype_info_t* apptype_info, struct kbp_db **kbp_db_ternary, struct kbp_ad_db **ad_db_ternary)
{
    struct kbp_key *key_ternary;
    int db_id_ternary = 4;
    int db_size = 1024;
    int res = 0;

     /* initialize ternary db for customer specific configurations */
    res = kbp_db_init((struct kbp_device*)(kbp_info->device_p), KBP_DB_ACL, db_id_ternary, db_size, kbp_db_ternary);
    if(res)
    {
        printf("Error, [ternary db] kbp_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize kbp ternary key */
    res = kbp_key_init((struct kbp_device*)(kbp_info->device_p), &key_ternary);
    if(res)
    {
        printf("Error, [key_ternary] kbp_key_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* key format: ipv4_src_port(16b), tc(3b tc + 5b padding for sigment align) */
    /* add tc(3b tc + 5b padding for sigment align) field to ternary kbp key */
    res = kbp_key_add_field(key_ternary, "fwd_dec_tc", 8, KBP_KEY_FIELD_TERNARY);
    if(res)
    {
        printf("Error, [ternary kbp key - IPv4SrcPort] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ipv4_src_port(16b) field to ternary kbp key */
    res = kbp_key_add_field(key_ternary, "ipv4_src_port", 16, KBP_KEY_FIELD_TERNARY);
    if(res)
    {
        printf("Error, [ternary kbp key - IPv4SrcPort] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ternary key to ternary kbp db  */
    res = kbp_db_set_key(*kbp_db_ternary, key_ternary);
    if(res)
    {

        printf("Error, [ternary key] kbp_db_set_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize the ternary ad db */
    res = kbp_ad_db_init((struct kbp_device*)(kbp_info->device_p), db_id_ternary, db_size, NLM_TBL_ADLEN_24B, ad_db_ternary);
    if(res)
    {

        printf("Error, [ternary ad db] kbp_ad_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate the ternary ad db with ternary kbp db lookup */
    res = kbp_db_set_ad(*kbp_db_ternary, *ad_db_ternary);
    if(res)
    {

        printf("Error, [ternary ad] kbp_db_set_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ternary db to instruction */
    res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[bcmFieldAppTypeIp4Ucast].inst_p), *kbp_db_ternary, db_id_ternary);
    if(res)
    {
        printf("Error, [ternary db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* kbp driver configuration */
int flexible_mode_kbp_configuration(int unit, bcm_core_t core, bcm_switch_kbp_info_t* kbp_info, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t* apptype_info, struct kbp_db **kbp_db_em, struct kbp_db **kbp_db_ternary, struct kbp_ad_db **ad_db_em, struct kbp_ad_db **ad_db_ternary)
{
    int res = 0;

    /* build master key for all instructions */
    res = build_kbp_master_key(unit, core, kbp_info, apptype, apptype_info);
    if(res)
    {
        printf("Error, build_kbp_master_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add new dbs only for bcmFieldAppTypeIp4Ucast apptype */
    if (apptype == bcmFieldAppTypeIp4Ucast) 
    {
        /* add new em db */
        res = add_kbp_new_em_db(core, kbp_info, apptype_info, kbp_db_em, ad_db_em);
        if(res)
        {
            printf("Error, add_kbp_new_db failed: %s\n", kbp_get_status_string(res));
            return  res;
        }

        /* add new ternary db */
        res = add_kbp_new_ternary_db(core, kbp_info, apptype_info, kbp_db_ternary, ad_db_ternary);
        if(res)
        {
            printf("Error, add_kbp_new_ternary_db failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }    

    /* add fw/rpf/dummy dbs to all instructions */
    res = add_kbp_db(*kbp_info, apptype, apptype_info);
    if(res)
    {
        printf("Error, add_kbp_db failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* install instruction */
    res = kbp_instruction_install((kbp_instruction_t*)(apptype_info[apptype].inst_p));
    if(res)
    {
        printf("Error, kbp_instruction_install failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* update lut data per apptype */
int update_kbp_lut_access_info(int unit, bcm_core_t core, bcm_switch_kbp_info_t kbp_info, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t* apptype_info)
{
    int res = 0;

    res = kbp_instruction_set_opcode((kbp_instruction_t*)(apptype_info[apptype].inst_p),apptype_info[apptype].opcode);
    if(res)
    {
        printf("Error, kbp_instruction_set_opcode failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* add em entry to kbp */
int add_em_kbp_entry(struct kbp_db *kbp_db_em, struct kbp_ad_db *ad_db_em)
{
    struct kbp_entry *kbp_entry_em;                     /* kbp em entry */
    struct kbp_ad *ad_entry_em;                         /* kbp ad em entry */
    uint8_t em_data[4] = {0x7f,0xff,0xff,0x03};         /* kbp em data lookup - 4B for DIP */
    uint8_t ad_data = 0x11;                             /* ad data result from kbp */
    int res = 0;

    /* add em entry to em ad db */
    res = kbp_ad_db_add_entry(ad_db_em, &ad_data, &ad_entry_em);
    if(res)
    {
        printf("Error, [em entry] kbp_ad_db_add_entry failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add em entry to kbp em db */
    res = kbp_db_add_em(kbp_db_em, (uint8*)em_data, &kbp_entry_em);
    if(res)
    {
        printf("Error, kbp_db_add_em failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate kbp em entry with em ad entry */
    res = kbp_entry_add_ad(kbp_db_em, kbp_entry_em, ad_entry_em);
    if(res)
    {
        printf("Error, [kbp em entry] kbp_entry_add_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* install kbp em db */
    res = kbp_db_install(kbp_db_em);
    if(res)
    {
        printf("Error, [kbp em db] kbp_db_install failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* add acl entry to kbp */
int add_acl_kbp_entry(struct kbp_db *kbp_db_ternary, struct kbp_ad_db *ad_db_ternary)
{

    struct kbp_entry *kbp_entry_ternary;                /* kbp ternary entry */
    struct kbp_ad *ad_entry_ternary;                    /* kbp ad ternary entry */
    uint8_t ternary_data[3] = {0x00,0x33,0x44};         /* kbp ternary data lookup - 8b (3b tc + 5b zero padding), 16b for L4 src port */
    uint8_t ternary_mask[3] = {0xf8,0x00,0x00};         /* kbp ternary data mask lookup - 8b (3b tc + 5b zero padding), 16b for L4 src port */
    uint32_t priority = 1;                              /* priority for acl entry in kbp */
    uint8_t ad_data = 0x22;                             /* ad data result from kbp */
    int res = 0;

    /* add ternary entry to ternary ad db */
    res = kbp_ad_db_add_entry(ad_db_ternary, &ad_data, &ad_entry_ternary);
    if(res)
    {
        printf("Error, [ternary entry] kbp_ad_db_add_entry failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ternary entry to kbp ternary db */
    res = kbp_db_add_ace(kbp_db_ternary, (uint8*)ternary_data, (uint8*)ternary_mask, priority, &kbp_entry_ternary);
    if(res)
    {
        printf("Error, kbp_db_add_ace failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate kbp ternary entry with ternary ad entry */
    res = kbp_entry_add_ad(kbp_db_ternary, kbp_entry_ternary, ad_entry_ternary);
    if(res)
    {
        printf("Error, [kbp ternary entry] kbp_entry_add_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* install kbp ternary db */
    res = kbp_db_install(kbp_db_ternary);
    if(res)
    {
        printf("Error, [kbp ternary db] kbp_db_install failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* application refernece for kbp flexible mode */
int flexible_mode_ref_app(int unit)
{
    int res = 0;
    bcm_core_t core = 0;                                /* core id */
    bcm_switch_kbp_info_t kbp_info;                     /* hold all kbp configutarion */
    struct kbp_db *kbp_db_em;                           /* kbp em db */
    struct kbp_db *kbp_db_ternary;                      /* kbp ternary db */
    struct kbp_ad_db *ad_db_em;                         /* kbp ad em db */
    struct kbp_ad_db *ad_db_ternary;                    /* kbp ad ternary db */
    bcm_switch_kbp_apptype_info_t apptype_info[bcmFieldAppTypeCount];       /* app type info */
    bcm_field_AppType_t apptype;                        /* apptype */

    /* add acls to master key */
    res = flexible_mode_acl_add(unit);
    if(res)
    {
        printf("Error, flexible_mode_superset_add failed\n");
        return  res;
    }

    /* get kbp pointer from SDK */
    res = bcm_switch_kbp_info_get(unit, core, &kbp_info);
    if(res)
    {
        printf("Error, bcm_switch_kbp_info_get failed\n");
        return  res;
    }

    /* run over all apptypes */
    for (apptype = 0; apptype < bcmFieldAppTypeCount; ++apptype) 
    {
        /* get apptype info per apptype */
        res = bcm_switch_kbp_apptype_info_get(unit, core, apptype, apptype_info);
        if(res)
        {
            printf("Error, bcm_switch_kbp_apptype_info_get failed\n");
            return  res;
        }

        /* check apptype is valid */
        if (apptype_info[apptype].is_valid)
        {
            /* set kbp configuration - customer specific configurations
               also add fwd/rpf dbs and example of em and acl dbs */
            res = flexible_mode_kbp_configuration(unit, core, &kbp_info, apptype, apptype_info, &kbp_db_em, &kbp_db_ternary , &ad_db_em, &ad_db_ternary);
            if(res)
            {
                printf("Error, flexible_kbp_user_specific_configurations failed\n");
                return  res;
            }

            /* update lut info*/
            res = update_kbp_lut_access_info(unit, core, kbp_info, apptype, apptype_info);
            if(res)
            {
                printf("Error, update_kbp_lut_access_info failed\n");
                return  res;
            }
        }
    }

    /* device lock */
    res = kbp_device_lock((struct kbp_device*)(kbp_info.device_p));
    if(res)
    {
        printf("Error, kbp_device_lock failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* example - add em entry to kbp */
    res = add_em_kbp_entry(kbp_db_em, ad_db_em);
    if(res)
    {
        printf("Error, add_em_kbp_entry failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* example - add acl entry to kbp */
    res = add_acl_kbp_entry(kbp_db_ternary, ad_db_ternary);
    if(res)
    {
        printf("Error, add_acl_kbp_entry failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return  res;
}
#else /* not defined(INCLUDE_KBP) */

#include <shared/swstate/access/sw_state_access.h>

/* get kbp pointer from SDK - added for non-kbp compilation case */
int bcm_petra_switch_kbp_info_get(int unit, bcm_core_t core, bcm_switch_kbp_info_t *kbp_info)
{
    return 0;
}

/* get apptype info per apptype - added for non-kbp compilation case */
int bcm_petra_switch_kbp_apptype_info_get(int unit, bcm_core_t core, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t *apptype_info)
{
    return 0;
}

#endif
