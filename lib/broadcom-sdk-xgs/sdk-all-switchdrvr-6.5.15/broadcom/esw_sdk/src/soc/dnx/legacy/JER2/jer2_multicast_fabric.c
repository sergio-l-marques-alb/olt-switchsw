/** \file jer2_multicast_fabric.c
 *
 * Functions for handling fabric multicast.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_MULTICAST

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/mcm/memregs.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/legacy/SAND/Utils/sand_conv.h>
#include <soc/dnx/legacy/mbcm.h>
#include <soc/dnx/legacy/JER2/jer2_multicast_fabric.h>


#include <bcm_int/dnx/cosq/ingress/iqs.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/*
 * Numeric correction used in Fabric Multicast rate calculation
 */
#define JER2_FMC_RATE_DELTA_CONST  (7)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/**
 * \brief 
 *   Fabric Multicast classes.
 *
 * \remark
 *   FMQs are devided to classes, each class can be configured seperately.
 */
typedef enum
{
    /*
     * Invalid Class
     */
    JER2_FMC_CLASS_INVALID = -1,
    /*
     * Class 0
     */
    JER2_FMC_CLASS_0 = 0,
    /*
     * Class 1
     */
    JER2_FMC_CLASS_1 = 1,
    /*
     * Class 2
     */
    JER2_FMC_CLASS_2 = 2,
    /*
     * Class 3
     */
    JER2_FMC_CLASS_3 = 3,
    /*
     * Number of classes
     */
    NUM_JER2_FMC_CLASS = 4,
} JER2_FMC_CLASS;


/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/**
 * \brief
 *   Verify FMC info.
 * \param [in] unit -
 *   The unit number.
 * \param [in] info -
 *   FMC info to check validity.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   jer2_mult_fabric_credit_source_set.
 */
static int
  jer2_mult_fabric_credit_source_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  SOC_DNX_MULT_FABRIC_INFO    *info);

/**
 * \brief
 *   Set Best Effort FMQ configurations.
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   The core where the BE-FMQ resides.
 * \param [in] info -
 *   BE-FMQ configurations.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_be_set(
      DNX_SAND_IN  int                          unit,
      DNX_SAND_IN  int                          core,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_BE_INFO    *info);

/**
 * \brief
 *   Get Best Effort FMQ configurations.
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   The core where the BE-FMQ resides.
 * \param [out] info -
 *   BE-FMQ configurations.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_be_get(
      DNX_SAND_IN  int                       unit,
      DNX_SAND_IN  int                       core,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_BE_INFO *info);

/**
 * \brief
 *   Set Guaranteed FMQ configurations.
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   The core where the G-FMQ resides.
 * \param [in] info -
 *   G-FMQ configurations.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_gu_set(
      DNX_SAND_IN  int                       unit,
      DNX_SAND_IN  int                       core,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_GR_INFO *info);

/**
 * \brief
 *   Get Guaranteed FMQ configurations.
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   The core where the G-FMQ resides.
 * \param [out] info -
 *   G-FMQ configurations.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_gu_get(
      DNX_SAND_IN  int                       unit,
      DNX_SAND_IN  int                       core,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_GR_INFO *info);

/**
 * \brief
 *   Set Best Effort FMQ WFQ.
 * \param [in] unit -
 *   The unit number.
 * \param [in] info -
 *   BE-FMQ configurations.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_be_wfq_set(
      DNX_SAND_IN  int                          unit,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_BE_INFO     *info);

/**
 * \brief
 *   Get Best Effort FMQ WFQ.
 * \param [in] unit -
 *   The unit number.
 * \param [out] info -
 *   BE-FMQ configurations.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_be_wfq_get(
      DNX_SAND_IN  int                           unit,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_BE_INFO       *info);

/**
 * \brief
 *   Set FMQ Shaper.
 * \param [in] unit -
 *   The unit number.
 * \param [in] reg_desc -
 *   The register of the shaper.
 * \param [in] max_burst_f -
 *   The field of max burst.
 * \param [in] shaper_rate_f -
 *   The field of shaper's rate.
 * \param [in] info -
 *   Shaper configurations.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_shaper_set(
      DNX_SAND_IN  int                              unit,
      DNX_SAND_IN  soc_reg_t                        reg_desc,
      DNX_SAND_IN  soc_field_t                      max_burst_f,
      DNX_SAND_IN  soc_field_t                      shaper_rate_f,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_SHAPER_INFO     *info);

/**
 * \brief
 *   Get FMQ Shaper.
 * \param [in] unit -
 *   The unit number.
 * \param [in] reg_desc -
 *   The register of the shaper.
 * \param [in] max_burst_f -
 *   The field of max burst.
 * \param [in] shaper_rate_f -
 *   The field of shaper's rate.
 * \param [out] info -
 *   Shaper configurations.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_shaper_get(
      DNX_SAND_IN  int                              unit,
      DNX_SAND_IN  soc_reg_t                        reg_desc,
      DNX_SAND_IN  soc_field_t                      max_burst_f,
      DNX_SAND_IN  soc_field_t                      shaper_rate_f,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_SHAPER_INFO     *info);

/**
 * \brief
 *   Set FMC class credit source.
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   The core id.
 * \param [in] multicast_class_ndx -
 *   The FMC class to set credit source for.
 * \param [in] info -
 *   Credit source information.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   FMQs have 4 classes. Each class can receive credits directly (simple
 *   FMQs mode) or through scheduling scheme (enhance FMQs mode).
 *   On enhance mode, the scheduling scheme is built upon 4 HRs.
 *   So need to configure if credits to the class can be received directly
 *   or through scheduling scheme, and if scheduling scheme than need the
 *   HR ids.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_port_set(
      DNX_SAND_IN  int                          unit,
      DNX_SAND_IN  int                          core,
      DNX_SAND_IN  JER2_FMC_CLASS         multicast_class_ndx,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO   *info);

/**
 * \brief
 *   Get FMC class credit source.
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   The core id.
 * \param [in] multicast_class_ndx -
 *   The FMC class to get credit source for.
 * \param [out] info -
 *   Credit source information.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   FMQs have 4 classes. Each class can receive credits directly (simple
 *   FMQs mode) or through scheduling scheme (enhance FMQs mode).
 *   On enhance mode, the scheduling scheme is built upon 4 HRs.
 *   So need to configure if credits to the class can be received directly
 *   or through scheduling scheme, and if scheduling scheme than need the
 *   HR ids.
 * \see
 *   None.
 */
static int
    jer2_mult_fabric_credit_source_port_get(
      DNX_SAND_IN  int                          unit,
      DNX_SAND_IN  int                          core,
      DNX_SAND_IN  JER2_FMC_CLASS         multicast_class_ndx,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO   *info);

/**
 * \brief
 *   Internal Rate to Clock conversion.
 * \param [in] unit -
 *   The unit number.
 * \param [in] rate_kbps -
 *   Rate in kbps.
 * \param [out] clk_interval -
 *   Clock interval to write in HW.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   Used for FMC configurations.
 * \see
 *   None.
 */
static int
  jer2_intern_rate2clock(
    DNX_SAND_IN  int unit,
    DNX_SAND_IN  uint32  rate_kbps,
    DNX_SAND_OUT uint32  *clk_interval);

/**
 * \brief
 *   Internal Clock to Rate conversion.
 * \param [in] unit -
 *   The unit number.
 * \param [in] clk_interval -
 *   Clock interval that was written in HW.
 * \param [out] rate_kbps -
 *   Rate in kbps.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   Used for FMC configurations.
 * \see
 *   None.
 */
static int
  jer2_intern_clock2rate(
    DNX_SAND_IN  int unit,
    DNX_SAND_IN  uint32  clk_interval,
    DNX_SAND_OUT uint32  *rate_kbps);

/*
 * See declaration.
 */
static int
  jer2_mult_fabric_credit_source_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  SOC_DNX_MULT_FABRIC_INFO    *info
  )
{
    uint8
        sch_in = FALSE,
        sch_ou = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    sch_in = TRUE;
    sch_in = sch_in && (info->best_effort.be_sch[JER2_FMC_CLASS_0].be_sch.multicast_class_valid);
    sch_in = sch_in && (info->best_effort.be_sch[JER2_FMC_CLASS_1].be_sch.multicast_class_valid);
    sch_in = sch_in && (info->best_effort.be_sch[JER2_FMC_CLASS_2].be_sch.multicast_class_valid);
    sch_in = sch_in && (info->guaranteed.gr_sch.multicast_class_valid);

    sch_ou = TRUE;
    sch_ou = sch_ou && (info->credits_via_sch);

    if (((sch_in) && (!sch_ou)) ||
        ((!sch_in) && (sch_ou)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "JER2_MULT_FABRIC_ILLEGAL_CONF_ERR");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_be_wfq_set(
      DNX_SAND_IN  int                          unit,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_BE_INFO     *info
    )
{
    uint32
        wfq_enable = info->wfq_enable == TRUE ? 0x1 : 0x0,
        buffer = 0;
    int
        rv;
    int core = _SHR_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);

    /*
    * SP vs WFQ info
    * if SP do not write WFQ. Otherwise write.
    */

    rv = READ_IPS_BFMC_CLASS_CONFIGr(unit, core, &buffer);
    SHR_IF_ERR_EXIT(rv);

    soc_reg_field_set(unit, IPS_BFMC_CLASS_CONFIGr, &buffer, BFMC_WFQ_ENf, wfq_enable);

    if (wfq_enable)
    {
        /* note: fileds names doesn't match class names */
        soc_reg_field_set(unit, IPS_BFMC_CLASS_CONFIGr, &buffer, BFMC_CLASS_2_WEIGHTf, info->be_sch[JER2_FMC_CLASS_0].weight);
        soc_reg_field_set(unit, IPS_BFMC_CLASS_CONFIGr, &buffer, BFMC_CLASS_1_WEIGHTf, info->be_sch[JER2_FMC_CLASS_1].weight);
        soc_reg_field_set(unit, IPS_BFMC_CLASS_CONFIGr, &buffer, BFMC_CLASS_0_WEIGHTf, info->be_sch[JER2_FMC_CLASS_2].weight);
    }

    rv = WRITE_IPS_BFMC_CLASS_CONFIGr(unit, core, buffer);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_be_wfq_get(
      DNX_SAND_IN  int                           unit,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_BE_INFO       *info
    )
{
    uint32
        wfq_enable = 0,
        buffer = 0;
    int
        rv;
    int core = _SHR_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);

    info->be_sch[JER2_FMC_CLASS_0].weight = 0;
    info->be_sch[JER2_FMC_CLASS_1].weight = 0;
    info->be_sch[JER2_FMC_CLASS_2].weight = 0;

    /*
    * SP vs WFQ info
    * if SP do not write WFQ. Otherwise write.
    */
    rv = READ_IPS_BFMC_CLASS_CONFIGr(unit, core, &buffer);
    SHR_IF_ERR_EXIT(rv);

    wfq_enable = soc_reg_field_get(unit, IPS_BFMC_CLASS_CONFIGr, buffer, BFMC_WFQ_ENf);

    info->wfq_enable = (wfq_enable==0x1 ? TRUE : FALSE);

    if (info->wfq_enable)
    {
        /* note: fileds names doesn't match class names */
        info->be_sch[JER2_FMC_CLASS_0].weight = soc_reg_field_get(unit, IPS_BFMC_CLASS_CONFIGr, buffer, BFMC_CLASS_2_WEIGHTf);
        info->be_sch[JER2_FMC_CLASS_1].weight = soc_reg_field_get(unit, IPS_BFMC_CLASS_CONFIGr, buffer, BFMC_CLASS_1_WEIGHTf);
        info->be_sch[JER2_FMC_CLASS_2].weight = soc_reg_field_get(unit, IPS_BFMC_CLASS_CONFIGr, buffer, BFMC_CLASS_0_WEIGHTf);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
  jer2_intern_rate2clock(
    DNX_SAND_IN  int unit,
    DNX_SAND_IN  uint32  rate_kbps,
    DNX_SAND_OUT uint32  *clk_interval
  )
{
  shr_error_e
    ret = _SHR_E_NONE;
  uint32
    cr_size = 0,
    ticks_per_sec = 0,
    interval = 0;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &cr_size)) ;

  ticks_per_sec = DNXCMN_CORE_CLOCK_HZ_GET(unit);

  if (rate_kbps == 0)
  {
    *clk_interval = 0;
  }
  else
  {
    ret = dnx_sand_kbits_per_sec_to_clocks(
            rate_kbps,
            cr_size  * 8 /*clock resolution is 1/8*/,
            ticks_per_sec,
            &interval
          );
    SHR_IF_ERR_EXIT(ret);

    *clk_interval = interval - JER2_FMC_RATE_DELTA_CONST;
  }

exit:
  SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
  jer2_intern_clock2rate(
    DNX_SAND_IN  int unit,
    DNX_SAND_IN  uint32  clk_interval,
    DNX_SAND_OUT uint32  *rate_kbps
  )
{
  shr_error_e
    ret = _SHR_E_NONE;
  uint32
    cr_size = 0,
    ticks_per_sec = 0,
    interval = 0;

  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &cr_size));

  ticks_per_sec = DNXCMN_CORE_CLOCK_HZ_GET(unit);

  if (clk_interval == 0)
  {
    *rate_kbps = 0;
  }
  else
  {
    interval = (clk_interval + JER2_FMC_RATE_DELTA_CONST);

    ret = dnx_sand_clocks_to_kbits_per_sec(
            interval,
            cr_size * 8 /*clock resolution is 1/8*/,
            ticks_per_sec,
            rate_kbps
          );
    SHR_IF_ERR_EXIT(ret);
  }

exit:
  SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_shaper_set(
      DNX_SAND_IN  int                              unit,
      DNX_SAND_IN  soc_reg_t                        reg_desc,
      DNX_SAND_IN  soc_field_t                      max_burst_f,
      DNX_SAND_IN  soc_field_t                      shaper_rate_f,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_SHAPER_INFO     *info
    )
{
    int
        res;
    uint32
        rate = 0,
        buffer = 0;
    int 
        rv;
    int core = _SHR_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);

    rv = soc_reg32_get(unit, reg_desc, core, 0, &buffer);
    SHR_IF_ERR_EXIT(rv);

    soc_reg_field_set(unit ,reg_desc, &buffer, max_burst_f, info->max_burst);

    res = jer2_intern_rate2clock(
          unit,
          info->rate,
          &rate
        );
    SHR_IF_ERR_EXIT(res);

    soc_reg_field_set(unit ,reg_desc, &buffer, shaper_rate_f, rate);

    rv = soc_reg32_set(unit, reg_desc, core, 0, buffer);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_shaper_get(
      DNX_SAND_IN  int                              unit,
      DNX_SAND_IN  soc_reg_t                        reg_desc,
      DNX_SAND_IN  soc_field_t                      max_burst_f,
      DNX_SAND_IN  soc_field_t                      shaper_rate_f,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_SHAPER_INFO     *info
    )
{
    int
        res;
    uint32
        rate = 0,
        buffer = 0;
    int core = _SHR_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_reg32_get(unit, reg_desc, core, 0, &buffer));

    info->max_burst = soc_reg_field_get(unit , reg_desc, buffer, max_burst_f);
    rate = soc_reg_field_get(unit , reg_desc, buffer, shaper_rate_f);

    res = jer2_intern_clock2rate(
          unit,
          rate,
          &(info->rate)
        );
    SHR_IF_ERR_EXIT(res);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_be_set(
      DNX_SAND_IN  int                          unit,
      DNX_SAND_IN  int                          core,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_BE_INFO    *info
    )
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    /* SP and WFQ info */
    rv = jer2_mult_fabric_credit_source_be_wfq_set(
          unit,
          info
        );
    SHR_IF_ERR_EXIT(rv);

    /* Set shaper info */
    rv = jer2_mult_fabric_credit_source_shaper_set(
          unit,
          IPS_BFMC_SHAPER_CONFIGr,
          BFMC_MAX_BURSTf,
          BFMC_MAX_CRDT_RATEf,
          &(info->be_shaper)
        );
    SHR_IF_ERR_EXIT(rv);

    /* Set port info */
    rv = jer2_mult_fabric_credit_source_port_set(
          unit,
          core,
          JER2_FMC_CLASS_3,
          &(info->be_sch[JER2_FMC_CLASS_0].be_sch)
        );
    SHR_IF_ERR_EXIT(rv);

    rv = jer2_mult_fabric_credit_source_port_set(
          unit,
          core,
          JER2_FMC_CLASS_2,
          &(info->be_sch[JER2_FMC_CLASS_1].be_sch)
        );
    SHR_IF_ERR_EXIT(rv);

    rv = jer2_mult_fabric_credit_source_port_set(
          unit,
          core,
          JER2_FMC_CLASS_1,
          &(info->be_sch[JER2_FMC_CLASS_2].be_sch)
        );
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_port_set(
      DNX_SAND_IN  int                          unit,
      DNX_SAND_IN  int                          core,
      DNX_SAND_IN  JER2_FMC_CLASS         multicast_class_ndx,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO   *info
    )
{
    uint32
        mcast_class_port_id = info->mcast_class_hr_id;
    uint32
        multicast_class_valid = info->multicast_class_valid == TRUE ? 1 : 0,
        buffer = 0;
    soc_reg_t
        mcast_reg;
    soc_field_t
        mcast_class_port_id_fld[NUM_JER2_FMC_CLASS] 
            = {MCAST_GFMC_PORT_IDf, MCAST_BFMC_1_PORT_IDf, MCAST_BFMC_2_PORT_IDf, MCAST_BFMC_3_PORT_IDf};
    soc_field_t
        multicast_class_valid_fld[NUM_JER2_FMC_CLASS] 
            = {MULTICAST_GFMC_ENABLEf, MULTICAST_BFMC_1_ENABLEf, MULTICAST_BFMC_2_ENABLEf, MULTICAST_BFMC_3_ENABLEf};

    SHR_FUNC_INIT_VARS(unit);

    switch (multicast_class_ndx) {
        case JER2_FMC_CLASS_0:
        case JER2_FMC_CLASS_1:
            mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r;
            break;
        case JER2_FMC_CLASS_2:
        case JER2_FMC_CLASS_3:
            mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "JER2_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR");
    }

    SHR_IF_ERR_EXIT(soc_reg32_get(unit, mcast_reg, core, 0, &buffer));

    if(multicast_class_valid) {
        soc_reg_field_set(unit, mcast_reg, &buffer, mcast_class_port_id_fld[multicast_class_ndx], mcast_class_port_id);
    }

    soc_reg_field_set(unit, mcast_reg, &buffer, multicast_class_valid_fld[multicast_class_ndx], multicast_class_valid);

    SHR_IF_ERR_EXIT(soc_reg32_set(unit, mcast_reg, core, 0, buffer));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_port_get(
      DNX_SAND_IN  int                          unit,
      DNX_SAND_IN  int                          core,
      DNX_SAND_IN  JER2_FMC_CLASS         multicast_class_ndx,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_CLASS_SCH_INFO   *info
    )
{
    uint32
        multicast_class_valid = 0,
        buffer = 0,
        hr_flow_id;
    soc_reg_t
        mcast_reg;
    soc_field_t
        mcast_class_port_id_fld[NUM_JER2_FMC_CLASS] 
            = {MCAST_GFMC_PORT_IDf, MCAST_BFMC_1_PORT_IDf, MCAST_BFMC_2_PORT_IDf, MCAST_BFMC_3_PORT_IDf},
        multicast_class_valid_fld[NUM_JER2_FMC_CLASS] 
            = {MULTICAST_GFMC_ENABLEf, MULTICAST_BFMC_1_ENABLEf, MULTICAST_BFMC_2_ENABLEf, MULTICAST_BFMC_3_ENABLEf};

    SHR_FUNC_INIT_VARS(unit);

    switch (multicast_class_ndx) {
        case JER2_FMC_CLASS_0:
        case JER2_FMC_CLASS_1:
            mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_0r;
            break;
        case JER2_FMC_CLASS_2:
        case JER2_FMC_CLASS_3:
            mcast_reg = SCH_SCH_FABRIC_MULTICAST_PORT_CONFIGURATION_REGISTER_1r;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "JER2_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR");
            break;
    }
  
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, mcast_reg, core,  0, &buffer));

    hr_flow_id = soc_reg_field_get(unit, mcast_reg, buffer, mcast_class_port_id_fld[multicast_class_ndx]);

    info->mcast_class_hr_id = hr_flow_id;

    multicast_class_valid = soc_reg_field_get(unit, mcast_reg, buffer, multicast_class_valid_fld[multicast_class_ndx]);
    info->multicast_class_valid = multicast_class_valid == 1 ? TRUE : FALSE;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_be_get(
      DNX_SAND_IN  int                       unit,
      DNX_SAND_IN  int                       core,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_BE_INFO *info
    )
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /* Get SP and WFQ info */
    rv = jer2_mult_fabric_credit_source_be_wfq_get(
          unit,
          info
        );
    SHR_IF_ERR_EXIT(rv);

    /* Get shaper info */
    rv = jer2_mult_fabric_credit_source_shaper_get(
          unit,
          IPS_BFMC_SHAPER_CONFIGr,
          BFMC_MAX_BURSTf,
          BFMC_MAX_CRDT_RATEf,
          &(info->be_shaper)
        );
    SHR_IF_ERR_EXIT(rv);

    /*
    * Get port info- inversion in the scheduler for the order
    * of importance of the multicast classes MC3 < MC2 < MC1 (best effort)
    */
    rv = jer2_mult_fabric_credit_source_port_get(
          unit,
          core,
          JER2_FMC_CLASS_3,
          &(info->be_sch[JER2_FMC_CLASS_0].be_sch)
        );
    SHR_IF_ERR_EXIT(rv);

    rv = jer2_mult_fabric_credit_source_port_get(
          unit,
          core,
          JER2_FMC_CLASS_2,
          &(info->be_sch[JER2_FMC_CLASS_1].be_sch)
        );
    SHR_IF_ERR_EXIT(rv);

    rv = jer2_mult_fabric_credit_source_port_get(
          unit,
          core,
          JER2_FMC_CLASS_1,
          &(info->be_sch[JER2_FMC_CLASS_2].be_sch)
        );
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_gu_set(
      DNX_SAND_IN  int                       unit,
      DNX_SAND_IN  int                       core,
      DNX_SAND_IN  SOC_DNX_MULT_FABRIC_GR_INFO *info
    )
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /* Set shaper info */
    rv = jer2_mult_fabric_credit_source_shaper_set(
          unit,
          IPS_GFMC_SHAPER_CONFIGr,
          GFMC_MAX_BURSTf,
          GFMC_MAX_CRDT_RATEf,
          &(info->gr_shaper)
        );
    SHR_IF_ERR_EXIT(rv);

    /*
    * Set port info - inversion in the scheduler for the order of importance of the
    * multicast classes: the guaranteed multicast class is 0
    */
    rv = jer2_mult_fabric_credit_source_port_set(
          unit,
          core,
          JER2_FMC_CLASS_0,
          &(info->gr_sch)
        );
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See declaration.
 */
static int
    jer2_mult_fabric_credit_source_gu_get(
      DNX_SAND_IN  int                       unit,
      DNX_SAND_IN  int                       core,
      DNX_SAND_OUT SOC_DNX_MULT_FABRIC_GR_INFO *info
    )
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /* Get shaper info */
    rv = jer2_mult_fabric_credit_source_shaper_get(
          unit,
          IPS_GFMC_SHAPER_CONFIGr,
          GFMC_MAX_BURSTf,
          GFMC_MAX_CRDT_RATEf,
          &(info->gr_shaper)
        );
    SHR_IF_ERR_EXIT(rv);

    /*
    * Set port info- inversion in the scheduler for the order of importance of the
    * multicast classes: the guaranteed multicast class is 0
    */
    rv = jer2_mult_fabric_credit_source_port_get(
          unit,
          core,
          JER2_FMC_CLASS_0,
          &(info->gr_sch)
        );
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
int
  jer2_mult_fabric_credit_source_set(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  int                        core,
    DNX_SAND_IN  SOC_DNX_MULT_FABRIC_INFO      *info
  )
{
    int
        rv;
    uint32
        buffer = 0,
        rate = 0;
   
    SHR_FUNC_INIT_VARS(unit);

    rv = jer2_mult_fabric_credit_source_verify(unit, info);
    SHR_IF_ERR_EXIT(rv);

    rv = jer2_intern_rate2clock(
          unit,
          info->max_rate,
          &rate
        );
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, core, 0, FMC_CREDITS_FROM_SCHf,  info->credits_via_sch));

    SHR_IF_ERR_EXIT(READ_IPS_FMC_SHAPER_CONFIGr(unit, core ,&buffer));
    soc_reg_field_set(unit, IPS_FMC_SHAPER_CONFIGr, &buffer, FMC_MAX_CRDT_RATEf, rate);
    soc_reg_field_set(unit, IPS_FMC_SHAPER_CONFIGr, &buffer, FMC_MAX_BURSTf, info->max_burst);
    SHR_IF_ERR_EXIT(WRITE_IPS_FMC_SHAPER_CONFIGr(unit, core ,buffer));

    rv = jer2_mult_fabric_credit_source_be_set(
          unit,
          core,
          &(info->best_effort)
        );
    SHR_IF_ERR_EXIT(rv);

    rv = jer2_mult_fabric_credit_source_gu_set(
          unit,
          core,
          &(info->guaranteed)
        );
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
int
  jer2_mult_fabric_credit_source_get(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  int                        core,
    DNX_SAND_OUT SOC_DNX_MULT_FABRIC_INFO      *info
  )
{
    int
        rv;
    uint32
        buffer = 0,
        credits_via_sch = 0,
        rate = 0;
   
    SHR_FUNC_INIT_VARS(unit);

    SOC_DNX_MULT_FABRIC_INFO_clear(info);

    rv = jer2_mult_fabric_credit_source_gu_get(
          unit,
          core,
          &(info->guaranteed)
        );
    SHR_IF_ERR_EXIT(rv);

    rv = jer2_mult_fabric_credit_source_be_get(
          unit,
          core,
          &(info->best_effort)
        );
    SHR_IF_ERR_EXIT(rv);

    rv = READ_IPS_FMC_SHAPER_CONFIGr(unit, core ,&buffer);
    SHR_IF_ERR_EXIT(rv);

    rate = soc_reg_field_get(unit, IPS_FMC_SHAPER_CONFIGr, buffer, FMC_MAX_CRDT_RATEf);

    rv = jer2_intern_clock2rate(
          unit,
          rate,
          &(info->max_rate)
        );
    SHR_IF_ERR_EXIT(rv);

    info->max_burst = soc_reg_field_get(unit, IPS_FMC_SHAPER_CONFIGr, buffer, FMC_MAX_BURSTf);

    SHR_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, core, 0, FMC_CREDITS_FROM_SCHf, &credits_via_sch));
    info->credits_via_sch = (credits_via_sch ? TRUE : FALSE);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
int
  jer2_mult_fabric_enhanced_set(
    DNX_SAND_IN  int                                 unit,
    DNX_SAND_IN  DNX_SAND_U32_RANGE                  *queue_range
  )
{
  uint32 reg32 = 0;
  SHR_FUNC_INIT_VARS(unit);

  if (queue_range->start > queue_range->end) {
      SHR_ERR_EXIT(_SHR_E_PARAM, "Queue start %u is higher than Queue end %u\n", queue_range->start, queue_range->end);
  }

  SHR_IF_ERR_EXIT(soc_reg32_get(unit, IPS_FMC_QNUM_RANGEr, SOC_CORE_ALL, 0, &reg32));
  soc_reg_field_set(unit, IPS_FMC_QNUM_RANGEr, &reg32, FMC_QNUM_LOWf, queue_range->start);
  soc_reg_field_set(unit, IPS_FMC_QNUM_RANGEr, &reg32, FMC_QNUM_HIGHf, queue_range->end);
  SHR_IF_ERR_EXIT(soc_reg32_set(unit, IPS_FMC_QNUM_RANGEr, SOC_CORE_ALL, 0, reg32));

exit:
  SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
int
  jer2_mult_fabric_enhanced_get(
    DNX_SAND_IN  int                                 unit,
    DNX_SAND_INOUT DNX_SAND_U32_RANGE                *queue_range
  )
{
  uint32 reg32 = 0;
  SHR_FUNC_INIT_VARS(unit);

  SHR_IF_ERR_EXIT(soc_reg32_get(unit, IPS_FMC_QNUM_RANGEr, SOC_CORE_ALL, 0, &reg32));

  queue_range->start = soc_reg_field_get(unit, IPS_FMC_QNUM_RANGEr, reg32, FMC_QNUM_LOWf);
  queue_range->end = soc_reg_field_get(unit, IPS_FMC_QNUM_RANGEr, reg32, FMC_QNUM_HIGHf);

exit:
  SHR_FUNC_EXIT;
}

/* } */

#undef BSL_LOG_MODULE


