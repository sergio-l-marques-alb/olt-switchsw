/*
 * $Id: ser.c,v 1.0 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_MEM

#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/mem.h>
#include <soc/chip.h>
#if defined(BCM_DPP_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#include <bcm_int/dpp/counters.h>
#endif /* defined(BCM_DPP_SUPPORT) || defined (BCM_DFE_SUPPORT) */

#include <soc/dpp/ARAD/arad_interrupts.h>

#include "testlist.h"

extern int bcm_common_linkscan_enable_set(int,int);

#if defined(BCM_ARAD_SUPPORT) || defined(BCM_DFE_SUPPORT)

#define MAX_NOF_ATTAMPTS_TO_GET_A_RW_BIT 100
#define BIT_IN_UINT32 (sizeof(uint32)*8)
#define MAX_SER_RETRIES 10

typedef enum {
    ECC1 = 0,
    ECC2
} error_type;

/* ser_test_params */
typedef struct ser_test_params_s {
    int unit;
    char* mem_name_parse;           /* memory name to parse */
    soc_mem_t mem;                  /* memory to test*/
    char* index_parse;              /* index to parse */
    int index;                      /* index to test */
    char* array_index_parse;        /* array index to parse */
    int array_index;                /* array index to test */
    int copyno;                     /* block number to test */
    int cache_state;                /* use cache state test */
    arad_interrupt_type interrupt;  /* interrupt to check result */
    error_type error;               /* error type to generate (ECC1, ECC2)*/
    uint8 run_all;                  /* if set we run the tr for each of the chip memories,otherwise for a specific memory as given to mem_name_parse*/
    uint32 start_from;              /* strat from id relevant if run_all set */
    uint32 count;              /* how many memories to run on */
    uint32 total_counter;            /* total number of memories  run the test*/
    uint32 skipped_counter;           /* total number of memories  skipped the test*/
    uint32 unjustified_skipped_counter; /* total number of memories  unjustified skipped the test and need to be check individually*/
    uint32 error_counter;            /* total number of memories  failed the test*/
    int help;                       /* show usage */
}ser_test_params_t;


static ser_test_params_t *ser_parameters[SOC_MAX_NUM_DEVICES];
static dcmn_block_control_info_t ser_test_blocks_control_info[SOC_MAX_NUM_DEVICES][NOF_DNX_BLOCKS];
void update_mem_int(int unit)
{
    ser_test_params_t *ser_test_params = ser_parameters[unit];
    soc_block_type_t block;
    soc_mem_t mem = ser_test_params->mem;

    block = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)) - SOC_BLK_FIRST_DNX;
    ser_test_params->interrupt = ser_test_blocks_control_info[unit][block].parity_int;
    if (SOC_MEM_FIELD_VALID(unit,mem,ECCf)) {
         ser_test_params->interrupt = ser_test_params->error ==ECC1  ? ser_test_blocks_control_info[unit][block].ecc1_int : ser_test_blocks_control_info[unit][block].ecc2_int;
    }


}

/*
 * Function:
 *      ser_test_get_block_num_unsafe
 * Purpose:
 *      finds a given memory block number after providing its copy number
 * Parameters:
 *      unit    - Device Number
 *      mem     - Memory to find its block number 
 *      copyno  - block copy number to look for
 * Returns:
 *      on success - block num
 *      on failure - -1
 */
int ser_test_get_block_num_unsafe(int unit, soc_mem_t mem, int copyno)
{
    int block_iter, min_block, max_block;

    /* Define range of blocks to iterate over */
    min_block = SOC_MEM_INFO(unit, mem).minblock;
    max_block = SOC_MEM_INFO(unit, mem).maxblock;

    /* Iterate over range and look for relevant copyno, and return the actual block number */
    for (block_iter = min_block; block_iter <= max_block; ++block_iter) 
    {
        if (SOC_BLOCK_INFO(unit, block_iter).number == copyno) 
        {
            return block_iter ;
        }
    }

    /* relevant copy of blk was not found */
    return -1;
}

char tr153_test_usage[] = 
"TR 153 memory ser test:\n"
" \n"
  "Memory=<value>                  -  Specifies a memory name to run test on - default is \"\"\n" 
  "Index=<value>                   -  Specifies index in memory to run test on - default is min index\n" 
  "ArrayIndex=<value>              -  Specifies array index in memory to run test on - default is min array index\n" 
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pedantic compiler\n";
#else
  
  "CopyNo=<file name>              -  Specifies copy number of memory (for memory which exists in several blocks) - default is 0 \n" 
  "CacheState=<1/0>                -  Specifies if need to validate the Ser fix from the cache or that the interrupt appearance is sufficant - default is 0 \n"
  "ErrorType=<1/0>                 -  Specifies if Error type is <0> ECC 1bit or <1> ECC 2bit - if memory is protected by PARITY, this argument is ignored - default is 0\n"
  "RunAll=<1/0>                    -  Specifies if the test run on specific memory<0> Or perform for each memory<1> \n"
  "StartFrom=id                    -  Specifies mem id to start from  relevant if RunAll set \n"
  "MemCount=id                    -   Specifies how many memories to run on \n"
  "Help=<1/0>                      -  Specifies if tr 153 help is on and exit or off - default is off\n"
  "\n"
;
#endif



/*
 * Function:
 *      memory_ser_test_init
 * Purpose:
 *      takes care of all of init process for the ser test, get arguments and make a perliminery sanity check for given args
 * Parameters:
 *      unit    - Device Number
 *      a       - count of arguments 
 *      p       - actual arguments
 * Returns:
 *      BCM_E_XXX
 */
extern int memory_ser_test_init(int unit, args_t *a, void **p)
{
    ser_test_params_t *ser_test_params = NULL;
    int num_soc_mem = NUM_SOC_MEM;
    parse_table_t parse_table;
    int rv = BCM_E_INIT;
    dcmn_block_control_info_t config;

    /** allocate memory for DB for test parameters **/
    ser_test_params = ser_parameters[unit];
    if (ser_test_params == NULL) 
    {
        ser_test_params = sal_alloc(sizeof(ser_test_params_t), "ser_test");
        if (ser_test_params == NULL) 
        {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s: cannot allocate memory test data\n"), ARG_CMD(a)));
            return -1;
        }
        sal_memset(ser_test_params, 0, sizeof(ser_test_params_t));
        ser_parameters[unit] = ser_test_params;
    }
    ser_test_params->start_from = 0;
    ser_test_params->count = NUM_SOC_MEM;

    /** seting default values **/
    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table,  "Memory",        PQ_STRING, "",       &(ser_test_params->mem_name_parse),    NULL);
    parse_table_add(&parse_table,  "Index",         PQ_STRING, "min",    &(ser_test_params->index_parse),       NULL);
    parse_table_add(&parse_table,  "ArrayIndex",    PQ_STRING, "min",    &(ser_test_params->array_index_parse), NULL);
    parse_table_add(&parse_table,  "CopyNo",        PQ_INT,    0,        &(ser_test_params->copyno),            NULL);
    parse_table_add(&parse_table,  "CacheState",    PQ_BOOL,   0,        &(ser_test_params->cache_state),       NULL);
    parse_table_add(&parse_table,  "ErrorType",     PQ_INT,    0,        &(ser_test_params->error),             NULL);
    parse_table_add(&parse_table,  "StartFrom",     PQ_INT,    0,        &(ser_test_params->start_from),        NULL);
    parse_table_add(&parse_table,  "MemCount",      PQ_DFL|PQ_INT,  &num_soc_mem,        &(ser_test_params->count),             NULL);
    parse_table_add(&parse_table,  "RunAll",        PQ_INT8,    0,        &(ser_test_params->run_all),           NULL);
    parse_table_add(&parse_table,  "Help",          PQ_INT,    0,        &(ser_test_params->help),              NULL);

    ser_test_params->unit = unit;

    /** print usage and exit **/ 
    if (ser_test_params->help) 
    {
        /* print usage */
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s"), tr153_test_usage));
        goto done;
    }


    config.gmo_reg = SOC_IS_ARADPLUS_AND_BELOW(unit) ? ECI_GLOBALFr : INVALIDr;
    dcmn_collect_blocks_control_info(unit,ser_test_blocks_control_info[unit],&config);
    /** parsing arguments and checking seting needed values, checking validity of given options **/
    if (parse_arg_eq(a, &parse_table) < 0) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s: Invalid option: %s\n"), ARG_CMD(a), ARG_CUR(a)));
        /* print usage */
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s"), tr153_test_usage));
        goto done;
    }

    /** making sure no extra options were given **/ 
    if (ARG_CNT(a) != 0) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s: extra options starting with \"%s\"\n"), ARG_CMD(a), ARG_CUR(a)));
        /* print usage */
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s"), tr153_test_usage));
        goto done;
    }

    if (!ser_test_params->run_all) {
        /** validating arguments values **/
        /* validate memory name and store mem enumerator in ser_test_params->mem */
        if (parse_memory_name(unit, &(ser_test_params->mem), ser_test_params->mem_name_parse, NULL, NULL) < 0) 
        {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"Memory \"%s\" is invalid\n"), ser_test_params->mem_name_parse));
            goto done;
        }

        if (!SOC_MEM_FIELD_VALID(unit,ser_test_params->mem,ECCf) &&
            !SOC_MEM_FIELD_VALID(unit,ser_test_params->mem,PARITYf)) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"Memory \"%s\" must contain PARITY or ECC fields\n"), ser_test_params->mem_name_parse));
            goto done;
        }
        /* validate memory index and store index in ser_test_params->index */
        ser_test_params->index = parse_memory_index(unit, ser_test_params->mem, ser_test_params->index_parse);
        if ( (ser_test_params->index < parse_memory_index(unit, ser_test_params->mem, "min")) || 
             (ser_test_params->index > parse_memory_index(unit, ser_test_params->mem, "max"))    ) 
        {

            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"index %d is invalid for memory \"%s\"\n"), ser_test_params->index, ser_test_params->mem_name_parse));
            goto done;
        }

        /* validate memory array index and store array index in ser_test_params->array_index */
        ser_test_params->array_index = parse_memory_array_index(unit, ser_test_params->mem, ser_test_params->array_index_parse);
        if ( (ser_test_params->array_index < parse_memory_array_index(unit, ser_test_params->mem, "min")) || 
             (ser_test_params->array_index > parse_memory_array_index(unit, ser_test_params->mem, "max"))    ) 
        {

            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"array index %d is invalid for memory \"%s\"\n"), ser_test_params->index, ser_test_params->mem_name_parse));
            goto done;
        }

        /* validate memory copy number */
        if (!SOC_MEM_BLOCK_VALID(unit, ser_test_params->mem, ser_test_params->copyno)) 
        {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"Copy Number %d is invalid for memory \"%s\"\n"), ser_test_params->copyno, ser_test_params->mem_name_parse));
            goto done;
        }


    }
    /* validate error type is acceptable */
    if ((ser_test_params->error != ECC1) && (ser_test_params->error != ECC2)) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"error type number %d is invalid\n"), ser_test_params->error ));
        goto done;
    }
    update_mem_int(unit);
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"choosing Interrupt number %d \n"), ser_test_params->interrupt ));
    /** Turn off other Threads **/
    bcm_common_linkscan_enable_set(unit,0);
    soc_counter_stop(unit);
    if (SOC_IS_ARAD(unit)) {
        rv = bcm_dpp_counter_bg_enable_set(unit, FALSE);
        if (BCM_E_NONE == rv)
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"counter processor background accesses suspended\n")));
        }
        else
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"counter processor background access suspend failed: %d (%s)\n"),rv, _SHR_ERRMSG(rv)));
        }
    }

    rv = BCM_E_NONE;

done:
    parse_arg_eq_done(&parse_table);
    if (rv != BCM_E_NONE) 
    {
        test_error(unit, "There was a problem with parameters, they were not entered correctly\n");
    }
    return rv; 
}

static  soc_field_t fields[] = {ECCf,PARITYf,NUM_SOC_FIELD};

int mem_ser_test_cb(int unit, soc_mem_t mem,void *p)
{
    soc_error_t rv = BCM_E_NONE;
    ser_test_params_t *ser_test_params = ser_parameters[unit];
    uint32 min_index,max_index;
    uint64 index_addition;
    uint64 sal_rand_max = COMPILER_64_INIT(0, SAL_RAND_MAX);
 


    

    /* we skip on memory if it doesnt contain ECC  and is not cacheble*/
    if (!dcmn_mem_contain_one_of_the_fields(unit,mem,fields) || 
        dcmn_tbl_is_dynamic(unit,mem) ||
        mem < ser_test_params->start_from ||
        mem > ser_test_params->start_from  + ser_test_params->count ||
        SOC_MEM_ALIAS_MAP(unit,mem) != INVALIDm ||
        SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem))==SOC_BLK_SCH ||
        (SOC_MEM_FIELD_VALID(unit,mem,ECCf) && soc_mem_field_length(unit,mem,ECCf)<=1)||
        (SOC_MEM_FIELD_VALID(unit,mem,PARITYf) && soc_mem_field_length(unit,mem,PARITYf)!=1)||
        (!soc_mem_cache_get(unit,mem,SOC_BLOCK_ALL) && (!SOC_MEM_FIELD_VALID(unit,mem,ECCf) || 
        (SOC_MEM_ECC_TYPE(unit,mem) != SOC_MEM_ECC_TYPE_NORMAL && SOC_MEM_ECC_TYPE(unit,mem) != SOC_MEM_ECC_TYPE_WIDE) ||
                                                        SOC_MEM_TYPE(unit,mem) == SOC_MEM_TYPE_XOR)) ) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
        ser_test_params->skipped_counter++;       
        return rv;
    }


    if (SOC_IS_JERICHO(unit)) {
        switch (mem) {
        case  EDB_EEDB_BANKm:
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
            ser_test_params->skipped_counter++;       
            ser_test_params->unjustified_skipped_counter++;       
            return rv;
        }
    }
    ser_test_params->total_counter++;
    ser_test_params->mem = mem;
    min_index =  parse_memory_index(unit, mem, "min");
    max_index =  parse_memory_index(unit, mem, "max");
    COMPILER_64_SET(index_addition, 0, max_index - min_index);
    COMPILER_64_UMUL_32(index_addition, sal_rand());
    COMPILER_64_UDIV_64(index_addition, sal_rand_max);
    ser_test_params->index = min_index + COMPILER_64_LO(index_addition);

    min_index = parse_memory_array_index(unit, mem, "min");
    max_index = parse_memory_array_index(unit, mem, "max");
    COMPILER_64_SET(index_addition, 0, max_index - min_index);
    COMPILER_64_UMUL_32(index_addition, sal_rand());
    COMPILER_64_UDIV_64(index_addition, sal_rand_max);
    ser_test_params->index = min_index + COMPILER_64_LO(index_addition);

    ser_test_params->copyno = 0;
    if (SOC_MEM_FIELD_VALID(unit,mem,ECCf)) {
         ser_test_params->error = (!soc_mem_cache_get(unit,mem,SOC_BLOCK_ALL)) ? ECC1 : ECC2;
    }
    update_mem_int(unit);
    rv = memory_ser_test_run_mem(unit,NULL,NULL);
     

    return rv;

}


/*
 * Function:
 *      memory_ser_test_run
 * Purpose:
 *      run the test, ser test should simulate ser error and check its handling
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int memory_ser_test_run(int unit, args_t *a, void *p)
{

    soc_error_t rv = BCM_E_NONE;
    ser_test_params_t *ser_test_params = NULL;

    ser_test_params = ser_parameters[unit];
    if (!ser_test_params->run_all) {
        return memory_ser_test_run_mem(unit,a,p);
    }
    sal_srand(sal_time());
    ser_test_params->total_counter=0;
    ser_test_params->error_counter=0;
    ser_test_params->skipped_counter=0;
    ser_test_params->unjustified_skipped_counter=0;
    if (soc_mem_iterate(unit, mem_ser_test_cb, &unit) < 0)
        LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "SER_MEM_TEST: unit %d  failed\n"), unit));
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Mem run(%d) failed(%d) skipped(%d) skipped need further investigation(%d) \n"), 
                                ser_test_params->total_counter,
                                ser_test_params->error_counter,
                                ser_test_params->skipped_counter,
                                ser_test_params->unjustified_skipped_counter
                                ));

    return rv;
}

/*
 * Function:
 *      memory_ser_test_run
 * Purpose:
 *      run the test, ser test should simulate ser error and check its handling
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      BCM_E_XXX
 */
extern int memory_ser_test_run_mem(int unit, args_t *a, void *p)
{
    uint32 mem_buff[SOC_MAX_MEM_WORDS], mask[SOC_MAX_MEM_WORDS], mem_buff_restored_from_cache[SOC_MAX_MEM_WORDS],orig_mem_buff[SOC_MAX_MEM_WORDS];
    uint32 interrupt_count, new_interrupt_count, sleep_for_a_bit = 2000000;
    int rv = BCM_E_NONE, test_failed = 0, entry_length;
    int nof_needed_bits, nof_bits_found = 0, random_bit, attampt_counter, toggle_counter, block_num;
    int bits_found[] = {-1, -1};
    ser_test_params_t *ser_test_params = NULL;
    soc_mem_t mem = 0;
    bcm_switch_event_control_t get_interrupt_count;
    uint8 succeed=0;
    uint32 i;
    soc_block_t block ;
    soc_reg_t   gmo_reg ;
    bcm_switch_event_control_t type;
    uint32 int_mask;

    SOC_INIT_FUNC_DEFS;

    /** Check Validity of Test Parameters **/ 
    ser_test_params = ser_parameters[unit];
    if (ser_test_params == NULL) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Invalid test params\n")));
        test_failed = 1;
        rv = BCM_E_UNAVAIL;
        goto done;
    }



    /** Print Test Parameters - Temp chunk, need to be removed once test is completed **/
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Recieved Arguments are:\n" 
                                                  "Memory name : %s\t"             "Memory num  : %d\t"          "Index       : %d\t"        "Array Index : %d\n"
                                                  "CopyNo      : %d\t"             "Cache State : %d\t"          "Interrupt   : %d\t"        "Error Type  : %d\n"),
                                         SOC_MEM_NAME(unit, ser_test_params->mem), ser_test_params->mem,         ser_test_params->index,     ser_test_params->array_index,
                                                  ser_test_params->copyno,         ser_test_params->cache_state, ser_test_params->interrupt, ser_test_params->error));


    type.event_id=ser_test_params->interrupt;
    type.action=bcmSwitchEventMask;
    type.index=0;
    bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,type,&int_mask);
    if (int_mask) {

        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Interrupt %d Masked sow skipping the test for Mem:%s\n"), ser_test_params->interrupt,SOC_MEM_NAME(unit, ser_test_params->mem))); 
        test_failed = 1;
        rv = BCM_E_PARAM;
        goto done;
    }

    /** Get Parameters and Preperations **/
    sal_memset(mask, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(mem_buff, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(mem_buff_restored_from_cache, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    mem = ser_test_params->mem;

    block = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)) - SOC_BLK_FIRST_DNX;
    gmo_reg = ser_test_blocks_control_info[unit][block].gmo_reg;

    if (!SOC_REG_IS_VALID(unit,gmo_reg) || SOC_REG_ARRAY(unit,gmo_reg) || !SOC_REG_FIELD_VALID(unit,gmo_reg,CPU_BYPASS_ECC_PARf)) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s block doesnt have general configuration register\n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }
    nof_needed_bits = ser_test_params->error == ECC2 ? 2 : 1;
    block_num = ser_test_get_block_num_unsafe(unit, mem, ser_test_params->copyno);
    sal_srand(sal_time());

    /** enable dynamic **/
    enable_dynamic_memories_access(unit);

    /** Check Memory is not W/O or R/O **/
    if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s Write Only memory - Invalid memory\n"), SOC_MEM_NAME(unit, mem)));
        test_failed = 1;
        rv = BCM_E_PARAM;
        goto done;
    }

    if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s Read Only memory - Invalid memory\n"), SOC_MEM_NAME(unit, mem)));
        test_failed = 1;
        rv = BCM_E_PARAM;
        goto done;
    }

    /** get given interrupt's count **/
    get_interrupt_count.event_id = ser_test_params->interrupt;
    get_interrupt_count.index = ser_test_params->copyno;
    get_interrupt_count.action = bcmSwitchEventStat;
    bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, get_interrupt_count, &interrupt_count);

    /** Get Bit/s to Toggle **/
    entry_length = soc_mem_entry_bits(unit, mem);
    if (entry_length == 0) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s entry size is 0, invalid size\n"), SOC_MEM_NAME(unit, mem)));
        test_failed = 1;
        rv = BCM_E_FAIL;
        goto done;
    }
    soc_mem_datamask_rw_get(unit, mem, mask); 
    /* get random bit, check if not r/o or w/o, and save it, repeat untill got all needed bits, or timed out. */
    for ( attampt_counter = 0; (attampt_counter < MAX_NOF_ATTAMPTS_TO_GET_A_RW_BIT) && (nof_needed_bits > nof_bits_found); ++attampt_counter) {
        random_bit = sal_rand() % entry_length ;
        /* check if bit is r/w */
        if ( mask[random_bit / BIT_IN_UINT32] & (1u << random_bit % BIT_IN_UINT32) ) {
            /* Save found Bit */
            bits_found[nof_bits_found++] = random_bit;
            /* mark bit as not r/w - inorder not to get the same one if more then 1 is needed */
            mask[random_bit / BIT_IN_UINT32] ^= (1u << random_bit % BIT_IN_UINT32);
        } else {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "bit %d/%d"               "of Memory %s is not r/w bit and is being skipped, skip count = %d\n"), 
                                                      random_bit, entry_length - 1, SOC_MEM_NAME(unit, mem),                           attampt_counter ));
        }
    }

    /** check if loop timed-out **/
    if (nof_needed_bits != nof_bits_found) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "attampt to get bits to change in Memory %s timed-out\n"), SOC_MEM_NAME(unit, mem)));
        test_failed = 1;
        rv = BCM_E_TIMEOUT;
        goto done;
    }


    /** Change Entry not in cache **/
    /* Read entry */
    _SOC_IF_ERR_EXIT(soc_mem_read_no_cache(unit, mem, block_num, ser_test_params->index, mem_buff));

    /* change requiered bits */
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "change bits at pos:")));
    sal_memcpy(orig_mem_buff,mem_buff,sizeof(orig_mem_buff));
    for (toggle_counter = 0; toggle_counter < nof_bits_found; ++toggle_counter) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "(%d) "), bits_found[toggle_counter]));
        mem_buff[bits_found[toggle_counter]/BIT_IN_UINT32] ^= (1u << bits_found[toggle_counter]%BIT_IN_UINT32);
    }
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "\n")));

    /** Bypass PARITY and ECC **/
    _SOC_IF_ERR_EXIT(soc_reg_field32_modify(unit, gmo_reg, REG_PORT_ANY, CPU_BYPASS_ECC_PARf, 0x1));

    /** Write modified entry **/ 
    _SOC_IF_ERR_EXIT(soc_mem_write_extended(unit, SOC_MEM_DONT_USE_CACHE, mem, block_num, ser_test_params->index, mem_buff));

    /** Re-activate PARITY and ECC and Read memory --> Should Trigger the Interrupt **/
    _SOC_IF_ERR_EXIT(soc_reg_field32_modify(unit, gmo_reg, REG_PORT_ANY, CPU_BYPASS_ECC_PARf, 0x0)); 
    _SOC_IF_ERR_EXIT(soc_mem_read_no_cache(unit, mem, block_num, ser_test_params->index, mem_buff_restored_from_cache));
     sal_usleep(sleep_for_a_bit);

    /** Check again given interrupt's count **/
    bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, get_interrupt_count, &new_interrupt_count);
    for (i=0;i<MAX_SER_RETRIES;i++) {
        if (new_interrupt_count <= interrupt_count) {
            /* if the counter test failed lets try read test maybe the nmemory already fixed*/
                sal_usleep(sleep_for_a_bit);
                cli_out("retry reading counter  %d\n",i);
                bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, get_interrupt_count, &new_interrupt_count);
                continue;
        }
        succeed=1;
        break;

        }    

    if (!succeed) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s Interrupt doesnt trigger(counter=%d) check to see if the interrupt number %d supplied  correct\n"), SOC_MEM_NAME(unit, mem), new_interrupt_count, ser_test_params->interrupt));
        test_failed = 1;
        rv = BCM_E_FAIL;
        goto done;
    }

    if (ser_test_params->error == ECC1  && SOC_MEM_FIELD_VALID(unit, mem, ECCf)) {
        /* in case of ecc1 test if our correction by shadow or by ecc1 correction succeed. we compare the corrected data with the original data*/
        _SOC_IF_ERR_EXIT(soc_mem_read_no_cache(unit, mem, block_num, ser_test_params->index, mem_buff_restored_from_cache));
        if (sal_memcmp(mem_buff_restored_from_cache, orig_mem_buff, sizeof(orig_mem_buff))) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Warning:Memory %s value doesnt corrected to it original value \n"), SOC_MEM_NAME(unit, mem)));
            test_failed = 0;
            rv = BCM_E_NONE;
            goto done;
        }




    }
done:

    if (test_failed) {
        test_error(unit, "Memory Ser Test Failed!\n");
        if(ser_test_params) {
            ser_test_params->error_counter++;
        }
    }
    return rv;

exit:
    ser_test_params->error_counter++;  
    test_error(unit, "Memory Ser Test Failed!\n");
    SOC_FUNC_RETURN;
}




/*
 * Function:
 *      memory_ser_test_done
 * Purpose:
 *      clean-up after ser test was done
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      BCM_E_XXX
 */
extern int memory_ser_test_done(int unit, void *p)
{
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Ser Tests Done\n")));
    sal_free(ser_parameters[unit]);
    ser_parameters[unit] = NULL;
    return 0;
}

#endif /* BCM_ARAD_SUPPORT || BCM_DFE_SUPPORT*/

#undef _ERR_MSG_MODULE_NAME

