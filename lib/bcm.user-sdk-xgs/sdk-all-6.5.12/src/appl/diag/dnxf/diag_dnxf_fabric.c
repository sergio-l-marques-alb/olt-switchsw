/** \file diag_dnxf_fabric.c
 * 
 * Soecial diagnostic and printout functions for DNXF devices
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxc/diag_dnxc_fabric.h>
#include <appl/diag/dnxf/diag_dnxf_fabric.h>

/*bcm*/
#include <bcm/fabric.h>
/*soc*/
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_cgm.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>
#endif /* BCM_DNXF_SUPPORT */
/* Put your new common defines in this file*/
#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnxc/legacy/fabric.h>
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

/*
 * LOCAL DEFINEs:
 * {
 */

 /*
  * LOCAL DEFINEs:
  * }
  */

 /*
  * LOCAL TYPEs:
  * }
  */

 /*
  * LOCAL TYPEs:
  * }
  */

/*
 * LOCAL FUNCTIONs:
 * {
 */

/*
* { DNXF DIAG printout function
*/

shr_error_e
diag_dnxf_thresholds_diag_print(
    int unit,
    int th_group_selected,
    int th_stage_selected,
    int pipe_selected,
    int priority_selected,
    int link_selected,
    sh_sand_control_t * sand_control)
{
    bcm_port_config_t config;
    _shr_dnxf_threshold_id_t threshold_id;
    uint32 threshold_idx, profile_0_link_idx, profile_1_link_idx;
    uint32 flags = 0x0, profile_0_links_count, profile_1_links_count;
    uint32 pipe, level, priority;
    bcm_cast_t cast;
    uint32 current_link_profile, previous_link_profile;
    bcm_port_t link, profile_0_links_arr[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS],
        profile_1_links_arr[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS];
    bcm_port_t first_link = 0, last_link = 0;
    bcm_port_t prof_0_first_link, prof_1_first_link, FE_last_link, FE_first_link;
    bcm_pbmp_t profile_0_links_pbmp, profile_1_links_pbmp;
    uint32 th_validity_bmp, th_with_two_pipes;
    int threshold_value;
    int stage_idx, nof_found_thresholds, i;
    uint32 first_pipe, last_pipe;
    uint32 first_priority, last_priority;
    uint32 found_thresholds_arr[DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLDS] = { 0 };
    /*
     * NOF of threshold stages to iterate over
     */
    uint32 nof_thresholds_stages_to_print;
    /*
     * Default order of the stages for printing thresholds
     */
    uint32 ths_stages_to_print[] = {
        DNXF_DIAG_CGM_TH_STAGE_RX,
        DNXF_DIAG_CGM_TH_STAGE_MIDDLE,
        DNXF_DIAG_CGM_TH_STAGE_TX,
        DNXF_DIAG_CGM_TH_STAGE_SHARED
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &config));
    if (link_selected != DIAG_DNXC_OPTION_ALL && !BCM_PBMP_MEMBER(config.sfi, link_selected))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Specified link is either not valid or not enabled! \n");
    }

    /*
     * Set default iteration parameters
     */
    first_pipe = 0;
    last_pipe = dnxf_data_fabric.pipes.nof_pipes_get(unit) - 1;
    first_priority = 0;
    last_priority = dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit) - 1;

    /*
     * If we have passed an argument we make sure that we will iterate only once over the selected parameter
     */
    if (pipe_selected != DIAG_DNXC_OPTION_ALL)
    {
        if (pipe_selected < 0 || pipe_selected > dnxf_data_fabric.pipes.nof_pipes_get(unit) - 1)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid pipe type parameter specified! Please see 'fabric threshold usage' \n");
        }
        first_pipe = pipe_selected;
        last_pipe = pipe_selected;
    }

    if (priority_selected != DIAG_DNXC_OPTION_ALL)
    {
        if (priority_selected < 0
            || priority_selected > dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit) - 1)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM,
                         "Invalid priority type parameter specified! Please see 'fabric threshold usage' \n");
        }
        first_priority = priority_selected;
        last_priority = priority_selected;
    }

    /*
     * Get all links associated to threshold profile 0
     */
    SHR_IF_ERR_EXIT(bcm_fabric_link_profile_get
                    (unit, 0, flags, dnxf_data_port.general.nof_links_get(unit), &profile_0_links_count,
                     profile_0_links_arr));
    /*
     * Get all links associated to threshold profile 1
     */
    SHR_IF_ERR_EXIT(bcm_fabric_link_profile_get
                    (unit, 1, flags, dnxf_data_port.general.nof_links_get(unit), &profile_1_links_count,
                     profile_1_links_arr));

    /*
     * Convert the two link arrays to port bmps
     */
    BCM_PBMP_CLEAR(profile_0_links_pbmp);
    BCM_PBMP_CLEAR(profile_1_links_pbmp);
    for (profile_0_link_idx = 0; profile_0_link_idx < profile_0_links_count; profile_0_link_idx++)
    {
        BCM_PBMP_PORT_ADD(profile_0_links_pbmp, profile_0_links_arr[profile_0_link_idx]);
    }
    for (profile_1_link_idx = 0; profile_1_link_idx < profile_1_links_count; profile_1_link_idx++)
    {
        BCM_PBMP_PORT_ADD(profile_1_links_pbmp, profile_1_links_arr[profile_1_link_idx]);
    }

    /*
     * Get starting positions
     */
    _SHR_PBMP_FIRST(profile_0_links_pbmp, prof_0_first_link);
    _SHR_PBMP_FIRST(profile_1_links_pbmp, prof_1_first_link);

    if (prof_0_first_link == -1)
    {
        /*
         * If all links are profile 1
         */
        previous_link_profile = 1;
        first_link = prof_1_first_link;
    }
    else if (prof_1_first_link == -1)
    {
        /*
         * If all links are profile 0
         */
        previous_link_profile = 0;
        first_link = prof_0_first_link;
    }
    else
    {
        /*
         * Find which is the first link and its profile
         */
        if (prof_0_first_link < prof_1_first_link)
        {
            previous_link_profile = 0;
            first_link = prof_0_first_link;
        }
        else
        {
            previous_link_profile = 1;
            first_link = prof_1_first_link;
        }
    }
    FE_first_link = first_link;
    /*
     * Get last enabled FE link
     */
    _SHR_PBMP_LAST(config.sfi, FE_last_link);

    /*
     * Iterate over all stages as a Default
     */
    nof_thresholds_stages_to_print = sizeof(ths_stages_to_print) / sizeof(uint32);

    if (th_stage_selected != DIAG_DNXC_OPTION_ALL)
    {
        ths_stages_to_print[0] = th_stage_selected;
        nof_thresholds_stages_to_print = 1;
    }

    /*
     * Iterate over the selected stages
     */
    for (stage_idx = 0; stage_idx < nof_thresholds_stages_to_print; stage_idx++)
    {
        nof_found_thresholds = 0;
        /*
         * Iterate over all thresholds and decide which one to display
         */
        for (threshold_idx = 0; threshold_idx < dnxf_data_fabric.congestion.nof_thresholds_get(unit); threshold_idx++)
        {
            /*
             * Check if threshold should be displayed or not
             */
            if (!(dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->is_to_be_displayed))
            {
                continue;
            }
            /*
             * Check if currently displayed stage matches the one of the threshold
             */
            if (ths_stages_to_print[stage_idx] !=
                dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_stage)
            {
                continue;
            }

            /*
             * Filter only thresholds for selected Threshold Type (GCI/RCI/FC/DROP/SIZE)
             */
            if (dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_group !=
                th_group_selected)
            {
                continue;
            }

            /*
             * Check if we haven't already found a threshold for this GROUP on this STAGE
             */
            for (i = 0; i < nof_found_thresholds; i++)
            {
                if (threshold_idx == found_thresholds_arr[i])
                {
                    continue;
                }
            }
            /*
             * If the threhold uis indeed valid save it to the database and increment the counetr
             */
            found_thresholds_arr[nof_found_thresholds] = threshold_idx;
            nof_found_thresholds++;

            /*
             * Get the features validity bit map for the current threshold from the main info table using the key
             */
            th_validity_bmp = dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->th_validity_bmp;

            /*
             * Check if threshold has vallues only for pipe 0 and pipe 1. If so raise a flag
             */
            if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_ONLY_TWO_PIPES)
            {
                if (pipe_selected == 2)
                {
                    /*
                     * Print the name and skip it
                     */
                    LOG_CLI(("The threshold %s doesn't have values for pipe 2.\n",
                             dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str));
                    continue;
                }
                if (pipe_selected == DIAG_DNXC_OPTION_ALL)
                {
                    LOG_CLI(("The following threshold doesn't have values for pipe 2.\n"));
                }
                th_with_two_pipes = 1;
            }
            else
            {
                th_with_two_pipes = 0;
            }

            /*
             * Four handler switches for the four threshold indexes types 1._shr_dnxf_cgm_index_type_pipe
             * 2._shr_dnxf_cgm_index_type_pipe_level 3._shr_dnxf_cgm_index_type_priority
             * 4._shr_dnxf_cgm_index_type_cast_prio 
             */
            /*
             * PIPE type threshold { 
             */
            if (dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->index_type ==
                _shr_dnxf_cgm_index_type_pipe)
            {
                PRT_TITLE_SET("%s",
                              dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str);

                PRT_COLUMN_ADD("Links");
                if (pipe_selected == DIAG_DNXC_OPTION_ALL)
                {
                    PRT_COLUMN_ADD("Pipe 0");
                    PRT_COLUMN_ADD("Pipe 1");
                    PRT_COLUMN_ADD("Pipe 2");
                }
                else
                {
                    PRT_COLUMN_ADD("Pipe %d", first_pipe);
                }

                /*
                 * Profile non dependant thresholds will printout values for all links
                 */
                if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT)
                {
                    previous_link_profile = 0;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    if (link_selected != DIAG_DNXC_OPTION_ALL)
                    {
                        PRT_CELL_SET("%d", link_selected);
                    }
                    else
                    {
                        PRT_CELL_SET("ALL");
                    }
                    for (pipe = first_pipe; pipe <= last_pipe; pipe++)
                    {
                        if (th_with_two_pipes && pipe == 2)
                        {
                            PRT_CELL_SET("N/A");
                        }
                        else
                        {
                            threshold_id = pipe;
                            SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                            (unit, previous_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                             (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                            PRT_CELL_SET("%d", threshold_value);
                        }
                    }
                }
                else
                {
                    /*
                     * Two cases handeled below 1. If specific link selected: only one iteration - print link and its
                     * threshold values 2. If requested info is for all links: group links with same profile and print 
                     * them in consequiteve manner 
                     */
                    BCM_PBMP_ITER(config.sfi, link)
                    {
                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            link = link_selected;
                        }
                        /*
                         * See what is the profile of the link
                         */
                        if (BCM_PBMP_MEMBER(profile_0_links_pbmp, link))
                        {
                            current_link_profile = 0;
                        }
                        else
                        {
                            current_link_profile = 1;
                        }

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            previous_link_profile = current_link_profile;
                        }
                        else
                        {
                            /*
                             * Skip iteration until you see a link with different profile
                             */
                            if (current_link_profile == previous_link_profile && link != FE_last_link)
                            {
                                continue;
                            }
                            else
                            {
                                if (link != FE_last_link)
                                {
                                    last_link = link - 1;
                                }
                                else
                                {
                                    last_link = FE_last_link;
                                }
                            }
                        }
                        /*
                         * Print link number on link group
                         */
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            PRT_CELL_SET("%d", link_selected);
                        }
                        else
                        {
                            if (first_link != last_link)
                            {
                                PRT_CELL_SET("%d - %d", first_link, last_link);
                            }
                            else
                            {
                                PRT_CELL_SET("%d", first_link);
                            }
                        }

                        /*
                         * Print next group of links associated to same threshold profile
                         */
                        for (pipe = first_pipe; pipe <= last_pipe; pipe++)
                        {
                            if (th_with_two_pipes && pipe == 2)
                            {
                                PRT_CELL_SET("N/A");
                            }
                            else
                            {
                                threshold_id = pipe;
                                SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                                (unit, previous_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                                 (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                                PRT_CELL_SET("%d", threshold_value);
                            }
                        }

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            break;
                        }

                        /*
                         * Prepare for next profile iteration
                         */
                        previous_link_profile = current_link_profile;
                        first_link = link;
                    }
                }
                first_link = FE_first_link;
                PRT_COMMITX;
            }
            /*
             * PIPE type threshold } 
             */
            /*
             * PIPE LEVEL type threshold { 
             */
            if (dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->index_type ==
                _shr_dnxf_cgm_index_type_pipe_level)
            {
                PRT_TITLE_SET("%s",
                              dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str);

                PRT_COLUMN_ADD("Link");
                PRT_COLUMN_ADD("Level");
                if (pipe_selected == DIAG_DNXC_OPTION_ALL)
                {
                    PRT_COLUMN_ADD("Pipe 0");
                    PRT_COLUMN_ADD("Pipe 1");
                    PRT_COLUMN_ADD("Pipe 2");
                }
                else
                {
                    PRT_COLUMN_ADD("Pipe %d", pipe_selected);
                }
                /*
                 * Profile non dependant thresholds will printout values for all links
                 */
                if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT)
                {
                    previous_link_profile = 0;

                    for (level = 0; level < dnxf_data_fabric.congestion.nof_threshold_levels_get(unit); level++)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            PRT_CELL_SET("%d", link_selected);
                        }
                        else
                        {
                            PRT_CELL_SET("ALL");
                        }

                        PRT_CELL_SET("%d", level);
                        for (pipe = first_pipe; pipe <= last_pipe; pipe++)
                        {
                            threshold_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(pipe, level);
                            SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                            (unit, previous_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                             (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                            PRT_CELL_SET("%d", threshold_value);
                        }
                    }
                }
                else
                {
                    /*
                     * Two cases handeled below 1. If specific link selected: only one iteration - print link and its
                     * threshold values 2. If requested info is for all links: group links with same profile and print 
                     * them in consequiteve manner 
                     */
                    BCM_PBMP_ITER(config.sfi, link)
                    {
                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            link = link_selected;
                        }
                        /*
                         * See what is the profile of the link
                         */
                        if (BCM_PBMP_MEMBER(profile_0_links_pbmp, link))
                        {
                            current_link_profile = 0;
                        }
                        else
                        {
                            current_link_profile = 1;
                        }

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            previous_link_profile = current_link_profile;
                        }
                        else
                        {
                            /*
                             * Skip iteration until you see a link with different profile
                             */
                            if (current_link_profile == previous_link_profile && link != FE_last_link)
                            {
                                continue;
                            }
                            else
                            {
                                if (link != FE_last_link)
                                {
                                    last_link = link - 1;
                                }
                                else
                                {
                                    last_link = FE_last_link;
                                }
                            }
                        }

                        /*
                         * Print next group of links associated to same threshold profile
                         */
                        for (level = 0; level < dnxf_data_fabric.congestion.nof_threshold_levels_get(unit); level++)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                            /*
                             * Print link number on link group
                             */
                            if (link_selected != DIAG_DNXC_OPTION_ALL)
                            {
                                PRT_CELL_SET("%d", link_selected);
                            }
                            else
                            {
                                if (first_link != last_link)
                                {
                                    PRT_CELL_SET("%d - %d", first_link, last_link);
                                }
                                else
                                {
                                    PRT_CELL_SET("%d", first_link);
                                }
                            }

                            PRT_CELL_SET("%d", level);
                            for (pipe = first_pipe; pipe <= last_pipe; pipe++)
                            {
                                threshold_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(pipe, level);
                                SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                                (unit, previous_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                                 (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                                PRT_CELL_SET("%d", threshold_value);
                            }
                        }

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            break;
                        }

                        /*
                         * Prepare for next profile iteration
                         */
                        previous_link_profile = current_link_profile;
                        first_link = link;
                    }
                }
                first_link = FE_first_link;
                PRT_COMMITX;
            }
            /*
             * PIPE LEVEL type threshold } 
             */
            /*
             * PRIORITY type threshold { 
             */
            if (dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->index_type ==
                _shr_dnxf_cgm_index_type_priority)
            {
                PRT_TITLE_SET("%s",
                              dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str);

                PRT_COLUMN_ADD("Link");
                if (priority_selected == DIAG_DNXC_OPTION_ALL)
                {
                    PRT_COLUMN_ADD("Priority 0");
                    PRT_COLUMN_ADD("Priority 1");
                    PRT_COLUMN_ADD("Priority 2");
                    PRT_COLUMN_ADD("Priority 3");
                }
                else
                {
                    PRT_COLUMN_ADD("Priority %d", priority_selected);
                }
                /*
                 * Profile non dependant thresholds will printout values for all links
                 */
                if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT)
                {
                    previous_link_profile = 0;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                    if (link_selected != DIAG_DNXC_OPTION_ALL)
                    {
                        PRT_CELL_SET("%d", link_selected);
                    }
                    else
                    {
                        PRT_CELL_SET("ALL");
                    }

                    for (priority = first_priority; priority <= last_priority; priority++)
                    {
                        threshold_id = priority;
                        SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                        (unit, previous_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                         (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                        PRT_CELL_SET("%d", threshold_value);
                    }
                }
                else
                {
                    /*
                     * Two cases handeled below 1. If specific link selected: only one iteration - print link and its
                     * threshold values 2. If requested info is for all links: group links with same profile and print 
                     * them in consequiteve manner 
                     */
                    BCM_PBMP_ITER(config.sfi, link)
                    {
                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            link = link_selected;
                        }
                        /*
                         * See what is the profile of the link
                         */
                        if (BCM_PBMP_MEMBER(profile_0_links_pbmp, link))
                        {
                            current_link_profile = 0;
                        }
                        else
                        {
                            current_link_profile = 1;
                        }

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            previous_link_profile = current_link_profile;
                        }
                        else
                        {
                            /*
                             * Skip iteration until you see a link with different profile
                             */
                            if (current_link_profile == previous_link_profile && link != FE_last_link)
                            {
                                continue;
                            }
                            else
                            {
                                if (link != FE_last_link)
                                {
                                    last_link = link - 1;
                                }
                                else
                                {
                                    last_link = FE_last_link;
                                }
                            }
                        }
                        /*
                         * Print link number on link group
                         */
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            PRT_CELL_SET("%d", link_selected);
                        }
                        else
                        {
                            if (first_link != last_link)
                            {
                                PRT_CELL_SET("%d - %d", first_link, last_link);
                            }
                            else
                            {
                                PRT_CELL_SET("%d", first_link);
                            }
                        }

                        for (priority = first_priority; priority <= last_priority; priority++)
                        {
                            threshold_id = priority;
                            SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                            (unit, previous_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                             (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                            PRT_CELL_SET("%d", threshold_value);
                        }

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            break;
                        }

                        /*
                         * Prepare for next profile iteration
                         */
                        previous_link_profile = current_link_profile;
                        first_link = link;
                    }
                }
                first_link = FE_first_link;
                PRT_COMMITX;
            }
            /*
             * PRIORITY type threshold } 
             */

            /*
             * CAST PRIORITY type threshold { 
             */
            if (dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->index_type ==
                _shr_dnxf_cgm_index_type_cast_prio)
            {
                PRT_TITLE_SET("%s",
                              dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str);

                PRT_COLUMN_ADD("Link");
                PRT_COLUMN_ADD("CAST");
                if (priority_selected == DIAG_DNXC_OPTION_ALL)
                {
                    PRT_COLUMN_ADD("Priority 0");
                    PRT_COLUMN_ADD("Priority 1");
                    PRT_COLUMN_ADD("Priority 2");
                    PRT_COLUMN_ADD("Priority 3");
                }
                else
                {
                    PRT_COLUMN_ADD("Priority %d", priority_selected);
                }
                /*
                 * Profile non dependant thresholds will printout values for all links
                 */
                if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT)
                {
                    previous_link_profile = 0;
                    for (cast = 0; cast < 2; cast++)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            PRT_CELL_SET("%d", link_selected);
                        }
                        else
                        {
                            PRT_CELL_SET("ALL");
                        }

                        if (cast == bcmCastUC)
                        {
                            PRT_CELL_SET("%s", "UC");
                        }
                        else
                        {
                            PRT_CELL_SET("%s", "MC");
                        }
                        for (priority = first_priority; priority <= last_priority; priority++)
                        {
                            threshold_id = BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(cast, priority);
                            SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                            (unit, previous_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                             (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                            PRT_CELL_SET("%d", threshold_value);
                        }
                    }
                }
                else
                {
                    BCM_PBMP_ITER(config.sfi, link)
                    {
                        /*
                         * Two cases handeled below 1. If specific link selected: only one iteration - print link and
                         * its threshold values 2. If requested info is for all links: group links with same profile
                         * and print them in consequiteve manner 
                         */
                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            link = link_selected;
                        }
                        /*
                         * See what is the profile of the link
                         */
                        if (BCM_PBMP_MEMBER(profile_0_links_pbmp, link))
                        {
                            current_link_profile = 0;
                        }
                        else
                        {
                            current_link_profile = 1;
                        }

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            previous_link_profile = current_link_profile;
                        }
                        else
                        {
                            /*
                             * Skip iteration until you see a link with different profile
                             */
                            if (current_link_profile == previous_link_profile && link != FE_last_link)
                            {
                                continue;
                            }
                            else
                            {
                                if (link != FE_last_link)
                                {
                                    last_link = link - 1;
                                }
                                else
                                {
                                    last_link = FE_last_link;
                                }
                            }
                        }

                        for (cast = 0; cast < 2; cast++)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                            /*
                             * Print link number on link group
                             */
                            if (link_selected != DIAG_DNXC_OPTION_ALL)
                            {
                                PRT_CELL_SET("%d", link_selected);
                            }
                            else
                            {
                                if (first_link != last_link)
                                {
                                    PRT_CELL_SET("%d - %d", first_link, last_link);
                                }
                                else
                                {
                                    PRT_CELL_SET("%d", first_link);
                                }
                            }
                            if (cast == bcmCastUC)
                            {
                                PRT_CELL_SET("%s", "UC");
                            }
                            else
                            {
                                PRT_CELL_SET("%s", "MC");
                            }
                            for (priority = first_priority; priority <= last_priority; priority++)
                            {
                                threshold_id = BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(cast, priority);
                                SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                                (unit, previous_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                                 (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                                PRT_CELL_SET("%d", threshold_value);
                            }
                        }

                        if (link_selected != DIAG_DNXC_OPTION_ALL)
                        {
                            break;
                        }

                        /*
                         * Prepare for next profile iteration
                         */
                        previous_link_profile = current_link_profile;
                        first_link = link;
                    }
                }
                first_link = FE_first_link;
                PRT_COMMITX;
            }
            /*
             * CAST PRIORITY type threshold } 
             */
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dnxf_queues_diag_print(
    int unit,
    int queue_stage_selected,
    sh_sand_control_t * sand_control)
{

    uint32 nof_pipes;
    int pipe_idx, block_idx, fifo_idx, dfl_bank_idx, ifm_block_idx;
    int sub_bank_idx = -1, most_occupied_bank = -1;
    uint32 dtl_link_group_first_link = 0, dtl_link_group_last_link = 0;
    uint32 max_nof_sub_bank_entries = 0, min_nof_bank_free_entries = 0;
    uint32 current_dfl_sub_bank_0_value, current_dfl_sub_bank_1_value;
    uint32 current_fifo_max_occupancy;
    soc_dnxf_queues_info_t fe_queues_info;
    char sub_bank_idx_buf[5], most_occupied_bank_buf[5];
    soc_dnxf_queues_dch_link_group_t *current_dch_link_group;
    soc_dnxf_queues_dtl_info_t *current_dtl_block;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI(("QUEUES status\n"));

    /*
     * Init the diag info structure which will be used to store the inforamation
     */
    soc_dnxf_queues_info_init(unit, &fe_queues_info);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_queues_get_info, (unit, &fe_queues_info)));
    nof_pipes = fe_queues_info.nof_pipes;

    /*
     * DCH print 
     */
    if (queue_stage_selected == DIAG_DNXC_OPTION_ALL || queue_stage_selected == diag_dch_stage_option)
    {
        PRT_TITLE_SET("DCH QUEUES");

        PRT_COLUMN_ADD("DCH#");
        PRT_COLUMN_ADD("Pipe");
        PRT_COLUMN_ADD("Link Group");
        PRT_COLUMN_ADD("Link");
        PRT_COLUMN_ADD("Max Occupancy level");

        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); block_idx++)
            {
                /*
                 * Each DCH is divided into 3 big IFM FIFO-s each responsible for 8 links from the overall 24
                 */
                for (ifm_block_idx = 0; ifm_block_idx < dnxf_data_device.blocks.nof_dch_link_groups_get(unit);
                     ifm_block_idx++)
                {
                    /*
                     * Display vallues only if there is any traffic through the link group
                     */
                    current_dch_link_group =
                        &fe_queues_info.dch_queues_info[pipe_idx].nof_dch[block_idx].dch_link_group[ifm_block_idx];
                    if (current_dch_link_group->max_occupancy_value)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("DCH%d", block_idx);
                        PRT_CELL_SET("%d", pipe_idx);
                        PRT_CELL_SET("%d - %d", current_dch_link_group->link_group_first_link,
                                     current_dch_link_group->link_group_last_link);
                        PRT_CELL_SET("%u", current_dch_link_group->most_occupied_link);
                        PRT_CELL_SET("%u", current_dch_link_group->max_occupancy_value);
                    }
                }
            }
        }
        PRT_COMMITX;
    }
    /*
     * DTM Printing { 
     */
    if (queue_stage_selected == DIAG_DNXC_OPTION_ALL || queue_stage_selected == diag_dtm_stage_option)
    {
        PRT_TITLE_SET("DTM QUEUES");

        PRT_COLUMN_ADD("DTM#");
        PRT_COLUMN_ADD("Pipe");
        PRT_COLUMN_ADD("DCH FIFO");
        PRT_COLUMN_ADD("MAX occupancy value");

        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
            {
                for (fifo_idx = 0; fifo_idx < dnxf_data_device.blocks.nof_dtm_fifos_get(unit); fifo_idx++)
                {
                    current_fifo_max_occupancy =
                        fe_queues_info.dtm_queues_info[pipe_idx].nof_dtm[block_idx].fifo_max_occupancy_value[fifo_idx];
                    /*
                     * Print only if value different than 0
                     */
                    if (current_fifo_max_occupancy)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("DTM%d", block_idx);
                        PRT_CELL_SET("%d", pipe_idx);
                        PRT_CELL_SET("%d", fifo_idx);
                        PRT_CELL_SET("%u", current_fifo_max_occupancy);
                    }
                }
            }
        }
        PRT_COMMITX;
        /*
         * DTM Printing } 
         */
    }

    /*
     * DTL print { 
     */
    if (queue_stage_selected == DIAG_DNXC_OPTION_ALL || queue_stage_selected == diag_dtl_stage_option)
    {

        PRT_TITLE_SET("DTL QUEUES");
        if (SOC_DNXF_IS_FE13(unit))
        {
            PRT_INFO_ADD("NON-Local route");
        }

        PRT_COLUMN_ADD("DTL #");
        PRT_COLUMN_ADD("Pipe");
        PRT_COLUMN_ADD("Link Group");
        PRT_COLUMN_ADD("Link");
        PRT_COLUMN_ADD("Max Occupancy level");

        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
            {
                current_dtl_block = &fe_queues_info.dtl_queues_info_nlr[pipe_idx].nof_dtl[block_idx];

                /*
                 * Print only if value different than 0
                 */
                if (current_dtl_block->max_occupancy_value)
                {
                    dtl_link_group_first_link = block_idx * dnxf_data_device.blocks.nof_links_in_dcq_get(unit);
                    dtl_link_group_last_link =
                        dtl_link_group_first_link + dnxf_data_device.blocks.nof_links_in_dcq_get(unit) - 1;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                    PRT_CELL_SET("DTL%d", block_idx);
                    PRT_CELL_SET("%d", pipe_idx);

                    PRT_CELL_SET("%d - %d", dtl_link_group_first_link, dtl_link_group_last_link);

                    PRT_CELL_SET("%u", current_dtl_block->most_occupied_link);
                    PRT_CELL_SET("%u", current_dtl_block->max_occupancy_value);
                }
            }
        }
        PRT_COMMITX;
    }
    /*
     * If FE13 Print Local-route table also
     */
    if (SOC_DNXF_IS_FE13(unit))
    {
        PRT_TITLE_SET("DTL QUEUES");
        PRT_INFO_ADD("Local route");

        PRT_COLUMN_ADD("DTL #");
        PRT_COLUMN_ADD("Pipe");
        PRT_COLUMN_ADD("Link Group");
        PRT_COLUMN_ADD("Link");
        PRT_COLUMN_ADD("Max Occupancy level");

        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
            {
                current_dtl_block = &fe_queues_info.dtl_queues_info_lr[pipe_idx].nof_dtl[block_idx];
                /*
                 * Print only if value different than 0
                 */
                if (current_dtl_block->max_occupancy_value)
                {
                    dtl_link_group_first_link = block_idx * dnxf_data_device.blocks.nof_links_in_dcq_get(unit);
                    dtl_link_group_last_link =
                        dtl_link_group_first_link + dnxf_data_device.blocks.nof_links_in_dcq_get(unit) - 1;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                    PRT_CELL_SET("DTL%d", block_idx);
                    PRT_CELL_SET("%d", pipe_idx);

                    PRT_CELL_SET("%d - %d", dtl_link_group_first_link, dtl_link_group_last_link);
                    PRT_CELL_SET("%u", current_dtl_block->most_occupied_link);
                    PRT_CELL_SET("%u", current_dtl_block->max_occupancy_value);
                }
            }
        }
        PRT_COMMITX;
    }
    /*
     * DTL print } 
     */

    /*
     * DFL print { 
     */
    if (queue_stage_selected == DIAG_DNXC_OPTION_ALL || queue_stage_selected == diag_dfl_stage_option)
    {
        PRT_TITLE_SET("DFL STATUS");

        PRT_COLUMN_ADD("DFL#");
        PRT_COLUMN_ADD("BANK");
        PRT_COLUMN_ADD("Sub-BANK");
        PRT_COLUMN_ADD("MAX entries");

        /*
         * MAX NOF sub-bank entries is 1800 = max NOF DFL bank entries (3600) / NOF sub-banks(2) 
         */
        max_nof_sub_bank_entries =
            dnxf_data_fabric.congestion.nof_dfl_bank_entries_get(unit) /
            dnxf_data_fabric.congestion.nof_dfl_sub_banks_get(unit);

        for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
        {
            /*
             * Reset values for next DCML block iteration
             */
            most_occupied_bank = -1;
            sub_bank_idx = -1;
            min_nof_bank_free_entries = max_nof_sub_bank_entries;

            for (dfl_bank_idx = 0; dfl_bank_idx < dnxf_data_fabric.congestion.nof_dfl_banks_get(unit); dfl_bank_idx++)
            {
                current_dfl_sub_bank_0_value =
                    fe_queues_info.dfl_queues_info[block_idx].nof_dfl[dfl_bank_idx].sub_bank_min_free_entries_value[0];
                current_dfl_sub_bank_1_value =
                    fe_queues_info.dfl_queues_info[block_idx].nof_dfl[dfl_bank_idx].sub_bank_min_free_entries_value[1];

                /*
                 * Check if current bank MIN free entries is smaller than current MIN value (Reset value is 1800)
                 */
                if (min_nof_bank_free_entries > current_dfl_sub_bank_0_value
                    || min_nof_bank_free_entries > current_dfl_sub_bank_1_value)
                {
                    most_occupied_bank = dfl_bank_idx;
                    /*
                     * See which sub-bank has smaller value for minimum free entries
                     */
                    if (current_dfl_sub_bank_0_value < current_dfl_sub_bank_1_value)
                    {
                        min_nof_bank_free_entries = current_dfl_sub_bank_0_value;
                        sub_bank_idx = 0;
                    }
                    else
                    {
                        min_nof_bank_free_entries = current_dfl_sub_bank_1_value;
                        sub_bank_idx = 1;
                    }
                }
            }
            sal_snprintf(most_occupied_bank_buf, 5, "%d", most_occupied_bank);
            sal_snprintf(sub_bank_idx_buf, 5, "%d", sub_bank_idx);

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("DFL%d", block_idx);
            PRT_CELL_SET("%s", most_occupied_bank != -1 ? most_occupied_bank_buf : "No occupancy");
            PRT_CELL_SET("%s", sub_bank_idx != -1 ? sub_bank_idx_buf : "No occupancy");
            /*
             * Keep the tendency from previous tables and print maximum BANK occupancy For that Show the max nof of
             * entries that occupies the BANK by substracting From the MAX entries value(1800) the minimum value of
             * free entries
             */
            PRT_CELL_SET("%u", max_nof_sub_bank_entries - min_nof_bank_free_entries);
        }

        PRT_COMMITX;
    }
    /*
     * DFL print } 
     */
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*Diag counters graphycal DNXF printout function*/
shr_error_e
diag_dnxf_counters_graphical_print(
    int unit,
    sh_sand_control_t * sand_control)
{
    uint32 nof_pipes, max_cells_in_fifos;
    uint64 temp_counter_data, dropped_cells;
    uint32 pipe_idx, prio_idx;
    soc_dnxf_counters_info_t fe_counters_info;
    char *printout_buffer_1[50];
    char *printout_buffer_2[50];
    char buf_val[32];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get counters Info
     */
    soc_dnxf_counters_info_init(&fe_counters_info);
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_counters_get_info, (unit, &fe_counters_info)));
    nof_pipes = fe_counters_info.nof_pipes;

    if (SOC_DNXF_IS_FE2(unit))
    {
        /*
         * Print diag counter information for FE2 { 
         */
        switch (nof_pipes)
        {
            case 1:
                *printout_buffer_1 = "                 ";
                break;
            case 2:
                *printout_buffer_1 = "                          ";
                break;
            case 3:
                *printout_buffer_1 = "                                   ";
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid number of pipes received!");
        }
        /*
         * DCH Print * { 
         */
        LOG_CLI(("%s    ||%s\n", *printout_buffer_1, *printout_buffer_1));
        LOG_CLI(("%s    \\/%s\n", *printout_buffer_1, *printout_buffer_1));
        PRT_TITLE_SET("%s  FE2", *printout_buffer_1);
        PRT_INFO_ADD("%sDCH(0-7)", *printout_buffer_1);
        PRT_COLUMN_ADD("  Counter name            ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_total_in +
                                  fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_total_in, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Fifo Discard:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_fifo_discard +
                                  fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_fifo_discard, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Unreach Discard:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_unreach_discard +
                                  fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_unreach_discard, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            max_cells_in_fifos =
                UTILEX_MAX(fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_max_fifo_cells,
                           fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_max_fifo_cells);
            PRT_CELL_SET("%d", max_cells_in_fifos);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_total_out +
                                  fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_COMMITX;
        /*
         * DCH Print * } 
         */
        LOG_CLI(("%s    ||%s\n", *printout_buffer_1, *printout_buffer_1));
        LOG_CLI(("%s    \\/%s\n", *printout_buffer_1, *printout_buffer_1));
        /*
         * DTM Print { 
         */
        PRT_TITLE_SET("%sDTM(0-7)", *printout_buffer_1);

        PRT_COLUMN_ADD("  Counter name            ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }

        /*
         * DTM Total incoming Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total incomming cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            temp_counter_data = fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_in_lr +
                fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_in_nlr +
                fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_in_lr +
                fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_in_nlr;
            format_uint64_decimal(buf_val, temp_counter_data, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTM MAX Cells in FIFO
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            max_cells_in_fifos = UTILEX_MAX(fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_max_fifo_cells_nlr,
                                            UTILEX_MAX(fe_counters_info.
                                                       dtm_fe3_counters_info[pipe_idx].dtm_max_fifo_cells_nlr,
                                                       fe_counters_info.
                                                       dtm_fe3_counters_info[pipe_idx].dtm_max_fifo_cells_lr));
            PRT_CELL_SET("%d", max_cells_in_fifos);
        }

        /*
         * DTM Total outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            temp_counter_data = fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_out_nlr +
                fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_out_lr +
                fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_out_nlr +
                fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_out_lr;
            format_uint64_decimal(buf_val, temp_counter_data, ',');
            PRT_CELL_SET("%s", buf_val);

        }

        PRT_COMMITX;
        /*
         * DTM Print } 
         */
        LOG_CLI(("%s    ||%s\n", *printout_buffer_1, *printout_buffer_1));
        LOG_CLI(("%s    \\/%s\n", *printout_buffer_1, *printout_buffer_1));
        /*
         * DTL Print { 
         */
        PRT_TITLE_SET("%sDTL(0-7)", *printout_buffer_1);

        PRT_COLUMN_ADD("  Counter name            ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }

        /*
         * DTL Total incoming Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incomming cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_in_nlr +
                                  fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL Dropped Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Dropped Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            dropped_cells = 0;
            for (prio_idx = 0; prio_idx < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); prio_idx++)
            {
                dropped_cells += fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_dropped_nlr_cells[prio_idx];
                dropped_cells += fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_dropped_nlr_cells[prio_idx];
            }
            format_uint64_decimal(buf_val, dropped_cells, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL MAX Cells in FIFOs
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            max_cells_in_fifos =
                UTILEX_MAX(fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_max_fifo_cells_nlr,
                           fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_max_fifo_cells_nlr);
            PRT_CELL_SET("%d", max_cells_in_fifos);
        }

        /*
         * DTL Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_out +
                                  fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL Print } 
         */

        PRT_COMMITX;

        /*
         * Print diag counter information for FE2 } 
         */
    }
    else
    {

        switch (nof_pipes)
        {
            case 1:
                *printout_buffer_1 = "                          ";
                *printout_buffer_2 = "               ";
                break;
            case 2:
                *printout_buffer_1 = "                                ";
                *printout_buffer_2 = "                          ";
                break;
            case 3:
                *printout_buffer_1 = "                                       ";
                *printout_buffer_2 = "                                     ";
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid number of pipes received!");
        }

        LOG_CLI(("%s||%s         %s/\\ \n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%s||%sFAP LINKS%s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%s\\/%s         %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%sFE1%s        %sFE3\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));

        /*
         * Title { 
         */
        PRT_TITLE_SET("");

        PRT_COLUMN_ADD("   Counter name   ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }

        PRT_COLUMN_ADD("");

        PRT_COLUMN_ADD("   Counter name   ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }
        /*
         * Title } 
         */
        /*
         * First Stage { 
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SKIP(1);
        PRT_CELL_SET("   DCH(0-3) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }
        PRT_CELL_SKIP(2);
        PRT_CELL_SET("   DTL(0-3) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }

        /*
         * DCH(0-3) Total INCOMING DTL(0-3) Total OUTGOING
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_total_in, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DCH(0-3) FIFO discard DTL(0-3) LOCAL ROUTE PRINT START
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("FIFO Discard:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_fifo_discard, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("LOCAL ROUTE");
        PRT_CELL_SKIP(nof_pipes);

        /*
         * DCH(0-3) Unreach discard DTL(0-3) LOCAL ROUTE Dropped Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Unreach Discard:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_unreach_discard, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  Dropped LR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            dropped_cells = 0;
            for (prio_idx = 0; prio_idx < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); prio_idx++)
            {
                dropped_cells += fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_dropped_lr_cells[prio_idx];
            }
            format_uint64_decimal(buf_val, dropped_cells, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DCH(0-3) MAX FIFO Cells DTL(0-3) LOCAL ROUTE MAX LR Cells in FIFOs
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_max_fifo_cells);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  MAX LR Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_max_fifo_cells_lr);
        }

        /*
         * DCH(0-3) Total Outgoing Cells DTL(0-3) LOCAL ROUTE Total Incoming Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET("---");
        PRT_CELL_SET("  Total Incoming LR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_in_lr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DCH(0-3) - DTL(0-3) NON-LOCAL ROUTE PRINT START
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("   |");
        PRT_CELL_SET("NON-LOCAL ROUTE");
        PRT_CELL_SKIP(3);

        /*
         * DCH(0-3) - DTL(0-3) NON-LOCAL ROUTE Dropped Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  Dropped NLR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            dropped_cells = 0;
            for (prio_idx = 0; prio_idx < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); prio_idx++)
            {
                dropped_cells += fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_dropped_nlr_cells[prio_idx];
            }
            format_uint64_decimal(buf_val, dropped_cells, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DCH(0-3) - DTL(0-3) NON-LOCAL ROUTE MAX LR Cells in FIFOs
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  MAX NLR Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_max_fifo_cells_nlr);
        }

        /*
         * DCH(0-3) - DTL(0-3) NON-LOCAL ROUTE Total incoming
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  Total incoming NLR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        /*
         * First Stage } 
         */

        /*
         * Second Stage { 
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SKIP(1);
        PRT_CELL_SET("   DTM(4-7) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }
        PRT_CELL_SET("   | ");
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("   DTM(0-3)");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }

        /*
         * DTM(4-7) Total Incoming Cells DTM(0-3) LOCAL ROUTE PRINT START
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET("   |");
        PRT_CELL_SET("LOCAL ROUTE:");
        PRT_CELL_SKIP(nof_pipes);

        /*
         * DTM(4-7) MAX Cells in FIFOs DTM(0-3) LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_max_fifo_cells_nlr);
        }
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  Total Outgoing LR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_out_lr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTM(4-7) Total Outgoing Cells DTM(0-3) LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_out_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  MAX LR Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_max_fifo_cells_lr);
        }

        /*
         * DTM(4-7) - DTM(0-3) LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("    -->");
        PRT_CELL_SET("  Total Incoming LR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_in_lr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTM(4-7) - DTM(0-3) NON-LOCAL ROUTE PRINT START
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("NON-LOCAL ROUTE");
        PRT_CELL_SKIP(nof_pipes);

        /*
         * DTM(4-7) MAX Cells in FIFOs DTM(0-3) NON-LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  Total Outgoing NLR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_out_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTM(4-7) MAX Cells in FIFOs DTM(0-3) NON-LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  MAX NLR Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_max_fifo_cells_nlr);
        }

        /*
         * DTM(4-7) - DTM(0-3) NON-LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  Total Incoming NLR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        /*
         * Second Stage } 
         */

        /*
         * Third Stage { 
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SKIP(1);
        PRT_CELL_SET("   DTL(4-7) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }
        PRT_CELL_SKIP(2);
        PRT_CELL_SET("   DCH(4-7) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }

        /*
         * DTL(4-7) Total Incoming Cells DCH(4-7) Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL(4-7) Dropped Cells DCH(4-7) FIFO Discard
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Dropped Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            dropped_cells = 0;
            for (prio_idx = 0; prio_idx < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); prio_idx++)
            {
                dropped_cells += fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_dropped_nlr_cells[prio_idx];
            }
            format_uint64_decimal(buf_val, dropped_cells, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("FIFO Discard:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_fifo_discard, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL(4-7) MAX Cells in FIFOs DCH(4-7) Unreach Discard
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_max_fifo_cells_nlr);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("Unreach Discard:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_unreach_discard, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL(4-7) Total Outgoing Cells DCH(4-7) MAX Cells in FIFO
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_max_fifo_cells);
        }

        /*
         * DTL(4-7) - DCH(4-7) Total Incoming Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_total_in, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * Third Stage } 
         */
        PRT_COMMITX;

        LOG_CLI(("%s||%s         %s/\\ \n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%s||%s   FE2   %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%s\\/%s         %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
* } DNXF DIAG printout functions
*/

/*
 * LOCAL FUNCTIONs:
 * }
 */
