/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2008
 *
 * *********************************************************************
 *
 * @filename           spgrc.h
 *
 * @purpose            Declarations related to graceful restart
 *
 * @component          Routing OSPF Component  
 *
 *
 * @create             12/18/2008
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef spgrc_h
#define spgrc_h

#include "std.h"
#include "datatypes.h"

/* Length of grace LSA TLVs as defined in RFC 3623. */
#define O2_GRACE_PERIOD_TLV_LEN    4
#define O2_RESTART_REASON_TLV_LEN  1
#define O2_IP_ADDRESS_TLV_LEN      4

/* Total length of grace LSA body */
#define O2_GRACE_LSA_LEN          24

/* Restart reason codes from grace LSA. RFC 3623. */
typedef enum
{
  O2_GR_REASON_UNKNOWN = 0,
  O2_GR_REASON_SW_RESTART = 1,
  O2_GR_REASON_SW_RELOAD = 2,
  O2_GR_REASON_FAILOVER = 3,
  O2_GR_REASON_NOT_RESTARTING = 100
  
} e_o2RestartReason;

typedef enum
{
  O2_GR_TLV_GRACE_PERIOD = 1,
  O2_GR_TLV_RESTART_REASON = 2,
  O2_GR_TLV_IP_ADDRESS = 3
} e_o2GraceLsaTlvType;

/* statistics that apply to the restarting router. Some of these are of
 * questionable value since counters are not retained across a restart. */
typedef struct restarting_router_stats_s
{
  /* number of planned graceful restarts */
  L7_uint32 plannedRestarts;

  /* number of unplanned restarts */
  L7_uint32 unplannedRestarts;

  /* Number of grace LSAs sent */
  L7_uint32 graceLsasSent;

  /* Error building grace LSA */
  L7_uint32 graceLsaBuildFail;

  /* Number of times construction of a grace LSA failed */
  L7_uint32 graceLsaSendFail;

  /* Number of times the grace period expired */
  L7_uint32 gracePeriodExpire;

  /* Successful completion of graceful restart */
  L7_uint32 successfulGracefulRestart;

  /* Premature termination of graceful restart */
  L7_uint32 failedGracefulRestart;

} restarting_router_stats_t;

/* statistics that apply to the helpful neighbor */
typedef struct helpful_neighbor_stats_s
{
  /* Number of grace LSAs received */
  L7_uint32 graceLsasReceived;

  /* Number of grace LSAs received from a neighbor not in FULL state */
  L7_uint32 graceLsaNbrNotFull;

  /* Refused to act as helpful neighbor because topology change was pending
   * when grace LSA received. */
  L7_uint32 topoChangePending;

  /* Grace LSA received after the grace period had already expired */
  L7_uint32 staleGraceLsa;

  /* Grace LSA received while a restart was already in progress */
  L7_uint32 ownRestartInProgress;

  /* no restart reason included in grace LSA */
  L7_uint32 noRestartReason;

  /* Local policy prohibits this router from being a helpful neighbor */
  L7_uint32 localPolicyConflict;

  /* updated the grace period for a neighbor */
  L7_uint32 gracePeriodUpdate;

  /* Failure entering helper mode for a neighbor */
  L7_uint32 failedToEnterHelperMode;

  /* Number of times this router entered helper mode for a neighbor */
  L7_uint32 enterHelperMode;

  /* Number of times this router exited helper mode for a neighbor */
  L7_uint32 exitHelperMode;

  /* Number of times we terminated helper mode because the grace period expired */
  L7_uint32 gracePeriodExpire;

  /* Call to exit helper mode when neighbor not restarting */
  L7_uint32 unexpectedExit;
  
} helpful_neighbor_stats_t;

/* Some debug counters for graceful restart */
/* ROBRICE - need to initialize these */
typedef struct ospf_gr_stats_s
{
  helpful_neighbor_stats_t helpfulNbrStats;

  restarting_router_stats_t restartingRtrStats;

} ospf_gr_stats_t;


#endif /* spgrc_h */
