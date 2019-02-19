/*
 * $Id: init.c,v 1.36 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * Built-in init related tests
 */

#if defined(BCM_WARM_BOOT_API_TEST)
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>    
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#include <appl/diag/system.h> /* for parse_small_integers */
#include <ibde.h>

#include <soc/debug.h>
#include <soc/cmext.h>
#include <soc/wb_engine.h>
#include <shared/bsl.h>
#include <bcm/stack.h>
#include <soc/drv.h>
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/port_sw_db.h>
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/drv.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
#endif /* BCM_DPP_SUPPORT */
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/link.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm/switch.h>
#ifdef BCM_DPP_SUPPORT
#include <bcm_int/dpp/switch.h>
#endif /* BCM_DPP_SUPPORT */
#endif /*BCM_WARM_BOOT_SUPPORT*/

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/shell.h>
#include <appl/diag/sysconf.h>
#include <appl/diag/dpp/sw_state_dump.h>

#if defined(BCM_ARAD_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <appl/diag/dcmn/init.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#include <appl/dcmn/rx_los/rx_los.h>

#include <soc/dcmn/dcmn_wb.h>
#endif

#include <appl/diag/dcmn/init_deinit.h>

#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING
  #define AGGRESSIVE_ALLOC_DEBUG_TESTING 0
#endif

#if defined(BCM_ARAD_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <appl/diag/dpp/kbp.h>
#endif /* defined(BCM_ARAD_SUPPORT) && defined(INCLUDE_KBP) && !defined(BCM_88030) */


#if defined(BCM_ARAD_SUPPORT) || defined(BCM_DFE_SUPPORT)
#define DEINIT_IF_ERR_EXIT(_pi) \
    if (_pi == 0) { \
        goto done; \
    } \

#define INIT_IF_ERR_EXIT(_pi) \
    if (_pi) { \
        continue; \
    } else { \
        goto done; \
    } \


char warmboot_test_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
"Warmboot Test Usage:\n"
"  Repeat=<value>, NoInit=<value>, NoDeinit=<value>, NoBcm=<value>, NoSoc=<value>.\n"
"  NoInt=<value>, NoRxLos=<value>, NoAppl=<value>         .\n"
"  1: The test will not run.\n"
#else
"Warmboot Test Usage:\n"
"  Repeat=<value>         the test will loop as many times as stated. (default=1)\n"
"  NoSync=<value>         1: The test will not perform a sync to persistant storage (to be used with autosync mode).\n"
"  NoInit=<value>         1: The test will not Init the Chip. \n"
"  NoDeinit=<value>       1: The test will not deinit the Chip.\n"
"  NoBcm=<value>          1: The test will not run on BCM.\n"
"  NoSoc=<value>          1: The test will not run on Soc.\n"
"  NoAttach=<value>       1: The test will not do device attech detach, set to 1 by default.\n"
"  NoInt=<value>          1: The test will not run on Interrupts.\n"
"  NoRxLos=<value>        1: The test will not run on RX LOS App.\n"
"  NoLinkscan=<value>     1: The test will not run on Linkscan.\n"
"  NoAppl=<value>         1: The test will not run on Application.\n"
#endif
;

#if AGGRESSIVE_ALLOC_DEBUG_TESTING
typedef struct {
    size_t addr;
    int size;
    void* stack[3];
    CONST char *desc;
} alloc_info;

int sal_alloc_debug_nof_allocs_get(void);
void sal_alloc_debug_last_allocs_get(alloc_info *array, int nof_last_allocs);
#define UNFREE_ALLOCS_FILE "unfree_allocs.txt"
#endif


STATIC appl_dcmn_init_param_t init_deinit[SOC_MAX_NUM_DEVICES];


#if AGGRESSIVE_ALLOC_DEBUG_TESTING
STATIC int init_aggressive_alloc_debug_testing(int itertaion_idx)
{
    static int32 after_deinit_nof_allocs = 0;
    int32 after_deinit_init_deinit_nof_allocs;
    static int32 after_deinit_dma_usgae = 0;
    int32 after_deinit_init_deinit_dma_usage;
    alloc_info *unfree_allocs;
    FILE *unfree_allocs_file;
    int rv = 0;
    uint32 allocation_idx;
    extern int _dma_get_usage(void); /* _dma_get_usage is implemented in linux-user-bde.c, which has no .h file. */

    if(itertaion_idx == 0) {     
        after_deinit_dma_usgae = _dma_get_usage();
        after_deinit_nof_allocs = sal_alloc_debug_nof_allocs_get();
    }

    if(itertaion_idx == 1) {
        after_deinit_init_deinit_dma_usage = _dma_get_usage();
        if(after_deinit_init_deinit_dma_usage > after_deinit_dma_usgae) {
            rv = -1;
            cli_out("Error: There are dma allocations that haven't been freed.\n");
            /* we don't return. We continue to check if there are unfree allocations done be sal_alloc */
        }
        after_deinit_init_deinit_nof_allocs = sal_alloc_debug_nof_allocs_get();

        if(after_deinit_init_deinit_nof_allocs > after_deinit_nof_allocs) {
            cli_out("The are unfree allocations done by sal_alloc.\n");
            rv = -1;
            unfree_allocs_file = sal_fopen(UNFREE_ALLOCS_FILE, "w");
            if(!unfree_allocs_file) {
                cli_out("Filed to open file %s.\n", UNFREE_ALLOCS_FILE);
                return rv;
            }
            unfree_allocs = sal_alloc(sizeof(alloc_info)*(after_deinit_init_deinit_nof_allocs - after_deinit_nof_allocs + 1), "");/* include this allocation*/
            if(!unfree_allocs) {
                cli_out("Memory allocation Failure.\n\r");
                return rv;
            }
            sal_alloc_debug_last_allocs_get(unfree_allocs, after_deinit_init_deinit_nof_allocs - after_deinit_nof_allocs + 1);
            for(allocation_idx = 0; allocation_idx < after_deinit_init_deinit_nof_allocs - after_deinit_nof_allocs; ++allocation_idx) {
                sal_fprintf(unfree_allocs_file, "allocation 0x%lx %d  from %p -> %p -> %p Description : %s\n",
                       (unsigned long)unfree_allocs[allocation_idx].addr, unfree_allocs[allocation_idx].size,
                       unfree_allocs[allocation_idx].stack[2],
                       unfree_allocs[allocation_idx].stack[1],
                       unfree_allocs[allocation_idx].stack[0],
                       unfree_allocs[allocation_idx].desc);
            }
            sal_free(unfree_allocs);
            sal_fclose(unfree_allocs_file);
            return rv;
        } else {
            return rv;
        }
    }

    return rv;
}
#endif


#ifndef NO_FILEIO
/* 
 * function to compare two files, 
 * returns BCM_E_NONE if the files are the same. 
*/ 
int 
diff_two_files(char *file_name1, char *file_name2)
{
    FILE *file1;
    FILE *file2;
    char str1[256];
    char str2[256];
    char *is_null1;
    char *is_null2;
    char diff_cmd[256];

    int rv = BCM_E_NONE;

    if ((file1 = sal_fopen(file_name1, "r")) == 0) {
        cli_out("Error opening sw dump file %s\n", file_name1);
        return BCM_E_INTERNAL;
    }
    if ((file2 = sal_fopen(file_name2, "r")) == 0) {
        sal_fclose(file1);
        cli_out("Error opening sw dump file %s\n", file_name2);
        return BCM_E_INTERNAL;
    }

    while (TRUE) {
        is_null1 = fgets(str1, 256, file1);
        is_null2 = fgets(str2, 256, file2);
        if ((is_null1 == NULL) || (is_null2 == NULL)) {
            break;
        }
        if(sal_strcmp(str1,str2) != 0) {
            rv = BCM_E_INTERNAL;
            cli_out("sw state has changed after warm reboot. to see the difference compare file %s with %s\n", file_name1, file_name2);
            cli_out("printing the first difference:\nbefore: %s\nafter: %s\n", str1, str2);
            goto exit;
        }
    }

    /* if only one file has reached EOF */
    if (is_null1 != is_null2){
            rv = BCM_E_INTERNAL;
            cli_out("sw state has changed after warm reboot. file %s and file %s have different length\n", file_name1, file_name2);
            goto exit;
    }

exit:
    sal_fclose(file1);
    sal_fclose(file2);

    if (rv != BCM_E_NONE) {
        /*build diff command*/
        sal_strncpy(diff_cmd,  "diff ", sizeof(diff_cmd));
        sal_strncat(diff_cmd, file_name1, sizeof(diff_cmd) - sal_strlen(diff_cmd) - 1);
        sal_strncat(diff_cmd, " ", sizeof(diff_cmd) - sal_strlen(diff_cmd) - 1);
        sal_strncat(diff_cmd, file_name2, sizeof(diff_cmd) - sal_strlen(diff_cmd) - 1);

        cli_out("********* diffing %s with %s **********\n", file_name1, file_name2);
        cli_out("note: using linux diff, diff may fail.\n");
        cli_out("note: when running in gdb diff is not printed to screen.\n");
        cli_out("      if no diff is printed, use breakpoint on this line\n");
        cli_out("      and manually diff the files immediately after the break\n");
        /* diff wont work when running inside gdb */
        system(diff_cmd);
        cli_out("*******************************************************************\n");
    }

#if defined(BCM_WARM_BOOT_API_TEST)
    /* delete the files if in wb test mode*/
    sal_remove(file_name1);
    sal_remove(file_name2);
#endif

    return rv;
}

#endif /*NO_FILEIO*/


unsigned int
do_error_on_leak_test(appl_dcmn_init_param_t *init_param)
{
    if (
        init_param->error_on_leak &&
        !init_param->no_init && 
        !init_param->no_deinit 
        ) {
        return 1;
    }

    return 0;

   
}

/*
 * Function:     init_deinit_test
 * Purpose:    Test Init & Deinit
 * Parameters:    u - unit #.
 *        a - pointer to arguments.
 *        p - ignored cookie.
 * Returns:    0
 */
int
init_deinit_test(int unit, args_t *a, void *p)
{
    int rv = 0x0;
    int i = 0;
    int r = 1;
    int partial_init = 0;
    int partial_deinit = 0;
#if (defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
    int j = 0;
    int compare = 0;
    int loop = 1;
#endif

    char file_name1[SOC_PROPERTY_NAME_MAX + 256];
    char file_name2[SOC_PROPERTY_NAME_MAX + 256];

#if defined(BCM_WARM_BOOT_API_TEST)
    int s;
    struct ifreq buffer;
    char mac_str[18];
#endif
    appl_dcmn_init_param_t *init_param = p;
    char    *stable_filename = NULL;
    unsigned long alloc_bytes_count_on_start=0,free_bytes_count_on_start=0;

    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(p);

    file_name1[0] = '\0';
    file_name2[0] = '\0';

    if (do_error_on_leak_test(init_param)) {
        sal_get_alloc_counters(&(alloc_bytes_count_on_start),&(free_bytes_count_on_start)); 
    }

    stable_filename = soc_property_get_str(unit, spn_STABLE_FILENAME);

    /* prefixing with unique files name to enable more than one parallel run from same SDK folder  */
    /* assuming stable_filename is unique for each separate run */
    if (NULL != stable_filename) {
#ifdef BCM_WARM_BOOT_API_TEST
        /* in WB test mode save it in negev memory instead on stable_filename location on the network HDD */
        sal_strncat(file_name1, "/dev/shm/warmboot_data", sizeof(file_name1) - 1);
        sal_strncat(file_name2, "/dev/shm/warmboot_data", sizeof(file_name2) - 1);
#else
        sal_strncat(file_name1, stable_filename, sizeof(file_name1) - 1);
        sal_strncat(file_name2, stable_filename, sizeof(file_name2) - 1);
#endif
        sal_strncat(file_name1, "_", sizeof(file_name1) - sal_strlen(file_name1) - 1);
        sal_strncat(file_name2, "_", sizeof(file_name2) - sal_strlen(file_name2) - 1);
    }

    sal_strncat(file_name1, "sw_state_pre_reboot.txt", sizeof(file_name1) - sal_strlen(file_name1) - 1);
    sal_strncat(file_name2, "sw_state_post_reboot.txt", sizeof(file_name2) - sal_strlen(file_name2) - 1);

    /*concatanate mac adress to file name if in special wb test mode*/
#if defined(BCM_WARM_BOOT_API_TEST)
    /*get HW Adress*/
    s = socket(PF_INET, SOCK_DGRAM, 0);
    sal_memset(&buffer, 0x00, sizeof(buffer));
    sal_strcpy(buffer.ifr_name, "eth0");
    ioctl(s, SIOCGIFHWADDR, &buffer);
    close(s);
    sal_snprintf(mac_str, sizeof(mac_str), "%02x%02x%02x%02x%02x%02x",
         (unsigned char)buffer.ifr_hwaddr.sa_data[0], (unsigned char)buffer.ifr_hwaddr.sa_data[1], (unsigned char)buffer.ifr_hwaddr.sa_data[2],
         (unsigned char)buffer.ifr_hwaddr.sa_data[3], (unsigned char)buffer.ifr_hwaddr.sa_data[4], (unsigned char)buffer.ifr_hwaddr.sa_data[5]);

    sal_strcat(file_name1, mac_str);
    sal_strcat(file_name2, mac_str);
#endif

    if(r < init_param->repeat)
    {
        r = init_param->repeat;
    }
  
    if ( (init_param->is_resources_check) && (init_param->warmboot) ) { 
        cli_out("Error, Warmboot test mode is not support with Resource-check.\n"); 
        rv = BCM_E_PARAM; 
        goto done; 
    }  

#if AGGRESSIVE_ALLOC_DEBUG_TESTING
    if(init_param->is_resources_check) {
        if(r > 2) {
            cli_out("Error, resources leak check always does two iterations.\n");
        }
        r = 2;
    }
#endif

#if ((defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT))
    /*currently test mode is relevant only for arad*/
    if (SOC_IS_SAND(unit)) {
        if(!init_param->warmboot) {
            /*Disable Warmboot test mode while tr 141 is running not in wb mode*/
            _DCMN_BCM_WARM_BOOT_API_TEST_OVERRIDE_WB_TEST_MODE(unit);
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    for(i = 0; i < r; i++)
    {   
#if (defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)        
        if (init_param->partial_init != 0) {
            GEN_ERR_TEST_SETUP(unit);
    		
            if (init_param->fake_soc_start != 0) {
                loop = ((init_param->fake_soc_range != 0) ? init_param->fake_soc_range : 1);   
                loop = (loop << 1) + 1;
            } else if (init_param->fake_bcm_start != 0) {
                loop = ((init_param->fake_bcm_range != 0) ? init_param->fake_bcm_range : 1); 
                loop = (loop << 1) + 1;
            } else {
                cli_out("Error, partial deinit test must contain the soc or bcm parameter.\n");
                rv = BCM_E_PARAM; 
                goto done;
            }
    	  } else {
    	    loop = 1;
        }
        GEN_ERR_CNT_SET(unit, GEN_ERR_TYPE_SOC, 0);
        GEN_ERR_CNT_SET(unit, GEN_ERR_TYPE_BCM, 0);
        for (j = 0; j < loop; j++) {
            if (init_param->partial_init != 0) {                
                if ((j % 2) == 0) {
                    if ((j + 1) != loop) {                    
                        if (init_param->fake_soc_start != 0) {
                            compare = init_param->fake_soc_start + (j/2);
                            GEN_ERR_CMP_SET(unit, GEN_ERR_TYPE_SOC, compare);                            
                        } else if (init_param->fake_bcm_start != 0) {
                            compare = init_param->fake_bcm_start + (j/2);
                            GEN_ERR_CMP_SET(unit, GEN_ERR_TYPE_BCM, compare);
                        }
                        partial_deinit = 1;
                        partial_init = 1;
                        cli_out("\nInit Deinit Tests - Partial deinit: %d\n", compare);
                        GEN_ERR_TEST_SET(unit, init_param->partial_init);                           
                    } else {
                        partial_deinit = 0;
                        partial_init = 0;
                        cli_out("\nInit Deinit Tests - Partial deinit cleanup\n");
                        GEN_ERR_TEST_SET(unit, GEN_ERR_TEST_CLEANUP);
                    }
                } else {
                    partial_deinit = 1;
                    partial_init = 0;
                    cli_out("\nInit Deinit Tests - Partial deinit check: %d\n", compare);
                    GEN_ERR_TEST_SET(unit, GEN_ERR_TEST_NONE);
                }
            } else {
                partial_deinit = 0;
                partial_init = 0;
                cli_out("\nInit Deinit Tests - Standard deinit\n");
                GEN_ERR_TEST_SET(unit, GEN_ERR_TEST_NONE);
            }
                
            GEN_ERR_RUN_SET(unit, GEN_ERR_TEST_DEINIT);
#endif
            /* unit not attached, directly do the init */
            if (SOC_CONTROL(unit) == NULL) {
                goto init;
            }

#if defined(BCM_ARAD_SUPPORT) || defined(BCM_DFE_SUPPORT)
            if (SOC_IS_ARAD(unit) || SOC_IS_DFE(unit)) {
                if (init_param->modid == -1 && init_param->no_deinit == 1) {
                    init_param->modid = 0;
                    init_param->base_modid = 0;
                }

                if(!init_param->no_init && !init_param->no_deinit && !init_param->no_appl ) {
                    if (init_param->modid == -1) {
                        cli_out("%d: Stk modid get.\n", unit);
                        rv = bcm_stk_modid_get(unit, &init_param->modid); 
                        if (rv < 0){
                            cli_out("Error, in bcm_stk_modid_get:\n");
                            /* Intentional check if partial deinit is 0,goto done*/
                            /* coverity[dead_error_line] */  
                            DEINIT_IF_ERR_EXIT(partial_deinit);
                        }
                    }
                }
            }
#endif

#if defined(BCM_WARM_BOOT_SUPPORT)
            if (init_param->warmboot) {
                if (!init_param->no_sync) {
                    /* sync the software state to external storage*/        
                    cli_out("%d: Sync sw state to persistent storage.\n", unit);

                    rv = bcm_switch_control_set(unit, bcmSwitchControlSync, 1);
                    if (rv < 0) {
                        cli_out("ERROR: in bcm_switch_control_set-bcmSwitchControlSync, failed to sync, rv= %d.\n", rv);
                        if (rv < 0) {
                            if(partial_deinit == 0) {
                                goto done;
                            }
                        }
                    }
                }
#if defined(BCM_DPP_SUPPORT)
                if (SOC_IS_DPP(unit)) {
                    /* perform software dump (1st dump prior to reboot) */
                    cli_out("%d: Dumping static SW State variables to file.\n", unit);
                    sw_state_dump(file_name1);
                    cli_out("%d: Dumping SW State Data Block to file.\n", unit);
                    shr_sw_state_block_dump(unit, file_name1, "a");
                    if (init_param->engine_dump == 1) {
                        cli_out("%d: Dumping WB Engine variables to file.\n", unit);
                        soc_wb_engine_dump(unit, 0, 0, -1, -1, 0, file_name1, "a");
                    }
                    /*sw_state_verbose[unit] = 1;*/
                }
#endif
            }
#endif 
            cli_out("%d: loop=%d (out of %d).\n", unit, i + 1, r);

            if (init_param->warmboot) {
                init_param->no_elk_device = 1;
            }

            if (!init_param->no_deinit) {

                rv = appl_dcmn_deinit(unit, init_param);
                if (rv < 0) {
                    /* Intentional check if partial deinit is 0,goto done*/
                    /* coverity[dead_error_line] */
                    DEINIT_IF_ERR_EXIT(partial_deinit);
                }
            }
    init:
            
#if (defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
             GEN_ERR_RUN_SET(unit, GEN_ERR_TEST_INIT);
#endif


#if AGGRESSIVE_ALLOC_DEBUG_TESTING
            if(init_param->is_resources_check) {
                rv = init_aggressive_alloc_debug_testing(i);
                if (rv < 0) {
                    cli_out("Error: init_aggressive_alloc_debug_testing () Failed:\n");
                }
                if (i == 1) {
                    /* There is no need to init in the second loop */
                    goto done;
                }
            }
#endif
            if (init_param->rc_load) {
                rv = diag_rc_load(unit);
            } else if (!init_param->no_init) {
                rv = appl_dcmn_init(unit, init_param);
                if (rv < 0) {
                   INIT_IF_ERR_EXIT(partial_init);
                }
            }

#if defined(BCM_WARM_BOOT_SUPPORT)
            if(init_param->warmboot) {
#if defined(BCM_DPP_SUPPORT)
                if (SOC_IS_DPP(unit)) {
                    /* perform software dump (2nd dump after reboot) */
                    cli_out("%d: Dumping static SW State variables to file.\n", unit);
                    sw_state_dump(file_name2);
                    cli_out("%d: Dumping SW State Data Block to file.\n", unit);
                    shr_sw_state_block_dump(unit, file_name2, "a");
                    if (init_param->engine_dump == 1) {
                        cli_out("%d: Dumping WB Engine variables to file.\n", unit);
                        soc_wb_engine_dump(unit, 0, 0, -1, -1, 0, file_name2, "a");
                    }
                    /*sw_state_verbose[unit] = 0;*/

                    /*diff the two files*/
#ifndef NO_FILEIO
                    cli_out("%s(): Diffing files %s with %s:\n", FUNCTION_NAME(), file_name1, file_name2);
                    rv = diff_two_files(file_name1, file_name2);
                    if (rv < 0) {
                       goto done;
                    }
#else /*NO_FILEIO*/
                    cli_out("%s(): Can't diff files as NO_FILEIO is defined (you can diff %s with %s externally).\n", FUNCTION_NAME(), file_name1, file_name2);
#endif /*NO_FILEIO*/
                }
#endif /*BCM_DPP_SUPPORT*/
                /* turn off warmboot flag in case not already turned off because of INCLUDE_INTR */
                soc_state[unit] = 0;
            }
#endif
#if (defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_GEN_ERR_MECHANISM)
        }
        if (init_param->partial_init != 0) {
            GEN_ERR_TEST_CLEANUP(unit);
        }
#endif
    }

#if ((defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT))
    if (SOC_IS_SAND(unit)) {
        if(!init_param->warmboot) {
             /*allow wb test when tr 141 finished running*/
            _DCMN_BCM_WARM_BOOT_API_TEST_RETRACT_OVERRIDEN_WB_TEST_MODE(unit);
        }
    }
#endif 

done:

    if (rv < 0) {
        if(init_param->warmboot) {
            test_error(unit, "Warmboot test Failed!!!\n");
        }else {
            test_error(unit, "Deinit-Init test Failed!!!\n");
        }
        return rv;
    }
    else
    {
        if (do_error_on_leak_test(init_param)) {
            unsigned long alloc_bytes_count;
            unsigned long free_bytes_count;
            sal_get_alloc_counters(&alloc_bytes_count,&free_bytes_count); 

            if ((alloc_bytes_count - alloc_bytes_count_on_start) > (free_bytes_count - free_bytes_count_on_start)) {
                cli_out("There is a Mem Leak in tr 141 allocated %lu bytes,  freed %lu bytes, leaked %lu bytes\n",
                        (alloc_bytes_count - alloc_bytes_count_on_start),
                        (free_bytes_count - free_bytes_count_on_start),
                        (alloc_bytes_count - alloc_bytes_count_on_start) - (free_bytes_count - free_bytes_count_on_start)
                        );
                rv = BCM_E_FAIL;
            }

        }

    }

    return rv;
}


int
init_deinit_test_init(int unit, args_t *a, void **p)
{
    appl_dcmn_init_param_t *init_param;
    init_param = &init_deinit[unit];

    sal_memset(init_param, 0x0, sizeof(appl_dcmn_init_param_t));

    /* set default modid & base_modid with non-valid values, which denotes */
    /* that previous values should be taken if those params were not supplied */
    init_param->modid = -1;
    init_param->base_modid = -1;

    *p = init_param;
    return   appl_dcmn_init_usage_parse(unit, a, init_param);

}

int
init_deinit_test_done(int unit, void *p)
{
    cli_out("Init Deinit Tests Done\n");
    return 0;
}



#endif
