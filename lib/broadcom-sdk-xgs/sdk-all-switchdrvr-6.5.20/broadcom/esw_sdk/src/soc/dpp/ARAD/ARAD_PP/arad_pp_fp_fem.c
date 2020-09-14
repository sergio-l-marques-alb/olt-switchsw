#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_fp_fem.c,v 1.66 Broadcom SDK $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * $
*/
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FP 
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_fem.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_FP_FEM_DB_ID_NDX_MAX                             (SOC_PPC_FP_NOF_DBS-1)
#define ARAD_PP_FP_FEM_ACTION_TYPE_MAX                           (SOC_PPC_NOF_FP_ACTION_TYPES_ARAD - 1)
#define ARAD_PP_FP_FEM_DB_STRENGTH_MAX                           (1023)
#define ARAD_PP_FP_FEM_DB_ID_MAX                                 (ARAD_PP_FP_FEM_DB_ID_NDX_MAX)
#define SOC_PPC_FP_FEM_ENTRY_STRENGTH_MAX                        (SOC_SAND_U16_MAX)

#define ARAD_PP_FP_FEM_SAND_U64_NOF_BITS                         (64)
#define ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS                       (4)

#define ARAD_PP_FP_BIT_LOC_LSB_CHANGE_KEY                        (5)
#define ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE                (2 * SOC_DPP_DEFS_GET(unit, tcam_action_width))
#define ARAD_PP_FP_KAPS_RESULT_ACTION_WIDTH                      (32)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  uint32 indx;
  uint32 size;
} action_inf;


typedef enum
{
  /*
   *  Mapping from Actions to buffer.
   */
  ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER = 0,
  /*
    *  Mapping from buffer to Actions.
   */
  ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION = 1,
  /*
   *  Number of types in ARAD_PP_FP_ACTION_BUFFER_DIRECTION
   */
  ARAD_PP_NOF_FP_ACTION_BUFFER_DIRECTIONS = 2
}ARAD_PP_FP_ACTION_BUFFER_DIRECTION;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_fp_fem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_INSERT_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_TAG_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_TAG_ACTION_TYPE_CONVERT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_IS_FEM_BLOCKING_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_DUPLICATE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_CONFIGURE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_CONFIGURATION_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_REMOVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FP_FEM_REORGANIZE),



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_fp_fem[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_PFG_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'pfg_ndx' is out of range. \n\r "
    "The range is: 0 - 4.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_DB_ID_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'db_id_ndx' is out of range. \n\r "
    "The range is: 0 - 127.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'db_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR",
    "The parameter 'db_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR,
    "SOC_PPC_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR",
    "The parameter 'entry_strength' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR,
    "SOC_PPC_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR",
    "The parameter 'entry_id' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static ARAD_PMF_FES_INPUT_INFO invalid_next_FES_info;

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


/*****************************************************************************
* Function:  arad_pp_fp_fem_pgm_per_pmf_pgm_get
* Purpose:  To get FEM program per given PMF program
* Params:
* unit (IN)                - Device Number
* stage (IN)                  -  PMF stage (ingress...)
* pmf_pgm_id (IN)        - PMF program id
* fem_pgm_id  (OUT)      - FEM program for given PMF program, read from SW DB
* Return:    (uint32)  Error in case of error
*******************************************************************************/
uint32
    arad_pp_fp_fem_pgm_per_pmf_pgm_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_id,
    SOC_SAND_OUT uint8                          *fem_pgm_id
    )
{

    uint32
      res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.fem_pgm_per_pmf_prog.get
                                                (unit,stage,pmf_pgm_id,fem_pgm_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    

    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_per_pmf_pgm_get()", 0, pmf_pgm_id);

}

/*****************************************************************************
* Function:  arad_pp_fp_fem_pgm_per_pmf_pgm_set
* Purpose:  To set FEM program per given PMF program
* Params:
* unit (IN)                - Device Number
* stage (IN)                  -  PMF stage (ingress...)
* pmf_pgm_id (IN)        - PMF program id
* fem_pgm_id  (IN)      - FEM program for given PMF program, write to SW DB
* Return:    (uint32)  Error in case of error
*******************************************************************************/
uint32
    arad_pp_fp_fem_pgm_per_pmf_pgm_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_id,
    SOC_SAND_IN  uint8                          fem_pgm_id
    )
{

    uint32
      res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.fem_pgm_per_pmf_prog.set
                                                (unit,stage,pmf_pgm_id,fem_pgm_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    

    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_per_pmf_pgm_set()", 0, pmf_pgm_id);

}


/*****************************************************************************
* Function:  arad_pp_fp_fem_pgm_id_remove
* Purpose:  Dealloc the FEM program for specific PMF program
* Params:
* unit (IN)                - Device Number
* stage (IN)                  -  PMF stage (ingress...)
 * pmf_pgm_id (IN)        - PMF program id to dealloc its fem pgm id
* Return:    (uint32)  Error in case of error
*******************************************************************************/
uint32
    arad_pp_fp_fem_pgm_id_remove(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_id
    )
{
    uint32
      res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);  

    
    res = arad_pp_fp_fem_pgm_per_pmf_pgm_set(unit,stage,pmf_pgm_id,ARAD_PMF_FEM_PGM_INVALID);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_per_pmf_pgm_set()", 0, pmf_pgm_id);  
}

/*****************************************************************************
* Function:  arad_pp_fp_fem_pgm_id_alloc
* Purpose:  In case for certain PMF program FEM program is not set yet, 
                    This function will choose a new FEM program.
                    Count how many PMF programs use FEM programs,
                    Choose the minimal used FEM program
* Params:
* unit (IN)                - Device Number
* stage (IN)                  -  PMF stage (ingress...)
* fem_pgm_id  (OUT)      - FEM program chosen
* Return:    (uint32)  Error in case of error
*******************************************************************************/
uint32
    arad_pp_fp_fem_pgm_id_alloc(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_OUT uint8                          *fem_pgm_id
    )
{
    uint32
      res = SOC_SAND_OK,
      pmf_pgm_id,
      num_of_pgm0 = 0,
      num_of_pgm1 = 0,
      num_of_pgm2 = 0;
    uint8
      fem_pgm_id_temp = ARAD_PMF_FEM_PGM_INVALID;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    for(pmf_pgm_id = 0; pmf_pgm_id < ARAD_PMF_NOF_PROGS; pmf_pgm_id++)
    {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.fem_pgm_per_pmf_prog.get
                                                (unit,stage,pmf_pgm_id,&fem_pgm_id_temp);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        switch(fem_pgm_id_temp)
        {
            case ARAD_PMF_FEM_PGM_FOR_ETH_0:
                num_of_pgm0++;
                break;
            case ARAD_PMF_FEM_PGM_FOR_ETH_1:
                num_of_pgm1++;
                break;
            case ARAD_PMF_FEM_PGM_FOR_ETH_2:
                num_of_pgm2++;
                break;
            default:
                /*In case of invalid or TM program no need to increment*/
                break;
        }
    }

    /*Allocate FEM pgm id by the fewest num of programs that use that fem pgm*/
    if(num_of_pgm0 <= num_of_pgm1 && num_of_pgm0 <= num_of_pgm2)
    {
        *fem_pgm_id = ARAD_PMF_FEM_PGM_FOR_ETH_0;
    }
    else if(num_of_pgm1 <= num_of_pgm0 && num_of_pgm1 <= num_of_pgm2)
    {
        *fem_pgm_id = ARAD_PMF_FEM_PGM_FOR_ETH_1;
    }
    else
    {
        *fem_pgm_id = ARAD_PMF_FEM_PGM_FOR_ETH_2;
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_id_new_get()", 0, pmf_pgm_id);
}

/*****************************************************************************
* Function:  arad_pp_fp_fem_pgm_id_bmp_get
* Purpose:  This function will return the FEM programs that has to be used for the FEM that is chosen for the db_id
                Limitation: Per PMF program there will be only one FEM program
                1) If its a TM database than the FEM program is ARAD_PMF_FEM_PGM_FOR_TM
                2) If its an ETH/not TM database then FEM program will be chosen in following way:
                     Count the num of FEM programs used by PMF programs and chose the FEM program has minimal referances
                     In other word it MOD3 (but starting from 8 and not 0 since its the first ETH program)
* Params:
* unit (IN)                             - Device Number
* db_id (IN)                            - DB id for the FEM that needs to be set/removed
* fem_pgm_bmp  (OUT)                 - FEM program bitmap that is chosen for the FEM that use the given db_id
                                                          might include more than one FEM program
* Return:    (uint32)  Error in case of error
*******************************************************************************/
uint32
  arad_pp_fp_fem_pgm_id_bmp_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_OUT uint8                     *fem_pgm_bmp
    )
{
    uint32
      res = SOC_SAND_OK,
      pmf_pgm_bmp_used = 0;
    uint8 
      is_for_tm,
      is_default_tm,
      fem_pgm_id_temp = ARAD_PMF_FEM_PGM_INVALID;
    SOC_PPC_FP_DATABASE_INFO
      fp_database_info;
    uint8 pmf_pgm_id =0;
    SOC_PPC_FP_DATABASE_STAGE  stage;
    

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(fem_pgm_bmp);
    /*Reset Value*/
    *fem_pgm_bmp  = 0; 
    /* Get the correct stage */
    res = arad_pp_fp_db_stage_get(
            unit,
            db_id,
            &stage
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_PPC_FP_DATABASE_INFO_clear(&fp_database_info);
    res = arad_pp_fp_database_get_unsafe(
          unit,
          db_id,
          &fp_database_info
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(unit, stage, db_id, 0, &pmf_pgm_bmp_used);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
    
    res = arad_pp_fp_database_is_tm_get(unit, &fp_database_info, &is_for_tm, &is_default_tm);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);


    if(is_for_tm)
    {
        /*For TM programs constant*/
        SOC_SAND_SET_BIT(*fem_pgm_bmp,1,ARAD_PMF_FEM_PGM_FOR_TM);
        /*All TM PMF prog use same FEM pgm*/
        for(pmf_pgm_id = 0 ; pmf_pgm_id < ARAD_PMF_NOF_PROGS; pmf_pgm_id++)
        {
            if (SOC_SAND_GET_BIT(pmf_pgm_bmp_used, pmf_pgm_id) == TRUE)
            {
                res = arad_pp_fp_fem_pgm_per_pmf_pgm_set(unit,stage,pmf_pgm_id,ARAD_PMF_FEM_PGM_FOR_TM);
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            }
        }
    }
    else
    {
        /*Scan if some PMF program already use some FEM program*/
        for(pmf_pgm_id = 0 ; pmf_pgm_id < ARAD_PMF_NOF_PROGS; pmf_pgm_id++)
        {
            if (SOC_SAND_GET_BIT(pmf_pgm_bmp_used, pmf_pgm_id) == TRUE)
            {
                res = arad_pp_fp_fem_pgm_per_pmf_pgm_get(unit,stage,pmf_pgm_id,&fem_pgm_id_temp);
                SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
                if(fem_pgm_id_temp == ARAD_PMF_FEM_PGM_INVALID)
                {
                    res = arad_pp_fp_fem_pgm_id_alloc(unit,stage,&fem_pgm_id_temp);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

                    res = arad_pp_fp_fem_pgm_per_pmf_pgm_set(unit,stage,pmf_pgm_id,fem_pgm_id_temp);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
                    
                }
                if(fem_pgm_id_temp <= ARAD_PMF_FEM_PGM_FOR_ETH_2)
                {
                    SOC_SAND_SET_BIT(*fem_pgm_bmp,1,fem_pgm_id_temp);   
                }
                else
                {
                    /*Shouldnt happen !!!*/
                    SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_ERR, 20, exit);
                }
            }
        }
         
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_pgm_id_set()", 0, db_id);
}

/*
 * allocate FES in program for given action 
 * move exist FES if needed (to preserve order)
 * write to hardware if required (according to flags)
 * update FES-info. (in order to affect next action)
 */
uint32
  arad_pp_fp_action_alloc_fes(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       entry_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_type, /* action*/
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  uint32                       my_priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_IN  uint32                       action_lsb,/* lsb of current action */
    SOC_SAND_IN  uint32                       action_len,/* length of current action */
    SOC_SAND_IN  int32                        required_nof_feses,/* no. of FESes for this DB. Ignore if negative. */
    SOC_SAND_INOUT ARAD_PMF_FES               fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint8                        *found
  )
{
    uint32
        fem_id_ndx,
        fes_tm_lsb,
        new_fes_id = 0;
    ARAD_PMF_FES_INPUT_INFO
        my_fes_info;
    ARAD_PMF_FEM_INPUT_INFO
        fes_input_info;
    ARAD_PMF_FES
        fes_sw_db_info;
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_FP_DATABASE_STAGE
        stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
    SOC_PPC_FP_FEM_ENTRY
        fem_entry2,
      fem_entry;
    SOC_PPC_FP_FEM_CYCLE
      fem_cycle;
    /* coverity[stack_use_overflow:FALSE] */
    SOC_PPC_FP_DATABASE_INFO               
        fp_db_info;
    uint8
        is_fem_blocking,
        use_kaps,
      is_fes_free,
      fem_pgm_id;
    SOC_SAND_SUCCESS_FAILURE
        success;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   "
                          "Allocate FES in prog:%d,  db_id:%d, action_type:%s \n\r"), prog_id, db_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_type)));

    *found = 0;

    /* Get the Database info */
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            stage,
            db_id,
            &fp_db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit); 

    use_kaps = (fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS) ? TRUE : FALSE;

    res = arad_pp_fp_fem_pgm_per_pmf_pgm_get(unit,stage,prog_id,&fem_pgm_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit); 
    if(fem_pgm_id == ARAD_PMF_FEM_PGM_INVALID)
    {
        res = arad_pp_fp_fem_pgm_id_alloc(unit,stage,&fem_pgm_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
        res = arad_pp_fp_fem_pgm_per_pmf_pgm_set(unit,stage,prog_id,fem_pgm_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
    }
    /* 
     * Use a standard way to insert FES / FEM 
     * See in the function for documentation 
     */
    SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
    SOC_PPC_FP_FEM_CYCLE_clear(&fem_cycle);
    fem_entry.is_for_entry = FALSE;
    fem_entry.db_id = db_id;
    fem_entry.db_strength = my_priority;
    fem_entry.entry_id = prog_id; /* Use entry-id for PMF-Program ID */
    fem_entry.action_type[0] = action_type;

    /* Check there is a place in some FES */
    is_fes_free = FALSE;
    for (fem_id_ndx = 0; (fem_id_ndx < ARAD_PMF_LOW_LEVEL_NOF_FESS) && (!is_fes_free); ++fem_id_ndx)
    {
        if (!fes_info[fem_id_ndx].is_used) {
            new_fes_id = fem_id_ndx;
            is_fes_free = TRUE;
        }
    }

    success = SOC_SAND_SUCCESS;
    if (is_fes_free == FALSE)
    {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   "
                              "FEM not found for db_id:%d\n\r"), db_id));
      success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      *found = 0;
      ARAD_DO_NOTHING_AND_EXIT;
    }

    /* in TM mode, any free FEM is valid: the FP action will appear only once, no resolution */
    if  ( (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) == 0 ) && (flags & ARAD_PP_FP_FEM_ALLOC_FES_TM) ) {
        success = SOC_SAND_SUCCESS;
        goto exit_tm;
    }
    /*
     * Note that 'fem_entry.is_for_entry' is FALSE so arad_pp_fp_fem_insert_unsafe()
     * Operates for FES only ('is_for_fem' is FALSE).
     *
     * If 'required_nof_feses' is specified then it is possible to know whether
     * current cycle has enogh space for the whole db.
     */
    {
        /*
         * Set it
         */
        res = arad_pp_fp_fem_insert_unsafe(
                unit,
                &fem_entry,
                &fem_cycle,
                flags,
                NULL,
                fes_info,
                fem_pgm_id,
                0,
                &new_fes_id,
                &success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    /* 
     * In case of failure: 
     * - see if the FEM 1st group blocks (presence of the same action type) 
     * - otherwise, consider the 2 FES groups as one group and try again 
     */

    
    if ((success != SOC_SAND_SUCCESS) &&
        (fem_pgm_id != ARAD_PMF_FEM_PGM_INVALID)) /*If the PMF prog doesnt have FEM PGM than it didnt configure any FEMS*/
    {
        /* Find if the 1st FEM group blocks */        
        is_fem_blocking = FALSE;
        for (fem_id_ndx = 0; (fem_id_ndx < ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP) && (!is_fem_blocking); ++fem_id_ndx)
        {
              SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry2);
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                      unit,
                      stage,
                      fem_pgm_id,
                      fem_id_ndx,
                      &fem_entry2
                    );
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 33, exit);
              if ((fem_entry2.is_for_entry == TRUE) && (fem_entry2.action_type[0] == action_type)) {
                  is_fem_blocking = TRUE;
              }
        }

        /* Re-arrange the 32 FESes */
        if (!is_fem_blocking) {
            res = arad_pp_fp_fem_insert_unsafe(
                    unit,
                    &fem_entry,
                    &fem_cycle,
                    (flags | ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP),
                    NULL,
                    fes_info,
                    fem_pgm_id,
                    0,
                    &new_fes_id,
                    &success
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }

exit_tm:
    *found = (success == SOC_SAND_SUCCESS);


    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   allocated FES-id:%d, \n\r"), new_fes_id));

    /* found and this only for check, then calculate action size */
    if(*found && (flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "FES action:%s, length: %d, lsb: %d\n\r"), SOC_PPC_FP_ACTION_TYPE_to_string(action_type), action_len, action_lsb));
    }
    /* write new FES to allocated place */
    else if(*found  && !(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
        /* new_fes_id */ 

        ARAD_PMF_FES_INPUT_INFO_clear(&my_fes_info);
        ARAD_PMF_FEM_INPUT_INFO_clear(&fes_input_info);
        
        /* Configure the FES differently in TM mode */
        if ((flags & ARAD_PP_FP_FEM_ALLOC_FES_TM) && (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) ==0 ) ) {
            fes_input_info.src_arad.is_key_src = TRUE;
            fes_input_info.src_arad.key_tcam_id = 0 /* always key-A */;
            /* Get the closest 16b multiply */
            fes_tm_lsb = (ARAD_PP_FP_KEY_LENGTH_TM_IN_BITS - action_lsb /* offset-lsb */ - 1);
            fes_input_info.src_arad.key_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(fes_tm_lsb); 
            fes_input_info.src_arad.lookup_cycle_id = 1; /* Always first cycle */
            fes_input_info.src_arad.use_kaps = use_kaps;

            my_fes_info.shift = fes_tm_lsb - fes_input_info.src_arad.key_lsb;
            my_fes_info.action_type = action_type;
            my_fes_info.is_action_always_valid = TRUE;
        }
        else if (flags & ARAD_PP_FP_FEM_ALLOC_FES_FROM_KEY) {
            fes_input_info.src_arad.is_key_src = TRUE;
            fes_input_info.src_arad.key_tcam_id = constraint->tcam_res_id[0];
            /* Get the closest 16b multiply */
            fes_input_info.src_arad.key_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(constraint->tcam_res_id[1]); 
            fes_input_info.src_arad.lookup_cycle_id = 1; /* Always first cycle */
            fes_input_info.src_arad.use_kaps = use_kaps;

            my_fes_info.shift = constraint->tcam_res_id[1] - fes_input_info.src_arad.key_lsb;
            my_fes_info.action_type = action_type;
            my_fes_info.is_action_always_valid = (flags & ARAD_PP_FP_FEM_ALLOC_FES_KEY_IS_CONDITIONAL_VALID)? FALSE: TRUE;
        }
        else {
            fes_input_info.src_arad.is_key_src = FALSE;
            
            fes_input_info.src_arad.key_tcam_id = constraint->tcam_res_id[ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID(action_lsb)];
            fes_input_info.src_arad.key_lsb = action_lsb; 
            fes_input_info.src_arad.lookup_cycle_id = constraint->cycle;
            fes_input_info.src_arad.use_kaps = use_kaps;

            my_fes_info.shift = (use_kaps) ? ARAD_PP_FP_FEM_ACTION_LSB_TO_SHIFT_USE_KAPS(action_lsb) : ARAD_PP_FP_FEM_ACTION_LSB_TO_SHIFT(action_lsb);
            my_fes_info.action_type = action_type;
            my_fes_info.is_action_always_valid = FALSE;
        }
        /* ALWAYS_VALID flag always wins */
        my_fes_info.is_action_always_valid = (flags & ARAD_PP_FP_FEM_ALLOC_FES_ALWAYS_VALID)? TRUE : my_fes_info.is_action_always_valid;
        if (SOC_IS_JERICHO_PLUS(unit) && action_len > 0) {
                my_fes_info.valid_bits = 32 - action_len;
        }
        /* polarity 0 flips the valid bit */
        my_fes_info.polarity = (flags & ARAD_PP_FP_FEM_ALLOC_FES_POLARITY)? 1 : 0;
        
        if (use_kaps && (fp_db_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE) && SOC_IS_JERICHO_PLUS(unit)) {
            if ((action_type == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) || (action_type == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2)) {
                my_fes_info.is_action_always_valid = TRUE;
            } else if (ARAD_PP_FP_FEM_IS_ALWAYS_VALID_LARGE_DIRECT(action_type)) {
                /* 
                 * On QUX, don't take valid bit for action result to support 24bits EEI when it located in zone3.
                 * Always without valid bit for EEI under this condition.
                 */
                my_fes_info.is_action_always_valid = TRUE;
            }
        }

        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   commit changes to HW: "
                              "FES action :%s, length: %d, "
                              "next action-lsb:%d  \n\r"), SOC_PPC_FP_ACTION_TYPE_to_string(action_type), action_len, action_lsb));

        res = arad_pmf_db_fem_input_set_unsafe(
                unit,
                prog_id,
                TRUE, /* is FES*/
                new_fes_id,
                &fes_input_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        
        /*
         * don't install invalid_next fes for combined action into hardware
         * Put fes info into tmp buffer.
         * Re-install invalid_next fes when next fes is allocated
         */
        if (!(flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED)) {
            res = arad_pmf_db_fes_set_unsafe(
                    unit,
                    prog_id,
                    new_fes_id,
                    &my_fes_info
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        } else {
            if ((action_type == SOC_PPC_FP_ACTION_TYPE_COUNTER_A) || (action_type == SOC_PPC_FP_ACTION_TYPE_SNP)) {
                /*install invalid_next fes first*/
                res = arad_pmf_db_fes_set_unsafe(
                        unit,
                        prog_id,
                        new_fes_id-1,
                        &invalid_next_FES_info
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                /*install this fes*/
                res = arad_pmf_db_fes_set_unsafe(
                        unit,
                        prog_id,
                        new_fes_id,
                        &my_fes_info
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
                sal_memset(&invalid_next_FES_info, 0, sizeof(ARAD_PMF_FES_INPUT_INFO));
            } else {
                /* store info and install nop fes */                
                sal_memcpy(&invalid_next_FES_info, &my_fes_info, sizeof(ARAD_PMF_FES_INPUT_INFO));
                my_fes_info.action_type = SOC_PPC_FP_ACTION_TYPE_NOP;
                res = arad_pmf_db_fes_set_unsafe(
                        unit,
                        prog_id,
                        new_fes_id,
                        &my_fes_info
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

            }
        }

        sal_memset(&fes_sw_db_info, 0, sizeof(fes_sw_db_info));
        fes_sw_db_info.action_type = action_type;
        fes_sw_db_info.db_id = db_id;
        fes_sw_db_info.entry_id = entry_id;
        fes_sw_db_info.is_used = 1;
        fes_sw_db_info.is_combined = (flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED) ? 1 : 0;        

        /* 
         * Update if this FES can be moved to the other 
         * group - for the future FESs 
         */

        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.set(
                    unit,
                    stage,
                    prog_id,
                    new_fes_id,
                    &fes_sw_db_info
                  );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc_fes()", 0, 0);
}


uint32
  arad_pp_fp_action_alloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], /* actions*/
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  )
{
    uint32 entry_id = -1;

    return arad_pp_fp_action_alloc_in_prog_with_entry(
                unit,
                db_id,
                prog_id,
                entry_id,
                flags,
                action_types,
                priority,
                constraint,
                action_alloced);

}

uint32 arad_pp_fp_action_alloc_feses(
    SOC_SAND_IN    int                            unit,
    SOC_SAND_IN    uint32                         db_id,
    SOC_SAND_IN    uint32                         prog_id,
    SOC_SAND_IN    uint32                         entry_id,
    SOC_SAND_IN    uint32                         flags[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
    SOC_SAND_IN    SOC_PPC_FP_ACTION_TYPE         action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], /* actions*/
    SOC_SAND_IN    uint32                         action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
    SOC_SAND_IN    uint32                         action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
    SOC_SAND_IN    uint32                         priority,
    SOC_SAND_IN    ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_INOUT ARAD_PMF_FES                   fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT   uint8                          *action_alloced
  )
{
  uint8
      fes_alloced = 0;
  uint32
      action_indx,
      res = SOC_SAND_OK;

  /*
   * Calculate the number of FESes required for this DB. They must all be on
   * the same cycle.
   */
  uint32
      required_nof_feses = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
      if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(action_types[action_indx])) {
          /* no FES needed for this action type */
          continue;
      }
      required_nof_feses++ ;
  }

  /* get relevant programs for the DB */
  for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
      if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(action_types[action_indx])) {
          /* no FES needed for this action type */
          continue;
      }

      res = arad_pp_fp_action_alloc_fes(
                unit,
                db_id,
                prog_id,
                entry_id,
                action_types[action_indx],
                flags[action_indx],
                priority,
                constraint,
                action_lsbs[action_indx],
                action_lengths[action_indx], /* Action length */
                required_nof_feses,
                fes_info,
                &fes_alloced
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20 + action_indx, exit);

      if(!fes_alloced) {
          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "    "
                                "FES: fail to allocate for DB %d, program %d, action %s, flags %d, priority %d, constraint action size %d \n\r"),
                     db_id, prog_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_types[action_indx]), flags[action_indx], priority, constraint->action_size));
          *action_alloced = 0; /* fail to allocate FES */
          goto exit;
      }
  }
  
  /* all allocation successed */
  *action_alloced = 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc_feses()", db_id, prog_id);
}
uint32
  arad_pp_fp_action_alloc_in_prog_with_entry(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  uint32                       entry_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], /* actions*/
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  )
{
  uint32
      action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      exp_action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      exp_action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      exp_action_flags[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      exp_action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX + 1],
      action_size,
      nof_actions,
      action_indx,
      exp_action_indx,
      fes_idx;
  ARAD_PMF_FES
      fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS];
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_STAGE
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
  /* coverity[stack_use_overflow:FALSE] */
  SOC_PPC_FP_DATABASE_INFO               
        fp_db_info;
  SOC_SAND_SUCCESS_FAILURE  
      success;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for(fes_idx = 0; fes_idx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_idx) {
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(
                unit,
                stage,
                prog_id,
                fes_idx,
                &fes_info[fes_idx]
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /* Get the Database info */
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            stage,
            db_id,
            &fp_db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit); 

  sal_memcpy(fp_db_info.action_types, action_types, sizeof(SOC_PPC_FP_ACTION_TYPE) * SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX);

  /* 
   * Get the minimal size for the actions and their LSBs
   */
  res = arad_pp_fp_action_to_lsbs(
          unit,
          stage,
          &fp_db_info,
          action_lsbs,
          action_lengths,
          &action_size,
          &nof_actions,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /* 
   * Get the action length and see if it is OK 
   */ 
  /* check doesn't exceed Tcam action size */
  if((action_size > constraint->action_size) || (success != SOC_SAND_SUCCESS)) {
    LOG_ERROR(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "Unit %d, Program Id %d, Failed to allocate action in program.\n\r"
                          "Action size exceed the maximal size in the constraint.\n\r"
                          "Action size %d, Maximal size in the constraint %d.\n\r"), unit, prog_id, action_size, constraint->action_size));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 80, exit); /* need more bits in TCAM action as expected */
  }

  /* Expand combined actions */
  exp_action_indx = 0;
  for (action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
      /*
       * Combined actions get expanded to their sub-actions
       * while taking care of updating action lsb/action length.
       * On the other side normal actions stay as is.
       * Currently, we have only one type of combined actions,
       * which is Double Action.
       * Each one of these double actions is constisted of
       * 3 normal actions:
       *
       * 1) INVALID_NEXT Action:
       *      This action is used in order to invalidate action A in case
       *      the user wants to use action B only. The activation bit for
       *      this action is the lsb bit of the double action, when
       *      this bit is set to 1, this action invalidates the action
       *      next to it, which in our case is action A.
       *
       * 2) Action A:
       *      Action A is the first action in the double action set. It's
       *      marked as always valid, which in turn makes this action run
       *      always, unless it becomes invalidated by the previous
       *      INVALID_NEXT action.
       *
       * 3) Action B:
       *      Action B is the second action in the double action set, it
       *      uses the second lsb bit of the double action as its valid
       *      bit.
       *
       * Following is a more detailed description of the double action
       * format, and the truth table corresponding to it:
       *
       *                 ************************
       *                 * Double Action Format *
       *                 ************************
       *
       * ACTION A FES (always valid)              INVALID_NEXT FES
       * <----------+------------>              <--------+-------->
       *  _________________________________________________________
       * | A/B Common Data (n-2) | B valid (1) | Invalid next (1) |
       * |_______________________|_____________|__________________|
       * <-----------------+------------------->
       *             ACTOIN B FES
       *                         <-------------+------------------>
       *                                     MODE
       *
       *                 *****************************
       *                 * Double Action Truth Table *
       *                 *****************************
       *
       * +---------------+---------+--------------+-------------+
       * | MODE (2 bits) | M valid | invalid_next | Action Run  |
       * +---------------+---------+--------------+-------------+
       * |       0       |   0     |      0       |      A      |
       * |       1       |   0     |      1       |     NOP     |
       * |       2       |   1     |      0       |    A + B    |
       * |       3       |   1     |      1       |      B      |
       * +---------------+---------+--------------+-------------+
       *
       */
      if (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER) {
          /* Meter And Counter double action */

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT;
          exp_action_lengths[exp_action_indx] = 0;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx];
          exp_action_flags[exp_action_indx] = flags | ARAD_PP_FP_FEM_ALLOC_FES_POLARITY | ARAD_PP_FP_FEM_ALLOC_FES_COMBINED;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_COUNTER_A;
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx] - 2;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx] + 2;
          exp_action_flags[exp_action_indx] = flags | ARAD_PP_FP_FEM_ALLOC_FES_ALWAYS_VALID | ARAD_PP_FP_FEM_ALLOC_FES_COMBINED;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_METER_A;
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx] - 1;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx] + 1;
          exp_action_flags[exp_action_indx] = flags;
          exp_action_indx++;
      } else if (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP) {
          /* Snoop And Trap double action */

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT;
          exp_action_lengths[exp_action_indx] = 0;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx];
          exp_action_flags[exp_action_indx] = flags | ARAD_PP_FP_FEM_ALLOC_FES_POLARITY | ARAD_PP_FP_FEM_ALLOC_FES_COMBINED;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_SNP;
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx] - 2;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx] + 2;
          exp_action_flags[exp_action_indx] = flags | ARAD_PP_FP_FEM_ALLOC_FES_ALWAYS_VALID | ARAD_PP_FP_FEM_ALLOC_FES_COMBINED;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED;
          /* (-1) bit from the end since snoop str is 2 bits unlike trap str which is 3 */
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx] - 2;
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx] + 1;
          exp_action_flags[exp_action_indx] = flags;
          exp_action_indx++;
      } else {

          exp_action_types[exp_action_indx] = action_types[action_indx];
          exp_action_lengths[exp_action_indx] = action_lengths[action_indx];
          exp_action_lsbs[exp_action_indx] = action_lsbs[action_indx];
          exp_action_flags[exp_action_indx] = flags;
          exp_action_indx++;
      }
  }
  if (exp_action_indx > SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
      LOG_ERROR(BSL_LS_SOC_FP,
          (BSL_META_U(unit,
                  "Unit %d, Program Id %d, Failed to allocate action in program.\n\r"
                  "Number of actions %d exceeds number of allowed actions per program (%d).\n\r"),
                  unit, prog_id, exp_action_indx, SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ACTION_FOMAT_ID_OUT_OF_RANGE_ERR, 90, exit);
  }
  exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID;

  res = arad_pp_fp_action_alloc_feses(
        unit,
        db_id,
        prog_id,
        entry_id,
        exp_action_flags,
        exp_action_types,
        exp_action_lsbs,
        exp_action_lengths,
        priority,
        constraint,
        fes_info,
        action_alloced
      );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc_in_prog()", db_id, prog_id);
}


uint32
  arad_pp_fp_action_alloc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       flags,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX], /* actions*/
    SOC_SAND_IN  uint32                       priority, 
    SOC_SAND_IN  uint32                       selected_cycle[ARAD_PMF_LOW_LEVEL_NOF_PROGS_ALL_STAGES],
    SOC_SAND_INOUT  ARAD_PP_FEM_ACTIONS_CONSTRAINT *constraint,
    SOC_SAND_OUT uint8                        *action_alloced
  )
{
  uint32   
      cur_prog;
  uint32
      exist_progs[1],
      prog_result;
  uint8
      action_alloced_in_prog = 0;
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_STAGE
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* get relevant programs for the DB */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            stage,
            db_id,
            0,
            exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  /* same management for new/old DBs */
  /* on all new programs allocate FES for each action */

  /* get first new program */
  cur_prog = 0;
  ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,ARAD_PMF_LOW_LEVEL_NOF_PROGS,FALSE,prog_result);
  while(prog_result != 0) {
      constraint->cycle = selected_cycle[cur_prog];
      res = arad_pp_fp_action_alloc_in_prog(
                unit,
                db_id,
                cur_prog,
                flags,
                action_types,
                priority,
                constraint,
                &action_alloced_in_prog
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if(!action_alloced_in_prog) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "    "
                              "FES: fail to allocate for DB %d, program %d, flags %d, priority %d, constraint action size %d \n\r"),
                   db_id, cur_prog, flags, priority, constraint->action_size));
          *action_alloced = 0;  /* fail to action allocate in this program */
          goto exit; /* done with FAIL*/
      }
      /* get next program */
      ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS-cur_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,TRUE,prog_result);
  }

  *action_alloced = 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_alloc()", 0, 0);
}


/*
 * de-allocate FES in program for given action 
 * This FES has to exist 
 */
uint32
  arad_pp_fp_action_dealloc_fes(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_type /* action*/
  )
{
    uint32
        new_fes_id = 0;
    int32
        indx = 0;
    uint8
        found;
    ARAD_PMF_FES_INPUT_INFO
        my_fes_info;
    ARAD_PMF_FEM_INPUT_INFO
        fes_input_info;
    ARAD_PMF_FES
        fes_sw_db_info;
    uint32
      res = SOC_SAND_OK;
    SOC_PPC_FP_DATABASE_STAGE
        stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   "
                          "De-allocate FES in prog:%d,  db_id:%d, action_type:%s \n\r"), prog_id, db_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_type)));

    found = 0;
    /* find the FES with this action */
    for(indx = 0; indx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++indx) {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(
                    unit,
                    stage,
                    prog_id,
                    indx,
                    &fes_sw_db_info
                  );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);


        if(fes_sw_db_info.is_used
           && (fes_sw_db_info.db_id == db_id)
           && ((fes_sw_db_info.action_type == action_type)
           /* Special case for trap action. Both SOC_PPC_FP_ACTION_TYPE_TRAP and SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED 
           map to the same hardware value, in the later case, customer knows that TRAP qualifier has trash values. */
           || ((fes_sw_db_info.action_type == SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED) && (action_type == SOC_PPC_FP_ACTION_TYPE_TRAP))
           || ((fes_sw_db_info.action_type == SOC_PPC_FP_ACTION_TYPE_TRAP) && (action_type == SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED)))) {
            new_fes_id = indx;
            found = 1;
            break;
        }
    }


    if(found == 0) {/* not such a FES */
      LOG_ERROR(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "   "
                            "FES deallocation failed:%d  - no FES with this action %s for DB %d and PMF-Program %d \n\r"), 
                 found, SOC_PPC_FP_ACTION_TYPE_to_string(action_type), db_id, prog_id));
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_ENTRY_ACTION_TYPE_NOT_IN_DB_ERR, 7, exit); 
    }

    LOG_DEBUG(BSL_LS_SOC_FP,
              (BSL_META_U(unit,
                          "   "
                          "Commit changes to HW: "
                          "De-allocate FES %d action :%s, "), new_fes_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_type)));

    ARAD_PMF_FES_INPUT_INFO_clear(&my_fes_info);
    my_fes_info.action_type = SOC_PPC_FP_ACTION_TYPE_NOP;
    res = arad_pmf_db_fes_set_unsafe(
            unit,
            prog_id,
            new_fes_id,
            &my_fes_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ARAD_PMF_FEM_INPUT_INFO_clear(&fes_input_info);
    fes_input_info.src_arad.is_nop = 1;

    res = arad_pmf_db_fem_input_set_unsafe(
            unit,
            prog_id,
            TRUE, /* is FES*/
            new_fes_id,
            &fes_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Update SW DB for the other actions */
    fes_sw_db_info.is_used = 0;
    fes_sw_db_info.db_id = 0;
    fes_sw_db_info.action_type = SOC_PPC_FP_ACTION_TYPE_INVALID;
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.set(
                unit,
                stage,
                prog_id,
                new_fes_id,
                &fes_sw_db_info
              );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);


    /* 
     * Re-order the FESes in this group to be at the beginning
     */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_dealloc_fes()", 0, 0);
}




uint32
  arad_pp_fp_action_dealloc_in_prog(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  uint32                       prog_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX] /* actions*/
  )
{
  uint32
      action_indx;
  uint32
      exp_action_indx = 0;
  uint32
      exp_action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX + 1];
  uint32
    res = SOC_SAND_OK;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* Expand double actions for deallocation, just like in allocation stage. Check "Double Actions" for more info */
  for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
      if (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER) {
          /* Meter And Counter double action */

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_COUNTER_A;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_METER_A;
          exp_action_indx++;
      } else if (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP) {
          /* Snoop And Trap double action */

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_SNP;
          exp_action_indx++;

          exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED;
          exp_action_indx++;
      } else {

          exp_action_types[exp_action_indx] = action_types[action_indx];
          exp_action_indx++;
      }
  }

  if (exp_action_indx > SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) {
      LOG_ERROR(BSL_LS_SOC_FP,
          (BSL_META_U(unit,
                  "Unit %d, Program Id %d\n\r"
                  "Number of actions %d exceeds number of allowed actions per program (%d).\n\r"
                  "This is an errornous state, since allocation stage should make sure this never happens\n\r"),
                  unit, prog_id, exp_action_indx, SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_FEM_NDX_OUT_OF_RANGE_ERR, 101, exit);
  }
  exp_action_types[exp_action_indx] = SOC_PPC_FP_ACTION_TYPE_INVALID;

  /* get relevant programs for the DB */
  for(exp_action_indx = 0; exp_action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && exp_action_types[exp_action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++exp_action_indx) {
      if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(exp_action_types[exp_action_indx])) {
          /* no FES needed for this action type */
          continue;
      }

      /* Find the FES to deallocate */
      res = arad_pp_fp_action_dealloc_fes(
                unit,
                db_id,
                prog_id,
                exp_action_types[exp_action_indx]
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20 + exp_action_indx, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_dealloc_in_prog()", db_id, prog_id);
}


uint32
  arad_pp_fp_action_dealloc(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       db_id,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_types[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX] /* actions*/
  )
{
  uint32   
      cur_prog;
  uint32
      exist_progs;
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* get relevant programs for the DB */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            db_id,
            0,
            &exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  /* same management for new/old DBs */
  /* on all new programs allocate FES for each action */

  /* get first new program */
  for (cur_prog = 0; cur_prog < SOC_DPP_DEFS_GET(unit, nof_ingress_pmf_programs); cur_prog++) {
      if ((1 << cur_prog) & exist_progs) {
          res = arad_pp_fp_action_dealloc_in_prog(
                    unit,
                    db_id,
                    cur_prog,
                    action_types
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_dealloc()", 0, 0);
}




/*
 * given DB id return list of action for this DB
 */
uint32
  arad_pp_fp_fem_db_actions_get(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_OUT  uint32                   action_lsbs[2]
  )
{
   return 0;
   
}

/* 
 * A subset sum problem.
 * Returns if there is a subset of action_array of which the action sizes accumulated to 'sum',
 * this subnet is identified by indexes returned in seq, the size of the subnet is returned in idx.
 * If it fails to find such a subnet, it returns the difference of accumulated action sizes which is closest
 * to sum.
 */
int action_sort_to_length(action_inf *action_array, uint32 n, uint32 sum, int *seq, int *idx, int *min_diff)
{
	if (n < 1) {
		/* No more elements in action_array */
		return 0;
	}
	
    if ((action_array[n-1].size+1) == sum) { /* action size +1 for valid bit */
		/* Find a subset of which the action sizes accumulated to TCAM action size */
		seq[*idx] = n-1;
		*idx += 1;
		return 1;
	}
	else {
		if (n == 1) return 0; /* Last one, not found */
		
		if (action_sort_to_length(action_array, n-1, sum, seq, idx, min_diff)) {
			/* Either we find a subset in action_array except the last element */
			return 1;
		}
		else if (sum > (action_array[n-1].size+1)) {
		    if ((*min_diff == 0) || (*min_diff >  (sum-(action_array[n-1].size+1)))) {
				*min_diff = sum-(action_array[n-1].size+1);
			}

			if (action_sort_to_length(action_array, n-1, sum-(action_array[n-1].size+1), seq, idx, min_diff)) {
                /* Or we find a subset in action_array except the last element which accumulates to sum-(action_array[n-1].size+1),
                 * in this case action_array[n-1] is part of the returned subnet.
                */
				seq[*idx] = n-1;
        		*idx += 1;
				return 1;
			}
		}
	}
	
	return 0;
}


/*
 * given list of action returns lsb of each action
 */
uint32
  arad_pp_fp_action_to_lsbs(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE stage,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO  *info,
      SOC_SAND_OUT uint32                    action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      SOC_SAND_OUT uint32                    action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
      SOC_SAND_OUT ARAD_TCAM_ACTION_SIZE     *action_size,
      SOC_SAND_OUT uint32                    *nof_actions,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE  *success
  )
{
    int32
        action_indx = 0;
    uint32
        action_lsb_ingress_tcam = 0,
      action_lsb_egress = 0,
        max_zone_length = 0,
        nof_bits, /* actually maximum number of bits needed */
        action_len_complete,
      action_len,
        zone_length[2] = {0, 0}, /* Number of bits taken per first 40b zone */
      res = SOC_SAND_OK;
    uint8 
      action_change_key_loop;
    int algorithm_indx = 0;
    int i = 0;
    int sequence[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX] = { 0 }; 
	int idx = 0;
	int min_diff = 0;
    action_inf act;    
    action_inf action_array[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    uint32 action_lsbs_tmp[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1];
    uint32 width_dft, width, move_index;
    uint32 loc_hw_id;
    uint8 resort=0;
    uint8 action_len_total;
    uint8 use_kaps = (info->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS) ? TRUE : FALSE;
    SOC_PPC_FP_DATABASE_TYPE db_type = info->db_type;
    uint32 *action_widths = (uint32 *)info->action_widths;
    SOC_PPC_FP_ACTION_TYPE *action_types = (SOC_PPC_FP_ACTION_TYPE *)info->action_types;
    SOC_PPC_FP_ACTION_TYPE* non_const_action_types = (SOC_PPC_FP_ACTION_TYPE*) action_types;
   

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    action_len_total = 0;
    for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID); ++action_indx) {
        if (db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE) {
            res = arad_pmf_db_fes_action_size_get_unsafe(
                    unit,
                    action_types[action_indx],
                    stage,
                    &action_len,
                    &action_lsb_egress
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
            action_len_total += action_len;
        }
    }

    /*in case action allocation fails we will sort the actions acording to their sizes and then try to allocate again */
    
    for (algorithm_indx = 0; algorithm_indx < 2; algorithm_indx++) {

        /*if the first try to allocate actions fail sort the action*/
        if (algorithm_indx == 1) {
            sal_memset(action_lsbs, 0x0, sizeof(uint32) * (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1));
            sal_memset(action_lengths, 0x0, sizeof(uint32) * (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1));
            if (action_sort_to_length(action_array, *nof_actions, max_zone_length, sequence, &idx, &min_diff) == 0) {
                /* Failed to find a subset of action sizes which accumulate to TCAM action size exactly,
                 * run it second time to find a cloest match. It should always succeed.
                */
				action_sort_to_length(action_array, *nof_actions, max_zone_length-min_diff, sequence, &idx, &min_diff);
            }

            /* arranges the actions according to the received sequence*/
            for(i = 0; i < idx; i++) {
                act = action_array[i];
                action_array[i] = action_array[sequence[i]];
                action_array[sequence[i]] = act;
            }
                
            for (action_indx = 0;  action_indx < *nof_actions ; action_indx++)
                non_const_action_types[action_indx]= action_array[action_indx].indx;
            action_lsb_ingress_tcam = 0;
            zone_length[0] = 0;
            zone_length[1] = 0;

        }


        nof_bits = 0;
        *nof_actions = 0;
        *success = SOC_SAND_SUCCESS;

    
    action_lsbs[0] = 0;
    action_lengths[0] = 0;
    max_zone_length = (use_kaps) ? ARAD_PP_FP_KAPS_RESULT_ACTION_WIDTH : ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE;
    /* 
     * For cascaded lookups, the action must be at the LSB of the action table
     * Loop first on the cascaded action, after on the other actions.
     */
    for (action_change_key_loop = 0; action_change_key_loop <= 1; action_change_key_loop++) {
        if ((stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) && (action_change_key_loop == 0)) {
            /* no cascaded databases at egress - single lookup stage */
            continue;
        }
        for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID); ++action_indx) {
            if ((action_change_key_loop == 0) && (action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY)) {
                continue;
            }
            else if ((action_change_key_loop == 1) && (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY)) {
                continue;
            }

                /*
                 * Starting Jer+, action length can be changed (see Valid bits feature).
                 * Also, or UDH actions on all devices, user can trim action width if not all 8 bits
                 * are necessary (On the condition that the user doesn't use the garbage bits at the other
                 * end in EPMF).
                 */
                if ((SOC_IS_JERICHO_PLUS(unit) ||
                    action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1 ||
                    action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2) &&
                    action_widths != NULL && action_widths[action_indx] > 0) {

                    action_len = action_widths[action_indx];
                } else {
                    res = arad_pmf_db_fes_action_size_get_unsafe(
                        unit,
                        action_types[action_indx],
                        stage,
                        &action_len,
                        &action_lsb_egress
                        );
                }
                action_array[action_indx].size=action_len;
                action_array[action_indx].indx=action_types[action_indx];

                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
                if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(action_types[action_indx])) {
                    action_lsbs[action_indx] = 0;
                    action_lsb_ingress_tcam = 0;
                    action_len_complete = action_len; /* not +1 for valid bit since it does not go the FES */
                } else if (stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
                    action_len_complete = action_len; /* not +1 for valid bit since the encoding is hard-coded */
                    action_lsbs[action_indx] = action_lsb_egress; /* HW lsb hard-coded at egress */
                } else {
                    /* Check if the action is in 31:0 or 39:8. Otherwise, try the second part */
                    if (((action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) || (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2)
                         || (action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_LEARN_SA_16_TO_47))
                        && (action_len == 32)) {
                        /* Special case User-Header: consider it as 31b action length since 32b action length + 1b validity not supported in FES HW */
                        action_len = 31;
                    }
                    /*KAPS for EEI(zone3) on QUX*/
                    if (ARAD_PP_FP_FEM_IS_ALWAYS_VALID_LARGE_DIRECT(action_types[action_indx]) && (action_len == 24)) {
                        action_len = 23;
                    }
                    
                    action_len_complete = action_len + 1; /* +1 for my valid */                    
                    action_lsb_ingress_tcam = ARAD_PP_FP_FEM_ACTION_LOCAL_LSB(use_kaps,zone_length[0], action_len_complete);
                    if ((zone_length[0] < max_zone_length /* Valid usage of macros */)
                        && (action_lsb_ingress_tcam + action_len_complete <= max_zone_length)) {
                        action_lsbs[action_indx] = action_lsb_ingress_tcam; /* my action lsb */
                    } else {
                        action_lsb_ingress_tcam = ARAD_PP_FP_FEM_ACTION_LOCAL_LSB(use_kaps,zone_length[1], action_len_complete);
                        if (zone_length[1] >= max_zone_length && action_len_complete > 0) {
                           LOG_ERROR(BSL_LS_SOC_FP,
                                      (BSL_META_U(unit,
                                                  "    "
                                                  "Error in action computation: "
                                                  "Not enough resources, try to use less actions\n\r")
                                       ));

                            *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
                            ARAD_DO_NOTHING_AND_EXIT;

                        }
                        action_lsbs[action_indx] = max_zone_length + action_lsb_ingress_tcam; /* my action lsb */
                    }
                }
                /* Update 1st zone size if needed */
                if (action_lsbs[action_indx] < max_zone_length) {
                    zone_length[0] = action_len_complete + action_lsb_ingress_tcam;
                } else {
                    zone_length[1] = action_len_complete + action_lsb_ingress_tcam;
                }
                nof_bits = SOC_SAND_MAX(nof_bits, action_lsbs[action_indx] + action_len_complete);
                /* action length: cannot be extrapolated by the LSBs due to the bit 40 jump */
                action_lengths[action_indx] = action_len_complete;
                *nof_actions += 1;
            }
        }

        
        action_lsbs[action_indx] = nof_bits;
        if (use_kaps) {
            /* action_size when using kaps resembles whether to use 32 bit or 64 bit entry size */
            if (nof_bits <= SOC_DPP_IMP_DEFS_GET(unit, kaps_dma_zone_width)) { 
                /* single zone is enough */
                *action_size = BITS2BYTES(SOC_DPP_IMP_DEFS_GET(unit, kaps_dma_zone_width));
                break;
            } else { 
                /* use double zone*/
                *action_size = 2 * BITS2BYTES(SOC_DPP_IMP_DEFS_GET(unit, kaps_dma_zone_width));
                break;
            }
        }
        else if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) || (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS)) {
            if (nof_bits <= SOC_DPP_DEFS_GET(unit, tcam_action_width)) {
                *action_size = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
                break; /* action allocation succeed */
            } else if (nof_bits <= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                *action_size = ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
                break; /* action allocation succeed */
            } else if (nof_bits <= 3 * SOC_DPP_DEFS_GET(unit, tcam_action_width)) {
                *action_size = ARAD_TCAM_ACTION_SIZE_THIRD_20_BITS;
                break; /* action allocation succeed */ 
            } else if (nof_bits <= 4 * SOC_DPP_DEFS_GET(unit, tcam_action_width)) {
                *action_size = ARAD_TCAM_ACTION_SIZE_FORTH_20_BITS;
                break; /* action allocation succeed */ 
            } else if (algorithm_indx==1){
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "    "
                                      "Error in action computation: "
                                      "For stage %s, total action size %d bits \n\r"),
                           SOC_PPC_FP_DATABASE_STAGE_to_string(stage), nof_bits));
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "\n\r")));

                *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
                ARAD_DO_NOTHING_AND_EXIT;
            }
        } else {
            break;
        }

    }

    /* put reduced action to highest location among of ations , support only one reduced action per group */
    if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)
        && (db_type != SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION)
        && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "reduced_action_support", 0)) {        

        /* keep a copy for action_lsbs */
        sal_memcpy(action_lsbs_tmp, action_lsbs, (sizeof(uint32)*(*nof_actions)));
        for (action_indx = 0; action_indx < *nof_actions; action_indx++) {
            res = arad_pmf_fem_action_width_default_get(unit, action_types[action_indx], &width_dft);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            res = arad_pmf_fem_action_width_get_unsafe(unit, action_types[action_indx],&width, &loc_hw_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            if ((width_dft > width) && (action_indx != (*nof_actions - 1))) {
                resort=1;
                move_index = action_indx;
                break;
            }
        }

        if (resort == 1) {
            /* Check if the reduced action resides in the 48 lsb or 48 msb of the wide action zone
             * Avoid case: reduced action splited into two zone.
             */
            if (action_lsbs_tmp[move_index] < ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                for (action_indx = (move_index+1); action_indx < *nof_actions; action_indx++) {
                    /* move to MSB of low 48-bits zone */
                    if (action_lsbs_tmp[action_indx] < ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                        action_lsbs[action_indx] = action_lsbs_tmp[action_indx] - action_lengths[move_index];
                        action_lsbs[move_index] += action_lengths[action_indx];
                    }
                }
            } else {
                for (action_indx = (move_index+1); action_indx < *nof_actions; action_indx++) {
                    /* move to MSB of high 48-bits zone */
                    if (action_lsbs_tmp[action_indx] >= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                        action_lsbs[action_indx] = action_lsbs_tmp[action_indx] - action_lengths[move_index];
                        action_lsbs[move_index] += action_lengths[action_indx];
                    }
                }
            }
        }        
    } 

    /* Fixed action size needed for Egress */
    if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        *action_size = ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_to_lsbs()", action_indx, 0);
}



/* 
 * Map between action values and buffer of the TCAM Action table
 */
uint32
  arad_pp_fp_action_value_buffer_mapping(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_IN  ARAD_PP_FP_ACTION_BUFFER_DIRECTION direction,
      SOC_SAND_INOUT SOC_PPC_FP_ACTION_VAL   action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
      SOC_SAND_INOUT uint32                  buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE],
      SOC_SAND_OUT   uint32                  *buffer_size
  )
{
    uint32
        action_lengths[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
        action_lsbs[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1],
        nof_actions,
        action_val_indx,
        action_val_indx2,
        action_indx,
        is_valid_bit,
        len,
        ext_count = 0,
        ext_index;
    uint8
        action_found;        
    uint32
      res = SOC_SAND_OK;
    uint32 action_val_16_msb_for_flp_0[3] = {0};

    SOC_PPC_FP_ACTION_VAL   
        action_vals_lcl[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    SOC_PPC_FP_DATABASE_STAGE
        stage; 
    ARAD_TCAM_ACTION_SIZE  
        action_size;
    /* coverity[stack_use_overflow:FALSE] */
    SOC_PPC_FP_DATABASE_INFO               
        fp_db_info;
    SOC_SAND_SUCCESS_FAILURE  
        success;
    uint8 is_vt_match_ipv4 = FALSE;
    uint8 is_fp_db_extended_fwrd = FALSE;
    uint8 is_fp_flp_match = FALSE;
    uint8 is_kaps_direct_action_always_valid=FALSE;
    uint8 check_condition=FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Get the correct stage */
    res = arad_pp_fp_db_stage_get(
              unit,
              db_id,
              &stage
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    /* Get the Database info */
    res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            stage,
            db_id,
            &fp_db_info
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit); 

    if ((fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_DBAL)){        
        if (fp_db_info.action_types[0]== SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) {/* FLP action */
            is_fp_flp_match = TRUE;
        } else {
			is_vt_match_ipv4 = TRUE;
		}
    }

#if defined(INCLUDE_KBP)
    if (fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_EXTENDED_DATABASES  && (fp_db_info.internal_table_id != ARAD_KBP_FRWRD_TBL_ID_IPV4_MC)){
         is_fp_db_extended_fwrd = TRUE;
    }
#endif 

    if ( is_vt_match_ipv4 ) /* In case of Flexible QinQ - there's only 1 action, which is the inlif value*/
    {
        action_lsbs[0] = 0;
        action_lengths[0] = 16 ; /*Length of Inlif */
        action_size = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
        nof_actions = 1 ; 
        fp_db_info.action_types[0] = SOC_PPC_FP_ACTION_TYPE_IN_LIF;
    }else if ( is_fp_flp_match ) /* In case of FLP - DBAL - we use only action */
    {
        action_lsbs[0] = 0;
        action_lengths[0] = 19 ;         

        action_lsbs[1] = 32;
        action_lengths[1] = 32 ;
        fp_db_info.action_types[1] = SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2;

        nof_actions = 2 ; 

        if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
            buffer[0] = action_vals[0].val; 
            buffer[1] = action_vals[1].val;
            goto exit;
        }
    }
#if defined(INCLUDE_KBP)
    else if (is_fp_db_extended_fwrd) {
        action_lsbs[0] = 64;
        action_lengths[0] = 24 ;
        action_lsbs[1] = 32;
        action_lengths[1] = 32 ;
        action_lsbs[2] = 0;
        action_lengths[2] = 32 ;
        action_lsbs[3] = 88;
        action_lengths[3] = 32 ;
        nof_actions = 4;
    }
#endif 
    else
    {
      /* get from action vals lsbs */
        res = arad_pp_fp_action_to_lsbs(
                unit,
                stage,
                &fp_db_info,
                action_lsbs,
                action_lengths,
                &action_size,
                &nof_actions,
                &success
              );
        if (buffer_size != NULL) {
            *buffer_size = action_size;
        }
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        if (success != SOC_SAND_SUCCESS) {
          LOG_ERROR(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "Unit %d DB-Id %d, Invalid action composition.\n\r"),
                     unit, db_id));
            SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 101, exit);
        }
    }

    /* Special case for FLP: all the actions must be encoded from bit 0, since they enter in different ELK DBs */
    if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) && (!is_fp_db_extended_fwrd)) {
        sal_memset(action_lsbs, 0x0, sizeof(uint32) * (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX+1));
    }

    /* 
     * Copy the action types and values to a common local array
     */
    for(action_val_indx = 0; action_val_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_val_indx) {
        SOC_PPC_FP_ACTION_VAL_clear(&action_vals_lcl[action_val_indx]);
        if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
            action_vals_lcl[action_val_indx].type = action_vals[action_val_indx].type;
            action_vals_lcl[action_val_indx].val = action_vals[action_val_indx].val;
        }
        else if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
            /* Presence of a valid bit for each TCAM action*/
            check_condition = ARAD_PP_FP_FEM_IS_ALWAYS_VALID_LARGE_DIRECT(fp_db_info.action_types[action_val_indx]) ? TRUE : FALSE;
            is_kaps_direct_action_always_valid = ((fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS) 
                && SOC_IS_JERICHO_PLUS(unit)
                && (fp_db_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE)
                && ((fp_db_info.action_types[action_val_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) 
                    || (fp_db_info.action_types[action_val_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2)
                    || check_condition)) ? TRUE : FALSE;

            is_valid_bit = ((stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) 
                            || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) 
                            || (is_vt_match_ipv4 )
                            || (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(fp_db_info.action_types[action_val_indx])) 
                            || (fp_db_info.action_types[action_val_indx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER)
                            || (fp_db_info.action_types[action_val_indx] == SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP)
                            || is_kaps_direct_action_always_valid)? 0: 1;

            /* See if the action is set in the buffer - no verification at egress */
            if ((action_val_indx < nof_actions) && (SHR_BITGET(buffer,action_lsbs[action_val_indx]) || (!is_valid_bit))) {
                action_vals_lcl[action_val_indx].type = fp_db_info.action_types[action_val_indx];
                len = action_lengths[action_val_indx];
                SHR_BITCOPY_RANGE(&(action_vals_lcl[action_val_indx].val), 0, buffer, action_lsbs[action_val_indx]+is_valid_bit, len-is_valid_bit); /* skip valid bit at ingress */
                if(SOC_IS_JERICHO(unit)){
                    if ((action_vals_lcl[action_val_indx].type >= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) &&
                        (action_vals_lcl[action_val_indx].type <= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7)) {
                        ext_count = SOC_PPC_BIT_TO_U32(len);

                        for (ext_index = 1; ext_index < ext_count; ext_index++) {
                            if (ext_index == (ext_count -1)) {
                                SHR_BITCOPY_RANGE(&(action_val_16_msb_for_flp_0[ext_index - 1]), (ext_index + 1) * 32 - len, buffer, action_lsbs[action_val_indx] + ext_index * 32, (len - ext_index * 32));
                            } else {
                                SHR_BITCOPY_RANGE(&(action_val_16_msb_for_flp_0[ext_index - 1]), 0, buffer, action_lsbs[action_val_indx] + ext_index * 32, 32);
                            }
                        }
                    }
                }else{
                    if (action_vals_lcl[action_val_indx].type == SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) {
                        /* if action is SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0 it maps to two actions (48 bits)*/
                        SHR_BITCOPY_RANGE(&(action_val_16_msb_for_flp_0[0]), 0, buffer, action_lsbs[action_val_indx]+32, 16); /* skip valid bit at ingress */
                    }
                }
            }
            else
            {
                action_vals_lcl[action_val_indx].type = SOC_PPC_FP_ACTION_TYPE_INVALID;
            }
        }
    }

    /* map from action type to placement in DB actions */
    action_val_indx2 = 0; /* Encode the number of valid actions up to now */
    for(action_val_indx = 0; action_val_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_val_indx) {
        if (action_vals_lcl[action_val_indx].type == SOC_PPC_FP_ACTION_TYPE_INVALID) {
            if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
                /* Stop the mapping if no more actions */
                break;
            }
            else if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
                /* This action is invalid*/
                action_vals[action_val_indx2].type = SOC_PPC_FP_ACTION_TYPE_INVALID;
                action_vals[action_val_indx2].val = 0;
                continue;
            }
        }

        action_found = 0;
        for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX && fp_db_info.action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID; ++action_indx) {
            /* 
             * The list of actions between the buffer and the Database is identical: 
             * thus, we can go to the correct iindex directly 
             */
            if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
                action_indx = action_val_indx;
            }

            if(action_vals_lcl[action_val_indx].type == fp_db_info.action_types[action_indx]) { /* if this is the action*/
                if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER) {
                    /* Presence of a valid bit for each TCAM action*/
                    check_condition = ARAD_PP_FP_FEM_IS_ALWAYS_VALID_LARGE_DIRECT(fp_db_info.action_types[action_indx]) ? TRUE : FALSE;

                    is_kaps_direct_action_always_valid = ((fp_db_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS)
                        && SOC_IS_JERICHO_PLUS(unit)
                        && (fp_db_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE)
                        && ((fp_db_info.action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1) 
                            || (fp_db_info.action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2)
                            || check_condition)) ? TRUE : FALSE;

                    is_valid_bit = ((stage != SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)
                                    || (is_vt_match_ipv4 )
                                    || (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(action_vals_lcl[action_val_indx].type)) 
                                    || (fp_db_info.action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_AND_METER)
                                    || (fp_db_info.action_types[action_indx] == SOC_PPC_FP_ACTION_TYPE_SNOOP_AND_TRAP)
                                    || is_kaps_direct_action_always_valid)? 0: 1;
                    len = action_lengths[action_indx];
                    if (is_valid_bit) {
                        SHR_BITSET(buffer, action_lsbs[action_indx]); /* set valid bit at LSB of the action (FES lsb) */
                    }
                    
                    SHR_BITCOPY_RANGE(buffer, action_lsbs[action_indx]+is_valid_bit,&(action_vals_lcl[action_val_indx].val), 0, len-is_valid_bit); /* Copy action value */
                    if (action_val_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX - 1) {
                        if (SOC_IS_JERICHO(unit)){
                            if ((action_vals_lcl[action_val_indx].type >= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) &&
                                (action_vals_lcl[action_val_indx].type <= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7)) {
                                ext_count = SOC_PPC_BIT_TO_U32(len);
                                for (ext_index = 1; ext_index < ext_count; ext_index++) {
                                    if (action_vals_lcl[action_val_indx + ext_index].type != action_vals_lcl[action_val_indx].type) {
                                        break;
                                    }

                                    if (ext_index == (ext_count -1)) {
                                        SHR_BITCOPY_RANGE(buffer, action_lsbs[action_indx] + ext_index * 32, &(action_vals_lcl[action_val_indx + ext_index].val), ((ext_index + 1) * 32 - len), (len - ext_index * 32));
                                        action_val_indx += ext_index;
                                    } else {
                                        SHR_BITCOPY_RANGE(buffer, action_lsbs[action_indx] + ext_index * 32, &(action_vals_lcl[action_val_indx + ext_index].val), 0, 32);
                                    }
                                }
                            }
                        }else{
                            if ((action_vals_lcl[action_val_indx].type == SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) && (action_vals_lcl[action_val_indx+1].type == SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0)) {
                                SHR_BITCOPY_RANGE(buffer, action_lsbs[action_indx]+32, &(action_vals_lcl[action_val_indx+1].val), 16, 16); /* Copy action value */
                                action_val_indx++;
                            }
                        }
                    }
                }
                else if (direction == ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION) {
                    action_vals[action_val_indx2].type = action_vals_lcl[action_val_indx].type;
                    action_vals[action_val_indx2].val = action_vals_lcl[action_val_indx].val;
                    if(SOC_IS_JERICHO(unit)){
                        if ((action_vals[action_val_indx2].type >= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) &&
                            (action_vals[action_val_indx2].type <= SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_7)) {
                            for (ext_index = 1; ext_index < ext_count; ext_index++) {
                                if (action_val_16_msb_for_flp_0[ext_index-1] != 0) {
                                    action_val_indx2++;
                                    action_vals[action_val_indx2].val = action_val_16_msb_for_flp_0[ext_index - 1];
                                    action_vals[action_val_indx2].type = action_vals_lcl[action_val_indx].type;
                                }
                            }
                        }
                    }else{
                        if (action_vals[action_val_indx2].type == SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0) {
                            action_vals[action_val_indx2+1].val = action_val_16_msb_for_flp_0[0];
                            action_vals[action_val_indx2+1].type = action_vals_lcl[action_val_indx].type;
                            action_val_indx2++; /*two actions is added*/
                        }
                    }
                }
                LOG_DEBUG(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "   "
                                      "Action Value to buffer: %d, "
                                      "Action type:%s, "
                                      "Action val:%d \n\r"), direction, 
                           SOC_PPC_FP_ACTION_TYPE_to_string(action_vals_lcl[action_val_indx].type), 
                           action_vals_lcl[action_val_indx].val));
                action_val_indx2 ++;
                action_found = 1;
                break;
            }
        }
        /* if action  Value is not exist in DB action Error */
        if(!action_found) {
          LOG_ERROR(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "Unit %d DB id %d Action Type %s : action Value type does not exist in DB action.\n\r"),
                     unit, db_id, SOC_PPC_FP_ACTION_TYPE_to_string(action_vals_lcl[action_val_indx].type)));
            SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR, 101, exit);
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_value_buffer_mapping()", db_id, 0);
}



/* 
 * Insert the Action values in the correct buffer for ex. for the TCAM Action table
 */
uint32
  arad_pp_fp_action_value_to_buffer(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_VAL     action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX],
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_OUT  uint32                   buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE],
      SOC_SAND_OUT  uint32                   *buffer_size
  )
{
    uint32
        fld_val,
        key_bitmap_constraint_cascaded,
        action_indx,
        action_size,
        action_lsb_egress,
      res = SOC_SAND_OK;
    SOC_PPC_FP_ACTION_VAL     
        action_vals_lcl[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    SOC_PPC_FP_DATABASE_STAGE
        stage; 
    SOC_PPC_FP_DATABASE_INFO
      fp_database_info; 
    uint8
        egress_action_ndx,
        key_ndx,
        is_key_fixed;
    SOC_PPC_FP_ACTION_TYPE
        action_egress_disabled[] = {SOC_PPC_FP_ACTION_TYPE_ACE_POINTER, SOC_PPC_FP_ACTION_TYPE_EGR_OFP};
    uint8 is_vt_match_ipv4 = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Init all the buffer to 0: no action done */
    sal_memset(buffer,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE);
    sal_memcpy(action_vals_lcl, action_vals, SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX * sizeof(SOC_PPC_FP_ACTION_VAL));

    /* Get the correct stage */
    res = arad_pp_fp_db_stage_get(
              unit,
              db_id,
              &stage
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    /* 
     * Special cases: 
     * - at egress,
     *    Disable all actions specified on action_egress_disabled[]. They are 'destination'-related actions.
     *    For Arad: Do this by by setting all ones. 
     *    For Jericho, set all-ones on actions that do not have 'valid' bit and set all-zero for
     *      actions that do have a 'valid' bit.
     *    Note that, on following steps (See arad_pp_fp_action_value_buffer_mapping() below), the
     *    actual values of the actions will be written over the fields that are now marked 'invalid'.
     *    This way, if any of the actions is not on input list (action_vals[]) then it will be
     *    marked 'invalid' and will not 'collide' with the others. Apparently, there is the hidden
     *    assumption here that only one such action is on input list.
     *    
     * - at ingress, build the correct format of the Cascaded action 
     */
    if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        for (egress_action_ndx = 0; egress_action_ndx < sizeof(action_egress_disabled) / sizeof(SOC_PPC_FP_ACTION_TYPE); egress_action_ndx++) {
            /* The buffer of the Destination and ACE-Pointer must be set to -1 to be disabled */
            res = arad_pmf_db_fes_action_size_get_unsafe(
                    unit,
                    action_egress_disabled[egress_action_ndx],
                    SOC_PPC_FP_DATABASE_STAGE_EGRESS,
                    &action_size,
                    &action_lsb_egress
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            if (SOC_IS_JERICHO(unit) && (action_egress_disabled[egress_action_ndx] == SOC_PPC_FP_ACTION_TYPE_EGR_OFP)) {
                fld_val = (((1 << action_size) - 1) & ~1);
            }
            else {
                fld_val = (1 << action_size) - 1;  /* Destination disabled */
            }
            SHR_BITCOPY_RANGE(buffer, action_lsb_egress, &fld_val, 0, action_size); 
        }
#if (0)
/* { */
        /*
         * This code is taken out since ACE_TYPE is not hard coded any more
         * but, rather, indirectly controlled by the user (which specifies the
         * elements that are activated on PPDB_A_TCAM_ACTION table.
         */
        if (SOC_IS_JERICHO(unit))
        {
            res = arad_pmf_db_fes_action_size_get_unsafe(
                    unit,
                    SOC_PPC_FP_ACTION_TYPE_ACE_TYPE,
                    SOC_PPC_FP_DATABASE_STAGE_EGRESS,
                    &action_size,
                    &action_lsb_egress
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            fld_val = 0x1; /* ACE-TYPE value = stat only*/
            SHR_BITCOPY_RANGE(buffer, action_lsb_egress, &fld_val, 0, action_size); 
        }
/* } */
#endif
    }
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
                 unit,
                 stage,
                 db_id,
                 &fp_database_info
        );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        if (fp_database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_DBAL )
        {
             is_vt_match_ipv4 = TRUE;
        }
        if (!is_vt_match_ipv4) {
            for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (fp_database_info.action_types[action_indx] != SOC_PPC_FP_ACTION_TYPE_INVALID); ++action_indx) {
                if ( (action_vals_lcl[action_indx].type == SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY) &&
                     !((fp_database_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE && fp_database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_KAPS ) ) ) {
                    /* 
                     * Get the coupled Database ID Key-Id - uniform 
                     * for all the PMF-Programs 
                     */ 

                    res = arad_pp_fp_key_alloc_key_cascaded_key_get(
                            unit,
                            fp_database_info.cascaded_coupled_db_id,
                            &is_key_fixed,
                            &key_ndx,
                            &key_bitmap_constraint_cascaded
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
                    if (!is_key_fixed) {
                      LOG_ERROR(BSL_LS_SOC_FP,
                                (BSL_META_U(unit,
                                            "   Error in entry add: "
                                            "For database %d, stage %s, the database is indicated to be cascaded with DB-Id %d."
                                            "This latest Database is not set as cascaded. \n\r"),
                                 db_id, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), fp_database_info.cascaded_coupled_db_id));
                        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 50, exit); /* must have active program */
                    }

                    /* 
                     * Encoding of the Cascaded action: 
                     * bits 19:4 for the action value itself 
                     * bits 3:2 to indicate the destination key 
                     * bits 1:0 for the operation type (0 - none, 1 - and, 2 - or, 3 - replace) 
                     */
                    /* Cascaded has changed starting JER+ up, we leave value as is */
                    if (!SOC_IS_JERICHO_PLUS(unit)) {
                        action_vals_lcl[action_indx].val = action_vals_lcl[action_indx].val << 4;
                        action_vals_lcl[action_indx].val |= key_ndx << 2;
                        action_vals_lcl[action_indx].val |= 2; /* OR the result - the LSBs are 0 there, the MSBs are 0 here */
                    }
                }
            }
        }
    }

    res = arad_pp_fp_action_value_buffer_mapping(
            unit,
            db_id,
            ARAD_PP_FP_ACTION_BUFFER_DIRECTION_ACTION_TO_BUFFER,
            action_vals_lcl,
            buffer,
            buffer_size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_value_to_buffer()", db_id, 0);
}

/* 
 * Retrieve the Action values from a buffer (the TCAM Action table value)
 */
uint32
  arad_pp_fp_action_buffer_to_value(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  uint32                    db_id,
      SOC_SAND_IN  uint32                   buffer[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE],
      SOC_SAND_OUT  SOC_PPC_FP_ACTION_VAL     action_vals[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX]
  )
{
    uint32
        action_indx,
      res = SOC_SAND_OK;
    uint32                   
        buffer_lcl[ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE];
    SOC_PPC_FP_DATABASE_STAGE
        stage; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Get the correct stage */
    res = arad_pp_fp_db_stage_get(
              unit,
              db_id,
              &stage
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    /* Init all the action_vals to Invalid */
    for(action_indx = 0; action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_indx) {
        SOC_PPC_FP_ACTION_VAL_clear(&action_vals[action_indx]);
    }
    sal_memcpy(buffer_lcl, buffer, ARAD_PP_FP_TCAM_ACTION_BUFFER_SIZE * sizeof(uint32));

    res = arad_pp_fp_action_value_buffer_mapping(
            unit,
            db_id,
            ARAD_PP_FP_ACTION_BUFFER_DIRECTION_BUFFER_TO_ACTION,
            action_vals,
            buffer_lcl,
            NULL
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Special processing for the Change-Key action to hide the special encoding */
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) {
        for(action_indx = 0;  (action_indx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX) && (action_vals[action_indx].type != SOC_PPC_FP_ACTION_TYPE_INVALID); ++action_indx) {
            if (action_vals[action_indx].type == SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY) {
               /* 
                * Encoding of the Cascaded action: 
                * bits 19:4 for the action value itself 
                */
               action_vals[action_indx].val = action_vals[action_indx].val >> 4;
           }
       }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_buffer_to_value()", db_id, 0);
}


/* Direct extraction legacy code */
/*
 *  Return the Action output size (number of
 *  significant bits)
 */
uint32
  arad_pp_fp_action_type_max_size_get(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE fp_action_type,
    SOC_SAND_OUT uint32            *action_size_in_bits,
    SOC_SAND_OUT uint32            *action_size_in_bits_in_fem
  )
{
  uint32
      action_lsb_egress,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_ACTION_TYPE_MAX_SIZE_GET);

  res = arad_pmf_db_fes_action_size_get_unsafe(
            unit,
            fp_action_type,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, /* only ingress PMF has FEMs */
            action_size_in_bits,
            &action_lsb_egress
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *action_size_in_bits_in_fem = *action_size_in_bits;
  /* Special case for Change Key where the LSB are not encoded by the user */
  if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(fp_action_type))
  {
    *action_size_in_bits_in_fem = *action_size_in_bits + ARAD_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
  }

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_type_max_size_get()", 0, 0);
}

/* 
 * Get the location inside the 32b Direct extraction key 
 * of a specific field and its length 
 * Assumption: no field is split into 2 Copy Engines (of 16b) 
 */
uint32
  arad_pp_fp_qual_lsb_and_length_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   db_id_ndx,
    SOC_SAND_IN  uint8                    used_kaps,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE     qual_type,
    SOC_SAND_OUT uint32                   *qual_lsb,
    SOC_SAND_OUT uint32                   *qual_length_no_padding
  )
{
  uint32
    qual_lsb_lcl = 0,
      exist_progs[1],
      db_prog,
      prog_result,
      qual_type_ndx,
      ce_indx,
    res;
  uint32
    qual_size_in_bits_no_padding = 0;
  SOC_PPC_FP_DATABASE_STAGE
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; /* Single stage with FEMs */
  ARAD_PP_FP_KEY_DP_PROG_INFO
      db_prog_info;
  ARAD_PMF_CE
    sw_db_ce;
  /* coverity[stack_use_overflow:FALSE] */
  SOC_PPC_FP_DATABASE_INFO
    database_info;
  ARAD_PMF_CE_IRPP_QUALIFIER_INFO
    qual_info;
  uint8
      is_equal = FALSE,
      is_found,
      is_slb_hash_in_quals = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_QUAL_LSB_AND_LENGTH_GET);

  /*
   * Get the DB info
   */
  SOC_PPC_FP_DATABASE_INFO_clear(&database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  
  if (SOC_IS_ARADPLUS(unit)) {
      if ((database_info.flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_IS_EQUAL_MSB)
          && (qual_type == SOC_PPC_FP_QUAL_IS_EQUAL))
      {
          is_equal = TRUE;
      }
  }

  /* get relevant programs for the DB */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            stage,
            db_id_ndx,
            0,
            exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  db_prog = 0;
  ARAD_PP_FP_KEY_FIRST_SET_BIT(exist_progs,db_prog,ARAD_PMF_LOW_LEVEL_NOF_PROGS,ARAD_PMF_LOW_LEVEL_NOF_PROGS,FALSE,prog_result);
  if(prog_result == 0) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 20, exit); /* must have active program */
  }

  /* get CE used for this DB */
  res = arad_pp_fp_db_prog_info_get(
          unit,
          stage,
          db_id_ndx,
          db_prog,
          &db_prog_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_fp_is_qual_identical(unit, qual_type, SOC_PPC_FP_QUAL_KEY_AFTER_HASHING, &is_slb_hash_in_quals);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 151, exit);

  is_found = FALSE;
  *qual_lsb = 0;
  *qual_length_no_padding = 0;

  if (SOC_IS_ARADPLUS(unit)
      && is_equal) 
  {
      /* No need to look for CE for this qualifier, since
       * it's not a part of the key construction, but overrides 
       * whatever is located in specific location (key[159:155] = {1'bmatch_all, 4'bmatch[3:0]}) 
       * of the direct extraction key.  
       */
      *qual_lsb = ARAD_PP_FP_TCAM_KEY_SIZE - 5;
      *qual_length_no_padding = 4;
  }
  else if (is_slb_hash_in_quals == TRUE) 
  {
      if (SOC_IS_JERICHO_PLUS(unit)) {
          /* processing for updating second key when contain hash key and cascaded key
           * Cascade key loacted close to hash key on lower direction: XXX--hash_key--cascaded_key
           * If both key exist, only [80-cascade key length] lsb of hash key is valid.
           */
          uint32 slb_hash_qual_lsb = 0;
          uint32 slb_hash_qual_nof_bits = 0;
          uint32 cascaded_key_qual_nof_bits = 0;

          for (qual_type_ndx = 0; qual_type_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_type_ndx++) {
              if (database_info.qual_types[qual_type_ndx] == qual_type) { 
                  res = arad_pmf_ce_internal_field_info_find(unit, qual_type, stage, 1, &is_found, &qual_info); 
                  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
                  if (!is_found) {
                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 46, exit);
                  }
                  slb_hash_qual_lsb = (db_prog_info.alloc_place == ARAD_PP_FP_KEY_CE_LOW) ? 0 : ARAD_PP_FP_TCAM_HALF_KEY_SIZE;
                  slb_hash_qual_nof_bits = qual_info.info.qual_nof_bits;
                  break;
              }
          } 

          if (qual_type_ndx >= SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
              LOG_ERROR(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "Unit %d DB id %d Program %d Cycle %d Qual Type %s was not found.\n\r"),
                         unit, db_id_ndx, db_prog, db_prog_info.cycle, SOC_PPC_FP_QUAL_TYPE_to_string(qual_type)));
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 40, exit);
          }
          
          for (qual_type_ndx = 0; qual_type_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_type_ndx++) { 
              res = arad_pp_fp_is_qual_identical(unit, database_info.qual_types[qual_type_ndx], SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED, &is_found);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 47, exit); 
              if (is_found) { 
                  res = arad_pmf_ce_internal_field_info_find(unit, database_info.qual_types[qual_type_ndx], stage, 1, &is_found, &qual_info); 
                  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit); 
                  if (!is_found) { 
                      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 49, exit);
                  }
                  cascaded_key_qual_nof_bits = qual_info.info.qual_nof_bits;
                  break;
              }
          }

          *qual_lsb = slb_hash_qual_lsb + cascaded_key_qual_nof_bits;
          *qual_length_no_padding = slb_hash_qual_nof_bits;          
      } else {
          for (qual_type_ndx = 0; qual_type_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_type_ndx++) {
              if (database_info.qual_types[qual_type_ndx] == qual_type) {
                  break;
              }
          }

          if (qual_type_ndx >= SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
              LOG_ERROR(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "Unit %d DB id %d Program %d Cycle %d Qual Type %s was not found.\n\r"),
                         unit, db_id_ndx, db_prog, db_prog_info.cycle, SOC_PPC_FP_QUAL_TYPE_to_string(qual_type)));
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 40, exit);
          }

          res = arad_pmf_ce_internal_field_info_find(unit, qual_type, stage, 1, &is_found, &qual_info);
          SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

          if (is_found) {
              *qual_lsb = (db_prog_info.alloc_place == ARAD_PP_FP_KEY_CE_HIGH)?0:ARAD_PP_FP_TCAM_HALF_KEY_SIZE;
              *qual_length_no_padding = qual_info.info.qual_nof_bits;
          }
      }

  }
  else 
  {
      
          /* for CE, get which bits of qualifier it writes and where */
          for(ce_indx = 0; ce_indx < db_prog_info.nof_ces; ++ce_indx) 
          {
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_ce.get(
                  unit,
                  stage,
                  db_prog,
                  db_prog_info.cycle,
                  db_prog_info.ces[db_prog_info.nof_ces - ce_indx - 1], /* Start by the highest CE-ID */
                  &sw_db_ce
                );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

            if (sw_db_ce.qual_type == qual_type)
            {
                /*
                * Get the qualifier size without padding.
                * Special case:
                *     For QUX large direct lookup, in case qualifier is split into two CEs, only the CE in lower bits of the key
                *     should be considered when calculating qual_lsb
                */
                if (!is_found) {
                    qual_lsb_lcl += sw_db_ce.lsb;
                }
                is_found = TRUE;
                if ((used_kaps == TRUE) && (ARAD_PP_FP_DIRECT_TABLE_KAPS_KEY_LENGTH > (SOC_IS_QUX(unit) ? SOC_DPP_DEFS_GET(unit, field_large_direct_lu_key_min_length) : 16))) {
                    qual_size_in_bits_no_padding += (sw_db_ce.msb - sw_db_ce.lsb + 1);
                    continue;
                } else {
                    qual_size_in_bits_no_padding = (sw_db_ce.msb - sw_db_ce.lsb + 1); /* Number of intersting bits in the CE */
                    break;
                }
            } else if (is_found == TRUE) {
                continue;
            }

            /* If not found, add the size of the Copy Engine to the qualifier */
            qual_lsb_lcl += (sw_db_ce.msb + 1);
          }
          if (!SOC_IS_JERICHO_PLUS(unit))
          {
              if (!is_found) {
                LOG_ERROR(BSL_LS_SOC_FP,
                          (BSL_META_U(unit,
                                      "Unit %d DB id %d Program %d Cycle %d Qual Type %s was not found.\n\r"),
                           unit, db_id_ndx, db_prog, db_prog_info.cycle, SOC_PPC_FP_QUAL_TYPE_to_string(qual_type)));
                  SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 50, exit);
              }
          }
          *qual_lsb = qual_lsb_lcl;
          *qual_length_no_padding = qual_size_in_bits_no_padding;
      
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_lsb_and_length_get()", 0, 0);
}



/* 
 * Given the Database and a specific bit index in the key, 
 * retrieve the qualifier type and the Qual LSB 
 */
    uint32
      arad_pp_fp_qual_type_and_local_lsb_get(
        SOC_SAND_IN  int                  unit,
        SOC_SAND_IN  uint32                   bit_ndx,
        SOC_SAND_IN  uint32                   db_id_ndx,
        SOC_SAND_OUT SOC_PPC_FP_QUAL_TYPE         *qual_type,
        SOC_SAND_OUT uint32                   *qual_lsb
      )
{
  uint32
      qual_indx,
      qual_length_no_padding,
      key_lsb,
    res;
  /* coverity[stack_use_overflow:FALSE] */
  SOC_PPC_FP_DATABASE_INFO
    database_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_QUAL_TYPE_AND_LOCAL_LSB_GET);

  *qual_type = BCM_FIELD_ENTRY_INVALID;
  *qual_lsb = 0;

  /*
   * Get the DB info
   */
  SOC_PPC_FP_DATABASE_INFO_clear(&database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          db_id_ndx,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for(qual_indx = 0;  (qual_indx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) 
       && (database_info.qual_types[qual_indx]!= BCM_FIELD_ENTRY_INVALID); ++qual_indx) {
        res = arad_pp_fp_qual_lsb_and_length_get(
                unit,
                db_id_ndx,
                FALSE, 
                database_info.qual_types[qual_indx],
                &key_lsb, /* LSB in the Key with 1st relevant bit for this qual */
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        if ((bit_ndx >= key_lsb) && (bit_ndx < key_lsb + qual_length_no_padding)) {
            *qual_type = database_info.qual_types[qual_indx];
            *qual_lsb = bit_ndx - key_lsb; 
            break;
        }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_qual_type_and_local_lsb_get()", 0, 0);
}

uint32
  arad_pp_fp_action_lsb_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE       action_type,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_INFO     *fp_database_info,
    SOC_SAND_OUT uint32                   *action_lsb
  )
{
  uint32
    action_size_in_bits,
    action_size_in_bits_in_fem,
    action_lsb_lcl = 0,
    res;
  uint32
    action_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_ACTION_LSB_GET);

  for (action_type_ndx = 0; action_type_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; ++action_type_ndx)
  {
    if (fp_database_info->action_types[action_type_ndx] != action_type)
    {
      /*
       * Get the action size
       */
      res = arad_pp_fp_action_type_max_size_get(
              unit,
              fp_database_info->action_types[action_type_ndx],
              &action_size_in_bits,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      action_lsb_lcl += action_size_in_bits;
    }
    else
    {
      break;
    }
  }

  *action_lsb = action_lsb_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_lsb_get()", 0, 0);
}


uint32
  arad_pp_fp_fem_duplicate(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                     is_for_tm,
    SOC_SAND_IN  uint32                    fem_id_orig,
    SOC_SAND_IN  uint32                    fem_id_dest,
    SOC_SAND_IN  uint8                     fem_pgm_id
  )
{
  uint32
      pmf_pgm_ndx,
      pmf_pgm_ndx_min,
      pmf_pgm_ndx_max,
    res = SOC_SAND_OK;
  ARAD_PMF_FEM_NDX
    fem_ndx_orig,
    fem_ndx_dest;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    action_format_info;
  uint32
    selected_bits_ndx;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    action_format_map_info;
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    selected_bits_info;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  SOC_PPC_FP_FEM_ENTRY
    fem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_DUPLICATE);
  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "Duplicate FEM %d to FEM %d \n\r"), 
             fem_id_orig, fem_id_dest));


  /*
   * Duplicate in the counter-packet-flow order:
   * Action (Ethernet: #2), Action map, 4b select and then input
   */
  ARAD_PMF_FEM_NDX_clear(&fem_ndx_orig);
  fem_ndx_orig.id = fem_id_orig;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&action_format_info);
  res = arad_pmf_fem_action_format_get_unsafe(
          unit,
          &fem_ndx_orig,
          fem_pgm_id, 
          &action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ARAD_PMF_FEM_NDX_clear(&fem_ndx_dest);
  fem_ndx_dest.id = fem_id_dest;
  res = arad_pmf_fem_action_format_set_unsafe(
          unit,
          &fem_ndx_dest,
          fem_pgm_id, 
          &action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
  {
    ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&action_format_map_info);
    res = arad_pmf_fem_action_format_map_get_unsafe(
            unit,
            &fem_ndx_orig,
            fem_pgm_id,
            selected_bits_ndx,
            &action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_pmf_fem_action_format_map_set_unsafe(
            unit,
            &fem_ndx_dest,
            fem_pgm_id,
            selected_bits_ndx,
            &action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&selected_bits_info);
  res = arad_pmf_fem_select_bits_get_unsafe(
          unit,
          &fem_ndx_orig,
          fem_pgm_id,
          &selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = arad_pmf_fem_select_bits_set_unsafe(
          unit,
          &fem_ndx_dest,
          fem_pgm_id,
          &selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = arad_pmf_prog_select_pmf_pgm_borders_get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            is_for_tm, 
            &pmf_pgm_ndx_min,
            &pmf_pgm_ndx_max
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  for (pmf_pgm_ndx = pmf_pgm_ndx_min; pmf_pgm_ndx < pmf_pgm_ndx_max; ++pmf_pgm_ndx)
  {
    ARAD_PMF_FEM_INPUT_INFO_clear(&fem_input_info);
    res = arad_pmf_db_fem_input_get_unsafe(
            unit,
            pmf_pgm_ndx,
            FALSE, /* is_fes */
            fem_id_orig,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = arad_pmf_db_fem_input_set_unsafe(
            unit,
            pmf_pgm_ndx,
            FALSE, /* is_fes */
            fem_id_dest,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

  /* 
   * Replicate the SW DB and cancel the original FEM
   */
  SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_orig,
          &fem_entry
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 90, exit);

  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_dest,
          &fem_entry
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);

  SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_orig,
          &fem_entry
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_duplicate()", 0, 0);
}



/* 
 * Check if entry_ndx_to_insert importance vs entry_ndx_to_compare 
 * FEM is blocking if  entry_ndx_to_compare is more important than 
 *  entry_ndx_to_insert
 */
STATIC
  uint32
    arad_pp_fp_fem_is_fem_blocking_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY     *entry_ndx_to_insert, 
      SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY     *entry_ndx_to_compare,
      SOC_SAND_OUT uint8                    *is_fem_blocking
    )
{
  uint32
    db_strength_strong,
    db_strength_weak,
    entry_strength_strong,
    entry_strength_weak,
    res = SOC_SAND_OK;
  uint8
    forbidden_fem_cycle2_found = FALSE;
  uint8 is_qax_drop_dest = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_FEM_BLOCKING_GET);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx_to_insert);
  SOC_SAND_CHECK_NULL_INPUT(entry_ndx_to_compare);

    if (SOC_IS_JERICHO_PLUS(unit)) {
      if ((entry_ndx_to_insert->action_type[0] == SOC_PPC_FP_ACTION_TYPE_DEST_DROP)
          && (entry_ndx_to_compare->action_type[0] == SOC_PPC_FP_ACTION_TYPE_DEST)) {
          is_qax_drop_dest = TRUE;
      }

      if ((entry_ndx_to_insert->action_type[0] == SOC_PPC_FP_ACTION_TYPE_DEST)
          && (entry_ndx_to_compare->action_type[0] == SOC_PPC_FP_ACTION_TYPE_DEST_DROP)) {
          is_qax_drop_dest = TRUE;
      }  
    }

  /*
   * Verify if the second FEM info is  stronger
   */
  if ((entry_ndx_to_insert->action_type[0] == entry_ndx_to_compare->action_type[0]) || is_qax_drop_dest)
  {
    /*
     * Compare the DB strengths
     */
    db_strength_strong = entry_ndx_to_insert->db_strength;
    db_strength_weak = entry_ndx_to_compare->db_strength;

    /* The strongest strength is the closest to zero */
    if (db_strength_weak > db_strength_strong)
    {
      forbidden_fem_cycle2_found = TRUE;
    }
    else if ((db_strength_strong == db_strength_weak)
              && (entry_ndx_to_insert->db_id == entry_ndx_to_compare->db_id)
              && (entry_ndx_to_insert->is_for_entry == TRUE))
    {
      /*
       * Compare the Entry strengths
       */
      entry_strength_strong = entry_ndx_to_insert->entry_strength;
      entry_strength_weak = entry_ndx_to_compare->entry_strength;

      /* The strongest strength is the closest to zero */
      if (entry_strength_weak > entry_strength_strong)
      {
        forbidden_fem_cycle2_found = TRUE;
      }
    }
  }

  *is_fem_blocking = forbidden_fem_cycle2_found;

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_fem_blocking_get()", 0, 0);
}

/*
 *  Conversion FP to PMF actions
 */
uint32
  arad_pp_fp_action_type_to_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE    fp_action_type,
    SOC_SAND_OUT uint32  *pmf_fem_action_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_ACTION_TYPE_TO_PMF_CONVERT);

  /* 1x1 mapping in Arad, conversion in lower level */
    *pmf_fem_action_type = fp_action_type;

    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_type_to_pmf_convert()", 0, 0);
}

uint32
  arad_pp_fp_action_type_from_pmf_convert(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  uint32               pmf_fem_action_type,
    SOC_SAND_OUT SOC_PPC_FP_ACTION_TYPE    *fp_action_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FP_ACTION_TYPE_FROM_PMF_CONVERT);

  /* 1x1 mapping in Arad, conversion in lower level */
  *fp_action_type = pmf_fem_action_type;

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_action_type_from_pmf_convert()", 0, 0);
}





STATIC
  uint32
    arad_pp_fp_fem_set(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  ARAD_PMF_FEM_NDX             *fem,
      SOC_SAND_IN  SOC_PPC_FP_ACTION_TYPE        action_type,
      SOC_SAND_IN  uint32                    extraction_lsb,
      SOC_SAND_IN  uint32                    nof_bits,
      SOC_SAND_IN  uint32                    base_value,
      SOC_SAND_IN  uint8                  fem_pgm_id
    )
{
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  uint32
    res,
    bit_loc_lsb,
    bit_ndx,
    bit_loc_ndx,
    selected_bits_ndx;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_SET);

  
  /*
   *    Selected-bits: no influence
   */
  ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
  fem_selected_bits_info.sel_bit_msb = 3;
  res = arad_pmf_fem_select_bits_set_unsafe(
          unit,
          fem,
          fem_pgm_id,
          &fem_selected_bits_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
   *    Maps to Action 2
   */
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
  fem_action_format_map_info.map_data = 0;
  fem_action_format_map_info.action_fomat_id = fem_pgm_id;
  fem_action_format_map_info.is_action_valid = TRUE;
  for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
  {
    res = arad_pmf_fem_action_format_map_set_unsafe(
            unit,
            fem,
            fem_pgm_id,
            selected_bits_ndx,
            &fem_action_format_map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

  /*
   *    Set the action
   */
  ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
  res = arad_pp_fp_action_type_to_pmf_convert(
          unit,
          action_type,
          &(fem_action_format_info.type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  fem_action_format_info.size = nof_bits;
  fem_action_format_info.base_value = base_value;

  /*
   *    1st stage: only one-to-one mapping from the lsb
   */
  bit_loc_lsb = 0;


  bit_ndx = bit_loc_lsb;
  for (bit_loc_ndx = 0; bit_loc_ndx < nof_bits - bit_loc_lsb; ++bit_loc_ndx)
  {
    fem_action_format_info.bit_loc[bit_ndx].type = ARAD_PMF_FEM_BIT_LOC_TYPE_KEY;
    fem_action_format_info.bit_loc[bit_ndx].val = extraction_lsb + bit_loc_ndx;

    bit_ndx ++;
  }

  res = arad_pmf_fem_action_format_set_unsafe(
          unit,
          fem,
          fem_pgm_id,
          &fem_action_format_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_set()", 0, 0);
}


uint32
  arad_pp_fp_fem_configure(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_VAL            *qual_info,
    SOC_SAND_IN  uint8                           fem_pgm_id,
    SOC_SAND_IN uint32                          fem_bypass_pgm_bmp
  )
{
  uint32
    selected_bits_ndx_msb,
    selected_bits_ndx_lsb,
    select_msb,
    select_lsb,
    expected_select,
    selected_value = 0,
    selected_mask = 0,
    get_select,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    qual_length_no_padding,
    key_select,
    action_lsb,
    qual_lsb,
    extraction_lsb = ARAD_PP_FP_TCAM_KEY_SIZE,
    extraction_msb = ARAD_PP_FP_TCAM_KEY_SIZE,
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_INFO
    *database_info = NULL;
  ARAD_PMF_FEM_NDX
    fem_ndx;
  uint32
    bit_loc_ndx,
    bit_ndx,
    fld_ndx,
    pmf_pgm_ndx,
    exist_progs,
    selected_bits_ndx;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  int32
    u64_ndx;
  uint8
    fem_pgm_id_temp,
    is_qual_bit_valid,
    is_action_applied,
    use_kaps = 0,
    use_tcam = 0,
    use_key = 0;
  ARAD_PP_FP_KEY_DP_PROG_INFO
      *db_prog_info = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_CONFIGURE);

  /*
   * Get the DB info
   */
  ARAD_ALLOC(database_info, SOC_PPC_FP_DATABASE_INFO, 1, "arad_pp_fp_fem_configure.database_info");
  SOC_PPC_FP_DATABASE_INFO_clear(database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  /*
   * 2 cases: Direct Extraction entries or TCAM/Direct Table Database
   * Otherwise, return error
   * The configuration of the FEM must be anti-packet-flow
   */
  /*
   * Set all except FEM input
   */
  res = arad_pp_fp_action_lsb_get(
          unit,
          entry_ndx->action_type[0],
          database_info,
          &action_lsb
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_pp_fp_action_type_max_size_get(
          unit,
          entry_ndx->action_type[0],
          &action_size_in_bits_max,
          &action_size_in_bits_in_fem
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ARAD_PMF_FEM_NDX_clear(&fem_ndx);
  fem_ndx.id = fem_id_ndx;
  if (
      ((database_info->db_type == SOC_PPC_FP_DB_TYPE_TCAM)
        ||(database_info->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE))
      && (entry_ndx->is_for_entry == FALSE)
      )
  {
    res = arad_pp_fp_fem_set(
            unit,
            &fem_ndx,
            entry_ndx->action_type[0],
            action_lsb,
            action_size_in_bits_in_fem,
            0 /* Base value */,
            fem_pgm_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else if (entry_ndx->is_for_entry == TRUE)
  {
    /*
     *  Set the action according to the params
     */
    ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
    res = arad_pp_fp_action_type_to_pmf_convert(
            unit,
            entry_ndx->action_type[0],
            &(fem_action_format_info.type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    fem_action_format_info.size = action_size_in_bits_in_fem;
    fem_action_format_info.base_value = fem_info->base_val;

    /*
     *	Get the bit mapping
     */
    bit_ndx = 0;

    /* Determine key select for FEM:
    * 1. Check action extraction bits
    */
    for (fld_ndx = 0; fld_ndx < fem_info->nof_fields; ++fld_ndx)
    {
        qual_lsb = 0;
        
        if (fem_info->fld_ext[fld_ndx].type == SOC_PPC_FP_QUAL_IRPP_KAPS_PASS1_PAYLOAD) {
            use_kaps = 1;
            if (extraction_lsb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
                key_select = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID_USE_KAPS(fem_info->fld_ext[fld_ndx].fld_lsb);
                extraction_lsb = key_select * ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS_USE_KAPS;
            }
            else if (fem_info->fld_ext[fld_ndx].fld_lsb < extraction_lsb) {
                key_select = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_ID_USE_KAPS(fem_info->fld_ext[fld_ndx].fld_lsb);
                extraction_lsb = key_select * ARAD_PP_FP_FEM_ACTION_TABLE_WIDTH_IN_BITS_USE_KAPS;
            }

            if ((fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits) > extraction_msb) {
                extraction_msb = fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits;
            }
        }
        else if((fem_info->fld_ext[fld_ndx].type >= SOC_PPC_FP_QUAL_IRPP_TCAM_0_RESULT)
            && (fem_info->fld_ext[fld_ndx].type <= SOC_PPC_FP_QUAL_IRPP_TCAM_3_RESULT)) {
            use_tcam = 1;
            if (extraction_lsb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
                key_select = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(fem_info->fld_ext[fld_ndx].fld_lsb);
                extraction_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_FES_LSB(key_select);
            }
            else if (fem_info->fld_ext[fld_ndx].fld_lsb < extraction_lsb) {
                key_select = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY(fem_info->fld_ext[fld_ndx].fld_lsb);
                extraction_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_FES_LSB(key_select);
            }

            if ((fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits) > extraction_msb) {
                extraction_msb = fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits;
            }
        }
        else if (fem_info->fld_ext[fld_ndx].type != BCM_FIELD_ENTRY_INVALID) {
            use_key = 1;
            res = arad_pp_fp_qual_lsb_and_length_get(
                    unit,
                    entry_ndx->db_id,
                    FALSE,
                    fem_info->fld_ext[fld_ndx].type,
                    &qual_lsb, /* LSB in the Key with 1st relevant bit for this qual */
                    &qual_length_no_padding
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  
            if (fem_info->fld_ext[fld_ndx].nof_bits + fem_info->fld_ext[fld_ndx].fld_lsb > qual_length_no_padding) {
              LOG_ERROR(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "Unit %d Qualifier %s : not enough qualifier bits in the Key\n\r"
                                    "Field number of bits %d, Field lsb %d, Field length without padding %d.\n\r"),
                         unit, SOC_PPC_FP_QUAL_TYPE_to_string(fem_info->fld_ext[fld_ndx].type), fem_info->fld_ext[fld_ndx].nof_bits,
                         fem_info->fld_ext[fld_ndx].fld_lsb, qual_length_no_padding));
                SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 43, exit);
            }

            if (extraction_lsb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
                extraction_lsb = ((qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb)/ARAD_PMF_FEM_KEY_LSB_MULTIPLE)*ARAD_PMF_FEM_KEY_LSB_MULTIPLE;
                extraction_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(extraction_lsb);
                extraction_msb = qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits;
            }
            else if ((qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb) < extraction_lsb) {
                extraction_lsb = ((qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb)/ARAD_PMF_FEM_KEY_LSB_MULTIPLE)*ARAD_PMF_FEM_KEY_LSB_MULTIPLE;
            }

            if ((qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits) > extraction_msb) {
                extraction_msb = qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + fem_info->fld_ext[fld_ndx].nof_bits;
            }
        }
    }

    if ((use_kaps + use_tcam + use_key) > 1) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "FEM: trying to extract from different sources\n\r")));
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 44, exit);
    }

    /*Determine key select for FEM:
     * 2. Check qualifier bits
     * Set the 4b selection and the mapping according to the mask
     */
    /*
     *	Selected-bits: get the MSB of the four bits mask
     *  In practice, assumption the mask is only in the first uint32 of u64
     */
    /* Get the first valid bit from MSB */
    for (u64_ndx = ARAD_PP_FP_FEM_SAND_U64_NOF_BITS - 1; u64_ndx >= 0; u64_ndx--)
    {
      if (u64_ndx >= 32)
      {
        is_qual_bit_valid = SOC_SAND_GET_BIT(qual_info->is_valid.arr[1], (u64_ndx - 32));
      }
      else
      {
        is_qual_bit_valid = SOC_SAND_GET_BIT(qual_info->is_valid.arr[0], u64_ndx);
      }

      if (is_qual_bit_valid > 0)
      {
        break;
      }
    }

    ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
    fem_selected_bits_info.sel_bit_msb = 3;
    qual_lsb = 0;
    qual_length_no_padding = 0;
    if (is_qual_bit_valid) { /* Filter found, verify validity */
        res = arad_pp_fp_qual_lsb_and_length_get(
                unit,
                entry_ndx->db_id,
                FALSE,
                qual_info->type,
                &qual_lsb, /* LSB in the Key with 1st relevant bit for this qual */
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
        if (u64_ndx > qual_length_no_padding) {
            LOG_ERROR(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "Unit %d Qualifier %s : not enough qualifier bits in the Key\n\r"
                                  "Field number of bits %d, Field length without padding %d.\n\r"),
                       unit, SOC_PPC_FP_QUAL_TYPE_to_string(fem_info->fld_ext[fld_ndx].type), u64_ndx,
                       qual_length_no_padding));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 46, exit);
        }

        /* Special case if action extracts from constant only but has a valid qualifier */
        if (((use_kaps + use_tcam + use_key) == 0)
            || ((extraction_lsb > 0)
                && (((extraction_lsb > qual_lsb) && (u64_ndx<3))
                    || ((u64_ndx>=3) && (extraction_lsb > (qual_lsb+u64_ndx-3)))))){

            /* derive FEM key select from qualifier */
            extraction_lsb = (qual_lsb/ARAD_PMF_FEM_KEY_LSB_MULTIPLE)*ARAD_PMF_FEM_KEY_LSB_MULTIPLE;
            extraction_lsb = ARAD_PP_FP_FEM_ACTION_LSB_TO_KEY_LSB(extraction_lsb);
        }
        
        if ((qual_lsb + u64_ndx) >= (extraction_lsb + 32)) {
            LOG_ERROR(BSL_LS_SOC_FP,
                      (BSL_META_U(unit,
                                  "FEM : qualifier bits not in the selected key:\n\r"
                                  "    qual_lsb %d, u64_ndx %d, extraction_lsb %d\n\r"),
                                  qual_lsb, u64_ndx, extraction_lsb));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 47, exit);
        }
        
        fem_selected_bits_info.sel_bit_msb = SOC_SAND_MAX(qual_lsb + u64_ndx - extraction_lsb, 3);
    }

    if (extraction_lsb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
        extraction_lsb = 0;
    }

    if (extraction_msb >= ARAD_PP_FP_TCAM_KEY_SIZE) {
        extraction_msb = 0;
    }

    if (((use_kaps + use_tcam + use_key) != 0) && (extraction_msb > (extraction_lsb + 32))) {
        LOG_ERROR(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "FEM extracted bits could not be put in one key:\n\r"
                              "    extraction_msb %d, extraction_lsb %d\n\r"),
                   extraction_msb, extraction_lsb));
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_KEY_UNKNOWN_QUAL_ERR, 48, exit);
    }

    for (fld_ndx = 0; fld_ndx < fem_info->nof_fields; ++fld_ndx)
    {
        qual_lsb = 0;

        if (!((fem_info->fld_ext[fld_ndx].type >= SOC_PPC_FP_QUAL_IRPP_TCAM_0_RESULT) &&
            (fem_info->fld_ext[fld_ndx].type >= SOC_PPC_FP_QUAL_IRPP_KAPS_PASS1_PAYLOAD)) &&
            (fem_info->fld_ext[fld_ndx].type != BCM_FIELD_ENTRY_INVALID)) {
            res = arad_pp_fp_qual_lsb_and_length_get(
                    unit,
                    entry_ndx->db_id,
                    FALSE,
                    fem_info->fld_ext[fld_ndx].type,
                    &qual_lsb, /* LSB in the Key with 1st relevant bit for this qual */
                    &qual_length_no_padding
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);
        }

        for (bit_loc_ndx = 0; bit_loc_ndx < fem_info->fld_ext[fld_ndx].nof_bits; ++bit_loc_ndx)
        {
            /* Constant Value case - only with NOF for action type */
            if (fem_info->fld_ext[fld_ndx].type == BCM_FIELD_ENTRY_INVALID)
            {
              fem_action_format_info.bit_loc[bit_ndx].type = ARAD_PMF_FEM_BIT_LOC_TYPE_CST;
              fem_action_format_info.bit_loc[bit_ndx].val = SOC_SAND_GET_BIT(fem_info->fld_ext[fld_ndx].cst_val, bit_loc_ndx);
            }
            else {
              fem_action_format_info.bit_loc[bit_ndx].type = ARAD_PMF_FEM_BIT_LOC_TYPE_KEY;
              fem_action_format_info.bit_loc[bit_ndx].val = qual_lsb + fem_info->fld_ext[fld_ndx].fld_lsb + bit_loc_ndx;
              fem_action_format_info.bit_loc[bit_ndx].val -= extraction_lsb;
            }        
        
          bit_ndx ++;
        }
    }

    res = arad_pmf_fem_action_format_set_unsafe(
            unit,
            &fem_ndx,
            fem_pgm_id,
            &fem_action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


    res = arad_pmf_fem_select_bits_set_unsafe(
            unit,
            &fem_ndx,
            fem_pgm_id,
            &fem_selected_bits_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    /*
     *  Maps to Action 2 according to the mask
     */
    ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
    fem_action_format_map_info.map_data = 0;
    fem_action_format_map_info.action_fomat_id = fem_pgm_id;
    if (is_qual_bit_valid) {
        res = arad_pp_fp_qual_lsb_and_length_get(
                unit,
                entry_ndx->db_id,
                FALSE,
                qual_info->type,
                &qual_lsb, /* LSB in the Key with 1st relevant bit for this qual */
                &qual_length_no_padding
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

        /* See which bits to consider in the qualifier bitspace */
        select_msb = is_qual_bit_valid? u64_ndx : 0;
        select_lsb = (is_qual_bit_valid && (u64_ndx > 3))?(u64_ndx - 3) : 0;
        /* See which bits to consider in the selected_bits_ndx bitspace */
        if ((qual_lsb + u64_ndx - extraction_lsb) >= 3) {
            selected_bits_ndx_lsb = (u64_ndx >= 3)?0:(3-u64_ndx);
            selected_bits_ndx_msb = 3;
        }
        else {
            selected_bits_ndx_msb = qual_lsb + u64_ndx - extraction_lsb;
            selected_bits_ndx_lsb = qual_lsb - extraction_lsb;
        }

        SHR_BITCOPY_RANGE(&selected_value, 0, &qual_info->val.arr[0], select_lsb, select_msb-select_lsb+1);
        SHR_BITCOPY_RANGE(&selected_mask, 0, &qual_info->is_valid.arr[0], select_lsb, select_msb-select_lsb+1);
        expected_select = selected_value & selected_mask;
    }
    else {
        select_msb = 0;
        select_lsb = 0;
        selected_bits_ndx_msb = 0;
        selected_bits_ndx_lsb = 0;
        expected_select = 0;
    }
    for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
    {
      SHR_BITCOPY_RANGE(&selected_value, 0, &selected_bits_ndx, selected_bits_ndx_lsb, selected_bits_ndx_msb-selected_bits_ndx_lsb+1);
      get_select = selected_value & selected_mask;
      if ((expected_select == get_select) || (!is_qual_bit_valid))
      {
        is_action_applied = TRUE;
      }
      else
      {
        is_action_applied = FALSE;
      }

      fem_action_format_map_info.is_action_valid = is_action_applied;
      res = arad_pmf_fem_action_format_map_set_unsafe(
              unit,
              &fem_ndx,
              fem_pgm_id,
              selected_bits_ndx,
              &fem_action_format_map_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }

  }
  else if (
            (database_info->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION)
           && (entry_ndx->is_for_entry == FALSE)
          )
  {
    /* Skip this step */
    ARAD_PP_DO_NOTHING_AND_EXIT;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR, 100, exit);
  }

  /*
   * Set the input according to the Database type
   */
    /* get relevant programs for the DB */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            entry_ndx->db_id,
            0,
            &exist_progs
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);

  ARAD_ALLOC(db_prog_info, ARAD_PP_FP_KEY_DP_PROG_INFO, 1, "arad_pp_fp_fem_configure.db_prog_info");

  for (pmf_pgm_ndx = 0; pmf_pgm_ndx < SOC_DPP_DEFS_GET(unit, nof_ingress_pmf_programs); ++pmf_pgm_ndx)
  {
    if ((exist_progs & (1 << pmf_pgm_ndx)) == 0)
    {
      continue;
    }

    res = arad_pp_fp_fem_pgm_per_pmf_pgm_get(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,pmf_pgm_ndx,&fem_pgm_id_temp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

    if (fem_pgm_id_temp != fem_pgm_id) {
        continue;
    }

    ARAD_PMF_FEM_INPUT_INFO_clear(&fem_input_info);

    fem_input_info.db_id = SOC_PPC_FP_NOF_DBS;
    res = arad_pp_fp_fem_pgm_per_pmf_pgm_get(unit,SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                                         pmf_pgm_ndx,&fem_input_info.pgm_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 113, exit);

    sal_memset(db_prog_info, 0, sizeof(*db_prog_info));
    /* get CE used for this DB */
    res = arad_pp_fp_db_prog_info_get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            entry_ndx->db_id,
            pmf_pgm_ndx,
            db_prog_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

    fem_input_info.db_id = entry_ndx->db_id; /* No importance, just for prints */
    if (SOC_SAND_GET_BIT(fem_bypass_pgm_bmp, pmf_pgm_ndx) != 0){
        /* Bypass this program, set as NOP */
        fem_input_info.src_arad.is_nop = TRUE; 
    }                
    else if ((database_info->db_type == SOC_PPC_FP_DB_TYPE_TCAM)
        || (database_info->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_TABLE))
    {
        fem_input_info.src_arad.is_key_src = use_key; /* Form TCAM PDs */
        fem_input_info.src_arad.use_kaps = use_kaps;
        fem_input_info.src_arad.key_lsb = extraction_lsb;
        if (use_tcam) {
            if (extraction_lsb >= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE) {
                fem_input_info.src_arad.key_lsb -= ARAD_PP_FP_TCAM_ACTION_TABLE_WIDTH_DOUBLE;
                fem_input_info.src_arad.key_tcam_id = db_prog_info->key_id[1];
            }
            else {
                fem_input_info.src_arad.key_tcam_id = db_prog_info->key_id[0];
            }
        }
        fem_input_info.src_arad.lookup_cycle_id = db_prog_info->cycle;
    }
    else if (database_info->db_type == SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION)
    {
      fem_input_info.src_arad.is_key_src = TRUE; 
      fem_input_info.src_arad.key_tcam_id = db_prog_info->key_id[0];
      fem_input_info.src_arad.lookup_cycle_id = db_prog_info->cycle;

      if (database_info->flags & SOC_PPC_FP_DATABASE_INFO_FLAGS_IS_EQUAL_MSB) {
          /* In case this Database is for compare, take 32 MSB bits of the key */
          fem_input_info.src_arad.key_lsb = 128; 
          if (SOC_IS_JERICHO_PLUS(unit)) {
              /* Compare doesn't use key7 anymore starting Jericho_Plus */
              fem_input_info.src_arad.is_key_src = FALSE; 
              fem_input_info.src_arad.is_compare = TRUE; 
          }
      }
      else{
          fem_input_info.src_arad.key_lsb = (db_prog_info->alloc_place == ARAD_PP_FP_KEY_CE_HIGH)? ARAD_PMF_FEM_KEY_LSB_FIRST_IN_MSB : 0; 
          fem_input_info.src_arad.key_lsb += extraction_lsb; 
      }
    }
    else
    {
        fem_input_info.src_arad.is_nop = TRUE; 
    }
    res = arad_pmf_db_fem_input_set_unsafe(
            unit,
            pmf_pgm_ndx,
            FALSE, /* is_fes */
            fem_id_ndx,
            &fem_input_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);
  }

  /*
   * SW DB set
   */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_ndx,
          entry_ndx
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);


exit:
  ARAD_FREE(database_info);
  ARAD_FREE(db_prog_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_configure()", 0, 0);
}

/*
 * Only for Direct Extraction entries
 */
uint32
  arad_pp_fp_fem_configuration_de_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    fem_id_ndx,
    SOC_SAND_IN  uint32                    cycle_ndx,
    SOC_SAND_IN  uint8                     fem_pgm_id,
    SOC_SAND_OUT SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_OUT SOC_PPC_FP_DIR_EXTR_ACTION_VAL *fem_info,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_VAL            *qual_info
  )
{
  uint32
    bit_ndx_lsb,
    local_lsb,
    key_bit_last,
    key_select_lsb = 0,
    exist_progs,
    fem_id_absolute,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    res = SOC_SAND_OK;
  SOC_PPC_FP_DATABASE_INFO
    database_info;
  ARAD_PMF_FEM_NDX
    fem_ndx;
  uint32
    bit_ndx,
    pmf_pgm_ndx,
    pmf_pgm_ndx_min,
    pmf_pgm_ndx_max,
    selected_bits_ndx_mask_last,
    selected_bits_ndx_all,
    selected_bits_ndx;
  ARAD_PMF_FEM_ACTION_FORMAT_INFO
    fem_action_format_info;
  ARAD_PMF_FEM_SELECTED_BITS_INFO
    fem_selected_bits_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  ARAD_PP_FP_KEY_DP_PROG_INFO
    *db_prog_info = NULL;
  int32
    fld_ndx,
    u64_ndx,
    u64_ndx2;
  uint8
    is_for_tm,
    is_default_tm,
    has_a_valid_entry[2],
    is_one,
    mask[ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS],
    val_mask[ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS];
  ARAD_PMF_FEM_BIT_LOC_TYPE
    type_previous;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_CONFIGURATION_GET);

  fem_id_absolute = fem_id_ndx + (cycle_ndx * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP);
  /*
   * Get the DB info
   */
  SOC_PPC_FP_DATABASE_INFO_clear(&database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          &database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_fp_database_is_tm_get(unit, &database_info, &is_for_tm, &is_default_tm);
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  /* get relevant programs for the DB */
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, entry_ndx->db_id, 0, &exist_progs);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);  

  /* At this point, guarantee it can be inserted */
  res = arad_pmf_prog_select_pmf_pgm_borders_get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
            is_for_tm /* is_for_tm */, 
            &pmf_pgm_ndx_min,
            &pmf_pgm_ndx_max
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  for (pmf_pgm_ndx = pmf_pgm_ndx_min; pmf_pgm_ndx < pmf_pgm_ndx_max; ++pmf_pgm_ndx)
  {
      if (SOC_SAND_GET_BIT(exist_progs, pmf_pgm_ndx) != 0x1){
          /* PMF Program not used */
          continue;
      }

      res = arad_pmf_db_fem_input_get_unsafe(
              unit,
              pmf_pgm_ndx,
              FALSE, /* is_fes */
              fem_id_ndx,
              &fem_input_info
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);      

      if (!fem_input_info.src_arad.is_nop) {
          if (fem_input_info.src_arad.is_key_src) {
              key_select_lsb = fem_input_info.src_arad.key_lsb;

              ARAD_ALLOC(db_prog_info, ARAD_PP_FP_KEY_DP_PROG_INFO, 1, "arad_pp_fp_fem_configuration_de_get.db_prog_info");
              res = arad_pp_fp_db_prog_info_get(
                      unit,
                      SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                      entry_ndx->db_id,
                      pmf_pgm_ndx,
                      db_prog_info
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

              if ((db_prog_info->alloc_place == ARAD_PP_FP_KEY_CE_HIGH)
                && (key_select_lsb >= ARAD_PMF_FEM_KEY_LSB_FIRST_IN_MSB)){
                  key_select_lsb -= ARAD_PMF_FEM_KEY_LSB_FIRST_IN_MSB;
              }
          }
          break;
      }
  }  
 
  /*
   * SW DB get
   */
  ARAD_PMF_FEM_NDX_clear(&fem_ndx);
  fem_ndx.id = fem_id_absolute;
  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
          unit,
          SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
          fem_pgm_id,
          fem_id_absolute,
          entry_ndx
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if (
       (database_info.db_type == SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION)
       && (entry_ndx->is_for_entry == TRUE)
      )
  {
      res = arad_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[0],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);


    /*
     * Get the action
     */
    ARAD_PMF_FEM_ACTION_FORMAT_INFO_clear(&fem_action_format_info);
    res = arad_pmf_fem_action_format_get_unsafe(
            unit,
            &fem_ndx,
            fem_pgm_id,
            &fem_action_format_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_pp_fp_action_type_from_pmf_convert(
            unit,
            fem_action_format_info.type,
            &(fem_info->type)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    fem_info->base_val = fem_action_format_info.base_value;

    /*
     *  Get the bit mapping
     */
    type_previous = ARAD_NOF_PMF_FEM_BIT_LOC_TYPES;
    key_bit_last = 0;
    fld_ndx = -1; /* -1 Due to the ++ for each new field */

    /* Special case for the Change Key action */
    bit_ndx_lsb = 0;
    if (ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(fem_info->type))
    {
      bit_ndx_lsb = ARAD_PP_FP_BIT_LOC_LSB_CHANGE_KEY;
    }

    for (bit_ndx = bit_ndx_lsb; bit_ndx < action_size_in_bits_in_fem; ++bit_ndx)
    {
      if (fem_action_format_info.bit_loc[bit_ndx].type == ARAD_PMF_FEM_BIT_LOC_TYPE_KEY)
      {
        if (
            (type_previous == fem_action_format_info.bit_loc[bit_ndx].type)
            && (key_bit_last + 1 == fem_action_format_info.bit_loc[bit_ndx].val)
            && (fld_ndx >= 0)
           )
        {
          fem_info->fld_ext[fld_ndx].nof_bits ++;
          key_bit_last = fem_action_format_info.bit_loc[bit_ndx].val;
          continue;
        }
        else
        {
          /*
           * New Field
           */
          fld_ndx ++;
          type_previous = fem_action_format_info.bit_loc[bit_ndx].type;
          key_bit_last = fem_action_format_info.bit_loc[bit_ndx].val;
          res = arad_pp_fp_qual_type_and_local_lsb_get(
                  unit,
                  key_bit_last + key_select_lsb,
                  entry_ndx->db_id,
                  &(fem_info->fld_ext[fld_ndx].type),
                  &(fem_info->fld_ext[fld_ndx].fld_lsb)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
          fem_info->fld_ext[fld_ndx].nof_bits = 1;
        }
      }
      else if (!ARAD_PP_FP_FEM_IS_ACTION_NOT_REQUIRE_FEM(fem_info->type)) /* CST, none for Key change / staggered presel select */
      {
        if (type_previous == fem_action_format_info.bit_loc[bit_ndx].type && fld_ndx >= 0)
        {
          SOC_SAND_SET_BIT(fem_info->fld_ext[fld_ndx].cst_val, fem_action_format_info.bit_loc[bit_ndx].val, fem_info->fld_ext[fld_ndx].nof_bits);
          fem_info->fld_ext[fld_ndx].nof_bits ++;
          continue;
        }
        else
        {
          /*
           * New field
           */
          fld_ndx ++;
          SOC_SAND_SET_BIT(fem_info->fld_ext[fld_ndx].cst_val, fem_action_format_info.bit_loc[bit_ndx].val, 0);
          fem_info->fld_ext[fld_ndx].nof_bits = 1;
          type_previous = fem_action_format_info.bit_loc[bit_ndx].type;
        }
      }
    }
    fem_info->nof_fields = fld_ndx + 1;


    /*
     * Get the mask
     */
    ARAD_PMF_FEM_SELECTED_BITS_INFO_clear(&fem_selected_bits_info);
    res = arad_pmf_fem_select_bits_get_unsafe(
            unit,
            &fem_ndx,
            fem_pgm_id,
            &fem_selected_bits_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    /* 
     * Get the Value and Mask, of type 0x1x 
     * Assumption that the configuration of the 16 possible values 
     * are with this TCAM format 
     * Loop on the four bits, 
     *   loop on 0/1
     *     see if there is at least a valid value (xxx1 and xxx0)  
     */
    selected_bits_ndx_mask_last = 0;
    for (selected_bits_ndx = 0; selected_bits_ndx < ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS; ++selected_bits_ndx)
    {
        for (is_one = FALSE; is_one <= TRUE; ++is_one)
        {
            has_a_valid_entry[is_one] = FALSE;
            for (selected_bits_ndx_all = 0; (selected_bits_ndx_all <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX) && (!has_a_valid_entry[is_one]); ++selected_bits_ndx_all)
            {
                if (SOC_SAND_GET_BIT(selected_bits_ndx_all, selected_bits_ndx) != is_one) {
                    /* The intersting bit has not the correct value is_one */
                    continue;
                }

              ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
              res = arad_pmf_fem_action_format_map_get_unsafe(
                      unit,
                      &fem_ndx,
                      fem_pgm_id,
                      selected_bits_ndx_all,
                      &fem_action_format_map_info
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

              has_a_valid_entry[is_one] |= fem_action_format_map_info.is_action_valid;
            }
        }

      /* Update the mask */
      if (has_a_valid_entry[0] && has_a_valid_entry[1])
      {
        mask[selected_bits_ndx] = 0;
        val_mask[selected_bits_ndx] = 0;
      }
      else 
      {
        selected_bits_ndx_mask_last = selected_bits_ndx;
        mask[selected_bits_ndx] = 0x1;
        val_mask[selected_bits_ndx] = (has_a_valid_entry[1])? 1 : 0;
      }
    }

    res = arad_pp_fp_qual_type_and_local_lsb_get(
            unit,
            fem_selected_bits_info.sel_bit_msb - 3 + selected_bits_ndx_mask_last + key_select_lsb,
            entry_ndx->db_id,
            &(qual_info->type),
            &(local_lsb)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    /* Report the valid bits in the result */
    for (bit_ndx = ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS; bit_ndx > 0; bit_ndx--)
    {
      u64_ndx = local_lsb - (ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS - bit_ndx);
      u64_ndx2 = selected_bits_ndx_mask_last - (ARAD_PP_FP_FEM_MASK_LENGTH_IN_BITS - bit_ndx);
      SOC_SAND_SET_BIT(qual_info->is_valid.arr[0], mask[u64_ndx2], u64_ndx);
      SOC_SAND_SET_BIT(qual_info->val.arr[0], val_mask[u64_ndx2], u64_ndx);
      if (u64_ndx == 0)
      {
        break;
      }
    }
  }


exit:
  ARAD_FREE(db_prog_info);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_configuration_de_get()", 0, 0);
}



uint32 arad_pp_fp_fes_sole_invalid_next_idx_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint8                   pgm_id,
    SOC_SAND_IN uint32                  cycle_to_use,
    SOC_SAND_IN uint32                  nof_fes_group,
    SOC_SAND_IN ARAD_PMF_FES            *fes_info,
    SOC_SAND_OUT int                    *idx
)
{
    uint32 fes_idx;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_COMBINE_INVALID_GET);

    SOC_SAND_CHECK_NULL_INPUT(idx);
    SOC_SAND_CHECK_NULL_INPUT(fes_info);

    *idx = -1;
    for (fes_idx = (cycle_to_use*nof_fes_group); fes_idx < ARAD_PMF_LOW_LEVEL_NOF_FESS; fes_idx++) {
        if ((fes_info[fes_idx].action_type == SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT)
            && (fes_info[fes_idx].is_combined)
            && ((fes_info[fes_idx+1].action_type != SOC_PPC_FP_ACTION_TYPE_COUNTER_A) 
                && (fes_info[fes_idx+1].action_type != SOC_PPC_FP_ACTION_TYPE_SNP))) {
            *idx = fes_idx - cycle_to_use*nof_fes_group;
            break;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fes_sole_invalid_next_idx_get()", 0, 0);

}


uint32 arad_pp_fp_fes_combined_action_pre(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                  pmf_pgm_ndx,
    SOC_SAND_IN uint32                  flags,
    SOC_SAND_IN uint8                   idx_free,
    SOC_SAND_IN uint8                   idx_invalid_next,
    SOC_SAND_IN uint32                  cycle_to_use,
    SOC_SAND_IN uint32                  nof_fes_per_group,
    SOC_SAND_INOUT uint8                *is_fem_higher,
    SOC_SAND_INOUT uint8                *is_fem_lower,
    SOC_SAND_INOUT ARAD_PMF_FES         *fes_info
)
{
    uint32 res = SOC_SAND_OK;
    uint32 idx_move_start, idx_move_end;
    uint32 idx_move_from, idx_move_to;
    uint32 group_base;
    uint8 idx, value_high, value_low;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_COMBINE_PRE_PROCESS);

    SOC_SAND_CHECK_NULL_INPUT(is_fem_higher);
    SOC_SAND_CHECK_NULL_INPUT(is_fem_higher);
    SOC_SAND_CHECK_NULL_INPUT(fes_info);

    if (idx_invalid_next == (idx_free-1)) {
        ARAD_PP_DO_NOTHING_AND_EXIT;
    }

    if (idx_invalid_next >= idx_free) {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FP_DB_TYPE_OUT_OF_RANGE_ERR, 110, exit);
    }

    group_base = cycle_to_use * nof_fes_per_group;

    /*
     * N: fes for invalid_next, S: fes for first action of combined action
     *
     * |--------|---|---------|---|------------|---|------------|
     * | FESx   | S |   FESy  | N |   FESz     |   |   ...      |
     * |--------|---|---------|---|------------|---|------------|
     *                          |
     *                          |--------------->|
     * |--------|---|---------|---|------------|---|------------|
     * | FESx   | S |   FESy  |   |   FESz     | N |   ...      |
     * |--------|---|---------|---|------------|---|------------|
     *                                  |        |
     *                              |---|    |---|
     *                              |        |   
     *                              V        V
     * |--------|---|---------|------------|---|---|------------|
     * | FESx   | S |   FESy  |   FESz     | N |   |   ...      |
     * |--------|---|---------|------------|---|---|------------|
     */
     
    /* move invalid_next just before free */
    idx_move_from = idx_invalid_next + group_base;
    idx_move_to = idx_free + group_base;
    ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

    idx_move_start = idx_free + group_base;
    idx_move_end = idx_invalid_next + 1 + group_base;
    res = arad_pmf_db_fes_chunk_move_unsafe(
            unit,
            pmf_pgm_ndx,
            flags,
            idx_move_start,
            idx_move_end,
            -1,
            fes_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

    /* update priority array */
    value_high = is_fem_higher[idx_invalid_next];
    value_low = is_fem_lower[idx_invalid_next];
    for (idx = idx_invalid_next; idx < idx_free-1; idx++) {
        is_fem_higher[idx] = is_fem_higher[idx+1];
        is_fem_lower[idx] = is_fem_lower[idx+1];
    }
    is_fem_higher[idx_free-1] = value_high;
    is_fem_lower[idx_free-1] = value_low;
    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fes_combined_action_inpre()", 0, 0);
}


uint32 arad_pp_fp_fes_combined_action_process(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                  pmf_pgm_ndx,
    SOC_SAND_IN uint8                   type,
    SOC_SAND_IN uint32                  flags,
    SOC_SAND_IN uint8                   is_insert_combine,
    SOC_SAND_IN uint8                   is_compare_combine,
    SOC_SAND_IN uint32                  cycle_to_use,
    SOC_SAND_IN uint32                  nof_fes_per_group,
    SOC_SAND_IN uint8                   idx_compare,
    SOC_SAND_INOUT uint8                *idx_free,
    SOC_SAND_INOUT ARAD_PMF_FES         *fes_info
)
{
    uint32 res = SOC_SAND_OK;
    uint32 idx_move_start, idx_move_end;
    uint32 idx_move_from, idx_move_to;
    uint32 group_base;
    
    int move_step = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_COMBINE_PROCESS);

    SOC_SAND_CHECK_NULL_INPUT(idx_free);
    SOC_SAND_CHECK_NULL_INPUT(fes_info);

    group_base = cycle_to_use * nof_fes_per_group;
    
    if (is_insert_combine && !is_compare_combine) {
        /*
         * N: fes for invalid_next, S: fes for first action of combined action
         *         
         * |--------|---|---------|---|---|------------|
         * | FESx   | S |   FESy  | N |   |   FESz     |
         * |--------|---|---------|---|---|------------|
         *            |
         *            |------(1)------->|
         * |--------|---|---------|---|---|------------|
         * | FESx   |   |   FESy  | N | S |   FESz     |
         * |--------|---|---------|---|---|------------|
         *                          |
         *            |<----(2)-----|
         * |--------|---|---------|---|---|------------|
         * | FESx   | N |   FESy  |   | S |   FESz     |
         * |--------|---|---------|---|---|------------|
         *                 |
         *                 |-(3)->|
         * |--------|---|---|---------|---|------------|
         * | FESx   | N |   |   FESy  | S |   FESz     |
         * |--------|---|---|---------|---|------------|
         */
        
        /*move compare FES to free space, and output a new space*/
        idx_move_from = idx_compare + group_base;
        idx_move_to = *idx_free + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);
        /*move invalid_next FES to the new space*/
        idx_move_from = *idx_free - 1 + group_base;
        idx_move_to = idx_compare + group_base;            
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

        /*calc chunk boarder and step*/
        if (type == ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H) {            
            idx_move_start = (*idx_free < idx_compare) ? (*idx_free+group_base+1) : (idx_compare+group_base+1);
            idx_move_end = (*idx_free > idx_compare) ? (*idx_free+group_base-2) : (idx_compare+group_base-2);
            move_step = (idx_move_start == idx_move_end) ? 0 : 1;
        } else {            
            idx_move_start = (*idx_free < idx_compare) ? (*idx_free+group_base) : (idx_compare+group_base);
            idx_move_end = (*idx_free > idx_compare) ? (*idx_free+group_base) : (idx_compare+group_base);
            move_step = (idx_move_start == idx_move_end) ? 0 : -1;
        }
        res = arad_pmf_db_fes_chunk_move_unsafe(
                unit,
                pmf_pgm_ndx,
                flags,
                idx_move_start,
                idx_move_end,
                move_step,
                fes_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
        if (type == ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H) {
            *idx_free = idx_compare + 1;
        } else {
            *idx_free = idx_compare;
        }
    } else if (!is_insert_combine && is_compare_combine) {
        /*
         * N: fes for invalid_next, S: fes for first action of combined action
         *
         * |--------|---|---|-------------|---|------------|
         * | FESx   | N | S |   FESy      |   |   FESz     |
         * |--------|---|---|-------------|---|------------|
         *            |   |---|  |
         *            |       |  |----| 
         *            |---|   |       |
         *                |   |       |
         *                V   V       V
         * |--------|---|---|---|-------------|------------|
         * | FESx   |   | N | S |   FESy      |   FESz     |
         * |--------|---|---|---|-------------|------------|
         *
         */

        /* move FES chunk */
        if (type == ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H) {            
            idx_move_start = (*idx_free < idx_compare) ? (*idx_free+group_base-1) : (idx_compare+group_base-1);
            idx_move_end = (*idx_free > idx_compare) ? (*idx_free+group_base-1) : (idx_compare+group_base-1);
            move_step = (idx_move_start == idx_move_end) ? 0 : 1;
        } else {            
            idx_move_start = (*idx_free < idx_compare) ? (*idx_free+group_base+1) : (idx_compare+group_base+1);
            idx_move_end = (*idx_free > idx_compare) ? (*idx_free+group_base) : (idx_compare+group_base);
            move_step = (idx_move_start == idx_move_end) ? 0 : -1;
        }
        res = arad_pmf_db_fes_chunk_move_unsafe(
                unit,
                pmf_pgm_ndx,
                flags,
                idx_move_start,
                idx_move_end,
                move_step,
                fes_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
        
        if (type == ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H) {
            *idx_free = idx_compare - 1;
        } else {
            *idx_free = idx_compare;
        }
    } else if (is_insert_combine && is_compare_combine) {
        /*
         * N: fes for invalid_next, S: fes for first action of combined action
         *
         * |--------|---|---|---------|---|---|------------|
         * | FESx   | n | s |   FESy  | N |   |   FESz     |
         * |--------|---|---|---------|---|---|------------|
         *                |
         *                |---------------->|
         * |--------|---|---|---------|---|---|------------|
         * | FESx   | n |   |   FESy  | N | s |   FESz     |
         * |--------|---|---|---------|---|---|------------|
         *                              |
         *                |<------------|
         * |--------|---|---|---------|---|---|------------|
         * | FESx   | n | N |   FESy  |   | s |   FESz     |
         * |--------|---|---|---------|---|---|------------|
         *            |
         *            |---------------->|
         * |--------|---|---|---------|---|---|------------|
         * | FESx   |   | N |   FESy  | n | s |   FESz     |
         * |--------|---|---|---------|---|---|------------|
         *                |
         *            |<--|
         * |--------|---|---|---------|---|---|------------|
         * | FESx   | N |   |   FESy  | N | S |   FESz     |
         * |--------|---|---|---------|---|---|------------|
         *
         */

        /* move exist combined FES */
        idx_move_from = idx_compare + group_base;
        idx_move_to = *idx_free + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

        /* move invalid_next */
        idx_move_from = *idx_free - 1 + group_base;
        idx_move_to = idx_compare + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

        idx_move_from = idx_compare - 1 + group_base;
        idx_move_to = *idx_free -1 + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);

        idx_move_from = idx_compare + group_base;
        idx_move_to = idx_compare - 1 + group_base;
        ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, idx_move_from, idx_move_to);
        
        *idx_free = idx_compare;
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fes_combined_action_process()", 0, 0);

}


/*
 * Handle the case when INVALID_NEXT is the last action of the 1st cycle (expect idx_invalid_next==15),
 * move the combined actions (INVALID_NEXT and the one behind it) to be one step
 * forward or backward.
 */
uint32 arad_pp_fp_fes_combined_action_post(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN uint32                  pmf_pgm_ndx,
    SOC_SAND_IN uint32                  flags,
    SOC_SAND_IN uint8                   idx_invalid_next,
    SOC_SAND_INOUT uint8                *idx_free,
    SOC_SAND_INOUT ARAD_PMF_FES         *fes_info
)
{
    uint32 res = SOC_SAND_OK;
    int fes_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(idx_free);
    SOC_SAND_CHECK_NULL_INPUT(fes_info);

    if (*idx_free > idx_invalid_next) {
        for (fes_id=idx_invalid_next-1; fes_id>=0; --fes_id) {
            if (!fes_info[fes_id].is_used) {
                /*
                 * Move [Free+1, INVALID_NEXT+1] actions one step backward to the free slot
                 * N: the INVALID_NEXT action
                 * X: the 1st action after INVALID_NEXT, this is the 2nd action of the combined action
                 *
                 * |--------|---|---------|---|---|------------|
                 * | FESx   |   |   FESy  | N | X |   ...      |
                 * |--------|---|---------|---|---|------------|
                 *
                 *            <-------------------                       
                 * |--------|---------|---|---|---|------------|
                 * | FESx   |   FESy  | N | X |   |   ...      |
                 * |--------|---------|---|---|---|------------|
                 */
                res = arad_pmf_db_fes_chunk_move_unsafe(
                        unit,
                        pmf_pgm_ndx,
                        flags,
                        fes_id + 1,
                        idx_invalid_next + 1,
                        -1,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
                break;
            }
            else if (!fes_info[fes_id].is_combined && (fes_info[fes_id].action_type != SOC_PPC_FP_ACTION_TYPE_SNP)
                && (fes_info[fes_id].action_type != SOC_PPC_FP_ACTION_TYPE_COUNTER_A)) {
                /*
                 * Move 1st non combined, not snoop, and not counter action to be after the combined actions
                 * N: the INVALID_NEXT action
                 * X: the 1st action after INVALID_NEXT, this is the 2nd action of the combined action
                 * M: the 1st non combined, not snoop, and not counter action (so we don't need to resolve any conflicts)
                 *
                 * |--------|---|---------|---|---|------------|
                 * | FESx   | M |   FESy  | N | X |   ...      |
                 * |--------|---|---------|---|---|------------|
                 *            |
                 *            ------------------
                 *                             \|/
                 * |--------|---------|---|---|---|------------|
                 * | FESx   |   FESy  | N | X | M |   ...      |
                 * |--------|---------|---|---|---|------------|
                 *
                 * M => idx_free
                 */
                ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, fes_id, *idx_free);
    
                /*
                 * Move [M+1, X] one step backwark
                 */
                res = arad_pmf_db_fes_chunk_move_unsafe(
                        unit,
                        pmf_pgm_ndx,
                        flags,
                        fes_id + 1,
                        idx_invalid_next + 1,
                        -1,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

                /*
                 * idx_free => INVALID_NEXT
                 */
                ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, *idx_free, idx_invalid_next+1);

                /*
                 * If X equals to idx_free then idx_free needs to be reduced by 1
                 */
                if (*idx_free == (idx_invalid_next+1)) {
                    *idx_free -= 1;
                }

                break;
            }
        }
    }
    else {
        for (fes_id=idx_invalid_next+2; fes_id<ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_id) {
            if (!fes_info[fes_id].is_used) {
                /*
                 * Move INVALID_NEXT and following actions one step forward to the free slot
                 * N: the INVALID_NEXT action
                 *
                 * |--------|---|---------|---|------------|
                 * | FESx   | N |   FESy  |   |   ...      |
                 * |--------|---|---------|---|------------|
                 *
                 *            ----------->                       
                 * |--------|---|---|---------|------------|
                 * | FESx   |   | N |   FESy  |   ...      |
                 * |--------|---|---|---------|------------|
                 */
                res = arad_pmf_db_fes_chunk_move_unsafe(
                        unit,
                        pmf_pgm_ndx,
                        flags,
                        idx_invalid_next,
                        fes_id - 1,
                        1,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
                break;
            }
            else if (!fes_info[fes_id].is_combined && (fes_info[fes_id].action_type != SOC_PPC_FP_ACTION_TYPE_SNP)
                && (fes_info[fes_id].action_type != SOC_PPC_FP_ACTION_TYPE_COUNTER_A)) {
                /*
                 * Move 1st non combined, not snoop, and not counter action to be before the combined actions
                 * N: the INVALID_NEXT action
                 * X: the 1st action after INVALID_NEXT, this is the 2nd action of the combined action
                 * M: the 1st non combined, not snoop, and not counter action (so we don't need to resolve any conflicts)
                 *
                 * |--------|---|---|---------|---|------------|
                 * | FESx   | N | X |   FESy  | M |   ...      |
                 * |--------|---|---|---------|---|------------|
                 *                              |
                 *            ------------------
                 *           \|/
                 * |--------|---|---|---|---------|------------|
                 * | FESx   | M | N | X |   FESy  |   ...      |
                 * |--------|---|---|---|---------|------------|
                 *
                 * M => idx_free
                 */
                ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, fes_id, *idx_free);
    
                /*
                 * Move [N, M-1] one step forward
                 */
                res = arad_pmf_db_fes_chunk_move_unsafe(
                        unit,
                        pmf_pgm_ndx,
                        flags,
                        idx_invalid_next,
                        fes_id - 1,
                        1,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

                /*
                 * idx_free => INVALID_NEXT
                 */
                ARAD_PP_FP_FES_OPERATION_MOVE(pmf_pgm_ndx, *idx_free, idx_invalid_next);
                break;
            }
        }
    }

    if ((fes_id < 0) || (fes_id>ARAD_PMF_LOW_LEVEL_NOF_FESS)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_FAILURE_OUT_OF_RESOURCES, SOC_SAND_GEN_ERR, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fes_combined_action_post()", 0, 0);
}



uint32
  arad_pp_fp_fem_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      pmf_pgm_ndx,
      pmf_pgm_ndx_min,
      pmf_pgm_ndx_max,
      pmf_pgm_bmp_used,
    cycle_ndx,
      fem_id_absolute,
    fem_id_ndx;
  SOC_PPC_FP_FEM_ENTRY
    fem_entry;
  ARAD_PMF_FEM_INPUT_INFO
    fem_input_info;
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO
    fem_action_format_map_info;
  uint32
    selected_bits_ndx;
  ARAD_PMF_FEM_NDX
    fem_ndx;
  uint8
      fem_pgm_id;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_REMOVE);

  /*
   * Look at the two cycles for the relevant FEMs
   */
  /* using the macro SOC_DPP_DEFS_MAX can compare definitions of the same value */
  /* coverity[same_on_both_sides] */
  for (cycle_ndx = 0; cycle_ndx < ARAD_PMF_NOF_CYCLES; ++cycle_ndx)
  {
    for (fem_id_ndx = 0; fem_id_ndx < ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP; ++fem_id_ndx)
    {
        for(fem_pgm_id = 0 ; (fem_pgm_id < ARAD_PMF_NOF_FEM_PGMS); ++fem_pgm_id)
        {
            fem_id_absolute = fem_id_ndx + (cycle_ndx * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP);
          SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                  unit,
                  SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                  fem_pgm_id,
                  fem_id_absolute,
                  &fem_entry
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
          if (entry_ndx->db_id == fem_entry.db_id)
          {
            if (
                (entry_ndx->is_for_entry == FALSE)
                || (entry_ndx->entry_id == fem_entry.entry_id)
               )
            {
              /*
                       * Disable this FEM: SW DB and NOP in input for all PFGs
                       */
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.progs.get(unit, SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, fem_entry.db_id, 0, &pmf_pgm_bmp_used);

              SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.set(
                      unit,
                      SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                      fem_pgm_id,
                      fem_id_absolute,
                      &fem_entry
                    );
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

              res = arad_pmf_prog_select_pmf_pgm_borders_get(
                        unit,
                        SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF,
                        fem_pgm_id, 
                        &pmf_pgm_ndx_min,
                        &pmf_pgm_ndx_max
                      );
              SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

              ARAD_PMF_FEM_INPUT_INFO_clear(&fem_input_info);

              
              for (pmf_pgm_ndx = pmf_pgm_ndx_min; pmf_pgm_ndx < pmf_pgm_ndx_max; ++pmf_pgm_ndx)
              {
                    if (SOC_SAND_GET_BIT(pmf_pgm_bmp_used, pmf_pgm_ndx) == FALSE){
                      /* PMF Program not used */
                      continue;
                    }
                    fem_input_info.db_id = 0; /* No importance */
                    fem_input_info.pgm_id = fem_pgm_id;
                    fem_input_info.src_arad.is_nop = TRUE;
                    res = arad_pmf_db_fem_input_set_unsafe(
                            unit,
                            pmf_pgm_ndx,
                            FALSE, /* is_for_fes */
                            fem_id_absolute,
                            &fem_input_info
                          );
                    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
              }

              /*
                       * Set all the mapping to action 3
                       */
              ARAD_PMF_FEM_NDX_clear(&fem_ndx);
              fem_ndx.id = fem_id_absolute;

              ARAD_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&fem_action_format_map_info);
              fem_action_format_map_info.map_data = 0;
              fem_action_format_map_info.action_fomat_id = ARAD_PMF_FEM_ACTION_DEFAULT_NOP_3; /* No real need for a NOP - valid bit instead */
              fem_action_format_map_info.is_action_valid = FALSE;
              for (selected_bits_ndx = 0; selected_bits_ndx <= ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_MAX; ++selected_bits_ndx)
              {
                res = arad_pmf_fem_action_format_map_set_unsafe(
                        unit,
                        &fem_ndx,
                        fem_pgm_id,
                        selected_bits_ndx,
                        &fem_action_format_map_info
                      );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
              }
            }
          }
        }
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_remove()", 0, 0);
}


/*********************************************************************
*     Compute the best configuration to add greedily Direct
 *     Extraction entries (preference to the new
 *     Database-ID). If set, set all the FEM (selected bits,
 *     actions) and its input. Look at the previous FEM
 *     configuration to shift the FEMs if necessary. The FEM
 *     input can be changed again upon the new TCAM DB
 *     creation.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_fp_fem_insert_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  uint32                         flags,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *fem_info,
    SOC_SAND_INOUT ARAD_PMF_FES                 fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_IN uint8                           fem_pgm_id,
    SOC_SAND_IN uint32                          fem_bypass_pgm_bmp,
    SOC_SAND_OUT uint32                         *fes_fem_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  )
{
  uint32
    output_size_in_bits,
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
      nof_fem_fes_per_group,
    res = SOC_SAND_OK;
  uint8
      is_for_fem,
      is_for_tm,
      /* indexing according to 32: 0-15 in general are used, and 0-31 when FES-one-group (cycle=0) */
      is_fem_higher[ARAD_PMF_LOW_LEVEL_NOF_FESS] = {FALSE},
    is_fem_lower[ARAD_PMF_LOW_LEVEL_NOF_FESS] = {FALSE},
    place_found;
  uint32
    action_ndx,
    fes_prio,
    fem_free_min_ndx,
    fem_free_min,
    fem_free_first=0,
    fem_free_last,
    cycle_ndx,
    nof_free_fems[ARAD_PMF_NOF_CYCLES],
    cycle_to_use,
    fes_id_indx,
    fes_id_indx_2,
    fem_id_ndx;
  SOC_PPC_FP_FEM_ENTRY
    entry_replicated,
    fem_entry;
  SOC_PPC_FP_FEM_CYCLE
    fem_cycle_lcl;
  ARAD_PMF_FEM_NDX
    fem;
  SOC_PPC_FP_DATABASE_STAGE
      stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF;
  char
      reason_for_fail[640] ;
  uint8 is_insert_combined = FALSE, is_exist_combined = FALSE;
  uint8 idx_free, idx_compare;
  int idx_invalid_next;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_INSERT_UNSAFE);

  /* 
   * Common implementation of FES / FEM insertion and computation 
   * Common steps: 
   * - divide the space in 2 cycles 
   * - for each cycle, see if there is some space:
   *    - at least a free FES / FEM
   *    - no blocking FES / FEM in the other group
   *    - no blocking FEM / FES in the other FES/FEM space
   * - if both cycles are free: 
   *    - Use cycle 0 for FEM, 1 for FES (more options for future FES / FEM)
   * - in each group / cycle, 
   *    - Compute the minimum legal FES/FEM: 0 for FES,
   *        action-type size dependent for FEM  
   *    - Compute the blocking FES / FEM of the same group 
   *    - Find the first empty slot and move the blocking FES/FEM upward
   *    - Insert the new FES/FEM in its slot
   * In case of FES with flag check-only: 
   *  - Same logic, just do not make in practice the move upward of
   *  blocking FES / FEM
   *  - Do not insert in practice the new FES/FEM in its slot
   * At removal: 
   *  - find the FES/FEM and remove it
   *  - compress the other FES/FEM in the same group backward
   */

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(success);

  /*
   * Verify
   */
  res = arad_pp_fp_fem_insert_verify(
          unit,
          entry_ndx,
          fem_cycle,
          fem_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* 
   * For the moment, FEM for Direct extraction entries, 
   * FES for TCAM / Direct table DBs 
   */
  is_for_fem = entry_ndx->is_for_entry;
  nof_fem_fes_per_group = (is_for_fem)? ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP:
                             ((flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP)? 
                                    ARAD_PMF_LOW_LEVEL_NOF_FESS:ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP);
  is_for_tm = ( flags & ARAD_PP_FP_FEM_ALLOC_FES_TM ) ? TRUE : FALSE ;                        
  

  
  if (!is_for_fem) {
      SOC_SAND_CHECK_NULL_INPUT(fes_info);
  }
  else {
      SOC_SAND_CHECK_NULL_INPUT(fem_info);
  }

      /*
       * Verify if there is a place, otherwise
       * return failure
       */

      reason_for_fail[0] = 0 ;
      place_found = TRUE ;
      if (!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP)) {
          /* no need to check in case of oen FES block - checked before */
          res = arad_pp_fp_fem_is_place_get_unsafe(
              unit,
              entry_ndx,
              fem_cycle,
              is_for_tm,
              fem_pgm_id,
              fes_info,
              &place_found,
              reason_for_fail,
              sizeof(reason_for_fail)
            ) ;
         SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
      if (place_found == TRUE)
      {
        *success = SOC_SAND_SUCCESS;
      }
      else if(!is_for_fem && !(flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP)) {
          /* Do not trigger error for FES allocation first attempt */
          *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES ;
          ARAD_PP_DO_NOTHING_AND_EXIT ;
      }
      else
      {
          LOG_ERROR(BSL_LS_SOC_FP,
              (BSL_META_U(
                  unit,
                  "\r\n  ==> FEM not found for db_id: %d, entry:%d \n\r"
                  "%s%s\n\r"
                  ), 
                  entry_ndx->db_id, entry_ndx->entry_id,
                  (sal_strlen(reason_for_fail) ? "  ==> Reason: " : ""),
                  reason_for_fail
              )
          ) ;
          *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES ;
          ARAD_PP_DO_NOTHING_AND_EXIT ;
      }
  

  /*
   * Set the cycle to insert it: if fixed cycle, this one
   * Otherwise, the one with least full FEMs
   */
  if (flags & ARAD_PP_FP_FEM_ALLOC_FES_CONSIDER_AS_ONE_GROUP) {
      cycle_to_use = 0;
  }
  else if (fem_cycle->is_cycle_fixed)
  {
    cycle_to_use = fem_cycle->cycle_id;
  }
  else
  {
    /* using the macro SOC_DPP_DEFS_MAX can compare definitions of the same value */
    /* coverity[same_on_both_sides] */
    for (cycle_ndx = 0; cycle_ndx < ARAD_PMF_NOF_CYCLES; ++cycle_ndx)
    {
      char
        reason_for_fail[1] ;
      reason_for_fail[0] = 0 ;
      SOC_PPC_FP_FEM_CYCLE_clear(&fem_cycle_lcl);
      fem_cycle_lcl.is_cycle_fixed = TRUE;
      fem_cycle_lcl.cycle_id = SOC_SAND_NUM2BOOL(cycle_ndx);
      res = arad_pp_fp_fem_is_place_get_unsafe(
              unit,
              entry_ndx,
              &fem_cycle_lcl,
              is_for_tm,
              fem_pgm_id,
              fes_info,
              &place_found,
              reason_for_fail,
              sizeof(reason_for_fail)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      nof_free_fems[cycle_ndx] = 0;

      if (place_found == FALSE)
      {
        continue;
      }

      /* Get the number of free FEMs in this group */
      for (fem_id_ndx = 0; fem_id_ndx < nof_fem_fes_per_group; ++fem_id_ndx)
      {
        fes_id_indx = fem_id_ndx + (fem_cycle_lcl.cycle_id * nof_fem_fes_per_group);
        SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
        if (is_for_fem) {
            res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                    unit,
                    stage,
                    fem_pgm_id,
                    fes_id_indx,
                    &fem_entry
                  );
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
        }
        else {
            fem_entry.action_type[0] = fes_info[fes_id_indx].is_used? 
                fes_info[fes_id_indx].action_type:SOC_PPC_FP_ACTION_TYPE_INVALID;
        }

        if (fem_entry.action_type[0] == SOC_PPC_FP_ACTION_TYPE_INVALID)
        {
          nof_free_fems[cycle_ndx] ++;
        }
      }
    }

    /* Use preferably cycle 0 for FEM, cycle 1 for FES */
    cycle_ndx = (is_for_fem)? 0:1;
    if (nof_free_fems[cycle_ndx])
    {
      cycle_to_use = cycle_ndx;
    }
    else
    {
      cycle_to_use = 1 - cycle_ndx;
    }
  }

  /*
   * If success, re-organize the FEMs:
   * Find the FEMs to transfer (i.e. duplicate) before inserting it
   * Find the first available FEM, and starts from it
   */
  for (action_ndx = 0; action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
      /* only the first action (action 0) is relevant for the FES */
      if ((!is_for_fem) && action_ndx) {
          break;
      }

    if (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    /*
     * Get the first FEM to consider
     * Assumption that the first free FEM means there is no action afterwards
     */
    fem_free_min = 0; /* Good value for FES, since every FES is HW similar */
    for (fem_id_ndx = 0; (fem_id_ndx < nof_fem_fes_per_group) && is_for_fem; ++fem_id_ndx)
    {
      ARAD_PMF_FEM_NDX_clear(&fem);
      fem.cycle_ndx = cycle_to_use;
      fem.id = fem_id_ndx;
      res = arad_pmf_fem_output_size_get(
              unit,
              &fem,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = arad_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[action_ndx],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      if (action_size_in_bits_in_fem <= output_size_in_bits)
      {
        /* Avoid FEM 0 / 1 when the Base-offset is positive */
        if ((output_size_in_bits != 4) || (!(entry_ndx->is_base_positive[action_ndx])))
        {
          fem_free_min = fem_id_ndx;
          break;
        }
      }
    }

    /* 
     * Init the higher-FEM array 
     */
    for (fem_id_ndx = 0; fem_id_ndx < nof_fem_fes_per_group; ++fem_id_ndx)
    {
      is_fem_higher[fem_id_ndx] = 0;
    }

    /*
     *  Assumption: there IS a free FEM
     *  Get the higher priority FEMs/FESs bitmap
     */
    fem_free_min_ndx = nof_fem_fes_per_group;
    for (fem_free_first = fem_free_min; fem_free_first < nof_fem_fes_per_group; ++fem_free_first)
    {
        if (is_for_fem) {
          SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                  unit,
                  stage,
                  fem_pgm_id,
                  fem_free_first + (cycle_to_use * nof_fem_fes_per_group),
                  &fem_entry
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

          SOC_PPC_FP_FEM_ENTRY_clear(&entry_replicated);
          ARAD_COPY(&entry_replicated, entry_ndx, SOC_PPC_FP_FEM_ENTRY, 1);
          entry_replicated.action_type[0] = entry_ndx->action_type[action_ndx];
          res = arad_pp_fp_fem_is_fem_blocking_get(
                  unit,
                  &entry_replicated,
                  &fem_entry,
                  &(is_fem_higher[fem_free_first])
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

          res = arad_pp_fp_fem_is_fem_blocking_get(
                  unit,
                  &fem_entry,
                  &entry_replicated,
                  &(is_fem_lower[fem_free_first])
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

          if ((fem_entry.action_type[0] == SOC_PPC_FP_ACTION_TYPE_INVALID) 
              && (fem_free_min_ndx == nof_fem_fes_per_group))
          {
            fem_free_min_ndx = fem_free_first;
          }
        }
        else /* For FES */ {
            fes_id_indx = fem_free_first + (cycle_to_use * nof_fem_fes_per_group);
            if(fes_info[fes_id_indx].is_used 
                && ((fes_info[fes_id_indx].action_type == entry_ndx->action_type[action_ndx])
                    || ((fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_DEST_DROP) && (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_DEST))
                    || ((fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_DEST) && (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_DEST_DROP)))) {
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.prio.get(unit, stage, fes_info[fes_id_indx].db_id, &fes_prio);
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
                /*combined invalid_next doesn't compare here */
                if (!((flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED)
                        && (fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT))) {
                    /* A highest strength is stronger (127 is the strongest) */
                    if (fes_prio > entry_ndx->db_strength) {
                        is_fem_higher[fem_free_first] = TRUE;
                    }
                    else if (fes_prio < entry_ndx->db_strength) {
                        is_fem_lower[fem_free_first] = TRUE;
                    }
                }
                
            }
            
            if ((!fes_info[fes_id_indx].is_used) 
                && (fem_free_min_ndx == nof_fem_fes_per_group)) {
                fem_free_min_ndx = fem_free_first;
            }
        }
    }

    if (flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED) {
        is_insert_combined = ((entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_COUNTER_A)
                            || (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_SNP)) ? TRUE : FALSE;
    }

    /*
     * Duplication if necessary: 
     * - backward for lower priority 
     * - forward for higher priority 
     */

      fem_free_first = fem_free_min_ndx;
      fem_free_last = fem_free_first;
      idx_free = fem_free_min_ndx;  /* idx_free: index of FES after every sort */

        /*
         * pre-process for combined action insert.
         * After this action we should put FES(invalid_next) before first free FES
         */
        if (!is_for_fem && is_insert_combined) {
            res = arad_pp_fp_fes_sole_invalid_next_idx_get(
                        unit,
                        entry_ndx->entry_id,
                        cycle_to_use,
                        nof_fem_fes_per_group,
                        fes_info,
                        &idx_invalid_next);
            SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
            if (idx_invalid_next != -1) {
                res = arad_pp_fp_fes_combined_action_pre(
                        unit,
                        entry_ndx->entry_id,
                        flags,
                        idx_free,
                        idx_invalid_next,
                        cycle_to_use,
                        nof_fem_fes_per_group,
                        is_fem_higher,
                        is_fem_lower,
                        fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
            }
        }
      
      /* Loop from next-after-free to end of group */
      for (fem_id_ndx = fem_free_first + 1; fem_id_ndx < nof_fem_fes_per_group; fem_id_ndx++)
      {
        if (is_fem_lower[fem_id_ndx] == TRUE)
        {
          fem_free_last = fem_id_ndx;

          if (is_for_fem) {
              res = arad_pp_fp_fem_duplicate(
                      unit,
                      is_for_tm,
                      fem_free_last + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_free_first + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_pgm_id
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
          } else {              
              /*in case combined action*/
              fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group);
              if (fes_info[fes_id_indx].is_combined) {
                  if ((fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_SNP)
                        || (fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_COUNTER_A)) {
                      is_exist_combined = TRUE;
                  }
              }
              
              if (is_insert_combined || is_exist_combined) {
                  idx_compare = fem_free_last;                  
                  res = arad_pp_fp_fes_combined_action_process(
                            unit,
                            entry_ndx->entry_id,
                            ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_L,
                            flags,
                            is_insert_combined,
                            is_exist_combined,
                            cycle_to_use,
                            nof_fem_fes_per_group,
                            idx_compare,
                            &idx_free,
                            fes_info);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
              } else {
                  /* Copy in the current info */
                  fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group); /* From */
                  fes_id_indx_2 = fem_free_first + (cycle_to_use * nof_fem_fes_per_group); /* To */

                  sal_memcpy(&fes_info[fes_id_indx_2],&fes_info[fes_id_indx],sizeof(ARAD_PMF_FES));
                  fes_info[fes_id_indx].is_used = 0;
                  /* update hardware if needed */
                  if(!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
                      res = arad_pmf_db_fes_move_unsafe(
                            unit,
                            entry_ndx->entry_id, /* prog_id, */
                            fes_id_indx,
                            fes_id_indx_2,
                            &fes_info[fes_id_indx_2]
                          );
                      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
                  }
              }
          }
          if (is_insert_combined || is_exist_combined) {
            fem_free_first = idx_free;
          } else {
            fem_free_first = fem_id_ndx;
          }
      
        }
      }

      /* Backward - in fact loop from first-1 to 0 - cause of all the -1 */
      fem_free_last = fem_free_first;
      idx_free = fem_free_first; 
      for (fem_id_ndx = fem_free_first; fem_id_ndx > 0; fem_id_ndx--)
      {
        if (is_fem_higher[fem_id_ndx - 1] == TRUE)
        {
          fem_free_last = fem_id_ndx - 1;

          if (is_for_fem) {
              res = arad_pp_fp_fem_duplicate(
                      unit,
                      is_for_tm,
                      fem_free_last + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_free_first + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                      fem_pgm_id
                    );
              SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
          } else {
              fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group);
              if (fes_info[fes_id_indx].is_combined) {
                  if ((fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_SNP)
                        || (fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_COUNTER_A)) {
                      is_exist_combined = TRUE;
                  }
              }

              if (is_insert_combined || is_exist_combined) {
                  idx_compare = fem_free_last;                  
                  res = arad_pp_fp_fes_combined_action_process(
                            unit,
                            entry_ndx->entry_id,
                            ARAD_PP_FP_FES_COMBINE_MOVE_CONDITION_H,
                            flags,
                            is_insert_combined,
                            is_exist_combined,
                            cycle_to_use,
                            nof_fem_fes_per_group,
                            idx_compare,
                            &idx_free,
                            fes_info);
                  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
              } else {
                  /* Copy in the current info */
                  fes_id_indx = fem_free_last + (cycle_to_use * nof_fem_fes_per_group); /* From */
                  fes_id_indx_2 = fem_free_first + (cycle_to_use * nof_fem_fes_per_group); /* To */
                  
                  sal_memcpy(&fes_info[fes_id_indx_2],&fes_info[fes_id_indx],sizeof(ARAD_PMF_FES));
                  fes_info[fes_id_indx].is_used = 0;
                  /* update hardware if needed */
                  if(!(flags & ARAD_PP_FP_FEM_ALLOC_FES_CHECK_ONLY)) {
                      res = arad_pmf_db_fes_move_unsafe(
                            unit,
                            entry_ndx->entry_id, /* prog_id, */
                            fes_id_indx,
                            fes_id_indx_2,
                            &fes_info[fes_id_indx_2]
                          );
                      SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
                  }
              }
          }
          
          fem_free_first = fem_id_ndx - 1;
        }
      }

        if (is_insert_combined || is_exist_combined) {
            fem_free_last = idx_free;
        }
        *fes_fem_id = fem_free_last + (cycle_to_use * nof_fem_fes_per_group);

        /*
         * Configure the last free FEM - in case of FEM
         */
        if (is_for_fem) {
            res = arad_pp_fp_fem_configure(
                    unit,
                    fem_free_last + (cycle_to_use * ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP),
                    &entry_replicated,
                    &(fem_info->actions[action_ndx]),
                    &(fem_info->qual_vals[0]),
                    fem_pgm_id,
                    fem_bypass_pgm_bmp
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
        }
        else {
            fes_id_indx = ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP - 1;
            if (fes_info[fes_id_indx].is_used && fes_info[fes_id_indx].is_combined
                && (fes_info[fes_id_indx].action_type == SOC_PPC_FP_ACTION_TYPE_INVALID_NEXT)) {
                res = arad_pp_fp_fes_combined_action_post(
                          unit,
                          entry_ndx->entry_id,
                          flags,
                          fes_id_indx,
                          &idx_free,
                          fes_info);
                SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
            }

            fes_info[*fes_fem_id].is_used = 1;
            fes_info[*fes_fem_id].db_id = entry_ndx->db_id;
            fes_info[*fes_fem_id].action_type = entry_ndx->action_type[0];
            fes_info[*fes_fem_id].is_combined = (flags & ARAD_PP_FP_FEM_ALLOC_FES_COMBINED) ? 1 : 0;
        }
  }
        


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_insert_unsafe()", 0, 0);
}

uint32
  arad_pp_fp_fem_insert_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE           *fem_cycle,
    SOC_SAND_IN  SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *fem_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_INSERT_VERIFY);

  res = SOC_PPC_FP_FEM_ENTRY_verify(unit, entry_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FP_FEM_CYCLE, fem_cycle, 15, exit);
  if (entry_ndx->is_for_entry) {
    /* Verify only for FEM */
      res = SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_verify(unit, fem_info, SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, FALSE /* alloc_fes_flag */);
      SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_insert_verify()", 0, 0);
}

STATIC
  uint32
    arad_pp_fp_fem_is_place_get_for_cycle(
      SOC_SAND_IN  int                    unit,
      SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY   *entry_ndx,
      SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE   *fem_info,
      SOC_SAND_IN  uint8                  fem_pgm_id,
      SOC_SAND_INOUT ARAD_PMF_FES         fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
      SOC_SAND_OUT uint8                  *place_found,
      SOC_SAND_OUT char                   *reason_for_fail,
      SOC_SAND_IN int32                   reason_for_fail_len
    )
{
  uint32
    action_size_in_bits_max,
    action_size_in_bits_in_fem,
    output_size_in_bits,
    res = SOC_SAND_OK;
  uint32
    fem_fes_group_ndx,
    nof_fem_fes_per_group,
    nof_fem_fes_groups,
    nof_fem_fes_per_group_other_type,
    action_ndx,
    fem_id_ndx;
  uint8
    is_higher_prio, /* boolean - if FALSE, then lower priority - no dont care */
    is_other_type_fes_fem, /* If FES, 1 for FEM and conversely */
    is_for_fem,
    fes_is_found = FALSE,
    fem_is_found = FALSE;
  SOC_PPC_FP_FEM_ENTRY
    fem_entry_reference,
    fem_entry;
  ARAD_PMF_FEM_NDX
    fem;
  SOC_PPC_FP_ACTION_TYPE
    fem_action_type_bmp[ARAD_PMF_LOW_LEVEL_FEM_ID_MAX + 1];
  SOC_PPC_FP_FEM_ENTRY
    fem_entry_high,
    fem_entry_low;
  SOC_PPC_FP_DATABASE_INFO
    fp_database_info;
  uint8
    is_for_tm,
    is_default_tm,
    forbidden_fem_cycle2_found = FALSE;
  uint32
    fes_id_indx,
    cycle_curr, /* Cycle of the current FES / FEM */
    other_cycle;
  SOC_PPC_FP_DATABASE_STAGE
    stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
  char
    *loc_reason_for_fail ;
  int32
    loc_reason_for_fail_len ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_PLACE_GET_FOR_CYCLE);

  loc_reason_for_fail = reason_for_fail ;
  loc_reason_for_fail_len = reason_for_fail_len ;
  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fem_info);
  SOC_SAND_CHECK_NULL_INPUT(place_found);
  SOC_SAND_CHECK_NULL_INPUT(reason_for_fail);

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "Try to find FEM/FES for db_id:%d, entry:%d, with Info: cycle %d, cycle-fixed: %d  \n\r"), 
             entry_ndx->db_id, entry_ndx->entry_id, fem_info->cycle_id, fem_info->is_cycle_fixed));

  /* 
   * For the moment, FEM for Direct extraction entries, 
   * FES for TCAM / Direct table DBs 
   */
  is_for_fem = entry_ndx->is_for_entry;
  nof_fem_fes_per_group = (is_for_fem)? ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP:ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP;
  nof_fem_fes_per_group_other_type = (!is_for_fem)? ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP:ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP;
  nof_fem_fes_groups = 2; /* Always 2 groups */

  if (!is_for_fem) {
      SOC_SAND_CHECK_NULL_INPUT(fes_info);
  }

  SOC_PPC_FP_DATABASE_INFO_clear(&fp_database_info);
  res = arad_pp_fp_database_get_unsafe(
          unit,
          entry_ndx->db_id,
          &fp_database_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_fp_database_is_tm_get(unit, &fp_database_info, &is_for_tm, &is_default_tm);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);


  /*
   * Verify if there is a stronger / weaker FEM in the
   * other cycle.
   * Assumption: do not remove FEMs from the other cycle at the end (HW set) 
   *  
   * Besides, verify if there is a blocking FES (in case of FEM or conversely) 
   * for each cycle  
   */
  other_cycle = ARAD_PMF_NOF_CYCLES - fem_info->cycle_id - 1;
  for (action_ndx = 0; action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
      /* only the first action (action 0) is relevant for the FES */
      if ((!is_for_fem) && action_ndx) {
          break;
      }

    if (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry_reference);
    ARAD_COPY(&fem_entry_reference, entry_ndx, SOC_PPC_FP_FEM_ENTRY, 1);
    fem_entry_reference.action_type[0] = entry_ndx->action_type[action_ndx];
    /*
     * Loop on: 
     * - Type to look at: current type first (FES/FEM), other type then (FEM/FES) 
     * - Number of groups to look at (1 for current type), 2 for the other type 
     * - For each index, look if blocking by: 
     *  - see if higher or lower priority (cannot be dont care since any other group
     *      has a priority vs the current group)
     *  - Copy into FEM entry type
     *  - Use blocking
     * - When blocking found, stop everything
     */
    for (is_other_type_fes_fem = FALSE; (is_other_type_fes_fem <= TRUE)
                                 && (forbidden_fem_cycle2_found == FALSE); is_other_type_fes_fem++) {
        for (fem_fes_group_ndx = 0; fem_fes_group_ndx < (is_other_type_fes_fem?nof_fem_fes_groups:1 /* Only other cycle otherwise */)
                                 && (forbidden_fem_cycle2_found == FALSE); fem_fes_group_ndx++) {
            for (fem_id_ndx = 0; (fem_id_ndx < (is_other_type_fes_fem? nof_fem_fes_per_group_other_type:nof_fem_fes_per_group)) 
                                 && (forbidden_fem_cycle2_found == FALSE); ++fem_id_ndx)
            {
                /* 
                 * Compute when the other FES/FEM is at higher priority
                 */
                if (!is_other_type_fes_fem) {
                    /* Between FEMs or FESs, cycle 1 higher priority */
                    is_higher_prio = (other_cycle == 1);
                }
                else if (is_for_fem && (other_cycle == 0)) {
                    /* FEMs at cycle 1 are always higher priority */
                    is_higher_prio = FALSE;
                }
                else if (is_for_fem && (other_cycle == 1)) {
                    /* FEMs at cycle 0 are higher priority if FES < 16 */
                    is_higher_prio = fem_fes_group_ndx;
                }
                else if ((!is_for_fem) && (other_cycle == 1)) {
                    /* FESs at cycle 0 are lowest priority */
                    is_higher_prio = TRUE;
                }
                else /* (!is_for_fem) && (other_cycle == 0) */ {
                    /* FES at cycle 1 are more important than FEM cycle 0 but not cycle 1*/
                    is_higher_prio = fem_fes_group_ndx;
                }
                /* Cycle of this FES / FEM */
                cycle_curr = is_other_type_fes_fem? fem_fes_group_ndx:other_cycle;

                /* 
                 * See if this FEM or FES is blocking
                 */
                SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry_high);
                SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry_low);
                SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
                /* Get the current FES/FEM absolute index */
                fes_id_indx = fem_id_ndx + (cycle_curr * (is_other_type_fes_fem? nof_fem_fes_per_group_other_type: nof_fem_fes_per_group));
                if (is_for_fem? (!is_other_type_fes_fem): is_other_type_fes_fem) {
                    /* The object to compare with is a FEM */
                  res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                          unit,
                          stage,
                          fem_pgm_id,
                          fes_id_indx,
                          &fem_entry
                        );
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
                }
                else /* For FES */ {
                    if(fes_info[fes_id_indx].is_used) {
                        fem_entry.is_for_entry = FALSE;
                        fem_entry.db_id = fes_info[fes_id_indx].db_id;
                        fem_entry.action_type[0] = fes_info[fes_id_indx].action_type;

                        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.db_info.prio.get(unit, stage, fes_info[fes_id_indx].db_id, &fem_entry.db_strength);
                        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);
                    }
                }
                /*
                 * Switch the Higher FEM info according to the priority
                 */
                if (is_higher_prio)
                {
                  ARAD_COPY(&fem_entry_high, &fem_entry, SOC_PPC_FP_FEM_ENTRY, 1);
                  ARAD_COPY(&fem_entry_low, &fem_entry_reference, SOC_PPC_FP_FEM_ENTRY, 1);
                }
                else
                {
                  ARAD_COPY(&fem_entry_high, &fem_entry_reference, SOC_PPC_FP_FEM_ENTRY, 1);
                  ARAD_COPY(&fem_entry_low, &fem_entry, SOC_PPC_FP_FEM_ENTRY, 1);
                }

                res = arad_pp_fp_fem_is_fem_blocking_get(
                      unit,
                      &fem_entry_high,
                      &fem_entry_low,
                      &forbidden_fem_cycle2_found
                    ) ;
                SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            }
        }
    }
  }
  if (forbidden_fem_cycle2_found == TRUE)
  {
    *place_found = FALSE;
    if (entry_ndx) {
      if (reason_for_fail) {
        if (loc_reason_for_fail_len > 1) {
          uint32
            strlen_reason_for_fail ;
          sal_snprintf(
            loc_reason_for_fail,
            loc_reason_for_fail_len,
            "%s(): BLOCKING found for %s for action %d ('%s') on db %d\r\n"
            "  ==> fem_entry_high: db %d action %d db_strength %d\r\n"
            "  ==> fem_entry_low: db %d action %d db_strength %d\r\n",
            __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
            entry_ndx->action_type[0],
            SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[0]),entry_ndx->db_id,
            fem_entry_high.db_id,fem_entry_high.action_type[0],fem_entry_high.db_strength,
            fem_entry_low.db_id,fem_entry_low.action_type[0],fem_entry_low.db_strength
          ) ;
          loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
          /*
           * The following is not necessary when this contribution is last on
           * current procedure.
           */
          strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
          loc_reason_for_fail_len -= strlen_reason_for_fail ;
          loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
        }
      }
    }
    ARAD_PP_DO_NOTHING_AND_EXIT;
  } else {
    /*
     * If this FEM is not blocked then reset 'reason_for_fail' variables.
     * This is not really necessary here but is added for good practice.
     */
    reason_for_fail[0] = 0 ;
    loc_reason_for_fail = reason_for_fail ;
    loc_reason_for_fail_len = reason_for_fail_len ;
  }
  /*
   * No forbidden FEM found, now look if there are enough free FEMs
   * with the minimal size in the same cycle
   * Build a bitmap to fill all along
   */
  for (fem_id_ndx = 0; (fem_id_ndx < nof_fem_fes_per_group) && (fes_is_found == FALSE); ++fem_id_ndx)
  {
      if (is_for_fem) {
        /*
         *  Get the FEM info
         */
        SOC_PPC_FP_FEM_ENTRY_clear(&fem_entry);
        res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fem_entry.get(
                unit,
                stage,
                fem_pgm_id,
                fem_id_ndx + (fem_info->cycle_id * nof_fem_fes_per_group),
                &fem_entry
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        fem_action_type_bmp[fem_id_ndx] = fem_entry.action_type[0];
      }
      else /* For FES */ {
          fes_id_indx = fem_id_ndx + (fem_info->cycle_id * nof_fem_fes_per_group);
          if (!fes_info[fes_id_indx].is_used) {
              fes_is_found = TRUE;
              LOG_DEBUG(BSL_LS_SOC_FP,
                        (BSL_META_U(unit,
                                    "   "
                                    "FES found for db_id:%d, entry:%d, action: %s , FES-ID: %d\n\r"), 
                         entry_ndx->db_id, entry_ndx->entry_id, 
                         SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), fes_id_indx));
          }
      }
  }

  for (action_ndx = 0; (action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX); action_ndx ++)
  {
      /* only the first action (action 0) is relevant for the FES */
      if ((!is_for_fem) && action_ndx) {
          break;
      }

    if (entry_ndx->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    fem_is_found = FALSE;
    for (fem_id_ndx = 0; (fem_id_ndx < nof_fem_fes_per_group) && (fem_is_found == FALSE) && is_for_fem; ++fem_id_ndx)
    {
      /*
       *  Check the FEM has sufficient place to copy all the needed bits
       *  Get the FEM size and the action type size
       */
      ARAD_PMF_FEM_NDX_clear(&fem);
      fem.cycle_ndx = fem_info->cycle_id;
      fem.id = fem_id_ndx;
      res = arad_pmf_fem_output_size_get(
              unit,
              &fem,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

      res = arad_pp_fp_action_type_max_size_get(
              unit,
              entry_ndx->action_type[action_ndx],
              &action_size_in_bits_max,
              &action_size_in_bits_in_fem
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      /* Avoid FEM 0 / 1 when the Base-offset is positive */
      if (action_size_in_bits_in_fem > output_size_in_bits)
      {
          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "   "
                                "FEM skipped for action: %s (size: %d). FEM-ID: %d, max-size %d  is too small \n\r"), 
                     SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), action_size_in_bits_in_fem,
                     fem_id_ndx, output_size_in_bits));
        continue;
      }

      if ((output_size_in_bits == 4) && (entry_ndx->is_base_positive[action_ndx] == TRUE))
      {
          LOG_DEBUG(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "   "
                                "FEM skipped for action: %s (size: %d). FEM-ID: %d, max-size %d  is 4b and has a positive base (0/1) %d \n\r"), 
                     SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), action_size_in_bits_in_fem,
                     fem_id_ndx, output_size_in_bits,
                     entry_ndx->is_base_positive[action_ndx]));
        continue;
      }

      if (fem_action_type_bmp[fem_id_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
      {
        fem_is_found = TRUE;
        fem_action_type_bmp[fem_id_ndx] = entry_ndx->action_type[action_ndx];
        LOG_DEBUG(BSL_LS_SOC_FP,
                  (BSL_META_U(unit,
                              "   "
                              "FEM found for db_id:%d, entry:%d, action: %s (size: %d). FEM-ID: %d, max-size %d  \n\r"), 
                   entry_ndx->db_id, entry_ndx->entry_id, 
                   SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[action_ndx]), action_size_in_bits_in_fem,
                   fem_id_ndx, output_size_in_bits));
      }
    }
    if ( (is_for_fem && (fem_is_found == FALSE)) || ((!is_for_fem) && (!fes_is_found))) {
      *place_found = FALSE;
      if (entry_ndx) {
        if (reason_for_fail) {
          if (loc_reason_for_fail_len > 1) {
            uint32
              strlen_reason_for_fail ;
            sal_snprintf(
              loc_reason_for_fail,
              loc_reason_for_fail_len,
              "%s(): NO SPACE found for %s for action %d ('%s') on db %d\r\n",
              __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
              entry_ndx->action_type[0],
              SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[0]),entry_ndx->db_id
            ) ;
            loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
            /*
             * The following is not necessary when this contribution is last on
             * current procedure.
             */
            strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
            loc_reason_for_fail_len -= strlen_reason_for_fail ;
            loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
          }
        }
      }
      ARAD_PP_DO_NOTHING_AND_EXIT;
    } else {
      /*
       * If this FEM is not blocked then reset 'reason_for_fail' variables.
       * This is not really necessary here but is added for good practice.
       */
      reason_for_fail[0] = 0 ;
      loc_reason_for_fail = reason_for_fail ;
      loc_reason_for_fail_len = reason_for_fail_len ;
    }
  }

  *place_found = TRUE;

exit:
  /*
   * Contribute this procedure's signature to 'reason_for_fail' mechanism.
   */
  if (*place_found == 0) {
    if (entry_ndx) {
      if (reason_for_fail) {
        if (loc_reason_for_fail_len > 1) {
          uint32
            strlen_reason_for_fail ;
          sal_snprintf(
            loc_reason_for_fail,
            loc_reason_for_fail_len,
            "%s(): No %s found for action '%s'\r\n",
            __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
            SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[0])
          ) ;
          loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
          /*
           * The following is not necessary when this contribution is last on
           * current procedure.
           */
          strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
          loc_reason_for_fail_len -= strlen_reason_for_fail ;
          loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
        }
      }
    }
  } else {
    /*
     * If place for FES/FEM was found then clear potential contributions to 'reason_for_fail'
     */
    if (reason_for_fail) {
      loc_reason_for_fail_len = reason_for_fail_len ;
      loc_reason_for_fail = reason_for_fail ;
      reason_for_fail[0] = 0 ;
    }
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_place_get_for_cycle()", 0, 0);
}
/*********************************************************************
*     Check out if there is an empty FEM for this entry.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_fp_fem_is_place_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY   *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE   *fem_info,
    SOC_SAND_IN  uint8                  is_for_tm,
    SOC_SAND_IN  uint8                  fem_pgm_id,
    SOC_SAND_INOUT ARAD_PMF_FES         fes_info[ARAD_PMF_LOW_LEVEL_NOF_FESS],
    SOC_SAND_OUT uint8                  *place_found,
    SOC_SAND_OUT char                   *reason_for_fail,
    SOC_SAND_IN int32                   reason_for_fail_len
  )
{
  uint32
    program_rsrc = 0,
    res = SOC_SAND_OK;
  uint32
    pmf_pgm_ndx_min,
    pmf_pgm_ndx_max,
    pmf_pgm_ndx,
    cycle_ndx,
    fes_idx;
  uint8
    is_for_fem,
    fem_is_found_lcl,
    fem_is_found = FALSE;
  SOC_PPC_FP_FEM_CYCLE
    fem_info_cycle;
  ARAD_PMF_FES         
    fes_info_lcl[ARAD_PMF_LOW_LEVEL_NOF_FESS]; /* The current FES info if FES, per PMF-Program if FEM */
  SOC_PPC_FP_DATABASE_STAGE
    stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 
  char
    *loc_reason_for_fail ;
  int32
    loc_reason_for_fail_len ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_PLACE_GET_UNSAFE);

  loc_reason_for_fail = reason_for_fail ;
  loc_reason_for_fail_len = reason_for_fail_len ;

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fem_info);
  SOC_SAND_CHECK_NULL_INPUT(place_found);
  SOC_SAND_CHECK_NULL_INPUT(reason_for_fail) ;

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "   "
                        "Find FEM for db_id:%d, entry:%d, with Info: cycle %d, cycle-fixed: %d  \n\r"), 
             entry_ndx->db_id, entry_ndx->entry_id, fem_info->cycle_id, fem_info->is_cycle_fixed));

  /*
   * Verify
   */
  res = arad_pp_fp_fem_is_place_get_verify(
          unit,
          entry_ndx,
          fem_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Copy in case of FES the FES-info*/
  is_for_fem = entry_ndx->is_for_entry;
  if (!is_for_fem) {
      sal_memcpy(fes_info_lcl, fes_info, ARAD_PMF_LOW_LEVEL_NOF_FESS * sizeof(ARAD_PMF_FES));
  } else {
      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.progs.get(unit, stage, 0, &program_rsrc);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  }

  res = arad_pmf_prog_select_pmf_pgm_borders_get(
            unit,
            stage,
            is_for_tm /* is_for_tm */, 
            &pmf_pgm_ndx_min,
            &pmf_pgm_ndx_max
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  /*
   * If cycle fixed, try it.
   * If not, try both cycles and return success if at least one succeeded
   */
  /* using the macro SOC_DPP_DEFS_MAX can compare definitions of the same value */
  /* coverity[same_on_both_sides] */
  for (cycle_ndx = 0; (cycle_ndx < ARAD_PMF_NOF_CYCLES) && (fem_is_found == FALSE); ++cycle_ndx)
  {
    if ((fem_info->is_cycle_fixed == TRUE) && (cycle_ndx != fem_info->cycle_id)) {
      continue;
    }
    SOC_PPC_FP_FEM_CYCLE_clear(&fem_info_cycle);
    fem_info_cycle.is_cycle_fixed = TRUE; /* Look only for this cycle */
    fem_info_cycle.cycle_id = SOC_SAND_NUM2BOOL(cycle_ndx);

    
    fem_is_found_lcl = TRUE;
    for (pmf_pgm_ndx = pmf_pgm_ndx_min; (pmf_pgm_ndx < pmf_pgm_ndx_max) && (fem_is_found_lcl == TRUE); ++pmf_pgm_ndx) {
        /* Go over this only once for FES */
        if ((!is_for_fem) && (pmf_pgm_ndx != pmf_pgm_ndx_min)) {
            break;
        }

        /* For FEM, get the FES info. */
        if (is_for_fem) {
            if (!SHR_BITGET(&program_rsrc, pmf_pgm_ndx)) {
                /* Program not valid, continue */
                continue;
            }

            for(fes_idx = 0; fes_idx < ARAD_PMF_LOW_LEVEL_NOF_FESS; ++fes_idx) {
                res = sw_state_access[unit].dpp.soc.arad.tm.pmf.pgm_fes.get(
                          unit,
                          stage,
                          pmf_pgm_ndx,
                          fes_idx,
                          &fes_info_lcl[fes_idx]
                      );
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 16, exit);
            }
        }

        res = arad_pp_fp_fem_is_place_get_for_cycle(
                unit,
                entry_ndx,
                &fem_info_cycle,
                fem_pgm_id,
                fes_info_lcl,               
                &fem_is_found_lcl,
                loc_reason_for_fail,
                loc_reason_for_fail_len
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        /*
         * Make sure 'reason_for_fail' variables are updated after contribution
         * by last called procedure in chain (arad_pp_fp_fem_is_place_get_for_cycle)
         */
        if (loc_reason_for_fail) {
            if (loc_reason_for_fail_len > 1) {
                if (!fem_is_found_lcl) {
                    uint32
                      strlen_reason_for_fail ;
                    strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
                    loc_reason_for_fail_len -= strlen_reason_for_fail ;
                    loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
		    if (loc_reason_for_fail_len > 1) {
                        if (entry_ndx) {
                            sal_snprintf(
                              loc_reason_for_fail,
                              loc_reason_for_fail_len,
                              "%s(): No %s found on cycle %d program %d\r\n",
                              __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
                              cycle_ndx,pmf_pgm_ndx
                            ) ;
                            loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
                            /*
                             * The following is not necessary when this contribution is last on
                             * current procedure.
                             */
                            strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
                            loc_reason_for_fail_len -= strlen_reason_for_fail ;
                            loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
                        }
                    }
                } else {
                    /*
                     * If FEM was found then get rid of any contributions by last
                     * called procedure (arad_pp_fp_fem_is_place_get_for_cycle)
                     */
                    loc_reason_for_fail_len = reason_for_fail_len ;
                    loc_reason_for_fail = reason_for_fail ;
                    reason_for_fail[0] = 0 ;
                }
            }
        }
    }

    /* False if failed for a Program, True otherwise */
    fem_is_found = fem_is_found_lcl;
  }

  *place_found = fem_is_found;

exit:
  /*
   * Contribute this procedure's signature to 'reason_for_fail' mechanism.
   */
  if (*place_found == 0) {
    if (entry_ndx) {
      if (reason_for_fail) {
        if (loc_reason_for_fail_len > 1) {
          uint32
            strlen_reason_for_fail ;
          sal_snprintf(
            loc_reason_for_fail,
            loc_reason_for_fail_len,
            "%s(): No %s found for action '%s'\r\n",
            __func__,(entry_ndx->is_for_entry ? "FEM" : "FES"),
            SOC_PPC_FP_ACTION_TYPE_to_string(entry_ndx->action_type[0])
          ) ;
          loc_reason_for_fail[loc_reason_for_fail_len - 1] = 0 ;
          /*
           * The following is not necessary when this contribution is last on
           * current procedure.
           */
          strlen_reason_for_fail = sal_strlen(loc_reason_for_fail) ;
          loc_reason_for_fail_len -= strlen_reason_for_fail ;
          loc_reason_for_fail = &loc_reason_for_fail[strlen_reason_for_fail] ;
        }
      }
    }
  } else {
    /*
     * If place for FES/FEM was found then clear potential contributions to 'reason_for_fail'
     */
    if (reason_for_fail) {
      loc_reason_for_fail_len = reason_for_fail_len ;
      loc_reason_for_fail = reason_for_fail ;
      reason_for_fail[0] = 0 ;
    }
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_place_get_unsafe()", 0, 0);
}

uint32
  arad_pp_fp_fem_is_place_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY           *entry_ndx,
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE            *fem_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FP_FEM_IS_PLACE_GET_VERIFY);

  res = SOC_PPC_FP_FEM_ENTRY_verify(unit, entry_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FP_FEM_CYCLE, fem_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_fp_fem_is_place_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_fp_fem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_fp_fem_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_fp_fem;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_fp_fem module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_fp_fem_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_fp_fem;
}
uint32
  SOC_PPC_FP_FEM_ENTRY_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY *info
  )
{
  uint32
    action_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_strength, ARAD_PP_FP_FEM_DB_STRENGTH_MAX, ARAD_PP_FP_FEM_DB_STRENGTH_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->db_id, ARAD_PP_FP_FEM_DB_ID_MAX, ARAD_PP_FP_FEM_DB_ID_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->entry_strength, SOC_PPC_FP_FEM_ENTRY_STRENGTH_MAX, SOC_PPC_FP_FEM_ENTRY_STRENGTH_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->entry_id, SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit), SOC_PPC_FP_FEM_ENTRY_ID_OUT_OF_RANGE_ERR, 14, exit);
  for (action_ndx = 0; action_ndx < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX; action_ndx ++)
  {
      if (action_ndx && (!info->is_for_entry)) {
          /* FES, single action */
          break;
      }

    if (info->action_type[action_ndx] == SOC_PPC_FP_ACTION_TYPE_INVALID)
    {
      continue;
    }

    SOC_SAND_ERR_IF_ABOVE_MAX(info->action_type[action_ndx], ARAD_PP_FP_FEM_ACTION_TYPE_MAX, SOC_PPC_FP_ACTION_TYPES_OUT_OF_RANGE_ERR, 20 + action_ndx, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FP_FEM_ENTRY_verify()",0,0);
}

uint32
  SOC_PPC_FP_FEM_CYCLE_verify(
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FP_FEM_CYCLE_verify()",0,0);
}


void
  ARAD_PP_FEM_ACTIONS_CONSTRAINT_clear(
    SOC_SAND_OUT ARAD_PP_FEM_ACTIONS_CONSTRAINT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(ARAD_PP_FEM_ACTIONS_CONSTRAINT));
  info->cycle = ARAD_PP_FP_KEY_CYCLE_ANY;
  info->action_size = ARAD_TCAM_ACTION_SIZE_SECOND_20_BITS;


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

#endif /* of #if defined(BCM_88650_A0) */

