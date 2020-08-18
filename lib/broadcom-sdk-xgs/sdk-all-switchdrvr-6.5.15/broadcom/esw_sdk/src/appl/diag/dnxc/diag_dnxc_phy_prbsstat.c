/** \file diag_dnxc_phy_prbsstat.c
 *
 * PRBSStat is a command to periodically collect PRBS error counters and
  * compute BER based on the port configuration and the observed errorr
  * counters. The command processes and displays counters and BER calculation
  * for all lanes on a given port.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COMMON

/* shared */
#include <shared/bsl.h>
/* appl */
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <soc/portmod/portmod.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager_utils.h>
#endif

#define DNXC_PRBS_STAT_F_INIT        (1 << 0)
#define DNXC_PRBS_STAT_F_RUNNING     (1 << 1)

#define DNXC_PRBS_STAT_IS_INIT(f)       (f & DNXC_PRBS_STAT_F_INIT)
#define DNXC_PRBS_STAT_IS_RUNNING(f)    (f & DNXC_PRBS_STAT_F_RUNNING)

#define DNXC_PRBS_STAT_MAX_LANES        8

#define DNXC_PRBS_STAT_LOCK(lock)      \
    if (lock) { \
        sal_mutex_take(lock, sal_mutex_FOREVER);   \
    }

#define DNXC_PRBS_STAT_UNLOCK(lock) \
    if (lock) { \
        sal_mutex_give(lock); \
    }

typedef enum
{
    DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK = 0,
    DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST = -1,
    DIAG_DNXC_PHY_PRBS_STAT_PREV_LOCK_LOST = -2,
    DIAG_DNXC_PHY_PRBS_STAT_COUNTER_OVF = 0x7FFFFFFF
} diag_dnxc_phy_prbs_stat_status_e;


/*
 * The main control block
 *
 * The hardware polling interval is stored in secs. Due to the latched
 * clear-on-read hardware register behavior, the interval determines
 * how fast new data is available to be examined with show counters / show
 * ber. If show commands are called before an interval is expired,
 * there will be no new errors or ber computation available.
 *
 * Loss of lock events are recorded. For counters, there are lossoflock
 * counters that get displayed. For BER computation, once there is a
 * loss of lock, the BER computation will not be updated until the
 * next show ber is called. This ensures loss of lock events are
 * noticed in the show ber output.
 *
 */
typedef struct dnxc_prbs_stat_subcounter_s {
    uint64 errors;      /* PRBS errors */
    uint64 losslock;    /* Loss of lock */
} dnxc_prbs_stat_subcounter_t;

typedef struct dnxc_prbs_stat_counter_s {
    dnxc_prbs_stat_subcounter_t acc;
    dnxc_prbs_stat_subcounter_t cur;
} dnxc_prbs_stat_counter_t;

typedef struct dnxc_prbs_stat_ber_s {
    uint32 ber_int;
    uint32 ber_remainder;
    uint32 ber_pow;
} dnxc_prbs_stat_ber_t;

typedef struct dnxc_prbs_stat_pinfo_s {
    int speed;
    int lanes;
    bcm_port_phy_fec_t fec_type;
    int state[DNXC_PRBS_STAT_MAX_LANES];
    int intervals[DNXC_PRBS_STAT_MAX_LANES];
    dnxc_prbs_stat_counter_t counters[DNXC_PRBS_STAT_MAX_LANES];
    dnxc_prbs_stat_ber_t ber[DNXC_PRBS_STAT_MAX_LANES];
} dnxc_prbs_stat_pinfo_t;

typedef struct dnxc_prbs_stat_cb_s {
    uint32 flags;
    int secs;
    bcm_pbmp_t pbmp;
    dnxc_prbs_stat_pinfo_t pinfo[BCM_PBMP_PORT_MAX];
    sal_sem_t sem;
    sal_thread_t thread_id;
    sal_mutex_t lock;
} dnxc_prbs_stat_cb_t;

static dnxc_prbs_stat_cb_t dnxc_prbs_stat_cb[BCM_MAX_NUM_UNITS];
static uint32 dnxc_prbs_stat_max_pow = 18;

static sh_sand_option_t dnxc_phy_prbs_stat_start_options[] = {
    {"pbmp",        SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "0",   NULL, NULL, SH_SAND_ARG_FREE},
    {"interval",    SAL_FIELD_TYPE_INT32, "interval [seconds]",     "1",   NULL, "1-180", SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnxc_phy_prbs_stat_start_man = {
    .brief =    "Start or display PRBSStat.",
    .full =     "Start collecting PRBS errors on given ports.\n"
                "If no option is specified, dump current PRBSStat configuration.",
    .synopsis = "<pbmp> <interval>",
    .examples = "1 interval=30 \n"
                "1 \n"
                "\n",
};

static sh_sand_man_t dnxc_phy_prbs_stat_stop_man = {
    .brief =    "Stop PRBSStat.",
    .full =     "Stop collecting PRBS errors.\n",
    .synopsis = NULL,
    .examples = NULL,
};

static sh_sand_option_t dnxc_phy_prbs_stat_counters_options[] = {
    {"pbmp",        SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "0",   NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnxc_phy_prbs_stat_counters_man = {
    .brief =    "Display PRBS errors.",
    .full =     "Display collected PRBS errors within a full interval.",
    .synopsis = NULL,
    .examples = NULL,
};

static sh_sand_option_t dnxc_phy_prbs_stat_ber_options[] = {
    {"pbmp",        SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "0",   NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnxc_phy_prbs_stat_ber_man = {
    .brief =    "Display BER.",
    .full =     "Display computed BERs.",
    .synopsis = NULL,
    .examples = NULL,
};

static sh_sand_option_t dnxc_phy_prbs_stat_clear_options[] = {
    {"pbmp",        SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "0",   NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_man_t dnxc_phy_prbs_stat_clear_man = {
    .brief =    "Clear PRBS errors.",
    .full =     "Clear collected PRBS errors.",
    .synopsis = NULL,
    .examples = NULL,
};

static shr_error_e
diag_dnxc_phy_prbs_stat_validate_pbmp(int unit, bcm_pbmp_t *pbmp)
{
    bcm_pbmp_t check_pbmp;
    bcm_port_config_t *port_config = NULL;
    char pbmp_str[FORMAT_PBMP_MAX];

    SHR_FUNC_INIT_VARS(unit);

    port_config = sal_alloc(sizeof(bcm_port_config_t), "port_config");
    if (NULL == port_config) {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Not enough memory to allocate 'port_config'.\n");
    }

    sal_memset(pbmp_str, 0, (sizeof(char) * FORMAT_PBMP_MAX));
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));
    BCM_PBMP_CLEAR(check_pbmp);
    BCM_PBMP_ASSIGN(check_pbmp, *pbmp);
    BCM_PBMP_REMOVE(check_pbmp, port_config->port);
    if (BCM_PBMP_NOT_NULL(check_pbmp))
    {
        sal_memset(pbmp_str, 0, (sizeof(char) * FORMAT_PBMP_MAX));
        SHR_CLI_EXIT(_SHR_E_PARAM, "Port %s either doesn't exists or is not enabled.\n",
            shr_pbmp_range_format(check_pbmp, pbmp_str, sizeof(pbmp_str)));
    }

exit:
    if (NULL != port_config)
    {
        sal_free(port_config);
    }
    SHR_FUNC_EXIT;
}

static int
diag_dnxc_phy_prbs_stat_init(int unit)
{
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    if (DNXC_PRBS_STAT_IS_INIT(pscb->flags)) {
        return 1;
    }

    sal_memset(pscb, 0, sizeof(dnxc_prbs_stat_cb_t));
    if ((pscb->lock = sal_mutex_create("PRBSStat lock")) == NULL) {
        return 0;
    }

    if ((pscb->sem = sal_sem_create("PRBSStat sleep", sal_sem_BINARY, 0)) == NULL) {
        return 0;
    }

    pscb->flags |= DNXC_PRBS_STAT_F_INIT;
    return 1;
}

static void
diag_dnxc_phy_prbs_stat_counter_init(int unit)
{
    int lane;
    int status;
    bcm_port_t port;
    bcm_gport_t gport;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_counter_t *psco;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    BCM_PBMP_ITER(pscb->pbmp, port) {
        pspi = &(pscb->pinfo[port]);
        psco = pspi->counters;

        sal_memset(psco, 0, sizeof(dnxc_prbs_stat_counter_t)*DNXC_PRBS_STAT_MAX_LANES);
        sal_memset(&pspi->ber, 0, sizeof(dnxc_prbs_stat_ber_t)*DNXC_PRBS_STAT_MAX_LANES);

        /* Prime PRBS once to clear hardware counters */
        for (lane = 0; lane < pspi->lanes; lane++) {
            if (IS_SFI_PORT(unit, port)) {
                bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &status);
            } else {
                BCM_PHY_GPORT_LANE_PORT_SET(gport, lane, port);
                bcm_port_control_get(unit, gport, bcmPortControlPrbsRxStatus, &status);
            }
            pspi->intervals[lane] = 0;
        }
    }
}

static shr_error_e
diag_dnxc_phy_prbs_stat_dump_cfg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t port;
    char buf[FORMAT_PBMP_MAX];
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    cli_out("PRBSStat interval=%ds\n", pscb->secs);

    PRT_TITLE_SET("PRBSStat Ports");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Bit");
    PRT_COLUMN_ADD("Lanes");

    BCM_PBMP_ITER(pscb->pbmp, port) {
        bcm_pbmp_t pbmp;
        pspi = &(pscb->pinfo[port]);

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", BCM_PORT_NAME(unit, port));

        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, port);
        sal_memset(buf, 0, (sizeof(char) * FORMAT_PBMP_MAX));
        PRT_CELL_SET("%s", shr_pbmp_range_format(pbmp, buf, sizeof(buf)));

        PRT_CELL_SET("%d", pspi->lanes);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_phy_prbs_stat_show_counters(
    int unit,
    bcm_pbmp_t *pbmp,
    sh_sand_control_t *sand_control)
{
    int lane;
    int unlocked = 0;
    bcm_port_t port;
    uint64 delta_error;
    uint64 delta_losslock;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_counter_t *psco;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("PRBSStat Counters");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Stats");
    PRT_COLUMN_ADD("Accumulated count");
    PRT_COLUMN_ADD("Last count");
    PRT_COLUMN_ADD("Count Per Second");

    BCM_PBMP_ITER(*pbmp, port) {
        pspi = &(pscb->pinfo[port]);
        psco = pspi->counters;

        for (lane = 0; lane < pspi->lanes; lane++) {
            if (pspi->state[lane] == DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST) {
                unlocked++;
            }
        }

        if (pspi->lanes == unlocked) {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", BCM_PORT_NAME(unit, port));
            PRT_CELL_SET("No PRBS lock");
            PRT_CELL_SET("N/A");
            PRT_CELL_SET("N/A");
            PRT_CELL_SET("N/A");
            unlocked = 0;
            continue;
        }

        for (lane = 0; lane < pspi->lanes; lane++) {
            char buf[64];
            delta_error = psco[lane].acc.errors - psco[lane].cur.errors;
            delta_losslock = psco[lane].acc.losslock - psco[lane].cur.losslock;

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s[%d]", BCM_PORT_NAME(unit, port), lane);

            if (delta_losslock) {
                PRT_CELL_SET("LossLock");

                /* Show couters since beginning */
                format_uint64_decimal(buf, psco[lane].acc.losslock, ',');
                PRT_CELL_SET("%s", buf);

                /* Show counters since last show */
                format_uint64_decimal(buf, delta_losslock, ',');
                PRT_CELL_SET("%s", buf);

                /* No rate */
                PRT_CELL_SET("");

                DNXC_PRBS_STAT_LOCK(pscb->lock);
                psco[lane].cur.losslock = psco[lane].acc.losslock;
                DNXC_PRBS_STAT_UNLOCK(pscb->lock);
            } else {
                if (delta_error) {
                    PRT_CELL_SET("Errors");
                    /* Show couters since beginning */
                    format_uint64_decimal(buf, psco[lane].acc.errors, ',');
                    PRT_CELL_SET("%s", buf);
                    /* Show counters since last show */
                    format_uint64_decimal(buf, delta_error, ',');
                    PRT_CELL_SET("%s", buf);

                    /* Show counters per second */
                    format_uint64_decimal(buf, delta_error/(pscb->secs * pspi->intervals[lane]), ',');
                    PRT_CELL_SET("%s/s", buf);
                } else {
                    PRT_CELL_SET("No Errors");
                    /* Show couters since beginning */
                    format_uint64_decimal(buf, psco[lane].acc.errors, ',');
                    PRT_CELL_SET("%s", buf);
                    PRT_CELL_SET("0");
                    PRT_CELL_SET("0/s");
                }

                DNXC_PRBS_STAT_LOCK(pscb->lock);
                pspi->intervals[lane] = 0;
                psco[lane].cur.errors = psco[lane].acc.errors;
                DNXC_PRBS_STAT_UNLOCK(pscb->lock);

            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_phy_prbs_stat_show_ber(
    int unit,
    bcm_pbmp_t *pbmp,
    sh_sand_control_t *sand_control)
{
    int lane;
    bcm_port_t port;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("PRBSStat Ber");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Stats");
    PRT_COLUMN_ADD("Ber");

    BCM_PBMP_ITER(*pbmp, port) {
        pspi = &(pscb->pinfo[port]);

        for (lane = 0; lane < pspi->lanes; lane++) {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s[%d]", BCM_PORT_NAME(unit, port), lane);

            if (pspi->state[lane] == DIAG_DNXC_PHY_PRBS_STAT_PREV_LOCK_LOST) {
                PRT_CELL_SET("LossOfLock");
                PRT_CELL_SET("");
            } else if (pspi->state[lane] == DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST) {
                PRT_CELL_SET("No lock");
                PRT_CELL_SET("");
            } else {
                PRT_CELL_SET("PRBS locked");
                PRT_CELL_SET("%d.%02de-%02d", pspi->ber[lane].ber_int,
                    pspi->ber[lane].ber_remainder,
                    pspi->ber[lane].ber_pow);
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * If there is a port configuration change, update the new configuration
 * and clear counters, BER
 */
static shr_error_e
diag_dnxc_phy_prbs_stat_pinfo_update(int unit, bcm_port_t port)
{
    bcm_port_resource_t resource;
    portmod_speed_config_t port_speed_config;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_counter_t *psco;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    pspi = &(pscb->pinfo[port]);

    portmod_speed_config_t_init(unit, &port_speed_config);
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));
    bcm_port_resource_t_init(&resource);
    SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &resource));
    if ((pspi->speed == resource.speed) &&
        (pspi->lanes == port_speed_config.num_lane) &&
        (pspi->fec_type == resource.fec_type)) {
        SHR_EXIT();
    }

    /* Update port configs */
    DNXC_PRBS_STAT_LOCK(pscb->lock);
    pspi->speed = resource.speed;
    pspi->lanes = port_speed_config.num_lane;
    pspi->fec_type = resource.fec_type;

    psco = pspi->counters;
    sal_memset(psco, 0, sizeof(dnxc_prbs_stat_counter_t)*DNXC_PRBS_STAT_MAX_LANES);
    sal_memset(&pspi->ber, 0, sizeof(dnxc_prbs_stat_ber_t)*DNXC_PRBS_STAT_MAX_LANES);
    DNXC_PRBS_STAT_UNLOCK(pscb->lock);

exit:
    SHR_FUNC_EXIT;
}


extern shr_error_e
dnxc_phy_diag_serdes_rate_get(
    int unit,
    bcm_port_t port,
    uint32 *serdes_rate);


static shr_error_e
diag_dnxc_phy_prbs_stat_ber_compute(int unit, bcm_port_t port,
                                uint64 delta, sal_time_t secs, dnxc_prbs_stat_ber_t *ber)
{
    uint32 rate, ber_pow, ber_int = 0, ber_remainder = 0;
    uint64 total_bits, temp_status;
    uint64 base = COMPILER_64_INIT(0, 0);
    uint64 ber_int64, ber_remainder64;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    /* Make sure BER is computed at max if no errors */
    if (delta == 0) {
        delta = 1;
    }

    pspi = &(pscb->pinfo[port]);
    dnxc_phy_diag_serdes_rate_get(unit, port, &rate);

    /*
     * Get the total bits
     */
    COMPILER_64_SET(total_bits, 0, secs * 1000000);
    COMPILER_64_UMUL_32(total_bits, rate);

    for (ber_pow = 0; ber_pow <= dnxc_prbs_stat_max_pow; ber_pow++)
    {
        if (ber_pow == 0) {
            COMPILER_64_SET(base, 0, 1);
        } else {
            COMPILER_64_UMUL_32(base, 10);
        }

        COMPILER_64_COPY(temp_status, base);
        COMPILER_64_UMUL_32(temp_status, delta);

        COMPILER_64_COPY(ber_int64, temp_status);
        COMPILER_64_UDIV_64(ber_int64, total_bits);

        COMPILER_64_TO_32_LO(ber_int, ber_int64);

        if (ber_int >= 1)
        {
            break;
        }
        else if (ber_pow == dnxc_prbs_stat_max_pow)
        {
            /** coverity[copy_paste_error:FALSE] */
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error happened while calculating BER!\n");
        }
    }
    /*
     * BER = nof_errors / (interval * serdes rate)
     */
    COMPILER_64_UMUL_32(temp_status, 100);
    COMPILER_64_COPY(ber_remainder64, temp_status);
    COMPILER_64_UDIV_64(ber_remainder64, total_bits);
    COMPILER_64_TO_32_LO(ber_remainder, ber_remainder64);
    ber_remainder = ber_remainder % 100;

    ber->ber_int = ber_int;
    ber->ber_remainder = ber_remainder;
    ber->ber_pow = ber_pow;

exit:
    SHR_FUNC_EXIT;
}

static void
diag_dnxc_phy_prbs_stat_ber_update(int unit, bcm_port_t port, int lane, int delta)
{
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    pspi = &(pscb->pinfo[port]);
    sal_memset(&(pspi->ber[lane]), 0, sizeof(dnxc_prbs_stat_ber_t));

    if (delta >= 0) {
        diag_dnxc_phy_prbs_stat_ber_compute(unit, port, delta, pscb->secs, &(pspi->ber[lane]));
    }
}


static int
diag_dnxc_phy_prbs_stat_collect(int unit, bcm_port_t port)
{
    int rv;
    int lane;
    int status;
    uint64 lcount;
    bcm_gport_t gport;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_counter_t *psco;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    diag_dnxc_phy_prbs_stat_pinfo_update(unit, port);
    pspi = &(pscb->pinfo[port]);
    psco = pspi->counters;

    DNXC_PRBS_STAT_LOCK(pscb->lock);
    for (lane = 0; lane < pspi->lanes; lane++) {
        lcount = 0;

        if (IS_SFI_PORT(unit, port)) {
            rv = bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &status);
        } else {
            BCM_PHY_GPORT_LANE_PORT_SET(gport, lane, port);
            rv = bcm_port_control_get(unit, gport, bcmPortControlPrbsRxStatus, &status);
        }
        if (BCM_FAILURE(rv)) {
            DNXC_PRBS_STAT_UNLOCK(pscb->lock);
            return 0;
        }

        switch (status) {
            case DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK:
                pspi->state[lane] = DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK;
                break;

            case DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST:
                pspi->state[lane] = DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK_LOST;
                break;

            case DIAG_DNXC_PHY_PRBS_STAT_PREV_LOCK_LOST:
                psco[lane].acc.losslock++;
                pspi->state[lane] = DIAG_DNXC_PHY_PRBS_STAT_PREV_LOCK_LOST;
                break;

            default:
                COMPILER_64_SET(lcount, 0, status);
                psco[lane].acc.errors += lcount;
                pspi->intervals[lane]++;
                pspi->state[lane] = DIAG_DNXC_PHY_PRBS_STAT_CURR_LOCK;
                if (status != DIAG_DNXC_PHY_PRBS_STAT_COUNTER_OVF) {
                    /*
                     * Do not update BER to preserve the loss of lock event.
                     * The next show BER will 'unlatch' the loss of lock event
                     * and restart BER calculation. Without this, the loss
                     * of lock event will be lost if multiple intervals
                     * transpire between show ber calls.
                     */
                    diag_dnxc_phy_prbs_stat_ber_update(unit, port, lane, (int)status);
                }
        }
    }
    DNXC_PRBS_STAT_UNLOCK(pscb->lock);

    return 1;
}

static void
diag_dnxc_phy_prbs_stat_thread(int unit)
{
    int u_interval;
    bcm_port_t port;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        dnx_err_recovery_utils_excluded_thread_add(unit);
    }
#endif

    while (pscb->secs) {
        u_interval = pscb->secs * 1000000; /* Adjust to usec */

        BCM_PBMP_ITER(pscb->pbmp, port) {
            if (0 == diag_dnxc_phy_prbs_stat_collect(unit, port)) {
                cli_out("Failed collecting PRBS stats for port %d.\n", port);
            }
        }

        sal_sem_take(pscb->sem, u_interval);
    }

    DNXC_PRBS_STAT_LOCK(pscb->lock);
    BCM_PBMP_ITER(pscb->pbmp, port) {
        sal_memset(pscb->pinfo[port].counters,
            0, sizeof(dnxc_prbs_stat_counter_t)*DNXC_PRBS_STAT_MAX_LANES);
    }
    pscb->flags &= ~DNXC_PRBS_STAT_F_RUNNING;
    DNXC_PRBS_STAT_UNLOCK(pscb->lock);

    pscb->thread_id = NULL;
    cli_out("PRBSStat thread exiting...\n");
    sal_thread_exit(0);
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_start(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_port_present = FALSE;
    int is_interval_present = FALSE;
    bcm_pbmp_t pbmp;
    int interval;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    if (0 == diag_dnxc_phy_prbs_stat_init(unit)) {
        SHR_CLI_EXIT(_SHR_E_INIT, "PRBSStat: Initialization failed.\n");
    }

    SH_SAND_IS_PRESENT("pbmp", is_port_present);
    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_IS_PRESENT("interval", is_interval_present);
    SH_SAND_GET_INT32("interval", interval);

    if (FALSE == is_port_present && FALSE == is_interval_present) {
        if (!DNXC_PRBS_STAT_IS_RUNNING(pscb->flags)) {
            cli_out("PRBSStat: not running\n");
        } else {
            diag_dnxc_phy_prbs_stat_dump_cfg(unit, args, sand_control);
        }

        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_validate_pbmp(unit, &pbmp));

    /*
     * Allow start to be called while thread has already been running to update
     * interval and pbmp
     */
    if (DNXC_PRBS_STAT_IS_RUNNING(pscb->flags)) {
        DNXC_PRBS_STAT_LOCK(pscb->lock);
    }

    if (TRUE == is_port_present) {
        BCM_PBMP_ASSIGN(pscb->pbmp, pbmp);
    }

    if (interval != pscb->secs) {
        pscb->secs = interval;
    }

    diag_dnxc_phy_prbs_stat_counter_init(unit);

    if (DNXC_PRBS_STAT_IS_RUNNING(pscb->flags)) {
        DNXC_PRBS_STAT_UNLOCK(pscb->lock);
        SHR_EXIT();
    }

    pscb->thread_id = sal_thread_create("PRBSStat",
                                    SAL_THREAD_STKSZ,
                                    100,
                                    (void (*)(void*))diag_dnxc_phy_prbs_stat_thread,
                                    INT_TO_PTR(unit));
    if (pscb->thread_id == SAL_THREAD_ERROR) {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "PRBSStat: Could not create PRBSStat thread\n");
        pscb->flags &= ~DNXC_PRBS_STAT_F_RUNNING;
        SHR_EXIT();
    }

    cli_out("PRBSStat thread started...\n");
    pscb->flags |= DNXC_PRBS_STAT_F_RUNNING;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_stop(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    if (DNXC_PRBS_STAT_IS_RUNNING(pscb->flags)) {
        /* Signal thread to stop running */
        pscb->secs = 0;
        cli_out("PRBSStat stopping thread.\n");
        sal_sem_give(pscb->sem);
    } else {
        cli_out("PRBSStat thread is not running.\n");
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_counters(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_port_present = FALSE;
    bcm_pbmp_t pbmp;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    if (!DNXC_PRBS_STAT_IS_RUNNING(pscb->flags)) {
        cli_out("PRBSStat: not running\n");
        SHR_EXIT();
    }

    SH_SAND_IS_PRESENT("pbmp", is_port_present);
    if (FALSE == is_port_present) {
        BCM_PBMP_ASSIGN(pbmp, pscb->pbmp);
    } else {
        SH_SAND_GET_PORT("pbmp", pbmp);
    }

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_validate_pbmp(unit, &pbmp));

    diag_dnxc_phy_prbs_stat_show_counters(unit, &pbmp, sand_control);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_ber(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_port_present = FALSE;
    bcm_pbmp_t pbmp;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    if (!DNXC_PRBS_STAT_IS_RUNNING(pscb->flags)) {
        cli_out("PRBSStat: not running\n");
        SHR_EXIT();
    }

    SH_SAND_IS_PRESENT("pbmp", is_port_present);
    if (FALSE == is_port_present) {
        BCM_PBMP_ASSIGN(pbmp, pscb->pbmp);
    } else {
        SH_SAND_GET_PORT("pbmp", pbmp);
    }

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_validate_pbmp(unit, &pbmp));

    diag_dnxc_phy_prbs_stat_show_ber(unit, &pbmp, sand_control);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_prbs_stat_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_port_present = FALSE;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    dnxc_prbs_stat_pinfo_t *pspi;
    dnxc_prbs_stat_counter_t *psco;
    dnxc_prbs_stat_cb_t *pscb = &dnxc_prbs_stat_cb[unit];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("pbmp", is_port_present);
    if (FALSE == is_port_present) {
        BCM_PBMP_ASSIGN(pbmp, pscb->pbmp);
    } else {
        SH_SAND_GET_PORT("pbmp", pbmp);
    }

    SHR_IF_ERR_EXIT(diag_dnxc_phy_prbs_stat_validate_pbmp(unit, &pbmp));

    DNXC_PRBS_STAT_LOCK(pscb->lock);
    BCM_PBMP_ITER(pbmp, port) {
        pspi = &(pscb->pinfo[port]);
        psco = pspi->counters;

        sal_memset(psco, 0, sizeof(dnxc_prbs_stat_counter_t)*DNXC_PRBS_STAT_MAX_LANES);
    }
    DNXC_PRBS_STAT_UNLOCK(pscb->lock);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnxc_phy_prbsst_cmds[] = {
    /*keyword,          action,                                                 command, options,                                                                man*/
    {"start",       cmd_dnxc_phy_prbs_stat_start,       NULL, dnxc_phy_prbs_stat_start_options,           &dnxc_phy_prbs_stat_start_man},
    {"stop",        cmd_dnxc_phy_prbs_stat_stop,        NULL, NULL,                                       &dnxc_phy_prbs_stat_stop_man},
    {"counters",    cmd_dnxc_phy_prbs_stat_counters,    NULL, dnxc_phy_prbs_stat_counters_options,        &dnxc_phy_prbs_stat_counters_man},
    {"ber",         cmd_dnxc_phy_prbs_stat_ber,         NULL, dnxc_phy_prbs_stat_ber_options,             &dnxc_phy_prbs_stat_ber_man},
    {"clear",       cmd_dnxc_phy_prbs_stat_clear,       NULL, dnxc_phy_prbs_stat_clear_options,           &dnxc_phy_prbs_stat_clear_man},
    {NULL}
};

sh_sand_man_t dnxc_phy_prbsstat_man = {
    .brief    = "Diagnostic for PRBSSTat",
    .full     = "Diagnostic commands for collecting/displaying PRBS status.",
};

