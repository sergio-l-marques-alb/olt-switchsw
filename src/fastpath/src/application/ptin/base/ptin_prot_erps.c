/**
 * ptin_prot_erps.c 
 *  
 * Implements ITU-T G.8032/Y.1344 Ethernet Ring Protection 
 * Switching (ERPS)
 *
 * @author joaom (6/3/2013)
 * (joao-v-mateiro@ptinovacao.pt) 
 *  
 */


#include "ptin_globaldefs.h"
#ifdef PTIN_ENABLE_ERPS

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/sched.h>

#include "ptin_prot_erps.h"
#include "ptin_hal_erps.h"
#include "logger.h"
#include "ptin_cnfgr.h"
#include "fdb_api.h"
#include "ptin_intf.h"

#include "ptin_debug.h"

#define SM_MODIFICATIONS  // State machine modification according to Table VIII.1

#define SM_PTIN_MODS      // State machine modification due to observed incoherent situations

/* *******************************************************************************/
/*                                  GLOBAL VARS                                  */
/* *******************************************************************************/

tblErps_t               tbl_erps[MAX_PROT_PROT_ERPS];
erpsVlanInclusionList_t tbl_erps_vlanList[MAX_PROT_PROT_ERPS];

static const char *stateToString[]  = {"FRZ", "Z_Init", "A_Idle", "B_Protection", "C_ManualSwitch", "D_ForcedSwitch", "E_Pending"};
static const char *locReqToString[] = {"0", "WTBRun", "WTBExp", "WTRRun", "WTRExp", "MS", "SFc", "SF", "FS", "CLEAR", "NONE", "11", "12"};
static const char *remReqToString[] = {"NR", "1", "2", "3", "4", "5", "6", "MS", "8", "9", "10", "SF", "12", "FS", "EVENT", "NONE"};
static const char *strPortState[]   = {"FLUSHING", "BLOCKING"};
static const char *strTimerCmd[]    = { "STOP", "START" };
static const char *strTimer[]       = { "WTR Timer", "WTB Timer", "GUARD Timer" };

static L7_uint8 force_erps_SF[MAX_PROT_PROT_ERPS][2];

/* Semaphore to control concurrent accesses */
void *ptin_prot_erps_sem = L7_NULLPTR;

/* *******************************************************************************/
/*                                   FUNCTIONS                                   */
/* *******************************************************************************/

/* dummy */
int rd_alarms_dummy(L7_uint8 slot, L7_uint32 index)
{
  return(0);
}

L7_RC_t aps_rxdummy(L7_uint8 erps_idx, L7_uint8 *req, L7_uint8 *status, L7_uint8 *nodeid, L7_uint32 *rxport)
{
  return(0);
}

L7_RC_t aps_txdummy(L7_uint8 erps_idx, L7_uint8 req, L7_uint8 status)
{
  return(0);
}

int prot_proc_dummy(L7_uint8 prot_id)
{
  return(0);
}


int ptin_prot_erps_instance_proc(L7_uint8 erps_idx);

int ptin_erps_aps_tx(L7_uint8 erps_idx, L7_uint8 req, L7_uint8 status, int line_callback);

int ptin_erps_blockOrUnblockPort(L7_uint8 erps_idx, L7_uint8 port, L7_uint8 portState, int line_callback);

int ptin_erps_FlushFDB(L7_uint8 erps_idx, int line_callback);

/**
 * ERPS Instace# initialization
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx
 * 
 * @return int
 */
int ptin_erps_init_entry(L7_uint8 erps_idx)
{
  int ret = erps_idx;

  PT_LOG_DEBUG(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if ((erps_idx>=MAX_PROT_PROT_ERPS)) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].admin                            = PROT_ERPS_ENTRY_FREE;

  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]       = PROT_ERPS_SF_CLEAR;
  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]       = PROT_ERPS_SF_CLEAR;

  tbl_erps[erps_idx].wtr_timer                        = 0;
  tbl_erps[erps_idx].wtr_CMD                          = TIMER_CMD_STOP;
  tbl_erps[erps_idx].wtb_timer                        = 0;
  tbl_erps[erps_idx].wtb_CMD                          = TIMER_CMD_STOP;
  tbl_erps[erps_idx].holdoff_timer                    = 0;
  tbl_erps[erps_idx].holdoff_timer_previous           = 0;

  tbl_erps[erps_idx].operator_cmd                     = PROT_ERPS_OPCMD_NR;
  tbl_erps[erps_idx].operator_cmd_port                = PROT_ERPS_PORT0;

  tbl_erps[erps_idx].localRequest                     = LReq_NONE;
  tbl_erps[erps_idx].localReqPort                     = 0;
  tbl_erps[erps_idx].remoteRequest                    = RReq_NONE;

  tbl_erps[erps_idx].apsReqStatusTx                   = 0;
  tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT0]  = 0;
  tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT1]  = 0;  

  memset(tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0], 0, PROT_ERPS_MAC_SIZE);
  memset(tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1], 0, PROT_ERPS_MAC_SIZE);

  tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT0]        = 0;
  tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT1]        = 0;

  tbl_erps[erps_idx].portState[PROT_ERPS_PORT0]       = ERPS_PORT_FLUSHING;
  tbl_erps[erps_idx].portState[PROT_ERPS_PORT1]       = ERPS_PORT_FLUSHING;
  tbl_erps[erps_idx].dnfStatus                        = 0;

  tbl_erps[erps_idx].state_machine                    = ERPS_STATE_SetLocal(ERPS_STATE_Z_Init);

  tbl_erps[erps_idx].hal.rd_alarms                    = rd_alarms_dummy;
  tbl_erps[erps_idx].hal.aps_rxfields                 = aps_rxdummy;
  tbl_erps[erps_idx].hal.aps_txfields                 = aps_txdummy;

  tbl_erps[erps_idx].hal.prot_proc                    = prot_proc_dummy;

  tbl_erps[erps_idx].waitingcicles                    = 0;

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * VLAN List# initialization
 * 
 * @author joaom (6/11/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_vlanList_init_entry(L7_uint8 erps_idx)
{
  int       ret = PROT_ERPS_EXIT_OK;

  PT_LOG_DEBUG(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if ((erps_idx>=MAX_PROT_PROT_ERPS)) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  memset(tbl_erps_vlanList[erps_idx].vid_bmp, 0, (1<<12)/(sizeof(L7_uint8)*8));

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * ERPS Instaces initialization
 * 
 * @author joaom (6/5/2013)
 * 
 * @return int
 */
int ptin_erps_init(void)
{
  int       ret = PROT_ERPS_EXIT_OK;
  L7_uint8  erps_idx;

  PT_LOG_INFO(LOG_CTX_ERPS, "Initializing ERPS Module");

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    ptin_erps_init_entry(erps_idx);
    ptin_erps_vlanList_init_entry(erps_idx);
  }
  
  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Get the first free entry
 * 
 * @author joaom (6/5/2013)
 * 
 * @return int 
 */
int ptin_erps_get_free_entry(void)
{
  int       ret = PROT_ERPS_UNAVAILABLE;
  L7_uint8  erps_idx;

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
      ret = erps_idx;
      PT_LOG_DEBUG(LOG_CTX_ERPS, "ret:%d, done.", ret);
      return(ret);
    }
  }
  
  PT_LOG_WARN(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Allocate an ERPS instance
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param new_group 
 * 
 * @return int 
 */
int ptin_erps_add_entry( L7_uint8 erps_idx, erpsProtParam_t *new_group)
{
  int ret = erps_idx;

  PT_LOG_DEBUG(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if ( (erps_idx == PROT_ERPS_UNUSEDIDX) || (erps_idx >= MAX_PROT_PROT_ERPS) ) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_BUSY) {
    //ret = PROT_ERPS_INDEX_IN_USE;
    osapiSemaGive(ptin_prot_erps_sem);
    PT_LOG_WARN(LOG_CTX_ERPS, "ret:%d, ENTRY_BUSY done.", ret);
    return(ret);
  }

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  memcpy( &tbl_erps[erps_idx].protParam, new_group, sizeof(erpsProtParam_t) );

  #if 0
  {
    int byte, bit, vid;
    for (byte=0; byte<(sizeof(tbl_erps[erps_idx].protParam.vid_bmp)); byte++) {
      for (bit=0; bit<8; bit++) {
        if ((tbl_erps[erps_idx].protParam.vid_bmp[byte] >> bit) & 1) {
          vid = (byte*8)+bit;
          PT_LOG_DEBUG(LOG_CTX_ERPS, "ERPS#%d VLAN %d", erps_idx, vid);
        }
      }
    }
  }
  #endif


  // *100ms; [0, 10] seconds
  if (tbl_erps[erps_idx].protParam.holdoffTimer > 100) {
    tbl_erps[erps_idx].protParam.holdoffTimer = 100;
  }

  // *10ms; [10, 2000] miliseconds
  if (tbl_erps[erps_idx].protParam.guardTimer > 200) {
    tbl_erps[erps_idx].protParam.guardTimer = 200;
  }
  else if (tbl_erps[erps_idx].protParam.guardTimer < 1) {
    tbl_erps[erps_idx].protParam.guardTimer = 1;
  }
  
    
  if (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_NON_REVERTIVE_OPERATION) {
    tbl_erps[erps_idx].protParam.waitToRestoreTimer = 0;
  }
  else {

    // *1m; [1, 12] minutes
    if (tbl_erps[erps_idx].protParam.waitToRestoreTimer > 12) {
      tbl_erps[erps_idx].protParam.waitToRestoreTimer = 12;
    }
    else if (tbl_erps[erps_idx].protParam.waitToRestoreTimer < 1) {
      tbl_erps[erps_idx].protParam.waitToRestoreTimer = 1;
    }

  }

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0] = PROT_ERPS_SF_CLEAR;
  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1] = PROT_ERPS_SF_CLEAR;

  tbl_erps[erps_idx].wtr_timer                  = 0;
  tbl_erps[erps_idx].wtr_CMD                    = TIMER_CMD_STOP;

  tbl_erps[erps_idx].wtb_timer                  = 0;
  tbl_erps[erps_idx].wtb_CMD                    = TIMER_CMD_STOP;

  tbl_erps[erps_idx].guard_timer                = 0;
  tbl_erps[erps_idx].guard_timer_previous       = 0;  
  tbl_erps[erps_idx].guard_CMD                  = TIMER_CMD_STOP;

  tbl_erps[erps_idx].holdoff_timer              = 0;
  tbl_erps[erps_idx].holdoff_timer_previous     = 0;

  tbl_erps[erps_idx].operator_cmd               = PROT_ERPS_OPCMD_NR;
  tbl_erps[erps_idx].operator_cmd_port          = PROT_ERPS_PORT0;

  tbl_erps[erps_idx].localRequest               = LReq_NONE;
  tbl_erps[erps_idx].localReqPort               = 0;
  tbl_erps[erps_idx].remoteRequest              = RReq_NONE;

  tbl_erps[erps_idx].apsReqStatusTx             = 0;

  tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT0]  = 0;
  tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT1]  = 0;  

  memset(tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0], 0, PROT_ERPS_MAC_SIZE);
  memset(tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1], 0, PROT_ERPS_MAC_SIZE);

  tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT0]  = 0;
  tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT1]  = 0;


  tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] = ERPS_PORT_FLUSHING;
  tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] = ERPS_PORT_FLUSHING;
  tbl_erps[erps_idx].dnfStatus                  = 0;

  tbl_erps[erps_idx].state_machine              = ERPS_STATE_SetLocal(ERPS_STATE_Z_Init);

  tbl_erps[erps_idx].waitingcicles              = PROT_ERPS_WAITING_CICLES_INIT;

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  tbl_erps[erps_idx].hal.rd_alarms              = ptin_hal_erps_rd_alarms;

  tbl_erps[erps_idx].hal.aps_txfields           = ptin_hal_erps_sendaps;

  tbl_erps[erps_idx].hal.aps_rxfields           = ptin_hal_erps_rcvaps;

  tbl_erps[erps_idx].hal.prot_proc              = ptin_prot_erps_instance_proc;

  tbl_erps[erps_idx].admin                      = PROT_ERPS_ENTRY_BUSY;

  ptin_hal_erps_entry_init(erps_idx);

  ptin_hal_erps_internal_vlans_used_sync(erps_idx);

  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Used to reconfigure an existing entry
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param mask 
 * @param conf 
 * 
 * @return int 
 */
int ptin_erps_conf_entry(L7_uint8 erps_idx, L7_uint32 mask, erpsProtParam_t *conf)
{
  int ret = erps_idx;
  //int byte, bit, vid;

  PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin==PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_UNAVAILABLE;
    PT_LOG_WARN(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (mask & ERPS_CONF_MASK_BIT_ISOPENRING)     tbl_erps[erps_idx].protParam.isOpenRing = conf->isOpenRing;

  if (mask & ERPS_CONF_MASK_BIT_PORT0CFMIDX)    tbl_erps[erps_idx].protParam.port0CfmIdx = conf->port0CfmIdx;

  if (mask & ERPS_CONF_MASK_BIT_PORT1CFMIDX)    tbl_erps[erps_idx].protParam.port1CfmIdx = conf->port1CfmIdx;

  if (mask & ERPS_CONF_MASK_BIT_REVERTIVE)      tbl_erps[erps_idx].protParam.revertive = conf->revertive;

  if (mask & ERPS_CONF_MASK_BIT_GUARDTIMER)     tbl_erps[erps_idx].protParam.guardTimer = conf->guardTimer;

  if (mask & ERPS_CONF_MASK_BIT_HOLDOFFTIMER)   tbl_erps[erps_idx].protParam.holdoffTimer = conf->holdoffTimer;

  if (mask & ERPS_CONF_MASK_BIT_WAITTORESTORE)  tbl_erps[erps_idx].protParam.waitToRestoreTimer = conf->waitToRestoreTimer;

  //tbl_erps[erps_idx].protParam.continualTxInterval    = conf->continualTxInterval;
  //tbl_erps[erps_idx].protParam.rapidTxInterval        = conf->rapidTxInterval;

  if (mask & ERPS_CONF_MASK_BIT_VIDBMP)
  {
    memcpy( tbl_erps[erps_idx].protParam.vid_bmp, conf->vid_bmp, sizeof(conf->vid_bmp) );

    ptin_hal_erps_convert_vid_init(erps_idx);

    ptin_hal_erps_queue_vlans_used_clear(erps_idx);
    ptin_hal_erps_internal_vlans_used_sync(erps_idx);

    #if 0
    for (byte=0; byte<(sizeof(tbl_erps[erps_idx].protParam.vid_bmp)); byte++) {
      for (bit=0; bit<8; bit++) {
        if ((tbl_erps[erps_idx].protParam.vid_bmp[byte] >> bit) & 1) {
          vid = (byte*8)+bit;
          PT_LOG_DEBUG(LOG_CTX_ERPS, "ERPS#%d VLAN %d", erps_idx, vid);
        }
      }
    }
    #endif
  }


  // Values Validation

  if (tbl_erps[erps_idx].protParam.holdoffTimer > 100) {   // [0, 10] seconds
    tbl_erps[erps_idx].protParam.holdoffTimer = 100;
  }


  if (tbl_erps[erps_idx].protParam.guardTimer > 2000) {    // [2, 2000] miliseconds
    tbl_erps[erps_idx].protParam.guardTimer = 2000;
  }
  else if (tbl_erps[erps_idx].protParam.guardTimer < 2) {  // [2, 2000] miliseconds
    tbl_erps[erps_idx].protParam.guardTimer = 2;
  }
  
    
  if (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_NON_REVERTIVE_OPERATION) {
    tbl_erps[erps_idx].protParam.waitToRestoreTimer = 0;
  }
  else {
    if (tbl_erps[erps_idx].protParam.waitToRestoreTimer > 12) {      // [5, 12] minutes
      tbl_erps[erps_idx].protParam.waitToRestoreTimer = 12;
    }
    else if (tbl_erps[erps_idx].protParam.waitToRestoreTimer < 1) {  // [5, 12] minutes
      tbl_erps[erps_idx].protParam.waitToRestoreTimer = 1;
    }
  }
  
  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}



/**
 * Reinit an ERPS instance
 * 
 * @author joaom (7/17/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_reinit_entry( L7_uint8 erps_idx)
{
  int ret = erps_idx;

  PT_LOG_DEBUG(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if ( (erps_idx == PROT_ERPS_UNUSEDIDX) || (erps_idx >= MAX_PROT_PROT_ERPS) ) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin != PROT_ERPS_ENTRY_BUSY) {
    osapiSemaGive(ptin_prot_erps_sem);
    //ret = PROT_ERPS_INDEX_IN_USE;
    PT_LOG_WARN(LOG_CTX_ERPS, "ret:%d, ENTRY_FREE done.", ret);
    return(ret);
  }

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0] = PROT_ERPS_SF_CLEAR;
  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1] = PROT_ERPS_SF_CLEAR;

  tbl_erps[erps_idx].wtr_timer                  = 0;
  tbl_erps[erps_idx].wtr_CMD                    = TIMER_CMD_STOP;

  tbl_erps[erps_idx].wtb_timer                  = 0;
  tbl_erps[erps_idx].wtb_CMD                    = TIMER_CMD_STOP;

  tbl_erps[erps_idx].guard_timer                = 0;
  tbl_erps[erps_idx].guard_timer_previous       = 0;  
  tbl_erps[erps_idx].guard_CMD                  = TIMER_CMD_STOP;

  tbl_erps[erps_idx].holdoff_timer              = 0;
  tbl_erps[erps_idx].holdoff_timer_previous     = 0;

  tbl_erps[erps_idx].operator_cmd               = PROT_ERPS_OPCMD_NR;
  tbl_erps[erps_idx].operator_cmd_port          = PROT_ERPS_PORT0;

  tbl_erps[erps_idx].localRequest               = LReq_NONE;
  tbl_erps[erps_idx].localReqPort               = 0;
  tbl_erps[erps_idx].remoteRequest              = RReq_NONE;

  tbl_erps[erps_idx].apsReqStatusTx             = 0;

  tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT0]  = 0;
  tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT1]  = 0;  

  memset(tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0], 0, PROT_ERPS_MAC_SIZE);
  memset(tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1], 0, PROT_ERPS_MAC_SIZE);

  tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT0]  = 0;
  tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT1]  = 0;


  tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] = ERPS_PORT_FLUSHING;
  tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] = ERPS_PORT_FLUSHING;
  tbl_erps[erps_idx].dnfStatus                  = 0;

  tbl_erps[erps_idx].state_machine              = ERPS_STATE_SetLocal(ERPS_STATE_Z_Init);

  tbl_erps[erps_idx].waitingcicles              = PROT_ERPS_WAITING_CICLES_INIT;

  ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Delete ERPS# instance
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_remove_entry(L7_uint8 erps_idx)
{
  int ret = erps_idx;

  PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    PT_LOG_NOTICE(LOG_CTX_ERPS, "Entry free.", ret);
    return(ret);
  }

  ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
  ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
  ptin_erps_FlushFDB(erps_idx, __LINE__);

  ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);  

  tbl_erps[erps_idx].admin = PROT_ERPS_ENTRY_REMOVE_PENDING;

  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Delete all ERPS
 * 
 * @author joaom (6/5/2013)
 * 
 * @return int 
 */
int ptin_erps_clear(void)
{
  int erps_idx, ret=PROT_ERPS_EXIT_OK;

  PT_LOG_TRACE(LOG_CTX_ERPS, "");

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    if ((ret=ptin_erps_remove_entry(erps_idx))!=erps_idx) {
      PT_LOG_ERR(LOG_CTX_ERPS, "ERROR: (%d) while removing ERPS#%d\n\r", ret, erps_idx);
      ret = PROT_ERPS_EXIT_NOK1;
    }
  }

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Get ERPS# Protection Parameters configuration
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param group 
 * 
 * @return int 
 */
int ptin_erps_get_entry(L7_uint8 erps_idx, erpsProtParam_t *group)
{
  int ret = erps_idx;

  PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin==PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_UNAVAILABLE;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  memcpy(group, &tbl_erps[erps_idx].protParam, sizeof(erpsProtParam_t));
  
  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Operator Command
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param cmd_port 
 * 
 * @return int 
 */
int ptin_erps_cmd_force(L7_uint8 erps_idx, L7_uint8 cmd_port)
{
  int ret = erps_idx;

  PT_LOG_INFO(LOG_CTX_ERPS, "ERPS#%d: %d", erps_idx, cmd_port);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_UNAVAILABLE;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].operator_cmd       = PROT_ERPS_OPCMD_FS;
  tbl_erps[erps_idx].operator_cmd_port  = cmd_port==1? PROT_ERPS_PORT1:PROT_ERPS_PORT0;
  
  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Operator Command
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param cmd_port 
 * 
 * @return int 
 */
int ptin_erps_cmd_manual(L7_uint8 erps_idx, L7_uint8 cmd_port)
{
  int ret = erps_idx;

  PT_LOG_INFO(LOG_CTX_ERPS, "ERPS#%d: %d", erps_idx, cmd_port);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_UNAVAILABLE;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].operator_cmd == PROT_ERPS_OPCMD_FS) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_EXIT_NOK1;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].operator_cmd       = PROT_ERPS_OPCMD_MS;
  tbl_erps[erps_idx].operator_cmd_port  = cmd_port==1? PROT_ERPS_PORT1:PROT_ERPS_PORT0;

  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Operator Command
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_cmd_clear(L7_uint8 erps_idx)
{
  int ret = erps_idx;

  PT_LOG_INFO(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_UNAVAILABLE;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  //10.1.9. Local Priority Logic
  //The Clear command is only valid if:
  //a) a local Forced Switch or Manual Switch command is in effect (Clear operation a) described in clause 8)

  #if 1
  if ( (tbl_erps[erps_idx].operator_cmd != PROT_ERPS_OPCMD_NR) && (tbl_erps[erps_idx].operator_cmd != PROT_ERPS_OPCMD_OC) && 
       (tbl_erps[erps_idx].operator_cmd != PROT_ERPS_OPCMD_FS) && (tbl_erps[erps_idx].operator_cmd != PROT_ERPS_OPCMD_MS)     ){
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_EXIT_NOK1;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }
  #endif

  tbl_erps[erps_idx].operator_cmd       = PROT_ERPS_OPCMD_OC;
  tbl_erps[erps_idx].operator_cmd_port  = PROT_ERPS_PORT0;
  
  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Operator Command
 * 
 * @author joaom (7/1/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_cmd_lockout(L7_uint8 erps_idx)
{
  int ret = erps_idx;

  PT_LOG_INFO(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_UNAVAILABLE;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].operator_cmd       = PROT_ERPS_OPCMD_LO;
  tbl_erps[erps_idx].operator_cmd_port  = PROT_ERPS_PORT0;
  
  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Operator Command
 * 
 * @author joaom (7/1/2013)
 * 
 * @param erps_idx 
 * @param cmd_port 
 * 
 * @return int 
 */
int ptin_erps_cmd_replaceRpl(L7_uint8 erps_idx, L7_uint8 cmd_port)
{
  int ret = erps_idx;

  PT_LOG_INFO(LOG_CTX_ERPS, "ERPS#%d: %d", erps_idx, cmd_port);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_UNAVAILABLE;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].operator_cmd       = PROT_ERPS_OPCMD_ReplaceRPL;
  tbl_erps[erps_idx].operator_cmd_port  = cmd_port==1? PROT_ERPS_PORT1:PROT_ERPS_PORT0;
  
  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Operator Command
 * 
 * @author joaom (7/1/2013)
 * 
 * @param erps_idx 
 * @param cmd_port 
 * 
 * @return int 
 */
int ptin_erps_cmd_exercise(L7_uint8 erps_idx, L7_uint8 cmd_port)
{
  int ret = erps_idx;

  PT_LOG_INFO(LOG_CTX_ERPS, "ERPS#%d: %d", erps_idx, cmd_port);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_UNAVAILABLE;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  /*** TO BE DONE ***/
  tbl_erps[erps_idx].operator_cmd       = PROT_ERPS_OPCMD_ExeSignal;
  tbl_erps[erps_idx].operator_cmd_port  = cmd_port==1? PROT_ERPS_PORT1:PROT_ERPS_PORT0;
  
  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Get ERPS Status
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param status 
 * 
 * @return int 
 */
int ptin_erps_get_status(L7_uint8 erps_idx, erpsStatus_t *status)
{
  int ret = erps_idx;

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  if (tbl_erps[erps_idx].admin==PROT_ERPS_ENTRY_FREE) {
    osapiSemaGive(ptin_prot_erps_sem);
    ret = PROT_ERPS_UNAVAILABLE;
    return(ret);
  }

  status->port0_SF           = tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0];
  status->port1_SF           = tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1];
  status->port0State         = tbl_erps[erps_idx].portState[PROT_ERPS_PORT0];
  status->port1State         = tbl_erps[erps_idx].portState[PROT_ERPS_PORT1];

  status->apsReqStatusTx     = tbl_erps[erps_idx].apsReqStatusTx;
  status->apsReqStatusRxP0   = tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT0];
  status->apsReqStatusRxP1   = tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT1];

  memcpy(status->apsNodeIdRxP0, tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0], PROT_ERPS_MAC_SIZE);
  memcpy(status->apsNodeIdRxP1, tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1], PROT_ERPS_MAC_SIZE);

  status->state_machine      = ERPS_STATE_GetState(tbl_erps[erps_idx].state_machine);
  status->dnfStatus          = tbl_erps[erps_idx].dnfStatus;

  status->guard_timer        = tbl_erps[erps_idx].guard_timer;
  status->wtr_timer          = tbl_erps[erps_idx].wtr_timer;
  status->wtb_timer          = tbl_erps[erps_idx].wtb_timer;
  status->holdoff_timer      = tbl_erps[erps_idx].holdoff_timer;

  osapiSemaGive(ptin_prot_erps_sem);

  return(ret);
}


/**
 * ERPS Debug: Print configurations and Status information
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_rd_entry(L7_uint8 erps_idx)
{
  int ret = erps_idx;

  char *strBool[]       = { "FALSE", "TRUE", "FUZZYDO" };
  char *strPortRole[]   = { "NON RPL", "RPL Owner", "RPL Neighbour" };
  char *strRevertive[]  = { "Non-Revertive", "Revertive" };
  char *strCmd[]        = { "NR", "OC", "LO", "FS", "MS", "ReplaceRPL", "ExeSignal" };
  char *strCmdPath[]    = { "Switch to PORT0", "Switch to PORT1", "NONE" };

  PT_LOG_INFO(LOG_CTX_ERPS, "ERPS#%d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    PT_LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

  printf("\n-----------------------------------------");
  printf("\n  admin               %d",                      tbl_erps[erps_idx].admin);
  printf("\n-----------------------------------------");
  printf("\n\nERPS#%d Protection Parameters:\n",            erps_idx);
  printf("\n  ringId              %d",                      tbl_erps[erps_idx].protParam.ringId);
  printf("\n  isOpenRing          %s",                      strBool[tbl_erps[erps_idx].protParam.isOpenRing]);
  printf("\n  controlVid          %d",                      tbl_erps[erps_idx].protParam.controlVid);
  printf("\n  megLevel            %d",                      tbl_erps[erps_idx].protParam.megLevel);
  printf("\n");
  printf("\n  port0.slot          %d",                      tbl_erps[erps_idx].protParam.port0.slot);
  printf("\n  port0.type          %d",                      tbl_erps[erps_idx].protParam.port0.type);
  printf("\n  port0.idx           %d",                      tbl_erps[erps_idx].protParam.port0.idx);
  printf("\n  port0Role           %s",                      strPortRole[tbl_erps[erps_idx].protParam.port0Role]);
  printf("\n  port0CfmIdx         %d",                      tbl_erps[erps_idx].protParam.port0CfmIdx);
  printf("\n");
  printf("\n  port1.slot          %d",                      tbl_erps[erps_idx].protParam.port1.slot);
  printf("\n  port1.type          %d",                      tbl_erps[erps_idx].protParam.port1.type);
  printf("\n  port1.idx           %d",                      tbl_erps[erps_idx].protParam.port1.idx);
  printf("\n  port1Role           %s",                      strPortRole[tbl_erps[erps_idx].protParam.port1Role]);
  printf("\n  port1CfmIdx         %d",                      tbl_erps[erps_idx].protParam.port1CfmIdx);
  printf("\n");
  printf("\n  revertive           %s",                      strRevertive[tbl_erps[erps_idx].protParam.revertive]);
  printf("\n  guardTimer          %d (x10 ms)",             tbl_erps[erps_idx].protParam.guardTimer);
  printf("\n  holdoffTimer        %d (x100 ms)",            tbl_erps[erps_idx].protParam.holdoffTimer);
  printf("\n  waitToRestoreTimer  %d (x1 min)",             tbl_erps[erps_idx].protParam.waitToRestoreTimer);
  printf("\n");
  printf("\n  continualTxInterval %d",                      tbl_erps[erps_idx].protParam.continualTxInterval);
  printf("\n  rapidTxInterval     %d",                      tbl_erps[erps_idx].protParam.rapidTxInterval);
  printf("\n-----------------------------------------");
  printf("\n\nERPS#%d Status:\n",                           erps_idx);
  printf("\n  status_SF[P0]       %d",                      tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]);
  printf("\n  status_SF[P1]       %d",                      tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]);
  printf("\n");
  printf("\n  wtr_CMD             %s",                      strTimerCmd[tbl_erps[erps_idx].wtr_CMD]);
  printf("\n  wtb_CMD             %s",                      strTimerCmd[tbl_erps[erps_idx].wtb_CMD]);
  printf("\n  guard_CMD           %s",                      strTimerCmd[tbl_erps[erps_idx].guard_CMD]);
  printf("\n");
  printf("\n  wtr_timer           %d",                      tbl_erps[erps_idx].wtr_timer);
  printf("\n  wtb_timer           %d",                      tbl_erps[erps_idx].wtb_timer);
  printf("\n  guard_timer         %d",                      tbl_erps[erps_idx].guard_timer);
  printf("\n  guard_timer_p       %d",                      tbl_erps[erps_idx].guard_timer_previous);
  printf("\n  holdoff_timer       %d",                      tbl_erps[erps_idx].holdoff_timer);
  printf("\n  holdoff_timer_p     %d",                      tbl_erps[erps_idx].holdoff_timer_previous);
  printf("\n"); 
  printf("\n  operator_cmd        (0x%x) %s",               tbl_erps[erps_idx].operator_cmd, strCmd[tbl_erps[erps_idx].operator_cmd]);
  printf("\n  operator_cmd_port   (%d)   %s",               tbl_erps[erps_idx].operator_cmd_port, strCmdPath[tbl_erps[erps_idx].operator_cmd_port]);
  printf("\n-----------------------------------------");
  printf("\nAPS State Machine:\n");
  printf("\n  localRequest        %s",                      locReqToString[tbl_erps[erps_idx].localRequest - 100]);
  printf("\n  localReqPort        %d",                      tbl_erps[erps_idx].localReqPort);
  printf("\n  remoteRequest       %s",                      remReqToString[tbl_erps[erps_idx].remoteRequest]);
  printf("\n  apsReqStatusTx      (0x%x) %s",               tbl_erps[erps_idx].apsReqStatusTx, remReqToString[((tbl_erps[erps_idx].apsReqStatusTx >> 12) & 0xF)]);
  printf("\n  apsReqStatusRx[P0]  (0x%x) %s",               tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT0], remReqToString[((tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT0] >> 12) & 0xF)]);
  printf("\n  apsReqStatusRx[P1]  (0x%x) %s",               tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT1], remReqToString[((tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT1] >> 12) & 0xF)]);
  printf("\n  apsNodeIdRx[P0]     %.2x:%.2x:%.2x:%.2x:%.2x:%.2x", tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0][0], tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0][1], tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0][2], 
                                                                  tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0][3], tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0][4], tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0][5]);
  printf("\n  apsNodeIdRx[P1]     %.2x:%.2x:%.2x:%.2x:%.2x:%.2x", tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1][0], tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1][1], tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1][2], 
                                                                  tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1][3], tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1][4], tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1][5]);  
  printf("\n  apsBprRx[P0]        %d",                      tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT0]);
  printf("\n  apsBprRx[P1]        %d",                      tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT1]);
  printf("\n  portState[P0]       (0x%x) %s",               tbl_erps[erps_idx].portState[PROT_ERPS_PORT0], strPortState[tbl_erps[erps_idx].portState[PROT_ERPS_PORT0]]);
  printf("\n  portState[P1]       (0x%x) %s",               tbl_erps[erps_idx].portState[PROT_ERPS_PORT1], strPortState[tbl_erps[erps_idx].portState[PROT_ERPS_PORT1]]);
  printf("\n  dnfStatus           %d",                      tbl_erps[erps_idx].dnfStatus);
  printf("\n  state_machine       (0x%x) %s:%s",            tbl_erps[erps_idx].state_machine,
                                                            stateToString[ERPS_STATE_GetState(tbl_erps[erps_idx].state_machine)],
                                                            ERPS_STATE_IsLocal(tbl_erps[erps_idx].state_machine)? "L":"R");
  printf("\n-----------------------------------------\n\r");

  osapiSemaGive(ptin_prot_erps_sem);

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * ERPS Debug: Print configurations and Status information
 * 
 * @author joaom (6/11/2013)
 * 
 * @return int 
 */
int ptin_erps_rd_allentry(void)
{
  int ret = PROT_ERPS_EXIT_OK;
  int byte, bit, vid;

  L7_uint8 erps_idx;

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {

    osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

    if ( tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE)
    {
      osapiSemaGive(ptin_prot_erps_sem);
      continue;
    }

    printf("\n-----------------------------------------");
    printf("\n ERPS#%d Protection Parameters:",              erps_idx);
    printf("\n ringId              %d",                      tbl_erps[erps_idx].protParam.ringId);
    printf("\n controlVid          %d",                      tbl_erps[erps_idx].protParam.controlVid);
    printf("\n VLAN ID List       ");
    for (byte=0; byte<(sizeof(tbl_erps[erps_idx].protParam.vid_bmp)); byte++) {
      for (bit=0; bit<8; bit++) {
        if ((tbl_erps[erps_idx].protParam.vid_bmp[byte] >> bit) & 1) {
          vid = (byte*8)+bit;
          printf(" %d;", vid);
        }
      }
    }

    osapiSemaGive(ptin_prot_erps_sem);

    printf("\n");
  }

  return(ret);
}


/**
 * ERPS Debug: Print configurations and Status information
 * 
 * @author joaom (6/11/2013)
 * 
 * @return int 
 */
int ptin_erps_dump(L7_uint8 erps_idx)
{
  int ret = PROT_ERPS_EXIT_OK;

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ptin_erps_rd_allentry();
  } else {
    ptin_erps_rd_entry(erps_idx);
  }

  fflush(stdout);

  return(ret);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/
//                                                                               /
//      E T H E R N E T   R I N G   P R O T E C T I O N   S W I T C H I N G      /
//                                                                               /
//                         S T A T E   M A C H I N E                             /
//                                                                               /
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/


/**
 * Block or unblock ERP Port
 * 
 * @author joaom (6/7/2013)
 * 
 * @param erps_idx 
 * @param port 
 * @param portState 
 * 
 * @return int 
 */
int ptin_erps_blockOrUnblockPort(L7_uint8 erps_idx, L7_uint8 port, L7_uint8 portState, int line_callback)
{
  int ret = PROT_ERPS_EXIT_OK;

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }
  if (port > PROT_ERPS_PORT1)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d: Invalid port id %u", port);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: port %d, portState %s (line_callback %d)", erps_idx, port, strPortState[portState], line_callback);

  #ifdef SM_PTIN_MODS
  // If both ports were in Flushing State and now changing to Blocking, force a Flush FDB
  if ( (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_FLUSHING) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_FLUSHING) && (portState == ERPS_PORT_BLOCKING) ) {
    ptin_erps_FlushFDB(erps_idx, __LINE__);
  }
  #endif

  tbl_erps[erps_idx].portState[port] = portState;

  tbl_halErps[erps_idx].hwSync = 1;

  // When the ring port
  // is changed to be blocked - as indicated by the block/unblock ring ports signal - the flush logic
  // deletes the current (node ID, BPR) pair on both ring ports.
  if (portState == ERPS_PORT_BLOCKING) {
    memset(tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0], 0, PROT_ERPS_MAC_SIZE);
    memset(tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1], 0, PROT_ERPS_MAC_SIZE);

    tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT0] = 0;
    tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT1] = 0;
  }
  
  //PT_LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Set condition of either failed (i.e., signal fail (SF)) or 
 * non-failed (OK) 
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param path 
 * 
 * @return int 
 */
int ptin_erps_force_alarms(L7_uint8 erps_idx, L7_uint8 port, L7_uint8 sf)
{
  int ret = PROT_ERPS_EXIT_OK;  

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }
  if (port > PROT_ERPS_PORT1)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d: Invalid port id %u", port);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  PT_LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d: port %d, SF %d", erps_idx, port, sf&1);

  force_erps_SF[erps_idx][port] = sf;

  return(ret);
}


/**
 * Get condition of either failed (i.e., signal fail (SF)) or 
 * non-failed (OK) 
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param port 
 * 
 * @return int 
 */
int ptin_erps_rd_alarms(L7_uint8 erps_idx, L7_uint8 port)
{
  int ret[2] = {PROT_ERPS_EXIT_OK,PROT_ERPS_EXIT_OK};
  static int ret_h[2] = {PROT_ERPS_EXIT_OK,PROT_ERPS_EXIT_OK};

  //PT_LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d: port %d", erps_idx, port);

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }
  if (port > PROT_ERPS_PORT1)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d: Invalid port id %u", port);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  if (force_erps_SF[erps_idx][port]) {
    ret[port] = (force_erps_SF[erps_idx][port] & 1);

    if (ret_h[port] != ret[port]) {
      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: ret[%d] %d", erps_idx, port, ret[port]);
      ret_h[port] = ret[port];
    }
    return ret[port];
  }

  if ( (port == PROT_ERPS_PORT0) && (tbl_erps[erps_idx].protParam.port0CfmIdx != 255) )
  {
    ret[port] = ( tbl_erps[erps_idx].hal.rd_alarms(0, tbl_erps[erps_idx].protParam.port0CfmIdx) | (ptin_intf_los_get(tbl_erps[erps_idx].protParam.port0.idx)) );
    if (ret_h[port] != ret[port]) {
      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: ret[%d] %d", erps_idx, port, ret[port]);
      ret_h[port] = ret[port];
    }
    return ret[port];
  }

  else if ( (port == PROT_ERPS_PORT1) && (tbl_erps[erps_idx].protParam.port1CfmIdx != 255) )
  {
    ret[port] = ( tbl_erps[erps_idx].hal.rd_alarms(0, tbl_erps[erps_idx].protParam.port1CfmIdx) | (ptin_intf_los_get(tbl_erps[erps_idx].protParam.port1.idx)) );
    if (ret_h[port] != ret[port]) {
      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: ret[%d] %d", erps_idx, port, ret[port]);
      ret_h[port] = ret[port];
    }
    return ret[port];
  }

  // CFM Not defined; use Port link Down
  else if (port == PROT_ERPS_PORT0) {
    ret[port] = ptin_intf_los_get(tbl_erps[erps_idx].protParam.port0.idx);
    if (ret_h[port] != ret[port]) {
      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: ret[%d] %d", erps_idx, port, ret[port]);
      ret_h[port] = ret[port];
    }
    return ret[port];
  }
  else if (port == PROT_ERPS_PORT1) {
    ret[port] = ptin_intf_los_get(tbl_erps[erps_idx].protParam.port1.idx);
    if (ret_h[port] != ret[port]) {
      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: ret[%d] %d", erps_idx, port, ret[port]);
      ret_h[port] = ret[port];
    }
    return ret[port];
  }

  
  PT_LOG_ERR(LOG_CTX_ERPS, "ERPS#%d: Upss...", erps_idx);
  return(PROT_ERPS_EXIT_OK);
}

int ptin_erps_rd_alarms_test(L7_uint8 ptin_port)
{
  L7_uint8 alarm;

  alarm = ptin_intf_los_get(ptin_port);

  PT_LOG_NOTICE(LOG_CTX_ERPS,"\n\nERPS: port %d, alarm %d\n\n", ptin_port, alarm);
  printf("\n\nERPS: port %d, alarm %d\n\n", ptin_port, alarm); fflush(stdout);

  return(0);
}


/**
 * APS messages transmission
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param req 
 * @param fPath 
 * @param path 
 * 
 * @return int 
 */
int ptin_erps_aps_tx(L7_uint8 erps_idx, L7_uint8 req, L7_uint8 status, int line_callback)
{
  int ret = PROT_ERPS_EXIT_OK;
  L7_uint16 apsTx;

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  if (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) {
    status |= RReq_STAT_BPR_SET(PROT_ERPS_PORT0);
  } else if (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) {
    status |= RReq_STAT_BPR_SET(PROT_ERPS_PORT1);
  }

  apsTx = ((req << 12) & 0xF000) | (status & 0x00FF);

  if (tbl_erps[erps_idx].apsReqStatusTx != apsTx) {
    PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Tx R-APS Request 0x%x = %s(0x%x) (line_callback %d)",  erps_idx, req, remReqToString[req], status, line_callback);
  }

  tbl_erps[erps_idx].hal.aps_txfields(erps_idx, req, status);

  tbl_erps[erps_idx].apsReqStatusTx = apsTx;

  return(ret);
}


/**
 * APS messages reception
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx      [in]
 * @param req_status    [out]
 * @param nodeid        [out]
 * @param rxport        [out]
 * @param line_callback [in]
 * 
 * @return int 
 */
int ptin_erps_aps_rx(L7_uint8 erps_idx, L7_uint8 *req, L7_uint8 *status, L7_uint8 *nodeid, L7_uint32 *rxport, int line_callback)
{
  int ret;

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  //PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d (line_callback %d)", erps_idx, line_callback);

  ret = tbl_erps[erps_idx].hal.aps_rxfields(erps_idx, req, status, nodeid, rxport);

  return(ret);
}


/**
 * Force ERPS FSM new state
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param state 
 * 
 * @return int 
 */
int ptin_erps_FSM_remote_transition(L7_uint8 erps_idx, L7_uint8 state_machine)
{
  int ret = PROT_ERPS_EXIT_OK;

  return(ret);
}


/**
 * Force ERPS FSM new state
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param state 
 * 
 * @return int 
 */
int ptin_erps_FSM_transition(L7_uint8 erps_idx, L7_uint8 state_machine, int line_callback)
{
  int ret = PROT_ERPS_EXIT_OK;

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  if (ERPS_STATE_IgnoreLocal(tbl_erps[erps_idx].state_machine) != ERPS_STATE_IgnoreLocal(state_machine))
  {
    PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Changing state from (0x%x) %s:%s to (0x%x) %s:%s (line_callback %d)", 
              erps_idx, 
              tbl_erps[erps_idx].state_machine, stateToString[ERPS_STATE_GetState(tbl_erps[erps_idx].state_machine)], ERPS_STATE_IsLocal(tbl_erps[erps_idx].state_machine)? "L":"R", 
              state_machine, stateToString[ERPS_STATE_GetState(state_machine)], ERPS_STATE_IsLocal(state_machine)? "L":"R", 
              line_callback);

    tbl_erps[erps_idx].state_machine_h  = tbl_erps[erps_idx].state_machine;
    tbl_erps[erps_idx].state_machine    = state_machine;

    if (ERPS_STATE_IsLocal(state_machine))
    {
      tbl_erps[erps_idx].remoteRequest = RReq_NONE;
      tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT0]  = 0;
      tbl_erps[erps_idx].apsReqStatusRx[PROT_ERPS_PORT1]  = 0;
    }
    else
    {
      tbl_erps[erps_idx].localRequest = LReq_NONE;
      tbl_erps[erps_idx].localReqPort = PROT_ERPS_PORT0;
    }

    ptin_erps_FSM_remote_transition(erps_idx, state_machine);
  }

  return(ret);
}


/**
 * Flush FDB
 * 
 * @author joaom (6/7/2013)
 * 
 * @param erps_idx 
 * @param line_callback 
 * 
 * @return int 
 */
int ptin_erps_FlushFDB(L7_uint8 erps_idx, int line_callback)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Flushing FDB (line_callback %d)", erps_idx, line_callback);

  tbl_halErps[erps_idx].hwFdbFlush = 1;

  return(PROT_ERPS_EXIT_OK);
}


/**
 * Start Timer
 * 
 * @author joaom (6/26/2013)
 * 
 * @param erps_idx 
 * @param line_callback 
 * 
 * @return int 
 */
int ptin_erps_startTimer(L7_uint8 erps_idx, L7_uint8 timer, L7_uint8 timerCmd, int line_callback)
{
  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  switch (timer) {
  case WTR_TIMER_CMD:
    tbl_erps[erps_idx].wtr_CMD = timerCmd;
    if (timerCmd == TIMER_CMD_STOP) tbl_erps[erps_idx].wtr_timer = 0;
    break;
  case WTB_TIMER_CMD:
    tbl_erps[erps_idx].wtb_CMD = timerCmd;
    if (timerCmd == TIMER_CMD_STOP) tbl_erps[erps_idx].wtb_timer = 0;
    break;
  case GUARD_TIMER_CMD:
    tbl_erps[erps_idx].guard_CMD = timerCmd;
    if (timerCmd == TIMER_CMD_STOP) tbl_erps[erps_idx].guard_timer = 0;
    break;
  }

  PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: %s %s (line_callback %d)", erps_idx, strTimer[timer], strTimerCmd[timerCmd], line_callback);

  return(PROT_ERPS_EXIT_OK);
}


/**
 * ERPS instance State machine
 * 
 * @author joaom (6/5/2013)
 * 
 * @param i 
 * 
 * @return int 
 */
int ptin_prot_erps_instance_proc(L7_uint8 erps_idx)
{
  L7_uint8  SF[2];

  L7_uint8  localRequest                      = LReq_NONE;
  L7_uint8  remoteRequest                     = RReq_NONE;
  L7_uint16 apsReqStatusRx                    = 0;
  L7_uint8  apsReqRx                          = 0;
  L7_uint8  apsStatusRx                       = 0;
  L7_uint8  apsNodeIdRx[PROT_ERPS_MAC_SIZE]   = {0};
  L7_uint32 apsRxPort                         = 0;
  L7_uint8  topPriorityRequest                = LReq_NONE;  // The current top priority request as defined in sub-clause 10.1.1.
  L7_BOOL   haveChanges                       = L7_FALSE;
  L7_uint8  reqPort                           = 0;          // Request or Failed Port

  /* Validate arguments */
  if (erps_idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not valid", erps_idx);
    return PROT_ERPS_INDEX_VIOLATION;
  }

  //PT_LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d: admin %d", erps_idx, tbl_erps[erps_idx].admin);

  switch (ERPS_STATE_GetState(tbl_erps[erps_idx].state_machine)) {
  
  case ERPS_STATE_Z_Init:

    //Stop guard timer
    tbl_erps[erps_idx].guard_timer                      = 0;
    tbl_erps[erps_idx].guard_timer_previous             = 0;
    tbl_erps[erps_idx].guard_CMD                        = TIMER_CMD_STOP;

    //Stop WTR timer
    tbl_erps[erps_idx].wtr_timer                        = 0;
    tbl_erps[erps_idx].wtr_CMD                          = TIMER_CMD_STOP;

    //Stop WTB timer
    tbl_erps[erps_idx].wtb_timer                        = 0;
    tbl_erps[erps_idx].wtb_CMD                          = TIMER_CMD_STOP;

    #ifdef SM_MODIFICATIONS
    //Clear DNF
    tbl_erps[erps_idx].dnfStatus                        = 0;
    #endif

    //If RPL Owner Node:
    if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {

      //Block RPL port
      //Unblock non-RPL port
      if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL ) {                      // Port0 is RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
      } else {                                                                                  // Port1 is RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      }

      //Tx R-APS (NR)
      ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);

      //If revertive:
      if ( (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION) ) {
        #ifdef SM_MODIFICATIONS
        // Start WTB timer
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_START, __LINE__);
        #else
        // Start WTR timer
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_START, __LINE__);
        #endif
      }
    }
    // Else if RPL Neighbour Node:
    else if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
      // Block RPL Port
      // Unblock non-RPL port
      if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR ) {             // Port0 is RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
      } else {                                                                                  // Port1 is RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      }

      //Tx R-APS (NR)
      ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);
    }
    //Else:
    else {
      // Block one ring port   
      // Unblock other ring port      
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);

      // Tx R-APS (NR)
      ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);
    }

    // Next node state: E
    ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_E_Pending), __LINE__);
    break;

  }//switch


  if (tbl_erps[erps_idx].waitingcicles) {
    tbl_erps[erps_idx].waitingcicles--;
    return(PROT_ERPS_EXIT_OK);
  }


  //--------------------------------------------------------------------------------------------
  // Alarm Indication (SF)
  SF[PROT_ERPS_PORT0] = ptin_erps_rd_alarms(erps_idx, PROT_ERPS_PORT0);
  SF[PROT_ERPS_PORT1] = ptin_erps_rd_alarms(erps_idx, PROT_ERPS_PORT1);


  //--------------------------------------------------------------------------------------------
  // R-APS request (Rx check)
  if ( L7_SUCCESS == ptin_erps_aps_rx(erps_idx, &apsReqRx, &apsStatusRx, apsNodeIdRx, &apsRxPort, __LINE__) ) {

    L7_uint8 apsReqRxOtherPort;
    L7_uint8 apsStatusRxOtherPort;

    if (apsRxPort > PROT_ERPS_PORT1)
    {
      PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d not: apsRxPort %d not valid", erps_idx, apsRxPort);
      return PROT_ERPS_INDEX_VIOLATION;
    }
#if 0
    remoteRequest = apsReqRx;

    apsReqStatusRx = tbl_erps[erps_idx].apsReqStatusRx[apsRxPort];

    tbl_erps[erps_idx].apsReqStatusRx[apsRxPort] = ((apsReqRx << 12) & 0xF000) | (apsStatusRx & 0x00FF);
#else
    apsReqRxOtherPort = (tbl_erps[erps_idx].apsReqStatusRx[!apsRxPort] >> 12) & 0xF;
    apsStatusRxOtherPort = (tbl_erps[erps_idx].apsReqStatusRx[!apsRxPort] & 0x00FF);

    apsReqStatusRx = tbl_erps[erps_idx].apsReqStatusRx[apsRxPort];
    tbl_erps[erps_idx].apsReqStatusRx[apsRxPort] = ((apsReqRx << 12) & 0xF000) | (apsStatusRx & 0x00FF);

    if (apsReqStatusRx != tbl_erps[erps_idx].apsReqStatusRx[apsRxPort]) {

      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Received R-APS Request(0x%x) = %s(0x%x), apsRxPort %d, Node Id %.2x%.2x%.2x%.2x%.2x%.2x", erps_idx, apsReqRx,
              remReqToString[apsReqRx], APS_GET_STATUS(apsStatusRx), apsRxPort,
              apsNodeIdRx[0],
              apsNodeIdRx[1],
              apsNodeIdRx[2],
              apsNodeIdRx[3],
              apsNodeIdRx[4],
              apsNodeIdRx[5]);

      if (apsReqRx != apsReqRxOtherPort) {
        remoteRequest = apsReqRx;
      }
      else if ((apsReqRx == apsReqRxOtherPort) && (apsStatusRx != apsStatusRxOtherPort)) {
        remoteRequest = apsReqRx;

        /* Clear on the other Port */
        tbl_erps[erps_idx].apsReqStatusRx[!apsRxPort] = 0;
      }
      else {
        PT_LOG_NOTICE(LOG_CTX_ERPS,"ERPS#%d: Ignoring Received R-APS. Other Port Request(0x%x) = %s(0x%x)", erps_idx, apsReqRxOtherPort,
              remReqToString[apsReqRxOtherPort], APS_GET_STATUS(apsStatusRxOtherPort));
      }
    }
#endif

  }
    

  //--------------------------------------------------------------------------------------------
  // 10.1.9.  Local Priority Logic
  //--------------------------------------------------------------------------------------------
  //Local priority logic evaluates the local operator commands (in ETH_C_MI_RAPS_ExtCMD)
  //according to the current top priority request. The commands Clear, Manual Switch and Forced
  //Switch from the operator, are forwarded to the Priority Logic.

  //--------------------------------------------------------------------------------------------
  // Operator Command
  if ( tbl_erps[erps_idx].operator_cmd ) {

    if ( (tbl_erps[erps_idx].operator_cmd == PROT_ERPS_OPCMD_OC) ) {
      //10.1.9. Local Priority Logic
      //The Clear command is only valid if:
      //a) a local Forced Switch or Manual Switch command is in effect (Clear operation a) described in clause 8), or
      //b) a local Ethernet Ring Node is an RPL Owner Node and top priority request is neither R-APS (FS) nor R-APS (MS) (Clear operation  b) or  c) described in clause 8).

      // *** Verification of a) is done when command is received ***
      localRequest = LReq_CLEAR;
      reqPort = tbl_erps[erps_idx].operator_cmd_port;

      // *** Verification of b) ***
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        if ( (remoteRequest != RReq_FS) && (remoteRequest != RReq_MS) ) {
          localRequest = LReq_CLEAR;
          reqPort = tbl_erps[erps_idx].operator_cmd_port;
        }
      }
    }

    else if ( (tbl_erps[erps_idx].operator_cmd == PROT_ERPS_OPCMD_FS) ) {
      localRequest = LReq_FS;
      reqPort = tbl_erps[erps_idx].operator_cmd_port;
    }

    else if ( (tbl_erps[erps_idx].operator_cmd == PROT_ERPS_OPCMD_MS) ) {
      localRequest = LReq_MS;
      reqPort = tbl_erps[erps_idx].operator_cmd_port;
    }

    else if ( (tbl_erps[erps_idx].operator_cmd == PROT_ERPS_OPCMD_LO) ) {
      //Lockout of protection - This command disables the protection group.
      // Next node state: Freeze
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_Freeze), __LINE__);
    }

    else if ( (tbl_erps[erps_idx].operator_cmd == PROT_ERPS_OPCMD_ReplaceRPL) ) {
      //Replace the RPL - This command moves the RPL by blocking a different ring link and unblocking the RPL permanently
    }    
  }


  //----------------------------------------------------------------------------------------------------
  // 10.1.1.  Priority logic
  //----------------------------------------------------------------------------------------------------
  //This process receives requests from multiple sources. The request with the highest priority in Table
  //10-1, is declared as the top priority request.
  //
  // 1.   Clear (operator command)
  // 2.   Forced Switch (operator command)
  // 3.   R-APS Forced Switch (operator command)
  // 4.   Local Signal Fail (OAM / control-plane / server indication)
  // 5.   Local Clear Signal Fail (OAM / control-plane / server indication)
  // 6.   R-APS Signal Fail (OAM / control-plane / server indication)
  // 7.   R-APS Manual Switch (operator command)
  // 8.   Manual Switch (operator command)
  // 9.   WTR Expires (WTR timer)
  // 10.  WTR Running (WTR timer)
  // 11.  WTB Expires (WTB timer)
  // 12.  WTB Running (WTB timer)
  // 13.  R-APS (NR,RB)
  // 14.  R-APS (NR)


  // 1.   Clear (operator command)
  if (localRequest == LReq_CLEAR) {
    topPriorityRequest = LReq_CLEAR;
  }

  // 2.   Forced Switch (operator command)
  else if (localRequest == LReq_FS) {
    topPriorityRequest = LReq_FS;
  }

  // 3.   R-APS Forced Switch (operator command)
  else if (remoteRequest == RReq_FS) {
    topPriorityRequest = RReq_FS;
  }

  else {

    if ( SF[PROT_ERPS_PORT0] ) {
        localRequest = LReq_SF;
        reqPort = PROT_ERPS_PORT0;
    }
    if ( (SF[PROT_ERPS_PORT0] != tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) ) {
      if ( (SF[PROT_ERPS_PORT0]) ) {
        localRequest = LReq_SF;
        reqPort = PROT_ERPS_PORT0;
        PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: SF[PROT_ERPS_PORT0]", erps_idx);
      } else /*if ( !(SF[PROT_ERPS_PORT0]) )*/ {
        localRequest = LReq_SFc;
        reqPort = PROT_ERPS_PORT0;
        PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: SF[PROT_ERPS_PORT0] Clear", erps_idx);
      }
    }

    if ( (SF[PROT_ERPS_PORT1]) && (localRequest == LReq_NONE) ) {
        localRequest = LReq_SF;
        reqPort = PROT_ERPS_PORT1;
    }
    if ( (SF[PROT_ERPS_PORT1] != tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) ) {
      if ( (SF[PROT_ERPS_PORT1]) && (localRequest != LReq_SF) && (reqPort != PROT_ERPS_PORT1) ) {
        localRequest = LReq_SF;
        reqPort = PROT_ERPS_PORT1;
        PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: SF[PROT_ERPS_PORT1]", erps_idx);
      } else if ( !(SF[PROT_ERPS_PORT1]) && (localRequest != LReq_SFc) ) {
        localRequest = LReq_SFc;
        reqPort = PROT_ERPS_PORT1;
        PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: SF[PROT_ERPS_PORT1] Clear", erps_idx);
      }
    }

    /* Validate */
    if (reqPort > PROT_ERPS_PORT1)
    {
      PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d: reqPort %u not valid", erps_idx, reqPort);
      return PROT_ERPS_INDEX_VIOLATION;
    }

    // 4.   Local Signal Fail (OAM / control-plane / server indication)
    if (localRequest == LReq_SF) {
      topPriorityRequest = LReq_SF;
    }

    // 5.   Local Clear Signal Fail (OAM / control-plane / server indication)
    else if (localRequest == LReq_SFc) {
      topPriorityRequest = LReq_SFc;
    }  

    // 6.   R-APS Signal Fail (OAM / control-plane / server indication)
    else if (remoteRequest == RReq_SF) {
      topPriorityRequest = RReq_SF;
    }

    // 7.   R-APS Manual Switch (operator command)
    else if (remoteRequest == RReq_MS) {
      topPriorityRequest = RReq_MS;
    }
     
    // 8.   Manual Switch (operator command)
    else if (localRequest == LReq_MS) {
      topPriorityRequest = LReq_MS;
    }

    // 9.   WTR Expires (WTR timer)
    // 10.  WTR Running (WTR timer)
    else if ( (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION) &&
              (tbl_erps[erps_idx].wtr_CMD == TIMER_CMD_START)                                ) {

      // WaitToRestore is in steps of 1min
      // wait_restore_timer units is ms. Proc is called every PROT_ERPS_CALL_PROC_MS
      // Wait_restore_timer/60000 converts to ms

      tbl_erps[erps_idx].wtr_timer+=PROT_ERPS_CALL_PROC_MS;
      if ( (tbl_erps[erps_idx].wtr_timer/60000) >= tbl_erps[erps_idx].protParam.waitToRestoreTimer ) {

        localRequest = LReq_WTRExp;
        PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: WTRExp", erps_idx);

        tbl_erps[erps_idx].wtr_timer = 0;             // Reset timer value
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;  // Stop timer

      }

      // 9.
      if (localRequest == LReq_WTRExp) {
        topPriorityRequest = LReq_WTRExp;
      }

      // 10.
      else {
        topPriorityRequest = LReq_WTRRun;
      }

    }

    // 11.  WTB Expires (WTB timer)
    // 12.  WTB Running (WTB timer)
    else if ( tbl_erps[erps_idx].wtb_CMD == TIMER_CMD_START ) {

      // wtb_timer units is ms. Proc is called every PROT_ERPS_CALL_PROC_MS
      // WTB timer is defined to be 5 seconds longer than the guardtimer

      tbl_erps[erps_idx].wtb_timer+=PROT_ERPS_CALL_PROC_MS;
      if ( (tbl_erps[erps_idx].wtb_timer) >= (tbl_erps[erps_idx].protParam.guardTimer+5000) ) {

        localRequest = LReq_WTBExp;
        PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: WTBExp", erps_idx);

        tbl_erps[erps_idx].wtb_timer = 0;             // Reset timer value
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;  // Stop timer

      }

      // 11.
      if (localRequest == LReq_WTBExp) {
        topPriorityRequest = LReq_WTBExp;
      }

      // 12.
      else {
        topPriorityRequest = LReq_WTBRun;
      }

    }

    // 13.  R-APS (NR,RB)
    else if ( (remoteRequest == RReq_NR) && ( APS_GET_STATUS(apsStatusRx) & RReq_STAT_RB) ) {
      topPriorityRequest = RReq_NR;
    }

    // 14.  R-APS (NR)
    else if (remoteRequest == RReq_NR) {
      topPriorityRequest = RReq_NR;
    }

  }

  //--------------------------------------------------------------------------------------//
  // Operation of States
  //--------------------------------------------------------------------------------------//

  if (localRequest == LReq_SF) {

    // Process holdoff timer
    // holdoff_timer unit is ms! Proc is called every PROT_ERPS_CALL_PROC_MS
    // holdoffTimer is in steps of 100ms

    if (tbl_erps[erps_idx].protParam.holdoffTimer) {

      tbl_erps[erps_idx].holdoff_timer_previous = tbl_erps[erps_idx].holdoff_timer;
      tbl_erps[erps_idx].holdoff_timer+=PROT_ERPS_CALL_PROC_MS;
      if (tbl_erps[erps_idx].holdoff_timer < tbl_erps[erps_idx].holdoff_timer_previous) {
        memset(&tbl_erps[erps_idx].holdoff_timer, 0xFF, sizeof(tbl_erps[erps_idx].holdoff_timer));
      }

      if (tbl_erps[erps_idx].holdoff_timer < (tbl_erps[erps_idx].protParam.holdoffTimer*100)) {

        PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: hold-off Timer %d (ms)", erps_idx, tbl_erps[erps_idx].holdoff_timer);

        localRequest = LReq_NONE;
      } else {
        tbl_erps[erps_idx].holdoff_timer = 0;
      }
    }
  }


  // Process guardTimer
  // guardTimer unit is ms! Proc is called every PROT_ERPS_CALL_PROC_MS
  // guardTimer is in steps of 10ms

  if (tbl_erps[erps_idx].guard_CMD == TIMER_CMD_START) {

    tbl_erps[erps_idx].guard_timer_previous = tbl_erps[erps_idx].guard_timer;
    tbl_erps[erps_idx].guard_timer+=PROT_ERPS_CALL_PROC_MS;
    if (tbl_erps[erps_idx].guard_timer < tbl_erps[erps_idx].guard_timer_previous) {
      memset(&tbl_erps[erps_idx].guard_timer, 0xFF, sizeof(tbl_erps[erps_idx].guard_timer));
    }

    if (tbl_erps[erps_idx].guard_timer < (tbl_erps[erps_idx].protParam.guardTimer*10)) {

      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Guard Timer %d (ms)", erps_idx, tbl_erps[erps_idx].guard_timer);

      tbl_erps[erps_idx].remoteRequest = remoteRequest = RReq_NONE; /* Ignore Remote Request */
    } else {
      tbl_erps[erps_idx].guard_CMD = TIMER_CMD_STOP;
      tbl_erps[erps_idx].guard_timer = 0;
    }
  }


  // 10.1.1.  Priority logic
  //The evaluation of the top priority request is repeated every time a local request changes or an RAPS message is received.
  //Ring protection requests, commands and R-APS signals have the priorities as specified in Table 10-1.


  if ( localRequest != LReq_NONE ) {
    if (SF[PROT_ERPS_PORT0] != tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Updating SF[PROT_ERPS_PORT0] from %d to %d", erps_idx, tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0], SF[PROT_ERPS_PORT0]);
      tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0] = SF[PROT_ERPS_PORT0];
    }
    if (SF[PROT_ERPS_PORT1] != tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Updating SF[PROT_ERPS_PORT1] from %d to %d", erps_idx, tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1], SF[PROT_ERPS_PORT1]);
      tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1] = SF[PROT_ERPS_PORT1];
    }
  }

  if ( (localRequest != LReq_NONE) ) {
    if ( (tbl_erps[erps_idx].localRequest != localRequest) /*|| (tbl_erps[erps_idx].localReqPort != reqPort)*/ ) {
    
      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: localRequest: change from %s to %s on port %d", erps_idx, locReqToString[tbl_erps[erps_idx].localRequest - 100], locReqToString[localRequest - 100], reqPort);
      tbl_erps[erps_idx].localRequest = localRequest;
      tbl_erps[erps_idx].localReqPort = reqPort;

      if (topPriorityRequest>100) haveChanges = L7_TRUE;
      else PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Remote Request with higher Priority...", erps_idx);
    }
  }
  if ( remoteRequest != RReq_NONE ) {
    if ( remoteRequest != tbl_erps[erps_idx].remoteRequest ) {
      
      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: remoteRequest: change from %s to %s", erps_idx, remReqToString[tbl_erps[erps_idx].remoteRequest], remReqToString[remoteRequest]);

      tbl_erps[erps_idx].remoteRequest = remoteRequest;

      if (topPriorityRequest<100) haveChanges = L7_TRUE;
      else PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Local Request with higher Priority...", erps_idx);

    }
    else if ( (remoteRequest == RReq_NR) && ( (APS_GET_STATUS(apsReqStatusRx) & RReq_STAT_RB) != (APS_GET_STATUS(tbl_erps[erps_idx].apsReqStatusRx[apsRxPort]) & RReq_STAT_RB) ) ) {

      PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: remoteRequest: NR flags change from 0x%x to 0x%x", erps_idx, APS_GET_STATUS(apsReqStatusRx), APS_GET_STATUS(tbl_erps[erps_idx].apsReqStatusRx[apsRxPort]));

      if (topPriorityRequest<100) haveChanges = L7_TRUE;
      else PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Local Request with higher Priority...", erps_idx);
    }
  }

  if (!(haveChanges)) {
    return(PROT_ERPS_EXIT_OK);
  }

  // The flush logic retains for each ring port the information of node ID and blocked port reference 
  // (BPR) of the last R-APS message received over that ring port.
  // 
  // For each new R-APS message received over one ring port, it extracts the (node ID, BPR) pair and 
  // compares it with the previous (node ID, BPR) pair stored for that ring port. If it is different from the
  // previous pair stored, then the previous pair is deleted and the newly received (node ID, BPR) pair is
  // stored for that ring port; and if it is different from the (node ID, BPR) pair already stored at the
  // other ring port, then a flush FDB action is triggered except when the new R-APS message has DNF
  // or the receiving Ethernet ring node's node ID. An R-APS (NR) message received by this process
  // does not cause a flush FDB, however, it causes the deletion of the current (node ID, BPR) pair on
  // the receiving ring port. However, the received (node ID, BPR) pair is not stored.
  if (remoteRequest == RReq_NR) {
    memset(tbl_erps[erps_idx].apsNodeIdRx[apsRxPort], 0, PROT_ERPS_MAC_SIZE);
    tbl_erps[erps_idx].apsBprRx[apsRxPort] = 0;
  }
  else if (remoteRequest != RReq_NONE) {
    L7_uint8 bpr;

    // extracts the (node ID, BPR) pair ...
    bpr = (APS_GET_STATUS(apsStatusRx) & RReq_STAT_BPR)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0;

    // ...and compares it with the previous (node ID, BPR)
    if (memcmp(tbl_erps[erps_idx].apsNodeIdRx[apsRxPort], apsNodeIdRx, PROT_ERPS_MAC_SIZE) || (tbl_erps[erps_idx].apsBprRx[apsRxPort] != bpr))
    {
      // If it is different from the previous pair stored, then the previous pair is deleted and the newly received (node ID, BPR) pair is
      // stored for that ring port;
      memcpy(tbl_erps[erps_idx].apsNodeIdRx[apsRxPort], apsNodeIdRx, PROT_ERPS_MAC_SIZE);
      tbl_erps[erps_idx].apsBprRx[apsRxPort] = bpr;

      // and if it is different from the (node ID, BPR) pair already stored at the
      // other ring port, then a flush FDB action is triggered
      if ((memcmp(tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT0], tbl_erps[erps_idx].apsNodeIdRx[PROT_ERPS_PORT1], PROT_ERPS_MAC_SIZE)) || 
          (tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT0] != tbl_erps[erps_idx].apsBprRx[PROT_ERPS_PORT1]))
      {
        // except when the new R-APS message has DNF
        // or the receiving Ethernet ring node's node ID.
        if (!((APS_GET_STATUS(apsStatusRx) & RReq_STAT_DNF) || (tbl_erps[erps_idx].dnfStatus)))
        {
          ptin_erps_FlushFDB(erps_idx, __LINE__);
        }
      }
    }
  }

  PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Processing Changes...", erps_idx);
  if (topPriorityRequest>100) {
    PT_LOG_INFO(LOG_CTX_ERPS, "ERPS#%d: topPriorityRequest (0x%x) %s(:L), request port %d", erps_idx, topPriorityRequest, locReqToString[topPriorityRequest-100], reqPort);

    #ifdef SM_PTIN_MODS
    tbl_erps[erps_idx].remoteRequest = RReq_NONE;
    #endif
  } else {
    reqPort = apsRxPort;
    PT_LOG_INFO(LOG_CTX_ERPS, "ERPS#%d: topPriorityRequest (0x%x) %s(:R), request port %d", erps_idx, topPriorityRequest, remReqToString[topPriorityRequest], reqPort);
  }

  /* Validate */
  if (reqPort > PROT_ERPS_PORT1)
  {
    PT_LOG_ERR(LOG_CTX_ERPS,"ERPS#%d: reqPort %u not valid", erps_idx, reqPort);
    return PROT_ERPS_INDEX_VIOLATION;
  }


  //-------------------------------------------------------------------------
  //  Node state - The current state of the Ethernet Ring Node
  //-------------------------------------------------------------------------  

  switch (ERPS_STATE_GetState(tbl_erps[erps_idx].state_machine)) {

  case ERPS_STATE_Freeze:
    break;

  case ERPS_STATE_A_Idle:

    //Clear 2
    if ( topPriorityRequest == LReq_CLEAR ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
      break;
    }

    //FS 3 
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_DNF, __LINE__);
        
        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, reqPort, ERPS_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }

    //R-APS (FS)  4
    if ( topPriorityRequest == RReq_FS ) {

      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }

    //local SF  5
    if ( topPriorityRequest == LReq_SF ) {
      #ifdef SM_MODIFICATIONS

      //Check which port is in SF: PORT0
      if (reqPort == PROT_ERPS_PORT0) {

        //If failed ring port is RPL port:
        if (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) {
          // Block failed ring port
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
          }
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
          }

          // Tx R-APS (SF, DNF)
          ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

          // Unblock non-failed ring port
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
          }
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
          }

          // Set DNF status
          tbl_erps[erps_idx].dnfStatus = 1;
        }

        //Else if failed ring port is RPL next-neighbour port:
        else if (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEXTNEIGH) {
          /*** TO BE DONE - RPL next-neighbour Not yet supported ***/
          // Block failed ring port
          // Tx R-APS (SF) from failed ring port
          // Unblock non-failed ring port
        }

        //Else:
        else {
          // Block failed ring port
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
          }
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
          }

          // Tx R-APS (SF)
          ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

          // Unblock non-failed ring port
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
          }
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
          }

          //Flush FDB
          ptin_erps_FlushFDB(erps_idx, __LINE__);
        }

      } 
      //Check which port is in SF: PORT1
      else {

        //If failed ring port is RPL port:
        if (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) {
          // Block failed ring port
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
          }
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
          }

          // Tx R-APS (SF, DNF)
          ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

          // Unblock non-failed ring port
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
          }
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
          }
          
          // Set DNF status
          tbl_erps[erps_idx].dnfStatus = 1;
        }

        //Else if failed ring port is RPL next-neighbour port:
        else if (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEXTNEIGH) {   /*** TO BE DONE - RPL next-neighbour Not yet supported ***/
          // Block failed ring port
          // Tx R-APS (SF) from failed ring port
          // Unblock non-failed ring port
        }

        //Else:
        else {
          // Block failed ring port
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
          }
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
          }

          // Tx R-APS (SF)
          ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

          // Unblock non-failed ring port
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
          }
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
          }

          //Flush FDB
          ptin_erps_FlushFDB(erps_idx, __LINE__);
        }

      }

      #else

      //If failed ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {
        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }
      }
      // Else:
      else {
        // Block failed ring port
        if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
        }
        if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
        }

        // Tx R-APS (SF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-failed ring port  
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      #endif

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }

    //local clear SF  6
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
      break;
    }

    //R-APS (SF)  7
    if ( topPriorityRequest == RReq_SF ) {

      #ifdef SM_MODIFICATIONS
      //Unblock non-failed ring port
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      }
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
      }

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If not DNF
      if (!(APS_GET_STATUS(apsStatusRx) & RReq_STAT_DNF)) {
        // flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);

        #if 0 // Removed: undesired behaviors observed
        // If RPL next-neighbour node
        if ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEXTNEIGH) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEXTNEIGH)) {
          /*** TO BE DONE - RPL next-neighbour Not yet supported ***/
          //  Tx three R-APS (SF) message
          ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);
        }
        // If RPL owner node
        if ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) {
          //  Tx three R-APS (SF) message
          ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

          //  clear DNF status
          tbl_erps[erps_idx].dnfStatus = 0;
        }
        #endif
      }

      //Else:
      else {
        // If RPL owner node
        if ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) {
          //  set DNF status
          tbl_erps[erps_idx].dnfStatus = 1;
        }
      }
      #else
      //Unblock non-failed ring port
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      }
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
      }

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      #endif

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_B_Protection),__LINE__);
      break;
    }

    //R-APS (MS)  8
    if ( topPriorityRequest == RReq_MS ) {
      //Unblock non-failed ring port
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      }
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
      }

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }

    //MS 9
    if ( topPriorityRequest == LReq_MS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {

        //Tx R-APS (MS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_MS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, reqPort, ERPS_PORT_BLOCKING, __LINE__);

        // Tx R-APS (MS)
        ptin_erps_aps_tx(erps_idx, RReq_MS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }

    //WTR Expires  10
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
      break;
    }

    //WTR Running  11
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
      break;
    }

    //WTB Expires  12
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
      break;
    }

    //WTB Running  13
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
      break;
    }

    //R-APS (NR, RB)  14
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsStatusRx) & RReq_STAT_RB) ) {
      //Unblock non-RPL port
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
      }

      //If Not RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPL) ) {
        // Stop Tx R-APS  
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_A_Idle),__LINE__);
      break;
    }

    //R-APS (NR)  15
    if ( topPriorityRequest == RReq_NR ) {
      //If neither RPL Owner Node nor RPL Neighbour Node, and remote Node ID is higher than own Node ID:
      if ( ((tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPL))                   &&
           ((tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPLNEIGHBOUR) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPLNEIGHBOUR)) && 
           (memcmp(apsNodeIdRx, ERPS_NODE_ID, PROT_ERPS_MAC_SIZE) > 0) ) {

        //Unblock non-failed ring port
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }

        //Stop Tx R-APS
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_A_Idle),__LINE__);
      break;
    }

    break;


  case ERPS_STATE_B_Protection:
    
    //Clear 16
    if ( topPriorityRequest == LReq_CLEAR ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }

    //FS 17
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, reqPort, ERPS_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }

    //R-APS (FS)  18
    if ( topPriorityRequest == RReq_FS ) {
      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //local SF  19
    if ( topPriorityRequest == LReq_SF ) {
      //If failed ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {

        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }
      }
      // Else:
      else {
        // Block failed ring port
        if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
        }
        if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
        }


        // Tx R-APS (SF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-failed ring port  
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //local clear SF  20
    if ( topPriorityRequest == LReq_SFc ) {
      //Start guard timer
      ptin_erps_startTimer(erps_idx, GUARD_TIMER_CMD, TIMER_CMD_START, __LINE__);

      //Tx R-APS (NR)
      ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
        //Start WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_START, __LINE__);
      }

      #ifdef SM_PTIN_MODS
      #if 0
      //If RPL Owner Node (Neighbour) and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                             ) {
        //Start WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_START, __LINE__);
      }
      #endif
      #endif
      
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
      break;
    }


    //R-APS (SF)  21
    if ( topPriorityRequest == RReq_SF ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //R-APS (MS)  22
    if ( topPriorityRequest == RReq_MS ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //MS 23
    if ( topPriorityRequest == LReq_MS ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //WTR Expires  24
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //WTR Running  25
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //WTB Expires  26
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //WTB Running  27
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }

    //R-APS (NR, RB)  28
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsStatusRx) & RReq_STAT_RB) ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_E_Pending),__LINE__);
      break;
    }

    //R-APS (NR)  29
    if ( topPriorityRequest == RReq_NR ) {

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                    ) {

        //Start WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_START, __LINE__);

        #ifdef SM_PTIN_MODS
        // Stop Tx APS otherwise BAD stuff happen
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
        #endif
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_E_Pending),__LINE__);
      break;
    }


    break;


  case ERPS_STATE_C_ManualSwitch:

    //Clear 30
    if ( topPriorityRequest == LReq_CLEAR ) {
      //If any ring port blocked:
      if ( (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) || (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) ) {

        // Start guard timer  
        ptin_erps_startTimer(erps_idx, GUARD_TIMER_CMD, TIMER_CMD_START, __LINE__);

        // Tx R-APS (NR)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);
      }

      // If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                    ) {
        // Start WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_START, __LINE__);
      }

      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
      break;
    }


    //FS 31
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, reqPort, ERPS_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }

    //R-APS (FS)  32
    if ( topPriorityRequest == RReq_FS ) {
      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);      

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //local SF  33
    if ( topPriorityRequest == LReq_SF ) {
      //If failed ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {
        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port  
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }
      }
      // Else:
      else {
        // Block failed ring port       
        if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
        }
        if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
        }

        // Tx R-APS (SF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-failed ring port
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //local clear SF  34
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }


    //R-APS (SF)  35
    if ( topPriorityRequest == RReq_SF ) {
      //Unblock non-failed ring port
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      }
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
      }

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_B_Protection),__LINE__);
      break;
    }

    //R-APS (MS)  36
    if ( topPriorityRequest == RReq_MS ) {
      //If any ring port blocked:
      if ( (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) || (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) ) {

        // Start guard timer  
        ptin_erps_startTimer(erps_idx, GUARD_TIMER_CMD, TIMER_CMD_START, __LINE__);

        // Tx R-APS (NR)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);

        // If RPL Owner Node and revertive mode:
        if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
              (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
          // Start WTB
          ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_START, __LINE__);
        }
      }

      // Next node state: E (*) *: If both ring ports are unblocked, next node state is C
      if ( (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) ) {
        ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_C_ManualSwitch),__LINE__);
        break;
      } else {
        ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_E_Pending),__LINE__);
        break;
      }
    }

    //MS 37
    if ( topPriorityRequest == LReq_MS ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }


    //WTR Expires  38
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }

    //WTR Running  39
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }

    //WTB Expires  40
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }

    //WTB Running  41
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }

    //R-APS (NR, RB)  42
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsStatusRx) & RReq_STAT_RB) ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_E_Pending),__LINE__);
      break;
    }


    //R-APS (NR)  43
    if ( topPriorityRequest == RReq_NR ) {

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                    ) {
        //Start WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_START, __LINE__);
      }

      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_E_Pending),__LINE__);
      break;
    }


    break;


  case ERPS_STATE_D_ForcedSwitch:

    //Clear 44
    if ( topPriorityRequest == LReq_CLEAR ) {
      //If any ring port blocked:
      if ( (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) || (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) ) {
        // Start guard timer  
        ptin_erps_startTimer(erps_idx, GUARD_TIMER_CMD, TIMER_CMD_START, __LINE__);

        // Tx R-APS (NR)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);

        // If RPL Owner Node and revertive mode:
        if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
          // Start WTB
          ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_START, __LINE__);
        }
      }

      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
      break;
    }


    //FS 45
    if ( topPriorityRequest == LReq_FS ) {
      //Block requested ring port
      ptin_erps_blockOrUnblockPort(erps_idx, reqPort, ERPS_PORT_BLOCKING, __LINE__);

      //Tx R-APS (FS)
      ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

      //Flush FDB
      ptin_erps_FlushFDB(erps_idx, __LINE__);

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //R-APS (FS)  46
    if ( topPriorityRequest == RReq_FS ) {
      //No action     
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }

    //local SF  47
    if ( topPriorityRequest == LReq_SF ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //local clear SF  48
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //R-APS (SF)  49
    if ( topPriorityRequest == RReq_SF ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //R-APS (MS)  50
    if ( topPriorityRequest == RReq_MS ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //MS 51
    if ( topPriorityRequest == LReq_MS ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //WTR Expires  52
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //WTR Running  53
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //WTB Expires  54
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }

    //WTB Running  55
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    } 

    //R-APS (NR, RB)  56
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsStatusRx) & RReq_STAT_RB) ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_E_Pending),__LINE__);
      break;
    }

    //R-APS (NR)  57      
    if ( topPriorityRequest == RReq_NR ) {

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                  ) {
        //Start WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_START, __LINE__);
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_E_Pending),__LINE__);
      break;
    }

    break;


  case ERPS_STATE_E_Pending:

    //Clear 58
    if ( topPriorityRequest == LReq_CLEAR ) {
      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
        //Stop WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
      }
      //If RPL port is blocked:
      //-->RPL Port is port0 or port1?
      if ( ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) ) {

        // Tx R-APS (NR, RB,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

        // Unblock non-RPL port
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);

      } else if ( ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) ) {

        // Tx R-APS (NR, RB,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

        // Unblock non-RPL port
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);

      }
      //Else:
      else {
        //Block RPL port
        if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
        } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
        }
        
        // Tx R-APS (NR, RB)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB, __LINE__);

        // Unblock non-RPL port
        if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
      break;
    }

    //FS 59
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_DNF, __LINE__);
        
        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, reqPort, ERPS_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
        //Stop WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //R-APS (FS)  60
    if ( topPriorityRequest == RReq_FS ) {
      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
        //Stop WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
      }
           
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_D_ForcedSwitch),__LINE__);
      break;
    }


    //local SF  61
    if ( topPriorityRequest == LReq_SF ) {
      //If failed ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {

        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port  
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }
      }
      // Else:
      else {
          // Block failed ring port  
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
          }
          if (tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
          }

          // Tx R-APS (SF)
          ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

          // Unblock non-failed ring port
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
          }
          if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
          }

          // Flush FDB
          ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
        //Stop WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
      }

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //local clear SF  62
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
      break;
    }


    //R-APS (SF)  63
    if ( topPriorityRequest == RReq_SF ) {
      //Unblock non-failed ring port
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      }
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
      }

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {

        #ifdef SM_MODIFICATIONS
        //and not DNF
        if (!(APS_GET_STATUS(apsStatusRx) & RReq_STAT_DNF)) {
          //   clear DNF status 
          tbl_erps[erps_idx].dnfStatus = 0;
        }
        #endif

        //Stop WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
        //Stop WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
      }
      
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_B_Protection),__LINE__);
      break;
    }


    //R-APS (MS)  64
    if ( topPriorityRequest == RReq_MS ) {
      //Unblock non-failed ring port
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
      }
      if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
      }

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
        //Stop WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
      }

      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }

    //MS 65
    if ( topPriorityRequest == LReq_MS ) {

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
        //Stop WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
      }

      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[reqPort] == ERPS_PORT_BLOCKING) {

        //Tx R-APS (MS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_MS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, reqPort, ERPS_PORT_BLOCKING, __LINE__);

        // Tx R-APS (MS)
        ptin_erps_aps_tx(erps_idx, RReq_MS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (reqPort == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERPS_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
      break;
    }

    //WTR Expires  66
    if ( topPriorityRequest == LReq_WTRExp ) {
      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        
        //Stop WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
        
        //If RPL port is blocked:
        if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) ) {

          //Tx R-APS (NR, RB,DNF)   
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          //Unblock non-RPL port
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
          
        } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) ) {

          //Tx R-APS (NR, RB,DNF)   
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          //Unblock non-RPL port
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);          
        }

        //Else :
        else {
          // Block RPL port
          if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
          } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
          }

          #ifdef SM_MODIFICATIONS
          //If DNF status
          if (tbl_erps[erps_idx].dnfStatus) {
            //   Tx R-APS (NR, RB, DNF)
            ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);
          }
          //Else:
          else {
            //   Tx R-APS (NR, RB)
            ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB, __LINE__);

            //   Flush FDB
            ptin_erps_FlushFDB(erps_idx, __LINE__);
          }
          
          //  Unblock non-RPL port
          if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
          } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
          }

          //  clear DNF status
          tbl_erps[erps_idx].dnfStatus = 0;
          
          #else

          // Tx R-APS (NR, RB)   
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB, __LINE__);

          // Unblock non-RPL port   
          if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
          } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
          }

          // Flush FDB
          ptin_erps_FlushFDB(erps_idx, __LINE__);
          #endif
        }
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
      break;
    }


    //WTR Running  67
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
      break;
    }

    //WTB Expires  68
    if ( topPriorityRequest == LReq_WTBExp ) {
      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {

        //Stop WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_STOP, __LINE__);

        // If RPL port is blocked:   
        if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) ) {

          // Tx R-APS (NR, RB,DNF)
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          // Unblock non-RPL port
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);

        } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) ) {

          // Tx R-APS (NR, RB,DNF)
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          // Unblock non-RPL port
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);

        }
        //Else:
        else {
          //Block RPL port
          if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
          } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
          }
          
          // Tx R-APS (NR, RB)  
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB, __LINE__);

          // Unblock non-RPL port
          if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
          } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEXTNEIGH) ) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
          }

          // Flush FDB
          ptin_erps_FlushFDB(erps_idx, __LINE__);
        }
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
      break;
    }


    //WTB Running  69
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
      break;
    } 

    //R-APS (NR, RB)  70
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsStatusRx) & RReq_STAT_RB) ) {
      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        ptin_erps_startTimer(erps_idx, WTR_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
        //Stop WTB
        ptin_erps_startTimer(erps_idx, WTB_TIMER_CMD, TIMER_CMD_STOP, __LINE__);
      }

      //If neither RPL Owner Node nor RPL Neighbour Node:
      if ( ((tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPL))                   &&
           ((tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPLNEIGHBOUR) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPLNEIGHBOUR))     ) {

        //Unblock ring ports
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);

        //Stop Tx R-APS
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      //If RPL Neighbour Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
        //Block RPL port
        //Unblock non-RPL port
        if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR ) {             // Port0 is RPL
        	ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_BLOCKING, __LINE__);
        	ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        } else {                                                                                  // Port1 is RPL
        	ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_BLOCKING, __LINE__);
        	ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }

        //Stop Tx R-APS      
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

        #ifdef SM_PTIN_MODS
        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
        #endif
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_A_Idle),__LINE__);
      break;
    }


    //R-APS (NR)  71
    if ( topPriorityRequest == RReq_NR ) {
      //If remote Node ID is higher than own Node ID:
      if (memcmp(apsNodeIdRx, ERPS_NODE_ID, PROT_ERPS_MAC_SIZE) > 0) {
        //Unblock non-failed ring port
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERPS_PORT_FLUSHING, __LINE__);
        }
        if (!tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERPS_PORT_FLUSHING, __LINE__);
        }

        // Stop Tx R-APS
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERPS_STATE_SetRemote(ERPS_STATE_E_Pending),__LINE__);
      break;
    }

    break;

  }//switch

  if ( (tbl_erps[erps_idx].operator_cmd == PROT_ERPS_OPCMD_OC) ) {
    PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: Operator Command CLEAR", erps_idx);

    tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]       = PROT_ERPS_SF_CLEAR;
    tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]       = PROT_ERPS_SF_CLEAR;

    tbl_erps[erps_idx].wtr_timer                        = 0;
    tbl_erps[erps_idx].wtr_CMD                          = TIMER_CMD_STOP;
    tbl_erps[erps_idx].wtb_timer                        = 0;
    tbl_erps[erps_idx].wtb_CMD                          = TIMER_CMD_STOP;
    tbl_erps[erps_idx].holdoff_timer                    = 0;
    tbl_erps[erps_idx].holdoff_timer_previous           = 0;

    tbl_erps[erps_idx].operator_cmd                     = PROT_ERPS_OPCMD_NR;
    tbl_erps[erps_idx].operator_cmd_port                = PROT_ERPS_PORT0;

    tbl_erps[erps_idx].localRequest                     = LReq_NONE;
    tbl_erps[erps_idx].localReqPort                     = PROT_ERPS_PORT0;
    tbl_erps[erps_idx].remoteRequest                    = RReq_NONE;
  }

  // Give some time to have system stability
  tbl_erps[erps_idx].waitingcicles = PROT_ERPS_WAITING_CICLES_PROC;

  PT_LOG_TRACE(LOG_CTX_ERPS, "ERPS#%d: ... Changes done!\n\n", erps_idx);

  return(PROT_ERPS_EXIT_OK);
}


/**
 * ERPS Periodic Callback
 * 
 * @author joaom (6/5/2013)
 * 
 * @return int 
 */
int ptin_prot_erps_proc(void)
{
  L7_uint8 erps_idx;
//L7_uint8 flush_pending = 0;

  // CONTROL
  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++)
  {
    osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

    if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE)
    {
      osapiSemaGive(ptin_prot_erps_sem);
      continue;
    }

    tbl_erps[erps_idx].hal.prot_proc(erps_idx);

    osapiSemaGive(ptin_prot_erps_sem);
  }

  // HW Sync
  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++)
  {
    osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

    if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE)
    {
      osapiSemaGive(ptin_prot_erps_sem);
      continue;
    }

    ptin_hal_erps_hwSync(erps_idx);

//  flush_pending |= tbl_halErps[erps_idx].hwFdbFlush;
//  tbl_halErps[erps_idx].hwFdbFlush = 0;

    osapiSemaGive(ptin_prot_erps_sem);
  }

//if (flush_pending) {
//  PT_LOG_TRACE(LOG_CTX_ERPS, "Flushing!\n");
//  fdbFlush();
//}

  // HW FDB Flush
#if 1
  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++)
  {
    osapiSemaTake(ptin_prot_erps_sem, L7_WAIT_FOREVER);

    if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE)
    {
      osapiSemaGive(ptin_prot_erps_sem);
      continue;
    }

    ptin_hal_erps_hwFdbFlush(erps_idx);

    if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_REMOVE_PENDING)
    {
      ptin_hal_erps_queue_vlans_used_clear(erps_idx);
      tbl_erps[erps_idx].admin = PROT_ERPS_ENTRY_FREE;
    }

    osapiSemaGive(ptin_prot_erps_sem);
  }
#endif

  return(PROT_ERPS_EXIT_OK);
}


/****************************************************************************** 
 * ERPS Task Init
 ******************************************************************************/

// Task id
L7_uint32 erps_TaskId = L7_ERROR;

/**
 * Task with infinite loop
 */
void ptin_erps_task(void)
{
  L7_uint64 time_ref;

  PT_LOG_INFO(LOG_CTX_ERPS,"ERPS Task started");

  if (osapiTaskInitDone(L7_PTIN_ERPS_TASK_SYNC)!=L7_SUCCESS) {
    PT_LOG_FATAL(LOG_CTX_ERPS, "Error syncing task");
    PTIN_CRASH();
  }

  PT_LOG_INFO(LOG_CTX_ERPS,"ERPS task ready");

  ptin_erps_init();

  ptin_hal_erps_init();

  /* Infinite Loop */
  while (1) {
    time_ref = osapiTimeMicrosecondsGet();
    
    ptin_prot_erps_proc();

    usleep(PROT_ERPS_CALL_PROC_US);

    proc_runtime_meter_update(PTIN_PROC_ERPS_INSTANCE, (L7_uint32) (osapiTimeMicrosecondsGet() - time_ref));
  }
}




/**
 * Initializes ERPS module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_prot_erps_init(void)
{
  /* Create semaphore to control concurrent accesses */
  ptin_prot_erps_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (ptin_prot_erps_sem == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Failed to create ptin_prot_erps_sem semaphore!");
    return L7_FAILURE;
  }

  /* Create task for ERProtection State Machine */
  erps_TaskId = osapiTaskCreate("ptin_prot_erps_task", ptin_erps_task, 0, 0,
                                L7_DEFAULT_STACK_SIZE,
                                11,
                                0);

  if (erps_TaskId == L7_ERROR) {
    PT_LOG_FATAL(LOG_CTX_CNFGR, "Could not create task ptin_prot_erps_task");
    return L7_FAILURE;
  }
  PT_LOG_INFO(LOG_CTX_CNFGR,"Task ptin_prot_erps_task created");

  if (osapiWaitForTaskInit (L7_PTIN_ERPS_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS) {
    PT_LOG_FATAL(LOG_CTX_CNFGR,"Unable to initialize ptin_prot_erps_task()\n");
    return(L7_FAILURE);
  }
  PT_LOG_INFO(LOG_CTX_CNFGR,"Task ptin_prot_erps_task initialized");

  return L7_SUCCESS;
}



/****************************************************************************/

/*
 * TEST
 */
void ptin_prot_erps_test(int test, int param1, int param2, int param3, int param4, int param5)
{
  if (test == 0) {
    PT_LOG_INFO(LOG_CTX_ERPS,"Test 0: Add prot ERPS#%d", param1);

    erpsProtParam_t new_group;
    memset(&new_group, 0, sizeof(erpsProtParam_t));
    new_group.controlVid = param2;
    new_group.port0.idx =  param3;
    new_group.port1.idx =  param4;
    new_group.ringId =     param5;
    ptin_erps_add_entry( /*erps_idx*/ param1, &new_group);

    ptin_hal_erps_entry_init(/*erps_idx*/ param1);

  }
  else if (test == 1) {
    PT_LOG_INFO(LOG_CTX_ERPS,"Test 1: Rem prot ERPS#%d", param1);

    ptin_erps_remove_entry( /*erps_idx*/ param1);

    ptin_hal_erps_entry_deinit(/*erps_idx*/ param1);

  }
  else if (test == 2) {
    PT_LOG_INFO(LOG_CTX_ERPS,"Test 2: ERPS#%d Port State", param1);

    tbl_erps[param1].portState[PROT_ERPS_PORT0] = param2 == 1? ERPS_PORT_BLOCKING : ERPS_PORT_FLUSHING;
    tbl_erps[param1].portState[PROT_ERPS_PORT1] = param3 == 1? ERPS_PORT_BLOCKING : ERPS_PORT_FLUSHING;
  }
  else {
    PT_LOG_INFO(LOG_CTX_ERPS,"\n\nUSAGE: \n"
                          "  Test 0: Add prot\n"
                          "  Test 1: Rem prot\n"
                          "  Test 2: Port State\n");
  }
}

#endif  // PTIN_ENABLE_ERPS
