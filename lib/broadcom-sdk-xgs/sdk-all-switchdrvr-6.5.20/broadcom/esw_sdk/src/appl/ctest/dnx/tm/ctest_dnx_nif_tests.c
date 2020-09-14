/** \file ctest_dnx_nif_tests.c
 *
 * Tests for NIF
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <soc/portmod/portmod.h>
#include <soc/portmod/pm8x50_internal.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/dbx/dbx_file.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>

typedef enum diag_dnx_nif_vco_type_e
{
    DIAG_DNX_NIF_VCO_ILKN_28p1G = 0,
    DIAG_DNX_NIF_VCO_ILKN_27p3G,
    DIAG_DNX_NIF_VCO_ILKN_26p5G,
    DIAG_DNX_NIF_VCO_ETH_26p5G,
    DIAG_DNX_NIF_VCO_ILKN_25p7G,
    DIAG_DNX_NIF_VCO_ETH_25p7G,
    DIAG_DNX_NIF_VCO_ILKN_25G,
    DIAG_DNX_NIF_VCO_ILKN_20p6G,
    DIAG_DNX_NIF_VCO_ETH_20p6G,
    DIAG_DNX_NIF_VCO_NO_SPEED,
    DIAG_DNX_NIF_VCO_COUNT
} diag_dnx_nif_vco_type_t;

typedef struct diag_dnx_nif_vco_allocate_input_s
{
    diag_dnx_nif_vco_type_t request_vco_0;
    diag_dnx_nif_vco_type_t request_vco_1;
    diag_dnx_nif_vco_type_t current_tvco;
    diag_dnx_nif_vco_type_t current_ovco;
} diag_dnx_nif_vco_allocate_input_t;

typedef struct diag_dnx_nif_vco_allocate_output_s
{
    portmod_vco_type_t tvco_rate;
    portmod_vco_type_t ovco_rate;
    int result;                 /* Algorithm result - if cannot allocate request: DIAG_DNX_NIF_VCO_ALGO_FAIL, if
                                 * managed to allocate request: DIAG_DNX_NIF_VCO_ALGO_SUCCESS */
    int is_valid;               /* 0: Input is not valid and should not be tested. 1: input valid */
} diag_dnx_nif_vco_allocate_output_t;

#define DIAG_DNX_NIF_VCO_ALGO_FAIL     (0)
#define DIAG_DNX_NIF_VCO_ALGO_SUCCESS  (1)

#define DIAG_DNX_NIF_VCO_IS_ETH(vco) \
        ( (vco == DIAG_DNX_NIF_VCO_ETH_26p5G) || (vco == DIAG_DNX_NIF_VCO_ETH_25p7G) || (vco == DIAG_DNX_NIF_VCO_ETH_20p6G) )

#define DIAG_DNX_NIF_VCO_IS_ILKN(vco) \
        ( (vco == DIAG_DNX_NIF_VCO_ILKN_28p1G) || (vco == DIAG_DNX_NIF_VCO_ILKN_27p3G) || (vco == DIAG_DNX_NIF_VCO_ILKN_26p5G) || (vco == DIAG_DNX_NIF_VCO_ILKN_25p7G) || (vco == DIAG_DNX_NIF_VCO_ILKN_25G) || (vco == DIAG_DNX_NIF_VCO_ILKN_20p6G))

#define DIAG_DNX_NIF_VCO_IS_ILKN_ONLY(vco) \
        ( (vco == DIAG_DNX_NIF_VCO_ILKN_28p1G) || (vco == DIAG_DNX_NIF_VCO_ILKN_27p3G) || (vco == DIAG_DNX_NIF_VCO_ILKN_25G) )

#define DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(rate) \
        ( (rate == portmodVCO25P781G) || (rate == portmodVCO26P562G) )

#define DIAG_DNX_NIF_ILKN_MAX_SPEED      600000

static shr_error_e
diag_dnx_nif_vco_rate_get(
    int unit,
    diag_dnx_nif_vco_type_t vco,
    portmod_vco_type_t * vco_rate)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (vco)
    {
        case DIAG_DNX_NIF_VCO_ILKN_28p1G:
            *vco_rate = portmodVCO28P125G;
            break;
        case DIAG_DNX_NIF_VCO_ILKN_27p3G:
            *vco_rate = portmodVCO27P343G;
            break;
        case DIAG_DNX_NIF_VCO_ILKN_26p5G:
            *vco_rate = portmodVCO26P562G;
            break;
        case DIAG_DNX_NIF_VCO_ETH_26p5G:
            *vco_rate = portmodVCO26P562G;
            break;
        case DIAG_DNX_NIF_VCO_ILKN_25p7G:
            *vco_rate = portmodVCO25P781G;
            break;
        case DIAG_DNX_NIF_VCO_ETH_25p7G:
            *vco_rate = portmodVCO25P781G;
            break;
        case DIAG_DNX_NIF_VCO_ILKN_25G:
            *vco_rate = portmodVCO25G;
            break;
        case DIAG_DNX_NIF_VCO_ILKN_20p6G:
            *vco_rate = portmodVCO20P625G;
            break;
        case DIAG_DNX_NIF_VCO_ETH_20p6G:
            *vco_rate = portmodVCO20P625G;
            break;
        case DIAG_DNX_NIF_VCO_NO_SPEED:
            *vco_rate = portmodVCOInvalid;
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal VCO type %d\n", vco);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_nif_vco_rate_numeric_get(
    int unit,
    portmod_vco_type_t vco_rate,
    int *numeric_rate)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (vco_rate)
    {
        case portmodVCO28P125G:
            *numeric_rate = 28125;
            break;
        case portmodVCO27P343G:
            *numeric_rate = 27343;
            break;
        case portmodVCO26P562G:
            *numeric_rate = 26562;
            break;
        case portmodVCO25P781G:
            *numeric_rate = 25781;
            break;
        case portmodVCO25G:
            *numeric_rate = 25000;
            break;
        case portmodVCO20P625G:
            *numeric_rate = 20625;
            break;
        case portmodVCOInvalid:
            *numeric_rate = 0;
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal VCO type %d\n", vco_rate);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_nif_vco_bitmap_request_add(
    int unit,
    diag_dnx_nif_vco_type_t vco,
    pm8x50_vcos_bmp_t * required_vcos_bmp)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (vco)
    {
        case DIAG_DNX_NIF_VCO_ILKN_28p1G:
            PM8x50_VCO_ILKN_28P125G_SET(*required_vcos_bmp);
            break;
        case DIAG_DNX_NIF_VCO_ILKN_27p3G:
            PM8x50_VCO_ILKN_27P343G_SET(*required_vcos_bmp);
            break;
        case DIAG_DNX_NIF_VCO_ILKN_26p5G:
            PM8x50_VCO_ILKN_26P562G_SET(*required_vcos_bmp);
            break;
        case DIAG_DNX_NIF_VCO_ETH_26p5G:
            PM8x50_VCO_ETH_26P562G_SET(*required_vcos_bmp);
            break;
        case DIAG_DNX_NIF_VCO_ILKN_25p7G:
            PM8x50_VCO_ILKN_25P781G_SET(*required_vcos_bmp);
            break;
        case DIAG_DNX_NIF_VCO_ETH_25p7G:
            PM8x50_VCO_ETH_25P781G_SET(*required_vcos_bmp);
            break;
        case DIAG_DNX_NIF_VCO_ILKN_25G:
            PM8x50_VCO_ILKN_25G_SET(*required_vcos_bmp);
            break;
        case DIAG_DNX_NIF_VCO_ILKN_20p6G:
            PM8x50_VCO_ILKN_20P625G_SET(*required_vcos_bmp);
            break;
        case DIAG_DNX_NIF_VCO_ETH_20p6G:
            PM8x50_VCO_ETH_20P625G_SET(*required_vcos_bmp);
            break;
        case DIAG_DNX_NIF_VCO_NO_SPEED:
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_PARAM, "Illegal VCO type %d\n", vco);
    }

exit:
    SHR_FUNC_EXIT;
}

static char *
diag_dnx_nif_vco_type_str_get(
    diag_dnx_nif_vco_type_t vco)
{
    switch (vco)
    {
        case DIAG_DNX_NIF_VCO_ILKN_28p1G:
            return "ILKN_28p1G";
            break;
        case DIAG_DNX_NIF_VCO_ILKN_27p3G:
            return "ILKN_27p3G";
            break;
        case DIAG_DNX_NIF_VCO_ILKN_26p5G:
            return "ILKN_26p5G";
            break;
        case DIAG_DNX_NIF_VCO_ETH_26p5G:
            return "ETH_26p5G";
            break;
        case DIAG_DNX_NIF_VCO_ILKN_25p7G:
            return "ILKN_25p7G";
            break;
        case DIAG_DNX_NIF_VCO_ETH_25p7G:
            return "ETH_25p7G";
            break;
        case DIAG_DNX_NIF_VCO_ILKN_25G:
            return "ILKN_25G";
            break;
        case DIAG_DNX_NIF_VCO_ILKN_20p6G:
            return "ILKN_20p6G";
            break;
        case DIAG_DNX_NIF_VCO_ETH_20p6G:
            return "ETH_20p6G";
            break;
        case DIAG_DNX_NIF_VCO_NO_SPEED:
            return "NO_SPEED";
            break;
        default:
            return "VCO_ILLEGAL";
    }
}

static char *
diag_dnx_nif_vco_rate_str_get(
    portmod_vco_type_t vco_rate)
{
    switch (vco_rate)
    {
        case portmodVCO28P125G:
            return "PLL_28p1G";
            break;
        case portmodVCO27P343G:
            return "PLL_27p3G";
            break;
        case portmodVCO26P562G:
            return "PLL_26p5G";
            break;
        case portmodVCO25P781G:
            return "PLL_25p7G";
            break;
        case portmodVCO25G:
            return "PLL_25G";
            break;
        case portmodVCO20P625G:
            return "PLL_20p6G";
            break;
        case portmodVCOInvalid:
            return "NO_PLL";
            break;
        default:
            return "PLL_ILLEGAL";
    }
}

/*
 * \brief
 *   CTest reference allocation algorithm
 */
static shr_error_e
diag_dnx_nif_vco_request_allocate(
    int unit,
    diag_dnx_nif_vco_allocate_input_t * input,
    diag_dnx_nif_vco_allocate_output_t * output)
{
    portmod_vco_type_t rate, request_rate, request0_rate, request1_rate, tvco_rate, ovco_rate;
    int request0_rate_numeric, request1_rate_numeric;
    diag_dnx_nif_vco_type_t vco_idx;
    uint32 used_vco_rates_array[portmodVCOCount] = { 0 };
    int nof_request_vco_rates = 0;
    int is_eth_supported_rate;
    int is_request0_20g = 0, is_request1_20g = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Assume valid input
     */
    output->is_valid = 1;

    /*
     * ====================
     * Check input validity
     * ====================
     */
    if (input->request_vco_0 == input->request_vco_1)
    {
        /*
         * Requests must be different.
         */
        output->is_valid = 0;
        SOC_EXIT;
    }

    if ((input->current_tvco == input->current_ovco) && (input->current_tvco != DIAG_DNX_NIF_VCO_NO_SPEED))
    {
        /*
         * TVCO and OVCO cannot be identical, unless both NO_SPEED
         */
        output->is_valid = 0;
        SOC_EXIT;
    }

    if (input->current_tvco == DIAG_DNX_NIF_VCO_ETH_20p6G)
    {
        /*
         * 20G Ethernet not allowed on TVCO (20G TVCO cannot drive Ethernet)
         */
        output->is_valid = 0;
        SOC_EXIT;
    }

    if (DIAG_DNX_NIF_VCO_IS_ETH(input->current_ovco) &&
        ((DIAG_DNX_NIF_VCO_IS_ILKN_ONLY(input->current_tvco)) || (input->current_tvco == DIAG_DNX_NIF_VCO_NO_SPEED) ||
         (input->current_tvco == DIAG_DNX_NIF_VCO_ETH_20p6G) || (input->current_tvco == DIAG_DNX_NIF_VCO_ILKN_20p6G)))
    {
        /*
         * If TVCO is either ILKN only or NO_SPEED or 20G, it cannot drive Ethernet,  Therefore OVCO cannot be Ethernet.
         */
        output->is_valid = 0;
        SOC_EXIT;
    }

    SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->request_vco_0, &request0_rate));
    SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->request_vco_1, &request1_rate));
    if (request0_rate == request1_rate)
    {
        /*
         * Requeste0 and request1 cannot be with the same rate, otherwise it is narrowed to the Ethernet case.
         */
        output->is_valid = 0;
        SOC_EXIT;
    }

    /*
     * ===================
     * Check for solutions
     * ===================
     */

    /*
     * Check number of requested rates
     */
    for (vco_idx = DIAG_DNX_NIF_VCO_ILKN_28p1G; vco_idx < DIAG_DNX_NIF_VCO_COUNT; vco_idx++)
    {
        if ((input->request_vco_0 == vco_idx) || (input->request_vco_1 == vco_idx) ||
            (input->current_tvco == vco_idx) || (input->current_ovco == vco_idx))
        {
            SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, vco_idx, &rate));
            used_vco_rates_array[rate] = 1;
        }
    }

    nof_request_vco_rates = 0;
    for (rate = (portmodVCOInvalid + 1); rate < portmodVCOCount; rate++)
    {
        if (used_vco_rates_array[rate] == 1)
        {
            nof_request_vco_rates++;
        }
    }

    if (nof_request_vco_rates > 2)
    {
        /*
         * No solution: number of requested rates is bigger than 2
         */
        output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
        SOC_EXIT;
    }

    /*
     * Solution Type 1: only one speed is requested and speed exists
     */
    if (input->request_vco_1 == DIAG_DNX_NIF_VCO_NO_SPEED)
    {
        if ((input->request_vco_0 == input->current_tvco) || (input->request_vco_0 == input->current_ovco))
        {
            SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->current_tvco, &output->tvco_rate));
            SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->current_ovco, &output->ovco_rate));
            output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
            SOC_EXIT;
        }
    }

    /*
     * Solution Type 2: two speeds are requested and both exists
     */
    if (input->request_vco_1 != DIAG_DNX_NIF_VCO_NO_SPEED)
    {
        if (((input->request_vco_0 == input->current_tvco) || (input->request_vco_0 == input->current_ovco)) &&
            ((input->request_vco_1 == input->current_tvco) || (input->request_vco_1 == input->current_ovco)))
        {
            SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->current_tvco, &output->tvco_rate));
            SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->current_ovco, &output->ovco_rate));
            output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
            SOC_EXIT;
        }
    }

    /*
     * Solution Type 3: Single speed was requested
     */
    if (input->request_vco_1 == DIAG_DNX_NIF_VCO_NO_SPEED)
    {
        SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->request_vco_0, &request_rate));
        SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->current_tvco, &tvco_rate));
        SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->current_ovco, &ovco_rate));

        if ((request_rate == tvco_rate) &&
            (!DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_0) ||
             (DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_0) && DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(tvco_rate))))
        {
            /*
             * TVCO match
             */
            output->tvco_rate = tvco_rate;
            output->ovco_rate = ovco_rate;
            output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
            SOC_EXIT;
        }

        if ((input->request_vco_0 == DIAG_DNX_NIF_VCO_ETH_20p6G) &&
            ((tvco_rate == portmodVCOInvalid) && (ovco_rate == portmodVCO20P625G)))
        {
            /*
             * Dummy 25.781G speed on TVCO
             */
            output->tvco_rate = portmodVCO25P781G;
            output->ovco_rate = portmodVCO20P625G;
            output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
            SOC_EXIT;
        }

        if ((request_rate == ovco_rate) &&
            (!DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_0) ||
             (DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_0) && DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(tvco_rate))))
        {
            /*
             * OVCO match
             */
            output->tvco_rate = tvco_rate;
            output->ovco_rate = ovco_rate;
            output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
            SOC_EXIT;
        }

        if ((tvco_rate == portmodVCOInvalid) && (ovco_rate == portmodVCOInvalid))
        {
            /*
             * Both PLLs are free
             */
            if (input->request_vco_0 == DIAG_DNX_NIF_VCO_ETH_20p6G)
            {
                /*
                 * Dummy 25.781G speed on TVCO
                 */
                output->tvco_rate = portmodVCO25P781G;
                output->ovco_rate = portmodVCO20P625G;
                output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                SOC_EXIT;
            }

            if (DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request_rate))
            {
                /*
                 * If rate can support Ethernet --> set on TVCO
                 */
                output->tvco_rate = request_rate;
                output->ovco_rate = ovco_rate;
                output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                SOC_EXIT;
            }
            else
            {
                /*
                 * If rate cannot support Ethernet --> set on OVCO
                 */
                output->tvco_rate = tvco_rate;
                output->ovco_rate = request_rate;
                output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                SOC_EXIT;
            }
        }

        if (tvco_rate == portmodVCOInvalid)
        {
            /*
             * TVCO free. OVCO was already validated not to be Ethernet.
             */
            if (input->request_vco_0 == DIAG_DNX_NIF_VCO_ETH_20p6G)
            {
                /*
                 * TVCO cannot be set to 20G Ethernet.
                 */
                output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                SOC_EXIT;
            }
            else
            {
                output->tvco_rate = request_rate;
                output->ovco_rate = ovco_rate;
                output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                SOC_EXIT;
            }
        }

        if (ovco_rate == portmodVCOInvalid)
        {
            /*
             * OVCO free
             */
            if (DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_0) && !DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(tvco_rate))
            {
                /*
                 * OVCO free but TVCO cannot support Ethernet
                 */
                output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                SOC_EXIT;
            }
            else
            {
                output->tvco_rate = tvco_rate;
                output->ovco_rate = request_rate;
                output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                SOC_EXIT;
            }
        }

        output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
        SOC_EXIT;
    }

    /*
     * Solution Type 4: Two speeds were requested
     */
    if (input->request_vco_1 != DIAG_DNX_NIF_VCO_NO_SPEED)
    {
        SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->request_vco_0, &request0_rate));
        SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->request_vco_1, &request1_rate));
        SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->current_tvco, &tvco_rate));
        SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, input->current_ovco, &ovco_rate));

        if ((tvco_rate != portmodVCOInvalid) && (ovco_rate != portmodVCOInvalid))
        {
            /*
             * Both current PLLs are occupied
             */
            if ((request0_rate == tvco_rate) && (request1_rate == ovco_rate))
            {
                if (!DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(tvco_rate) &&
                    (DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_0) || DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_1)))
                {
                    /*
                     * FAIL, request for Ethernet while TVCO cannot support Ethernet
                     */
                    output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                    SOC_EXIT;
                }
                else
                {
                    output->tvco_rate = tvco_rate;
                    output->ovco_rate = ovco_rate;
                    output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                    SOC_EXIT;
                }
            }
            else
            {
                if ((tvco_rate == ovco_rate) && (request0_rate != request1_rate))
                {
                    /*
                     * Two PLLs are occupied but with the same rate. On the other hand, two different VCO rates were requested --> can't serve.
                     */
                    output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                    SOC_EXIT;
                }
            }
        }

        if ((request0_rate == ovco_rate) && (request1_rate == tvco_rate))
        {
            /*
             * Both current PLLs are occupied, test the opposite match
             */
            if (!DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(tvco_rate) &&
                (DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_0) || DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_1)))
            {
                /*
                 * FAIL, request for Ethernet while TVCO cannot support Ethernet
                 */
                output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                SOC_EXIT;
            }
            else
            {
                output->tvco_rate = tvco_rate;
                output->ovco_rate = ovco_rate;
                output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                SOC_EXIT;
            }
        }

        if ((tvco_rate == portmodVCOInvalid) && (ovco_rate == portmodVCOInvalid))
        {
            /*
             * Both PLLs are free
             */
            is_request0_20g = (input->request_vco_0 == DIAG_DNX_NIF_VCO_ETH_20p6G) ? 1 : 0;
            is_request1_20g = (input->request_vco_1 == DIAG_DNX_NIF_VCO_ETH_20p6G) ? 1 : 0;

            if ((is_request0_20g && !DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request1_rate)) ||
                (is_request1_20g && !DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request0_rate)))
            {
                /*
                 * If 20G Ethernet is requested, the other request should be able to support Ethernet.
                 */
                output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                SOC_EXIT;
            }

            SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_numeric_get(unit, request0_rate, &request0_rate_numeric));
            SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_numeric_get(unit, request1_rate, &request1_rate_numeric));

            if (DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request0_rate) && DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request1_rate))
            {
                /*
                 * If both requests support Ethernet --> allocate according to rate
                 */
                if (request0_rate_numeric > request1_rate_numeric)
                {
                    output->tvco_rate = request0_rate;
                    output->ovco_rate = request1_rate;
                    output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                    SOC_EXIT;
                }
                else
                {
                    output->tvco_rate = request1_rate;
                    output->ovco_rate = request0_rate;
                    output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                    SOC_EXIT;
                }
            }
            else if (!DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request0_rate) &&
                     !DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request1_rate))
            {
                /*
                 * If both requests do not support Ethernet --> allocate according to rate
                 */
                if (request0_rate_numeric < request1_rate_numeric)
                {
                    output->tvco_rate = request0_rate;
                    output->ovco_rate = request1_rate;
                    output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                    SOC_EXIT;
                }
                else
                {
                    output->tvco_rate = request1_rate;
                    output->ovco_rate = request0_rate;
                    output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                    SOC_EXIT;
                }
            }
            else if (DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request0_rate))
            {
                output->tvco_rate = request0_rate;
                output->ovco_rate = request1_rate;
                output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                SOC_EXIT;
            }
            else if (DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request1_rate))
            {
                output->tvco_rate = request1_rate;
                output->ovco_rate = request0_rate;
                output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                SOC_EXIT;
            }
        }

        if (tvco_rate == portmodVCOInvalid)
        {
            /*
             * TVCO is free
             */
            if ((ovco_rate == request0_rate) || (ovco_rate == request1_rate))
            {
                if (ovco_rate == request1_rate)
                {
                    is_eth_supported_rate = DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request0_rate) ? 1 : 0;
                }
                else
                {
                    is_eth_supported_rate = DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(request1_rate) ? 1 : 0;
                }

                if (!is_eth_supported_rate &&
                    (DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_0) || DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_1)))
                {
                    /*
                     * Match on OVCO, with request for Ethernet port. But other PLL, which is going to TVCO doesn't support Ethernet
                     */
                    output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                    SOC_EXIT;
                }
                else
                {
                    if (ovco_rate == request1_rate)
                    {
                        output->tvco_rate = request0_rate;
                        output->ovco_rate = ovco_rate;
                        output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                        SOC_EXIT;
                    }
                    else
                    {
                        output->tvco_rate = request1_rate;
                        output->ovco_rate = ovco_rate;
                        output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                        SOC_EXIT;
                    }
                }
            }
            else
            {
                output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                SOC_EXIT;
            }
        }

        if (ovco_rate == portmodVCOInvalid)
        {
            /*
             * OVCO is free
             */
            if ((tvco_rate == request0_rate) || (tvco_rate == request1_rate))
            {

                is_eth_supported_rate = DIAG_DNX_NIF_VCO_RATE_SUPPORT_ETH(tvco_rate);

                if (!is_eth_supported_rate &&
                    (DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_0) || DIAG_DNX_NIF_VCO_IS_ETH(input->request_vco_1)))
                {
                    /*
                     * Match on TVCO, with request for Ethernet port. But other TVCO do not support Ethernet
                     */
                    output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                    SOC_EXIT;
                }
                else
                {
                    if (tvco_rate == request1_rate)
                    {
                        output->tvco_rate = tvco_rate;
                        output->ovco_rate = request0_rate;
                        output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                        SOC_EXIT;
                    }
                    else
                    {
                        output->tvco_rate = tvco_rate;
                        output->ovco_rate = request1_rate;
                        output->result = DIAG_DNX_NIF_VCO_ALGO_SUCCESS;
                        SOC_EXIT;
                    }
                }
            }
            else
            {
                output->result = DIAG_DNX_NIF_VCO_ALGO_FAIL;
                SOC_EXIT;
            }
        }
    }

    /*
     * Shouldn't reach here
     */
    SHR_CLI_EXIT(_SHR_E_INTERNAL, "Not handled ==> Request (%d %d), TVCO %d, OVCO %d\n",
                 input->request_vco_0, input->request_vco_1, input->current_tvco, input->current_ovco);

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   portmod VCO allocation CTest.
 *   The test compares portmod VCO allocation algorithm to a reference algorithm.
 *   The test returns PASS if result is identical for all input combinations, otherwise FAIL.
 */
static shr_error_e
diag_dnx_nif_vco_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    diag_dnx_nif_vco_type_t request_vco_0, request_vco_1, current_tvco, current_ovco;
    diag_dnx_nif_vco_allocate_input_t input;
    diag_dnx_nif_vco_allocate_output_t output;
    shr_error_e rv, result;
    pm8x50_vcos_bmp_t required_vcos_bmp = 0;
    portmod_vco_type_t tvco_rate, ovco_rate;
    int nof_test_cases = 0;
    void *file = NULL;
    char *filename = NULL;
    char teststr[SH_SAND_MAX_TOKEN_SIZE];
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Open Output file if required
     */
    SH_SAND_GET_STR("FILE", filename);
    if (!ISEMPTY(filename))
    {
        if ((file = dbx_file_open(filename)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Cannot open:%s\n", filename);
        }

        /*
         * Create File Header
         */
        sal_snprintf(teststr, SH_SAND_MAX_TOKEN_SIZE,
                     "REQUEST_VCO0,REQUEST_VCO1,CURRENT_TVCO,CURRENT_OVCO,RESULT,RESULT_TVCO,RESULT_OVCO\n");
        dbx_file_write(file, teststr);
    }

    /*
     * Disable portmod prints
     */
    sh_process_command(unit, "debug soc port off");

    /*
     * Main test loop
     */
    for (request_vco_0 = DIAG_DNX_NIF_VCO_ILKN_28p1G; request_vco_0 < DIAG_DNX_NIF_VCO_NO_SPEED; request_vco_0++)
    {
        for (request_vco_1 = DIAG_DNX_NIF_VCO_ILKN_28p1G; request_vco_1 <= DIAG_DNX_NIF_VCO_NO_SPEED; request_vco_1++)
        {
            for (current_tvco = DIAG_DNX_NIF_VCO_ILKN_28p1G; current_tvco <= DIAG_DNX_NIF_VCO_NO_SPEED; current_tvco++)
            {
                for (current_ovco = DIAG_DNX_NIF_VCO_ILKN_28p1G; current_ovco <= DIAG_DNX_NIF_VCO_NO_SPEED;
                     current_ovco++)
                {

                    /*
                     * Run CTest reference allocation function
                     */
                    input.request_vco_0 = request_vco_0;
                    input.request_vco_1 = request_vco_1;
                    input.current_tvco = current_tvco;
                    input.current_ovco = current_ovco;
                    SHR_IF_ERR_EXIT(diag_dnx_nif_vco_request_allocate(unit, &input, &output));

                    if (!output.is_valid)
                    {
                        /*
                         * Skip illegal cases
                         */
                        continue;
                    }

                    /*
                     * Run portmod allocation function
                     */
                    required_vcos_bmp = 0;
                    SHR_IF_ERR_EXIT(diag_dnx_nif_vco_bitmap_request_add(unit, request_vco_0, &required_vcos_bmp));
                    SHR_IF_ERR_EXIT(diag_dnx_nif_vco_bitmap_request_add(unit, request_vco_1, &required_vcos_bmp));
                    SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, current_tvco, &tvco_rate));
                    SHR_IF_ERR_EXIT(diag_dnx_nif_vco_rate_get(unit, current_ovco, &ovco_rate));
                    rv = pm8x50_vcos_request_allocate(unit, flags, required_vcos_bmp, &tvco_rate, &ovco_rate);
                    result = (rv == _SHR_E_NONE) ? DIAG_DNX_NIF_VCO_ALGO_SUCCESS : DIAG_DNX_NIF_VCO_ALGO_FAIL;

                    /*
                     * Validate portmod algorithm result matches reference algorithm result
                     */
                    if (output.result != result)
                    {
                        SHR_CLI_EXIT(_SHR_E_INTERNAL, "PASS/FAIL indications do not match portmod %d, reference %d.\n "
                                     "Request (%d %d), TVCO %d, OVCO %d\n",
                                     result, output.result, request_vco_0, request_vco_1, current_tvco, current_ovco);
                    }

                    if (result == DIAG_DNX_NIF_VCO_ALGO_SUCCESS)
                    {
                        /*
                         * If algorithm return success compare TVCO, OVCO values
                         */
                        if (output.tvco_rate != tvco_rate)
                        {
                            SHR_CLI_EXIT(_SHR_E_INTERNAL, "TVCO rates do not match (portmod %d, reference %d).\n "
                                         "Request (%d %d), TVCO %d, OVCO %d\n",
                                         tvco_rate, output.tvco_rate, request_vco_0, request_vco_1, current_tvco,
                                         current_ovco);
                        }

                        if (output.ovco_rate != ovco_rate)
                        {
                            SHR_CLI_EXIT(_SHR_E_INTERNAL, "OVCO do not match (portmod %d, reference %d).\n"
                                         "Request (%d %d), TVCO %d, OVCO %d\n",
                                         ovco_rate, output.ovco_rate, request_vco_0, request_vco_1, current_tvco,
                                         current_ovco);
                        }
                    }

                    /*
                     * Write test case into file, if required
                     */
                    if (file != NULL)
                    {

                        sal_snprintf(teststr, SH_SAND_MAX_TOKEN_SIZE, "%s,%s,%s,%s,%s,%s,%s\n",
                                     diag_dnx_nif_vco_type_str_get(request_vco_0),
                                     diag_dnx_nif_vco_type_str_get(request_vco_1),
                                     diag_dnx_nif_vco_type_str_get(current_tvco),
                                     diag_dnx_nif_vco_type_str_get(current_ovco),
                                     result ? "PASS" : "FAIL",
                                     diag_dnx_nif_vco_rate_str_get(tvco_rate),
                                     diag_dnx_nif_vco_rate_str_get(ovco_rate));

                        dbx_file_write(file, teststr);
                    }

                    nof_test_cases++;
                }
            }
        }
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS. Number of cases tested = %d\n", nof_test_cases);

exit:

    /*
     * Close output file
     */
    if (file != NULL)
    {
        dbx_file_close(file);
    }

    /*
     * Enable portmod prints
     */
    sh_process_command(unit, "debug soc port info");
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   CDU reset dbal read.
 */
static shr_error_e
diag_dnx_nif_cdu_reset_get(
    int unit,
    int core,
    int cdu_id,
    int *in_reset)
{
    uint32 entry_handle_id;
    uint32 in_reset_dbal, in_lanes_reset_dbal;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_CDU_PM_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_id);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PM_RESET, INST_SINGLE, &in_reset_dbal);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PM_LANES_RESET, INST_SINGLE, &in_lanes_reset_dbal);

    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *in_reset = ((in_reset_dbal == DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET)
                 || (in_lanes_reset_dbal == DBAL_ENUM_FVAL_NIF_CDU_PM_RESET_REVERSE_LOGIC_IN_RESET)) ? 1 : 0;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * \brief
 *   CDU power optimization test.
 *   PMs that are disabled in device should be in reset and powered down.
 *   PMs that are enabled should be out of reset and powered up no matter how many ports
 *   are configured on them.
 */
static shr_error_e
diag_dnx_nif_cdu_reset_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int nof_test_cases = 0;
    void *file = NULL;
    char *filename = NULL;
    char teststr[SH_SAND_MAX_TOKEN_SIZE];
    uint32 fail_cnt = 0;
    uint32 ethu_idx, ethu_nof, type;
    uint32 pm_index;
    bcm_pbmp_t phys, supported_phys;
    int first_phy_port, nof_cdus_per_core, nof_phys_per_cdu, nof_phys_per_core;
    int core, cdu_id, ethu_id_in_core;
    int is_phy_supported, in_reset = 1, is_fail;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Open Output file if required
     */
    SH_SAND_GET_STR("FILE", filename);
    if (!ISEMPTY(filename))
    {
        if ((file = dbx_file_open(filename)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Cannot open:%s\n", filename);
        }

        /*
         * Create File Header
         */
        sal_snprintf(teststr, SH_SAND_MAX_TOKEN_SIZE, "%s,%s,%s,%s,%s,%s,%s,%s\n",
                     "ethu_idx", "type", "core", "cdu_id", "phy_id", "is_phy_supported", "in_reset", "result");
        dbx_file_write(file, teststr);

    }

    nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
    nof_phys_per_cdu = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
    nof_phys_per_core = nof_cdus_per_core * nof_phys_per_cdu;

    ethu_nof = dnx_data_nif.eth.ethu_properties_info_get(unit)->key_size[0];

    /*
     * iterate over all ethu_index in ethu_properties 
     */
    for (ethu_idx = 0; ethu_idx < ethu_nof; ethu_idx++)
    {
        nof_test_cases += 1;

        /*
         * for each pm in ethu check pm has supported phy 
         */
        type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_idx)->type;

        if (type != imbDispatchTypeImb_cdu)
        {
            continue;
        }

        pm_index = dnx_data_nif.eth.ethu_properties_get(unit, ethu_idx)->pms[0];
        phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;
        supported_phys = dnx_data_nif.phys.general_get(unit)->supported_phys;

        /*
         * check if pm phy are supported 
         */
        _SHR_PBMP_AND(supported_phys, phys);
        _SHR_PBMP_COUNT(supported_phys, is_phy_supported);
        is_phy_supported = is_phy_supported > 0 ? 1 : 0;

        _SHR_PBMP_FIRST(phys, first_phy_port);

        core = first_phy_port / nof_phys_per_core;
        cdu_id = (first_phy_port / nof_phys_per_cdu);
        ethu_id_in_core = cdu_id % nof_cdus_per_core;

        SHR_IF_ERR_EXIT(diag_dnx_nif_cdu_reset_get(unit, core, ethu_id_in_core, &in_reset));

        /*
         * when supported all CDUs should NOT be in_reset 
         */
        is_fail = (is_phy_supported) ^ (!in_reset);

        if (is_fail)
        {
            fail_cnt++;
        }

        if (file != NULL)
        {
            sal_snprintf(teststr, SH_SAND_MAX_TOKEN_SIZE, "%u,%s,%d,%d,%d,%s,%s,%s\n",
                         ethu_idx,
                         "cdu",
                         core,
                         cdu_id,
                         first_phy_port,
                         (is_phy_supported ? "supported" : "not_supported"),
                         (in_reset ? "reset" : "out_of_reset"), (is_fail ? "fail" : "pass"));

            dbx_file_write(file, teststr);
        }
    }

    if (fail_cnt != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Test FAILED. Number of cases tested = %d\n", nof_test_cases);
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS. Number of cases tested = %d\n", nof_test_cases);

exit:

    /*
     * Close output file
     */
    if (file != NULL)
    {
        dbx_file_close(file);
    }

    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Test ILKN number of segment calculation  
 *   Checking several cases of segment calculation.
 */
static shr_error_e
diag_dnx_nif_ilkn_nof_segment_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int nof_test_cases = 0;
    void *file = NULL;
    char *filename = NULL;
    char teststr[SH_SAND_MAX_TOKEN_SIZE];
    int core_clk, ilkn_burst_short;
    uint32 fail_cnt = 0;
    int num_lane, lanes_allowed_nof;
    int idx, speed, nof_segments;
    int is_fail;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Open Output file if required
     */
    SH_SAND_GET_STR("FILE", filename);
    if (!ISEMPTY(filename))
    {
        if ((file = dbx_file_open(filename)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_UNAVAIL, "Cannot open:%s\n", filename);
        }

        /*
         * Create File Header
         */
        sal_snprintf(teststr, SH_SAND_MAX_TOKEN_SIZE, "%s,%s,%s,%s,%s\n",
                     "core_clk", "num_lane", "ilkn_burst_short", "speed", "result");
        dbx_file_write(file, teststr);

    }

    core_clk = dnx_data_device.general.core_clock_khz_get(unit);
    lanes_allowed_nof = dnx_data_nif.ilkn.lanes_allowed_nof_get(unit);

    /*
     * iterate over all supported lane num and valid speed
     */

    for (num_lane = 1; num_lane <= lanes_allowed_nof; num_lane++)
    {
        for (ilkn_burst_short = 32; ilkn_burst_short <= 128; ilkn_burst_short += 32)
        {
            for (idx = 0; idx < dnx_data_nif.ilkn.supported_interfaces_info_get(unit)->key_size[0]; idx++)
            {
                if (dnx_data_nif.ilkn.supported_interfaces_get(unit, idx)->is_valid)
                {
                    speed = dnx_data_nif.ilkn.supported_interfaces_get(unit, idx)->speed;
                    if (num_lane * speed > DIAG_DNX_NIF_ILKN_MAX_SPEED)
                    {
                        continue;
                    }
                    nof_test_cases += 1;

                    SHR_IF_ERR_EXIT(dnx_algo_port_imb_ilkn_nof_segments_calc
                                    (unit, core_clk, num_lane, ilkn_burst_short, speed, &nof_segments));
                    /*
                     * verification: nof segments not bigger than maximum.
                     */
                    is_fail = (nof_segments > dnx_data_nif.ilkn.segments_max_nof_get(unit));

                    if (is_fail)
                    {
                        fail_cnt++;
                    }

                    if (file != NULL)
                    {
                        sal_snprintf(teststr, SH_SAND_MAX_TOKEN_SIZE, "%d,%d,%d,%d,%s\n",
                                     core_clk, num_lane, ilkn_burst_short, speed, (is_fail ? "fail" : "pass"));

                        dbx_file_write(file, teststr);
                    }
                }
            }
        }
    }

    if (fail_cnt != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Test FAILED. Number of cases tested = %d\n", nof_test_cases);
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS. Number of cases tested = %d\n", nof_test_cases);

exit:
    /*
     * Close output file
     */
    if (file != NULL)
    {
        dbx_file_close(file);
    }

    SHR_FUNC_EXIT;
}

static sh_sand_option_t sh_dnx_tm_nif_vco_test_options[] = {
    {"FILE", SAL_FIELD_TYPE_STR, "File name for exporting test results (CSV format)", ""},
    {NULL}
};

static sh_sand_man_t diag_dnx_nif_vco_test_man = {
    "Test portmod VCO allocation algorithm",
    "Test portmod VCO allocation algorithm. Test cases results can be exported by indicating output file name",
    NULL,
    "ctest tm nif vco file=output.csv"
};

static sh_sand_option_t sh_dnx_tm_nif_cdu_reset_test_options[] = {
    {"FILE", SAL_FIELD_TYPE_STR, "File name for exporting test results (CSV format)", ""},
    {NULL}
};

static sh_sand_man_t diag_dnx_nif_cdu_reset_test_man = {
    "Test CDU reset state",
    "Test CDU reset state. Test cases results can be exported by indicating output file name",
    NULL,
    "ctest tm nif cdu_reset file=output.csv"
};

static sh_sand_option_t sh_dnx_tm_nif_ilkn_nof_segment_test_options[] = {
    {"FILE", SAL_FIELD_TYPE_STR, "File name for exporting test results (CSV format)", ""},
    {NULL}
};

static sh_sand_man_t diag_dnx_nif_ilkn_nof_segment_test_man = {
    "Test ILKN number of segment calculation",
    "Test ILKN number of segment calculation. Test cases results can be exported by indicating output file name",
    NULL,
    "ctest tm nif ilkn_nof_segment file=output.csv"
};

sh_sand_cmd_t sh_dnx_tm_nif_test_cmds[] = {
    {"vco", diag_dnx_nif_vco_test_cmd, NULL, sh_dnx_tm_nif_vco_test_options, &diag_dnx_nif_vco_test_man},
    {"cdu_reset", diag_dnx_nif_cdu_reset_test_cmd, NULL, sh_dnx_tm_nif_cdu_reset_test_options,
     &diag_dnx_nif_cdu_reset_test_man},
    {"ilkn_nof_segment", diag_dnx_nif_ilkn_nof_segment_test_cmd, NULL, sh_dnx_tm_nif_ilkn_nof_segment_test_options,
     &diag_dnx_nif_ilkn_nof_segment_test_man},
    {NULL}
};
