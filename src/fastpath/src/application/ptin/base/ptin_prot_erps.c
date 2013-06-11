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

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ptin_prot_erps.h"
#include "logger.h"
#include "ptin_cnfgr.h"


/* *******************************************************************************/
/*                                  GLOBAL VARS                                  */
/* *******************************************************************************/

tblErps_t               tbl_erps[MAX_PROT_PROT_ERPS];
erpsVlanInclusionList_t tbl_erps_vlanList[MAX_PROT_PROT_ERPS];

static const char *stateToString[]  = {"FRZ", "Z_Init", "A_Idle", "B_Protection", "C_ManualSwitch", "D_ForceSwitch", "E_Pending"};
static const char *locReqToString[] = {"0", "WTBRun", "WTBExp", "WTRRun", "WTRExp", "MS", "SFc", "SF", "FS", "CLEAR", "NONE", "11", "12"};
static const char *remReqToString[] = {"NR", "1", "2", "3", "4", "5", "6", "MS", "8", "9", "10", "SF", "12", "FS", "EVENT", "NONE"};


/* *******************************************************************************/
/*                                   FUNCTIONS                                   */
/* *******************************************************************************/

/* dummy */
int rd_alarms_dummy(L7_uint8 slot, L7_uint32 index)
{
  return(0);
}

int aps_rxdummy(L7_uint8 slot, L7_uint32 index, L7_uint8* msgfields)
{
  return(0);
}

int aps_txdummy(L7_uint8 slot, L7_uint32 index, L7_uint8 msgfields)
{
  return(0);
}

int switch_path_dummy(L7_uint32 erps_idx, L7_uint8 path, L7_uint8 difunde0_naodifunde1)
{
  return(0);
}

int prot_proc_dummy(L7_uint32 prot_id)
{
  return(0);
}


int ptin_prot_erps_instance_proc(L7_uint32 erps_idx);

int ptin_erps_aps_tx(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status, int line_callback);


/**
 * ERPS Instace# initialization
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx
 * 
 * @return int
 */
int ptin_erps_init_entry(L7_uint32 erps_idx)
{
  int ret = erps_idx;

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d", erps_idx);

  if ((erps_idx>=MAX_PROT_PROT_ERPS)) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].admin                        = PROT_ERPS_ENTRY_FREE;

  tbl_erps[erps_idx].protParam.ringId             = 0;

  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]   = PROT_ERPS_SF_CLEAR;
  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]   = PROT_ERPS_SF_CLEAR;

  tbl_erps[erps_idx].wtr_timer                    = 0;
  tbl_erps[erps_idx].wtr_CMD                      = TIMER_CMD_STOP;
  tbl_erps[erps_idx].wtb_timer                    = 0;
  tbl_erps[erps_idx].wtb_CMD                      = TIMER_CMD_STOP;
  tbl_erps[erps_idx].holdoff_timer                = 0;
  tbl_erps[erps_idx].holdoff_timer_previous       = 0;

  tbl_erps[erps_idx].operator_cmd                 = PROT_ERPS_OPCMD_NR;
  tbl_erps[erps_idx].operator_cmd_port            = 0;

  tbl_erps[erps_idx].localRequest                 = LReq_NONE;
  tbl_erps[erps_idx].remoteRequest                = RReq_NONE;

  tbl_erps[erps_idx].apsReqStatusTx               = 0;
  tbl_erps[erps_idx].apsReqStatusRx               = 0;
  memset(tbl_erps[erps_idx].apsNodeIdRx,            0, PROT_ERPS_MAC_SIZE);

  tbl_erps[erps_idx].portState[PROT_ERPS_PORT0]   = ERP_PORT_FLUSHING;
  tbl_erps[erps_idx].portState[PROT_ERPS_PORT1]   = ERP_PORT_FLUSHING;
  tbl_erps[erps_idx].rplBlockedPortSide           = PROT_ERPS_PORT0;

  tbl_erps[erps_idx].state_machine                = ERP_STATE_SetLocal(ERPS_STATE_Z_Init);


  tbl_erps[erps_idx].hal.rd_alarms                = rd_alarms_dummy;
  tbl_erps[erps_idx].hal.aps_rxfields             = aps_rxdummy;
  tbl_erps[erps_idx].hal.aps_txfields             = aps_txdummy;

  tbl_erps[erps_idx].hal.switch_path              = switch_path_dummy;
  tbl_erps[erps_idx].hal.prot_proc                = prot_proc_dummy;

  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
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
int ptin_erps_vlanList_init_entry(L7_uint32 erps_idx)
{
  int       ret = PROT_ERPS_EXIT_OK;

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d", erps_idx);

  if ((erps_idx>=MAX_PROT_PROT_ERPS)) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  memset(tbl_erps_vlanList[erps_idx].vid_bmp, 0, (1<<12)/(sizeof(L7_uint8)*8));
  memset(tbl_erps_vlanList[erps_idx].isOwnerVid_bmp, 0, (1<<12)/(sizeof(L7_uint8)*8));

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
  L7_uint32 erps_idx;

  LOG_INFO(LOG_CTX_ERPS, "Initializing ERPS Module");

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    ptin_erps_init_entry(erps_idx);
    ptin_erps_vlanList_init_entry(erps_idx);
  }
  
  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
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
  L7_uint32 erps_idx;

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
      ret = erps_idx;
      LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
      return(ret);
    }
  }
  
  LOG_WARNING(LOG_CTX_ERPS, "ret:%d, done.", ret);
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
int ptin_erps_add_entry( L7_uint32 erps_idx, erpsProtParam_t *new_group)
{
  int ret = erps_idx;

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d", erps_idx);

  if (erps_idx >= MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_BUSY) {
    ret = PROT_ERPS_INDEX_IN_USE;
    LOG_WARNING(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  memcpy( &tbl_erps[erps_idx].protParam, new_group, sizeof(erpsProtParam_t) );

  /*** TO BE DONE *** Maybe more validation are needed ***/

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
  tbl_erps[erps_idx].guard_CMD                  = TIMER_CMD_STOP;

  tbl_erps[erps_idx].holdoff_timer              = 0;
  tbl_erps[erps_idx].holdoff_timer_previous     = 0;

  tbl_erps[erps_idx].operator_cmd               = PROT_ERPS_OPCMD_NR;
  tbl_erps[erps_idx].operator_cmd_port          = PROT_ERPS_PORT0;

  tbl_erps[erps_idx].localRequest               = LReq_NONE;
  tbl_erps[erps_idx].remoteRequest              = RReq_NONE;

  tbl_erps[erps_idx].apsReqStatusTx             = 0;
  tbl_erps[erps_idx].apsReqStatusRx             = 0;
  memset(tbl_erps[erps_idx].apsNodeIdRx,          0, PROT_ERPS_MAC_SIZE);

  tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] = ERP_PORT_FLUSHING;
  tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] = ERP_PORT_FLUSHING;
  tbl_erps[erps_idx].rplBlockedPortSide         = PROT_ERPS_PORT0;

  tbl_erps[erps_idx].state_machine              = ERP_STATE_SetLocal(ERPS_STATE_A_Idle);

  tbl_erps[erps_idx].waitingstates              = 0;

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  /*** TO BE DONE ***/
//tbl_erps[erps_idx].hal.rd_mep_alarms          = oam_get_alarms;

  tbl_erps[erps_idx].hal.prot_proc              = ptin_prot_erps_instance_proc;

  tbl_erps[erps_idx].admin                      = PROT_ERPS_ENTRY_BUSY;

  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Used to reconfigure an existing entry
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param conf 
 * 
 * @return int 
 */
int ptin_erps_conf_entry(L7_uint32 erps_idx, erpsProtParam_t *conf)
{
  int ret = erps_idx;

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin==PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_WARNING(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].protParam.revertive              = conf->revertive;

  tbl_erps[erps_idx].protParam.holdoffTimer           = conf->holdoffTimer;
  tbl_erps[erps_idx].protParam.guardTimer             = conf->guardTimer;
  tbl_erps[erps_idx].protParam.waitToRestoreTimer     = conf->waitToRestoreTimer;

  tbl_erps[erps_idx].protParam.continualTxInterval    = conf->continualTxInterval;
  tbl_erps[erps_idx].protParam.rapidTxInterval        = conf->rapidTxInterval;


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
  
  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
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
int ptin_erps_remove_entry(L7_uint32 erps_idx)
{
  int ret = erps_idx;

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    LOG_NOTICE(LOG_CTX_ERPS, "Entry free.", ret);
  }

  ptin_erps_init_entry(erps_idx);
  ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
  
  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
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

  LOG_TRACE(LOG_CTX_ERPS, "");

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    if ((ret=ptin_erps_remove_entry(erps_idx))!=erps_idx) {
      LOG_ERR(LOG_CTX_ERPS, "ERROR: (%d) while removing ERPS# %d\n\r", ret, erps_idx);
      ret = PROT_ERPS_EXIT_NOK1;
    }
  }

  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
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
int ptin_erps_get_entry(L7_uint32 erps_idx, erpsProtParam_t *group)
{
  int ret = erps_idx;

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin==PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  memcpy(group, &tbl_erps[erps_idx].protParam, sizeof(erpsProtParam_t));
  
  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
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
int ptin_erps_cmd_clear(L7_uint32 erps_idx)
{
  int ret = erps_idx;

  LOG_INFO(LOG_CTX_ERPS, "ERPS# %d", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].operator_cmd       = PROT_ERPS_OPCMD_OC;
  tbl_erps[erps_idx].operator_cmd_port  = 0;  
  
  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Operator Command
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param switch_path 
 * 
 * @return int 
 */
int ptin_erps_cmd_force(L7_uint32 erps_idx, L7_uint8 switch_path)
{
  int ret = erps_idx;

  LOG_INFO(LOG_CTX_ERPS, "ERPS# %d: %d", erps_idx, switch_path);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  /*** TO BE DONE ***/
  
  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Operator Command
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param switch_path 
 * 
 * @return int 
 */
int ptin_erps_cmd_manual(L7_uint32 erps_idx, L7_uint8 switch_path)
{
  int ret = erps_idx;

  LOG_INFO(LOG_CTX_ERPS, "ERPS# %d: %d", erps_idx, switch_path);


  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  /*** TO BE DONE ***/

  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
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
int ptin_erps_get_status(L7_uint32 erps_idx, erpsStatus_t *status)
{
  int ret = erps_idx;

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    return(ret);
  }

  if (tbl_erps[erps_idx].admin==PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    return(ret);
  }

  /*** TO BE DONE ***/

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
int ptin_erps_rd_entry(L7_uint32 erps_idx)
{
  int ret = erps_idx;

  char *strBool[]       = { "FALSE", "TRUE", "FUZZY :) " };
  char *strPortRole[]   = { "NON RPL", "RPL Owner", "RPL Neighbour" };
  char *strRevertive[]  = { "Non-Revertive", "Revertive" };
  char *strTimerCmd[]   = { "STOP", "START" };
  char *strCmd[]        = { "NR", "OC", "LO", "FS", "MS", "ReplaceRPL", "ExeSignal" };
  char *strCmdPath[]    = { "Switch to PORT0", "Switch to PORT1", "NONE" };
  char *strPortState[]  = { "BLOCKING", "FLUSHING" };

  LOG_INFO(LOG_CTX_ERPS, "ERPS# %d", erps_idx);

  /*** TO BE DONE ***/

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_ERR(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  printf("\n-----------------------------------------");
  printf("\n  admin               %d",                      tbl_erps[erps_idx].admin);
  printf("\n-----------------------------------------");
  printf("\n\nERPS Protection Parameters:\n");
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
  printf("\n\nERPS States:\n");
  printf("\n  status_SF[PORT0]    %d",                      tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]);
  printf("\n  status_SF[PORT1]    %d",                      tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]);
  printf("\n");
  printf("\n  wtr_CMD             %s",                      strTimerCmd[tbl_erps[erps_idx].wtr_CMD]);
  printf("\n  wtb_CMD             %s",                      strTimerCmd[tbl_erps[erps_idx].wtb_CMD]);
  printf("\n  guard_CMD           %s",                      strTimerCmd[tbl_erps[erps_idx].guard_CMD]);
  printf("\n");
  printf("\n  wtr_timer           %d",                      tbl_erps[erps_idx].wtr_timer);
  printf("\n  wtb_timer           %d",                      tbl_erps[erps_idx].wtb_timer);
  printf("\n  guard_timer         %d",                      tbl_erps[erps_idx].guard_timer);
  printf("\n  holdoff_timer       %d",                      tbl_erps[erps_idx].holdoff_timer);
  printf("\n  holdoff_timer_p     %d",                      tbl_erps[erps_idx].holdoff_timer_previous);
  printf("\n"); 
  printf("\n  operator_cmd        (0x%x) %s",               tbl_erps[erps_idx].operator_cmd, strCmd[tbl_erps[erps_idx].operator_cmd]);
  printf("\n  operator_cmd_port   (%d)   %s",               tbl_erps[erps_idx].operator_cmd_port, strCmdPath[tbl_erps[erps_idx].operator_cmd_port]);
  printf("\n-----------------------------------------");
  printf("\nAPS State Machine:\n");
  printf("\n  localRequest        %s",                      locReqToString[tbl_erps[erps_idx].localRequest]);
  printf("\n  remoteRequest       %s",                      remReqToString[tbl_erps[erps_idx].remoteRequest]);
  printf("\n  apsReqStatusTx      (0x%x) %s",               tbl_erps[erps_idx].apsReqStatusTx, remReqToString[APS_GET_REQ(tbl_erps[erps_idx].apsReqStatusTx)]);
  printf("\n  apsReqStatusRx      (0x%x) %s",               tbl_erps[erps_idx].apsReqStatusRx, remReqToString[APS_GET_REQ(tbl_erps[erps_idx].apsReqStatusRx)]);
  printf("\n  portState[PORT0]    (0x%x) %s",               tbl_erps[erps_idx].portState[PROT_ERPS_PORT0], strPortState[tbl_erps[erps_idx].portState[PROT_ERPS_PORT0]]);
  printf("\n  portState[PORT1]    (0x%x) %s",               tbl_erps[erps_idx].portState[PROT_ERPS_PORT1], strPortState[tbl_erps[erps_idx].portState[PROT_ERPS_PORT1]]);
  printf("\n  rplBlockedPortSide  %d",                      tbl_erps[erps_idx].rplBlockedPortSide);
  printf("\n  state_machine       (0x%x) %s:%s",            tbl_erps[erps_idx].state_machine,
                                                            stateToString[ERP_STATE_GetState(tbl_erps[erps_idx].state_machine)],
                                                            ERP_STATE_IsLocal(tbl_erps[erps_idx].state_machine)? "L":"R");
  printf("\n-----------------------------------------\n\r");

  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
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

  L7_uint32 erps_idx;

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    printf("\n-----------------------------------------");
    printf("\n ERPS# %d: admin     %d",                      erps_idx, tbl_erps[erps_idx].admin);
    printf("\n-----------------------------------------");
    printf("\n ERPS Protection Parameters:");
    printf("\n ringId              %d",                      tbl_erps[erps_idx].protParam.ringId);
    printf("\n controlVid          %d",                      tbl_erps[erps_idx].protParam.controlVid);
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
int ptin_erps_dump(L7_uint32 erps_idx)
{
  int ret = PROT_ERPS_EXIT_OK;

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ptin_erps_rd_allentry();
  } else {
    ptin_erps_rd_entry(erps_idx);
  }

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
int ptin_erps_blockOrUnblockPort(L7_uint32 erps_idx, L7_uint8 port, L7_uint8 portState, int line_callback)
{
  int ret = PROT_ERPS_EXIT_OK;

  LOG_INFO(LOG_CTX_ERPS, "ERPS# %d: port %d, portState %d (line_callback %d)", erps_idx, port, portState, line_callback);

  tbl_erps[erps_idx].portState[port] = portState;

  tbl_erps[erps_idx].hal.switch_path(erps_idx, port, 0);

  //reg_alarm(0, erps_idx, path);  -> Must be done inside hal.switch_path() function
  
  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Get condition of either failed (i.e., signal fail (SF)) or 
 * non-failed (OK) 
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * @param path 
 * 
 * @return int 
 */
int ptin_erps_rd_alarms(L7_uint32 erps_idx, L7_uint8 path)
{
  int ret = PROT_ERPS_EXIT_OK;

  LOG_TRACE(LOG_CTX_ERPS,"ERPS# %d, path %d", erps_idx, path);

  tbl_erps[erps_idx].hal.rd_alarms(0,0);

  return(ret);
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
int ptin_erps_aps_tx(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status, int line_callback)
{
  int ret = PROT_ERPS_EXIT_OK;

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d: %d(%d) (line_callback %d)", erps_idx, req_state, status, line_callback);

  tbl_erps[erps_idx].hal.aps_txfields(0, req_state, status);

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
int ptin_erps_aps_rx(L7_uint32 erps_idx, L7_uint8 *req_status, L7_uint8 *nodeid, L7_uint32 *rxport, int line_callback)
{
  int ret = PROT_ERPS_EXIT_OK;

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d (line_callback %d)", erps_idx, line_callback);

  tbl_erps[erps_idx].hal.aps_rxfields(0, 0, 0);

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
int ptin_erps_FSM_transition(L7_uint32 erps_idx, L7_uint8 state, int line_callback)
{
  int ret = PROT_ERPS_EXIT_OK;

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d: Changing state from %d to %d (line_callback %d)", erps_idx, tbl_erps[erps_idx].state_machine, state, line_callback);

  tbl_erps[erps_idx].state_machine_h  = tbl_erps[erps_idx].state_machine;
  tbl_erps[erps_idx].state_machine    = state;

  return(ret);
}


/**
 * Force APS transmission
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
int prot_aps_forceTx(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status)
{
  int ret = erps_idx;

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
    return(ret);
  }

  if (req_state > 16) {
    LOG_TRACE(LOG_CTX_ERPS, "erps_idx ID %d: Invalid Req", erps_idx);
  }

  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d: Request Tx = %s(%d)", erps_idx, remReqToString[req_state], status);

  ptin_erps_aps_tx(erps_idx, req_state, status, __LINE__);
  
  LOG_TRACE(LOG_CTX_ERPS, "ret:%d, done.", ret);
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
int ptin_erps_FlushFDB(L7_uint32 erps_idx, int line_callback)
{
  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d: Flushing FDB (line_callback %d)", erps_idx, line_callback);

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
int ptin_prot_erps_instance_proc(L7_uint32 erps_idx)
{

  L7_uint8  SF[2];

  L7_uint8  localRequest        = LReq_NONE;
  L7_uint8  remoteRequest       = LReq_NONE;
  L7_uint8  apsReqStatusRx      = 0;
  L7_uint32 apsRxPort           = 0;
  L7_uint8  topPriorityRequest  = LReq_NONE;  //The current top priority request as defined in sub-clause 10.1.1.
  L7_BOOL   haveChanges         = L7_FALSE;
  L7_uint8  req_port            = 0;          // Request or Failed Port


  LOG_TRACE(LOG_CTX_ERPS,"ERPS# %d: admin %d", erps_idx, tbl_erps[erps_idx].admin);

  if (tbl_erps[erps_idx].waitingstates) {
    tbl_erps[erps_idx].waitingstates--;
    return(erps_idx);
  }


  //--------------------------------------------------------------------------------------------
  // Alarm Indication (SF)
  SF[PROT_ERPS_PORT0] = ptin_erps_rd_alarms(erps_idx, PROT_ERPS_PORT0);
  SF[PROT_ERPS_PORT1] = ptin_erps_rd_alarms(erps_idx, PROT_ERPS_PORT1);


  //--------------------------------------------------------------------------------------------
  // R-APS request
  ptin_erps_aps_rx(erps_idx, &apsReqStatusRx, tbl_erps[erps_idx].apsNodeIdRx, &apsRxPort, __LINE__);

  remoteRequest = APS_GET_REQ(apsReqStatusRx);
  
  LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d: Received R-APS Request(0x%x) = %s(0x%x)", erps_idx, remoteRequest,
            remReqToString[remoteRequest], APS_GET_STATUS(apsReqStatusRx));


  //--------------------------------------------------------------------------------------------
  // 10.1.9.  Local Priority Logic
  //--------------------------------------------------------------------------------------------
  //Local priority logic evaluates the local operator commands (in ETH_C_MI_RAPS_ExtCMD)
  //according to the current top priority request. The commands Clear, Manual Switch and Forced
  //Switch from the operator, are forwarded to the Priority Logic.

  //--------------------------------------------------------------------------------------------
  // Operator Command
  if ( tbl_erps[erps_idx].operator_cmd ) {

    if ( (tbl_erps[erps_idx].operator_cmd & PROT_ERPS_OPCMD_OC) ) {
      //10.1.9. Local Priority Logic
      //The Clear command is only valid if:
      //a) a local Forced Switch or Manual Switch command is in effect (Clear operation a) described in clause 8), or
      //b) a local Ethernet Ring Node is an RPL Owner Node and top priority request is neither R-APS (FS) nor R-APS (MS) (Clear operation  b) or  c) described in clause 8).

      // *** Verification of a) is done when command is received ***

      /// TO BE DONE


      // *** Verification of b) ***

      /// TO BE DONE


      localRequest = LReq_CLEAR;
      req_port = tbl_erps[erps_idx].operator_cmd_port;
    }

    else if ( (tbl_erps[erps_idx].operator_cmd & PROT_ERPS_OPCMD_FS) ) {
      localRequest = LReq_FS;
      req_port = tbl_erps[erps_idx].operator_cmd_port;
    }

    else if ( (tbl_erps[erps_idx].operator_cmd & PROT_ERPS_OPCMD_MS) ) {
      localRequest = LReq_MS;
      req_port = tbl_erps[erps_idx].operator_cmd_port;
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

    if ( (SF[PROT_ERPS_PORT0] != tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]) ) {
      if ( SF[PROT_ERPS_PORT0] ) {
        localRequest = LReq_SF;
        req_port = PROT_ERPS_PORT0;
        LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d, SF[PROT_ERPS_PORT0]", erps_idx);
      } else {
        localRequest = LReq_SFc;
        req_port = PROT_ERPS_PORT0;
        LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d, SF[PROT_ERPS_PORT0] Clear", erps_idx);
      }
    }

    if ( (SF[PROT_ERPS_PORT1] != tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]) && (localRequest != LReq_SF) ) {
      if ( SF[PROT_ERPS_PORT1] ) {
        localRequest = LReq_SF;
        req_port = PROT_ERPS_PORT1;
        LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d, SF[PROT_ERPS_PORT1]", erps_idx);
      } else {
        localRequest = LReq_SFc;
        req_port = PROT_ERPS_PORT1;
        LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d, SF[PROT_ERPS_PORT1] Clear", erps_idx);
      }
    }


    // 4.   Local Signal Fail (OAM / control-plane / server indication)
    else if (localRequest == LReq_SF) {
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

      // WaitToRestore em multiplos de 1min
      // wait_restore_timer em ms! Passam 10ms cada vez que se entra neste ciclo

      // Wait_restore_timer/60000 converter para ms!

      tbl_erps[erps_idx].wtr_timer+=PROT_ERPS_CALL_PROC_MS;
      if ( (tbl_erps[erps_idx].wtr_timer/60000) >= tbl_erps[erps_idx].protParam.waitToRestoreTimer ) {

        localRequest = LReq_WTRExp;
        LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d, WTRExp", erps_idx);

        tbl_erps[erps_idx].wtr_timer = 0;          // Reset timer value
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP; // Stop timer

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
    else if (localRequest == LReq_WTBExp) {
      /// TO BE DONE
      topPriorityRequest = LReq_WTBExp;
    }

    // 12.  WTB Running (WTB timer)
    else if (localRequest == LReq_WTRRun) {
      /// TO BE DONE
      topPriorityRequest = LReq_WTRRun;
    }

    // 13.  R-APS (NR,RB)
    else if ( (remoteRequest == RReq_NR) && ( APS_GET_STATUS(apsReqStatusRx) & RReq_STAT_RB) ) {
      /// TO BE DONE
      topPriorityRequest = RReq_NR;
      
      // Rx Port information
      tbl_erps[erps_idx].rplBlockedPortSide = apsRxPort;
    }

    // 14.  R-APS (NR)
    else if (remoteRequest == RReq_NR) {
      /// TO BE DONE
      topPriorityRequest = RReq_NR;
    }

  }

  //--------------------------------------------------------------------------------------//
  // Operation of States
  //--------------------------------------------------------------------------------------//

#if 0 /*** TO BE DONE ***/
  if (tbl_erps[erps_idx].cx_active == PROT_ERPS_PORT0) {

    if ( SF[PROT_ERPS_PORT0] ) {

      // Processamento do holdoff timer
      // holdoff_timer em ms! Passam PROT_ERPS_CALL_PROC_MS ms cada vez que se entra neste ciclo
      // holdoffTimer esta em multiplos de 100ms
      tbl_erps[erps_idx].holdoff_timer_previous = tbl_erps[erps_idx].holdoff_timer;
      tbl_erps[erps_idx].holdoff_timer+=PROT_ERPS_CALL_PROC_MS;
      if (tbl_erps[erps_idx].holdoff_timer < tbl_erps[erps_idx].holdoff_timer_previous) {
        memset(&tbl_erps[erps_idx].holdoff_timer, 0xFF, sizeof(tbl_erps[erps_idx].holdoff_timer));
      }

      if (tbl_erps[erps_idx].holdoff_timer < (tbl_erps[erps_idx].protParam.holdoffTimer*100)) {

        LOG_TRACE(LOG_CTX_ERPS, "prot id=%d, SF[PROT_ERPS_PORT0], hold off timer %d", i, tbl_erps[erps_idx].holdoff_timer);

        SF[PROT_ERPS_PORT0] = 0;
      } else {
        tbl_erps[erps_idx].holdoff_timer = 0;
      }

    } else {
      tbl_erps[erps_idx].holdoff_timer = 0;
    }
  }
#endif


  // 10.1.1.  Priority logic
  //The evaluation of the top priority request is repeated every time a local request changes or an RAPS message is received.
  //Ring protection requests, commands and R-APS signals have the priorities as specified in Table 10-1.

  if ( (localRequest != LReq_NONE) && (tbl_erps[erps_idx].localRequest != localRequest) ) {
    haveChanges = L7_TRUE;
    LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d, localRequest: change from %d to %d", erps_idx, tbl_erps[erps_idx].localRequest, localRequest);
  }
  if ( (remoteRequest != RReq_NONE) && (tbl_erps[erps_idx].remoteRequest != remoteRequest) ) {
    haveChanges = L7_TRUE;
    LOG_TRACE(LOG_CTX_ERPS, "ERPS# %d, remoteRequest: change from %d to %d", erps_idx, tbl_erps[erps_idx].remoteRequest, remoteRequest);
  }
  if (!haveChanges) {
    return(PROT_ERPS_EXIT_OK);
  }


  //-------------------------------------------------------------------------
  //  Node state - The current state of the Ethernet Ring Node
  //-------------------------------------------------------------------------  

  switch (ERP_STATE_GetState(tbl_erps[erps_idx].state_machine)) {
  
  case ERPS_STATE_Freeze:
    break;

  case ERPS_STATE_Z_Init:
    break;

    //Stop guard timer
    tbl_erps[erps_idx].guard_timer                      = 0;
    tbl_erps[erps_idx].guard_CMD                        = TIMER_CMD_STOP;

    //Stop WTR timer
    tbl_erps[erps_idx].wtr_timer                        = 0;
    tbl_erps[erps_idx].wtr_CMD                          = TIMER_CMD_STOP;

    //Stop WTB timer
    tbl_erps[erps_idx].wtb_timer                        = 0;
    tbl_erps[erps_idx].wtb_CMD                          = TIMER_CMD_STOP;

    //If RPL Owner Node:
    if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
      //Block RPL port
      //Unblock non-RPL port
      if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL ) {                      // Port0 is RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
      } else {                                                                                  // Port1 is RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      }

      //Tx R-APS (NR)
      ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);

      //If revertive:
      if ( (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION) ) {
        // Start WTR timer
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_START;
      }
    }
    // Else if RPL Neighbour Node:
    else if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
      // Block RPL Port
      // Unblock non-RPL port
      if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR ) {             // Port0 is RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
      } else {                                                                                  // Port1 is RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      }

      //Tx R-APS (NR)
      ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);
    }
    //Else:
    else {
      // Block one ring port   
      // Unblock other ring port      
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      // Tx R-APS (NR)
      ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);
    }

    // Next node state: E
    ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending), __LINE__);

    break;

  case ERPS_STATE_A_Idle:
    //Clear 2
    if ( topPriorityRequest == LReq_CLEAR ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }

    //FS 3 
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_DNF, __LINE__);
        
        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }

    //R-APS (FS)  4
    if ( topPriorityRequest == RReq_FS ) {

      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }

    //local SF  5
    if ( topPriorityRequest == LReq_SF ) {

      //If failed ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {
        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block failed ring port
        ptin_erps_blockOrUnblockPort(erps_idx, req_port, ERP_PORT_FLUSHING, __LINE__);

        // Tx R-APS (SF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-failed ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }

    //local clear SF  6
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }

    //R-APS (SF)  7
    if ( topPriorityRequest == RReq_SF ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }

    //R-APS (MS)  8
    if ( topPriorityRequest == RReq_MS ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }

    //MS 9
    if ( topPriorityRequest == LReq_MS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (MS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_MS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, req_port , ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (MS)
        ptin_erps_aps_tx(erps_idx, RReq_MS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }

    //WTR Expires  10
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }

    //WTR Running  11
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }

    //WTB Expires  12
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }

    //WTB Running  13
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }

    //R-APS (NR, RB)  14
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsReqStatusRx) & RReq_STAT_RB) ) {
      //Unblock non-RPL port
      if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL ) {                   // Port0 is non-RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      }
      if ( tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL ) {                   // Port1 is non-RPL
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      }

      //If Not RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPL) ) {
        // Stop Tx R-APS  
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }

    //R-APS (NR)  15
    if ( topPriorityRequest == RReq_NR ) {
      //If neither RPL Owner Node nor RPL Neighbour Node, and remote Node ID is higher than own Node ID:
      if ( ((tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPL))                   &&
           ((tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPLNEIGHBOUR) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPLNEIGHBOUR)) && 
           (0 /* remote Node ID is higher than own Node ID */)                                                                                      ) {

        //Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        //Stop Tx R-APS
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }

    break;

  case ERPS_STATE_B_Protection:
    
    //Clear 16
    if ( topPriorityRequest == LReq_CLEAR ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }

    //FS 17
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }

    //R-APS (FS)  18
    if ( topPriorityRequest == RReq_FS ) {
      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //local SF  19
    if ( topPriorityRequest == LReq_SF ) {
      //If failed ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block failed ring port
        ptin_erps_blockOrUnblockPort(erps_idx, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (SF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-failed ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //local clear SF  20
    if ( topPriorityRequest == LReq_SFc ) {
      //Start guard timer
      tbl_erps[erps_idx].guard_CMD = TIMER_CMD_START;

      //Tx R-APS (NR)
      ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
        //Start WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_START;
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }


    //R-APS (SF)  21
    if ( topPriorityRequest == RReq_SF ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //R-APS (MS)  22
    if ( topPriorityRequest == RReq_MS ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //MS 23
    if ( topPriorityRequest == LReq_MS ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //WTR Expires  24
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //WTR Running  25
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //WTB Expires  26
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //WTB Running  27
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }

    //R-APS (NR, RB)  28
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsReqStatusRx) & RReq_STAT_RB) ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }

    //R-APS (NR)  29
    if ( topPriorityRequest == RReq_NR ) {

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                    ) {

        //Start WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_START;
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }


    break;

  case ERPS_STATE_C_ManualSwitch:

    //Clear 30
    if ( topPriorityRequest == LReq_CLEAR ) {
      //If any ring port blocked:
      if ( (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) || (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {

        // Start guard timer  
        tbl_erps[erps_idx].guard_CMD = TIMER_CMD_START;

        // Tx R-APS (NR)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);
      }

      // If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                    ) {
        // Start WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_START;
      }

      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }


    //FS 31
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }

    //R-APS (FS)  32
    if ( topPriorityRequest == RReq_FS ) {
      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);      

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //local SF  33
    if ( topPriorityRequest == LReq_SF ) {
      //If failed ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {
        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block failed ring port       
        ptin_erps_blockOrUnblockPort(erps_idx, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (SF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //local clear SF  34
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }


    //R-APS (SF)  35
    if ( topPriorityRequest == RReq_SF ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }

    //R-APS (MS)  36
    if ( topPriorityRequest == RReq_MS ) {
      //If any ring port blocked:
      if ( (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) || (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {

        // Start guard timer  
        tbl_erps[erps_idx].guard_CMD = TIMER_CMD_START;

        // Tx R-APS (NR)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);

        // If RPL Owner Node and revertive mode:
        if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
              (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
          // Start WTB
          tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_START;
        }
      }

      // Next node state: E (*)
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }

    //MS 37
    if ( topPriorityRequest == LReq_MS ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }


    //WTR Expires  38
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }

    //WTR Running  39
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }

    //WTB Expires  40
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }

    //WTB Running  41
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }

    //R-APS (NR, RB)  42
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsReqStatusRx) & RReq_STAT_RB) ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }


    //R-APS (NR)  43
    if ( topPriorityRequest == RReq_NR ) {

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                    ) {
        //Start WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_START;
      }

      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }


    break;

  case ERPS_STATE_D_ForcedSwitch:

    //Clear 44
    if ( topPriorityRequest == LReq_CLEAR ) {
      //If any ring port blocked:
      if ( (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) || (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {
        // Start guard timer  
        tbl_erps[erps_idx].guard_CMD = TIMER_CMD_START;

        // Tx R-APS (NR)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_ZEROS, __LINE__);

        // If RPL Owner Node and revertive mode:
        if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
          // Start WTB
          tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_START;
        }
      }

      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }


    //FS 45
    if ( topPriorityRequest == LReq_FS ) {
      //Block requested ring port
      ptin_erps_blockOrUnblockPort(erps_idx, req_port, ERP_PORT_BLOCKING, __LINE__);

      //Tx R-APS (FS)
      ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

      //Flush FDB
      ptin_erps_FlushFDB(erps_idx, __LINE__);

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //R-APS (FS)  46
    if ( topPriorityRequest == RReq_FS ) {
      //No action     
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }

    //local SF  47
    if ( topPriorityRequest == LReq_SF ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //local clear SF  48
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //R-APS (SF)  49
    if ( topPriorityRequest == RReq_SF ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //R-APS (MS)  50
    if ( topPriorityRequest == RReq_MS ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //MS 51
    if ( topPriorityRequest == LReq_MS ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //WTR Expires  52
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //WTR Running  53
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //WTB Expires  54
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }

    //WTB Running  55
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    } 

    //R-APS (NR, RB)  56
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsReqStatusRx) & RReq_STAT_RB) ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }

    //R-APS (NR)  57      
    if ( topPriorityRequest == RReq_NR ) {

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                  ) {
        //Start WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_START;
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }

    break;

  case ERPS_STATE_E_Pending:

    //Clear 58
    if ( topPriorityRequest == LReq_CLEAR ) {
      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;
      }
      //If RPL port is blocked:
      //-->RPL Port is port0 or port1?
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) ) {

        // Tx R-APS (NR, RB,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

        // Unblock non-RPL port
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {

        // Tx R-APS (NR, RB,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

        // Unblock non-RPL port
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);

      }
      //Else:
      else {
        //Block RPL port
        if (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
        } else if (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
        }
        
        // Tx R-APS (NR, RB)  
        ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB, __LINE__);

        // Unblock non-RPL port
        if (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
        } else if (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL) {
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
        }

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }

    //FS 59
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_DNF, __LINE__);
        
        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(erps_idx, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;
      }

      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //R-APS (FS)  60
    if ( topPriorityRequest == RReq_FS ) {
      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;
      }
           
      // Next node state: D
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_D_ForcedSwitch),__LINE__);
    }


    //local SF  61
    if ( topPriorityRequest == LReq_SF ) {
      //If failed ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
          // Block failed ring port  
          ptin_erps_blockOrUnblockPort(erps_idx, req_port, ERP_PORT_BLOCKING, __LINE__);

          // Tx R-APS (SF)
          ptin_erps_aps_tx(erps_idx, RReq_SF, RReq_STAT_ZEROS, __LINE__);

          // Unblock non-failed ring port
          ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

          // Flush FDB
          ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;
      }

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //local clear SF  62
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }


    //R-APS (SF)  63
    if ( topPriorityRequest == RReq_SF ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;
      }

      // Next node state: B
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_B_Protection),__LINE__);
    }


    //R-APS (MS)  64
    if ( topPriorityRequest == RReq_MS ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;
      }

      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }

    //MS 65
    if ( topPriorityRequest == LReq_MS ) {

      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;
      }

      //If requested ring port is already blocked:
      if (tbl_erps[erps_idx].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (MS,DNF)
        ptin_erps_aps_tx(erps_idx, RReq_MS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (MS)
        ptin_erps_aps_tx(erps_idx, RReq_MS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(erps_idx, __LINE__);
      }

      // Next node state: C
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_C_ManualSwitch),__LINE__);
    }

    //WTR Expires  66
    if ( topPriorityRequest == LReq_WTRExp ) {
      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        
        //Stop WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;
        
        //If RPL port is blocked:
        if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) ) {

          //Tx R-APS (NR, RB,DNF)   
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          //Unblock non-RPL port  
          if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL ) {                   // Port0 is non-RPL
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          }
          if ( tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL ) {                   // Port1 is non-RPL
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          }
        }
        //Else :
        else {
          // Block RPL port
          if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL ) {                       // Port0 is RPL
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
          } else if ( tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL ) {                // Port1 is RPL
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
          }

          // Tx R-APS (NR, RB)   
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB, __LINE__);

          // Unblock non-RPL port   
          if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL ) {                   // Port0 is non-RPL
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          }
          if ( tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL ) {                   // Port1 is non-RPL
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          }

          // Flush FDB
          ptin_erps_FlushFDB(erps_idx, __LINE__);
        }
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }


    //WTR Running  67
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }

    //WTB Expires  68
    if ( topPriorityRequest == LReq_WTBExp ) {
      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;

        // If RPL port is blocked:   
        if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) ) {

          // Tx R-APS (NR, RB,DNF)
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          // Unblock non-RPL port
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

        } else if ( (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {

          // Tx R-APS (NR, RB,DNF)
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          // Unblock non-RPL port
          ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);

        }
        //Else:
        else {
          //Block RPL port
          if (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
          } else if (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
          }
          
          // Tx R-APS (NR, RB)  
          ptin_erps_aps_tx(erps_idx, RReq_NR, RReq_STAT_RB, __LINE__);

          // Unblock non-RPL port
          if (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_NONRPL) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          } else if (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_NONRPL) {
            ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
          }

          // Flush FDB
          ptin_erps_FlushFDB(erps_idx, __LINE__);
        }
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }


    //WTB Running  69
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    } 

    //R-APS (NR, RB)  70
    if ( (topPriorityRequest == RReq_NR) && (APS_GET_STATUS(apsReqStatusRx) & RReq_STAT_RB) ) {
      //If RPL Owner Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[erps_idx].wtb_CMD = TIMER_CMD_STOP;
      }

      //If neither RPL Owner Node nor RPL Neighbour Node:
      if ( ((tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPL) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPL))                 &&
           ((tbl_erps[erps_idx].protParam.port0Role != ERPS_PORTROLE_RPLNEIGHBOUR) && (tbl_erps[erps_idx].protParam.port1Role != ERPS_PORTROLE_RPLNEIGHBOUR))     ) {

        //Unblock ring ports
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
        ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

        //Stop Tx R-APS
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      //If RPL Neighbour Node:
      if ( (tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) || (tbl_erps[erps_idx].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
        //Block RPL port
        //Unblock non-RPL port
        if ( tbl_erps[erps_idx].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR ) {             // Port0 is RPL
        	ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
        	ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
        } else {                                                                                  // Port1 is RPL
        	ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
        	ptin_erps_blockOrUnblockPort(erps_idx, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
        }

        //Stop Tx R-APS      
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_A_Idle),__LINE__);
    }


    //R-APS (NR)  71
    if ( topPriorityRequest == RReq_NR ) {
      //If remote Node ID is higher than own Node ID:
      if (0 /*** TO BE DONE ***/) {
        //Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(erps_idx, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Stop Tx R-APS
        ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(erps_idx,ERP_STATE_SetLocal(ERPS_STATE_E_Pending),__LINE__);
    }

    break;

  }//switch



#if 0
  if ( (tbl_erps[erps_idx].state_machine_h != tbl_erps[erps_idx].state_machine)   || 
       ( (localRequest != LReq_NONE) && (tbl_erps[erps_idx].localRequest != localRequest) )         ||
       ( (remoteRequest != RReq_NONE) && (tbl_erps[erps_idx].remoteRequest != remoteRequest) )

     ) {

    if (ERP_STATE_IsLocal(tbl_erps[erps_idx].state_machine)) {

//    LOG_TRACE(LOG_CTX_ERPS, "\n"
//              "|    State    | REQ(FP,P) |\n"
//              "   %3s:%3s:L    %3s(%d,%d) \n",
//              stateToString[ERP_STATE_STATE(tbl_erps[erps_idx].state_machine)], reqToString[localRequest],
//              reqToString[PSC_REQ(tbl_erps[erps_idx].apsRequestTx)], PSC_FPATH(tbl_erps[erps_idx].apsRequestTx), PSC_PATH(tbl_erps[erps_idx].apsRequestTx));
    } else {
//    LOG_TRACE(LOG_CTX_ERPS, "\n"
//              "|    State    | REQ(FP,P) |\n"
//              "   %3s:%3s:R    %3s(%d,%d) \n",
//              stateToString[ERP_STATE_STATE(tbl_erps[erps_idx].state_machine)], reqToString[remoteRequest],
//              reqToString[PSC_REQ(tbl_erps[erps_idx].apsRequestTx)], PSC_FPATH(tbl_erps[erps_idx].apsRequestTx), PSC_PATH(tbl_erps[erps_idx].apsRequestTx));
    }
  }
#endif


  if (localRequest != LReq_NONE) {
    tbl_erps[erps_idx].localRequest = localRequest;
  }
  if (remoteRequest != RReq_NONE) {
    tbl_erps[erps_idx].remoteRequest = remoteRequest;
  }

  tbl_erps[erps_idx].apsReqStatusRx = apsReqStatusRx;

  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0] = SF[PROT_ERPS_PORT0];
  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1] = SF[PROT_ERPS_PORT1];

  if ( (tbl_erps[erps_idx].operator_cmd & PROT_ERPS_OPCMD_OC) ) {
    tbl_erps[erps_idx].operator_cmd = PROT_ERPS_OPCMD_NR;
    tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0] = 0;
    tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1] = 0;
  }


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
  L7_uint32 erps_idx;

  for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {

    //LOG_TRACE(LOG_CTX_ERPS,"ERPS#%d; admin %d", erps_idx, tbl_erps[erps_idx].admin);

    if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
      continue;
    }

    tbl_erps[erps_idx].hal.prot_proc(erps_idx);

  }

  return(PROT_ERPS_EXIT_OK);
}




/****************************************************************************** 
 * ERPS Task Init
 ******************************************************************************/

L7_uint32 erps_TaskId = L7_ERROR;



/**
 * Task with infinite loop
 */
void ptin_erps_task(void)
{
  LOG_INFO(LOG_CTX_ERPS,"ERPS Task started");

  if (osapiTaskInitDone(L7_PTIN_ERPS_TASK_SYNC)!=L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_ERPS, "Error syncing task");
    PTIN_CRASH();
  }

  LOG_INFO(LOG_CTX_ERPS,"ERPS task ready");

  ptin_erps_init();

  /*
   * TEST
  */
#if 0
  erpsProtParam_t new_group;
  memset(&new_group, 0, sizeof(erpsProtParam_t));
  ptin_erps_add_entry( /*erps_idx*/ 1, &new_group);
#endif

  /* Infinite Loop */
  while (1) {
    ptin_prot_erps_proc();

    sleep(10);
    //sleep(PROT_ERPS_CALL_PROC_MS);
  }
}




/**
 * Initializes ERPS module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_prot_erps_init(void)
{
  /* Create task for ERProtection State Machine */
  erps_TaskId = osapiTaskCreate("ptin_prot_erps_task", ptin_erps_task, 0, 0,
                                L7_DEFAULT_STACK_SIZE,
                                L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                L7_DEFAULT_TASK_SLICE);

  if (erps_TaskId == L7_ERROR) {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Could not create task ptin_prot_erps_task");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_PTIN_CNFGR,"Task ptin_prot_erps_task created");

  if (osapiWaitForTaskInit (L7_PTIN_ERPS_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to initialize ptin_prot_erps_task()\n");
    return(L7_FAILURE);
  }
  LOG_INFO(LOG_CTX_PTIN_CNFGR,"Task ptin_prot_erps_task initialized");

  return L7_SUCCESS;
}

