/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_sm.c
*
* @purpose   Multiple Spanning tree State Machine
*
* @component dot1s
*
* @comments
*
* @create    9/19/2002
*
* @author    skalyanam
*
* @end
*
**********************************************************************/

#include "dot1s_include.h"
#include "dot1s_nsf.h"
#include "dot1q_api.h"

extern  DOT1S_PORT_STATS_t      *dot1sPortStats;
extern  DOT1S_INSTANCE_INFO_t   *dot1sInstance;
static  L7_uint32 dot1sDepth = 0;
static  DOT1S_CIST_PRIORITY_VECTOR_t  cistRootPathPriVec[L7_MAX_INTERFACE_COUNT+1];
static  DOT1S_PORTID_t          cistRootPathPortId[L7_MAX_INTERFACE_COUNT+1];
static  DOT1S_MSTI_PRIORITY_VECTOR_t  mstiRootPathPriVec[L7_MAX_INTERFACE_COUNT+1];
static  DOT1S_PORTID_t          mstiRootPathPortId[L7_MAX_INTERFACE_COUNT+1];
static  L7_ushort16 VIDList[L7_MAX_VLANS + 1];

extern L7_INTF_MASK_t portCommonChangeMask;
extern L7_INTF_MASK_t *portInstInfoChangeMask;



static dot1s_pim_states_t pimStateTable[dot1sPimEvents][PIM_STATES] =
{
/*Ev/St PIM_DISABLED   PIM_AGED      PIM_UPDATE    PIM_CURRENT   PIM_OTHER    PIM_NOT_DESIG      PIM_REP_DESIG  PIM_SUP_DESIG  PIM_RECEIVE  PIM_INFERIOR_DESIG */
/*E1*/ {PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED,  PIM_DISABLED,  PIM_DISABLED,  PIM_DISABLED},
/*E2*/ {PIM_DISABLED, PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES,    PIM_STATES},
/*E3*/ {PIM_DISABLED, PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES,    PIM_STATES},
/*E4*/ {PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_SUP_DESIG,    PIM_STATES},
/*E5*/ {PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_REP_DESIG,    PIM_STATES},
/*E5A*/{PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_INFERIOR_DESIG,    PIM_STATES},
/*E6*/ {PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_NOT_DESIG,    PIM_STATES},
/*E7*/ {PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_OTHER,    PIM_STATES},
/*E8*/ {PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_RECEIVE,  PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES,    PIM_STATES},
/*E9*/ {PIM_STATES,   PIM_UPDATE,   PIM_STATES,   PIM_UPDATE,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES,    PIM_STATES},
/*E10*/{PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_AGED,     PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES,    PIM_STATES},
/*E11*/{PIM_AGED,     PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES,    PIM_STATES},
/*E12*/{PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED,  PIM_DISABLED,  PIM_DISABLED,  PIM_DISABLED}
};
/*static dot1s_pim_states_t pimStateTable[dot1sPimEvents][PIM_STATES] =*/
/*{ */
/*Ev/St PIM_DISABLED   PIM_ENABLE    PIM_AGED      PIM_UPDATE    PIM_CURRENT   PIM_OTHER    PIM_NOT_DESIG      PIM_REP_DESIG  PIM_SUP_DESIG  PIM_RECEIVE   */
/*E1*/ /*{PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED,  PIM_DISABLED,  PIM_DISABLED  },*/
/*E2*/ /*{PIM_DISABLED, PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES    }, */
/*E3*/ /*{PIM_DISABLED, PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES    }, */
/*E4*/ /*{PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_SUP_DESIG }, */
/*E5*/ /*{PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_REP_DESIG }, */
/*E6*/ /*{PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_NOT_DESIG      }, */
/*E7*/ /*{PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_OTHER     }, */
/*E8*/ /*{PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_RECEIVE,  PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES    }, */
/*E9*/ /*{PIM_STATES,   PIM_STATES,   PIM_UPDATE,   PIM_STATES,   PIM_UPDATE,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES    }, */
/*E10*//*{PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_AGED,     PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES    }, */
/*E11*//*{PIM_ENABLED,  PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,   PIM_STATES,    PIM_STATES,    PIM_STATES    }, */
/*E12*//*{PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED, PIM_DISABLED,  PIM_DISABLED,  PIM_DISABLED  }  */
/*}; */

static dot1s_prs_states_t prsStateTable[dot1sPrsEvents-dot1sPimEvents-1][PRS_STATES] =
{
/*Ev/St PRS_INIT    PRS_RECEIVE  */
/*E1*/ {PRS_INIT,   PRS_INIT},
/*E13*/{PRS_STATES, PRS_RECEIVE}
};


static dot1s_prt_disabledStates_t prtDisabledStateTable[dot1sPrtDisabledEvents-dot1sPrsEvents-1][PRT_DISABLED_STATES] =
{
/*Ev/St  INIT        DISABLE_PORT       DISABLED_PORT      */
/*16*/ {PRT_INIT_PORT,   PRT_INIT_PORT,   PRT_INIT_PORT},
/*17*/ {PRT_DISABLE_PORT,  PRT_DISABLE_PORT,  PRT_DISABLE_PORT},
/*18*/ {PRT_DISABLED_STATES,  PRT_DISABLED_PORT,  PRT_DISABLED_STATES},
/*19*/ {PRT_DISABLED_STATES,  PRT_DISABLED_STATES,  PRT_DISABLED_PORT},
/*20*/ {PRT_DISABLED_STATES, PRT_DISABLED_STATES,PRT_DISABLED_PORT},
/*21*/ {PRT_DISABLED_STATES, PRT_DISABLED_STATES, PRT_DISABLED_PORT},
/*22*/ {PRT_DISABLED_STATES, PRT_DISABLED_STATES, PRT_DISABLED_PORT}

};





static dot1s_prt_masterStates_t prtMasterStateTable[dot1sPrtMasterEvents-dot1sPrtDisabledEvents-1][PRT_MASTER_STATES - PRT_MASTER_FIRST] =
{
/*Ev/St  PRT_MASTER_PROPOSED        PRT_MASTER_AGREED       PRT_MASTER_SYNCED      PRT_MASTER_RETIRED     PRT_MASTER_PORT    PRT_MASTER_DISCARD   PRT_MASTER_LEARN      PRT_MASTER_FORWARD     */
/*3.2.1*/  {PRT_MASTER_PORT,  PRT_MASTER_PORT,   PRT_MASTER_PORT,   PRT_MASTER_PORT,   PRT_MASTER_PORT,    PRT_MASTER_PORT,   PRT_MASTER_PORT,   PRT_MASTER_PORT},
/*3.2.2*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_PROPOSED,PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.3*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_AGREED,  PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.4*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_AGREED,  PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.5*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_SYNCED,  PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.6*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_SYNCED,  PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.7*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_SYNCED,  PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.8*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_SYNCED,  PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.9*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_RETIRED, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.10*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_DISCARD, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.11*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_LEARN,   PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},
/*3.2.12*/ {PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_FORWARD, PRT_MASTER_STATES, PRT_MASTER_STATES, PRT_MASTER_STATES},

};



static dot1s_prt_desigStates_t prtDesigStateTable[dot1sPrtDesigEvents-dot1sPrtMasterEvents-1][PRT_DESIG_STATES - PRT_DESIG_FIRST] =
{
  /*Ev/St  PRT_DESIG_PROPOSE        PRT_DESIG_AGREED       PRT_DESIG_SYNCED      PRT_DESIG_RETIRED     PRT_DESIG_PORT    PRT_DESIG_DISCARD   PRT_DESIG_LEARN      PRT_DESIG_FORWARD     */
/*3.3.1*/ {PRT_DESIG_PORT,   PRT_DESIG_PORT,   PRT_DESIG_PORT,   PRT_DESIG_PORT,   PRT_DESIG_PORT,   PRT_DESIG_PORT,   PRT_DESIG_PORT,   PRT_DESIG_PORT},
/*3.3.2*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_PROPOSE,PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},
/*3.3.3*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_AGREED, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},
/*3.3.4*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_SYNCED, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},
/*3.3.5*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_SYNCED, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},
/*3.3.6*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_SYNCED, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},
/*3.3.7*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_SYNCED, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},
/*3.3.8*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_RETIRED,PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},
/*3.3.9*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_DISCARD,PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},
/*3.3.10*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_LEARN,  PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},
/*3.3.11*/ {PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_FORWARD,PRT_DESIG_STATES, PRT_DESIG_STATES, PRT_DESIG_STATES},

};



static dot1s_prt_rootStates_t prtRootStateTable[dot1sPrtRootEvents-dot1sPrtDesigEvents-1][PRT_ROOT_STATES - PRT_ROOT_FIRST] =
{
  /*Ev/St  PRT_ROOT_PROPOSED        PRT_ROOT_AGREED       PRT_ROOT_SYNCED      PRT_REROOT     PRT_ROOT_PORT    PRT_REROOTED   PRT_ROOT_LEARN      PRT_ROOT_FORWARD     */
  /*3.4.1*/ {PRT_ROOT_PORT,   PRT_ROOT_PORT,   PRT_ROOT_PORT,   PRT_ROOT_PORT,   PRT_ROOT_PORT,    PRT_ROOT_PORT,   PRT_ROOT_PORT,   PRT_ROOT_PORT},
  /*3.4.2*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_PROPOSED,PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},
  /*3.4.3*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_AGREED,  PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},
  /*3.4.4*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_AGREED,  PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},
  /*3.4.5*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_SYNCED,  PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},
  /*3.4.6*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_SYNCED,  PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},
  /*3.4.7*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_REROOT,       PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},
  /*3.4.8*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_PORT,    PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},
  /*3.4.9*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_REROOTED,     PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},
  /*3.4.10*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_LEARN,   PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},
  /*3.4.11*/ {PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_FORWARD, PRT_ROOT_STATES, PRT_ROOT_STATES, PRT_ROOT_STATES},

};


static dot1s_prt_altBkupStates_t prtAltBkupStateTable[dot1sPrtAltBackupEvents-dot1sPrtRootEvents-1][PRT_ALTBKUP_STATES - PRT_ALTBKUP_FIRST] =
{
  /*Ev/St  PRT_ALTERNATE_PROPOSED        PRT_ALTERNATE_AGREED       PRT_BLOCK_PORT      PRT_BACKUP_PORT     PRT_ALTERNATE_PORT     */
  /*3.5.1*/ {PRT_BLOCK_PORT,     PRT_BLOCK_PORT,     PRT_BLOCK_PORT,     PRT_BLOCK_PORT,     PRT_BLOCK_PORT},
  /*3.5.2*/ {PRT_BLOCK_PORT,     PRT_BLOCK_PORT,     PRT_BLOCK_PORT,     PRT_BLOCK_PORT,     PRT_BLOCK_PORT},
  /*3.5.3*/ {PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTERNATE_PROPOSED},
  /*3.5.4*/ {PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTERNATE_AGREED},
  /*3.5.5*/ {PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTERNATE_AGREED},
  /*3.5.6*/ {PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTERNATE_PORT, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES},
  /*3.5.7*/ {PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_BACKUP_PORT},
  /*3.5.8*/ {PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTERNATE_PORT},
  /*3.5.9*/ {PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTERNATE_PORT},
  /*3.5.10*/ {PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTERNATE_PORT},
  /*3.5.11*/ {PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTBKUP_STATES, PRT_ALTERNATE_PORT},

};

static dot1s_prx_states_t prxStateTable[dot1sPrxEvents-dot1sPrtEvents-1][PRX_STATES] =
{
/*Ev/St  DISACRD  RECEIVE  */
/*E1*/  {PRX_DISCARD, PRX_DISCARD},
/*E41*/ {PRX_DISCARD, PRX_DISCARD},
/*E42*/ {PRX_RECEIVE, PRX_RECEIVE}
};

static dot1s_pst_states_t pstStateTable[dot1sPstEvents-dot1sPrxEvents][PST_STATES] =
{
/*Ev/St  DISCARDING      LEARNING        FORWARDING  */
/*E1*/  {PST_DISCARDING, PST_DISCARDING, PST_DISCARDING},
/*E43*/ {PST_LEARNING,   PST_STATES,     PST_STATES},
/*E44*/ {PST_STATES,     PST_DISCARDING, PST_STATES},
/*E45*/ {PST_STATES,     PST_FORWARDING, PST_STATES},
/*E46*/ {PST_STATES,     PST_STATES,     PST_DISCARDING}
};

static dot1s_tcm_states_t  tcmStateTable[dot1sTcmEvents-dot1sPstEvents-1][TCM_STATES] =
{
/*Ev/St   INIT        INACTIVE        DETECTED    ACTIVE            NOTIFIED_TCN NOTIFIED_TC PROPAGATING ACKNOWLEDGED */
/*E1*/   {TCM_INACTIVE,   TCM_INACTIVE,       TCM_INACTIVE,   TCM_INACTIVE,         TCM_INACTIVE,    TCM_INACTIVE,   TCM_INACTIVE,   TCM_INACTIVE},
/*E47*/  {TCM_STATES, TCM_LEARNING,   TCM_STATES, TCM_NOTIFIED_TC,  TCM_STATES,  TCM_STATES, TCM_STATES, TCM_STATES},
/*E48*/  {TCM_STATES, TCM_LEARNING,   TCM_STATES, TCM_NOTIFIED_TCN, TCM_STATES,  TCM_STATES, TCM_STATES, TCM_STATES},
/*E49*/  {TCM_STATES, TCM_LEARNING,   TCM_STATES, TCM_ACKNOWLEDGED, TCM_STATES,  TCM_STATES, TCM_STATES, TCM_STATES},
/*E50*/  {TCM_STATES, TCM_LEARNING,   TCM_STATES, TCM_PROPAGATING,  TCM_STATES,  TCM_STATES, TCM_STATES, TCM_STATES},
/*E51*/  {TCM_STATES, TCM_DETECTED,   TCM_STATES, TCM_STATES,       TCM_STATES,  TCM_STATES, TCM_STATES, TCM_STATES},
/*E52*/  {TCM_STATES, TCM_DETECTED,   TCM_STATES, TCM_STATES,       TCM_STATES,  TCM_STATES, TCM_STATES, TCM_STATES},
/*E53*/  {TCM_STATES, TCM_DETECTED,   TCM_STATES, TCM_STATES,       TCM_STATES,  TCM_STATES, TCM_STATES, TCM_STATES},
  /*event 54 is removed as per D14*/
/*E54*/  /*{TCM_STATES, TCM_STATES, TCM_STATES, INACTIVE,     TCM_STATES,  TCM_STATES, TCM_STATES, TCM_STATES},*/
/*E55*/  {TCM_STATES, TCM_STATES,     TCM_STATES, TCM_INACTIVE,         TCM_STATES,  TCM_STATES, TCM_STATES, TCM_STATES}
};

/******************************************************************************/
/************the state table acc. IEEE 802.1D***********/
/******************************************************************************/
/*static dot1s_ppm_states_t  ppmStateTable[dot1sPpmEvents-dot1sTcmEvents-1][PPM_STATES] = */
/*{*/
/*Ev/St   INIT           SEND_RSTP   SENDING_RSTP      SEND_STP    SENDING_STP  */
/*E1*//*   {PPM_INIT,      PPM_INIT,   PPM_INIT,         PPM_INIT,   PPM_INIT       },*/
/*E56*//*  {PPM_INIT,      PPM_INIT,   PPM_INIT,         PPM_INIT,   PPM_INIT       },*/
/*E57*//*  {PPM_SEND_RSTP, PPM_STATES, PPM_STATES,       PPM_STATES, PPM_STATES },*/
/*E58*//*  {PPM_STATES,      PPM_STATES, PPM_SENDING_RSTP, PPM_STATES, PPM_SENDING_STP},*/
/*E59*//*  {PPM_STATES,    PPM_STATES, PPM_SENDING_RSTP, PPM_STATES, PPM_SENDING_STP},*/
/*E60*//*  {PPM_SEND_STP,  PPM_STATES, PPM_STATES,       PPM_STATES, PPM_STATES },*/
/*E61*//*  {PPM_STATES,    PPM_STATES, PPM_STATES,       PPM_STATES, PPM_SEND_RSTP  },*/
/*E62*//*  {PPM_STATES,    PPM_STATES, PPM_SEND_RSTP,    PPM_STATES, PPM_SEND_RSTP  },*/
/*E63*//*  {PPM_STATES,    PPM_STATES, PPM_SEND_STP,     PPM_STATES, PPM_STATES },*/
/*E64*//*  {PPM_STATES,    PPM_STATES, PPM_SEND_STP,     PPM_STATES, PPM_STATES } */
/*};*/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*The new state table for the PPM is defined in IEEE 802.1y*/
/******************************************************************************/
static dot1s_ppm_states_t  ppmStateTable[dot1sPpmEvents-dot1sTcmEvents-1][PPM_STATES] =
{
/*EV/St   CHECKING_RSTP      SELECTING_STP      SENSING       */
/*E1 */  {PPM_CHECKING_RSTP, PPM_CHECKING_RSTP, PPM_CHECKING_RSTP},
/*E56*/  {PPM_CHECKING_RSTP, PPM_STATES,      PPM_STATES},
/*E57*/  {PPM_SENSING,     PPM_SENSING,       PPM_STATES},
/*E58*/  {PPM_STATES,        PPM_SENSING,       PPM_CHECKING_RSTP},
/*E59*/  {PPM_STATES,      PPM_SENSING,       PPM_CHECKING_RSTP},
/*E60*/  {PPM_STATES,        PPM_STATES,        PPM_SELECTING_STP},
/*E61*/  {PPM_STATES,        PPM_STATES,        PPM_CHECKING_RSTP}
};
/******************************************************************************/
/******************************************************************************/
static dot1s_ptx_states_t  ptxStateTable[dot1sPtxEvents-dot1sPpmEvents-1][PTX_STATES] =
{
/*Ev/St  INIT        PERIODIC    IDLE          RSTP        TCN         CONFIG      */
/*E1*/  {PTX_INIT,   PTX_INIT,   PTX_INIT,     PTX_INIT,   PTX_INIT,   PTX_INIT},
/*E65*/ {PTX_STATES, PTX_STATES, PTX_PERIODIC, PTX_STATES, PTX_STATES, PTX_STATES},
/*E66*/ {PTX_STATES, PTX_STATES, PTX_RSTP,     PTX_STATES, PTX_STATES, PTX_STATES},
/*E67*/ {PTX_STATES, PTX_STATES, PTX_TCN,      PTX_STATES, PTX_STATES, PTX_STATES},
/*E68*/ {PTX_STATES, PTX_STATES, PTX_CONFIG,   PTX_STATES, PTX_STATES, PTX_STATES}
};
/******************************************************************************/
/************the state table below is being redefined in IEEE 802.1y***********/
/******************************************************************************/
static dot1s_bdm_states_t  bdmStateTable[dot1sBdmEvents-dot1sPtxEvents-1][BDM_STATES] =
{
/*Ev/St  BDM_EDGE        BDM_NOT_EDGE    */
/*9.0*/  {BDM_EDGE,    BDM_EDGE},
/*9.1*/  {BDM_NOT_EDGE,  BDM_NOT_EDGE},
/*9.2*/  {BDM_NOT_EDGE,  BDM_STATES},
/*9.3*/  {BDM_NOT_EDGE,  BDM_STATES},
/*9.4*/  {BDM_STATES,    BDM_EDGE},
/*9.5*/  {BDM_STATES,    BDM_EDGE}
};

/******************************************************************************/
/*********The new state table for the BDM is defined in IEEE 802.1y************/
/******************************************************************************/
/*static dot1s_bdm_states_t  bdmStateTable[dot1sBdmEvents-dot1sPtxEvents-1][BDM_STATES] = */
/*{*/
/*Ev/St  EDGE            NOT_EDGE    */
/*E69*/ /*{BDM_EDGE,       BDM_EDGE      },*/
/*E70*/ /*{BDM_NOT_EDGE,   BDM_NOT_EDGE  },*/
/*E71*/ /*{BDM_NOT_EDGE,   BDM_STATES},*/
/*E72*/ /*{BDM_NOT_EDGE,   BDM_STATES},*/
/*E73*/ /*{BDM_STATES,     BDM_NOT_EDGE  },*/
/*E74*/ /*{BDM_STATES,     BDM_EDGE      }*/
/*};*/
/******************************************************************************/
/******************************************************************************/

/* this variable is used check for the full duplexity once in the BPDU receive path
 * this variable is then used for the rest of the BPDU processing
 * it is updated for every BPDU received on a particular port.
 * we can use this for every port receiving a BPDU as we will be processing
 * only one BPDU from one port at a time in this dot1s thread/task
 */
static L7_BOOL pointTopointCheck = L7_TRUE;
static L7_BOOL dot1s_enable_debug_detail = L7_FALSE;
static L7_BOOL isAllSynced(L7_uint32 portNum, L7_uint32 instIndex, L7_BOOL role_alternate);
static L7_uint32 forwardDelayGet(DOT1S_PORT_COMMON_t *p);

static L7_RC_t dot1sGenerateEventsForPrt(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
static L7_RC_t dot1sGenerateEventsForTcm(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
static L7_BOOL isRerooted(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex);
static void generateRequestedPortEvents(L7_uint32 instIndex );
static void generateRequestedReRootedPortEvents( L7_uint32 instIndex );
static L7_BOOL dot1sIsPortActiveForInstance(L7_uint32 instIndex, L7_uint32 intIfNum);
extern L7_BOOL *syncInProgress;

void dot1s_debug_detail (L7_BOOL enable)
{
  dot1s_enable_debug_detail = enable;
}



void dot1s_print_vec (DOT1S_CIST_PRIORITY_VECTOR_t *vec)
{
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Root ID Priority              : %d\n", vec->rootId.priInstId);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Root ID MAC                   : %02x\n", vec->rootId.macAddr[5]);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"External Root Path Cost       : %d\n", vec->extRootPathCost);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Regional Root ID Priority     : %d\n", vec->regRootId.priInstId);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Regional Root ID MAC          : %02x\n", vec->regRootId.macAddr[5]);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Internal Root Path Cost       : %d\n", vec->intRootPathCost);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Designated Bridge ID Priority : %d\n", vec->dsgBridgeId.priInstId);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Designated Bridge ID MAC      : %02x\n", vec->dsgBridgeId.macAddr[5]);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Designated Port ID            : %d\n", vec->dsgPortId);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Rx Port ID                    : %d\n", vec->rxPortId);
}
void dot1s_print_times ( char * header, DOT1S_CIST_TIMES_t *times)
{
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s:", header);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," msAge(%d) maxAge(%d)  remHops(%d)\n",
                times->msgAge, times->maxAge, times->remHops);
}
void dot1s_print_msti_vec(DOT1S_MSTI_PRIORITY_VECTOR_t *vec)
{
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Reg Root ID Priority          : %d\n", vec->regRootId.priInstId);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Reg Root ID MAC               : %02x\n", vec->regRootId.macAddr[5]);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Internal Root Path Cost       : %d\n", vec->intRootPathCost);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Designated Bridge ID Priority : %d\n", vec->dsgBridgeId.priInstId);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Designated Bridge ID MAC      : %02x\n", vec->dsgBridgeId.macAddr[5]);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Designated Port ID            : %d\n", vec->dsgPortId);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Rx Port ID                    : %d\n", vec->rxPortId);


}
void dot1s_print_msti_times ( char * header, DOT1S_MSTI_TIMES_t *times)
{
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: remHops(%d)\n", header,times->remHops);
}


void dot1s_debugPrintVectors(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex,  L7_uint32 rootPortId)
{
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Root role for %d and designated for %d  (%d)  \n", p->portNum, rootPortId, dot1sInstance->msti[instIndex].mstiRootPortId );
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Designated Priority vector \n");
  dot1s_print_msti_vec(&p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority);
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"MSTI(%d) Root Priority vector \n", instIndex);
  dot1s_print_msti_vec(&dot1sInstance->msti[instIndex].mstiRootPriority);
}

void debug_dot1s_event (DOT1S_PORT_COMMON_t *p, dot1sEvents_t dot1sEvent, L7_uint32 instIndex, L7_uint32 depth)
{


  DOT1S_DEBUG_STATE_PRINT(dot1sEvent, p, instIndex, depth);
}
/*********************************************************************
* @purpose  This is the classifier which dispatches the received dot1s event to
*           a particular state machine
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    bufHandle  @b{(input)} handle to network buffer
* @param    msg        @b{(input)} pointer to any message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sStateMachineClassifier(dot1sEvents_t dot1sEvent,
                                    DOT1S_PORT_COMMON_t *p,
                                    L7_uint32 instIndex,
                                    DOT1S_MSTP_ENCAPS_t   *bpdu,
                                    void *msg)
{
  L7_RC_t rc;
  if (p == L7_NULLPTR )
  {
    return L7_FAILURE;
  }

  if (dot1sDepth == DOT1S_THRESHOLD_STATEMACHINE_EVENTS_DEPTH)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
          "Warning :  Reached threshold of recurring state machine events");
    dot1sDebugEventsTraceStart(0);
    /* dot1sDebugEventsStart()*/
  }
  if (dot1sDepth > DOT1S_MAX_STATEMACHINE_EVENTS_DEPTH)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DOT1S_COMPONENT_ID,
     "Error: Reached the limit on number of recurring state machine events");
    dot1sDebugEventsTraceStop();
    LOG_ERROR(0);

  }
  dot1sDepth++;
  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_STATE_MACHINE))
  {
    dot1sDebugEventsTrace(p, dot1sEvent, instIndex, dot1sDepth);
  }
  if (dot1sEvent < dot1sPimEvents)
  {
    rc = dot1sPimMachine(dot1sEvent, p, instIndex, bpdu);
  }
  else if (dot1sEvent < dot1sPrsEvents && dot1sEvent > dot1sPimEvents)
  {
    rc = dot1sPrsMachine(dot1sEvent, instIndex);
  }
  else if (dot1sEvent < dot1sPrtEvents && dot1sEvent > dot1sPrsEvents)
  {
    rc = dot1sPrtMachine(dot1sEvent, p, instIndex);
  }
  else if (dot1sEvent < dot1sPrxEvents && dot1sEvent > dot1sPrtEvents)
  {
    rc = dot1sPrxMachine(dot1sEvent, p, bpdu);
  }
  else if (dot1sEvent < dot1sPstEvents && dot1sEvent > dot1sPrxEvents)
  {
    rc = dot1sPstMachine(dot1sEvent, p, instIndex);
  }
  else if (dot1sEvent < dot1sTcmEvents && dot1sEvent > dot1sPstEvents)
  {
    rc = dot1sTcmMachine(dot1sEvent, p, instIndex);
  }
  else if (dot1sEvent < dot1sPpmEvents && dot1sEvent > dot1sTcmEvents)
  {
    rc = dot1sPpmMachine(dot1sEvent, p);
  }
  else if (dot1sEvent < dot1sPtxEvents && dot1sEvent > dot1sPpmEvents)
  {
    rc = dot1sPtxMachine(dot1sEvent, p);
  }
  else if (dot1sEvent < dot1sBdmEvents && dot1sEvent > dot1sPtxEvents)
  {
    rc = dot1sBdmMachine(dot1sEvent, p);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "dot1s Event out of range %d \n", dot1sEvent);
    dot1sDepth--;
    return L7_FAILURE;
  }
  dot1sDepth--;
  return rc;
}
/***************************************************************************/
/*******************************PRS Support Routines************************/
/***************************************************************************/

/*********************************************************************
* @purpose  Clears the rcvdMsg param for all CIST and all MSTIs, for
*           all ports
*
* @param
*
* @returns  L7_SUCCESS  on a successful clearing of the rcvdMsg Parm
* @returns  L7_FAILURE  on any other failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sRcvdMsgsClearAll()
{
  /*get the port records for all ports and for each ST instance set the
  rcvdMsg to L7_FALSE*/

  L7_uint32 j;
  DOT1S_PORT_COMMON_t *dot1sIntf;

  dot1sIntf = dot1sIntfFindFirst();
  while (dot1sIntf != L7_NULLPTR)
  {
    dot1sIntf->portInstInfo[DOT1S_CIST_INDEX].rcvdMsg = L7_FALSE;
    /*now loop through all the MSTIs*/
    for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
        dot1sIntf->portInstInfo[j].rcvdMsg = L7_FALSE;
    }
    dot1sIntf = dot1sIntfFindNext(dot1sIntf->portNum);
  }/* while*/
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the rcvdSTP or rcvdRSTP to TRUE depending on the
*           version of the received BPDU
*
* @param    p         @b{(input)} pointer to port entry
* @param    bufHandle @b{(input)} handle to network buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBPDUVersionUpdate(DOT1S_PORT_COMMON_t *p,
                               DOT1S_MSTP_ENCAPS_t *bpdu)
{
  DOT1S_BRIDGE_t *bridge;

  /*check if rcvd bpdu is a config or tcn bpdu*/
  if (dot1sIsConfigBPDU(bpdu) == L7_TRUE ||
      dot1sIsTcnBPDU(bpdu) == L7_TRUE)
  {
    /*set the rcvdSTP to true and set rcvdrst to false*/
    p->rcvdSTP = L7_TRUE;
    p->rcvdRSTP = L7_FALSE;
    p->rcvdMSTP = L7_FALSE;

  }

  bridge = dot1sBridgeParmsFind();

  /*check if the rcvd bpdu was a rstp bpdu and the force version parameter
  is set to a value greater or equal to 2*/
  if ((dot1sIsRstBPDU(bpdu) == L7_TRUE ||
       dot1sIsMstBPDU(bpdu) == L7_TRUE) &&
      bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W)
  {
    p->rcvdRSTP = L7_TRUE;
    p->rcvdSTP = L7_FALSE;
    if (dot1sIsMstBPDU(bpdu) == L7_TRUE)
      p->rcvdMSTP = L7_TRUE;
    else
      p->rcvdMSTP = L7_FALSE;

  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if the received BPDU is a config BPDU
*
* @param    bufHandle @b{(input)} Handle for the network buffer
*
* @returns  L7_TRUE  config BPDU
* @returns  L7_FALSE not a config BPDU
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sIsConfigBPDU(DOT1S_MSTP_ENCAPS_t *bpdu)
{
  L7_BOOL         rc = L7_FALSE;

  /*memcpy(&protocolIdentifier, &bpdu->protocolIdentifier, sizeof(L7_ushort16));*/

  /*IEEE 802.1s 14.4 (a)*/
  if (bpdu->protocolIdentifier == 0 &&
      (bpdu->bdpuType == 0) && /*bpdu type should be zero*/
      bpdu->packetLength >= (35+3))
  {
    rc = L7_TRUE;
  }
  return rc;
}
/*********************************************************************
* @purpose  Checks if the received BPDU is a TCN BPDU
*
* @param    bufHandle @b{(input)} Handle for the network buffer
*
* @returns  L7_TRUE  TCN BPDU
* @returns  L7_FALSE not a TCN BPDU
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sIsTcnBPDU(DOT1S_MSTP_ENCAPS_t *bpdu)
{
  L7_BOOL         rc = L7_FALSE;

  /*memcpy(&protocolIdentifier, &bpdu->protocolIdentifier, sizeof(L7_ushort16));*/

  /*IEEE 802.1s 14.4 (b)*/
  if (bpdu->protocolIdentifier == 0 &&
      (bpdu->bdpuType == 0x80) && /*bpdu type should be 1000 0000 (0x80)*/
      bpdu->packetLength >= 4)
  {
    rc = L7_TRUE;
  }
  return rc;
}



/*********************************************************************
* @purpose  Checks if the received BPDU is a TCN BPDU
*
* @param    bufHandle @b{(input)} Handle for the network buffer
*
* @returns  L7_TRUE  TCN BPDU
* @returns  L7_FALSE not a TCN BPDU
*
* @comments
*
* @end
*********************************************************************/
dot1s_bpdu_types_t dot1sBPDUTypeGet(DOT1S_MSTP_ENCAPS_t *bpdu)
{
  dot1s_bpdu_types_t bpduType = BPDU_TYPE_INVALID;
  L7_ushort16 moduloVer3Len;
  L7_uint32 integralVer3Len;

  /*memcpy(&protocolIdentifier, &bpdu->protocolIdentifier, sizeof(L7_ushort16));*/

  /*IEEE 802.1s 14.4 (b)*/
  if (bpdu->protocolIdentifier == 0)
  {
    moduloVer3Len = bpdu->ver3Len % (sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t));
    integralVer3Len = bpdu->ver3Len / (sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t));


    if ((bpdu->bdpuType == 0x80) && /*bpdu type should be 1000 0000 (0x80)*/
        bpdu->packetLength >= 4)
    {
      bpduType = BPDU_TYPE_TCN;
    }
    else if ( (bpdu->bdpuType == 0) && /*bpdu type should be zero*/
              bpdu->packetLength >= (35+3))
    {

      bpduType = BPDU_TYPE_CONFIG;
    }
    else if ( (bpdu->protocolVersion == 0x02) &&
              (bpdu->bdpuType == 0x02) &&
              bpdu->packetLength >= (36+3))
    {
      bpduType = BPDU_TYPE_RSTP;

    }
    /*IEEE 802.1s 14.4 (d)*/
    else if ( (bpdu->protocolVersion >= 0x03) &&
              (bpdu->bdpuType == 0x02) && /*bpdu type should be 0000 0010 (0x02)*/
              ((bpdu->packetLength >= (35) && bpdu->packetLength < (103)) ||
               ((bpdu->ver1Len != 0)) ||
               (moduloVer3Len != 0 || integralVer3Len > (64 + 4))))
    {
      bpduType = BPDU_TYPE_RSTP;

    }
    /*IEEE 802.1s 14.4 (e)*/
    else if ( (bpdu->protocolVersion >= 0x03) &&
              (bpdu->bdpuType == 0x02) && /*bpdu type should be 0000 0010 (0x02)*/
              (bpdu->packetLength >= (102)) &&
              (bpdu->ver1Len == 0) &&
              (moduloVer3Len == 0) &&
              (integralVer3Len >= 0) &&
              (integralVer3Len <= 64 + 4)
            )
    {
      bpduType = BPDU_TYPE_MSTP;

    }


  }



  return bpduType;
}

/*********************************************************************
* @purpose  Checks if the received BPDU is a RSTP BPDU
*
* @param    bufHandle @b{(input)} Handle for the network buffer
*
* @returns  L7_TRUE  RSTP BPDU
* @returns  L7_FALSE not a RSTP BPDU
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sIsRstBPDU(DOT1S_MSTP_ENCAPS_t *bpdu)
{
  L7_BOOL         rc = L7_FALSE;
  L7_ushort16       moduloVer3Len;
  L7_ushort16       integralVer3Len;



  /*IEEE 802.1s 14.4 (c)*/
  if (bpdu->protocolIdentifier == 0 &&
      (bpdu->protocolVersion == 0x02) &&
      (bpdu->bdpuType == 0x02) &&
      bpdu->packetLength >= (36+3))
  {
    rc = L7_TRUE;
    return rc;
  }

  moduloVer3Len = bpdu->ver3Len % (sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t));
  integralVer3Len = bpdu->ver3Len / (sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t));

  /*IEEE 802.1s 14.4 (d)*/
  if (bpdu->protocolIdentifier == 0 &&
      (bpdu->protocolVersion >= 0x03) &&
      (bpdu->bdpuType == 0x02) && /*bpdu type should be 0000 0010 (0x02)*/
      ((bpdu->packetLength >= (35) && bpdu->packetLength < (103)) ||
       ((bpdu->ver1Len != 0)) ||
       (moduloVer3Len != 0 || integralVer3Len > (64 + 4))))
  {
    rc = L7_TRUE;
  }
  return rc;
}
/*********************************************************************
* @purpose  Checks if the received BPDU is a MSTP BPDU
*
* @param    bufHandle @b{(input)} Handle for the network buffer
*
* @returns  L7_TRUE  MSTP BPDU
* @returns  L7_FALSE not a MSTP BPDU
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sIsMstBPDU(DOT1S_MSTP_ENCAPS_t *bpdu)
{

  L7_BOOL         rc = L7_FALSE;
  L7_ushort16 moduloVer3Len;
  L7_uint32 integralVer3Len;


  moduloVer3Len = bpdu->ver3Len % (sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t));
  integralVer3Len = bpdu->ver3Len / (sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t));

  /*IEEE 802.1s 14.4 (e)*/
  if ((bpdu->protocolIdentifier == 0x00) &&
      (bpdu->protocolVersion >= 0x03) &&
      (bpdu->bdpuType == 0x02) && /*bpdu type should be 0000 0010 (0x02)*/
      (bpdu->packetLength >= (102)) &&
      (bpdu->ver1Len == 0) &&
      (moduloVer3Len == 0) &&
      (integralVer3Len >= 0) &&
      (integralVer3Len <= 64 + 4)
     )
  {
    rc = L7_TRUE;
  }
  return rc;
}

/*********************************************************************
* @purpose  Checks if the received BPDU conveys a MST Configuration
*           Identifier that matches that held for the bridge
*
* @param    p         @b{(input)} pointer to port entry
* @param    bufHandle @b{(input)} handle to network buffer
*
* @returns  L7_TRUE  Same region
* @returns  L7_FALSE Different Region
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sFromSameRegion(DOT1S_PORT_COMMON_t *p, DOT1S_MSTP_ENCAPS_t   *bpdu)
{
  L7_BOOL         brc = L7_FALSE;
  DOT1S_BRIDGE_t    *bridge;
  L7_BOOL mstBpdu;


  bridge = dot1sBridgeParmsFind();
  mstBpdu = dot1sIsMstBPDU(bpdu);

  if ((p->rcvdRSTP == L7_TRUE) && (bridge->ForceVersion == DOT1S_FORCE_VERSION_DOT1S) &&
      (mstBpdu == L7_TRUE))
  {
    if (memcmp(&bpdu->mstConfigId, &bridge->MstConfigId.formatSelector, DOT1S_BPDU_CONFIG_ID_LEN) == 0)
    {
      brc = L7_TRUE;
    }

  }
  return brc;
}

/*********************************************************************
* @purpose  Sets the rcvdMsg for the CIST, and makes the received CST
*           or CIST message available to the CIST Port Information state
*           machines
*
* @param    p         @b{(input)} pointer to port entry
* @param    bufHandle @b{(input)} handle to network buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sRcvdMsgsSet(DOT1S_PORT_COMMON_t *p, DOT1S_MSTP_ENCAPS_t  *bpdu)
{
  L7_RC_t         rc;
  L7_uint32       numMstiMsgs = 0;
  L7_ushort16       mstid;
  L7_uint32         index, i , instNum, j;
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(p->portNum);

  numMstiMsgs = bpdu->ver3Len/sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t);


  /* how do we make this packet available to the rest of the system?
   * to CIST Port Information State Machine
   * need to capture the CIST message and send that along with rcvdMsg event
   * to the PIM
   * copy the info in the bpdu to the relevant cist msg structure
   */


  if (DOT1S_DEBUG(DOT1S_DEBUG_RCVD_FLAGS, DOT1S_CIST_INDEX))
  {
    DOT1S_DEBUG_MSG(" Rcvd BPDU on Port(%d) CIST message has flags(%x) %s %s %s %s \n",
                    p->portNum,bpdu->cistFlags, (bpdu->cistFlags & DOT1S_LEARNING_FLAG_MASK) ? "Learning" : "",
                    (bpdu->cistFlags & DOT1S_FORWARDING_FLAG_MASK) ? "Forwarding" : "",
                    (bpdu->cistFlags & DOT1S_PROPOSAL_FLAG_MASK) ? "Proposal" : "",
                    (bpdu->cistFlags & DOT1S_AGREEMENT_FLAG_MASK) ? "Agreement" : ""
                   );
  }



  rc = dot1sInstNumFind(DOT1S_CIST_INDEX, &instNum);

  /*Issue the pimRcvdMsg cmd for the cist*/
  /*rc = dot1sIssueCmd(pimRcvdMsg, p->portNum, instNum, (void *)&cistMsg);*/
  if (p->portInstInfo[DOT1S_CIST_INDEX].pimState == PIM_DISABLED)
    rc = dot1sStateMachineClassifier(pimRcvdMsg, p, DOT1S_CIST_INDEX, bpdu, L7_NULLPTR);

  if (p->portInstInfo[DOT1S_CIST_INDEX].updtInfo == L7_FALSE &&
      p->portInstInfo[DOT1S_CIST_INDEX].pimState == PIM_CURRENT)
  {

    if (p->loopInconsistent == L7_TRUE)
    {
      /* Received a BPDU: Clear the loop inconsistent state*/
      dot1sLoopInconsistentSet(p,DOT1S_CIST_INDEX,L7_FALSE);
    }

    p->portInstInfo[DOT1S_CIST_INDEX].rcvdMsg = L7_TRUE;
    /*Send event E8*/
    /*rc = dot1sIssueCmd(pimRcvdXstMsgNotUpdtXstInfo, p->portNum, instNum, (void *)&cistMsg);*/
    rc = dot1sStateMachineClassifier(pimRcvdXstMsgNotUpdtXstInfo, p, DOT1S_CIST_INDEX, bpdu, L7_NULLPTR);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "dot1sRcvdMsgsSet(): updtInfo CIST is L7_TRUE, discarding Msg on port %u\n", p->portNum);
    dot1sPortStats[portIndex].rxDiscards++;
  }

  /*same for the MSTI messages that follow in the BPDU*/
  if (p->rcvdInternal == L7_TRUE)
  {
    /* now we look into the bpdu with the help of ver3Len we start at the
     * end of CIST Remaining Hops and knock of the number of MSTI Configuration msgs
     * ver3Len should be a multiple of 16 bytes
     */

    /*p->portInstInfo[instIndex].inst.msti.mstiMsgTimes */

    if (numMstiMsgs > 4)
    {
      numMstiMsgs = numMstiMsgs - 4;
      if (numMstiMsgs > 0)
      {
        for (i=1;i<=numMstiMsgs;i++)
        {
          /*memcpy(&mstid,&bpdu->mstiMsg[i-1].mstiRegRootId[0], sizeof(L7_ushort16));*/
          mstid = bpdu->mstiMsg[i-1].mstiRegRootId.priInstId;
          mstid = mstid & DOT1S_MSTID_MASK; /*getting rid of the 4 MSbits*/
          /*Check for invalid mstIDs*/
          if((mstid < L7_DOT1S_MSTID_MIN) || (mstid > L7_DOT1S_MSTID_MAX))
          {
            return L7_FAILURE;
          }

          /*convert this mstid into an mst index */
          rc = dot1sInstIndexFind((L7_uint32)mstid, &index);
          if (rc == L7_FAILURE)
          {
            /*mstid does not exists in our region*/
            return L7_FAILURE;
          }
          /*fill out the msg for this msti and issue the pimRcvdMsg cmd*/

          if (DOT1S_DEBUG(DOT1S_DEBUG_RCVD_FLAGS, index))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Rcvd BPDU on Port(%d) MSTI(%d)  message has flags(%x) %s %s %s %s \n",
                          p->portNum,index, bpdu->mstiMsg[i-1].mstiFlags,
                          (bpdu->mstiMsg[i-1].mstiFlags & DOT1S_LEARNING_FLAG_MASK)   ? "Learning" : "",
                          (bpdu->mstiMsg[i-1].mstiFlags & DOT1S_FORWARDING_FLAG_MASK) ? "Forwarding" : "",
                          (bpdu->mstiMsg[i-1].mstiFlags & DOT1S_PROPOSAL_FLAG_MASK)   ? "Proposal" : "",
                          (bpdu->mstiMsg[i-1].mstiFlags & DOT1S_AGREEMENT_FLAG_MASK)  ? "Agreement" : ""
                         );
          }


          /*issue the cmd*/
          /*rc = dot1sIssueCmd(pimRcvdMsg, p->portNum, mstid, (void *)&mstiMsg);*/
          if (p->portInstInfo[index].pimState == PIM_DISABLED)
          {
            rc = dot1sStateMachineClassifier(pimRcvdMsg, p, index, bpdu, L7_NULLPTR);
          }
          else if ((p->portInstInfo[index].updtInfo == L7_FALSE) &&
                   p->portInstInfo[index].pimState == PIM_CURRENT)
          {

            p->portInstInfo[index].rcvdMsg = L7_TRUE;

            /*Send event E8*/
            /*rc = dot1sIssueCmd(pimRcvdXstMsgNotUpdtXstInfo, p->portNum, mstid, (void *)&mstiMsg);*/
            rc = dot1sStateMachineClassifier(pimRcvdXstMsgNotUpdtXstInfo, p, index, bpdu, L7_NULLPTR);
          }
          else
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
                    "dot1sRcvdMsgsSet(): updtInfo MSTI is L7_TRUE, discarding Msg on port %u\n", p->portNum);
          }
        }/*end for (i=1;i<=numMstiMsgs;i++)*/
      }/*end if (numMstiMsgs > 0)*/
    }/*end if (numMstiMsgs > 4)*/
  }/*end if (p->rcvdInternal == L7_TRUE)*/
  else
  {
    /*rcvdInternal is false need to update the roles on the running MSTIs*/
    for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
      {
        p->portInstInfo[j].reselect = L7_TRUE;
        p->portInstInfo[j].selected = L7_FALSE;
        rc = dot1sStateMachineClassifier(prsReselect,p ,j , L7_NULL, L7_NULLPTR);
      }
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Sets the TCN flags
*
* @param    p         @b{(input)} pointer to port entry
* @param    bufHandle @b{(input)} handle to network buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcFlagsSet(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_uint32         i;
  L7_uchar8             bpduFlags;
  L7_uint32 mstID; 

  bpduFlags = p->portInstInfo[instIndex].rcvdBpduFlags;
  if (instIndex == DOT1S_CIST_INDEX)
  {
    if (bpduFlags & DOT1S_TCNACK_FLAG_MASK)
      p->rcvdTcAck = L7_TRUE;

    if (bpduFlags & DOT1S_TC_FLAG_MASK)
    {
      p->portInstInfo[DOT1S_CIST_INDEX].rcvdTc = L7_TRUE;
      if (dot1sInstNumFind(instIndex, &mstID) == L7_SUCCESS)
      {
        trapMgrReceivedTopologyChange(mstID,p->portNum);
      }
      if (p->rcvdInternal == L7_FALSE)
      {
        for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
        {
          if (dot1sInstanceMap[i].inUse == L7_TRUE)
          {
            p->portInstInfo[i].rcvdTc = L7_TRUE;
            if (DOT1S_DEBUG(DOT1S_DEBUG_RCVD_TC,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Port (%d) inst(%d)"
                           "Rcvd TC \n",__FUNCTION__, p->portNum, instIndex);
            }
          }
        }
      }


    }
  }
  else
  {
    if (bpduFlags & DOT1S_TC_FLAG_MASK)
    {
      p->portInstInfo[instIndex].rcvdTc = L7_TRUE;
      if (dot1sInstNumFind(instIndex, &mstID) == L7_SUCCESS)
      {
        trapMgrReceivedTopologyChange(mstID,p->portNum);
      }
      if (DOT1S_DEBUG(DOT1S_DEBUG_RCVD_TC,instIndex))
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Port (%d) inst(%d)"
                     "Rcvd TC \n",__FUNCTION__, p->portNum, instIndex);
      }
    }
  }

  return L7_SUCCESS;
}
/**************************************************************************
 *******************************PRX State Machine Routines*****************
 **************************************************************************
 */
/*********************************************************************
* @purpose  Actions to be performed in the PRX state DISACRD
*
* @param    p         @b{(input)} pointer to port entry
* @param    bufHandle @b{(input)} handle to network buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrxDiscardAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_FAILURE;
  DOT1S_BRIDGE_t   *bridge;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
    return L7_FAILURE;

  p->rcvdBpdu = L7_FALSE;
  p->rcvdRSTP = L7_FALSE;
  p->rcvdSTP  = L7_FALSE;
  p->rcvdMSTP = L7_FALSE;

  rc = dot1sRcvdMsgsClearAll();

  p->edgeDelayWhile = bridge->MigrateTime;

  p->prxState = PRX_DISCARD;

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PRX state RECEIVE
*
* @param    p         @b{(input)} pointer to port entry
* @param    bufHandle @b{(input)} handle to network buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrxReceiveAction(DOT1S_PORT_COMMON_t *p, DOT1S_MSTP_ENCAPS_t   *bpdu)
{
  L7_RC_t rc = L7_FAILURE;
  DOT1S_BRIDGE_t  *bridge;
  dot1s_bpdu_types_t bpduType;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
    return L7_FAILURE;

  bpduType = dot1sBPDUTypeGet(bpdu);
  if (bpduType == BPDU_TYPE_INVALID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Invalid BPDU received on port (%d) \n", p->portNum);
    return rc;

  }

  p->operEdge = L7_FALSE;
  if (p->adminEdge && bridge->bpduGuard)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1S_COMPONENT_ID,
      "Received BPDU on an edge port(%d), setting the port to discarding", p->portNum);
    p->bpduGuardEffect = L7_TRUE;
    L7_INTF_SETMASKBIT(portCommonChangeMask, p->portNum);
  }
  else
  {
    rc = dot1sBPDUVersionUpdate(p,bpdu);

    p->prevRcvdInternal = p->rcvdInternal; /* 28317 */
    p->rcvdInternal = dot1sFromSameRegion(p,bpdu);

    if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_RCVD_INTERNAL))
    {
      DOT1S_DEBUG_MSG("%s: Port %d BPDU received from %s to region (prev %s) \n",
                      __FUNCTION__, p->portNum, p->rcvdInternal ? "Internal" : "External",
                      p->prevRcvdInternal ? "Internal" : "External");
    }
    /* Note earlier we used to ignore the BPDU when Mdelay is runninng.
       But that is no longer required per IEEE-802.1Q-Rev  */
    rc = dot1sRcvdMsgsSet(p,bpdu);

    /*rc = dot1sTcFlagsSet(p,bpdu);*/

    p->operEdge = L7_FALSE;

    p->edgeDelayWhile = bridge->MigrateTime;
  }

  p->rcvdBpdu = L7_FALSE;
  p->prxState = PRX_RECEIVE;

  return rc;
}

/*********************************************************************
* @purpose  This routine claculates the next state fo the PRX state machine
*           and executes the action for that next state
*
* @param    dot1sEvent@b{(input)} dot1s event number
* @param    p         @b{(input)} pointer to port entry
* @param    bufHandle @b{(input)} handle to network buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrxMachine(L7_uint32 dot1sEvent,
                        DOT1S_PORT_COMMON_t *p,
                        DOT1S_MSTP_ENCAPS_t   *bpdu)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_prx_states_t  nextState;
  L7_uint32       normalizedEvent;
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(p->portNum);




  normalizedEvent = dot1sEvent - dot1sPrtEvents - 1;

  if (p->prxState >= PRX_STATES)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Invalid PRX state: Normalized "
               "event (%d) curr state %d \n", normalizedEvent, p->prxState);
    return L7_FAILURE;
  }

  nextState = prxStateTable[normalizedEvent][p->prxState];

  switch (nextState)
  {
    case PRX_DISCARD:
      rc = dot1sPrxDiscardAction(p);

      if (dot1sEvent != prxBegin)
      {
        /* Increment the discard count only if the event is a received BPDU */
        dot1sPortStats[portIndex].rxDiscards++;
      }
      break;
    case PRX_RECEIVE:
      if (bpdu ==L7_NULLPTR)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"BPDU cannot be NULL \n");
        LOG_ERROR(0);
      }

      dot1sPrxReceiveAction(p, bpdu);
      rc = dot1sPrxGenerateEvents(p);
      break;
    default:
      break;
  }/*end switch (nextState)*/

  if (bpdu != L7_NULL)
  {
    /*increment port stats for rx */
    if (L7_TRUE == dot1sIsConfigBPDU(bpdu))
      dot1sPortStats[portIndex].stpRx++;
    else if (L7_TRUE == dot1sIsRstBPDU(bpdu))
      dot1sPortStats[portIndex].rstpRx++;
    else if (L7_TRUE == dot1sIsMstBPDU(bpdu))
      dot1sPortStats[portIndex].mstpRx++;
    else
      dot1sPortStats[portIndex].rxDiscards++;
  }
  return rc;
}

/*********************************************************************
* @purpose  Events that the receive machine needs to generate and propagate
*
* @param    p         @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrxGenerateEvents(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t   rc = L7_FAILURE;
  DOT1S_BRIDGE_t  *bridge;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

  bridge = dot1sBridgeParmsFind();

  if (p->bpduGuardEffect == L7_TRUE)
  {
    nimGetIntfName(p->portNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
            "Received BPDU on Edge port Disabling interface %s", ifName);
    (void)nimSetIntfAdminState(p->portNum, L7_DIAG_DISABLE);
    /* Since we are setting the port to disable don't need to process
       any more events
    */
    return L7_SUCCESS;
  }


  if (p->rcvdRSTP == L7_TRUE &&
      p->sendRSTP == L7_FALSE &&
      p->ppmState == PPM_SENSING &&
      bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W )
  {
    /*Send Event E59*/
    rc = dot1sStateMachineClassifier(ppmRstpVersionNotSendRstpRcvdSTP, p,
                                     L7_NULL, L7_NULL, L7_NULLPTR);
  }

  if (p->rcvdSTP == L7_TRUE &&
      p->sendRSTP == L7_TRUE &&
      p->ppmState == PPM_SENSING )
  {
    /*Send Event E58*/
    rc = dot1sStateMachineClassifier(ppmSendRSTPRcvdStp, p, L7_NULL,
                                     L7_NULL, L7_NULLPTR);
  }

  /*Send event E70 Invoking the BDM machine*/
  generateEventsForBDM(p);

  return rc;
}

/**************************************************************************
 *******************************PTX State Machine Routines*****************
 **************************************************************************
 */

/*********************************************************************
* @purpose  Actions to be performed in the PTX state INIT
*
* @param    p         @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPtxInitAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_FAILURE;

  p->newInfoCist = L7_FALSE;
  p->newInfoMist = L7_FALSE;

  /* This variable is not really used by this state machine and hence the init does not belong here
     but this is the only per port state machine that has an init function. */
  p->loopInconsistent = L7_FALSE;

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }




  p->helloWhen = 0;
  p->txCount = 0;

  p->ptxState = PTX_INIT;
  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PTX state PERIODIC
*
* @param    p         @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPtxPeriodicAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_BOOL   cistRootPort = L7_FALSE;
  L7_BOOL   cistDesignatedPort = L7_FALSE;
  L7_BOOL   mstiRootPort = L7_FALSE;
  L7_BOOL   mstiDesignatedPort = L7_FALSE;
  L7_uint32 j,index = 0;

  if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_ROOT)
    cistRootPort = L7_TRUE;
  if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_DESIGNATED)
    cistDesignatedPort = L7_TRUE;

  for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].role == ROLE_ROOT)
      {
        mstiRootPort = L7_TRUE;
        index = j;
        break;
      }
  }
  for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].role == ROLE_DESIGNATED)
      {
        mstiDesignatedPort = L7_TRUE;
        /*index = j;*/
        break;
      }
  }

  if (p->newInfoCist ||
      ((cistDesignatedPort == L7_TRUE) ||
       ((cistRootPort == L7_TRUE) &&
        (p->portInstInfo[DOT1S_CIST_INDEX].tcWhile != 0))))
    p->newInfoCist = L7_TRUE;
  else
    p->newInfoCist = L7_FALSE;

  if (p->newInfoMist ||
      ((mstiDesignatedPort == L7_TRUE) ||
       ((mstiRootPort == L7_TRUE) &&
        (p->portInstInfo[index].tcWhile != 0))))
    p->newInfoMist = L7_TRUE;
  else
    p->newInfoMist = L7_FALSE;


  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }
  p->txCount = 0;

  /*p->helloWhen = dot1sInstance->cist.CistBridgeTimes.bridgeHelloTime;*/
  /*p->helloWhen = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.helloTime; */

  p->ptxState = PTX_PERIODIC;

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PTX state CONFIG
*
* @param    p         @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPtxConfigAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_FAILURE;

  p->newInfoCist = L7_FALSE;
  p->newInfoMist = L7_FALSE;

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }
  rc = dot1sConfigTx(p);

  p->txCount += 1;
  p->tcAck = L7_FALSE;

  p->ptxState = PTX_CONFIG;

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PTX state TCN
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPtxTcnAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_FAILURE;

  p->newInfoCist = L7_FALSE;
  p->newInfoMist = L7_FALSE;
  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }

  rc = dot1sTcnTx(p);

  p->txCount += 1;

  p->ptxState = PTX_TCN;

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PTX state RSTP
*
* @param    p          @b{(input)} pointer to port entry

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPtxRstpAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_FAILURE;

  p->newInfoCist = L7_FALSE;
  p->newInfoMist = L7_FALSE;

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }
  rc = dot1sMstpTx(p);

  /* If this port is an alternate responding to proposals
     We need to stop sending the PDU now.
  */
  dot1sNeedForBpduTxStopCheck(p);
  p->txCount += 1;
  /*SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s:Port (%d) setting tcAck to false \n", __FUNCTION__,p->portNum);*/
  /*p->tcAck = L7_FALSE;  To fix an ANVL test case */

  p->ptxState = PTX_RSTP;

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PTX state IDLE
*
* @param    p          @b{(input)} pointer to port entry

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPtxIdleAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_SUCCESS;

  p->ptxState = PTX_IDLE;
  p->helloWhen = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.helloTime;

  return rc;
}
/*********************************************************************
* @purpose  This routine claculates the next state fo the PTX state machine
*           and executes the action for that next state
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPtxMachine(L7_uint32 dot1sEvent, DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_ptx_states_t  nextState;
  L7_uint32       normalizedEvent;

  normalizedEvent = dot1sEvent - dot1sPpmEvents - 1;

  nextState = ptxStateTable[normalizedEvent][p->ptxState];

  switch (nextState)
  {
    case PTX_INIT:
      dot1sPtxInitAction(p);
      /*UCT to the Idle State*/
      dot1sPtxIdleAction(p);
      rc = dot1sPtxGenerateEvents(p);
      break;
    case PTX_PERIODIC:
      rc = dot1sPtxPeriodicAction(p);
      /*UCT to the Idle State*/
      rc = dot1sPtxIdleAction(p);
      rc = dot1sPtxGenerateEvents(p);
      break;
    case PTX_IDLE:
      rc = dot1sPtxIdleAction(p);
      break;
    case PTX_RSTP:
      rc = dot1sPtxRstpAction(p);
      /*UCT to the Idle State*/
      rc = dot1sPtxIdleAction(p);
      break;
    case PTX_TCN:
      rc = dot1sPtxTcnAction(p);
      /*UCT to the Idle State*/
      rc = dot1sPtxIdleAction(p);
      break;
    case PTX_CONFIG:
      rc = dot1sPtxConfigAction(p);
      /*UCT to the Idle State*/
      rc = dot1sPtxIdleAction(p);
      break;
    default:
      break;
  }/*end switch (nextState)*/
  return rc;
}
/*********************************************************************
* @purpose  Events that the receive machine needs to generate and propagate
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPtxGenerateEvents(DOT1S_PORT_COMMON_t *p)
{
  L7_uint32     j;
  L7_RC_t       rc = L7_SUCCESS;
  DOT1S_BRIDGE_t  *bridge;
  L7_BOOL       cistRootPort = L7_FALSE;
  L7_BOOL       cistDesignatedPort = L7_FALSE;
  L7_BOOL       mstiRootPort = L7_FALSE;
  L7_BOOL       mstiDesignatedPort = L7_FALSE;
  L7_BOOL       allSelected = L7_TRUE;
  L7_BOOL       allUpdtInfo = L7_FALSE;

  bridge = dot1sBridgeParmsFind();
  /***************************
   * generate events for ptx
   ***************************
   */
  /*all events are qualified with &&selected && !updtInfo*/
  for (j = 0; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].selected == L7_FALSE)
      {
        allSelected = L7_FALSE;
        break;
      }
  }
  for (j = 0; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].updtInfo == L7_TRUE)
      {
        allUpdtInfo = L7_TRUE;
        break;
      }
  }

  if (allSelected == L7_TRUE &&
      allUpdtInfo == L7_FALSE)
  {
    if (p->helloWhen == 0)
    {
      /*Send Event E65*/
      rc = dot1sStateMachineClassifier(ptxHelloWhenZero, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }
    if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_ROOT)
      cistRootPort = L7_TRUE;
    if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_DESIGNATED)
      cistDesignatedPort = L7_TRUE;

    for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
        if (p->portInstInfo[j].role == ROLE_ROOT)
        {
          mstiRootPort = L7_TRUE;
          break;
        }
    }
    for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
        if (p->portInstInfo[j].role == ROLE_DESIGNATED)
        {
          mstiDesignatedPort = L7_TRUE;
          break;
        }
    }

    if (p->sendRSTP == L7_TRUE &&
        ((p->newInfoCist == L7_TRUE &&
          (cistRootPort == L7_TRUE || cistDesignatedPort == L7_TRUE)) ||
         (p->newInfoMist == L7_TRUE && (mstiRootPort == L7_TRUE || mstiDesignatedPort == L7_TRUE))) &&
        (p->txCount < bridge->TxHoldCount) && (p->helloWhen != 0)
       )
    {
      /*Send Event E66*/
      /*rc = dot1sIssueCmd(ptxSendRstp, p->portNum, L7_NULL, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(ptxSendRstp, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }
    if (p->sendRSTP == L7_FALSE &&
        p->newInfoCist == L7_TRUE &&
        cistRootPort == L7_TRUE &&
        p->txCount < bridge->TxHoldCount &&
        p->helloWhen != 0
       )
    {
      /*Send Event E67*/
      /*rc = dot1sIssueCmd(ptxSendTcn, p->portNum, L7_NULL, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(ptxSendTcn, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }
    else if (p->sendRSTP == L7_FALSE &&
             p->newInfoCist == L7_TRUE &&
             cistDesignatedPort == L7_TRUE &&
             p->txCount < bridge->TxHoldCount &&
             p->helloWhen != 0
            )
    {
      /*Send Event E68*/
      /*rc = dot1sIssueCmd(ptxSendConfig, p->portNum, L7_NULL, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(ptxSendConfig, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }

  }/*end if (allSelected == L7_TRUE &&....*/
  return rc;
}

/*********************************************************************
* @purpose  Check if the port has to keep on transmitting PDUs.
*           If not tell the helpers
*
* @param    p         @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sNeedForBpduTxStopCheck(DOT1S_PORT_COMMON_t *p)
{
  L7_BOOL   cistRootPort = L7_FALSE;
  L7_BOOL   cistDesignatedPort = L7_FALSE;
  L7_BOOL   mstiRootPort = L7_FALSE;
  L7_BOOL   mstiDesignatedPort = L7_FALSE;
  L7_uint32 j,index = 0;

  DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_HELPER_DETAIL,
                  "%s(%d) port %d \n", __FUNCTION__, __LINE__, p->portNum);
  p->pduTxStopCheck = L7_FALSE;

  if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_ROOT)
    cistRootPort = L7_TRUE;
  if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_DESIGNATED)
    cistDesignatedPort = L7_TRUE;

  /* Do we need to keep transmitting PDU for CIST? */
  if ((cistDesignatedPort == L7_TRUE) ||
       ((cistRootPort == L7_TRUE) &&
        (p->portInstInfo[DOT1S_CIST_INDEX].tcWhile != 0)))
  {
    return L7_SUCCESS;
  }


  for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].role == ROLE_DESIGNATED)
      {
        mstiDesignatedPort = L7_TRUE;
        /*index = j;*/
        break;
      }
  }

  for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].role == ROLE_ROOT)
      {
        mstiRootPort = L7_TRUE;
        index = j;
        break;
      }
  }

  /* Do we need to keep transmitting PDU for any instance? */
  if ((mstiDesignatedPort == L7_TRUE) ||
       ((mstiRootPort == L7_TRUE) &&
        (p->portInstInfo[index].tcWhile != 0)))
  {
    return L7_SUCCESS;
  }

  if (dot1sHelperFuncTable.dot1sNotifyHelperPduStop)
  {
    return dot1sHelperFuncTable.dot1sNotifyHelperPduStop(p->portNum);
  }
  /* We do not have any reason to transmit pdu's stop the helper if present*/
  return L7_SUCCESS;
}

/**************************************************************************
 *******************************PIM Support Routines***********************
 **************************************************************************
*/
/*********************************************************************
* @purpose  Deciphers the received CIST message
*
* @param    p          @b{(input)} pointer to port entry
* @param    cistMsg    @b{(input)} pointer to the rcvd CIST message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
DOT1S_RCVD_INFO_t dot1sInfoCistRcv(DOT1S_PORT_COMMON_t *p, DOT1S_MSTP_ENCAPS_t  *bpdu)
{
  DOT1S_RCVD_INFO_t   info = OtherInfo;
  DOT1S_PRI_COMP_t  comparison;
  L7_BOOL     isDesignated = L7_TRUE;
  L7_uint32          flagRole = 0, i;
  DOT1S_CIST_TIMES_t *msgTimes;
  DOT1S_CIST_PRIORITY_VECTOR_t *msgPrio;
  /* Check for TCN */
  if (dot1sIsTcnBPDU(bpdu) == L7_TRUE)
  {
    p->rcvdTcn = L7_TRUE;
    /* Note : we are starting from 1 as cist will UCT to notified_TC state */
    for (i = 1;i<=L7_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
      if (dot1sInstanceMap[i].inUse == L7_TRUE)
        p->portInstInfo[i].rcvdTc = L7_TRUE;
    }
    return info;
  }


  p->portInstInfo[DOT1S_CIST_INDEX].rcvdBpduFlags = bpdu->cistFlags;

  /* IEEE 802.1Q-2003 says in sec 14.6 the for STP BPDUS
     rest of the flags should be ignored on receipt. Clearing the flags
  */
  if (dot1sIsConfigBPDU(bpdu) == L7_TRUE)
  {
    p->portInstInfo[DOT1S_CIST_INDEX].rcvdBpduFlags &= DOT1S_TCNACK_TC_FLAG_MASK ;
  }

  msgTimes = &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgTimes;
  msgPrio = &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority;



  /* Copy message values into priority vector and msg times vector */
  msgTimes->fwdDelay = bpdu->fwdDelay;
  msgTimes->helloTime = bpdu->helloTime;
  msgTimes->maxAge = bpdu->maxAge;
  msgTimes->msgAge = bpdu->msgAge;
  msgTimes->remHops = bpdu->cistRemHops;


  msgPrio->dsgBridgeId.priInstId = bpdu->cistBridgeId.priInstId;
  memcpy(msgPrio->dsgBridgeId.macAddr,
         bpdu->cistBridgeId.macAddr,
         L7_MAC_ADDR_LEN);

  msgPrio->dsgPortId = bpdu->cistPortId;
  msgPrio->extRootPathCost = bpdu->cistExtRootPathCost;
  msgPrio->intRootPathCost = bpdu->cistIntRootPathCost;

  if (p->rcvdInternal == L7_FALSE) /*13.10*/
  {
    msgPrio->intRootPathCost = 0;
  }

  msgPrio->regRootId.priInstId = bpdu->cistRegRootId.priInstId;
  memcpy(msgPrio->regRootId.macAddr,
         bpdu->cistRegRootId.macAddr, L7_MAC_ADDR_LEN);

  msgPrio->rootId.priInstId = bpdu->cistRootId.priInstId;
  memcpy(msgPrio->rootId.macAddr,
         bpdu->cistRootId.macAddr, L7_MAC_ADDR_LEN);

  msgPrio->rxPortId = p->portInstInfo[DOT1S_CIST_INDEX].portId;


  if (dot1sIsConfigBPDU(bpdu) == L7_TRUE || dot1sIsRstBPDU(bpdu) == L7_TRUE)  /*13.10*/
  {
    msgPrio->intRootPathCost = 0;

    msgPrio->dsgBridgeId.priInstId = bpdu->cistRegRootId.priInstId;
    memcpy(msgPrio->dsgBridgeId.macAddr,
           bpdu->cistRegRootId.macAddr, L7_MAC_ADDR_LEN);
    msgTimes->remHops = dot1sInstance->cist.CistBridgeTimes.maxHops;

  }

  /*Issue the pimRcvdMsg cmd for the cist*/
  /************************************************************************************/


  flagRole = (L7_uint32) ((p->portInstInfo[DOT1S_CIST_INDEX].rcvdBpduFlags & DOT1S_PORT_ROLE_FLAG_MASK) >> 2);
  if (L7_TRUE == p->rcvdRSTP)
  {
    /*find out the role of this RSTP or MSTP BPDU*/
    if (flagRole  != DOT1S_PORT_ROLE_DESIG)
      isDesignated = L7_FALSE;
  }

  /* initially find out whether the received msg is the same, better or worse than
  * what we have as the cist port priority
  */
  comparison = dot1sCistPriVecCompare(msgPrio, &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority);


  if ((flagRole  == DOT1S_PORT_ROLE_ROOT) ||
      (flagRole == DOT1S_PORT_ROLE_ALT_BCK))
  {
    if (comparison == SAME || comparison == WORSE)
    {
      info = InferiorRootAlternateInfo;
    }
    else
    {
      /*Log message probably dispute */
    }
  }
  else if (isDesignated == L7_TRUE)
  {
    /* Accomodate for the fact that we have already decremented remHops with infoInternal = true */
    if (comparison == SAME)
    {
      if ((msgTimes->fwdDelay != p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.fwdDelay) ||
          (msgTimes->maxAge != p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.maxAge) ||
          (msgTimes->msgAge != p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.msgAge)  ||
          (msgTimes->helloTime != p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.helloTime) ||
          (msgTimes->remHops != p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.remHops)
         )
      {

        info = SuperiorDesignatedInfo;
      }
      else
      {
        info = RepeatedDesignatedInfo;
      }

    }

    else if ((comparison == BETTER) ||
             ((memcmp(msgPrio->dsgBridgeId.macAddr,
                      p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority.dsgBridgeId.macAddr,
                      L7_MAC_ADDR_LEN) == 0) &&
              ((msgPrio->dsgPortId & DOT1S_PORTNUM_MASK) ==
               (p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority.dsgPortId & DOT1S_PORTNUM_MASK)))
            )
    {

      info = SuperiorDesignatedInfo;

    }
    else if (comparison == WORSE)
    {
      info = InferiorDesignatedInfo;
    }

  }/* End if (isDesignated == L7_TRUE*/

  return info;

}

/*********************************************************************
* @purpose  Based on the CIST message received the the mstiMasterd param
*           for this port on all MSTIs is cleared
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sMasteredCistRecord(DOT1S_PORT_COMMON_t *p)
{
  L7_uint32 i;

  if (p->rcvdInternal == L7_FALSE)
  {
    for (i=1; i<= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
      if (dot1sInstanceMap[i].inUse == L7_TRUE)
        p->portInstInfo[i].inst.msti.mstiMastered = L7_FALSE;
    }
  }
  return L7_SUCCESS;
}

/*   */
/*********************************************************************
* @purpose  For a given inst number return the index in the BPDU
*
* @param    instNum    @b{(input)} instance Number of the instance(not index)
* @param    bpdu       @b{(input)} pointer to BPDU
* @param    bpduIndex  @b{(output)} index of corresponding message inside the bpdu
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments returns a enumerated value signifining the nature of the info
*
* @end
*********************************************************************/

L7_RC_t dot1sFindBPDUIndex(L7_uint32 instNum, DOT1S_MSTP_ENCAPS_t   *bpdu, L7_uint32 *bpduIndex)
{
  L7_uint32 i;
  L7_uint32 mstid;
  L7_uint32 numMstiMsgs;

  numMstiMsgs = bpdu->ver3Len/sizeof(DOT1S_BYTE_MSTI_CFG_MSG_t);

  if (numMstiMsgs > 4)
  {
    numMstiMsgs -= 4;
  }


  for (i = 0; i < numMstiMsgs; i++)
  {
    mstid = bpdu->mstiMsg[i].mstiRegRootId.priInstId;
    mstid = mstid & DOT1S_MSTID_MASK; /*getting rid of the 4 MSbits*/
    if (mstid == instNum)
    {
      *bpduIndex = i;
      return L7_SUCCESS;
    }
  }


  return L7_FAILURE;

}


/*********************************************************************
* @purpose  Deciphers the received MSTI message
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    mstiMsg    @b{(input)} pointer to msti message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments returns a enumerated value signifining the nature of the info
*
* @end
*********************************************************************/
DOT1S_RCVD_INFO_t dot1sInfoMstiRcv(DOT1S_PORT_COMMON_t *p,
                                   L7_uint32 instIndex,
                                   DOT1S_MSTP_ENCAPS_t  *bpdu)
{

  DOT1S_RCVD_INFO_t info = OtherInfo;
  DOT1S_PRI_COMP_t  comparison;
  L7_BOOL     isDesignated = L7_TRUE;
  L7_uint32         flagRole = 0;
  DOT1S_MSTI_PRIORITY_VECTOR_t *msgPrio;
  DOT1S_MSTI_TIMES_t *msgTimes;
  L7_ushort16     bridgePri;
  L7_ushort16     portPri;
  L7_ushort16       mstid;
  L7_uint32 bpduIndex; /* Index of the instance in the bpdu */
  L7_uint32            instNum;
  L7_RC_t rc;


  if (p->rcvdInternal == L7_FALSE)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"rcvd internal is false return other info  \n");
    return info;
  }
  if (dot1sInstNumFind(instIndex, &instNum) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Cannot find inst number for Index %d \n", instIndex);
    return info;
  }


  /*convert this mstid into an mst index */
  rc = dot1sFindBPDUIndex(instNum,bpdu, &bpduIndex);
  if (rc == L7_FAILURE)
  {
    /*mstid does not exists in our region*/
    return info;
  }



  /***********************************************************************************************************/
  /* use InstIndex to access the port common structure and bpduIndex to acces within bpdu */
  p->portInstInfo[instIndex].rcvdBpduFlags = bpdu->mstiMsg[bpduIndex].mstiFlags;
  /*same for the MSTI messages that follow in the BPDU*/
  msgTimes = &p->portInstInfo[instIndex].inst.msti.mstiMsgTimes;
  msgPrio = &p->portInstInfo[instIndex].inst.msti.mstiMsgPriority;


  /* now we look into the bpdu with the help of ver3Len we start at the
   * end of CIST Remaining Hops and knock of the number of MSTI Configuration msgs
   * ver3Len should be a multiple of 16 bytes
   */

  /*p->portInstInfo[instIndex].inst.msti.mstiMsgTimes */


  mstid = bpdu->mstiMsg[bpduIndex].mstiRegRootId.priInstId;
  mstid = mstid & DOT1S_MSTID_MASK; /*getting rid of the 4 MSbits*/
  /*convert this mstid into an mst index */

  /*confirm that this index is adjusted SKTBD*/
  /*fill out the msg for this msti and issue the pimRcvdMsg cmd*/
  /*      mstiMsg.cistExtRootPathCost = bpdu->cistExtRootPathCost;*/


  /*       mstiMsg.cistRegRootId.priInstId = bpdu->cistRegRootId.priInstId;                        */
  /*       memcpy(mstiMsg.cistRegRootId.macAddr, bpdu->cistRegRootId.macAddr, L7_MAC_ADDR_LEN);    */
  /*       mstiMsg.cistRootId.priInstId = bpdu->cistRootId.priInstId;                              */
  /*       memcpy(mstiMsg.cistRootId.macAddr, bpdu->cistRootId.macAddr, L7_MAC_ADDR_LEN);          */


  /* use the bridge priority in the rcvd msg, the cist bridge id and the MSTID
   * to construct the msti designated bridge id
   */


  memcpy(msgPrio->dsgBridgeId.macAddr,
         bpdu->cistBridgeId.macAddr, L7_MAC_ADDR_LEN);
  /*get the bridge priority from the rcvd bpdu*/
  bridgePri = (L7_ushort16)bpdu->mstiMsg[bpduIndex].mstiBridgePriority;
  /*make sure that we look at the correct 4 msbits*/
  bridgePri = ((bridgePri >> 4)<<12);
  msgPrio->dsgBridgeId.priInstId = mstid | bridgePri;

  /*get the port priority from the rcvd msg*/
  portPri = (L7_ushort16)bpdu->mstiMsg[bpduIndex].mstiPortPriority;
  portPri = ((portPri >> 4)<<12);
  msgPrio->dsgPortId = (DOT1S_PORTNUM_FROM_PORTID_GET(bpdu->cistPortId)) | portPri;


  msgPrio->intRootPathCost = bpdu->mstiMsg[bpduIndex].mstiIntRootPathCost;


  msgPrio->regRootId.priInstId = bpdu->mstiMsg[bpduIndex].mstiRegRootId.priInstId;

  memcpy(msgPrio->regRootId.macAddr,
         bpdu->mstiMsg[bpduIndex].mstiRegRootId.macAddr,
         L7_MAC_ADDR_LEN);

  msgPrio->rxPortId = p->portInstInfo[instIndex].portId;

  msgTimes->remHops = bpdu->mstiMsg[bpduIndex].mstiRemHops;



  /*************************************************************************************************************/

  flagRole = (L7_uint32)((bpdu->mstiMsg[bpduIndex].mstiFlags & DOT1S_PORT_ROLE_FLAG_MASK) >> 2);
  if (L7_TRUE == p->rcvdRSTP)
  {
    /*find out the role of this RSTP or MSTP BPDU*/
    if (flagRole  != DOT1S_PORT_ROLE_DESIG)
      isDesignated = L7_FALSE;
  }



  comparison = dot1sMstiPriVecCompare(msgPrio, &p->portInstInfo[instIndex].inst.msti.mstiPortPriority);

  /* Return rootInfo if root port role and message priority vector is same or worse than port prio vector*/
  if ((flagRole  == DOT1S_PORT_ROLE_ROOT) ||
      (flagRole == DOT1S_PORT_ROLE_ALT_BCK))
  {
    if (comparison == SAME || comparison == WORSE)
    {
      info = InferiorRootAlternateInfo;
    }
    else
    {
      /* dispute ?? */
    }
  }
  else if (isDesignated == L7_TRUE)
  {

    if (comparison == SAME)
    {
      if (msgTimes->remHops !=
          p->portInstInfo[instIndex].inst.msti.mstiPortTimes.remHops)
      {
        info = SuperiorDesignatedInfo;

      }
      else
      {
        info = RepeatedDesignatedInfo;

      }
    }/*end if (comparison == SAME) */
    else if ((comparison == BETTER) ||
             ((memcmp(msgPrio->dsgBridgeId.macAddr,
                      p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId.macAddr,
                      L7_MAC_ADDR_LEN) == 0) &&
              ((msgPrio->dsgPortId & DOT1S_PORTNUM_MASK) == (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgPortId & DOT1S_PORTNUM_MASK)))
            )
    {
      info = SuperiorDesignatedInfo;
    }
    else if (comparison == WORSE)
    {
      info = InferiorDesignatedInfo;
    }

  }/*end if (isDesignated == L7_TRUE) */
  else
  {
    if (dot1s_enable_debug_detail)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s OtherInfo to be returned, port(%d), instIndex(%d) ", __FUNCTION__,
                    p->portNum, instIndex);
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"roleFlag %d, comparison %d\n", flagRole, comparison);
    }
  }

  return info;

}

/*********************************************************************
* @purpose  Sets or Clears the agreement flag for the CIST
*
* @param    p          @b{(input)} pointer to port entry
* @param    cistMsg    @b{(input)} pointer to CIST message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sAgreementCistRecord(DOT1S_PORT_COMMON_t *p)
{
  L7_BOOL     agreementFlagSet;
  L7_uint32     i;
  DOT1S_BRIDGE_t  *bridge;

  bridge = dot1sBridgeParmsFind();

  if (bridge == L7_NULLPTR)
  {
    return L7_FAILURE;
  }


  if (p->portInstInfo[DOT1S_CIST_INDEX].rcvdBpduFlags & DOT1S_AGREEMENT_FLAG_MASK)
    agreementFlagSet = L7_TRUE;
  else
    agreementFlagSet = L7_FALSE;


  if (agreementFlagSet == L7_TRUE &&
      bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W &&
      pointTopointCheck == L7_TRUE)
  {
    p->portInstInfo[DOT1S_CIST_INDEX].agreed = L7_TRUE;
    p->portInstInfo[DOT1S_CIST_INDEX].proposing = L7_FALSE;
    if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE,DOT1S_CIST_INDEX))
    {
      DOT1S_DEBUG_MSG("%s:Port %d CIST Agreed at %d\n", __FUNCTION__,p->portNum, osapiUpTimeRaw());
    }
  }
  else
  {
    p->portInstInfo[DOT1S_CIST_INDEX].agreed = L7_FALSE;
  }

  if (p->rcvdInternal == L7_FALSE)
  {
    for (i=1;i<=L7_MAX_MULTIPLE_STP_INSTANCES;i++)
    {
      if (dot1sInstanceMap[i].inUse == L7_TRUE)
      {
        p->portInstInfo[i].agreed = p->portInstInfo[DOT1S_CIST_INDEX].agreed;
        p->portInstInfo[i].proposing = p->portInstInfo[DOT1S_CIST_INDEX].proposing;
      }
    }
  }

  return L7_SUCCESS;

}
/*********************************************************************
* @purpose   Sets or Clears the agreement flag for the MSTI
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    mstiMsg    @b{(input)} pointer to msti message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sAgreementMstiRecord(DOT1S_PORT_COMMON_t *p,
                                 L7_uint32 instIndex)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_BOOL     agreementFlagSet;
  DOT1S_CIST_PRIORITY_VECTOR_t *cistMsg;

  if (p->rcvdInternal == L7_FALSE)
  {
    /*MSTI msgs received from bridges external to the MST region are discarded*/
    return L7_SUCCESS;
  }

  cistMsg = &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority;
  if (p->portInstInfo[instIndex].rcvdBpduFlags & DOT1S_AGREEMENT_FLAG_MASK)
  {
    if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE,instIndex))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) instindex(%d) Received agreement \n", __FUNCTION__,
                    p->portNum, instIndex);
    }
    agreementFlagSet = L7_TRUE;
  }
  else
    agreementFlagSet = L7_FALSE;


  if ((agreementFlagSet == L7_TRUE) && (pointTopointCheck == L7_TRUE) &&
      ((dot1sIdEqual(cistMsg->rootId ,
                     p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority.rootId)) &&
       (cistMsg->extRootPathCost == p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority.extRootPathCost) &&
       dot1sIdEqual(cistMsg->regRootId, p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority.regRootId)
      )
     )
  {
    if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE,instIndex))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) instindex(%d) setting agreed to true \n", __FUNCTION__,
                    p->portNum, instIndex);
    }
    p->portInstInfo[instIndex].agreed = L7_TRUE;
    p->portInstInfo[instIndex].proposing = L7_FALSE;
  }
  else
  {
    p->portInstInfo[instIndex].agreed = L7_FALSE;
  }

  rc = L7_SUCCESS;
  return rc;
}

/*********************************************************************
* @purpose  Sets or Clears the proposed flag in the CIST
*
* @param    p          @b{(input)} pointer to port entry
* @param    cistMsg    @b{(input)} pointer to CIST message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
* @end
*********************************************************************/
L7_RC_t dot1sProposalCistRecord(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_BOOL proposalFlagSet;
  L7_uint32 i, roleFlag = 0;
  DOT1S_PORT_ROLE_t portRole = ROLE_DISABLED;


  if (p->portInstInfo[DOT1S_CIST_INDEX].rcvdBpduFlags & DOT1S_PROPOSAL_FLAG_MASK)
    proposalFlagSet = L7_TRUE;
  else
    proposalFlagSet = L7_FALSE;
  roleFlag =(L7_uint32)(p->portInstInfo[DOT1S_CIST_INDEX].rcvdBpduFlags & DOT1S_PORT_ROLE_FLAG_MASK);
  roleFlag = roleFlag >> 2;
  if (roleFlag == DOT1S_PORT_ROLE_DESIG)
  {
    portRole = ROLE_DESIGNATED;
  }
  if (proposalFlagSet == L7_TRUE &&
      /* pointTopointCheck == L7_TRUE && */
      portRole == ROLE_DESIGNATED)
  {
    p->portInstInfo[DOT1S_CIST_INDEX].proposed = L7_TRUE;
    if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE,DOT1S_CIST_INDEX))
    {
      DOT1S_DEBUG_MSG("%s: Port %d CIST Proposed at %d\n",__FUNCTION__, p->portNum, osapiUpTimeRaw());
    }
  }
  /* else
   {
   p->portInstInfo[DOT1S_CIST_INDEX].proposed = L7_FALSE;
   }
  */
  if (p->rcvdInternal == L7_FALSE)
  {
    for (i=1;i<=L7_MAX_MULTIPLE_STP_INSTANCES;i++)
    {
      if (dot1sInstanceMap[i].inUse == L7_TRUE)
        p->portInstInfo[i].proposed = p->portInstInfo[DOT1S_CIST_INDEX].proposed;
    }
  }

  rc = L7_SUCCESS;
  return rc;
}

/*********************************************************************
* @purpose  Sets or Clears the proposed flag in the MSTI
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    mstiMsg    @b{(input)} pointer to msti message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sProposalMstiRecord(DOT1S_PORT_COMMON_t *p,
                                L7_uint32 instIndex)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32     roleFlag = 0;


  roleFlag =(L7_uint32)(p->portInstInfo[instIndex].rcvdBpduFlags & DOT1S_PORT_ROLE_FLAG_MASK);
  roleFlag = roleFlag >> 2;

  if ((p->portInstInfo[instIndex].rcvdBpduFlags & DOT1S_PROPOSAL_FLAG_MASK) &&
      roleFlag == DOT1S_PORT_ROLE_DESIG)
  {
    p->portInstInfo[instIndex].proposed = L7_TRUE;
  }
  /*else
  {
  p->portInstInfo[instIndex].proposed = L7_FALSE;
  } */

  rc = L7_SUCCESS;
  return rc;
}

/*********************************************************************
* @purpose  Calculates the effective age, or remainingHops, to limit
*           the propagation and longevity of received STP information
*           for the CIST
*
* @param    p          @b{(input)} pointer to port entry
* @param    cistMsg    @b{(input)} pointer to CIST message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sRcvdInfoWhileUpdt(DOT1S_PORT_COMMON_t *p,
                               L7_uint32 instIndex)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     msgAge;
  L7_uint32     maxAge;
  L7_uint32     helloTime;
  L7_uint32         remHops;



/*
  Updates rcvdInfoWhile (13.21). The value assigned to rcvdInfoWhile is three times the Hello Time, if
either:
a) Message Age, incremented by 1 second and rounded to the nearest whole second, does not exceed
Max Age and the information was received from a Bridge external to the MST Region (rcvdInternal
FALSE);
or
b) remainingHops, decremented by one, is greater than zero and the information was received from a
Bridge internal to the MST Region (rcvdInternal TRUE);
and is zero otherwise.
The values of Message Age, Max Age, remainingHops, and Hello Time used in these calculations are taken
from the CIST’s portTimes parameter (13.24.13), and are not changed by this procedure.

*/



  msgAge = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.msgAge;
  if ( msgAge % DOT1S_TIMER_UNIT > DOT1S_TIMER_UNIT/2)
    msgAge = msgAge/ DOT1S_TIMER_UNIT + 1;
  else
    msgAge /= DOT1S_TIMER_UNIT;

  maxAge = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.maxAge/ DOT1S_TIMER_UNIT;

  if (instIndex == DOT1S_CIST_INDEX)
  {
    remHops = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.remHops;
  }
  else
    remHops = p->portInstInfo[instIndex].inst.msti.mstiPortTimes.remHops;

  helloTime = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.helloTime;


  p->portInstInfo[instIndex].rcvdInfoWhile = ((p->rcvdInternal == L7_FALSE) && ((msgAge + 1) < maxAge)) ||
                                             ((p->rcvdInternal == L7_TRUE) && (remHops > 1) )
                                             ? 3 * helloTime : 0;


  if (DOT1S_DEBUG(DOT1S_DEBUG_RCVD_INFO, instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Port %d inst %d Rcvd Info While %d \n",
                  p->portNum, instIndex,
                  p->portInstInfo[instIndex].rcvdInfoWhile);

  }


  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Checks if the received CIST priority vector is better or the
*           same as the CIST port priority vector
*
* @param    p          @b{(input)} pointer to port entry
* @param    cistMsg    @b{(input)} pointer to CIST message
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sBetterOrSameInfoCist(DOT1S_PORT_COMMON_t *p,
                                  DOT1S_CIST_PRIORITY_VECTOR_t *cistMsgPrio,
                                  DOT1S_INFO_t newInfoIs)
{
  L7_BOOL       rc = L7_FALSE;
  DOT1S_PRI_COMP_t  comparison;

  /*802.1Q-REV/D5.0 13.26.1 */
  if (newInfoIs != p->portInstInfo[DOT1S_CIST_INDEX].infoIs)
  {
    return L7_FALSE;
  }

  comparison = dot1sCistPriVecCompare(cistMsgPrio, &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority);

  if (comparison == SAME || comparison == BETTER)
  {
    if (dot1sIdEqual(cistMsgPrio->dsgBridgeId,
                     p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority.dsgBridgeId) &&
        (cistMsgPrio->dsgPortId == p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority.dsgPortId)
       )
      rc = L7_TRUE;
    else
      rc = L7_FALSE;
  }
  else
    rc = L7_FALSE;

  return rc;
}
/*********************************************************************
* @purpose  Checks if the received MSTI priority vector is better or the
*           same as the MSTI port priority vector
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    mstiMsg    @b{(input)} pointer to msti message
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sBetterOrSameInfoMsti(DOT1S_PORT_COMMON_t *p,
                                  L7_uint32 instIndex,
                                  DOT1S_MSTI_PRIORITY_VECTOR_t *mstiMsgPrio)
{
  L7_BOOL       rc = L7_FALSE;
  DOT1S_PRI_COMP_t  comparison;

  comparison = dot1sMstiPriVecCompare(mstiMsgPrio, &p->portInstInfo[instIndex].inst.msti.mstiPortPriority);

  if (comparison == SAME || comparison == BETTER)
  {
    if (dot1sIdEqual(mstiMsgPrio->dsgBridgeId,
                     p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId) &&
        (mstiMsgPrio->dsgPortId == p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgPortId)
       )
    {
      rc = L7_TRUE;
    }
    else
      rc = L7_FALSE;
  }
  else
    rc = L7_FALSE;

  return rc;
}

/**************************************************************************
 *******************************PIM State Machine Routines*****************
 **************************************************************************
 */

/*********************************************************************
* @purpose  Actions to be performed in the PIM state DISABLED
*
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimDisabledAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].rcvdMsg = L7_FALSE;
  p->portInstInfo[instIndex].proposing = L7_FALSE;
  p->portInstInfo[instIndex].proposed = L7_FALSE;
  p->portInstInfo[instIndex].agree = L7_FALSE;
  p->portInstInfo[instIndex].agreed = L7_FALSE;

  p->portInstInfo[instIndex].rcvdInfoWhile = 0;

  p->portInstInfo[instIndex].infoIs = INFO_DISABLED;

  /*************************************************/
  /*as per email from ieee 802.1*/
  p->portInstInfo[instIndex].selected = L7_FALSE;
  p->portInstInfo[instIndex].reselect = L7_TRUE;
  /*************************************************/

  p->portInstInfo[instIndex].pimState = PIM_DISABLED;


  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Actions to be performed in the PIM state ENABLED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimEnabledAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;
  /*this state has been removed in draft 15*/

  return rc;
}


/*********************************************************************
* @purpose  Actions to be performed in the PIM state AGED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimAgedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  /* LoopGuard: Set loopInconsistent state if loopGuard enabled and BPDU is aged */
  if (p->loopGuard == L7_TRUE)
  {
    dot1sLoopInconsistentSet(p, instIndex, L7_TRUE);


  }

  p->portInstInfo[instIndex].infoIs = INFO_AGED;

  p->portInstInfo[instIndex].reselect = L7_TRUE;
  p->portInstInfo[instIndex].selected = L7_FALSE;

  p->portInstInfo[instIndex].pimState = PIM_AGED;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PIM state UPDATE
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimUpdateAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_BOOL       betterorsame = L7_FALSE;

  p->portInstInfo[instIndex].proposing = L7_FALSE;
  p->portInstInfo[instIndex].proposed = L7_FALSE;
  p->portInstInfo[instIndex].synced = L7_FALSE;

  if (DOT1S_DEBUG(DOT1S_DEBUG_PIM,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Enter \n",__FUNCTION__);
  }


  /*p->portInstInfo[instIndex].sync = p->portInstInfo[instIndex].changedMaster;*/
  if (instIndex == DOT1S_CIST_INDEX)
  {
    /*memcpy(&cistMsg.priVec,
           &p->portInstInfo[instIndex].inst.cist.cistDesignatedPriority,
       sizeof(DOT1S_CIST_PRIORITY_VECTOR_t)); */
    /*if (p->portInstInfo[DOT1S_CIST_INDEX].infoIs == INFO_MINE)
  {*/
    betterorsame = dot1sBetterOrSameInfoCist(p, &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority,INFO_MINE);
    /*}*/
  }
  else
  {
    /*memcpy(&mstiMsg.priVec,
           &p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority,
       sizeof(DOT1S_MSTI_PRIORITY_VECTOR_t)); */
    if (p->portInstInfo[instIndex].infoIs == INFO_MINE)
    {
      betterorsame = dot1sBetterOrSameInfoMsti(p,instIndex, &p->portInstInfo[instIndex].inst.msti.mstiMsgPriority);
    }
  }

  if ((p->portInstInfo[instIndex].agreed &&
       betterorsame /*&&
       (p->portInstInfo[instIndex].changedMaster == L7_FALSE)*/))
    p->portInstInfo[instIndex].agreed = L7_TRUE;
  else
    p->portInstInfo[instIndex].agreed = L7_FALSE;

  p->portInstInfo[instIndex].synced = p->portInstInfo[instIndex].synced &&
                                      p->portInstInfo[instIndex].agreed;

  if (instIndex == DOT1S_CIST_INDEX)
  {
    memcpy((char *)&p->portInstInfo[instIndex].inst.cist.cistPortPriority,
           (char *)&p->portInstInfo[instIndex].inst.cist.cistDesignatedPriority,
           sizeof(DOT1S_CIST_PRIORITY_VECTOR_t));

    memcpy((char *)&p->portInstInfo[instIndex].inst.cist.cistPortTimes,
           (char *)&p->portInstInfo[instIndex].inst.cist.cistDesignatedTimes,
           sizeof(DOT1S_CIST_TIMES_t));

    p->newInfoCist = L7_TRUE;
  }
  else /*one of the mstis*/
  {
    memcpy((char *)&p->portInstInfo[instIndex].inst.msti.mstiPortPriority,
           (char *)&p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority,
           sizeof(DOT1S_MSTI_PRIORITY_VECTOR_t));

    memcpy((char *)&p->portInstInfo[instIndex].inst.msti.mstiPortTimes,
           (char *)&p->portInstInfo[instIndex].inst.msti.mstiDesignatedTimes,
           sizeof(DOT1S_MSTI_TIMES_t));

    p->newInfoMist = L7_TRUE;
  }
  if (DOT1S_DEBUG(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL,DOT1S_CIST_INDEX))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }
  p->portInstInfo[instIndex].changedMaster = L7_FALSE;
  p->portInstInfo[instIndex].updtInfo = L7_FALSE;
  p->portInstInfo[instIndex].infoIs = INFO_MINE;

  p->portInstInfo[instIndex].pimState = PIM_UPDATE;

  rc = L7_SUCCESS;

  if (DOT1S_DEBUG(DOT1S_DEBUG_PIM,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Exit \n",__FUNCTION__);
  }


  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PIM state CURRENT
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimCurrentAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  if (DOT1S_DEBUG(DOT1S_DEBUG_PIM,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Enter \n",__FUNCTION__);
  }


  p->portInstInfo[instIndex].pimState = PIM_CURRENT;


  rc = L7_SUCCESS;
  if (DOT1S_DEBUG(DOT1S_DEBUG_PIM,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Exit \n",__FUNCTION__);
  }



  return rc;
}

/*********************************************************************
* @purpose   Actions to be performed in the PIM state RECEIVE
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    msg        @b{(input)} pointer to message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimReceiveAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex, DOT1S_MSTP_ENCAPS_t  *bpdu)
{
  L7_RC_t rc = L7_SUCCESS;
  pointTopointCheck = dot1sIhIsIntfSpeedFullDuplex(p->portNum);
  if (instIndex == DOT1S_CIST_INDEX)
  {


    p->portInstInfo[instIndex].rcvdInfo = dot1sInfoCistRcv(p, bpdu);

    rc = dot1sMasteredCistRecord(p);

    if (DOT1S_DEBUG(DOT1S_DEBUG_RCVD_INFO_DETAIL, DOT1S_CIST_INDEX))
    {
      DOT1S_DEBUG_MSG("RX Message priority vector Port (%d) CIST \n", p->portNum);
      dot1s_print_vec(&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority);
      dot1s_print_times("Cist Times",&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgTimes);
      DOT1S_DEBUG_MSG("Dot1s Port priority info \n");
      dot1s_print_vec(&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority);
      dot1s_print_times("Cist Times",&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes);
    }

  }
  else /*one of the mstis*/
  {

    p->portInstInfo[instIndex].rcvdInfo = dot1sInfoMstiRcv(p, instIndex, bpdu);
    dot1sMasteredMstiRecord(p, instIndex);
    rc = dot1sAgreeFlagCompute(p, instIndex); /*Fix for ANVL MSTP 9.5*/

    if (DOT1S_DEBUG(DOT1S_DEBUG_RCVD_INFO_DETAIL, instIndex))
    {
      DOT1S_DEBUG_MSG("Message Priority vector for port(%d) inst(%d) is:\n", p->portNum, instIndex);
      dot1s_print_msti_vec(&p->portInstInfo[instIndex].inst.msti.mstiMsgPriority);
      dot1s_print_msti_times("MSTI Times",&p->portInstInfo[instIndex].inst.msti.mstiMsgTimes);
      DOT1S_DEBUG_MSG("Port Priority vector for port(%d) inst(%d) is:\n", p->portNum, instIndex);
      dot1s_print_msti_vec(&p->portInstInfo[instIndex].inst.msti.mstiPortPriority);
      dot1s_print_msti_times("MSTI port Times",&p->portInstInfo[instIndex].inst.msti.mstiPortTimes);
    }

  }


  if (DOT1S_DEBUG(DOT1S_DEBUG_RCVD_INFO, instIndex))
  {
    DOT1S_DEBUG_MSG("Port(%d) Inst(%d) :rcvd Info = %s\n",p->portNum,instIndex,
                  dot1sRcvdInfoStringGet(p->portInstInfo[instIndex].rcvdInfo));
  }

  p->portInstInfo[instIndex].pimState = PIM_RECEIVE;

  /**************************
   * generate events for pim
   **************************
   */
  if (p->portInstInfo[instIndex].pimState == PIM_RECEIVE )
  {
    switch (p->portInstInfo[instIndex].rcvdInfo)
    {
      case  SuperiorDesignatedInfo:
        /*Send event E4*/
        /*rc = dot1sIssueCmd(pimSupDsgInfo, p->portNum, id, msg);*/
        rc = dot1sStateMachineClassifier(pimSupDsgInfo, p, instIndex, L7_NULL, L7_NULLPTR);
        break;
      case  RepeatedDesignatedInfo:
        /*Send Event E5A*/
        /*rc = dot1sIssueCmd(pimRepDsgInfo, p->portNum, id, msg);*/
        rc = dot1sStateMachineClassifier(pimRepDsgInfo, p, instIndex, L7_NULL, L7_NULLPTR);
        break;
      case  InferiorDesignatedInfo:
        /*Send Event E5*/
        /*rc = dot1sIssueCmd(pimRepDsgInfo, p->portNum, id, msg);*/
        rc = dot1sStateMachineClassifier(pimInferiorDesignatedInfo, p, instIndex, L7_NULL, L7_NULLPTR);
        break;
      case  InferiorRootAlternateInfo:
        /*Send Event E6*/
        /*rc = dot1sIssueCmd(pimRootInfo, p->portNum, id, msg);*/
        rc = dot1sStateMachineClassifier(pimRootAlternateInfo, p, instIndex, L7_NULL, L7_NULLPTR);
        break;
      case  OtherInfo:
        /*Send Event E7*/
        /*rc = dot1sIssueCmd(pimOtherInfo, p->portNum, id, msg);*/
        rc = dot1sStateMachineClassifier(pimOtherInfo, p, instIndex, L7_NULL, L7_NULLPTR);
        break;
      default:
        break;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Record the disputed Flag
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    msg        @b{(input)} pointer to the CIST or MSTI msg
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sRecordDispute(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL learningFlagSet ;



  learningFlagSet =  (p->portInstInfo[instIndex].rcvdBpduFlags & DOT1S_LEARNING_FLAG_MASK) ;


  /* For a given instance index and port if the message has learning flag set

     disputed variable is set and
   agreed variable is cleared

   Additionally if the instance index is CIST and the message was received
   from outside the region for each instance

   disputed variabl is set and
   agreed variable is cleared

 */


  if (learningFlagSet)
  {
    p->portInstInfo[instIndex].agreed = L7_FALSE;
    p->portInstInfo[instIndex].disputed = L7_TRUE;
    if (dot1s_enable_debug_detail)
    {

      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Setting disputed and clearing agreed Port(%d) Inst(%d) \n",
                    __FUNCTION__, p->portNum, instIndex);

    }

    if ((instIndex == DOT1S_CIST_INDEX) &&
        (p->rcvdInternal == L7_FALSE))
    {
      if (dot1s_enable_debug_detail)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                      "%s : Setting disputed and clearing agreed Port(%d) Inst(%d) for all MSTIs\n",
                      __FUNCTION__, p->portNum, instIndex);
      }
      for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
      {
        if (dot1sInstanceMap[i].inUse == L7_TRUE)
        {
          p->portInstInfo[i].agreed = L7_FALSE;
          p->portInstInfo[i].disputed = L7_TRUE;

        }
      }
    }

    if (dot1sCnfgrAppIsReady() != L7_TRUE)
    {
      if (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
          dot1sNsfFuncTable.dot1sIsIntfReconciled(p->portNum) != L7_TRUE)
      {
        dot1sNsfFuncTable.dot1sUpdateIntfReconciled(p->portNum);
      }
    }


  } /* LearningFlagSet */
  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PIM state OTHER
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimOtherAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].rcvdMsg = L7_FALSE;

  p->portInstInfo[instIndex].pimState = PIM_OTHER;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the PIM state ROOT
*
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    msg        @b{(input)} pointer to message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimNotDesignatedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t       rc = L7_FAILURE;



  if (instIndex == DOT1S_CIST_INDEX)
  {
    rc = dot1sAgreementCistRecord(p);
  }
  else /*one of the mstis*/
  {
    rc = dot1sAgreementMstiRecord(p, instIndex);
  }
  p->portInstInfo[instIndex].rcvdMsg = L7_FALSE;
  p->portInstInfo[instIndex].pimState = PIM_NOT_DESIG;

  /*Set TC FLags */
  dot1sTcFlagsSet(p, instIndex);
  /*
  p->portInstInfo[instIndex].proposing = L7_FALSE;
  */
  /* Need to recalculate the roles if the adjacent bridge has left/joined the region */
  if (p->prevRcvdInternal != p->rcvdInternal)
  {
    p->portInstInfo[instIndex].reselect = L7_TRUE;
    p->portInstInfo[instIndex].selected = L7_FALSE;
  }


  return rc;
}

/*********************************************************************
* @purpose   Actions to be performed in the PIM state REP_DESIG
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    msg        @b{(input)} pointer to message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimRepDesigAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t       rc = L7_FAILURE;

  p->infoInternal = p->rcvdInternal;

  if (instIndex == DOT1S_CIST_INDEX)
  {

    rc = dot1sProposalCistRecord(p);
    rc = dot1sAgreementCistRecord(p);
    rc = dot1sRcvdInfoWhileUpdt(p,DOT1S_CIST_INDEX);
  }
  else/*one of the mstis*/
  {

    rc = dot1sProposalMstiRecord(p, instIndex);
    rc = dot1sAgreementMstiRecord(p, instIndex);
    rc = dot1sRcvdInfoWhileUpdt(p, instIndex);
  }

  dot1sTcFlagsSet(p, instIndex);
  /* Need to recalculate the roles if the adjacent bridge has left/joined the region */
  if (p->prevRcvdInternal != p->rcvdInternal)
  {
    p->portInstInfo[instIndex].reselect = L7_TRUE;
    p->portInstInfo[instIndex].selected = L7_FALSE;
  }

  p->portInstInfo[instIndex].rcvdMsg = L7_FALSE;

  p->portInstInfo[instIndex].pimState = PIM_REP_DESIG;




  return rc;
}

/*********************************************************************
* @purpose   Actions to be performed in the PIM state PIM_INFERIOR_DESIG
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    msg        @b{(input)} pointer to message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimInferiorDesigAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t       rc = L7_FAILURE;


  dot1sRecordDispute(p,instIndex);
  p->portInstInfo[instIndex].rcvdMsg = L7_FALSE;

  p->portInstInfo[instIndex].pimState = PIM_INFERIOR_DESIG;

  if (p->prevRcvdInternal != p->rcvdInternal)
  {
    p->portInstInfo[instIndex].reselect = L7_TRUE;
    p->portInstInfo[instIndex].selected = L7_FALSE;
  }


  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PIM state SUP_DESIG
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    msg        @b{(input)} pointer to message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimSupDesigAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_BOOL       betterorsame = L7_FALSE;


  p->infoInternal = p->rcvdInternal;
  p->portInstInfo[instIndex].proposing = L7_FALSE;
  p->portInstInfo[instIndex].agreed = L7_FALSE;

  if (instIndex == DOT1S_CIST_INDEX)
  {

    rc = dot1sProposalCistRecord(p);
    betterorsame = dot1sBetterOrSameInfoCist(p,&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority ,
                                             INFO_RECEIVED );

    p->portInstInfo[instIndex].agree = (p->portInstInfo[instIndex].agree == L7_TRUE) && (betterorsame == L7_TRUE);

    rc = dot1sAgreementCistRecord(p);
  }
  else
  {

    rc = dot1sProposalMstiRecord(p, instIndex);
    if (p->portInstInfo[instIndex].infoIs == INFO_RECEIVED)
    {
      betterorsame = dot1sBetterOrSameInfoMsti(p, instIndex, &p->portInstInfo[instIndex].inst.msti.mstiMsgPriority);
    }
    /*p->portInstInfo[instIndex].agree = p->portInstInfo[instIndex].agree &&
                         betterorsame;*/
    if (p->portInstInfo[instIndex].agree == L7_TRUE &&
        betterorsame == L7_TRUE)
    {
      p->portInstInfo[instIndex].agree = L7_TRUE;
    }
    else
    {
      p->portInstInfo[instIndex].agree = L7_FALSE;
    }
    rc = dot1sAgreementMstiRecord(p, instIndex);
  }
  /*
  p->portInstInfo[instIndex].synced = p->portInstInfo[instIndex].synced &&
                    p->portInstInfo[instIndex].agreed;
  */
  if (p->portInstInfo[instIndex].synced == L7_TRUE &&
      p->portInstInfo[instIndex].agreed == L7_TRUE)
  {
    p->portInstInfo[instIndex].synced = L7_TRUE;
  }
  else
  {
    p->portInstInfo[instIndex].synced = L7_FALSE;
  }
  if (instIndex == DOT1S_CIST_INDEX)
  {
    memcpy((char *)&p->portInstInfo[instIndex].inst.cist.cistPortPriority,
           (char *)&p->portInstInfo[instIndex].inst.cist.cistMsgPriority,
           sizeof(DOT1S_CIST_PRIORITY_VECTOR_t));

    memcpy((char *)&p->portInstInfo[instIndex].inst.cist.cistPortTimes,
           (char *)&p->portInstInfo[instIndex].inst.cist.cistMsgTimes,
           sizeof(DOT1S_CIST_TIMES_t));

    if (p->portInstInfo[instIndex].inst.cist.cistPortTimes.helloTime < L7_DOT1S_BRIDGE_HELLOTIME_MIN * DOT1S_TIMER_UNIT)
    {
      p->portInstInfo[instIndex].inst.cist.cistPortTimes.helloTime = L7_DOT1S_BRIDGE_HELLOTIME_MIN * DOT1S_TIMER_UNIT;
    }
    else if (p->portInstInfo[instIndex].inst.cist.cistPortTimes.helloTime > L7_DOT1S_BRIDGE_HELLOTIME_MAX * DOT1S_TIMER_UNIT)
    {
      p->portInstInfo[instIndex].inst.cist.cistPortTimes.helloTime = L7_DOT1S_BRIDGE_HELLOTIME_MAX * DOT1S_TIMER_UNIT;
    }

    rc = dot1sRcvdInfoWhileUpdt(p,instIndex);
  }
  else
  {
    memcpy((char *)&p->portInstInfo[instIndex].inst.msti.mstiPortPriority,
           (char *)&p->portInstInfo[instIndex].inst.msti.mstiMsgPriority,
           sizeof(DOT1S_MSTI_PRIORITY_VECTOR_t));

    memcpy((char *)&p->portInstInfo[instIndex].inst.msti.mstiPortTimes,
           (char *)&p->portInstInfo[instIndex].inst.msti.mstiMsgTimes,
           sizeof(DOT1S_MSTI_TIMES_t));


    rc = dot1sRcvdInfoWhileUpdt(p, instIndex);
  }
  dot1sTcFlagsSet(p, instIndex);
  p->portInstInfo[instIndex].infoIs = INFO_RECEIVED;
  p->portInstInfo[instIndex].reselect = L7_TRUE;
  p->portInstInfo[instIndex].selected = L7_FALSE;
  p->portInstInfo[instIndex].rcvdMsg = L7_FALSE;

  p->portInstInfo[instIndex].pimState = PIM_SUP_DESIG;

  return rc;
}

/*********************************************************************
* @purpose  This routine claculates the next state fo the PIM state machine
*           and executes the action for that next state
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    msg        @b{(input)} pointer to message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimMachine(L7_uint32 dot1sEvent,
                        DOT1S_PORT_COMMON_t *p,
                        L7_uint32 instIndex,
                        DOT1S_MSTP_ENCAPS_t   *bpdu)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_pim_states_t  nextState;
  L7_uint32       normalizedEvent;

  normalizedEvent = dot1sEvent;


  if (p->portInstInfo[instIndex].pimState > PIM_STATES)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Invalid PIM state(%d) for port(%d) instIndex %d \n", __FUNCTION__,p->portInstInfo[instIndex].pimState,
                  p->portNum, instIndex);
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Event %d \n", dot1sEvent);
    return L7_FAILURE;
  }

  nextState =  pimStateTable[normalizedEvent][p->portInstInfo[instIndex].pimState];

  if (DOT1S_DEBUG(DOT1S_DEBUG_PIM,DOT1S_CIST_INDEX))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: curr state %d dot1sEvent %d normalized event %d  next state %d \n",__FUNCTION__,
                  p->portInstInfo[instIndex].pimState, dot1sEvent, normalizedEvent, nextState);
  }


  switch (nextState)
  {
    case PIM_DISABLED:
      dot1sPimDisabledAction(p, instIndex);
      rc = dot1sPimGenerateEvents(p, instIndex);
      break;

      /*case PIM_ENABLED:*/ /*this state has been removed in draft 15*/
      /*rc = dot1sPimEnabledAction(p, instIndex);*/
      /* break;*/
    case PIM_AGED:
      dot1sPimAgedAction(p, instIndex);
      rc = dot1sPimGenerateEvents(p, instIndex);
      break;
    case PIM_UPDATE:
      dot1sPimUpdateAction(p, instIndex);
      /*UCT to Current State*/
      dot1sPimCurrentAction(p, instIndex);
      rc = dot1sPimGenerateEvents(p, instIndex);
      break;
    case PIM_CURRENT:
      dot1sPimCurrentAction(p, instIndex);
      rc = dot1sPimGenerateEvents(p, instIndex);
      break;
    case PIM_OTHER:
      dot1sPimOtherAction(p, instIndex);
      /*UCT to Current State*/
      rc = dot1sPimCurrentAction(p, instIndex);
      /*rc = dot1sPimGenerateEvents(p, instIndex, msg);*/
      break;
    case PIM_NOT_DESIG:
      dot1sPimNotDesignatedAction(p, instIndex);
      /*UCT to Current State*/
      dot1sPimCurrentAction(p, instIndex);
      rc = dot1sPimGenerateEvents(p, instIndex);
      break;
    case PIM_REP_DESIG:
      dot1sPimRepDesigAction(p, instIndex);
      /*UCT to Current State*/
      dot1sPimCurrentAction(p, instIndex);
      rc = dot1sPimGenerateEvents(p, instIndex);
      break;
    case PIM_INFERIOR_DESIG:
      rc = dot1sPimInferiorDesigAction(p, instIndex);
      /*UCT to Current State*/
      rc = dot1sPimCurrentAction(p, instIndex);
      rc = dot1sPimGenerateEvents(p, instIndex);
      break;
    case PIM_SUP_DESIG:
      rc = dot1sPimSupDesigAction(p, instIndex);
      /*UCT to Current State*/
      rc = dot1sPimCurrentAction(p, instIndex);
      rc = dot1sPimGenerateEvents(p, instIndex);
      break;
    case PIM_RECEIVE:
      if (bpdu != L7_NULLPTR)
      {
        dot1sPimReceiveAction(p, instIndex, bpdu);
        rc = dot1sPimGenerateEvents(p, instIndex);
      }
      else
        rc = L7_FAILURE;
      break;
    default:
      if (dot1sEvent == pimRcvdXstMsgNotUpdtXstInfo ||
          dot1sEvent == pimSupDsgInfo ||
          dot1sEvent == pimRepDsgInfo ||
          dot1sEvent == pimInferiorDesignatedInfo ||
          dot1sEvent == pimRootAlternateInfo ||
          dot1sEvent == pimOtherInfo)
      {
        p->portInstInfo[instIndex].rcvdMsg = L7_FALSE;
      }
      break;
  }/*end switch (nextState)*/

  return rc;
}
/*********************************************************************
* @purpose  Events that the port information machine needs to generate and propagate
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    msg        @b{(input)} pointer to message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPimGenerateEvents(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_uint32     id;
  L7_RC_t       rc;

  DOT1S_BRIDGE_t  *bridge;
  L7_BOOL       rcvdMstiInfo = L7_FALSE;


  bridge = dot1sBridgeParmsFind();

  rc = dot1sInstNumFind(instIndex, &id);


  if (p->portEnabled == L7_FALSE &&
      p->portInstInfo[instIndex].infoIs != INFO_DISABLED)
  {
    /*Send event E12*/
    rc = dot1sIssueCmd(pimPortDisabledInfoIsNotEqualDisabled, p->portNum, id, L7_NULLPTR);
  }
  if (instIndex == DOT1S_CIST_INDEX)
  {
    if (p->portInstInfo[instIndex].rcvdInfoWhile == 0 &&
        p->portInstInfo[instIndex].infoIs == INFO_RECEIVED &&
        p->portInstInfo[instIndex].updtInfo == L7_FALSE &&
        p->portInstInfo[instIndex].rcvdMsg == L7_FALSE
       )
    {
      /*Send Event E10*/
      rc = dot1sStateMachineClassifier(pimRcvdRcvdInfoWhileZeroNotUpdtInfoNotRcvdXstInfo,
                                       p, instIndex , L7_NULL, L7_NULLPTR);
    }
  }
  else
  {

    if (p->portInstInfo[instIndex].rcvdMsg == L7_TRUE &&
        p->portInstInfo[DOT1S_CIST_INDEX].rcvdMsg == L7_FALSE)
    {
      rcvdMstiInfo = L7_TRUE;
    }


    if (p->portInstInfo[instIndex].rcvdInfoWhile == 0 &&
        p->portInstInfo[instIndex].infoIs == INFO_RECEIVED &&
        p->portInstInfo[instIndex].updtInfo == L7_FALSE &&
        rcvdMstiInfo == L7_FALSE
       )
    {
      /*Send Event E10*/
      rc = dot1sStateMachineClassifier(pimRcvdRcvdInfoWhileZeroNotUpdtInfoNotRcvdXstInfo,
                                       p, instIndex, L7_NULL, L7_NULLPTR);
    }

  }

  if (p->portInstInfo[instIndex].updtInfo == L7_TRUE &&
      p->portInstInfo[instIndex].selected == L7_TRUE)
    /*Send Event E9*/
    /*rc = dot1sIssueCmd(pimSelectedUpdtInfo, p->portNum, id, L7_NULLPTR);*/
    rc = dot1sStateMachineClassifier(pimSelectedUpdtInfo, p, instIndex, L7_NULL, L7_NULLPTR);
  /***************************
   * generate events for prs
   *************************
   */
  if (p->portInstInfo[instIndex].reselect == L7_TRUE)
  {
    /*Send Event E13*/
    /*rc = dot1sIssueCmd(prsReselect, p->portNum, id, L7_NULLPTR);*/
    rc = dot1sStateMachineClassifier(prsReselect, p, instIndex, L7_NULL, L7_NULLPTR);
  }




  /**************************
   * generate events for prt
   *************************
   */

  dot1sGenerateEventsForPrt(p,instIndex);

  dot1sGenerateEventsForTcm(p,instIndex);

  /* See if there is any need to transmit BPDU */
  if (p->sendRSTP == L7_TRUE &&
      (p->newInfoCist == L7_TRUE ||
       p->newInfoMist == L7_TRUE) &&
      p->txCount < bridge->TxHoldCount && (p->helloWhen != 0) )
  {
    /*Send Event E66*/
    /*rc = dot1sIssueCmd(ptxSendRstp, p->portNum, L7_NULL, L7_NULLPTR);*/
    rc = dot1sStateMachineClassifier(ptxSendRstp, p, L7_NULL, L7_NULL, L7_NULLPTR);
  }


  return rc;
}

/**************************************************************************
 *******************************PRS Support Routines***********************
 **************************************************************************
 */
/*********************************************************************
* @purpose  Sets the selectedRoles to DisabledPort for all ports of a
*           bridge for a given instance (CIST or MSTI)
*
* @param    instIndex      @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sRolesDisabledTreeUpdt(L7_uint32 instIndex)
{
  DOT1S_PORT_COMMON_t   *p;
  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    p->portInstInfo[instIndex].selectedRole = ROLE_DISABLED;
    p = dot1sIntfFindNext(p->portNum);
  }/* while */


  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Clears the reselect param for the tree for all ports of
*           the bridge
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sReselectTreeClear(L7_uint32 instIndex)
{
  DOT1S_PORT_COMMON_t   *p;

  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    p->portInstInfo[instIndex].reselect = L7_FALSE;
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the reselect param for the tree for all ports of
*           the bridge
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sReselectTreeSet(L7_uint32 instIndex)
{
  DOT1S_PORT_COMMON_t   *p;

  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    p->portInstInfo[instIndex].reselect = L7_TRUE;
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Calculates the different CIST Spanning Tree Vectors and
*           timer values
*
* @param    @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sRolesCistUpdt()
{
  L7_RC_t             rc = L7_FAILURE;
  L7_uint32           i ,j;
  DOT1S_PORT_COMMON_t       *p;
  DOT1S_BRIDGE_t        *bridge;
  DOT1S_CIST_PRIORITY_VECTOR_t  bestPriVec;
  DOT1S_PORTID_t        bestPortId = 0;
  DOT1S_PRI_COMP_t        comparison;
  L7_uint32           maxAge;
  L7_BOOL             changedMaster, isRoot;

  memset((void *)&cistRootPathPriVec, 0xFF, sizeof(cistRootPathPriVec));
  bzero((char *)&bestPriVec, (L7_int32)sizeof(DOT1S_CIST_PRIORITY_VECTOR_t));


  bridge = dot1sBridgeParmsFind();
  /*calculate all the root path priority vectors*/ /*13.26.25 a*/
  i = 1;
  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    if (p->portEnabled == L7_TRUE &&
        p->portInstInfo[DOT1S_CIST_INDEX].infoIs == INFO_RECEIVED)
    {
      if (p->rcvdInternal == L7_FALSE)
      {
        memcpy(&cistRootPathPriVec[i],
               &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority,
               sizeof(DOT1S_CIST_PRIORITY_VECTOR_t));
        cistRootPathPriVec[i].extRootPathCost += p->portInstInfo[DOT1S_CIST_INDEX].ExternalPortPathCost;
        memcpy(&cistRootPathPriVec[i].regRootId,
               &dot1sInstance->cist.BridgeIdentifier,
               sizeof(DOT1S_BRIDGEID_t));
        cistRootPathPortId[i] = p->portInstInfo[DOT1S_CIST_INDEX].portId;
        if (cistRootPathPriVec[i].intRootPathCost != 0)
        {
          return L7_FAILURE;/*should have been set to zero*/
        }
      }
      else /*received from inside our own region*/
      {
        memcpy(&cistRootPathPriVec[i],
               &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority,
               sizeof(DOT1S_CIST_PRIORITY_VECTOR_t));
        cistRootPathPriVec[i].intRootPathCost += p->portInstInfo[DOT1S_CIST_INDEX].InternalPortPathCost;
        cistRootPathPortId[i] = p->portInstInfo[DOT1S_CIST_INDEX].portId;
      }
    }
    p = dot1sIntfFindNext(p->portNum);
    /* This technically could increment i to be greater than the array size
       But since the array cistRootPathPriVec is defined of size L7_MAX_INTERFACE_COUNT+ 1,
       it is greater than or equal to L7_DOT1S_MAX_INTERFACE_COUNT. So we should be fine.
    */
    i++;
  }/* while */
  /*calculate the bridge's root priority vector*/  /*13.26.25 b*/
  /*start with the best as the bridge priority vector*/
  memcpy(&bestPriVec, &dot1sInstance->cist.CistBridgePriority, sizeof(DOT1S_CIST_PRIORITY_VECTOR_t));
  /* restore the i to point to the last entry in the root path pri vector*/
  i--;
  for (;i > 0;i--)
  {

    p = dot1sIntfFind(DOT1S_PORTNUM_FROM_PORTID_GET(cistRootPathPriVec[i].rxPortId));
    if (p != L7_NULLPTR && p->portNum != 0)
    {
      if ((p->portEnabled == L7_TRUE) &&
          (p->portInstInfo[DOT1S_CIST_INDEX].infoIs == INFO_RECEIVED) &&
          (p->restrictedRole == L7_FALSE) &&
          ((memcmp(cistRootPathPriVec[i].dsgBridgeId.macAddr,
                   dot1sInstance->cist.CistBridgePriority.dsgBridgeId.macAddr, L7_MAC_ADDR_LEN) != 0) &&
           (memcmp(cistRootPathPriVec[i].rootId.macAddr,
                   dot1sInstance->cist.CistBridgePriority.dsgBridgeId.macAddr, L7_MAC_ADDR_LEN) != 0))
         )
      {

        if ((p->rcvdInternal == L7_TRUE) && memcmp(cistRootPathPriVec[i].regRootId.macAddr,
                                                   dot1sInstance->cist.CistBridgePriority.dsgBridgeId.macAddr, L7_MAC_ADDR_LEN) == 0)
          continue;

        comparison = dot1sCistPriVecAllCompare(&bestPriVec, &cistRootPathPriVec[i]);
        if (comparison == WORSE)
        {
          /*replace the best with the current cistRootPathPriVec[i]*/
          memcpy(&bestPriVec, &cistRootPathPriVec[i], sizeof(DOT1S_CIST_PRIORITY_VECTOR_t));
          bestPortId = cistRootPathPortId[i];
        }
      }
    }
  }/* for (;i > 0;i--) */
  /* Check if this bridge is to become the Root and send a trap if so */
  dot1sNewRootCheck(dot1sInstance->cist.CistBridgePriority.rootId,
                    dot1sInstance->cist.cistRootPriority.rootId,
                    bestPriVec.rootId,
                    DOT1S_CIST_INDEX);

  /*check if we need to set changedMaster flag for the MSTIs*/

  changedMaster = (((bestPriVec.extRootPathCost != 0) || (dot1sInstance->cist.cistRootPriority.extRootPathCost != 0)) &&
                   (dot1sIdNotEqual(bestPriVec.regRootId, dot1sInstance->cist.cistRootPriority.regRootId ) == L7_TRUE)
                  ) ;



  memcpy(&dot1sInstance->cist.cistRootPriority,
         &bestPriVec,
         sizeof(DOT1S_CIST_PRIORITY_VECTOR_t));
  dot1sInstance->cist.cistRootPortId = bestPortId;

  /*calculate the bridge's root times*/   /*13.26.25 c*/
  comparison =  dot1sCistPriVecAllCompare(&bestPriVec, &dot1sInstance->cist.CistBridgePriority);
  if (comparison == SAME)
  {
    isRoot = L7_TRUE;
    dot1sInstance->cist.cistRootTimes.fwdDelay =
    dot1sInstance->cist.CistBridgeTimes.bridgeFwdDelay;
    dot1sInstance->cist.cistRootTimes.helloTime =
    dot1sInstance->cist.CistBridgeTimes.bridgeHelloTime;
    dot1sInstance->cist.cistRootTimes.maxAge =
    dot1sInstance->cist.CistBridgeTimes.bridgeMaxAge;
    dot1sInstance->cist.cistRootTimes.msgAge =
    dot1sInstance->cist.CistBridgeTimes.msgAge;
    dot1sInstance->cist.cistRootTimes.remHops =
    dot1sInstance->cist.CistBridgeTimes.maxHops;

  }
  else
  {
    isRoot = L7_FALSE;
    p = dot1sIntfFind(DOT1S_PORTNUM_FROM_PORTID_GET(bestPortId));
    if (p != L7_NULLPTR && p->portNum != 0)
    {
      memcpy((char *)&dot1sInstance->cist.cistRootTimes,
             (char *)&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes,
             sizeof(DOT1S_CIST_TIMES_t));
      maxAge = dot1sInstance->cist.cistRootTimes.maxAge/DOT1S_TIMER_UNIT;

      if (p->rcvdInternal == L7_FALSE)
      {
        if (maxAge > 16)
        {
          if (maxAge % 16 >= 8)
            dot1sInstance->cist.cistRootTimes.msgAge += (maxAge/16 + 1) * DOT1S_TIMER_UNIT;
          else
            dot1sInstance->cist.cistRootTimes.msgAge += (maxAge/16) * DOT1S_TIMER_UNIT;
        }
        else
          dot1sInstance->cist.cistRootTimes.msgAge += (1 * DOT1S_TIMER_UNIT);

      }
      else
      {
        if (dot1sInstance->cist.cistRootTimes.remHops > 0)
          dot1sInstance->cist.cistRootTimes.remHops--;

      }
    } /* end if (p != L7_NULLPTR && p->portNum != 0) */
  }

  /*calculate the designated priority vector for each port*//*13.26.25 d*/
  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    memcpy((char *)&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority,
           (char *)&dot1sInstance->cist.cistRootPriority,
           sizeof(DOT1S_CIST_PRIORITY_VECTOR_t));

    memcpy(&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.dsgBridgeId,
           &dot1sInstance->cist.BridgeIdentifier,
           sizeof(DOT1S_BRIDGEID_t));

    p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.dsgPortId =
    p->portInstInfo[DOT1S_CIST_INDEX].portId;
    p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.rxPortId =
    p->portInstInfo[DOT1S_CIST_INDEX].portId;

    if ((p->sendRSTP == L7_FALSE) ||
        (bridge->ForceVersion < DOT1S_FORCE_VERSION_DOT1W)
       )
    {
      memcpy(&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.regRootId,
             &dot1sInstance->cist.BridgeIdentifier,
             sizeof(DOT1S_BRIDGEID_t));
    }
    /*in the same loop calculate the designated times for each port*/   /*13.26.25 e*/
    memcpy((char *)&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes,
           (char *)&dot1sInstance->cist.cistRootTimes,
           sizeof(DOT1S_CIST_TIMES_t));
    if ((isRoot == L7_TRUE) && (p->HelloTime != DOT1S_INVALID_HELLO_TIME))
    {
      /* If this bridge is the root copy the port specific configurable values
         to the designated priority vector. :For now that only means hello time */
      p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.helloTime =
      p->HelloTime;

    }


    if ((changedMaster == L7_TRUE) && (p->portLinkState == L7_UP))
    {

      for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
      {
        if (dot1sInstanceMap[j].inUse == L7_TRUE)
        {
          p->portInstInfo[j].changedMaster = L7_TRUE;
          if (p->infoInternal == L7_TRUE)
          {
            p->portInstInfo[j].agree = L7_FALSE;
            p->portInstInfo[j].agreed = L7_FALSE;
            p->portInstInfo[j].synced = L7_FALSE;
            p->portInstInfo[j].sync = L7_TRUE;
            p->portInstInfo[j].reselect = L7_TRUE;

          }
        }
      }
    }/*if ((changedMaster == L7_TRUE) && (p->portLinkState == L7_UP))*/
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  /*13.26.25 f - l*/

  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    if (p->portInstInfo[DOT1S_CIST_INDEX].infoIs == INFO_DISABLED)
    {
      p->portInstInfo[DOT1S_CIST_INDEX].selectedRole = ROLE_DISABLED; /*13.26.25 f*/
    }
    else if (p->portInstInfo[DOT1S_CIST_INDEX].infoIs == INFO_AGED)
    {
      p->portInstInfo[DOT1S_CIST_INDEX].updtInfo = L7_TRUE;
      p->portInstInfo[DOT1S_CIST_INDEX].selectedRole = ROLE_DESIGNATED; /*13.26.25 g*/

    }
    else if (p->portInstInfo[DOT1S_CIST_INDEX].infoIs == INFO_MINE)
    {
      DOT1S_PRI_COMP_t compare;

      p->portInstInfo[DOT1S_CIST_INDEX].selectedRole = ROLE_DESIGNATED;  /*13.26.25 h*/


      compare = dot1sCistPriVecAllCompare(&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority,
                                          &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority);

      if ((compare != SAME ) ||
          ((p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.fwdDelay !=
            dot1sInstance->cist.cistRootTimes.fwdDelay) ||
           (p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.maxAge !=
            dot1sInstance->cist.cistRootTimes.maxAge) ||
           (p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.msgAge !=
            dot1sInstance->cist.cistRootTimes.msgAge) ||
           (p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.remHops !=
            dot1sInstance->cist.cistRootTimes.remHops) ||
           (p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.helloTime !=
            dot1sInstance->cist.cistRootTimes.helloTime)
          )
         )
      {
        p->portInstInfo[DOT1S_CIST_INDEX].updtInfo = L7_TRUE;
      }
    }
    else if (p->portInstInfo[DOT1S_CIST_INDEX].infoIs == INFO_RECEIVED)
    {
      if (L7_TRUE == dot1sCistRootPriVecDerived(&dot1sInstance->cist.cistRootPriority,
                                                &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority,
                                                p->portInstInfo[DOT1S_CIST_INDEX].ExternalPortPathCost,
                                                p->portInstInfo[DOT1S_CIST_INDEX].InternalPortPathCost,
                                                p->rcvdInternal))
      {
        p->portInstInfo[DOT1S_CIST_INDEX].selectedRole = ROLE_ROOT;  /*13.26.25 i*/
        p->portInstInfo[DOT1S_CIST_INDEX].updtInfo = L7_FALSE;
      }
      else
      {
        if (dot1sCistPriVecAllCompare(&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority,
                                      &p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority) != BETTER)
        {
          if ((memcmp(&p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortPriority.dsgBridgeId,
                      &dot1sInstance->cist.BridgeIdentifier,
                      sizeof(DOT1S_BRIDGEID_t)) != 0)
             )
          {
            p->portInstInfo[DOT1S_CIST_INDEX].selectedRole = ROLE_ALTERNATE;  /*13.26.25 j*/
            p->portInstInfo[DOT1S_CIST_INDEX].updtInfo = L7_FALSE;

          }
          else
          {
            p->portInstInfo[DOT1S_CIST_INDEX].selectedRole = ROLE_BACKUP;
            p->portInstInfo[DOT1S_CIST_INDEX].updtInfo = L7_FALSE;

          }
        }
        else
        {
          p->portInstInfo[DOT1S_CIST_INDEX].selectedRole = ROLE_DESIGNATED;
          p->portInstInfo[DOT1S_CIST_INDEX].updtInfo = L7_TRUE;
          p->portInstInfo[DOT1S_CIST_INDEX].agreed = L7_FALSE;
          p->portInstInfo[DOT1S_CIST_INDEX].agree = L7_FALSE;
          if (p->rcvdInternal == L7_FALSE)
          {
            /* Set the agreed and agree of this port in all MSTIs to FALSE */
            for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
            {
              if (dot1sInstanceMap[j].inUse == L7_TRUE)
              {
                p->portInstInfo[j].agree = L7_FALSE;
                p->portInstInfo[j].agreed = L7_FALSE;
                p->portInstInfo[j].updtInfo = L7_TRUE;
              }
            }
          }/* if (p->rcvdInternal == L7_FALSE)*/

        }
      } /* end if else cistpriorityvectorderieved */
    } /* end else if info is recvd */
    if (changedMaster && (p->portLinkState == L7_UP))
    {
      for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
      {
        if ((dot1sInstanceMap[j].inUse == L7_TRUE) &&
            (p->portInstInfo[j].selected == L7_TRUE &&
             p->portInstInfo[j].updtInfo == L7_FALSE))
        {
          p->portInstInfo[j].sync = L7_TRUE;
          p->portInstInfo[j].synced = L7_FALSE;
        }
      }
    }

    p = dot1sIntfFindNext(p->portNum);
  }/* while */


  rc = L7_SUCCESS;
  return rc;

}
/*********************************************************************
* @purpose  Calculates the different MST Spanning Tree Vectors and
*           timer values for a MSTI
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sRolesMstiUpdt(L7_uint32 instIndex)
{
  L7_RC_t             rc = L7_FAILURE;
  L7_uint32           i;
  DOT1S_BRIDGE_t        *bridge;
  DOT1S_MSTI_PRIORITY_VECTOR_t  bestPriVec;
  DOT1S_PORTID_t        bestPortId = 0;
  DOT1S_PRI_COMP_t        comparison;
  DOT1S_PORT_COMMON_t       *p;

  memset((void *)&mstiRootPathPriVec, 0xFF, sizeof(mstiRootPathPriVec));
  bzero((char *)&bestPriVec, (L7_int32)sizeof(DOT1S_MSTI_PRIORITY_VECTOR_t));
  bridge = dot1sBridgeParmsFind();
  /*calculate all the root path priority vectors*/ /*13.26.26 a*/
  i = 1;
  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    if (p->portInstInfo[instIndex].infoIs != INFO_DISABLED &&
        p->portInstInfo[instIndex].infoIs == INFO_RECEIVED)
    {
      memcpy(&mstiRootPathPriVec[i],
             &p->portInstInfo[instIndex].inst.msti.mstiPortPriority,
             sizeof(DOT1S_MSTI_PRIORITY_VECTOR_t));
      mstiRootPathPriVec[i].intRootPathCost += p->portInstInfo[instIndex].InternalPortPathCost;
      mstiRootPathPortId[i] = p->portInstInfo[instIndex].portId;
    }
    p = dot1sIntfFindNext(p->portNum);
    i++;
  }/* while */
  /*calculate the bridge's root priority vector*/  /*13.26.26 b*/
  /*start with the best as the bridge priority vector*/
  memcpy(&bestPriVec, &dot1sInstance->msti[instIndex].MstiBridgePriority, sizeof(DOT1S_MSTI_PRIORITY_VECTOR_t));
  /* restore the i to point to the last entry in the root path pri vector*/
  i--;
  for (;i > 0;i--)
  {
    p = dot1sIntfFind(DOT1S_PORTNUM_FROM_PORTID_GET(mstiRootPathPriVec[i].rxPortId));
    if (p != L7_NULLPTR && p->portNum != 0)
    {
      if ((p->portInstInfo[instIndex].infoIs == INFO_RECEIVED) &&
          (p->restrictedRole == L7_FALSE) &&
          ((memcmp(mstiRootPathPriVec[i].dsgBridgeId.macAddr,
                   dot1sInstance->msti[instIndex].MstiBridgePriority.dsgBridgeId.macAddr,
                   L7_MAC_ADDR_LEN) != 0) &&
           (memcmp(mstiRootPathPriVec[i].regRootId.macAddr,
                   dot1sInstance->msti[instIndex].MstiBridgePriority.dsgBridgeId.macAddr,
                   L7_MAC_ADDR_LEN) != 0))
         )
      {
        comparison = dot1sMstiPriVecAllCompare(&bestPriVec, &mstiRootPathPriVec[i]);
        if (comparison == WORSE)
        {
          /*replace the best with the current mstiRootPathPriVec[i]*/
          memcpy(&bestPriVec, &mstiRootPathPriVec[i], sizeof(DOT1S_MSTI_PRIORITY_VECTOR_t));
          bestPortId = mstiRootPathPortId[i];
        }
      }
    }
  }/* for (;i > 0;i--) */

  /* Check if this bridge is to become the Root and send a trap if so */
  rc = dot1sNewRootCheck(dot1sInstance->msti[instIndex].MstiBridgePriority.regRootId,
                         dot1sInstance->msti[instIndex].mstiRootPriority.regRootId,
                         bestPriVec.regRootId,
                         instIndex);

  memcpy(&dot1sInstance->msti[instIndex].mstiRootPriority,
         &bestPriVec,
         sizeof(DOT1S_MSTI_PRIORITY_VECTOR_t));
  dot1sInstance->msti[instIndex].mstiRootPortId = bestPortId;

  /*calculate the bridge's root times*/   /*13.26.26 c*/
  comparison =  dot1sMstiPriVecAllCompare(&bestPriVec, &dot1sInstance->msti[instIndex].MstiBridgePriority);
  if (comparison == SAME)
  {
    dot1sInstance->msti[instIndex].mstiRootTimes.remHops = dot1sInstance->msti[instIndex].MstiBridgeTimes.bridgeMaxHops;
  }
  else
  {
    p = dot1sIntfFind(DOT1S_PORTNUM_FROM_PORTID_GET(bestPortId));
    if (p != L7_NULLPTR && p->portNum != 0)
    {
      memcpy((char *)&dot1sInstance->msti[instIndex].mstiRootTimes,
             (char *)&p->portInstInfo[instIndex].inst.msti.mstiPortTimes,
             sizeof(DOT1S_MSTI_TIMES_t));
      /* May have to consider overflow */
      if (dot1sInstance->msti[instIndex].mstiRootTimes.remHops > 0)
        dot1sInstance->msti[instIndex].mstiRootTimes.remHops--;
    }
  }

  /*calculate the designated priority vector for each port*//*13.26.26 d*/
  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    memcpy((char *)&p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority,
           (char *)&dot1sInstance->msti[instIndex].mstiRootPriority,
           sizeof(DOT1S_MSTI_PRIORITY_VECTOR_t));

    memcpy(&p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgBridgeId,
           &dot1sInstance->msti[instIndex].BridgeIdentifier,
           sizeof(DOT1S_BRIDGEID_t));
    p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgPortId =
    p->portInstInfo[instIndex].portId;
    p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.rxPortId =
    p->portInstInfo[instIndex].portId;


    /*in the same loop calculate the designated times for each port*/   /*13.26.25 e*/
    memcpy((char *)&p->portInstInfo[instIndex].inst.msti.mstiDesignatedTimes,
           (char *)&dot1sInstance->msti[instIndex].mstiRootTimes,
           sizeof(DOT1S_MSTI_TIMES_t));

    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  /*13.26.26 f - l*/

  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    if (p->portInstInfo[instIndex].infoIs == INFO_DISABLED)
    {
      p->portInstInfo[instIndex].selectedRole = ROLE_DISABLED; /*13.26.26 f*/
    }

    else if ((p->portInstInfo[DOT1S_CIST_INDEX].infoIs == INFO_RECEIVED)
        && (p->infoInternal == L7_FALSE))
    {
      if (p->portInstInfo[DOT1S_CIST_INDEX].selectedRole == ROLE_ROOT)
      {
        p->portInstInfo[instIndex].selectedRole = ROLE_MASTER;   /*13.26.26 g*/
        /*if (DOT1S_PROTO_INST_DEBUG(instIndex) == L7_TRUE)
        {
             SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Port(%d) Inst(%d) :Selected Role = MASTER\n",p->portNum,instIndex );
        } */


        if (((dot1sIdNotEqual(p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId,
                              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgBridgeId)) ||
             (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgPortId !=
              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgPortId) ||
             (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.intRootPathCost !=
              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.intRootPathCost) ||
             dot1sIdNotEqual(p->portInstInfo[instIndex].inst.msti.mstiPortPriority.regRootId,
                             p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.regRootId) ||
             (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.rxPortId !=
              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.rxPortId)
            ) ||
            ((p->portInstInfo[instIndex].inst.msti.mstiPortTimes.remHops !=
              dot1sInstance->msti[instIndex].mstiRootTimes.remHops)
            )
           )
        {
          p->portInstInfo[instIndex].updtInfo = L7_TRUE;
        }
      }
      else if (p->portInstInfo[DOT1S_CIST_INDEX].selectedRole == ROLE_ALTERNATE)
      {
        p->portInstInfo[instIndex].selectedRole = ROLE_ALTERNATE;   /*13.26.26 h*/
        /*if (DOT1S_PROTO_INST_DEBUG(instIndex) == L7_TRUE)
        {
             SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Port(%d) Inst(%d) :Selected Role = ALTERNATE\n",p->portNum,instIndex);
        } */


        if (((dot1sIdNotEqual(p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId,
                              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgBridgeId)) ||
             (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgPortId !=
              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgPortId) ||
             (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.intRootPathCost !=
              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.intRootPathCost) ||
             dot1sIdNotEqual(p->portInstInfo[instIndex].inst.msti.mstiPortPriority.regRootId,
                             p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.regRootId) ||
             (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.rxPortId !=
              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.rxPortId)
            ) ||
            ((p->portInstInfo[instIndex].inst.msti.mstiPortTimes.remHops !=
              dot1sInstance->msti[instIndex].mstiRootTimes.remHops)
            )
           )
        {
          p->portInstInfo[instIndex].updtInfo = L7_TRUE;
        }
      }/* else if (p->portInstInfo[DOT1S_CIST_INDEX].selectedRole == ROLE_ALTERNATE) */

    }
    else
    {


      if (p->portInstInfo[instIndex].infoIs == INFO_AGED)
      {
        p->portInstInfo[instIndex].updtInfo = L7_TRUE;
        p->portInstInfo[instIndex].selectedRole = ROLE_DESIGNATED; /*13.26.25 i*/
      }
      else if (p->portInstInfo[instIndex].infoIs == INFO_MINE)
      {
        p->portInstInfo[instIndex].selectedRole = ROLE_DESIGNATED;  /*13.26.25 j*/

        if (((dot1sIdNotEqual(p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId,
                              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgBridgeId)) ||
             (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgPortId !=
              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgPortId) ||
             (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.intRootPathCost !=
              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.intRootPathCost) ||
             dot1sIdNotEqual(p->portInstInfo[instIndex].inst.msti.mstiPortPriority.regRootId,
                             p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.regRootId) ||
             (p->portInstInfo[instIndex].inst.msti.mstiPortPriority.rxPortId !=
              p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.rxPortId)
            ) ||
            ((p->portInstInfo[instIndex].inst.msti.mstiPortTimes.remHops !=
              dot1sInstance->msti[instIndex].mstiRootTimes.remHops)
            )
           )
        {
          p->portInstInfo[instIndex].updtInfo = L7_TRUE;
        }
      }

      else if (p->portInstInfo[instIndex].infoIs == INFO_RECEIVED)
      {

        if (dot1sMstiRootPriVecDerived(&dot1sInstance->msti[instIndex].mstiRootPriority,
                                       &p->portInstInfo[instIndex].inst.msti.mstiPortPriority,
                                       p->portInstInfo[instIndex].InternalPortPathCost) == L7_TRUE)
        {
          p->portInstInfo[instIndex].selectedRole = ROLE_ROOT;  /*13.26.26 k*/
          p->portInstInfo[instIndex].updtInfo = L7_FALSE;
        }
        else
        {

          if (dot1sMstiPriVecAllCompare(&p->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority,
                                        &p->portInstInfo[instIndex].inst.msti.mstiPortPriority) != BETTER)
          {

            if (memcmp(&p->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId,
                       &dot1sInstance->msti[instIndex].BridgeIdentifier,
                       sizeof(DOT1S_BRIDGEID_t)) != 0)
            {
              p->portInstInfo[instIndex].selectedRole = ROLE_ALTERNATE;  /*13.26.26 l*/
              p->portInstInfo[instIndex].updtInfo = L7_FALSE;
            }
            else
            {
              p->portInstInfo[instIndex].selectedRole = ROLE_BACKUP;  /*13.26.26 m*/
              p->portInstInfo[instIndex].updtInfo = L7_FALSE;
            }
          }
          else
          {
            p->portInstInfo[instIndex].selectedRole = ROLE_DESIGNATED;  /*13.26.26 n*/
            p->portInstInfo[instIndex].updtInfo = L7_TRUE;
            p->portInstInfo[instIndex].agree = L7_FALSE;
            p->portInstInfo[instIndex].agreed = L7_FALSE;
          }

        } /* end else MstiRootPriVecDerived */

      } /* end else if ((p->portInstInfo[instIndex].infoIs == INFO_RECEIVED) */

    }/*end if else if ((p->portInstInfo[DOT1S_CIST_INDEX].infoIs == INFO_RECEIVED)*/

    p = dot1sIntfFindNext(p->portNum);
  }/* while */
  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Sets the selected to TRUE for this instance for all ports
*           in the bridge
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sSelectedTreeSet(L7_uint32 instIndex)
{
  DOT1S_PORT_COMMON_t   *p;

  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    p->portInstInfo[instIndex].selected = L7_TRUE;
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Clears the selected param for this instance for all
*           ports in the bridge
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sSelectedTreeClear(L7_uint32 instIndex)
{
  DOT1S_PORT_COMMON_t   *p;

  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    p->portInstInfo[instIndex].selected = L7_FALSE;
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  return L7_SUCCESS;
}
/**************************************************************************
 *******************************PRS State Machine Routines*****************
 **************************************************************************
 */
/*********************************************************************
* @purpose  Actions to be performed in the PRS state INIT
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrsInitAction(L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  rc = dot1sRolesDisabledTreeUpdt(instIndex);

  if (instIndex == DOT1S_CIST_INDEX)
  {
    dot1sInstance->cist.allSyncedRequested = 0;
    dot1sInstance->cist.syncedPending = 0;
    dot1sInstance->cist.reRootedRequested = 0;
    dot1sInstance->cist.reRootedPending = 0;
  }
  else
  {
    dot1sInstance->msti[instIndex].allSyncedRequested = 0;
    dot1sInstance->msti[instIndex].syncedPending = 0;
    dot1sInstance->msti[instIndex].reRootedRequested = 0;
    dot1sInstance->msti[instIndex].reRootedPending = 0;
  }

  if (instIndex == DOT1S_CIST_INDEX)
    dot1sInstance->cist.prsState = PRS_INIT;
  else
    dot1sInstance->msti[instIndex].prsState = PRS_INIT;


  syncInProgress[instIndex] = L7_FALSE;
  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRS state RECEIVE
*
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrsReceiveAction(L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  dot1sReselectTreeClear(instIndex);
  if (instIndex == DOT1S_CIST_INDEX)
    rc = dot1sRolesCistUpdt();

  else
    rc = dot1sRolesMstiUpdt(instIndex);

  dot1sSelectedTreeSet(instIndex);

  if (instIndex == DOT1S_CIST_INDEX)
    dot1sInstance->cist.prsState = PRS_RECEIVE;
  else
    dot1sInstance->msti[instIndex].prsState = PRS_RECEIVE;

  return rc;
}

/*********************************************************************
* @purpose  This routine claculates the next state fo the PRS state machine
*           and executes the action for that next state
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrsMachine(L7_uint32 dot1sEvent, L7_uint32 instIndex)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_prs_states_t  nextState;
  L7_uint32       normalizedEvent;

  normalizedEvent = dot1sEvent - dot1sPimEvents - 1;



  if (instIndex == DOT1S_CIST_INDEX)
    nextState = prsStateTable[normalizedEvent][dot1sInstance->cist.prsState];
  else
    nextState = prsStateTable[normalizedEvent][dot1sInstance->msti[instIndex].prsState];

  switch (nextState)
  {
    case PRS_INIT:
      dot1sPrsInitAction(instIndex);
      /*UCT to Receive State*/
      dot1sPrsReceiveAction(instIndex);
      rc = dot1sPrsGenerateEvents(instIndex);
      break;
    case PRS_RECEIVE:
      dot1sPrsReceiveAction(instIndex);
      rc = dot1sPrsGenerateEvents(instIndex);
      break;
    default:
      break;
  }/*end switch (nextState)*/
  return rc;
}
/*********************************************************************
* @purpose  Events that the port role selection machine needs to generate and propagate
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrsGenerateEvents(L7_uint32 instIndex)
{
  L7_RC_t         rc;
  DOT1S_PORT_COMMON_t   *p;
  L7_uint32       id;

  rc = dot1sInstNumFind(instIndex, &id);

  /*
    The logic to process the root port first was removed from here
    as it created a sequencing issue for ANVL 18.1.
    refer to notes on defect 18150
    IEEE 802.1Q-2003 does not specify the order in which ports should be processed.
   */

  /***********************************/
  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    /**************************
     * generate events for pim
     *************************
     */
    /*Send Event E2*/
    /*rc = dot1sIssueCmd(pimUpdtInfo, p->portNum, id, L7_NULLPTR);*/
    if (p->portInstInfo[instIndex].updtInfo == L7_TRUE &&
        p->portInstInfo[instIndex].selected == L7_TRUE)
    {
      /*Send Event E9*/
      /*rc = dot1sIssueCmd(pimSelectedUpdtInfo, p->portNum, id, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(pimSelectedUpdtInfo, p, instIndex, L7_NULL, L7_NULLPTR);
    }
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    /**************************
     * generate events for prt
     **************************
     */
    if (p->portInstInfo[instIndex].selected == L7_TRUE &&
        p->portInstInfo[instIndex].updtInfo == L7_FALSE)
    {
      if (p->portInstInfo[instIndex].role != p->portInstInfo[instIndex].selectedRole)
      {
        if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_ROLE,instIndex))
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Port (%d) inst(%d) role changing from %s to %s \n",__FUNCTION__,
                        p->portNum, instIndex, dot1sRoleStringGet(p->portInstInfo[instIndex].role),
                        dot1sRoleStringGet(p->portInstInfo[instIndex].selectedRole));
        }

        if ((p->portInstInfo[instIndex].role == ROLE_ROOT) ||
        (p->portInstInfo[instIndex].role == ROLE_DESIGNATED))
        {
          p->pduTxStopCheck = L7_TRUE;
        }

        switch (p->portInstInfo[instIndex].selectedRole)
        {
          case ROLE_DISABLED:
            /*Send Event E14*/
            /*rc = dot1sIssueCmd(prtDisabledPortRoleNotEqualSelectedRole, p->portNum, id, L7_NULLPTR);*/
            rc = dot1sStateMachineClassifier(prtDisabledPortRoleNotEqualSelectedRole, p, instIndex, L7_NULL, L7_NULLPTR);
            break;
          case ROLE_ALTERNATE:
            /*Send Event E15*/
            /*rc = dot1sIssueCmd(prtAlternatePortRoleNotEqualSelectedRole, p->portNum, id, L7_NULLPTR);*/
            rc = dot1sStateMachineClassifier(prtAlternatePortRoleNotEqualSelectedRole, p, instIndex, L7_NULL, L7_NULLPTR);
            break;
          case ROLE_BACKUP:
            /*Send Event E16*/
            /*rc = dot1sIssueCmd(prtBackupPortRoleNotEqualSelectedRole, p->portNum, id, L7_NULLPTR);*/

            rc = dot1sStateMachineClassifier(prtBackupPortRoleNotEqualSelectedRole, p, instIndex, L7_NULL, L7_NULLPTR);
            break;
          case ROLE_ROOT:
            /*send event E23*/
            /*rc = dot1sIssueCmd(prtRootPortRoleNotEqualSelectedRole, p->portNum, id, L7_NULLPTR);*/

            rc = dot1sStateMachineClassifier(prtRootPortRoleNotEqualSelectedRole, p, instIndex, L7_NULL, L7_NULLPTR);
            break;
          case ROLE_DESIGNATED:
            /*send event E24*/
            /*rc = dot1sIssueCmd(prtDesigPortRoleNotEqualSelectedRole, p->portNum, id, L7_NULLPTR);*/

            rc = dot1sStateMachineClassifier(prtDesigPortRoleNotEqualSelectedRole, p, instIndex, L7_NULL, L7_NULLPTR);
            break;
          case ROLE_MASTER:
            /*send event E25*/
            /*rc = dot1sIssueCmd(prtMasterPortRoleNotEqualSelectedRole, p->portNum, id, L7_NULLPTR);*/
            rc = dot1sStateMachineClassifier(prtMasterPortRoleNotEqualSelectedRole, p, instIndex, L7_NULL, L7_NULLPTR);
            break;
          default:
            break;
        }/*end switch (p->portInstInfo[instIndex].selectedRole)*/
      }/*end if (p->portInstInfo[instIndex].role !=....*/
    }/*end if (p->portInstInfo[instIndex].selected == L7_TRUE....*/
    else
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Port %d inst %d Not setting the selected role as selected and updt info is true \n",
                    p->portNum, instIndex);
    }

    if (p->pduTxStopCheck == L7_TRUE)
    {
      dot1sNeedForBpduTxStopCheck(p);
    }
    if (instIndex != DOT1S_CIST_INDEX &&
        p->portInstInfo[instIndex].changedMaster == L7_TRUE &&
        p->rcvdInternal == L7_TRUE )
    {
      if (dot1s_enable_debug_detail)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Calling prtgenrate events as Master has changed \n");
      }

      p->portInstInfo[instIndex].changedMaster = L7_FALSE;
      dot1sPrtGenerateEvents(p,instIndex);
    }

    p = dot1sIntfFindNext(p->portNum);
  }/* while */
  return L7_SUCCESS;
}
/**************************************************************************
 ******************************PRT Support Routines************************
 **************************************************************************
 */
/*********************************************************************
* @purpose  Sets the reRoot to TRUE for this instance for all ports in
*           the bridge
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sReRootTreeSet(L7_uint32 instIndex)
{
  DOT1S_PORT_COMMON_t   *p;
  L7_RC_t         rc = L7_SUCCESS;

  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    if ( p->portEnabled == L7_TRUE /*&&
        p->portLinkState == L7_UP*/)
    {
      p->portInstInfo[instIndex].reRoot = L7_TRUE;
    }
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  return rc;
}

/*********************************************************************
* @purpose  Sets the sync to TRUE for this instance for all ports in
*           the bridge
*
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sSyncTreeSet(L7_uint32 instIndex)
{
  DOT1S_PORT_COMMON_t   *p;
  L7_RC_t         rc = L7_SUCCESS;

if (syncInProgress[instIndex] == L7_TRUE)
  {
    /* If a sync is already in progress for this instance return with a L7_SUCCESS*/
    return L7_SUCCESS;
  }

p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    if (p->portEnabled == L7_TRUE &&
        p->portLinkState == L7_UP)
    {
      p->portInstInfo[instIndex].sync = L7_TRUE;
      p->portInstInfo[instIndex].synced = L7_FALSE;
      p->portInstInfo[instIndex].agreed = L7_FALSE;

    }
#if 0
    if (instIndex == DOT1S_CIST_INDEX &&
        p->rcvdInternal == L7_FALSE)
    {


      for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
      {
        if (dot1sInstanceMap[j].inUse == L7_TRUE)
        {
          p->portInstInfo[j].sync = L7_TRUE;
          p->portInstInfo[j].synced = p->portInstInfo[DOT1S_CIST_INDEX].synced;
          p->portInstInfo[j].agreed = L7_FALSE;

        }

      }

    }

#endif
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

syncInProgress[instIndex] = L7_TRUE;
  /* Generate Events for PRT State Machine
 * for this port in this instance. Make this a seperate loop to ensure all the sync are set to true and
 * synced variable for the port reflects the latest synced.
*/
  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    if (p->portEnabled == L7_TRUE &&
        p->portLinkState == L7_UP)
    {
      /* dot1sSyncEventsGenerate(p,instIndex);*/
      dot1sGenerateEventsForPrt(p,instIndex);

#if 0
      if (instIndex == DOT1S_CIST_INDEX &&
          p->rcvdInternal == L7_FALSE)
      {
        for (j = 1; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
        {
          if (dot1sInstanceMap[j].inUse == L7_TRUE)
          {
            /*dot1sSyncEventsGenerate(p,j);*/
            dot1sGenerateEventsForPrt(p,j);
          }
        }

      }/* if instIndex ==DOT1S_CIST_INDEX */

#endif

    } /* if port enabled and linkedup */
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  return rc;
}
/***************************************************************************
 *******************************PRT State Machine Routines******************
 ***************************************************************************
 */

/*********************************************************************
* @purpose  Actions to be performed in the PRT state INIT
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtInitPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t       rc = L7_FAILURE;
  DOT1S_BRIDGE_t  *bridge;

  p->portInstInfo[instIndex].role = ROLE_DISABLED;
  /*p->portInstInfo[instIndex].selectedRole = ROLE_DISABLED;*/
  p->portInstInfo[instIndex].learn = L7_FALSE;
  p->portInstInfo[instIndex].forward = L7_FALSE;
  p->portInstInfo[instIndex].synced = L7_FALSE;
  p->portInstInfo[instIndex].sync = L7_TRUE;

  p->portInstInfo[instIndex].reRoot = L7_TRUE;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
    return rc;

  p->portInstInfo[instIndex].rrWhile = bridge->FwdDelay;
  p->portInstInfo[instIndex].fdWhile = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.maxAge;

  p->portInstInfo[instIndex].rbWhile = 0;

  p->portInstInfo[instIndex].prtState = PRT_INIT_PORT;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state BACKUP
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtBackupPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].rbWhile = 2*(dot1sInstance->cist.CistBridgeTimes.bridgeHelloTime);

  p->portInstInfo[instIndex].prtState = PRT_BACKUP_PORT;

  rc = L7_SUCCESS;

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PRT state BLOCKED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDisablePortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{


  p->portInstInfo[instIndex].role = p->portInstInfo[instIndex].selectedRole;
  L7_INTF_SETMASKBIT(portInstInfoChangeMask[instIndex], p->portNum);
  p->portInstInfo[instIndex].learn = L7_FALSE;
  p->portInstInfo[instIndex].forward = L7_FALSE;

  p->pduTxStopCheck = L7_TRUE;
  p->portInstInfo[instIndex].prtState = PRT_DISABLE_PORT;

  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Actions to be performed in the PRT state BLOCKED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtBlockPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].role = p->portInstInfo[instIndex].selectedRole;
  p->portInstInfo[instIndex].learn = L7_FALSE;
  p->portInstInfo[instIndex].forward = L7_FALSE;

  p->portInstInfo[instIndex].prtState = PRT_BLOCK_PORT;
  p->pduTxStopCheck = L7_TRUE;

  L7_INTF_SETMASKBIT(portInstInfoChangeMask[instIndex], p->portNum);
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state ACTIVE
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDisabledPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].fdWhile = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.maxAge;
  p->portInstInfo[instIndex].synced = L7_TRUE;
  p->portInstInfo[instIndex].rrWhile = 0;
  p->portInstInfo[instIndex].sync = L7_FALSE;
  p->portInstInfo[instIndex].reRoot = L7_FALSE;

  p->portInstInfo[instIndex].prtState = PRT_DISABLED_PORT;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state ACTIVE
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtMasterPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;


  p->portInstInfo[instIndex].role = ROLE_MASTER;

  L7_INTF_SETMASKBIT(portInstInfoChangeMask[instIndex], p->portNum);
  /* rc = dot1sMstiMasterFlagCompute(p, instIndex);  QREV TBD */
  /*Fix for MSTP ANVL 1.13*/

  p->portInstInfo[instIndex].prtState = PRT_MASTER_PORT;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state ACTIVE
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDesigPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{


  p->portInstInfo[instIndex].role = ROLE_DESIGNATED;

  L7_INTF_SETMASKBIT(portInstInfoChangeMask[instIndex], p->portNum);
  /*rc = dot1sMstiMasterFlagCompute(p, instIndex); *//*Fix for MSTP ANVL 1.13*/

  p->portInstInfo[instIndex].prtState = PRT_DESIG_PORT;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state ACTIVE
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtRootPortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;


  p->portInstInfo[instIndex].role = p->portInstInfo[instIndex].selectedRole;

  L7_INTF_SETMASKBIT(portInstInfoChangeMask[instIndex], p->portNum);
  p->portInstInfo[instIndex].rrWhile = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.fwdDelay;
  p->portInstInfo[instIndex].prtState = PRT_ROOT_PORT;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state ACTIVE
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtAlternatePortAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  DOT1S_BRIDGE_t  *bridge;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
    return L7_FAILURE;

  p->portInstInfo[instIndex].fdWhile = forwardDelayGet(p);
  p->portInstInfo[instIndex].synced = L7_TRUE;
  p->portInstInfo[instIndex].rrWhile = 0;
  p->portInstInfo[instIndex].sync = L7_FALSE;
  p->portInstInfo[instIndex].reRoot = L7_FALSE;

  p->portInstInfo[instIndex].prtState = PRT_ALTERNATE_PORT ;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state REROOTED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1sPrtMasterRetiredAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].reRoot = L7_FALSE;

  p->portInstInfo[instIndex].prtState = PRT_MASTER_RETIRED;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state REROOTED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1sPrtDesigRetiredAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].reRoot = L7_FALSE;
  p->portInstInfo[instIndex].prtState = PRT_DESIG_RETIRED;


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state REROOTED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1sPrtRerootedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].reRoot = L7_FALSE;

  p->portInstInfo[instIndex].prtState = PRT_REROOTED;

  rc = L7_SUCCESS;

  return rc;
}


/*********************************************************************
* @purpose  Actions to be performed in the PRT state LISTEN
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtMasterDiscardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].learn = L7_FALSE;
  p->portInstInfo[instIndex].forward = L7_FALSE;
  p->portInstInfo[instIndex].fdWhile = forwardDelayGet(p);
  p->portInstInfo[instIndex].disputed = L7_FALSE;
  p->portInstInfo[instIndex].prtState = PRT_MASTER_DISCARD;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state LISTEN
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDesigDiscardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].learn = L7_FALSE;
  p->portInstInfo[instIndex].forward = L7_FALSE;
  p->portInstInfo[instIndex].fdWhile = forwardDelayGet(p);
  p->portInstInfo[instIndex].disputed = L7_FALSE;
  p->portInstInfo[instIndex].prtState = PRT_DESIG_DISCARD;
  return L7_SUCCESS;


}
/*********************************************************************
* @purpose  Actions to be performed in the PRT state LEARN
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtMasterLearnAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  DOT1S_BRIDGE_t  *bridge;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
    return L7_FAILURE;

  p->portInstInfo[instIndex].learn = L7_TRUE;
  p->portInstInfo[instIndex].fdWhile = forwardDelayGet(p);

  p->portInstInfo[instIndex].prtState = PRT_MASTER_LEARN;



  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state LEARN
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDesigLearnAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].learn = L7_TRUE;
  p->portInstInfo[instIndex].fdWhile = forwardDelayGet(p);

  p->portInstInfo[instIndex].prtState = PRT_DESIG_LEARN;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state LEARN
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtRootLearnAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].learn = L7_TRUE;
  p->portInstInfo[instIndex].fdWhile = forwardDelayGet(p);

  p->portInstInfo[instIndex].prtState = PRT_ROOT_LEARN;


  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Actions to be performed in the PRT state FORWARD
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtMasterForwardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].forward = L7_TRUE;
  p->portInstInfo[instIndex].fdWhile = 0;

  p->portInstInfo[instIndex].agreed = p->sendRSTP;


  p->portInstInfo[instIndex].prtState = PRT_MASTER_FORWARD;


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state FORWARD
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDesigForwardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].forward = L7_TRUE;
  p->portInstInfo[instIndex].fdWhile = 0;
  p->portInstInfo[instIndex].agreed = p->sendRSTP;

  p->portInstInfo[instIndex].prtState = PRT_DESIG_FORWARD;


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state FORWARD
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtRootForwardAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].forward = L7_TRUE;
  p->portInstInfo[instIndex].fdWhile = 0;

  p->portInstInfo[instIndex].prtState = PRT_ROOT_FORWARD;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state REROOT
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtRerootAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  rc = dot1sReRootTreeSet(instIndex);

  p->portInstInfo[instIndex].prtState = PRT_REROOT;

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PRT state SYNCED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1sPrtMasterSyncedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].rrWhile = 0;
  p->portInstInfo[instIndex].synced = L7_TRUE;
  p->portInstInfo[instIndex].sync = L7_FALSE;

  p->portInstInfo[instIndex].prtState = PRT_MASTER_SYNCED;
  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state SYNCED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1sPrtDesigSyncedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  p->portInstInfo[instIndex].rrWhile = 0;
  p->portInstInfo[instIndex].synced = L7_TRUE;
  p->portInstInfo[instIndex].sync = L7_FALSE;

  p->portInstInfo[instIndex].prtState = PRT_DESIG_SYNCED;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state SYNCED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1sPrtRootSyncedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{


  p->portInstInfo[instIndex].synced = L7_TRUE;
  p->portInstInfo[instIndex].sync = L7_FALSE;

  p->portInstInfo[instIndex].prtState = PRT_ROOT_SYNCED;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state AGREES
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtMasterAgreedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) instindex(%d) Agreeing \n", __FUNCTION__,
                  p->portNum, instIndex);
  }
  p->portInstInfo[instIndex].proposed = L7_FALSE;
  p->portInstInfo[instIndex].agree = L7_TRUE;
  p->portInstInfo[instIndex].sync = L7_FALSE;
  /* New Info is not set to true per standard : Not Sure abt that */

  p->portInstInfo[instIndex].prtState = PRT_MASTER_AGREED;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state AGREES
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDesigAgreedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) instindex(%d) Agreeing \n", __FUNCTION__,
                  p->portNum, instIndex);
  }
  p->portInstInfo[instIndex].proposed = L7_FALSE;
  p->portInstInfo[instIndex].sync = L7_FALSE;
  p->portInstInfo[instIndex].agree = L7_TRUE;
  if (instIndex == DOT1S_CIST_INDEX)
    p->newInfoCist = L7_TRUE;
  else
    p->newInfoMist = L7_TRUE;

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }
  p->portInstInfo[instIndex].prtState = PRT_DESIG_AGREED;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state AGREES
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtRootAgreedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) instindex(%d) Agreeing \n", __FUNCTION__,
                  p->portNum, instIndex);
  }
  p->portInstInfo[instIndex].proposed = L7_FALSE;
  p->portInstInfo[instIndex].agree = L7_TRUE;
  p->portInstInfo[instIndex].sync = L7_FALSE;

  if (instIndex == DOT1S_CIST_INDEX)
    p->newInfoCist = L7_TRUE;
  else
    p->newInfoMist = L7_TRUE;

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }

  p->portInstInfo[instIndex].prtState = PRT_ROOT_AGREED;

  rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state AGREES
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtAlternateAgreedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) instindex(%d) Agreeing \n", __FUNCTION__,
                  p->portNum, instIndex);
  }
  p->portInstInfo[instIndex].proposed = L7_FALSE;
  p->portInstInfo[instIndex].agree = L7_TRUE;
  if (instIndex == DOT1S_CIST_INDEX)
    p->newInfoCist = L7_TRUE;
  else
    p->newInfoMist = L7_TRUE;

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }
  p->portInstInfo[instIndex].prtState = PRT_ALTERNATE_AGREED;

  rc = L7_SUCCESS;

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PRT state PROPOSING
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDesigProposeAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;
  DOT1S_BRIDGE_t  *bridge;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
    return L7_FAILURE;


  p->portInstInfo[instIndex].proposing = L7_TRUE;
  if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE, DOT1S_CIST_INDEX))
  {
    DOT1S_DEBUG_MSG("%s:Port %d CIST Proposing at %d\n", __FUNCTION__,p->portNum, osapiUpTimeRaw());
  }

  if (instIndex == DOT1S_CIST_INDEX)
  {
    p->edgeDelayWhile = (pointTopointCheck == L7_TRUE) ? bridge->MigrateTime : dot1sInstance->cist.CistBridgeTimes.bridgeMaxAge;
    p->newInfoCist = L7_TRUE;
  }
  else
  {
    p->newInfoMist = L7_TRUE;
  }

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }
  p->portInstInfo[instIndex].prtState = PRT_DESIG_PROPOSE;
  rc = dot1sPtxGenerateEvents(p);

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PRT state PROPOSED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtMasterProposedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;


  if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_HANDSHAKE, instIndex))
  {
    DOT1S_DEBUG_MSG("%s:Port %d inst(%d) Proposing at %d\n", __FUNCTION__,p->portNum,instIndex, osapiUpTimeRaw());
  }

  p->portInstInfo[instIndex].proposed = L7_FALSE;
  p->portInstInfo[instIndex].prtState = PRT_MASTER_PROPOSED;


  rc = dot1sSyncTreeSet(instIndex);


  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state PROPOSED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtAlternateProposedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].proposed = L7_FALSE;
  p->portInstInfo[instIndex].prtState = PRT_ALTERNATE_PROPOSED;

  if (dot1s_enable_debug_detail)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: calling dot1sSyncTreeSet \n", __FUNCTION__);
  }


  rc = dot1sSyncTreeSet(instIndex);


  return rc;
}

/*********************************************************************
* @purpose  Actions to be performed in the PRT state PROPOSED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtRootProposedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].proposed = L7_FALSE;
  p->portInstInfo[instIndex].prtState = PRT_ROOT_PROPOSED;

  if (dot1s_enable_debug_detail)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: calling dot1sSyncTreeSet \n", __FUNCTION__);
  }


  rc = dot1sSyncTreeSet(instIndex);


  return rc;
}
/*********************************************************************
* @purpose  This routine claculates the next state fo the PRT state machine
*           and executes the action for that next state
*
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDisabledMachine(L7_uint32 dot1sEvent,
                                DOT1S_PORT_COMMON_t *p,
                                L7_uint32 instIndex)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_prt_disabledStates_t  nextState;
  L7_uint32       normalizedEvent;
  L7_uint32             normalizedState = PRT_INIT_PORT, currState;

  normalizedEvent = dot1sEvent - dot1sPrsEvents - 1;
  currState = p->portInstInfo[instIndex].prtState ;

  if (currState > PRT_INIT_PORT && currState < PRT_DISABLED_STATES  )
  {
    normalizedState =  currState;
  }

  nextState = prtDisabledStateTable[normalizedEvent][normalizedState];

  if (DOT1S_DEBUG(DOT1S_DEBUG_STATE_MACHINE,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: curr state %d dot1sEvent %d normalized event %d next state %d \n",__FUNCTION__,
                  currState, dot1sEvent, normalizedEvent, nextState);
  }


  switch (nextState)
  {
    case PRT_INIT_PORT:
      rc = dot1sPrtInitPortAction(p, instIndex);
      /* No break -fall through UCT */
    case PRT_DISABLE_PORT:
      rc = dot1sPrtDisablePortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_DISABLED_PORT:
      rc = dot1sPrtDisabledPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;

    default:
      break;
  }/*end switch (nextState)*/

  return rc;
}

/*********************************************************************
* @purpose  This routine claculates the next state fo the PRT state machine
*           and executes the action for that next state
*
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtMasterMachine(L7_uint32 dot1sEvent,
                              DOT1S_PORT_COMMON_t *p,
                              L7_uint32 instIndex)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_prt_masterStates_t  nextState;
  L7_uint32             normalizedState = 0, currState;
  L7_uint32       normalizedEvent;

  if (DOT1S_DEBUG(DOT1S_DEBUG_STATE_MACHINE,DOT1S_CIST_INDEX))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Enter ",__FUNCTION__);
  }

  normalizedEvent = dot1sEvent - dot1sPrtDisabledEvents - 1;
  currState = p->portInstInfo[instIndex].prtState ;

  if (DOT1S_DEBUG(DOT1S_DEBUG_STATE_MACHINE,DOT1S_CIST_INDEX))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Port(%d) inst %d reroot (%d) \n", p->portNum, instIndex, p->portInstInfo[instIndex].reRoot);
  }

  if (currState > PRT_MASTER_FIRST && currState < PRT_MASTER_STATES  )
  {
    normalizedState =  currState - PRT_MASTER_FIRST - 1;
  }
  nextState = prtMasterStateTable[normalizedEvent][normalizedState];

  if (DOT1S_DEBUG(DOT1S_DEBUG_STATE_MACHINE,DOT1S_CIST_INDEX))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: curr state %d dot1sEvent %d normalized event %d  normalizedState %d next state %d \n",__FUNCTION__,
                  p->portInstInfo[instIndex].prtState, dot1sEvent, normalizedEvent, normalizedState, nextState);
  }

  switch (nextState)
  {
    case PRT_MASTER_PROPOSED:
      rc = dot1sPrtMasterProposedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtMasterPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_MASTER_AGREED:
      rc = dot1sPrtMasterAgreedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtMasterPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_MASTER_SYNCED:
      rc = dot1sPrtMasterSyncedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtMasterPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_MASTER_RETIRED:
      rc = dot1sPrtMasterRetiredAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtMasterPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_MASTER_PORT:
      rc = dot1sPrtMasterPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_MASTER_DISCARD:
      rc = dot1sPrtMasterDiscardAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtMasterPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_MASTER_LEARN:
      rc = dot1sPrtMasterLearnAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtMasterPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_MASTER_FORWARD:
      rc = dot1sPrtMasterForwardAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtMasterPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    default:
      break;
  }/*end switch (nextState)*/

  if (DOT1S_DEBUG(DOT1S_DEBUG_STATE_MACHINE,DOT1S_CIST_INDEX))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Exit \n",__FUNCTION__);
  }

  return rc;
}

/*********************************************************************
* @purpose  This routine claculates the next state fo the PRT state machine
*           and executes the action for that next state
*
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtDesigMachine(L7_uint32 dot1sEvent,
                             DOT1S_PORT_COMMON_t *p,
                             L7_uint32 instIndex)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_prt_desigStates_t   nextState;
  L7_uint32             normalizedState = 0, currState ;
  L7_uint32       normalizedEvent;



  normalizedEvent = dot1sEvent - dot1sPrtMasterEvents - 1;


  currState = p->portInstInfo[instIndex].prtState ;

  if (currState > PRT_DESIG_FIRST && currState < PRT_DESIG_STATES  )
  {
    normalizedState =  currState - PRT_DESIG_FIRST - 1;
  }

  nextState = prtDesigStateTable[normalizedEvent][normalizedState];

  if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port %d instIndex %d curr state %d dot1sEvent %d normalized event %d  normalizedState %d next state %d \n",__FUNCTION__,
                  p->portNum, instIndex, p->portInstInfo[instIndex].prtState, dot1sEvent, normalizedEvent, normalizedState, nextState);

  }

  switch (nextState)
  {
    case PRT_DESIG_PROPOSE:
      rc = dot1sPrtDesigProposeAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtDesigPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_DESIG_AGREED:
      rc = dot1sPrtDesigAgreedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtDesigPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_DESIG_SYNCED:
      rc = dot1sPrtDesigSyncedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtDesigPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_DESIG_RETIRED:
      rc = dot1sPrtDesigRetiredAction(p, instIndex);
      /*No break -Fall Thru UCT to Active State*/
    case PRT_DESIG_PORT:
      rc = dot1sPrtDesigPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_DESIG_DISCARD:
      rc = dot1sPrtDesigDiscardAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtDesigPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_DESIG_LEARN:
      rc = dot1sPrtDesigLearnAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtDesigPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_DESIG_FORWARD:
      rc = dot1sPrtDesigForwardAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtDesigPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    default:
      break;
  }/*end switch (nextState)*/

  return rc;
}

/*********************************************************************
* @purpose  This routine claculates the next state fo the PRT state machine
*           and executes the action for that next state
*
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtRootMachine(L7_uint32 dot1sEvent,
                            DOT1S_PORT_COMMON_t *p,
                            L7_uint32 instIndex)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_prt_rootStates_t  nextState;
  L7_uint32             normalizedState = 0, currState;
  L7_uint32       normalizedEvent;



  normalizedEvent = dot1sEvent - dot1sPrtDesigEvents - 1;


  currState = p->portInstInfo[instIndex].prtState ;
  if (currState > PRT_ROOT_FIRST && currState < PRT_ROOT_STATES  )
  {
    normalizedState =  currState - PRT_ROOT_FIRST - 1;
  }


  nextState = prtRootStateTable[normalizedEvent][normalizedState];
  if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: curr state %d dot1sEvent %d normalized event %d  normalizedState %d next state %d \n",__FUNCTION__,
                  p->portInstInfo[instIndex].prtState, dot1sEvent, normalizedEvent, normalizedState, nextState);
  }


  switch (nextState)
  {
    case PRT_ROOT_PROPOSED:
      rc = dot1sPrtRootProposedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtRootPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_ROOT_AGREED:
      rc = dot1sPrtRootAgreedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtRootPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_ROOT_SYNCED:
      rc = dot1sPrtRootSyncedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtRootPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_REROOT:
      rc = dot1sPrtRerootAction(p, instIndex);
      /* NO break - Fall Thru : UCT to Active State*/
    case PRT_ROOT_PORT:
      rc = dot1sPrtRootPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_REROOTED:
      rc = dot1sPrtRerootedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtRootPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_ROOT_LEARN:
      rc = dot1sPrtRootLearnAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtRootPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_ROOT_FORWARD:
      rc = dot1sPrtRootForwardAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sPrtRootPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    default:
      break;
  }/*end switch (nextState)*/

  if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s(Exit): Port(%d) Inst (%d) curr state %d  \n",__FUNCTION__,
                  p->portNum, instIndex, p->portInstInfo[instIndex].prtState);
  }


  return rc;
}

/*********************************************************************
* @purpose  This routine claculates the next state fo the PRT state machine
*           and executes the action for that next state
*
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtAltBkupMachine(L7_uint32 dot1sEvent,
                               DOT1S_PORT_COMMON_t *p,
                               L7_uint32 instIndex)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_prt_altBkupStates_t   nextState;
  L7_uint32             normalizedState = 0, currState;
  L7_uint32       normalizedEvent;



  normalizedEvent = dot1sEvent - dot1sPrtRootEvents - 1;

  currState = p->portInstInfo[instIndex].prtState ;
  if (currState > PRT_ALTBKUP_FIRST && currState < PRT_ALTBKUP_STATES  )
  {
    normalizedState = currState - PRT_ALTBKUP_FIRST - 1;
  }



  nextState = prtAltBkupStateTable[normalizedEvent][normalizedState];

  if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: curr state %d dot1sEvent %d normalized event %d  normalizedState %d next state %d \n",__FUNCTION__,
                  p->portInstInfo[instIndex].prtState, dot1sEvent, normalizedEvent, normalizedState, nextState);
  }


  switch (nextState)
  {
    case PRT_BLOCK_PORT:
      rc = dot1sPrtBlockPortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_BACKUP_PORT:
      rc = dot1sPrtBackupPortAction(p, instIndex);
      /* No break -Fall thru UCT to Alternate_port */
    case PRT_ALTERNATE_PORT:
      rc = dot1sPrtAlternatePortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_ALTERNATE_PROPOSED:
      rc = dot1sPrtAlternateProposedAction(p, instIndex);
      /*UCT to Alternate_port*/
      rc = dot1sPrtAlternatePortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    case PRT_ALTERNATE_AGREED:
      rc = dot1sPrtAlternateAgreedAction(p, instIndex);
      /*UCT to Alternate_port State*/
      rc = dot1sPrtAlternatePortAction(p, instIndex);
      rc = dot1sPrtGenerateEvents(p,instIndex);
      break;
    default:
      break;
  }/*end switch (nextState)*/

  return rc;
}




/*********************************************************************
* @purpose  This routine claculates the next state fo the PRT state machine
*           and executes the action for that next state
*
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtMachine(L7_uint32 dot1sEvent,
                        DOT1S_PORT_COMMON_t *p,
                        L7_uint32 instIndex)
{
  L7_RC_t         rc = L7_FAILURE;

  /* The switch is on the selected role to generate the selectedRole Not
   * equal to the current role event. In all other cases obviously selected role is
   * going to be equal to the current role */
  switch (p->portInstInfo[instIndex].selectedRole)
  {
    case  ROLE_DISABLED:
      rc = dot1sPrtDisabledMachine(dot1sEvent,p, instIndex);
      break;
    case  ROLE_ROOT:
      rc = dot1sPrtRootMachine(dot1sEvent,p, instIndex);
      break;
    case ROLE_DESIGNATED:
      rc = dot1sPrtDesigMachine(dot1sEvent,p, instIndex);
      break;
    case ROLE_ALTERNATE:
    case ROLE_BACKUP:
      rc = dot1sPrtAltBkupMachine(dot1sEvent,p, instIndex);
      break;
    case ROLE_MASTER:
      rc = dot1sPrtMasterMachine(dot1sEvent,p, instIndex);
      break;
    default:
      break;


  }


  return rc;
}
/*********************************************************************
* @purpose  Events that the port role transition machine needs to generate and propagate
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPrtGenerateEvents(DOT1S_PORT_COMMON_t *p,
                               L7_uint32 instIndex)
{
  L7_RC_t       rc;
  L7_uint32     id;
  DOT1S_BRIDGE_t  *bridge;

  bridge = dot1sBridgeParmsFind();
  rc = dot1sInstNumFind(instIndex, &id);
  /***************************/
  /*generate events for pst*/
  /***************************/
  if (p->portInstInfo[instIndex].learn == L7_TRUE &&
      p->portInstInfo[instIndex].pstState == PST_DISCARDING)
  {
    /*Send Event E43*/
    /*rc = dot1sIssueCmd(pstLearn, p->portNum, id, L7_NULLPTR);*/
    rc = dot1sStateMachineClassifier(pstLearn, p, instIndex, L7_NULL, L7_NULLPTR);
  }
  else if (p->portInstInfo[instIndex].learn == L7_FALSE &&
           p->portInstInfo[instIndex].pstState == PST_LEARNING)
  {
    /*Send Event E44*/
    /*rc = dot1sIssueCmd(pstNotLearn, p->portNum, id, L7_NULLPTR);*/
    rc = dot1sStateMachineClassifier(pstNotLearn, p, instIndex, L7_NULL, L7_NULLPTR);
  }
  if (p->portInstInfo[instIndex].forward == L7_TRUE &&
      p->portInstInfo[instIndex].pstState == PST_LEARNING)
  {
    /*Send Event E45*/
    /*rc = dot1sIssueCmd(pstForward, p->portNum, id, L7_NULLPTR);*/
    rc = dot1sStateMachineClassifier(pstForward, p, instIndex, L7_NULL, L7_NULLPTR);
  }
  else if (p->portInstInfo[instIndex].forward == L7_FALSE &&
           p->portInstInfo[instIndex].pstState == PST_FORWARDING)
  {
    /*send event E46*/
    /*rc = dot1sIssueCmd(pstNotForward, p->portNum, id, L7_NULLPTR);*/
    rc = dot1sStateMachineClassifier(pstNotForward, p, instIndex, L7_NULL, L7_NULLPTR);
  }




  dot1sGenerateEventsForPrt(p,instIndex);

  /***************************
   * generate events for tcm
   **************************
  */
  dot1sGenerateEventsForTcm(p,instIndex);


  /* See if there is any need to transmit BPDU */
  if (p->sendRSTP == L7_TRUE &&
      (p->newInfoCist == L7_TRUE ||
       p->newInfoMist == L7_TRUE) &&
      p->txCount < bridge->TxHoldCount && (p->helloWhen != 0) )
  {
    /*Send Event E66*/
    /*rc = dot1sIssueCmd(ptxSendRstp, p->portNum, L7_NULL, L7_NULLPTR);*/
    rc = dot1sStateMachineClassifier(ptxSendRstp, p, L7_NULL, L7_NULL, L7_NULLPTR);
  }


  return L7_SUCCESS;
}
/**************************************************************************
 *******************************PST Support Routines***********************
 **************************************************************************
 */

/*********************************************************************
* @purpose  Takes steps neccessary to disable the forwarding function
*           for this interface
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Routine does not complete until forwarding has been disabled
*
*
* @end
*********************************************************************/
L7_RC_t dot1sForwardingDisable(L7_uint32 intIfNum, L7_uint32 instIndex)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 mstID;

  if ((rc = dot1sInstNumFind(instIndex, &mstID)) == L7_SUCCESS)
  {
    dot1sIhSetPortState(mstID, intIfNum, L7_DOT1S_DISCARDING);
  }

  return rc;
}
/*********************************************************************
* @purpose  Takes steps neccessary to disable the learning function
*           for this interface
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Routine does not complete until learning has been disabled
*
*
* @end
*********************************************************************/
L7_RC_t dot1sLearningDisable(L7_uint32 intIfNum, L7_uint32 instIndex)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 mstID;

  if ((rc = dot1sInstNumFind(instIndex, &mstID)) == L7_SUCCESS)
  {
    dot1sIhSetPortState(mstID, intIfNum, L7_DOT1S_DISCARDING);
  }

  return rc;
}
/*********************************************************************
* @purpose  Takes steps neccessary to enable the forwarding function
*           for this interface
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Routine does not complete until forwarding has been enabled
*
*
* @end
*********************************************************************/
L7_RC_t dot1sForwardingEnable(L7_uint32 intIfNum, L7_uint32 instIndex)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 mstID;

  if ((rc = dot1sInstNumFind(instIndex, &mstID)) == L7_SUCCESS)
  {
    dot1sIhSetPortState(mstID, intIfNum, L7_DOT1S_FORWARDING);
  }

  return rc;
}
/*********************************************************************
* @purpose  Takes steps neccessary to enable the learning function
*           for this interface
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Routine does not complete until learning has been enabled
*
*
* @end
*********************************************************************/
L7_RC_t dot1sLearningEnable(L7_uint32 intIfNum, L7_uint32 instIndex)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 mstID;

  if ((rc = dot1sInstNumFind(instIndex, &mstID)) == L7_SUCCESS)
  {
    dot1sIhSetPortState(mstID, intIfNum, L7_DOT1S_LEARNING);
  }

  return rc;
}
/***************************************************************************
 *******************************PST State Machine Routines******************
 **************************************************************************
 */
/*********************************************************************
* @purpose  Actions to be performed in the PST state DISACRDING
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPstDiscardingAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  /* portState will be set after the driver returns with a success */
  rc = dot1sLearningDisable(p->portNum, instIndex);
  if (rc == L7_SUCCESS)
  {
    /* learning and forwarding will be set after the driver returns with a success */
    p->portInstInfo[instIndex].pstState = PST_DISCARDING;
  }
  trapMgrDot1dStpPortStateNotForwardingLogTrap(p->portNum, instIndex);

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PST state LEARNING
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPstLearningAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  /* portState will be set after the driver returns with a success */
  rc = dot1sLearningEnable(p->portNum, instIndex);
  if (rc == L7_SUCCESS)
  {
    /* learning will be set after the driver returns with a success */
    p->portInstInfo[instIndex].pstState = PST_LEARNING;
  }
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PST state FORWARDING
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1sPstForwardingAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  /* portState will be set after the driver returns with a success */
  rc = dot1sForwardingEnable(p->portNum, instIndex);
  if (rc == L7_SUCCESS)
  {
    /* forwarding will be set after the driver returns with a success */
    p->portInstInfo[instIndex].pstState = PST_FORWARDING;
  }
  trapMgrDot1dStpPortStateForwardingLogTrap(p->portNum, instIndex);

  return rc;
}
/*********************************************************************
* @purpose  This routine claculates the next state fo the PST state machine
*           and executes the action for that next state
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPstMachine(L7_uint32 dot1sEvent,
                        DOT1S_PORT_COMMON_t *p,
                        L7_uint32 instIndex)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_pst_states_t  nextState, currState = PST_DISCARDING;
  L7_uint32       normalizedEvent;

  normalizedEvent = dot1sEvent - dot1sPrxEvents - 1;
  if (p->portInstInfo[instIndex].pstState < PST_STATES)
  {
    currState = p->portInstInfo[instIndex].pstState;
  }
  nextState = pstStateTable[normalizedEvent][currState];

  switch (nextState)
  {
    case PST_DISCARDING:
      rc = dot1sPstDiscardingAction(p, instIndex);
      /* dot1sPstGenerateEvents will be called after the driver returns with a success */
      break;
    case PST_LEARNING:
      rc = dot1sPstLearningAction(p, instIndex);
      /* dot1sPstGenerateEvents will be called after the driver returns with a success */break;
    case PST_FORWARDING:
      rc = dot1sPstForwardingAction(p, instIndex);
      /* dot1sPstGenerateEvents will be called after the driver returns with a success */break;
    default:
      break;
  }
  return rc;
}
/*********************************************************************
* @purpose  Events that the port state transition machine needs to generate and propagate
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPstGenerateEvents(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_uint32   id;
  L7_RC_t     rc;

  rc = dot1sInstNumFind(instIndex, &id);


  dot1sGenerateEventsForTcm(p,instIndex);
  /***************************
   *generate events for prt*
   *************************
   */
  if (p->portInstInfo[instIndex].selected == L7_TRUE &&
      p->portInstInfo[instIndex].updtInfo == L7_FALSE)
  {
    /* Root role does not have an event for either forwarding or discarding flag change
       So avaid unneccessary processing*/
    if (p->portInstInfo[instIndex].learning == L7_FALSE &&
        p->portInstInfo[instIndex].forwarding == L7_FALSE )
    { /*Send Event E17*/

      switch (p->portInstInfo[instIndex].role)
      {
        case ROLE_DISABLED:
          rc = dot1sStateMachineClassifier(prtDisabledNotLearningNotForwarding,
                                           p, instIndex , L7_NULL, L7_NULLPTR);
          break;
        case ROLE_BACKUP:
        case ROLE_ALTERNATE:
          rc = dot1sStateMachineClassifier(prtAltbkNotLrngNotFwdg,
                                           p, instIndex , L7_NULL, L7_NULLPTR);
          break;
        case ROLE_MASTER:
          if (p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            rc = dot1sStateMachineClassifier(prtMasterNotLrnNotFwdNotSynced,
                                             p, instIndex , L7_NULL, L7_NULLPTR);
          }
          break;
        case ROLE_DESIGNATED:
          if (p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            rc = dot1sStateMachineClassifier(prtDesigNotLrnNotFwdNotSynced,
                                             p, instIndex , L7_NULL, L7_NULLPTR);
          }
          break;
        default:
          rc = L7_SUCCESS;
          break;
      }/* Switch */
    }/* if lrng == false , fwdg== false */
  } /* selected Not Updt Info */

  return L7_SUCCESS;
}
/***************************************************************************
 *******************************TCM Support Routines************************
 ***************************************************************************
 */
/*********************************************************************
* @purpose  Flushes all Dynamic Filtering Entries in the Filtering Database
*           that contain information learned on this port, unless this port
*           is an Edge Port
*
* @param    intIfNum @b{(input)} port number
* @param    instIndex @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sFlush(L7_uint32 intIfNum, L7_uint32 instIndex)
{
  DOT1S_PORT_COMMON_t   *p;
  L7_NIM_QUERY_DATA_t nimQueryData;

  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
  {
    return L7_FAILURE;/* should never get here*/
  }
  p = dot1sIntfFind(intIfNum);

  if ((dot1sCnfgrAppIsReady() != L7_TRUE) &&
      (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
       (dot1sNsfFuncTable.dot1sIsIntfReconciled(intIfNum) != L7_TRUE)))
  {
    return L7_SUCCESS;
  }

  if ((p != L7_NULLPTR) && (p->operEdge != L7_TRUE))
  {
    /* only talk to the hardware when the hardware is valid */
    if ((nimQueryData.data.state == L7_INTF_ATTACHING) ||
        (nimQueryData.data.state == L7_INTF_DETACHING) ||
        (nimQueryData.data.state == L7_INTF_ATTACHED))
    {
      if (dot1sIsPortActiveForInstance(instIndex, intIfNum) == L7_TRUE)
      {

        if (DOT1S_DEBUG(DOT1S_DEBUG_FLUSH,instIndex))
        {
          DOT1S_DEBUG_MSG("%s: Flushing Dynamic Filtering entries for port %d at %lld\n",
                          __FUNCTION__,intIfNum,osapiTimeMillisecondsGet64());
        }

        return dtlDot1sFlush(intIfNum);
      }
    }/* Hw is valid*/
  }

  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Sets the value of the tcWhile param
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 dot1sTcWhileNew(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_uint32 instNum;
  L7_RC_t rc;
  L7_uint32 newTcWhile = L7_NULL;
  DOT1S_BRIDGE_t *bridge;

  bridge = dot1sBridgeParmsFind();

  if (p->portInstInfo[instIndex].tcWhile == L7_NULL)
  {

    if (p->sendRSTP == L7_TRUE)
    {
      /* Sets TCwhile to hello times + 1 and sets newInfo cist or msti to true */
      newTcWhile = p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistPortTimes.helloTime + DOT1S_TIMER_UNIT;
      if (instIndex == DOT1S_CIST_INDEX)
      {
        p->newInfoCist = L7_TRUE;
      }
      else
      {
        p->newInfoMist = L7_TRUE;
      }
      if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                      __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
      }

    }
    else
    {
      newTcWhile = dot1sInstance->cist.cistRootTimes.maxAge + dot1sInstance->cist.cistRootTimes.fwdDelay;
      if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY))
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d tcWhile %d \n",
                      __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist,newTcWhile);
      }
    }

    /* See if a topology change is already in progress */
    rc = dot1sInstNumFind(instIndex, &instNum);
    if ((rc == L7_SUCCESS) && (dot1sMstiTopologyChangeParmGet(instNum) != L7_TRUE))
    {
      /* Issue a topology change trap for the instance */
      rc = trapMgrStpInstanceTopologyChange(instNum);

      /* topology change notification */
      dot1sIhDoNotify(instNum, L7_NULL, L7_DOT1S_EVENT_TOPOLOGY_CHANGE);

      /* set the start time for the toplolgy change */
      if (instIndex != DOT1S_CIST_INDEX)
      {
        dot1sInstance->msti[instIndex].tcWhileChangeTime = osapiUpTimeRaw();
        dot1sInstance->msti[instIndex].tcWhileChange++;
      }
      else
      {
        dot1sInstance->cist.tcWhileChangeTime = osapiUpTimeRaw();
        dot1sInstance->cist.tcWhileChange++;
      }
    }


  }

  else
  {
    newTcWhile = p->portInstInfo[instIndex].tcWhile;


  }
  return newTcWhile;
}
/*********************************************************************
* @purpose  Sets the value of the tcWhile param to Zero
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcWhileZero(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  p->portInstInfo[instIndex].tcWhile = L7_NULL;
  /* If the port is designated port notify the helper(if present) that
     PDU has changed, else we may not need to keep on transmitting the pdu.
  */
  if (p->portInstInfo[instIndex].role != ROLE_DESIGNATED)
  {
    p->pduTxStopCheck = L7_TRUE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Sets the tcProp for the given instance for all ports
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcPropTreeSet(L7_uint32 intIfNum, L7_uint32 instIndex)
{
  L7_uint32       instId;
  DOT1S_PORT_COMMON_t   *p, *currPort;
  L7_RC_t         rc = L7_SUCCESS;

  rc = dot1sInstNumFind(instIndex, &instId);
  currPort = dot1sIntfFind(intIfNum);
  if (currPort == L7_NULLPTR)
  {
    return L7_FAILURE;
  }


  if (currPort->restrictedTcn == L7_TRUE)
  {
    /* ignore the Topology change if the originating port is restricted */
    return L7_SUCCESS;
  }

  p = dot1sIntfFindFirst();
  while (p != L7_NULLPTR)
  {
    if (p->portUpEnabledNum != 0 &&
        p->portNum != intIfNum &&
        p->portInstInfo[instIndex].tcProp == L7_FALSE &&
        p->operEdge == L7_FALSE)
    {
      p->portInstInfo[instIndex].tcProp = L7_TRUE;
      /***************************
       * generate events for tcm*
       **************************
       */
      /*rc = dot1sIssueCmd(tcmTcProp, p->portNum, id, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(tcmTcProp, p, instIndex, L7_NULL, L7_NULLPTR);
      if (DOT1S_DEBUG(DOT1S_DEBUG_DETECTED_TC,instIndex))
      {
        DOT1S_DEBUG_MSG("%s:Set tcprop for port (%d) ,inst (%d) to TRUE at %lld.",
                        __FUNCTION__,p->portNum,instIndex,osapiTimeMillisecondsGet64());
      }
    }
    p = dot1sIntfFindNext(p->portNum);
  }/* while */

  return rc;
}
/***************************************************************************
 *******************************TCM State Machine Routines******************
 ***************************************************************************
 */

/*********************************************************************
* @purpose  Actions to be performed in the TCM state INIT
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmInitAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  rc = dot1sFlush(p->portNum, instIndex);
  rc = dot1sTcWhileZero(p, instIndex);
  if (instIndex == DOT1S_CIST_INDEX)
  {
    p->tcAck = L7_FALSE;
  }

  p->portInstInfo[instIndex].tcmState = TCM_INACTIVE;
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the TCM state INACTIVE
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmInactiveAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  if (instIndex == DOT1S_CIST_INDEX)
  {
    p->portInstInfo[instIndex].rcvdTc = L7_FALSE;
    p->rcvdTcn = L7_FALSE;
    p->rcvdTcAck = L7_FALSE;
  }

  p->portInstInfo[instIndex].rcvdTc = L7_FALSE;
  p->portInstInfo[instIndex].tcProp = L7_FALSE;

  p->portInstInfo[instIndex].tcmState = TCM_LEARNING;

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the TCM state DETECTED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmDetectedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  if ((dot1sCnfgrAppIsReady() != L7_TRUE) &&
      (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
       (dot1sNsfFuncTable.dot1sIsIntfReconciled(p->portNum) != L7_TRUE)))
  {
    dot1sNsfFuncTable.dot1sNsfReconEventFunc(p->portNum, instIndex,
                                             dot1s_recon_event_flush);
    return L7_SUCCESS;
  }

  p->portInstInfo[instIndex].tcWhile = dot1sTcWhileNew(p, instIndex);
  rc = dot1sTcPropTreeSet(p->portNum, instIndex);

  if (instIndex == DOT1S_CIST_INDEX)
  {
    p->newInfoCist = L7_TRUE;
  }
  else
  {
    p->newInfoMist = L7_TRUE;
  }
  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port %d newInfo cist %d newInfo msti %d \n",
                  __FUNCTION__, p->portNum, p->newInfoCist, p->newInfoMist);
  }
  p->portInstInfo[instIndex].tcmState = TCM_DETECTED;
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the TCM state ACTIVE
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmActiveAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].tcmState = TCM_ACTIVE;

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the TCM state NOTIFIED_TCN
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmNotifiedTcnAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  p->portInstInfo[instIndex].tcWhile =  dot1sTcWhileNew(p, instIndex);

  p->portInstInfo[instIndex].tcmState = TCM_NOTIFIED_TCN;

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the TCM state NOTIFIED_TC
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmNotifiedTcAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t              rc = L7_FAILURE;
  L7_uint32            instNum, intIfNum;
  DOT1S_PORT_COMMON_t *pPort;
  L7_BOOL              anotherPort = L7_FALSE;

  if (instIndex == DOT1S_CIST_INDEX)
    p->rcvdTcn = L7_FALSE;
  p->portInstInfo[instIndex].rcvdTc = L7_FALSE;


  if (instIndex == DOT1S_CIST_INDEX && p->portInstInfo[instIndex].role == ROLE_DESIGNATED)
  {
    p->tcAck = L7_TRUE;
  }


  rc = dot1sTcPropTreeSet(p->portNum, instIndex);

  /*************************************************************************
   * If this port is the only interface that is linked up, enabled for
   * spanning tree and whose operEdge is false then a TC event needs to be
   * sent to interested parties. Otherwise the received TC, while not
   * starting a tcWhile timer, would go unnoticed.
   * If there is another port which is participating in spanning tree then
   * the dot1sTcPropTreeSet would propagate the TC to that port and that port's
   * tcWhile timer would be started via dot1sTcWhileNew wherein the TC event
   * would be sent to interested parties.
   */
  rc = dot1sInstNumFind(instIndex, &instNum);
  intIfNum = p->portNum;

  pPort = dot1sIntfFindFirst();
  while (pPort != L7_NULLPTR)
  {
    if (intIfNum != pPort->portNum)
    {
      if (pPort->portLinkState == L7_UP &&
          pPort->portEnabled == L7_TRUE &&
          pPort->operEdge == L7_FALSE)
      {
        anotherPort = L7_TRUE;
        break;
      }
    }
    pPort = dot1sIntfFindNext(pPort->portNum);
  }/* while */
  if (anotherPort == L7_FALSE)
  {
    /* topology change notification */
    dot1sIhDoNotify(instNum, L7_NULL, L7_DOT1S_EVENT_TOPOLOGY_CHANGE);
  }

  /*************************************************************************/

  p->portInstInfo[instIndex].tcmState = TCM_NOTIFIED_TC;

  if (DOT1S_DEBUG(DOT1S_DEBUG_DETECTED_TC,instIndex))
  {
    DOT1S_DEBUG_MSG("%s:Put port (%d) , instance (%d) in TCM_NOTIFIED_TC at %lld.",
                    __FUNCTION__,p->portNum,instIndex,osapiTimeMillisecondsGet64());
  }
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the TCM state PROPAGATING
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmPropagatingAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;


  p->portInstInfo[instIndex].tcWhile = dot1sTcWhileNew(p, instIndex);
  if (DOT1S_DEBUG(DOT1S_DEBUG_DETECTED_TC,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Port (%d) inst(%d) TC propogating TC %d \n",__FUNCTION__,
                  p->portNum, instIndex, p->portInstInfo[instIndex].tcWhile );
  }



  rc = dot1sFlush(p->portNum, instIndex);

  p->portInstInfo[instIndex].tcProp = L7_FALSE;


  p->portInstInfo[instIndex].tcmState = TCM_PROPAGATING;
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the TCM state ACKNOWLEDGED
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmAcknowledgedAction(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_FAILURE;

  rc = dot1sTcWhileZero(p, instIndex);
  p->rcvdTcAck = L7_FALSE;

  p->portInstInfo[instIndex].tcmState = TCM_ACKNOWLEDGED;

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  This routine claculates the next state fo the TCM state machine
*           and executes the action for that next state
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmMachine(L7_uint32 dot1sEvent,
                        DOT1S_PORT_COMMON_t *p,
                        L7_uint32 instIndex)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_tcm_states_t  nextState;
  L7_uint32       normalizedEvent;

  normalizedEvent = dot1sEvent - dot1sPstEvents - 1;

  nextState = tcmStateTable[normalizedEvent][p->portInstInfo[instIndex].tcmState];
  if (DOT1S_DEBUG(DOT1S_DEBUG_DETECTED_TC,instIndex))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Port (%d) inst(%d)Dot1sEvent %d curr state %d nextState %d   \n",__FUNCTION__,
                  p->portNum, instIndex, dot1sEvent, p->portInstInfo[instIndex].tcmState, nextState );
  }



  switch (nextState)
  {
    case TCM_INACTIVE:
      rc = dot1sTcmInitAction(p, instIndex);
      /*UCT to Inactive State*/
      rc = dot1sTcmInactiveAction(p, instIndex);
      rc = dot1sTcmGenerateEvents(p, instIndex);
      break;
    case TCM_LEARNING:
      rc = dot1sTcmInactiveAction(p, instIndex);
      rc = dot1sTcmGenerateEvents(p, instIndex);
      break;
    case TCM_DETECTED:
      if (DOT1S_DEBUG(DOT1S_DEBUG_DETECTED_TC,instIndex))
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Port (%d) inst(%d) TC detected \n",__FUNCTION__,
                      p->portNum, instIndex );
      }

      rc = dot1sTcmDetectedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sTcmActiveAction(p, instIndex);
      rc = dot1sTcmGenerateEvents(p, instIndex);
      break;
    case TCM_ACTIVE:
      rc = dot1sTcmActiveAction(p, instIndex);
      rc = dot1sTcmGenerateEvents(p, instIndex);
      break;
    case TCM_NOTIFIED_TCN:
      rc = dot1sTcmNotifiedTcnAction(p, instIndex);
      /*UCT to Notified_Tc State*/

    case TCM_NOTIFIED_TC:
      rc = dot1sTcmNotifiedTcAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sTcmActiveAction(p, instIndex);
      rc = dot1sTcmGenerateEvents(p, instIndex);
      break;
    case TCM_PROPAGATING:
      rc = dot1sTcmPropagatingAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sTcmActiveAction(p, instIndex);
      rc = dot1sTcmGenerateEvents(p, instIndex);
      break;
    case TCM_ACKNOWLEDGED:
      rc = dot1sTcmAcknowledgedAction(p, instIndex);
      /*UCT to Active State*/
      rc = dot1sTcmActiveAction(p, instIndex);
      rc = dot1sTcmGenerateEvents(p, instIndex);
      break;
    default:
      break;
  }/*end switch (nextState)*/
  return rc;
}
/*********************************************************************
* @purpose  Events that the topology change machine needs to generate and propagate
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sTcmGenerateEvents(DOT1S_PORT_COMMON_t *p,
                               L7_uint32 instIndex)
{

  L7_uint32       j;
  L7_RC_t         rc = L7_SUCCESS;
  L7_BOOL         cistRootPort = L7_FALSE;
  L7_BOOL         cistDesignatedPort = L7_FALSE;
  L7_BOOL         mstiRootPort = L7_FALSE;
  L7_BOOL         mstiDesignatedPort = L7_FALSE;
  L7_BOOL         allSelected = L7_TRUE;
  L7_BOOL         allUpdtInfo = L7_FALSE;
  DOT1S_BRIDGE_t    *bridge;

  bridge = dot1sBridgeParmsFind();


  /***************************
   *generate events for ptx*
   **************************
   */
  /*all events are qualified with &&selected && !updtInfo*/
  for (j = 0; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].selected == L7_FALSE)
      {
        allSelected = L7_FALSE;
        break;
      }
  }
  for (j = 0; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].updtInfo == L7_TRUE)
      {
        allUpdtInfo = L7_TRUE;
        break;
      }
  }

  if (allSelected == L7_TRUE &&
      allUpdtInfo == L7_FALSE)
  {
    if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_ROOT)
      cistRootPort = L7_TRUE;
    if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_DESIGNATED)
      cistDesignatedPort = L7_TRUE;

    for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
        if (p->portInstInfo[j].role == ROLE_ROOT)
        {
          mstiRootPort = L7_TRUE;
          break;
        }
    }
    for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
        if (p->portInstInfo[j].role == ROLE_DESIGNATED)
        {
          mstiDesignatedPort = L7_TRUE;
          break;
        }
    }

    if (p->sendRSTP == L7_TRUE &&
        ((p->newInfoCist == L7_TRUE &&
          (cistRootPort == L7_TRUE || cistDesignatedPort == L7_TRUE)) ||
         (p->newInfoMist == L7_TRUE && (mstiRootPort == L7_TRUE || mstiDesignatedPort == L7_TRUE))) &&
        (p->txCount < bridge->TxHoldCount) && (p->helloWhen != 0)
       )
    {
      /*Send Event E66*/
      /*rc = dot1sIssueCmd(ptxSendRstp, p->portNum, L7_NULL, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(ptxSendRstp, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }
    if (p->sendRSTP == L7_FALSE &&
        p->newInfoCist == L7_TRUE &&
        cistRootPort == L7_TRUE &&
        p->txCount < bridge->TxHoldCount &&
        p->helloWhen != 0
       )
    {
      /*Send Event E67*/
      /*rc = dot1sIssueCmd(ptxSendTcn, p->portNum, L7_NULL, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(ptxSendTcn, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }
    else if (p->sendRSTP == L7_FALSE &&
             p->newInfoCist == L7_TRUE &&
             cistDesignatedPort == L7_TRUE &&
             p->txCount < bridge->TxHoldCount &&
             p->helloWhen != 0
            )
    {
      /*Send Event E68*/
      /*rc = dot1sIssueCmd(ptxSendConfig, p->portNum, L7_NULL, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(ptxSendConfig, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }

  }/*end if (allSelected == L7_TRUE &&....*/

  return rc;
}

/***************************************************************************
 *******************************PPM State Machine Routines******************
 ***************************************************************************
 */
/*********************************************************************
* @purpose  Actions to be performed in the PPM state INIT
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPpmCheckingRstpAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_FAILURE;
  DOT1S_BRIDGE_t  *bridge;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
    return L7_FAILURE;

  p->mcheck = L7_FALSE;
  p->sendRSTP = (bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W);
  L7_INTF_SETMASKBIT(portCommonChangeMask, p->portNum);
  p->mdelayWhile = bridge->MigrateTime;


  p->ppmState = PPM_CHECKING_RSTP;

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PPM state SEND_RSTP
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPpmSelectingStpAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t       rc = L7_FAILURE;
  DOT1S_BRIDGE_t  *bridge;

  bridge = dot1sBridgeParmsFind();
  if (bridge == L7_NULLPTR)
    return L7_FAILURE;

  p->mdelayWhile = bridge->MigrateTime;
  p->sendRSTP = L7_FALSE;
  L7_INTF_SETMASKBIT(portCommonChangeMask, p->portNum);

  p->ppmState = PPM_SELECTING_STP;

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the PPM state SENDING_RSTP
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPpmSensingAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_FAILURE;

  p->rcvdRSTP = L7_FALSE;
  p->rcvdSTP = L7_FALSE;

  p->ppmState = PPM_SENSING;

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  This routine claculates the next state fo the PPM state machine
*           and executes the action for that next state
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1sPpmMachine(L7_uint32 dot1sEvent, DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_ppm_states_t  nextState;
  L7_uint32       normalizedEvent;

  normalizedEvent = dot1sEvent - dot1sTcmEvents - 1;

  nextState = ppmStateTable[normalizedEvent][p->ppmState];

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_PPM))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: curr state %d dot1sEvent %d normalized event %d  next state %d \n",__FUNCTION__,
                  p->ppmState, dot1sEvent, normalizedEvent, nextState);
  }


  switch (nextState)
  {
    case PPM_CHECKING_RSTP:
      rc = dot1sPpmCheckingRstpAction(p);
      rc = dot1sPpmGenerateEvents(p);
      break;
    case PPM_SELECTING_STP:
      rc = dot1sPpmSelectingStpAction(p);
      rc = dot1sPpmGenerateEvents(p);
      break;
    case PPM_SENSING:
      rc = dot1sPpmSensingAction(p);
      rc = dot1sPpmGenerateEvents(p);
      break;
    default:
      break;
  }
  return rc;
}
/*********************************************************************
* @purpose  Events that the port protocol migration machine needs to
*           generate and propagate
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPpmGenerateEvents(DOT1S_PORT_COMMON_t *p)
{
  L7_uint32     j;
  L7_RC_t       rc = L7_SUCCESS;
  L7_BOOL       cistRootPort = L7_FALSE;
  L7_BOOL       cistDesignatedPort = L7_FALSE;
  L7_BOOL       mstiRootPort = L7_FALSE;
  L7_BOOL       mstiDesignatedPort = L7_FALSE;
  L7_BOOL       allSelected = L7_TRUE;
  L7_BOOL       allUpdtInfo = L7_FALSE;
  DOT1S_BRIDGE_t  *bridge;

  bridge = dot1sBridgeParmsFind();


  if (p->rcvdSTP == L7_TRUE &&
      p->sendRSTP == L7_TRUE &&
      p->ppmState == PPM_SENSING )
  {
    /*Send Event E58*/
    /*rc = dot1sIssueCmd(ppmMDelayWhileNotZeroRcvdStp, p->portNum, L7_NULL, L7_NULLPTR);*/
    if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_PPM))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) generating ppmSendRSTPRcvdStp \n",__FUNCTION__, p->portNum);
    }


    rc = dot1sStateMachineClassifier(ppmSendRSTPRcvdStp, p, L7_NULL, L7_NULL, L7_NULLPTR);
  }

  if (p->rcvdRSTP == L7_TRUE &&
      p->sendRSTP == L7_FALSE &&
      p->ppmState == PPM_SENSING &&
      bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W )
  {
    /*Send Event E59*/
    /*rc = dot1sIssueCmd(ppmMDelayWhileNotZeroRcvdRstp, p->portNum, L7_NULL, L7_NULLPTR);*/
    if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_PPM))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) generating ppmRstpVersionNotSendRstpRcvdSTP \n",__FUNCTION__, p->portNum);
    }


    rc = dot1sStateMachineClassifier(ppmRstpVersionNotSendRstpRcvdSTP, p, L7_NULL, L7_NULL, L7_NULLPTR);
  }

  if (p->mcheck == L7_TRUE)
  {
    if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_PPM))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) generating ppmMCheck \n",__FUNCTION__, p->portNum);
    }


    rc = dot1sStateMachineClassifier(ppmMCheck, p, L7_NULL, L7_NULL, L7_NULLPTR);
  }


  if (p->portEnabled == L7_FALSE)
  {
    if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_PPM))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) generating ppmPortDisabled \n",__FUNCTION__, p->portNum);
    }


    rc = dot1sStateMachineClassifier(ppmPortDisabled, p, L7_NULL, L7_NULL, L7_NULLPTR);
  }


  /***************************
   *generate events for ptx*
   *************************
   */
  /*all events are qualified with &&selected && !updtInfo*/
  for (j = 0; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].selected == L7_FALSE)
      {
        allSelected = L7_FALSE;
        break;
      }
  }
  for (j = 0; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].updtInfo == L7_TRUE)
      {
        allUpdtInfo = L7_TRUE;
        break;
      }
  }

  if (allSelected == L7_TRUE &&
      allUpdtInfo == L7_FALSE)
  {
    if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_ROOT)
      cistRootPort = L7_TRUE;
    if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_DESIGNATED)
      cistDesignatedPort = L7_TRUE;

    for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
        if (p->portInstInfo[j].role == ROLE_ROOT)
        {
          mstiRootPort = L7_TRUE;
          break;
        }
    }
    for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
        if (p->portInstInfo[j].role == ROLE_DESIGNATED)
        {
          mstiDesignatedPort = L7_TRUE;
          break;
        }
    }

    if (p->sendRSTP == L7_TRUE &&
        ((p->newInfoCist == L7_TRUE &&
          (cistRootPort == L7_TRUE || cistDesignatedPort == L7_TRUE)) ||
         (p->newInfoMist == L7_TRUE && (mstiRootPort == L7_TRUE || mstiDesignatedPort == L7_TRUE))) &&
        (p->txCount < bridge->TxHoldCount) && (p->helloWhen != 0)
       )
    {
      /*Send Event E66*/
      /*rc = dot1sIssueCmd(ptxSendRstp, p->portNum, L7_NULL, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(ptxSendRstp, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }
    if (p->sendRSTP == L7_FALSE &&
        p->newInfoCist == L7_TRUE &&
        cistRootPort == L7_TRUE &&
        p->txCount < bridge->TxHoldCount &&
        p->helloWhen != 0
       )
    {
      /*Send Event E67*/
      /*rc = dot1sIssueCmd(ptxSendTcn, p->portNum, L7_NULL, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(ptxSendTcn, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }
    else if (p->sendRSTP == L7_FALSE &&
             p->newInfoCist == L7_TRUE &&
             cistDesignatedPort == L7_TRUE &&
             p->txCount < bridge->TxHoldCount &&
             p->helloWhen != 0
            )
    {
      /*Send Event E68*/
      /*rc = dot1sIssueCmd(ptxSendConfig, p->portNum, L7_NULL, L7_NULLPTR);*/
      rc = dot1sStateMachineClassifier(ptxSendConfig, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }

  }/*end if (allSelected == L7_TRUE &&....*/
  return rc;
}

/***************************************************************************
 *******************************BDM State Machine Routines******************
 **************************************************************************
 */
/*********************************************************************
* @purpose  Actions to be performed in the BDM state INIT
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBdmEdgeAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_FAILURE;

  p->operEdge = L7_TRUE;
  p->bdmState = BDM_EDGE;

  if (p->adminEdge != L7_TRUE)
  {
    if((dot1sCnfgrAppIsReady() != L7_TRUE) &&
       (dot1sNsfFuncTable.dot1sIsIntfReconciled &&
       (dot1sNsfFuncTable.dot1sIsIntfReconciled(p->portNum) != L7_TRUE)))
    {
      dot1sNsfFuncTable.dot1sNsfReconEventFunc(p->portNum, L7_DOT1S_MSTID_ALL,
                                               dot1s_recon_event_flush);
      return L7_SUCCESS;
    }
  }

  return rc;
}
/*********************************************************************
* @purpose  Actions to be performed in the BDM state BPDU_SEEN
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBdmNotEdgeAction(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_FAILURE;

  p->operEdge = L7_FALSE;

  p->bdmState = BDM_NOT_EDGE;
  return rc;
}
/*********************************************************************
* @purpose  This routine claculates the next state fo the BDM state machine
*           and executes the action for that next state
*
* @param    dot1sEvent @b{(input)} dot1s event number
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBdmMachine(L7_uint32 dot1sEvent, DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t         rc = L7_FAILURE;
  dot1s_bdm_states_t  nextState;
  L7_uint32       normalizedEvent;

  normalizedEvent = dot1sEvent - dot1sPtxEvents - 1;

  nextState = bdmStateTable[normalizedEvent][p->bdmState];

  if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_BDM))
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: curr state %d dot1sEvent %d normalized event %d   next state %d \n",__FUNCTION__,
                  p->bdmState, dot1sEvent, normalizedEvent, nextState);
  }


  switch (nextState)
  {
    case BDM_EDGE:
      rc = dot1sBdmEdgeAction(p);
      rc = dot1sBdmGenerateEvents(p);
      break;
    case BDM_NOT_EDGE:
      rc = dot1sBdmNotEdgeAction(p);
      rc = dot1sBdmGenerateEvents(p);
      break;
    default:
      break;
  }
  return rc;
}

/*********************************************************************
* @purpose  Events that the bridge detection machine needs to generate and propagate
*
* @param    p          @b{(input)} pointer to port entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBdmGenerateEvents(DOT1S_PORT_COMMON_t *p)
{
  L7_uint32 instIndex, id, index;
  L7_RC_t   rc;



  generateEventsForBDM(p);


  for (instIndex = 0; instIndex<= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
    {

      rc = dot1sInstNumFind(instIndex, &id);
      if (rc == L7_FAILURE)
      {
        return L7_FAILURE;
      }
      rc = dot1sInstIndexFind(id,  &index);
      if (rc == L7_FAILURE)
      {
        return L7_FAILURE;
      }
      /***************************
       *generate events for prt*
       **************************
       */
      if (p->portInstInfo[instIndex].selected == L7_TRUE &&
          p->portInstInfo[instIndex].updtInfo == L7_FALSE)
      {
        /* No need to process any events for prt as only
            designated role sm is concerned with oper edge */
        if (p->portInstInfo[instIndex].role == ROLE_DESIGNATED)
        {

          if (p->operEdge == L7_TRUE &&
              p->portInstInfo[instIndex].synced == L7_FALSE )
          {
            rc = dot1sStateMachineClassifier(prtDesigAgreedNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if ((p->portInstInfo[instIndex].learn ==L7_TRUE ||
               p->portInstInfo[instIndex].forward == L7_TRUE) &&
              p->operEdge == L7_FALSE &&
              ((p->portInstInfo[instIndex].sync == L7_TRUE &&
                p->portInstInfo[instIndex].synced == L7_FALSE) ||
               ((p->portInstInfo[instIndex].rrWhile == 0) &&
                p->portInstInfo[instIndex].reRoot == L7_TRUE) ||
               p->portInstInfo[instIndex].disputed == L7_TRUE
              )
             )
          {
            rc = dot1sStateMachineClassifier(prtDesigListen,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].learn == L7_FALSE &&
              p->portInstInfo[instIndex].sync == L7_FALSE  &&
              p->loopInconsistent == L7_FALSE &&
              ((p->portInstInfo[instIndex].fdWhile == 0) ||
               /*((bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W ) &&*/
               (p->portInstInfo[instIndex].agreed == L7_TRUE ||
                p->operEdge == L7_TRUE)) &&
              ((p->portInstInfo[instIndex].rrWhile == 0) ||
               p->portInstInfo[instIndex].reRoot == L7_FALSE)
             )
          {
            rc = dot1sStateMachineClassifier(prtDesigLearn,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].learn == L7_TRUE &&
              p->portInstInfo[instIndex].forward == L7_FALSE &&
              p->portInstInfo[instIndex].sync == L7_FALSE  &&
              ((p->portInstInfo[instIndex].fdWhile == 0) ||
               /*((bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W ) &&*/
               (p->portInstInfo[instIndex].agreed == L7_TRUE ||
                p->operEdge == L7_TRUE)) &&
              ((p->portInstInfo[instIndex].rrWhile == 0) ||
               p->portInstInfo[instIndex].reRoot == L7_FALSE)
             )
          {
            rc = dot1sStateMachineClassifier(prtDesigForward,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }
        } /* end if role is designated */

      }/*end if (p->portInstInfo[i].selected == L7_TRUE &&....*/

      /***************************
       *generate events for tcm*
       **************************
       */
      if (p->operEdge == L7_FALSE &&
          p->portInstInfo[instIndex].forwarding == L7_TRUE &&
          p->portInstInfo[instIndex].tcmState == TCM_LEARNING)
      {
        if ((p->portInstInfo[instIndex].role == ROLE_ROOT) ||
            (p->portInstInfo[instIndex].role == ROLE_DESIGNATED) ||
            (p->portInstInfo[instIndex].role == ROLE_MASTER) )
        {
          /*Send Event E51*/
          rc = dot1sStateMachineClassifier(tcmRoleRootFwdNotEdge, p,
                                           index, L7_NULL, L7_NULLPTR); /* Same event for the three port roles */
        }
      }
    }/*end if (dot1sInstanceMap[i].inUse == L7_TRUE)*/
  }/*end for (i = 0; i<= L7_MAX_MULTIPLE_STP_INSTANCES; i++)*/
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Check to see if ID1 is less than ID2
*
* @param    id1     @b{(input)} identifier 1
* @param    id2     @b{(input)} identifier 2
*
* @returns  L7_TRUE  id1 is < id2
* @returns  L7_FALSE other wise
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sIdLesser(DOT1S_BRIDGEID_t id1, DOT1S_BRIDGEID_t id2)
{
  if (id1.priInstId < id2.priInstId)
  {
    return L7_TRUE;
  }

  if ((id1.priInstId == id2.priInstId) &&
      (memcmp(id1.macAddr, id2.macAddr, L7_MAC_ADDR_LEN) < 0))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}
/*********************************************************************
* @purpose  Check to see if ID1 is equal to ID2
*
* @param    id1     @b{(input)} identifier 1
* @param    id2     @b{(input)} identifier 2
*
* @returns  L7_TRUE  id1 is same as id2
* @returns  L7_FALSE other wise
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sIdEqual(DOT1S_BRIDGEID_t id1, DOT1S_BRIDGEID_t id2)
{
  if ((id1.priInstId == id2.priInstId) &&
      (memcmp(id1.macAddr, id2.macAddr, L7_MAC_ADDR_LEN) == 0))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}
/*********************************************************************
* @purpose  Check to see if ID1 is NOT equal to ID2
*
* @param    id1     @b{(input)} identifier 1
* @param    id2     @b{(input)} identifier 2
*
* @returns  L7_TRUE  id1 is not same as id2
* @returns  L7_FALSE other wise
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sIdNotEqual(DOT1S_BRIDGEID_t id1, DOT1S_BRIDGEID_t id2)
{
  if ((id1.priInstId == id2.priInstId) &&
      (memcmp(id1.macAddr, id2.macAddr, L7_MAC_ADDR_LEN) == 0))
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Compare two cist rpiority vectors
*
* @param    pv1     @b{(input)} priority vector 1
* @param    pv2     @b{(input)} priority vector 2
*
* @returns  SAME    both are same
* @returns  BETTER  pv1 is better than pv2
* @returns  WORSE   pv1 is worse than pv2
*
* @comments
*
* @end
*********************************************************************/
DOT1S_PRI_COMP_t dot1sCistPriVecCompare(DOT1S_CIST_PRIORITY_VECTOR_t *pv1,
                                        DOT1S_CIST_PRIORITY_VECTOR_t *pv2)
{
  DOT1S_PRI_COMP_t  comparison;

  if (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
      (pv1->extRootPathCost == pv2->extRootPathCost) &&
      dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
      (pv1->intRootPathCost == pv2->intRootPathCost) &&
      dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
      (pv1->dsgPortId == pv2->dsgPortId)
     )
  {
    comparison = SAME;
  }
  else if ((dot1sIdLesser(pv1->rootId, pv2->rootId)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost < pv2->extRootPathCost)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost == pv2->extRootPathCost) &&
            dot1sIdLesser(pv1->regRootId, pv2->regRootId)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost == pv2->extRootPathCost) &&
            dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
            (pv1->intRootPathCost < pv2->intRootPathCost)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost == pv2->extRootPathCost) &&
            dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
            (pv1->intRootPathCost == pv2->intRootPathCost) &&
            dot1sIdLesser(pv1->dsgBridgeId, pv2->dsgBridgeId)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost == pv2->extRootPathCost) &&
            dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
            (pv1->intRootPathCost == pv2->intRootPathCost) &&
            dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
            (pv1->dsgPortId < pv2->dsgPortId))
          )
  {
    comparison = BETTER;
  }
  else
  {
    comparison = WORSE;
  }

  return comparison;


}
/*********************************************************************
* @purpose  Compare two msti rpiority vectors
*
* @param    pv1     @b{(input)} priority vector 1
* @param    pv2     @b{(input)} priority vector 2
*
* @returns  SAME    both are same
* @returns  BETTER  pv1 is better than pv2
* @returns  WORSE   pv1 is worse than pv2
*
* @comments
*
* @end
*********************************************************************/
DOT1S_PRI_COMP_t dot1sMstiPriVecCompare(DOT1S_MSTI_PRIORITY_VECTOR_t *pv1, DOT1S_MSTI_PRIORITY_VECTOR_t *pv2)
{
  DOT1S_PRI_COMP_t  comparison;

  if ((dot1sIdLesser(pv1->regRootId, pv2->regRootId)
      ) ||
      (dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
       (pv1->intRootPathCost < pv2->intRootPathCost)
      ) ||
      (dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
       (pv1->intRootPathCost == pv2->intRootPathCost) &&
       dot1sIdLesser(pv1->dsgBridgeId, pv2->dsgBridgeId)
      ) ||
      (dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
       (pv1->intRootPathCost == pv2->intRootPathCost) &&
       dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
       (pv1->dsgPortId < pv2->dsgPortId)
      )
     )
  {
    comparison = BETTER;
  }
  else if (dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
           (pv1->intRootPathCost == pv2->intRootPathCost) &&
           dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
           (pv1->dsgPortId == pv2->dsgPortId)
          )
  {
    comparison = SAME;
  }
  else
  {
    comparison = WORSE;
  }

  return comparison;
}
/*********************************************************************
* @purpose  Finds if the cist root priority vector is derived from a cist port priority vector with the cist root priority vector
*
* @param    rpv          @b{(input)} root priority vector
* @param    ppv          @b{(input)} port priority vector
* @param    epc          @b{(input)} external path cost
* @param    ipc          @b{(input)} internal path cost
* @param    rcvdInternal @b{(input)} priority vector is internal or outside the region

*
* @returns  L7_TRUE     the root priority vector was derived from the port priority vector
* @returns  L7_FALSE    otherwise
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sCistRootPriVecDerived(DOT1S_CIST_PRIORITY_VECTOR_t *rpv,
                                   DOT1S_CIST_PRIORITY_VECTOR_t *ppv,
                                   L7_uint32 epc,
                                   L7_uint32 ipc,
                                   L7_BOOL rcvdInternal)
{
  L7_BOOL rc;

  if (rcvdInternal == L7_FALSE)
  {
    if ((memcmp((char *)&rpv->dsgBridgeId, (char *)&ppv->dsgBridgeId, sizeof(DOT1S_BRIDGEID_t)) == 0) &&
        (rpv->dsgPortId == ppv->dsgPortId) &&
        (rpv->extRootPathCost == ppv->extRootPathCost + epc) &&
        (rpv->intRootPathCost == ppv->intRootPathCost) &&
        /*(memcmp((char *)&rpv->regRootId, (char *)&ppv->regRootId, sizeof(DOT1S_BRIDGEID_t)) == 0) &&*/
        (memcmp((char *)&rpv->rootId, (char *)&ppv->rootId, sizeof(DOT1S_BRIDGEID_t)) == 0) &&
        (rpv->rxPortId == ppv->rxPortId)
       )
    {
      rc = L7_TRUE;
    }
    else
      rc = L7_FALSE;
    if (rpv->intRootPathCost != 0)
      return L7_FALSE;/*sanity check, this should have been set to zero on reception*/
  }
  else
  {
    if ((memcmp((char *)&rpv->dsgBridgeId, (char *)&ppv->dsgBridgeId, sizeof(DOT1S_BRIDGEID_t)) == 0) &&
        (rpv->dsgPortId == ppv->dsgPortId) &&
        (rpv->extRootPathCost == ppv->extRootPathCost) &&
        (rpv->intRootPathCost == ppv->intRootPathCost + ipc) &&
        (memcmp((char *)&rpv->regRootId, (char *)&ppv->regRootId, sizeof(DOT1S_BRIDGEID_t)) == 0) &&
        (memcmp((char *)&rpv->rootId, (char *)&ppv->rootId, sizeof(DOT1S_BRIDGEID_t)) == 0) &&
        (rpv->rxPortId == ppv->rxPortId)
       )
    {
      rc = L7_TRUE;
    }
    else
      rc = L7_FALSE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Finds if the msti root priority vector is derived from a msti port priority vector with the cist root priority vector
*
* @param    rpv     @b{(input)} root priority vector
* @param    ppv     @b{(input)} port priority vector
* @param    ipc     @b{(input)} external path cost
*
* @returns  L7_TRUE     the root priority vector was derived from the port priority vector
* @returns  L7_FALSE    otherwise
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL dot1sMstiRootPriVecDerived(DOT1S_MSTI_PRIORITY_VECTOR_t *rpv,
                                   DOT1S_MSTI_PRIORITY_VECTOR_t *ppv,
                                   L7_uint32 ipc)
{

  L7_BOOL rc;


  if ((memcmp((char *)&rpv->dsgBridgeId, (char *)&ppv->dsgBridgeId, sizeof(DOT1S_BRIDGEID_t)) == 0) &&
      (rpv->dsgPortId == ppv->dsgPortId) &&
      (rpv->intRootPathCost == ppv->intRootPathCost + ipc) &&
      (memcmp((char *)&rpv->regRootId, (char *)&ppv->regRootId, sizeof(DOT1S_BRIDGEID_t)) == 0) &&
      (rpv->rxPortId == ppv->rxPortId)

     )
  {
    rc = L7_TRUE;
  }
  else
    rc = L7_FALSE;

  return rc;
}

/*********************************************************************
* @purpose  To cause the state machine for all the ports to transition to PIM:AGED state
*
* @param    p           @b{(input)} pointer to port entry
* @param    instIndex   @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is only for loop guard. To ensure that if the port which is otherwise forwarding, like
*            say Designated/Forwarding for an instance, another instance  on the same port
*            gets in loop inconsistent state, the port should go to loop inconsistent fr all the instances
*
*
* @end
*********************************************************************/
static L7_RC_t generateLoopInconsistEvent(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_uint32 j;
  L7_RC_t rc = L7_FAILURE;

  for (j = 0; j <= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (j == instIndex)
    {
      continue;
    }
    rc = dot1sStateMachineClassifier(pimRcvdRcvdInfoWhileZeroNotUpdtInfoNotRcvdXstInfo,
                                     p, j , L7_NULL, L7_NULLPTR);
  }
  return rc;
}
/*********************************************************************
* @purpose  All timer related events are generated here
*
* @param    p           @b{(input)} pointer to port entry
* @param    instIndex   @b{(input)} index of the instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments All events are directly fed to the state machien for immediate processing
*           as opposed to queing them again.
*
* @end
*********************************************************************/
L7_RC_t dot1sTimerGenerateEvents(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t       rc = L7_SUCCESS;
  L7_uint32     j;
  L7_BOOL       rcvdMstiInfo = L7_FALSE;
  DOT1S_BRIDGE_t  *bridge;
  L7_BOOL       reRooted = L7_FALSE;
  L7_BOOL       cistRootPort = L7_FALSE;
  L7_BOOL       cistDesignatedPort = L7_FALSE;
  L7_BOOL       mstiRootPort = L7_FALSE;
  L7_BOOL       mstiDesignatedPort = L7_FALSE;
  L7_BOOL       allSelected = L7_TRUE;
  L7_BOOL       allUpdtInfo = L7_FALSE;
  L7_uint32         *reRootedPending = L7_NULLPTR;

  bridge = dot1sBridgeParmsFind();

  if (bridge == L7_NULLPTR)
    return L7_FAILURE;

  /****************************
   * mdelay while timer events*
   ***************************
   */

  if ( p->mdelayWhile == 0 &&
       (p->ppmState == PPM_CHECKING_RSTP ||
        p->ppmState == PPM_SELECTING_STP))
  {
    /*Send Event E59*/

    if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_PPM))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port(%d) Generating msDelayWhileZero event ",__FUNCTION__,
                    p->portNum);
    }


    rc = dot1sStateMachineClassifier(ppmMDelayWhileZero, p, L7_NULL, L7_NULL, L7_NULLPTR);
  }



  /****************************
   *rcvdInfoWhile timer events*
   ***************************
   */

  if (p->portInstInfo[instIndex].pimState == PIM_CURRENT)
  {
    if (instIndex == DOT1S_CIST_INDEX)
    {
      if (p->portInstInfo[instIndex].rcvdInfoWhile == 0 &&
          p->portInstInfo[instIndex].infoIs == INFO_RECEIVED &&
          p->portInstInfo[instIndex].updtInfo == L7_FALSE &&
          p->portInstInfo[instIndex].rcvdMsg == L7_FALSE
         )
      {
        /*Send Event E10*/
        rc = dot1sStateMachineClassifier(pimRcvdRcvdInfoWhileZeroNotUpdtInfoNotRcvdXstInfo,
                                         p, instIndex , L7_NULL, L7_NULLPTR);
        /* Other instances on this port might be in designated role
         we need to force them to get the aged action
      */
        if (p->loopInconsistent)
        {
          generateLoopInconsistEvent(p,instIndex);

        }
      }
    }
    else
    {
      if (p->portInstInfo[instIndex].rcvdMsg == L7_TRUE &&
          p->portInstInfo[DOT1S_CIST_INDEX].rcvdMsg == L7_FALSE)
      {
        rcvdMstiInfo = L7_TRUE;
      }

      if (p->portInstInfo[instIndex].rcvdInfoWhile == 0 &&
          p->portInstInfo[instIndex].infoIs == INFO_RECEIVED &&
          p->portInstInfo[instIndex].updtInfo == L7_FALSE &&
          rcvdMstiInfo == L7_FALSE
         )
      {
        /*Send Event E10*/
        rc = dot1sStateMachineClassifier(pimRcvdRcvdInfoWhileZeroNotUpdtInfoNotRcvdXstInfo,
                                         p, instIndex, L7_NULL, L7_NULLPTR);
        if (p->loopInconsistent)
        {
          generateLoopInconsistEvent(p,instIndex);
        }
      }
    }/*end else*/
  }/*end if (p->portInstInfo[instIndex].pimState == PIM_CURRENT)*/

  if (p->portInstInfo[instIndex].selected == L7_TRUE &&
      p->portInstInfo[instIndex].updtInfo == L7_FALSE)
  {
    /****************************
       *helloWhen timer expires*
       ***************************
     */
    if (p->helloWhen == 0)
    {
      /*Send Event E65*/
      rc = dot1sStateMachineClassifier(ptxHelloWhenZero, p, instIndex, L7_NULL, L7_NULLPTR);
    }

    /****************************
       *fdWhile Timer event*
       ***************************
       */
    if (p->portInstInfo[instIndex].prtState == PRT_ALTERNATE_PORT &&
        p->portInstInfo[instIndex].fdWhile != forwardDelayGet(p) )
    {
      /*Send Event E19*/
      rc = dot1sStateMachineClassifier(prtAltbkFdWhileNotFwdDelay, p, instIndex, L7_NULL, L7_NULLPTR);
    }
    if (p->portInstInfo[instIndex].fdWhile == 0 &&
        p->portInstInfo[instIndex].learn == L7_FALSE)
    {


      /*Send Event E39*/
      if (p->portInstInfo[instIndex].role == ROLE_DESIGNATED)
      {
        rc = dot1sStateMachineClassifier(prtDesigLearn, p, instIndex, L7_NULL, L7_NULLPTR);
      }
      else if (p->portInstInfo[instIndex].role == ROLE_MASTER)
      {
        rc = dot1sStateMachineClassifier(prtMasterLearn, p, instIndex, L7_NULL, L7_NULLPTR);
      }
      else if (p->portInstInfo[instIndex].role == ROLE_ROOT)
      {
        rc = dot1sStateMachineClassifier(prtRootLearn, p, instIndex, L7_NULL, L7_NULLPTR);
      }
    }
    if (p->portInstInfo[instIndex].fdWhile == 0 &&
        p->portInstInfo[instIndex].learn == L7_TRUE &&
        p->portInstInfo[instIndex].forward == L7_FALSE)
    {
      /*send Event E40*/
      if (p->portInstInfo[instIndex].role == ROLE_DESIGNATED)
      {
        rc = dot1sStateMachineClassifier(prtDesigForward, p, instIndex, L7_NULL, L7_NULLPTR);
      }
      else if (p->portInstInfo[instIndex].role == ROLE_MASTER)
      {
        rc = dot1sStateMachineClassifier(prtMasterForward, p, instIndex, L7_NULL, L7_NULLPTR);
      }
      else if (p->portInstInfo[instIndex].role == ROLE_ROOT)
      {
        rc = dot1sStateMachineClassifier(prtRootForward, p, instIndex, L7_NULL, L7_NULLPTR);
      }
    }
    /****************************
       *rbWhile timer events*
       ***************************
       */
    if (p->portInstInfo[instIndex].rbWhile != 2*dot1sInstance->cist.CistBridgeTimes.bridgeHelloTime &&
        p->portInstInfo[instIndex].role == ROLE_BACKUP)
    {
      /*Send Event E18*/
      rc = dot1sStateMachineClassifier(prtAltbkRoleBkupRbWhile, p, instIndex, L7_NULL, L7_NULLPTR);
    }

    reRooted = isRerooted(p,instIndex);

    if (p->portInstInfo[instIndex].rbWhile == 0 &&
        p->portInstInfo[instIndex].learn == L7_FALSE &&
        p->portInstInfo[instIndex].role == ROLE_ROOT &&
        reRooted == L7_TRUE &&
        bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W)
    {
      /*Send Event E39*/
      /*if (DOT1S_PROTO_DEBUG() == L7_TRUE)
      {
         SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :  rbWhile %d Setting prtLearn Port(%d) Inst(%d) \n", __FUNCTION__ , p->portNum, instIndex);
      } */
      rc = dot1sStateMachineClassifier(prtRootLearn, p, instIndex, L7_NULL, L7_NULLPTR);
    }
    if (p->portInstInfo[instIndex].rbWhile == 0 &&
        p->portInstInfo[instIndex].learn == L7_TRUE &&
        p->portInstInfo[instIndex].forward == L7_FALSE &&
        p->portInstInfo[instIndex].role == ROLE_ROOT &&
        reRooted == L7_TRUE &&
        bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W)
    {
      /*Send Event E40*/
      rc = dot1sStateMachineClassifier(prtRootForward, p, instIndex, L7_NULL, L7_NULLPTR);
    }


    /****************************
       *rrwhile timer events*
       ***************************
       */
    if (p->portInstInfo[instIndex].rrWhile != bridge->FwdDelay &&
        p->portInstInfo[instIndex].role == ROLE_ROOT
       )
    {
      /*Send Event E35*/
      rc = dot1sStateMachineClassifier(prtRootRrWhileNotEqualFwd, p, instIndex, L7_NULL, L7_NULLPTR);
    }
    if (p->portInstInfo[instIndex].rrWhile == 0 &&
        p->portInstInfo[instIndex].reRoot == L7_TRUE)
    {
      /*Send Event E37*/
      if (p->portInstInfo[instIndex].role == ROLE_DESIGNATED)
      {
        if (DOT1S_DEBUG(DOT1S_DEBUG_STATE_MACHINE,instIndex))
        {
          SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s Generating prtDesigReRootRrWhileZero \n", __FUNCTION__);
        }
        rc = dot1sStateMachineClassifier(prtDesigReRootRrWhileZero, p, instIndex, L7_NULL, L7_NULLPTR);
      }
      else if (p->portInstInfo[instIndex].role == ROLE_MASTER)
      {
        rc = dot1sStateMachineClassifier(prtMasterReRootRrWhileZero, p, instIndex, L7_NULL, L7_NULLPTR);
      }
    }

    if (instIndex == DOT1S_CIST_INDEX)
    {
      reRootedPending = &dot1sInstance->cist.reRootedPending;
    }
    else
    {
      reRootedPending = &dot1sInstance->msti[instIndex].reRootedPending;
    }

    if ((p->portInstInfo[instIndex].rrWhile == 0) &&
        p->portNum == (*reRootedPending))
    {
      generateRequestedReRootedPortEvents(instIndex);
    }

    if (p->portInstInfo[instIndex].rrWhile != 0 &&
        (p->portInstInfo[instIndex].learn == L7_TRUE ||
         p->portInstInfo[instIndex].forward == L7_TRUE) &&
        p->operEdge == L7_FALSE &&
        p->portInstInfo[instIndex].reRoot == L7_TRUE)
    {
      /*Send Event E38*/
      if (p->portInstInfo[instIndex].role == ROLE_DESIGNATED)
      {
        rc = dot1sStateMachineClassifier(prtDesigListen, p, instIndex, L7_NULL, L7_NULLPTR);
      }
      else if (p->portInstInfo[instIndex].role == ROLE_MASTER)
      {
        rc = dot1sStateMachineClassifier(prtMasterListen, p, instIndex, L7_NULL, L7_NULLPTR);
      }
    }
    if (p->portInstInfo[instIndex].rrWhile == 0 &&
        p->portInstInfo[instIndex].learn == L7_FALSE &&
        p->portInstInfo[instIndex].role == ROLE_DESIGNATED &&
        bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W &&
        (p->portInstInfo[instIndex].agreed == L7_TRUE ||
         p->operEdge == L7_TRUE) &&
        p->portInstInfo[instIndex].sync == L7_FALSE)
    {
      /*Send event E39*/
      /*if (DOT1S_PROTO_DEBUG() == L7_TRUE)
      {
         SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : rrwhile Setting prtLearn Port(%d) Inst(%d) \n", __FUNCTION__ , p->portNum, instIndex);
      } */
      rc = dot1sStateMachineClassifier(prtDesigLearn, p, instIndex, L7_NULL, L7_NULLPTR);
    }


    if (p->portInstInfo[instIndex].rrWhile == 0 &&
        p->portInstInfo[instIndex].learn == L7_TRUE &&
        p->portInstInfo[instIndex].forward == L7_FALSE &&
        bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W &&
        p->portInstInfo[instIndex].role == ROLE_DESIGNATED &&
        (p->portInstInfo[instIndex].agreed == L7_TRUE ||
         p->operEdge == L7_TRUE) &&
        p->portInstInfo[instIndex].sync == L7_FALSE)
    {
      /*Send event E40*/
      rc = dot1sStateMachineClassifier(prtDesigForward, p, instIndex, L7_NULL, L7_NULLPTR);
    }

  }/*end if (p->portInstInfo[instIndex].selected == L7_TRUE &&....*/

  /***************************
   *generate events for ptx*
   **************************
   */

  if(p->pduTxStopCheck == L7_TRUE)
  {
    dot1sNeedForBpduTxStopCheck(p);
  }
  /*all events are qualified with &&selected && !updtInfo*/
  for (j = 0; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].selected == L7_FALSE)
      {
        allSelected = L7_FALSE;
        break;
      }
  }
  for (j = 0; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
  {
    if (dot1sInstanceMap[j].inUse == L7_TRUE)
      if (p->portInstInfo[j].updtInfo == L7_TRUE)
      {
        allUpdtInfo = L7_TRUE;
        break;
      }
  }

  if (allSelected == L7_TRUE &&
      allUpdtInfo == L7_FALSE)
  {
    if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_ROOT)
      cistRootPort = L7_TRUE;
    if (p->portInstInfo[DOT1S_CIST_INDEX].role == ROLE_DESIGNATED)
      cistDesignatedPort = L7_TRUE;

    for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
        if (p->portInstInfo[j].role == ROLE_ROOT)
        {
          mstiRootPort = L7_TRUE;
          break;
        }
    }
    for (j = 1; j<= L7_MAX_MULTIPLE_STP_INSTANCES; j++)
    {
      if (dot1sInstanceMap[j].inUse == L7_TRUE)
        if (p->portInstInfo[j].role == ROLE_DESIGNATED)
        {
          mstiDesignatedPort = L7_TRUE;
          break;
        }
    }


    if (p->sendRSTP == L7_TRUE &&
        ((p->newInfoCist == L7_TRUE &&
          (cistRootPort == L7_TRUE || cistDesignatedPort == L7_TRUE)) ||
         (p->newInfoMist == L7_TRUE && (mstiRootPort == L7_TRUE || mstiDesignatedPort == L7_TRUE))) &&
        (p->txCount < bridge->TxHoldCount) && (p->helloWhen != 0)
       )
    {
      /*Send Event E66*/
      rc = dot1sStateMachineClassifier(ptxSendRstp, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }
    if (p->sendRSTP == L7_FALSE &&
        p->newInfoCist == L7_TRUE &&
        cistRootPort == L7_TRUE &&
        p->txCount < bridge->TxHoldCount &&
        p->helloWhen != 0
       )
    {
      /*Send Event E67*/
      rc = dot1sStateMachineClassifier(ptxSendTcn, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }
    else if (p->sendRSTP == L7_FALSE &&
             p->newInfoCist == L7_TRUE &&
             cistDesignatedPort == L7_TRUE &&
             p->txCount < bridge->TxHoldCount &&
             p->helloWhen != 0
            )
    {
      /*Send Event E68*/
      rc = dot1sStateMachineClassifier(ptxSendConfig, p, L7_NULL, L7_NULL, L7_NULLPTR);
    }


    if (p->edgeDelayWhile == 0 &&
        p->autoEdge == L7_TRUE &&
        p->sendRSTP == L7_TRUE &&
        p->portInstInfo[DOT1S_CIST_INDEX].proposing == L7_TRUE
       )
    {
      if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_BDM))
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Generating event bdmEdgeDelayWhileZero \n");
      }

      rc = dot1sStateMachineClassifier(bdmEdgeDelayWhileZero, p, L7_NULL, L7_NULL, L7_NULLPTR);

    }
    else if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_BDM))
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," port (%d) edge delay while %d autoEdge %d send RSTP %d proposing %d \n",
                    p->portNum, p->edgeDelayWhile, p->autoEdge, p->sendRSTP , p->portInstInfo[DOT1S_CIST_INDEX].proposing);



  }/*end if (allSelected == L7_TRUE &&....*/
  return rc;
}
/*********************************************************************
* @purpose  Based on the MSTI message received the mstiMastered param
*           set or cleared
*
* @param    p          @b{(input)} pointer to port entry
* @param    instIndex  @b{(input)} index of the instance
* @param    mstiMsg    @b{(input)} pointer to MSTI message

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dot1sMasteredMstiRecord(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  if ((pointTopointCheck == L7_TRUE) &&
      (p->portInstInfo[instIndex].rcvdBpduFlags & DOT1S_MASTER_FLAG_MASK))
  {
    p->portInstInfo[instIndex].inst.msti.mstiMastered = L7_TRUE;
  }
  else
    p->portInstInfo[instIndex].inst.msti.mstiMastered = L7_FALSE;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Compare for New Root and send a trap if this bridge is to
*           become the root bridge.
*
* @param    bridgeId    @b{(input)} this bridge Id
* @param    rootId      @b{(input)} current root Id
* @param    bestId      @b{(input)} new best Id
* @param    instIndex   @b{(input)} instance index
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sNewRootCheck(DOT1S_BRIDGEID_t bridgeId,
                          DOT1S_BRIDGEID_t rootId,
                          DOT1S_BRIDGEID_t bestId,
                          L7_uint32 instIndex)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 instNum;

  /* compare the best bridge id with this bridge id */
  if (dot1sIdEqual(bestId, bridgeId))
  {
    /* this bridge is or will become the root */

    /* compare current root with this bridge id */
    if (dot1sIdNotEqual(rootId, bridgeId))
    {
      /* this bridge is not the current root */

      if ((rc = dot1sInstNumFind(instIndex, &instNum)) != L7_FAILURE)
      {
        /* this bridge will become the Designated root now,
         * issue the NewRoot Trap for the instance
         */
        rc = trapMgrStpElectedAsRootLogTrap();
      }
    }
  }

  /* compare current root with best bridge id */
  if (dot1sIdNotEqual(rootId, bestId))
  {
    /* root is changing */
    if ((rc = dot1sInstNumFind(instIndex, &instNum)) != L7_FAILURE)
    {
      rc = trapMgrStpNewRootElectedLogTrap(instNum, (L7_uchar8 *)&bestId);
    }
  }


  return(rc);
}
/*********************************************************************
* @purpose  Compute the agreed flag for this port and instance
*
* @param    p         @b{(input)} pointer to the port structure
* @param    instIndex @b{(input)} instance index of this MSTI Instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The agreed flag for this port and instance will be cleared if
*           the CIST Root Id, CIST Ext Root Path Cost, and CIST Reg Root Id
*           in the received BPDU are not the same as those for the CIST
*           designated pri vector for this port. The CIST Dsg pri vector has
*           already been computed, as the CIST msg is processed before any
*           MSTI msg. The CIST msg is recorded in the
*           p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority
*
*           UPDATE: dot1Q -REV4
*           The agree flag for this port and instance will be cleared if
*           the CIST Root Id, CIST Ext Root Path Cost, and CIST Reg Root Id
*           in the received BPDU are not better than or same as those for the CIST
*           designated pri vector for this port. The CIST Dsg pri vector has
*           already been computed, as the CIST msg is processed before any
*           MSTI msg. The CIST msg is recorded in the
*           p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority
*
*
*
* @end
*********************************************************************/
L7_RC_t dot1sAgreeFlagCompute(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  /* Note we are checking if Desig prio is better than Msg prio
     Which will make the Msg Prio not better or not the same as the desig prio */
  if ( dot1sIdLesser(p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.rootId,
                     p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority.rootId )
       ||
       ( p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.extRootPathCost >
         p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority.extRootPathCost) ||
       dot1sIdLesser(p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedPriority.regRootId,
                     p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistMsgPriority.regRootId)
     )
  {
    p->portInstInfo[instIndex].agree = L7_FALSE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Compute the master flag for this port and instance
*
* @param    p         @b{(input)} pointer to the port structure
* @param    instIndex @b{(input)} instance index of this MSTI Instance
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The master flag is set to L7_TRUE if the port role for this port
*           and instance is Dsg or root and the device has selected one of
*           its ports as the Master role or the mstiMastered flag is set for
*           this MSTI for any other device port with a root or dsg port role.
*           It is set false otherwise.
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiMasterFlagCompute(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_BOOL anyPortMaster = L7_FALSE;
  L7_BOOL anyPortMastered = L7_FALSE;
  DOT1S_PORT_COMMON_t *pPort;

  pPort = dot1sIntfFindFirst();
  while (pPort != L7_NULLPTR)
  {
    if (pPort->portInstInfo[instIndex].role == ROLE_MASTER)
    {
      anyPortMaster = L7_TRUE;
      break;
      /* We can afford to break here as the master condition requires
      * either anyPortMaster or anyPortMastered to be TRUE
      */
    }
    if ((pPort->portInstInfo[instIndex].role == ROLE_ROOT ||
         pPort->portInstInfo[instIndex].role == ROLE_DESIGNATED) &&
        pPort->portInstInfo[instIndex].inst.msti.mstiMastered == L7_TRUE)
    {
      anyPortMastered = L7_TRUE;
      break;
      /* We can afford to break here as the master condition requires
      * either anyPortMaster or anyPortMastered to be TRUE
      */
    }

    pPort = dot1sIntfFindNext(pPort->portNum);
  }/* while */

  if ((anyPortMaster == L7_TRUE || anyPortMastered == L7_TRUE) &&
      (p->portInstInfo[instIndex].role == ROLE_ROOT ||
       p->portInstInfo[instIndex].role == ROLE_DESIGNATED)
     )
  {
    p->portInstInfo[instIndex].inst.msti.mstiMaster = L7_TRUE;
  }
  else
  {
    p->portInstInfo[instIndex].inst.msti.mstiMaster = L7_FALSE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Compare two cist rpiority vectors
*
* @param    pv1     @b{(input)} priority vector 1
* @param    pv2     @b{(input)} priority vector 2
*
* @returns  SAME    both are same
* @returns  BETTER  pv1 is better than pv2
* @returns  WORSE   pv1 is worse than pv2
*
* @comments This comparison will compare the rxPortId as well
*
* @end
*********************************************************************/
DOT1S_PRI_COMP_t dot1sCistPriVecAllCompare(DOT1S_CIST_PRIORITY_VECTOR_t *pv1,
                                           DOT1S_CIST_PRIORITY_VECTOR_t *pv2)
{
  DOT1S_PRI_COMP_t  comparison;

  if (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
      (pv1->extRootPathCost == pv2->extRootPathCost) &&
      dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
      (pv1->intRootPathCost == pv2->intRootPathCost) &&
      dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
      (pv1->dsgPortId == pv2->dsgPortId) &&
      (pv1->rxPortId == pv2->rxPortId)
     )
  {
    comparison = SAME;
  }
  else if ((dot1sIdLesser(pv1->rootId, pv2->rootId)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost < pv2->extRootPathCost)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost == pv2->extRootPathCost) &&
            dot1sIdLesser(pv1->regRootId, pv2->regRootId)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost == pv2->extRootPathCost) &&
            dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
            (pv1->intRootPathCost < pv2->intRootPathCost)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost == pv2->extRootPathCost) &&
            dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
            (pv1->intRootPathCost == pv2->intRootPathCost) &&
            dot1sIdLesser(pv1->dsgBridgeId, pv2->dsgBridgeId)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost == pv2->extRootPathCost) &&
            dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
            (pv1->intRootPathCost == pv2->intRootPathCost) &&
            dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
            (pv1->dsgPortId < pv2->dsgPortId)) ||
           (dot1sIdEqual(pv1->rootId, pv2->rootId) &&
            (pv1->extRootPathCost == pv2->extRootPathCost) &&
            dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
            (pv1->intRootPathCost == pv2->intRootPathCost) &&
            dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
            (pv1->dsgPortId == pv2->dsgPortId) &&
            (pv1->rxPortId < pv2->rxPortId))
          )
  {
    comparison = BETTER;
  }
  else
  {
    comparison = WORSE;
  }

  return comparison;


}
/*********************************************************************
* @purpose  Compare two msti rpiority vectors
*
* @param    pv1     @b{(input)} priority vector 1
* @param    pv2     @b{(input)} priority vector 2
*
* @returns  SAME    both are same
* @returns  BETTER  pv1 is better than pv2
* @returns  WORSE   pv1 is worse than pv2
*
* @comments This comparison will compare the rxPortId as well
*
* @end
*********************************************************************/
DOT1S_PRI_COMP_t dot1sMstiPriVecAllCompare(DOT1S_MSTI_PRIORITY_VECTOR_t *pv1, DOT1S_MSTI_PRIORITY_VECTOR_t *pv2)
{
  DOT1S_PRI_COMP_t  comparison;

  if ((dot1sIdLesser(pv1->regRootId, pv2->regRootId)
      ) ||
      (dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
       (pv1->intRootPathCost < pv2->intRootPathCost)
      ) ||
      (dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
       (pv1->intRootPathCost == pv2->intRootPathCost) &&
       dot1sIdLesser(pv1->dsgBridgeId, pv2->dsgBridgeId)
      ) ||
      (dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
       (pv1->intRootPathCost == pv2->intRootPathCost) &&
       dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
       (pv1->dsgPortId < pv2->dsgPortId)
      ) ||
      (dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
       (pv1->intRootPathCost == pv2->intRootPathCost) &&
       dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
       (pv1->dsgPortId == pv2->dsgPortId) &&
       (pv1->rxPortId < pv2->rxPortId)
      )
     )
  {
    comparison = BETTER;
  }
  else if (dot1sIdEqual(pv1->regRootId, pv2->regRootId) &&
           (pv1->intRootPathCost == pv2->intRootPathCost) &&
           dot1sIdEqual(pv1->dsgBridgeId, pv2->dsgBridgeId) &&
           (pv1->dsgPortId == pv2->dsgPortId) &&
           (pv1->rxPortId == pv2->rxPortId)
          )
  {
    comparison = SAME;
  }
  else
  {
    comparison = WORSE;
  }

  return comparison;
}

/*********************************************************************
* @purpose  Calculate the value of the allSynced variable
*
* @param    portNum         @b{(input)} port Number of the requesting port
* @param    instIndex       @b{(input)} instIndex
* @param    role_alternate  @b{(input)} L7_BOOL true if the role of the requesting port is alternate
*
* @returns  TRUE    if All Synced is true per definition below
* @returns  FALSE   otherwise
*
* @comments
*
* @end
*********************************************************************/
static L7_BOOL isAllSynced(L7_uint32 portNum, L7_uint32 instIndex, L7_BOOL role_alternate)
{
  L7_uint32 *allSyncedRequested = L7_NULLPTR;
  L7_uint32 *syncedPending = L7_NULLPTR;
  DOT1S_PORT_COMMON_t *dot1sIntf;
  L7_BOOL checkPort = L7_TRUE;

  /*

   The condition allSynced is TRUE for a given Port, for a given Tree, if and only if:

   a) for all Ports for the given Tree, selected is TRUE,
      the Ports role is the same as its selectedRole,
      and updtInfo is FALSE; and

   b) the role of the given Port is

  1) Root Port or Alternate Port, and synced is TRUE for all Ports for the given Tree
     other than the Root Port; or

  2) Designated Port, and synced is TRUE for all Ports for the given Tree other than the given Port; or

  3) Master Port, and synced is TRUE for all Ports for the given Tree other than the given Port.


  */

  if (instIndex == DOT1S_CIST_INDEX)
  {
    allSyncedRequested = &dot1sInstance->cist.allSyncedRequested;
    syncedPending = &dot1sInstance->cist.syncedPending;
  }
  else
  {
    allSyncedRequested = &dot1sInstance->msti[instIndex].allSyncedRequested;
    syncedPending = &dot1sInstance->msti[instIndex].syncedPending;
  }

  dot1sIntf = dot1sIntfFindFirst();
  while (dot1sIntf != L7_NULLPTR)
  {

    if (dot1sIntf->portUpEnabledNum != 0 &&
        ((dot1sIntf->portInstInfo[instIndex].selected != L7_TRUE ||
          dot1sIntf->portInstInfo[instIndex].role != dot1sIntf->portInstInfo[instIndex].selectedRole ||
          dot1sIntf->portInstInfo[instIndex].updtInfo == L7_TRUE)  ||
         dot1sIntf->portInstInfo[instIndex].synced == L7_FALSE))
    {

      if (dot1sIntf->portInstInfo[instIndex].synced == L7_FALSE)
      {
        if (role_alternate == L7_TRUE )
        {
          if (dot1sIntf->portInstInfo[instIndex].role == ROLE_ROOT)
          {
            checkPort = L7_FALSE;
          }
        }
        else if (portNum == dot1sIntf->portNum)
        {
          checkPort = L7_FALSE;
        }

      }

      /* If no one has requested allSynced request to be notified */
      if (checkPort == L7_TRUE)
      {
          *syncedPending = dot1sIntf->portNum;
          if (*allSyncedRequested == 0)
          {
            *allSyncedRequested = portNum;

          }
          return L7_FALSE;
      }
    }
    dot1sIntf = dot1sIntfFindNext(dot1sIntf->portNum);
  }/* while*/
  syncInProgress[instIndex] = L7_FALSE;
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Calculate the value of the allSynced variable
*
* @param    portNum         @b{(input)} port Number of the requesting port
* @param    instIndex       @b{(input)} instIndex
* @param    role_alternate  @b{(input)} L7_BOOL true if the role of the requesting port is alternate
*
* @returns  TRUE    if All Synced is true per definition below
* @returns  FALSE   otherwise
*
* @comments
*
* @end
*********************************************************************/
static L7_uint32 forwardDelayGet(DOT1S_PORT_COMMON_t *p)
{

  if (p->sendRSTP)
  {
    return p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.helloTime;
  }

  return p->portInstInfo[DOT1S_CIST_INDEX].inst.cist.cistDesignatedTimes.fwdDelay;
}



L7_RC_t generateEventsForBDM(DOT1S_PORT_COMMON_t *p)
{
  L7_RC_t rc = L7_SUCCESS;

  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  switch (p->bdmState)
  {
    case BDM_EDGE:
      {
        if (p->portEnabled == L7_FALSE && p->adminEdge == L7_FALSE)
        {
          if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_BDM))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Generating event bdmNotPortEnabledNotAdminEdge \n");
          }

          rc = dot1sStateMachineClassifier(bdmNotPortEnabledNotAdminEdge, p, L7_NULL, L7_NULL, L7_NULLPTR);
        }

        if (p->operEdge == L7_FALSE)
        {
          if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_BDM))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Generating event bdmNotOperEdge \n");
          }


          rc = dot1sStateMachineClassifier(bdmNotOperEdge, p, L7_NULL, L7_NULL, L7_NULLPTR);
        }

      }
      break;
    case BDM_NOT_EDGE:
      {
        if (p->portEnabled == L7_FALSE && p->adminEdge == L7_TRUE)
        {
          if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_BDM))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Generating event bdmNotPortEnabledAdminEdge \n");
          }

          rc = dot1sStateMachineClassifier(bdmNotPortEnabledAdminEdge, p, L7_NULL, L7_NULL, L7_NULLPTR);

        }


        if (p->edgeDelayWhile == 0 &&
            p->autoEdge == L7_TRUE &&
            p->sendRSTP == L7_TRUE &&
            p->portInstInfo[DOT1S_CIST_INDEX].proposing == L7_TRUE
           )
        {
          if (DOT1S_DEBUG_COMMON(DOT1S_DEBUG_BDM))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Generating event bdmEdgeDelayWhileZero \n");
          }

          rc = dot1sStateMachineClassifier(bdmEdgeDelayWhileZero, p, L7_NULL, L7_NULL, L7_NULLPTR);

        }

      }
      break;

    default:
      break;
  }





  return rc;


}


L7_BOOL isRerooted(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_uint32 i = 0;
  L7_BOOL reRooted = L7_FALSE;
  DOT1S_PORT_COMMON_t *dot1sIntf;

  L7_uint32 *reRootedRequested = L7_NULLPTR;
  L7_uint32 *reRootedPending = L7_NULLPTR;

  if (instIndex == DOT1S_CIST_INDEX)
  {
    reRootedRequested = &dot1sInstance->cist.reRootedRequested;
    reRootedPending = &dot1sInstance->cist.reRootedPending;
  }
  else
  {
    reRootedRequested = &dot1sInstance->msti[instIndex].reRootedRequested;
    reRootedPending = &dot1sInstance->msti[instIndex].reRootedPending;
  }




  if (((p->portInstInfo[instIndex].learn == L7_FALSE) &&
       (p->portInstInfo[instIndex].role == ROLE_ROOT)) ||
      ((p->portInstInfo[instIndex].learn == L7_TRUE) &&
       (p->portInstInfo[instIndex].forward == L7_FALSE) &&
       (p->portInstInfo[instIndex].role == ROLE_ROOT)))
  {
    reRooted = L7_TRUE;
    /*for (i = 1; i<= ((L7_uint32)L7_MAX_INTERFACE_COUNT); i++)*/
    dot1sIntf = dot1sIntfFindFirst();
    while (dot1sIntf != L7_NULLPTR)
    {
      if (dot1sIntf->portUpEnabledNum != 0 &&
          p->portNum != dot1sIntf->portUpEnabledNum &&
          dot1sIntf->portInstInfo[instIndex].rrWhile != 0 )
      {
        reRooted = L7_FALSE;

        /* If no one has requested reRooted request to be notified */
        *reRootedPending = i;
        if (*reRootedRequested == 0)
        {
          *reRootedRequested = p->portNum;
        }
        break;
      }
      dot1sIntf = dot1sIntfFindNext(dot1sIntf->portNum);
    }/* while */

  }

  return reRooted;
}

L7_RC_t dot1sGenerateEventsForPrt(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{

  L7_RC_t       rc;
  L7_uint32     id/*,i*/;
  L7_BOOL       allSynced = L7_FALSE;
  DOT1S_BRIDGE_t  *bridge;
  L7_BOOL       reRooted;
  L7_uint32         *syncedPending = L7_NULLPTR;
  L7_uint32         *reRootedPending = L7_NULLPTR;
  /*DOT1S_PORT_COMMON_t *syncPort = L7_NULLPTR; */
  DOT1S_PORT_ROLE_t role;
  L7_uint32 prtState;

  bridge = dot1sBridgeParmsFind();
  rc = dot1sInstNumFind(instIndex, &id);

  /**************************
  * generate events for prt
  **************************
  */

  if (rc != L7_SUCCESS || !p)
  {
    return L7_FAILURE;
  }

  role = p->portInstInfo[instIndex].selectedRole;
  prtState = p->portInstInfo[instIndex].prtState;

  /* De Morgan's theroem !(A & B) = !A || !B */
  if (p->portInstInfo[instIndex].selected == L7_FALSE ||
      p->portInstInfo[instIndex].updtInfo == L7_TRUE)
  {
    /* All the events except UCT are qualified by selected and not updt info */
    return L7_SUCCESS;

  }

  if (role != p->portInstInfo[instIndex].role)
  {
    if (DOT1S_DEBUG(DOT1S_DEBUG_PROTO_ROLE,instIndex))
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: Port (%d) inst(%d) role changing from %s to %s \n",__FUNCTION__,
                    p->portNum, instIndex,
                    dot1sRoleStringGet(p->portInstInfo[instIndex].role), dot1sRoleStringGet(role));

    }

    if ((p->portInstInfo[instIndex].role == ROLE_ROOT) ||
        (p->portInstInfo[instIndex].role == ROLE_DESIGNATED))
    {
      p->pduTxStopCheck = L7_TRUE;
    }

  }

  if (instIndex == DOT1S_CIST_INDEX)
  {
    syncedPending = &dot1sInstance->cist.syncedPending;
    reRootedPending = &dot1sInstance->cist.reRootedPending;
  }
  else
  {
    syncedPending = &dot1sInstance->msti[instIndex].syncedPending;
    reRootedPending = &dot1sInstance->msti[instIndex].reRootedPending;
  }


  /*all events are qualified with &&selected && !updtInfo*/

  switch (role)
  {

    case  ROLE_DISABLED:
      {
        if (role != p->portInstInfo[instIndex].role )
        {
          rc = dot1sStateMachineClassifier(prtDisabledPortRoleNotEqualSelectedRole, p, instIndex,
                                           L7_NULL, L7_NULLPTR);
        }

        else if (prtState == PRT_DISABLE_PORT)
        {
          if (p->portInstInfo[instIndex].learning == L7_FALSE &&
              p->portInstInfo[instIndex].forwarding == L7_FALSE)
          {
            rc = dot1sStateMachineClassifier(prtDisabledNotLearningNotForwarding,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

        }
        else if (prtState == PRT_DISABLED_PORT)
        {
          if (p->portInstInfo[instIndex].sync == L7_TRUE )
          {
            rc = dot1sStateMachineClassifier(prtDisabledSync,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }
          if (p->portInstInfo[instIndex].reRoot == L7_TRUE)
          {
            rc = dot1sStateMachineClassifier(prtDisabledReroot,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }
          if (p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            rc = dot1sStateMachineClassifier(prtDisabledNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].synced ==L7_TRUE &&
              p->portNum == (*syncedPending))
          {
            generateRequestedPortEvents(instIndex);

          }

          if ((p->portInstInfo[instIndex].rrWhile == 0) &&
              p->portNum == (*reRootedPending))
          {
            generateRequestedReRootedPortEvents(instIndex);
          }



        }
      }
      break;
    case  ROLE_ROOT:
      {
        if (role != p->portInstInfo[instIndex].role)
        {
          rc = dot1sStateMachineClassifier(prtRootPortRoleNotEqualSelectedRole, p, instIndex,
                                           L7_NULL, L7_NULLPTR);
        }

        else if (prtState == PRT_ROOT_PORT)
        {
          /* TBD -All synced */
          if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   Port (%d) inst(%d) prt state(%d) fdWhile(%d), learn %d fwd %d \n",
                          __FUNCTION__, p->portNum, instIndex, p->portInstInfo[instIndex].prtState,
                          p->portInstInfo[instIndex].fdWhile,
                          p->portInstInfo[instIndex].learn, p->portInstInfo[instIndex].forward);

          }

          if (p->portInstInfo[instIndex].proposed == L7_TRUE &&
              p->portInstInfo[instIndex].agree == L7_FALSE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootProposedNotAgree\n", __FUNCTION__);
            }

            rc = dot1sStateMachineClassifier(prtRootProposedNotAgree,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }
          if (p->portInstInfo[instIndex].proposed == L7_TRUE &&
              p->portInstInfo[instIndex].agree == L7_TRUE )
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootProposedAgree\n", __FUNCTION__);
            }


            rc = dot1sStateMachineClassifier(prtRootProposedAgree,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }


          if ( p->portInstInfo[instIndex].agree == L7_FALSE)
          {
            allSynced = isAllSynced(p->portNum,instIndex,L7_FALSE);
            if (allSynced == L7_TRUE)
            {
              if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
              {
                SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootAllSyncedNotAgree\n", __FUNCTION__);
              }


              rc = dot1sStateMachineClassifier(prtRootAllSyncedNotAgree,p,instIndex,
                                               L7_NULL, L7_NULLPTR);
            }
          }

          if (p->portInstInfo[instIndex].agreed == L7_TRUE &&
              p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootAgreedNotSynced\n", __FUNCTION__);
            }


            rc = dot1sStateMachineClassifier(prtRootAgreedNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].sync == L7_TRUE &&
              p->portInstInfo[instIndex].synced == L7_TRUE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootSyncSynced\n", __FUNCTION__);
            }


            rc = dot1sStateMachineClassifier(prtRootSyncSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].forward ==L7_FALSE &&
              p->portInstInfo[instIndex].reRoot == L7_FALSE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootNotFwdNotReRoot\n", __FUNCTION__);
            }


            rc = dot1sStateMachineClassifier(prtRootNotFwdNotReRoot,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].reRoot == L7_TRUE &&
              p->portInstInfo[instIndex].forward == L7_TRUE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootReRootFwd\n", __FUNCTION__);
            }


            rc = dot1sStateMachineClassifier(prtRootReRootFwd,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].synced ==L7_TRUE &&
              p->portNum == (*syncedPending))
          {
            generateRequestedPortEvents(instIndex);

          }

          if (p->portInstInfo[instIndex].fdWhile == 0)
          {



            if (p->portInstInfo[instIndex].learn == L7_FALSE)
            {

              if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
              {
                SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootLearn\n", __FUNCTION__);
              }


              rc = dot1sStateMachineClassifier(prtRootLearn,p,instIndex,
                                               L7_NULL, L7_NULLPTR);
            }
            else if (p->portInstInfo[instIndex].forward == L7_FALSE)
            {
              if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
              {
                SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootForward 1\n", __FUNCTION__);
              }


              rc = dot1sStateMachineClassifier(prtRootForward,p,instIndex,
                                               L7_NULL, L7_NULLPTR);
            }
          }
          else if (bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W  &&
                   p->portInstInfo[instIndex].rbWhile == 0 &&
                   (p->portInstInfo[instIndex].learn == L7_FALSE ||
                    p->portInstInfo[instIndex].forward == L7_FALSE )
                  )
          {
            reRooted = isRerooted(p, instIndex);
            if (reRooted == L7_TRUE)
            {
              if (p->portInstInfo[instIndex].learn == L7_FALSE)
              {
                if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
                {
                  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :  prt state(%d) rbWhile %d, learn %d fwd %d rerooted %d \n", __FUNCTION__,
                                p->portInstInfo[instIndex].prtState, p->portInstInfo[instIndex].rbWhile,
                                p->portInstInfo[instIndex].learn,
                                p->portInstInfo[instIndex].forward, reRooted);

                }
                rc = dot1sStateMachineClassifier(prtRootLearn,p,instIndex,
                                                 L7_NULL, L7_NULLPTR);
              }
              else if (p->portInstInfo[instIndex].forward == L7_FALSE)
              {
                if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
                {
                  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s :   generating  prtRootForward 2\n", __FUNCTION__);
                }


                rc = dot1sStateMachineClassifier(prtRootForward,p,instIndex,
                                                 L7_NULL, L7_NULLPTR);
              }
            }/*if reRooted*/
          }


          if (DOT1S_DEBUG(DOT1S_DEBUG_ROOT_PRT,instIndex))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Finish   prt state(%d) learn %d fwd %d \n",
                          __FUNCTION__, p->portInstInfo[instIndex].prtState,
                          p->portInstInfo[instIndex].learn, p->portInstInfo[instIndex].forward);

          }
        }
      }
      break;
    case ROLE_DESIGNATED:
      {
        if (role != p->portInstInfo[instIndex].role)
        {

          rc = dot1sStateMachineClassifier(prtDesigPortRoleNotEqualSelectedRole, p, instIndex,
                                           L7_NULL, L7_NULLPTR);
        }

        else if (prtState == PRT_DESIG_PORT)
        {
          /* TBD -All synced */

          if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : case role designated prt state PRT_DESIG_PORT \n", __FUNCTION__);
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"Port(%d) inst(%d) forward(%d) agreed(%d) proposing(%d) operedge(%d) \n",
                          p->portNum,instIndex,p->portInstInfo[instIndex].forward,p->portInstInfo[instIndex].agreed,
                          p->portInstInfo[instIndex].proposing, p->operEdge);

            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"agree(%d) proposed(%d) reroot(%d)\n", p->portInstInfo[instIndex].agree,
                          p->portInstInfo[instIndex].proposed, p->portInstInfo[instIndex].reRoot);


          }


          if ((p->portInstInfo[instIndex].forward == L7_FALSE) &&
              (p->portInstInfo[instIndex].agreed == L7_FALSE) &&
              (p->portInstInfo[instIndex].proposing == L7_FALSE) &&
              (p->operEdge == L7_FALSE))
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigNotFwdNotAgNotPropNotEdge \n");
            }
            rc = dot1sStateMachineClassifier(prtDesigNotFwdNotAgNotPropNotEdge,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if ( p->portInstInfo[instIndex].agree == L7_FALSE ||
               p->portInstInfo[instIndex].proposed == L7_TRUE)
          {
            allSynced = L7_TRUE;
            allSynced = isAllSynced(p->portNum,instIndex,L7_FALSE);
            if (allSynced == L7_TRUE)
            {
              if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
              {
                SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigAllSynced \n");
              }
              rc = dot1sStateMachineClassifier(prtDesigAllSynced,p,instIndex,
                                               L7_NULL, L7_NULLPTR);
            }
          }

          if (p->portInstInfo[instIndex].learning == L7_FALSE &&
              p->portInstInfo[instIndex].forwarding == L7_FALSE &&
              p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigNotLrnNotFwdNotSynced \n");
            }
            rc = dot1sStateMachineClassifier(prtDesigNotLrnNotFwdNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].agreed == L7_TRUE &&
              p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigAgreedNotSynced \n");
            }
            rc = dot1sStateMachineClassifier(prtDesigAgreedNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->operEdge == L7_TRUE &&
              p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigEdgeNotSynced \n");
            }
            rc = dot1sStateMachineClassifier(prtDesigEdgeNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].sync == L7_TRUE &&
              p->portInstInfo[instIndex].synced == L7_TRUE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigSyncSynced \n");
            }
            rc = dot1sStateMachineClassifier(prtDesigSyncSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }
          if (p->portInstInfo[instIndex].rrWhile == 0 &&
              p->portInstInfo[instIndex].reRoot == L7_TRUE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigReRootRrWhileZero \n");
            }
            rc = dot1sStateMachineClassifier(prtDesigReRootRrWhileZero,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }



          if (p->portInstInfo[instIndex].synced ==L7_TRUE &&
              p->portNum == (*syncedPending))
          {
            generateRequestedPortEvents(instIndex);

          }

          if ((p->portInstInfo[instIndex].learn ==L7_TRUE ||
               p->portInstInfo[instIndex].forward == L7_TRUE) &&
              p->operEdge == L7_FALSE &&
              ((p->loopInconsistent == L7_TRUE) ||
               (p->portInstInfo[instIndex].sync == L7_TRUE &&
                p->portInstInfo[instIndex].synced == L7_FALSE) ||
               ((p->portInstInfo[instIndex].rrWhile == 0) &&
                p->portInstInfo[instIndex].reRoot == L7_TRUE) ||
               p->portInstInfo[instIndex].disputed == L7_TRUE
              )
             )
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigListen sync (%d) synced(%d) disputed (%d) rrwhile %d\n",
                            p->portInstInfo[instIndex].sync,p->portInstInfo[instIndex].synced,
                            p->portInstInfo[instIndex].disputed, p->portInstInfo[instIndex].rrWhile);
            }
            rc = dot1sStateMachineClassifier(prtDesigListen,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].learn == L7_FALSE &&
              p->portInstInfo[instIndex].sync == L7_FALSE  &&
              p->loopInconsistent == L7_FALSE &&
              ((p->portInstInfo[instIndex].fdWhile == 0) ||
               /*((bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W ) &&*/
               (p->portInstInfo[instIndex].agreed == L7_TRUE ||
                p->operEdge == L7_TRUE)) &&
              ((p->portInstInfo[instIndex].rrWhile == 0) ||
               p->portInstInfo[instIndex].reRoot == L7_FALSE)
             )
          {
            /*send event E39*/
            /*if (DOT1S_PROTO_DEBUG() == L7_TRUE)
            {
               SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Setting prtLearn Port(%d) Inst(%d) \n",
                                 __FUNCTION__, p->portNum, instIndex);
               SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"role (%d) Sync (%d) AllSynced (%d) agreed (%d)\n",
                      p->portInstInfo[instIndex].role,p->portInstInfo[instIndex].sync,allSynced,
                      p->portInstInfo[instIndex].agreed);
            }
*/
            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Sync %d fdWhile %d agreed %d operEdge %d  rrWhile %d reRoot %d \n",
                            p->portInstInfo[instIndex].sync,p->portInstInfo[instIndex].fdWhile,
                            p->portInstInfo[instIndex].agreed, p->operEdge,
                            p->portInstInfo[instIndex].rrWhile, p->portInstInfo[instIndex].reRoot);
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigLearn \n");
            }
            rc = dot1sStateMachineClassifier(prtDesigLearn, p, instIndex, L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].learn == L7_TRUE &&
              p->portInstInfo[instIndex].forward == L7_FALSE &&
              p->portInstInfo[instIndex].sync == L7_FALSE  &&
              p->loopInconsistent == L7_FALSE &&
              ((p->portInstInfo[instIndex].fdWhile == 0) ||
               /*((bridge->ForceVersion >= DOT1S_FORCE_VERSION_DOT1W ) &&*/
               (p->portInstInfo[instIndex].agreed == L7_TRUE ||
                p->operEdge == L7_TRUE)) &&
              ((p->portInstInfo[instIndex].rrWhile == 0) ||
               p->portInstInfo[instIndex].reRoot == L7_FALSE)
             )
          {
            /*send event E39*/
            /*rc = dot1sIssueCmd(prtLearn, p->portNum, id, L7_NULLPTR);*/
            /*if (DOT1S_PROTO_DEBUG() == L7_TRUE)
            {
               SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Setting prtLearn Port(%d) Inst(%d) \n",__FUNCTION__,
                                        p->portNum, instIndex);
               SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"role (%d) Sync (%d) AllSynced (%d) agreed (%d)\n",
                      p->portInstInfo[instIndex].role,p->portInstInfo[instIndex].sync,allSynced,
                      p->portInstInfo[instIndex].agreed);

            } */
            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigForward \n");
            }

            if (DOT1S_DEBUG(DOT1S_DEBUG_DESIG_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS," Sync %d fdWhile %d agreed %d operEdge %d  rrWhile %d reRoot %d \n",
                            p->portInstInfo[instIndex].sync,p->portInstInfo[instIndex].fdWhile,
                            p->portInstInfo[instIndex].agreed, p->operEdge,
                            p->portInstInfo[instIndex].rrWhile, p->portInstInfo[instIndex].reRoot);
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtDesigLearn \n");
            }

            rc = dot1sStateMachineClassifier(prtDesigForward, p, instIndex, L7_NULL, L7_NULLPTR);
          }

          if ((p->portInstInfo[instIndex].rrWhile == 0) &&
              p->portNum == (*reRootedPending))
          {
            generateRequestedReRootedPortEvents(instIndex);
          }
        }
      }
      break;

    case ROLE_BACKUP:
    case ROLE_ALTERNATE:
      {
        dot1sEvents_t temp_event;

        if (role != p->portInstInfo[instIndex].role)
        {
          temp_event = (role == ROLE_ALTERNATE) ? prtAlternatePortRoleNotEqualSelectedRole
                       : prtBackupPortRoleNotEqualSelectedRole;

          if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating %d \n", temp_event);
          }

          rc = dot1sStateMachineClassifier(temp_event, p, instIndex,
                                           L7_NULL, L7_NULLPTR);
        }

        else if (prtState == PRT_ALTERNATE_PORT)
        {
          if (p->portInstInfo[instIndex].proposed == L7_TRUE &&
              p->portInstInfo[instIndex].agree == L7_FALSE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtAltbkProposedNotAgree \n");
            }

            rc = dot1sStateMachineClassifier(prtAltbkProposedNotAgree,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].proposed == L7_TRUE &&
              p->portInstInfo[instIndex].agree == L7_TRUE )
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtAltbkProposedAgree \n");
            }

            rc = dot1sStateMachineClassifier(prtAltbkProposedAgree,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if ( p->portInstInfo[instIndex].agree == L7_FALSE)
          {
            allSynced = L7_TRUE;
            allSynced = isAllSynced(p->portNum,instIndex,L7_TRUE);

            if (allSynced == L7_TRUE)
            {
              if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
              {
                SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtAltbkAllSyncedNotAgree \n");
              }

              rc = dot1sStateMachineClassifier(prtAltbkAllSyncedNotAgree,p,instIndex,
                                               L7_NULL, L7_NULLPTR);
            }
            else if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"All synced is false \n");
            }



          }

          if (p->portInstInfo[instIndex].sync == L7_TRUE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtAltbkSync \n");
            }

            rc = dot1sStateMachineClassifier(prtAltbkSync,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].synced ==L7_TRUE &&
              p->portNum == (*syncedPending))
          {
            generateRequestedPortEvents(instIndex);

          }


          if (p->portInstInfo[instIndex].reRoot == L7_TRUE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtAltbkReroot \n");
            }

            rc = dot1sStateMachineClassifier(prtAltbkReroot,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtAltbkNotSynced \n");
            }

            rc = dot1sStateMachineClassifier(prtAltbkNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].fdWhile != forwardDelayGet(p) )
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtAltbkFdWhileNotFwdDelay \n");
            }

            rc = dot1sStateMachineClassifier(prtAltbkFdWhileNotFwdDelay,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].role == ROLE_BACKUP &&
              p->portInstInfo[instIndex].rbWhile != (2 * dot1sInstance->cist.CistBridgeTimes.bridgeHelloTime) )
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtAltbkRoleBkupRbWhile \n");
            }

            rc = dot1sStateMachineClassifier(prtAltbkRoleBkupRbWhile,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if ((p->portInstInfo[instIndex].rrWhile == 0) &&
              p->portNum == (*reRootedPending))
          {
            generateRequestedReRootedPortEvents(instIndex);
          }



        }
        else if (prtState == PRT_BLOCK_PORT)
        {
          if (p->portInstInfo[instIndex].learning == L7_FALSE &&
              p->portInstInfo[instIndex].forwarding == L7_FALSE)
          {
            if (DOT1S_DEBUG(DOT1S_DEBUG_ALTBKUP_PRT,instIndex))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"generating prtAltbkNotLrngNotFwdg \n");
            }

            rc = dot1sStateMachineClassifier(prtAltbkNotLrngNotFwdg,p,instIndex,
                                             L7_NULL, L7_NULLPTR);

          }
        }


      }
      break;

    case ROLE_MASTER:
      {
        if (role != p->portInstInfo[instIndex].role)
        {
          rc = dot1sStateMachineClassifier(prtMasterPortRoleNotEqualSelectedRole, p, instIndex,
                                           L7_NULL, L7_NULLPTR);
        }

        else if (prtState == PRT_MASTER_PORT)
        {
          if (DOT1S_DEBUG(DOT1S_DEBUG_STATE_MACHINE,DOT1S_CIST_INDEX))
          {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port(%d) instIndex(%d) proposed(%d) agree(%d) synced(%d) reroot(%d)  \n",
                          __FUNCTION__,
                          p->portNum, instIndex, p->portInstInfo[instIndex].proposed,
                          p->portInstInfo[instIndex].agree,
                          p->portInstInfo[instIndex].synced,p->portInstInfo[instIndex].reRoot );
          }

          if (p->portInstInfo[instIndex].proposed == L7_TRUE &&
              p->portInstInfo[instIndex].agree == L7_FALSE)
          {
            rc = dot1sStateMachineClassifier(prtMasterProposedNotAgree,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].proposed == L7_TRUE &&
              p->portInstInfo[instIndex].agree == L7_TRUE )
          {
            rc = dot1sStateMachineClassifier(prtMasterProposedAgree,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if ( p->portInstInfo[instIndex].agree == L7_FALSE)
          {
            allSynced = L7_TRUE;
            allSynced = isAllSynced(p->portNum,instIndex,L7_FALSE);
            if (allSynced == L7_TRUE)
            {
              rc = dot1sStateMachineClassifier(prtMasterAllSyncedNotAgree,p,instIndex,
                                               L7_NULL, L7_NULLPTR);
            }
          }

          if (p->portInstInfo[instIndex].learning == L7_FALSE &&
              p->portInstInfo[instIndex].forwarding == L7_FALSE &&
              p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            rc = dot1sStateMachineClassifier(prtMasterNotLrnNotFwdNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].agreed == L7_TRUE &&
              p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            rc = dot1sStateMachineClassifier(prtMasterAgreedNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->operEdge == L7_TRUE &&
              p->portInstInfo[instIndex].synced == L7_FALSE)
          {
            rc = dot1sStateMachineClassifier(prtMasterEdgeNotSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].sync == L7_TRUE &&
              p->portInstInfo[instIndex].synced == L7_TRUE)
          {
            rc = dot1sStateMachineClassifier(prtMasterSyncSynced,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }
          if (p->portInstInfo[instIndex].rrWhile == 0 &&
              p->portInstInfo[instIndex].reRoot == L7_TRUE)
          {
            rc = dot1sStateMachineClassifier(prtMasterReRootRrWhileZero,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].synced ==L7_TRUE &&
              p->portNum == (*syncedPending))
          {
            generateRequestedPortEvents(instIndex);

          }


          if ((p->portInstInfo[instIndex].learn ==L7_TRUE ||
               p->portInstInfo[instIndex].forward == L7_TRUE) &&
              p->operEdge == L7_FALSE &&
              ((p->portInstInfo[instIndex].sync == L7_TRUE &&
                p->portInstInfo[instIndex].synced == L7_FALSE) ||
               ((p->portInstInfo[instIndex].rrWhile != 0) &&
                p->portInstInfo[instIndex].reRoot == L7_TRUE) ||
               p->portInstInfo[instIndex].disputed == L7_TRUE
              )
             )
          {

            if (DOT1S_DEBUG(DOT1S_DEBUG_STATE_MACHINE,DOT1S_CIST_INDEX))
            {
              SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s: port(%d) instIndex(%d)  rrwhile (%d) reRoot(%d) disputed(%d) sync(%d)   \n"
                            ,__FUNCTION__, p->portNum, instIndex,
                            p->portInstInfo[instIndex].rrWhile,
                            p->portInstInfo[instIndex].reRoot,
                            p->portInstInfo[instIndex].disputed,
                            p->portInstInfo[instIndex].sync );
            }



            rc = dot1sStateMachineClassifier(prtMasterListen,p,instIndex,
                                             L7_NULL, L7_NULLPTR);
          }

          allSynced = L7_TRUE;
          allSynced = isAllSynced(p->portNum,instIndex,L7_FALSE);

          if (p->portInstInfo[instIndex].learn == L7_FALSE &&
              ((p->portInstInfo[instIndex].fdWhile == 0) ||
               (allSynced == L7_TRUE)) &&
              ((p->portInstInfo[instIndex].rrWhile == 0) ||
                p->portInstInfo[instIndex].reRoot == L7_FALSE) &&
              (p->portInstInfo[instIndex].sync == L7_FALSE)
             )
          {
            /*send event E39*/
            /*rc = dot1sIssueCmd(prtLearn, p->portNum, id, L7_NULLPTR);*/
            /*if (DOT1S_PROTO_DEBUG() == L7_TRUE)
            {
               SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Setting prtLearn Port(%d) Inst(%d) \n",
                                 __FUNCTION__, p->portNum, instIndex);
               SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"role (%d) Sync (%d) AllSynced (%d) agreed (%d)\n",
                      p->portInstInfo[instIndex].role,p->portInstInfo[instIndex].sync,allSynced,
                      p->portInstInfo[instIndex].agreed);
            }*/
            rc = dot1sStateMachineClassifier(prtMasterLearn, p, instIndex, L7_NULL, L7_NULLPTR);
          }

          if (p->portInstInfo[instIndex].learn == L7_TRUE &&
              p->portInstInfo[instIndex].forward == L7_FALSE &&
              ((p->portInstInfo[instIndex].fdWhile == 0) ||
               (allSynced == L7_TRUE)) &&
              ((p->portInstInfo[instIndex].rrWhile == 0) ||
                p->portInstInfo[instIndex].reRoot == L7_FALSE) &&
              (p->portInstInfo[instIndex].sync == L7_FALSE)
             )

          {
            /*send event E39*/
            /*rc = dot1sIssueCmd(prtLearn, p->portNum, id, L7_NULLPTR);*/
            /*if (DOT1S_PROTO_DEBUG() == L7_TRUE)
            {
               SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"%s : Setting prtLearn Port(%d) Inst(%d) \n",
         __FUNCTION__, p->portNum, instIndex);
               SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"role (%d) Sync (%d) AllSynced (%d) agreed (%d)\n",
                      p->portInstInfo[instIndex].role,p->portInstInfo[instIndex].sync,allSynced,
                      p->portInstInfo[instIndex].agreed);

            }*/
            rc = dot1sStateMachineClassifier(prtMasterForward, p, instIndex, L7_NULL, L7_NULLPTR);
          }

          if ((p->portInstInfo[instIndex].rrWhile == 0) &&
              p->portNum == (*reRootedPending))
          {
            generateRequestedReRootedPortEvents(instIndex);
          }
        }
      }
      break;
    default:
      break;


  }

  if (p->pduTxStopCheck == L7_TRUE)
  {
    dot1sNeedForBpduTxStopCheck(p);
  }

  return rc;


}
L7_RC_t dot1sGenerateEventsForTcm(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (p->portInstInfo[instIndex].tcmState)
  {
    case TCM_INACTIVE:
      /*  Add for fdbFlush
      if ((p->portInstInfo[instIndex].learn == L7_TRUE) &&
          p->portInstInfo[instIndex].fdbFlush == L7_FALSE)
      {
          rc = dot1sStateMachineClassifier(tcmRcvdTcn, p, DOT1S_CIST_INDEX, L7_NULL, L7_NULLPTR);
      }
      */
      break;
    case TCM_LEARNING:
      if (p->portInstInfo[instIndex].rcvdTc)
      {
        rc = dot1sStateMachineClassifier(tcmRcvdTc, p, instIndex, L7_NULL, L7_NULLPTR);
      }

      if (p->rcvdTcn == L7_TRUE &&
          instIndex == DOT1S_CIST_INDEX)
      {
        rc = dot1sStateMachineClassifier(tcmRcvdTcn, p, instIndex, L7_NULL, L7_NULLPTR);
      }

      if (p->rcvdTcAck == L7_TRUE &&
          instIndex == DOT1S_CIST_INDEX)
      {
        rc = dot1sStateMachineClassifier(tcmRcvdTcAck, p, instIndex, L7_NULL, L7_NULLPTR);
      }

      if (p->portInstInfo[instIndex].tcProp == L7_TRUE)
      {
        rc = dot1sStateMachineClassifier(tcmTcProp, p, instIndex, L7_NULL, L7_NULLPTR);
      }

      if (p->portInstInfo[instIndex].forwarding == L7_TRUE &&
          p->operEdge == L7_FALSE)
      {
        if (p->portInstInfo[instIndex].role == ROLE_ROOT )
        {
          /*Send event E51*/
          /*rc = dot1sIssueCmd(tcmRoleRootFwdNotEdge, p->portNum, id, L7_NULLPTR);*/
          rc = dot1sStateMachineClassifier(tcmRoleRootFwdNotEdge, p, instIndex, L7_NULL, L7_NULLPTR);
        }
        if (p->portInstInfo[instIndex].role == ROLE_DESIGNATED )
        {
          /*Send event E52*/
          /*rc = dot1sIssueCmd(tcmRoleDesigFwdNotEdge, p->portNum, id, L7_NULLPTR);*/
          rc = dot1sStateMachineClassifier(tcmRoleDesigFwdNotEdge, p, instIndex, L7_NULL, L7_NULLPTR);
        }
        if (p->portInstInfo[instIndex].role == ROLE_MASTER )
        {
          /*Send event E53*/
          /*rc = dot1sIssueCmd(tcmRoleMasterFwdNotEdge, p->portNum, id, L7_NULLPTR);*/
          rc = dot1sStateMachineClassifier(tcmRoleMasterFwdNotEdge, p, instIndex, L7_NULL, L7_NULLPTR);
        }
      }
      break;
    case TCM_ACTIVE:
      if ((p->portInstInfo[instIndex].role != ROLE_ROOT &&
           p->portInstInfo[instIndex].role != ROLE_DESIGNATED &&
           p->portInstInfo[instIndex].role != ROLE_MASTER) ||
          (p->operEdge == L7_TRUE))
      {
        /*Send event E55*/
        /*rc = dot1sIssueCmd(tcmRoleNotRootNotDesigNotMaster, p->portNum, id, L7_NULLPTR);*/
        rc = dot1sStateMachineClassifier(tcmRoleNotRootNotDesigNotMaster, p, instIndex, L7_NULL, L7_NULLPTR);
      }

      if (p->rcvdTcn == L7_TRUE &&
          instIndex == DOT1S_CIST_INDEX)
      {
        rc = dot1sStateMachineClassifier(tcmRcvdTcn, p, instIndex, L7_NULL, L7_NULLPTR);
      }

      if (p->portInstInfo[instIndex].rcvdTc)
      {
        rc = dot1sStateMachineClassifier(tcmRcvdTc, p, instIndex, L7_NULL, L7_NULLPTR);
      }

      if (p->rcvdTcAck == L7_TRUE &&
          instIndex == DOT1S_CIST_INDEX)
      {
        rc = dot1sStateMachineClassifier(tcmRcvdTcAck, p, instIndex, L7_NULL, L7_NULLPTR);
      }

      if (p->portInstInfo[instIndex].tcProp == L7_TRUE &&
          p->operEdge == L7_FALSE)
      {
        rc = dot1sStateMachineClassifier(tcmTcProp, p, instIndex, L7_NULL, L7_NULLPTR);
      }
      break;
    default:

      break;
  }


  return rc;

}

/* AllsyncedFlag is true now and some port had requestd allSync notification.
   Generate events for that port and clear all sync requested
*/
static void generateRequestedPortEvents( L7_uint32 instIndex )
{
  L7_uint32 *allSyncedRequested = L7_NULLPTR;
  DOT1S_PORT_COMMON_t *syncPort;

  if (instIndex == DOT1S_CIST_INDEX)
  {
    allSyncedRequested = &dot1sInstance->cist.allSyncedRequested;
  }
  else
  {
    allSyncedRequested = &dot1sInstance->msti[instIndex].allSyncedRequested;
  }

  syncPort = dot1sIntfFind(*allSyncedRequested);

  if (syncPort != L7_NULLPTR && syncPort->portNum != 0)
  {
    if (instIndex == DOT1S_CIST_INDEX)
    {
      dot1sInstance->cist.syncedPending = 0;
    }
    else
    {
      dot1sInstance->msti[instIndex].syncedPending = 0;
    }


    if (dot1s_enable_debug_detail)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"All Synced True :generating events for port %d inst %d\n", *allSyncedRequested, instIndex);

    }
    *allSyncedRequested = 0;
    dot1sPrtGenerateEvents(syncPort,instIndex);

  }
}
/* AllsyncedFlag is true now and some port had requestd allSync notification.
   Generate events for that port and clear all sync requested
*/
static void generateRequestedReRootedPortEvents( L7_uint32 instIndex )
{
  L7_uint32 *reRootedRequested = L7_NULLPTR;
  DOT1S_PORT_COMMON_t *reRootedPort;

  if (instIndex == DOT1S_CIST_INDEX)
  {
    reRootedRequested = &dot1sInstance->cist.reRootedRequested;
  }
  else
  {
    reRootedRequested = &dot1sInstance->msti[instIndex].reRootedRequested;
  }

  reRootedPort = dot1sIntfFind(*reRootedRequested);

  if (reRootedPort != L7_NULLPTR && reRootedPort->portNum != 0)
  {
    if (instIndex == DOT1S_CIST_INDEX)
    {
      dot1sInstance->cist.reRootedPending = 0;
    }
    else
    {
      dot1sInstance->msti[instIndex].reRootedPending = 0;
    }

    *reRootedRequested = 0;

    if (dot1s_enable_debug_detail)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,"reRooted True :generating events for port %d inst %d\n", *reRootedRequested, instIndex);
    }
    dot1sPrtGenerateEvents(reRootedPort,instIndex);
  }
}

void dot1sLoopInconsistentSet(DOT1S_PORT_COMMON_t *p, L7_uint32 instIndex, L7_BOOL mode)
{

  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(p->portNum);
  if (p->loopInconsistent == mode)
  {
    return;
  }

  p->loopInconsistent = mode;
  L7_INTF_SETMASKBIT(portCommonChangeMask, p->portNum);

  if (mode == L7_FALSE)
  {
    dot1sPortStats[portIndex].transitionsOutOfLoopInconsistentState++;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "SpanningTree-LoopGuard: LoopGuard Disabled: unblocking port %d on MST instance %d\n", p->portNum, instIndex);
    trapMgrStpInstanceLoopInconsistentEndTrap(instIndex, p->portNum);

  }
  else
  {
    dot1sPortStats[portIndex].transitionsIntoLoopInconsistentState++;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "SpanningTree-LoopGuard: LoopGuard blocking port: %d on MST instance: %d\n",
            p->portNum, instIndex);
    trapMgrStpInstanceLoopInconsistentStartTrap(instIndex, p->portNum);

  }
}

/*********************************************************************
* @purpose  returns True if any of the vlans associated with this instance
*           belong to the port.
*
* @param    intIfNum        @b{(input)} port Number of the requesting port
* @param    instIndex       @b{(input)} instIndex
*
* @comments
*
* @end
*********************************************************************/
static L7_BOOL dot1sIsPortActiveForInstance(L7_uint32 instIndex,
                                            L7_uint32 intIfNum)
{
    L7_uint32 numVlan = 0, i, mstID;
    L7_VLAN_MASK_t vidMask;
    L7_RC_t rc = L7_FAILURE;
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    if (dot1sInstNumFind(instIndex, &mstID) != L7_SUCCESS)
      return L7_TRUE;

    bzero((char *)&VIDList, (L7_int32)sizeof(VIDList));
    rc = dot1sMstiVIDList(mstID, VIDList, &numVlan);

    if (dot1qVIDListMaskGet(intIfNum, &vidMask) != L7_SUCCESS)
    {
       /* When in doubt: flush the port */
       return L7_TRUE;
    }

    for (i = 0; i < numVlan; i++)
    {
      if (VIDList[i] == 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1S_COMPONENT_ID,
          "%s :Port %s inst %d VIDList has element 0 numVlans %d \n",
               __FUNCTION__, ifName, instIndex, numVlan);
        return L7_TRUE;
      }

      if (L7_VLAN_ISMASKBITSET(vidMask, VIDList[i]))
        return L7_TRUE;
    }
    return L7_FALSE;
}


L7_RC_t dot1sPortStateMachineInit(DOT1S_PORT_COMMON_t *p, L7_BOOL portEnabled)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 instIndex;
  dot1sEvents_t bdmBeginEvent;

  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* generate begin events for all instances and cist that is in use
    - pim, prt, pst, tcm
   */
  rc = dot1sStateMachineClassifier(prxBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);
  rc = dot1sStateMachineClassifier(ppmBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);
  rc = dot1sStateMachineClassifier(ptxBegin, p, L7_NULL, L7_NULL, L7_NULLPTR);

  bdmBeginEvent = (p->adminEdge == L7_TRUE) ?
                   bdmBeginAdminEdge : bdmBeginNotAdminEdge;

  rc = dot1sStateMachineClassifier(bdmBeginEvent, p, L7_NULL, L7_NULL, L7_NULLPTR);

  for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    if (dot1sInstanceMap[instIndex].inUse == L7_TRUE)
    {
      rc = dot1sStateMachineClassifier(pstBegin, p, instIndex, L7_NULL, L7_NULLPTR);
      rc = dot1sStateMachineClassifier(pimBegin, p, instIndex, L7_NULL, L7_NULLPTR);
      rc = dot1sStateMachineClassifier(prtBegin, p, instIndex, L7_NULL, L7_NULLPTR);
      rc = dot1sStateMachineClassifier(tcmBegin, p, instIndex, L7_NULL, L7_NULLPTR);
      if (portEnabled == L7_TRUE)
      {
        rc = dot1sStateMachineClassifier(pimPortEnabled, p, instIndex, L7_NULL, L7_NULLPTR);
      }
      rc = dot1sPortStateMachineUpTimeReset(p);
    }
  }

  return rc;
}

/**************************************************************************
* @purpose   Resets guard values/frees timers
*
*
* @param    intIfNum - Interface number
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
*************************************************************************/
L7_RC_t dot1sResetBpduGuardEffect(DOT1S_PORT_COMMON_t *p)
{
  p->bpduGuardEffect = L7_FALSE;
  L7_INTF_SETMASKBIT(portCommonChangeMask, p->portNum);
  dot1sPortStateMachineInit(p, L7_TRUE);
  /*if (dot1sProcessGuardLinkUpChange(intIfNum) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
           "Failed to set BPDU Guard value FLASE for %d\n",intIfNum );
    return L7_FAILURE;
  }*/
  return L7_SUCCESS;
}

void dot1sCallCkPtServiceIfNeeded(L7_uint32 event)
{
  L7_uint32 i;
  L7_BOOL nonZero;

  L7_INTF_NONZEROMASK(portCommonChangeMask, nonZero);
  if (nonZero == L7_TRUE)
  {
    if (dot1sNsfFuncTable.dot1sCallCheckpointService)
    {
      DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT_DETAIL,
                      "%s: Calling ckpt for event %d ", __FUNCTION__, event);
      dot1sNsfFuncTable.dot1sCallCheckpointService(&portCommonChangeMask,
                                                   L7_DOT1S_MSTID_ALL);
      memset(&portCommonChangeMask, 0, sizeof(L7_INTF_MASK_t));
    }
  }

  for (i = 0; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    L7_INTF_NONZEROMASK(portInstInfoChangeMask[i], nonZero);
    if (nonZero == L7_TRUE)
    {
      if (dot1sNsfFuncTable.dot1sCallCheckpointService)
      {
        DOT1S_DEBUG_NSF(DOT1S_DEBUG_NSF_CKPT_DETAIL,
                        "%s: Calling instance ckpt for event %d ",
                         __FUNCTION__, event);
        dot1sNsfFuncTable.dot1sCallCheckpointService(&portInstInfoChangeMask[i],i);
        memset(&portInstInfoChangeMask[i], 0, sizeof(L7_INTF_MASK_t));
      }
    }/* non zero is true */
  } /* for instances */
}
