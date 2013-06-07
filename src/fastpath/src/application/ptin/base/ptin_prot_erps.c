/**
 * ptin_prot_erps.c 
 *  
 * Implements ITU-T G.8032/Y.1344 Ethernet Ring Protection 
 * Switching 
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

#define __ENABLE_RREQ__

/* *******************************************************************************/
/*                                  GLOBAL VARS                                  */
/* *******************************************************************************/

tblErps_t tbl_erps[MAX_PROT_PROT_ERPS];

static const char *stateToString[] = {"FRZ", "Z_Init", "A_Idle", "B_Protection", "C_ManualSwitch", "D_ForceSwitch", "E_Pending"};
static const char *ReqToString[] = {"NR", "1", "2", "3", "4", "5", "6", "MS", "8", "9", "10", "SF", "12", "FS", "EVENT", "NONE"};

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


int ptin_prot_erps_instance_proc(L7_uint32 i);

int ptin_erps_aps_tx(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status, int line_callback);


/**
 * Inicializa as variaveis para proteccao SNCP
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx index da entrada a iniciar
 * 
 * @return int Ver definicao de: ERROR Codes
 */
int ptin_erps_init_entry(L7_uint32 erps_idx)
{
  int ret;

  LOG_TRACE(LOG_CTX_OAM, "(erps_idx %d)", erps_idx);

  if ((erps_idx>=MAX_PROT_PROT_ERPS)) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_TRACE(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].admin                            = PROT_ERPS_ENTRY_FREE;

  tbl_erps[erps_idx].protParam.ringId                 = 0;

  tbl_erps[erps_idx].OAM_Alarms[PROT_ERPS_PORT0]   = PROT_ERPS_SF_CLEAR;
  tbl_erps[erps_idx].OAM_Alarms[PROT_ERPS_PORT1]   = PROT_ERPS_SF_CLEAR;
  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]    = PROT_ERPS_SF_CLEAR;
  tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]    = PROT_ERPS_SF_CLEAR;

  tbl_erps[erps_idx].wtr_timer                        = 0;
  tbl_erps[erps_idx].wtr_CMD                          = TIMER_CMD_STOP;
  tbl_erps[erps_idx].wtb_timer                        = 0;
  tbl_erps[erps_idx].wtb_CMD                          = TIMER_CMD_STOP;
  tbl_erps[erps_idx].holdoff_timer                    = 0;
  tbl_erps[erps_idx].holdoff_timer_previous           = 0;

  tbl_erps[erps_idx].operator_cmd                     = PROT_ERPS_OPCMD_NR;
  tbl_erps[erps_idx].operator_cmd_port                = 0;

  tbl_erps[erps_idx].localRequest                     = LReq_NONE;
  tbl_erps[erps_idx].remoteRequest                    = RReq_NONE;

  tbl_erps[erps_idx].apsRequestRx                     = 0;
  tbl_erps[erps_idx].apsRequestTx                     = 0;

  tbl_erps[erps_idx].portState[PROT_ERPS_PORT0]       = ERP_PORT_FLUSHING;
  tbl_erps[erps_idx].portState[PROT_ERPS_PORT1]       = ERP_PORT_FLUSHING;

  tbl_erps[erps_idx].state_machine                    = ERP_STATE_SetLocal(ERP_STATE_0_Init);


  tbl_erps[erps_idx].hal.rd_alarms                    = rd_alarms_dummy;
  tbl_erps[erps_idx].hal.aps_rxfields                 = aps_rxdummy;
  tbl_erps[erps_idx].hal.aps_txfields                 = aps_txdummy;

  tbl_erps[erps_idx].hal.switch_path                  = switch_path_dummy;
  tbl_erps[erps_idx].hal.prot_proc                    = prot_proc_dummy;

  ret = erps_idx;
  LOG_TRACE(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao para iniciar as tabelas de Software
 * 
 * @author joaom (6/5/2013)
 * 
 * @return int Ver definicao de: ERROR Codes
 */
int ptin_erps_init(void)
{
  int i;
  int ret;

  LOG_INFO(LOG_CTX_OAM, "Init");

  //Protection group SNCP
  for (i=0; i<MAX_PROT_PROT_ERPS; i++) {
    ptin_erps_init_entry(i);
  }

  ret = PROT_ERPS_EXIT_OK;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * retorna o primeiro id livre da tabela de proteccoes
 * 
 * @author joaom (6/5/2013)
 * 
 * @return int 
 */
int ptin_erps_get_free_entry(void)
{
  int i;
  int ret;

  LOG_INFO(LOG_CTX_OAM, "");

  for (i=0; i<MAX_PROT_PROT_ERPS; i++) {
    if (tbl_erps[i].admin == PROT_ERPS_ENTRY_FREE) {
      ret = i;
      LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
      return(ret);
    }
  }

  ret = PROT_ERPS_UNAVAILABLE;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao que permite adicionar uma instancia ERP
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
  L7_uint32 idx;
  int ret;

  LOG_INFO(LOG_CTX_OAM, "(erps_idx %d, ...)", erps_idx);

  if (erps_idx >= MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  if (erps_idx < 0) {
    idx = ptin_erps_get_free_entry();

    if (idx < 0) {
      ret = PROT_ERPS_UNAVAILABLE;
      LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
      return(ret);
    }
  } else {
    idx = erps_idx;
  }

  if (tbl_erps[idx].admin == PROT_ERPS_ENTRY_BUSY) {
    ret = PROT_ERPS_INDEX_IN_USE;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }



  memcpy( &tbl_erps[idx].protParam, new_group, sizeof(erpsProtParam_t) );

  if (tbl_erps[idx].protParam.revertive == PROT_ERPS_NON_REVERTIVE_OPERATION) {
    tbl_erps[idx].protParam.waitToRestoreTimer = 0;
    tbl_erps[idx].wtr_timer = 0;
  }

  if (tbl_erps[idx].protParam.holdoffTimer == 0) {
    tbl_erps[idx].holdoff_timer = 0;
    tbl_erps[idx].holdoff_timer_previous = 0;
  }

  if (tbl_erps[idx].protParam.holdoffTimer > 100) {    // [0, 10] seconds in steps of 100 ms. --> 100*100ms = 10s
    tbl_erps[idx].protParam.holdoffTimer = 100;
  }

  if (tbl_erps[idx].protParam.waitToRestoreTimer > 12) {    // [5, 12] minutes in steps of 1 min, default: 5 min.
    tbl_erps[idx].protParam.waitToRestoreTimer = 12;
  }
  if (tbl_erps[idx].protParam.waitToRestoreTimer < 1) {    // [5, 12] minutes in steps of 1 min, default: 5 min.
    tbl_erps[idx].protParam.waitToRestoreTimer = 1;
  }


  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  tbl_erps[idx].OAM_Alarms[PROT_ERPS_PORT0]     = PROT_ERPS_SF_CLEAR;
  tbl_erps[idx].OAM_Alarms[PROT_ERPS_PORT1]     = PROT_ERPS_SF_CLEAR;
  tbl_erps[idx].status_SF[PROT_ERPS_PORT0]      = PROT_ERPS_SF_CLEAR;
  tbl_erps[idx].status_SF[PROT_ERPS_PORT1]      = PROT_ERPS_SF_CLEAR;

  tbl_erps[idx].wtr_timer       = 0;
  tbl_erps[idx].wtr_CMD   = TIMER_CMD_STOP;
  tbl_erps[idx].holdoff_timer           = 0;
  tbl_erps[idx].holdoff_timer_previous  = 0;

  tbl_erps[idx].operator_cmd = PROT_ERPS_OPCMD_NR;
  tbl_erps[idx].operator_cmd_port = 0;

  tbl_erps[idx].localRequest = LReq_NONE;
  tbl_erps[idx].remoteRequest = RReq_NONE;

  tbl_erps[idx].apsRequestRx = 0;
  tbl_erps[idx].apsRequestTx = 0;

  tbl_erps[idx].portState[PROT_ERPS_PORT0] = ERP_PORT_FLUSHING;
  tbl_erps[idx].portState[PROT_ERPS_PORT1] = ERP_PORT_FLUSHING;

  tbl_erps[idx].state_machine = ERP_STATE_SetLocal(ERP_STATE_A_Idle);

//  tbl_erps[idx].hal.rd_mep_alarms =          oam_machine_get_alarms;
//  tbl_erps[idx].hal.rd_mep_psc_alarm =       oam_machine_get_alarm_psc;
//  tbl_erps[idx].hal.rd_mep_psc_fields =      oam_machine_rx_psc;
//  tbl_erps[idx].hal.switch_path =            sncp_mpls_switch_LSP;

  tbl_erps[idx].hal.prot_proc = ptin_prot_erps_instance_proc;

  tbl_erps[idx].admin = PROT_ERPS_ENTRY_BUSY;

  ret = idx;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao para configuracao de uma instancia existente
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
  int     ret;

  LOG_INFO(LOG_CTX_OAM, "(erps_idx %d, ...) {", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin==PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].protParam.revertive =          conf->revertive;

  tbl_erps[erps_idx].protParam.holdoffTimer =           conf->holdoffTimer;
  tbl_erps[erps_idx].protParam.waitToRestoreTimer =     conf->waitToRestoreTimer;

  tbl_erps[erps_idx].protParam.continualTxInterval =    conf->continualTxInterval;
  tbl_erps[erps_idx].protParam.rapidTxInterval =        conf->rapidTxInterval;

  if (tbl_erps[erps_idx].protParam.revertive == PROT_ERPS_NON_REVERTIVE_OPERATION) {
    tbl_erps[erps_idx].protParam.waitToRestoreTimer = 0;
    tbl_erps[erps_idx].wtr_timer = 0;
    tbl_erps[erps_idx].wtr_CMD = TIMER_CMD_STOP;
  }

  if (tbl_erps[erps_idx].protParam.holdoffTimer == 0) {
    tbl_erps[erps_idx].holdoff_timer = 0;
    tbl_erps[erps_idx].holdoff_timer_previous = 0;
  }

  if (tbl_erps[erps_idx].protParam.holdoffTimer > 100) {    // [0, 10] seconds in steps of 100 ms. --> 100*100ms = 10s
    tbl_erps[erps_idx].protParam.holdoffTimer = 100;
  }

  if (tbl_erps[erps_idx].protParam.waitToRestoreTimer > 12) {    // [5, 12] minutes in steps of 1 min, default: 5 min.
    tbl_erps[erps_idx].protParam.waitToRestoreTimer = 12;
  }
  if (tbl_erps[erps_idx].protParam.waitToRestoreTimer < 1) {    // [5, 12] minutes in steps of 1 min, default: 5 min.
    tbl_erps[erps_idx].protParam.waitToRestoreTimer = 1;
  }

  ret = erps_idx;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao para remocao de uma instancia
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_remove_entry(L7_uint32 erps_idx)
{
  int ret = 0;

  LOG_INFO(LOG_CTX_OAM, "(erps_idx %d)", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    LOG_INFO(LOG_CTX_OAM, "INFO: Entry free.", ret);
  }

  ptin_erps_init_entry(erps_idx);
  ptin_erps_aps_tx(erps_idx, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

  ret = erps_idx;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Faz o clear da tabela
 * 
 * @author joaom (6/5/2013)
 * 
 * @return int 
 */
int ptin_erps_clear(void)
{
  int i, ret=PROT_ERPS_EXIT_OK;

  LOG_INFO(LOG_CTX_OAM, "");

  for (i=0; i<MAX_PROT_PROT_ERPS; i++) {
    if ((ret=ptin_erps_remove_entry(i))!=i) {
      LOG_INFO(LOG_CTX_OAM, "ERROR: (%d) while removing sncp id %d\n\r", ret, i);
      ret = PROT_ERPS_EXIT_NOK1;
    }
  }

  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao para leitura de uma instancia de prot
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
  int ret;

  LOG_INFO(LOG_CTX_OAM, "(erps_idx %d, ...)", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin==PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  memcpy(group, &tbl_erps[erps_idx].protParam, sizeof(erpsProtParam_t));

  ret = erps_idx;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao que permite limpar o force/manual switch
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_cmd_clear(L7_uint32 erps_idx)
{
  int ret;

  LOG_INFO(LOG_CTX_OAM, "(erps_idx %d)", erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  tbl_erps[erps_idx].operator_cmd       = PROT_ERPS_OPCMD_OC;
  tbl_erps[erps_idx].operator_cmd_port  = 0;  

  ret = erps_idx;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao que permite forcar o switch_path
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
  int ret;

  LOG_INFO(LOG_CTX_OAM, "(erps_idx %d, switch_path %d)", erps_idx, switch_path);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

//  if ((switch_path == SWITCH_TO_P) && (tbl_erps[erps_idx].lockout_switching == 1)) {
//      ret = PROT_ERPS_EXIT_NOK1;
//      LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
//      return (ret);
//  }

//  tbl_erps[erps_idx].manual_switching =   SWITCH_DISABLED;
//  tbl_erps[erps_idx].force_switching =    switch_path;
//  tbl_erps[erps_idx].state_machine =      SNCP_SM_CMD;

//  if ( (tbl_erps[erps_idx].operator_cmd != PROT_ERPS_OPCMD_OC) && (tbl_erps[erps_idx].operator_cmd != PROT_ERPS_OPCMD_LO) ) {
//      tbl_erps[erps_idx].operator_cmd = PROT_ERPS_OPCMD_FS;
//      tbl_erps[erps_idx].operator_path_switching = switch_path;
//  }

  ret = erps_idx;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao que permite forcar o switch_path se, e so se, o caminho alternativo estiver OK
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
  int ret;

  LOG_INFO(LOG_CTX_OAM, "(erps_idx %d, switch_path %d)", erps_idx, switch_path);


  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

//  if ((switch_path == SWITCH_TO_P) && (tbl_erps[erps_idx].lockout_switching == 1)) {
//      ret = PROT_ERPS_EXIT_NOK1;
//      LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
//      return (ret);
//  }
//
//  if (tbl_erps[erps_idx].force_switching != SWITCH_DISABLED) {
//      ret = G8131_EXIT_NOK2;
//      LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
//      return (ret);
//  }

//  if ( (tbl_erps[erps_idx].operator_cmd != PROT_ERPS_OPCMD_OC) && (tbl_erps[erps_idx].operator_cmd != PROT_ERPS_OPCMD_LO) && (tbl_erps[erps_idx].operator_cmd != PROT_ERPS_OPCMD_FS) ) {
//      tbl_erps[erps_idx].operator_cmd = PROT_ERPS_OPCMD_MS;
//      tbl_erps[erps_idx].operator_path_switching = switch_path;
//  }

  ret = erps_idx;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funaco para leitura de estado/alarmistica
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
  int ret;

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    //  LOG_INFO(LOG_CTX_START_UP, "ret:%d, done.", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin==PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    //  LOG_INFO(LOG_CTX_START_UP, "ret:%d, done.", ret);
    return(ret);
  }

  //Variaveis auxiliares para processamento interno
//  status->cx_active =             tbl_erps[erps_idx].cx_active;
//  status->status_SF_W =           tbl_erps[erps_idx].OAM_Alarms[PROT_ERPS_PORT0];
//  status->status_SF_P =           tbl_erps[erps_idx].OAM_Alarms[PROT_ERPS_PORT1];
//  status->wait2restoreTimer =     (tbl_erps[erps_idx].wtr_timer / 1000);
//  status->holdoff_timer =        (tbl_erps[erps_idx].holdoff_timer / 1000);
//  status->pscRequestRx =          PSC_REQ(tbl_erps[erps_idx].apsRequestRx);
//  status->pscRequestTx =          PSC_REQ(tbl_erps[erps_idx].apsRequestTx);
//  status->fPathPathRx =           (PSC_FPATH(tbl_erps[erps_idx].apsRequestRx) << 8) | PSC_PATH(tbl_erps[erps_idx].apsRequestRx);
//  status->fPathPathTx =           (PSC_FPATH(tbl_erps[erps_idx].apsRequestTx) << 8) | PSC_PATH(tbl_erps[erps_idx].apsRequestTx);
//  status->modeMismatchesCounter = tbl_erps[erps_idx].modeMismatchesCounter;
//
//  LOG_TRACE(LOG_CTX_OAM, "Rx: REQ 0x%.2x, FPATH 0x%.4x, PATH 0x%.4x",
//            PSC_REQ(tbl_erps[erps_idx].apsRequestRx), PSC_FPATH(tbl_erps[erps_idx].apsRequestRx), PSC_PATH(tbl_erps[erps_idx].apsRequestRx));
//  LOG_TRACE(LOG_CTX_OAM, "Tx: REQ 0x%.2x, FPATH 0x%.4x, PATH 0x%.4x",
//            PSC_REQ(tbl_erps[erps_idx].apsRequestTx), PSC_FPATH(tbl_erps[erps_idx].apsRequestTx), PSC_PATH(tbl_erps[erps_idx].apsRequestTx));
//
  ret = erps_idx;
  //  LOG_INFO(LOG_CTX_START_UP, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao para debug: leitura de um grupo de proteccao
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_rd_entry(L7_uint32 erps_idx)
{
  int     ret;

//  char *strArchitecture[] = {"1+1", "1:1"};
//  char *strSwitchingType[] = {"Unidirectional", "Bidirectional"};
  char *strrevertive[] = {"Non-Revertive", "Revertive"};
//char *strcx_active[] = {"WORKING", "PROTECTION"};
  char *strCmd[] = {"NR", "OC", "LO", "--", "FS", "--", "--", "--", "MS"};
//  char *strCmdPath[] = {"NONE", "Switch to W", "Switch to P"};
//  char *strCmdWTR[] = {"STOP", "START"};

  printf("%s(erps_idx %d)",__FUNCTION__, erps_idx);

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    printf("ERROR: %d", ret);
    return(ret);
  }

  if (tbl_erps[erps_idx].admin == PROT_ERPS_ENTRY_FREE) {
    ret = PROT_ERPS_UNAVAILABLE;
    printf("ERROR: %d", ret);
    return(ret);
  }

  printf("\n-----------------------------------------");
  printf("\n  admin               %d",                     tbl_erps[erps_idx].admin);
  printf("\n-----------------------------------------");
  printf("\nerpsProtParam_t:\n");
//  printf("\n  W_slot              %d",                     tbl_erps[erps_idx].protParam.W_slot);
//  printf("\n  P_slot              %d",                     tbl_erps[erps_idx].protParam.P_slot);
//  printf("\n  mepLSP_W_idx        %d",                     tbl_erps[erps_idx].protParam.mepLSP_W_idx);
//  printf("\n  mepLSP_P_idx        %d",                     tbl_erps[erps_idx].protParam.mepLSP_P_idx);
//  printf("\n  Architecture        %s",                     strArchitecture[tbl_erps[erps_idx].protParam.Architecture]);
//  printf("\n  SwitchingType       %s",                     strSwitchingType[tbl_erps[erps_idx].protParam.SwitchingType]);
  printf("\n  revertive       %s",                     strrevertive[tbl_erps[erps_idx].protParam.revertive]);
  printf("\n  holdoffTimer        %d (x100 ms)",           tbl_erps[erps_idx].protParam.holdoffTimer);
  printf("\n  waitToRestoreTimer  %d (x1 min)",            tbl_erps[erps_idx].protParam.waitToRestoreTimer);
  printf("\n-----------------------------------------");
  printf("\nStates:\n");
//printf("\n  cx_active           %s",                     strcx_active[tbl_erps[erps_idx].cx_active]);
//printf("\n  cx_active_h         %s",                     strcx_active[tbl_erps[erps_idx].cx_active_h]);
  printf("\n  OAM_Alarms_w        0x%.2x (mel 0x20, mmg 0x10, unm 0x08, unp 0x04, loc 0x02, rdi 0x01)", 
         tbl_erps[erps_idx].OAM_Alarms[PROT_ERPS_PORT0]);
  printf("\n  OAM_Alarms_p        0x%.2x",                 tbl_erps[erps_idx].OAM_Alarms[PROT_ERPS_PORT1]);
  printf("\n  status_SF_W         %d",                     tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT0]);
  printf("\n  status_SF_P         %d",                     tbl_erps[erps_idx].status_SF[PROT_ERPS_PORT1]);
//  printf("\n  wtr                 %d",                     tbl_erps[erps_idx].wtr_timer);
//  printf("\n  wtr_cmd             %s",                     strCmdWTR[tbl_erps[erps_idx].wtr_CMD]);
  printf("\n  holdoff_timer      %d",                     tbl_erps[erps_idx].holdoff_timer);
  printf("\n  operator_cmd        (0x%x) %s",              tbl_erps[erps_idx].operator_cmd, strCmd[tbl_erps[erps_idx].operator_cmd]);
//  printf("\n  operator_path       (%d)   %s",              tbl_erps[erps_idx].operator_path_switching, strCmdPath[tbl_erps[erps_idx].operator_path_switching]);
  printf("\n-----------------------------------------");
  printf("\nPSC State Machine:\n");
  printf("\n  localRequest        %s",                     ReqToString[tbl_erps[erps_idx].localRequest]);
  printf("\n  remoteRequest       %s",                     ReqToString[tbl_erps[erps_idx].remoteRequest]);
//  printf("\n  pscRequestRx        (0x%x) %s(%d,%d)",
//               tbl_erps[erps_idx].apsRequestRx, ReqToString[PSC_REQ(tbl_erps[erps_idx].apsRequestRx)],
//               PSC_FPATH(tbl_erps[erps_idx].apsRequestRx), PSC_PATH(tbl_erps[erps_idx].apsRequestRx));
//  printf("\n  pscRequestTx        (0x%x) %s(%d,%d)",
//               tbl_erps[erps_idx].apsRequestTx, ReqToString[PSC_REQ(tbl_erps[erps_idx].apsRequestTx)],
//               PSC_FPATH(tbl_erps[erps_idx].apsRequestTx), PSC_PATH(tbl_erps[erps_idx].apsRequestTx));
//  printf("\n  modeMismatchesCntr  %d",                     tbl_erps[erps_idx].modeMismatchesCounter);
  printf("\n  psc_state_machine   (0x%x) %s:%s",
         tbl_erps[erps_idx].state_machine,
         stateToString[ERP_STATE_GetState(tbl_erps[erps_idx].state_machine)], ERP_STATE_IsLocal(tbl_erps[erps_idx].state_machine)? "L":"R");
//  printf("\n  psc_state_machine_h (0x%x) %s:%s",
//               tbl_erps[erps_idx].state_machine_h,
//               pscStateToString[ERP_STATE_STATE(tbl_erps[erps_idx].state_machine_h)], ERP_STATE_IsLocal(tbl_erps[erps_idx].state_machine_h)? "L":"R");
  printf("\n  waitingstates        %d",  tbl_erps[erps_idx].waitingstates);

  printf("\n-----------------------------------------\n\r");

  ret = erps_idx;
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
  int ret;

  LOG_INFO(LOG_CTX_OAM, "erps_idx %d: port %d, portState %d (line_callback %d)", erps_idx, port, portState, line_callback);

  tbl_erps[erps_idx].portState[port] = portState;

  tbl_erps[erps_idx].hal.switch_path(erps_idx, port, 0);

  //reg_alarm(0, erps_idx, path);  -> Must be done inside hal.switch_path() function

  ret = PROT_ERPS_EXIT_OK;
  LOG_INFO(LOG_CTX_OAM, "ret:%d, done.", ret);
  return(ret);
}


/**
 * Funcao para leitura de alarmes
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

  LOG_TRACE(LOG_CTX_OAM,"(erps_idx %d, path %d)", erps_idx, path);

  tbl_erps[erps_idx].hal.rd_alarms(0,0);

  // OAM Indication
  tbl_erps[erps_idx].OAM_Alarms[PROT_ERPS_PORT0] = 0;
  tbl_erps[erps_idx].OAM_Alarms[PROT_ERPS_PORT1] = 0;

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

  LOG_TRACE(LOG_CTX_OAM, "erps_idx %d: %d(%d) (line_callback %d)", erps_idx, req_state, status, line_callback);

  tbl_erps[erps_idx].hal.aps_txfields(0, req_state, status);

  return(ret);
}


/**
 * APS messages reception
 * 
 * @author joaom (6/5/2013)
 * 
 * @param erps_idx 
 * 
 * @return int 
 */
int ptin_erps_aps_rx(L7_uint32 erps_idx, L7_uint8 *req_state, L7_uint8 *status)
{
  int ret = PROT_ERPS_EXIT_OK;

  tbl_erps[erps_idx].hal.aps_rxfields(0, 0, status);

  return(ret);
}


/**
 * Force FSM new state
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

  LOG_TRACE(LOG_CTX_OAM, "erps_idx %d: Changing state from %d to %d (line_callback %d)", erps_idx, tbl_erps[erps_idx].state_machine, state, line_callback);

  tbl_erps[erps_idx].state_machine = state;

  return(PROT_ERPS_EXIT_OK);
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
  int ret;

  if (erps_idx>=MAX_PROT_PROT_ERPS) {
    ret = PROT_ERPS_INDEX_VIOLATION;
    LOG_TRACE(LOG_CTX_OAM, "ret:%d, done.", ret);
    return(ret);
  }

  if (req_state > 16) {
    LOG_TRACE(LOG_CTX_OAM, "erps_idx ID %d: Invalid Req", erps_idx);
  }

  LOG_TRACE(LOG_CTX_OAM, "erps_idx ID %d: Request Tx = %s(%d)", erps_idx, ReqToString[req_state], status);

  ptin_erps_aps_tx(erps_idx, req_state, status, __LINE__);

  ret = erps_idx;
  LOG_TRACE(LOG_CTX_OAM, "ret:%d, done.", ret);
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

  LOG_TRACE(LOG_CTX_OAM, "erps_idx %d: Flushing FDB (line_callback %d)", erps_idx, line_callback);

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
int ptin_prot_erps_instance_proc(L7_uint32 i)
{
  L7_uint8  localRequest;
  L7_uint8  remoteRequest;
  L7_uint8  topPriorityRequest;     //The current top priority request as defined in sub-clause 10.1.1.
  L7_BOOL   haveChanges = L7_FALSE;

  L7_uint8  SF[2];
  L7_uint8  apsStatusRx;
  L7_uint8  req_port;               // Request or Failed Port

  topPriorityRequest =  LReq_NONE;
  localRequest =        LReq_NONE;
  remoteRequest =       RReq_NONE;
  apsStatusRx =         0;
  req_port =            0;

  LOG_TRACE(LOG_CTX_OAM,"ERPS#%d; admin %d", i, tbl_erps[i].admin);

  if (tbl_erps[i].waitingstates) {
    tbl_erps[i].waitingstates--;
    return(i);
  }


  //--------------------------------------------------------------------------------------------
  // Alarm Indication (SF)
  SF[PROT_ERPS_PORT0] = ptin_erps_rd_alarms(i, PROT_ERPS_PORT0);
  SF[PROT_ERPS_PORT1] = ptin_erps_rd_alarms(i, PROT_ERPS_PORT1);


  //--------------------------------------------------------------------------------------------
  // R-APS request
  ptin_erps_aps_rx(i, &remoteRequest, &apsStatusRx);
  
  LOG_TRACE(LOG_CTX_OAM, "prot id=%d, Received Remote APS Request(0x%x) = %s(%d, %d)", i, remoteRequest,
            ReqToString[remoteRequest], apsStatusRx);


  //--------------------------------------------------------------------------------------------
  // 10.1.9.  Local Priority Logic
  //--------------------------------------------------------------------------------------------
  //Local priority logic evaluates the local operator commands (in ETH_C_MI_RAPS_ExtCMD)
  //according to the current top priority request. The commands Clear, Manual Switch and Forced
  //Switch from the operator, are forwarded to the Priority Logic.

  //--------------------------------------------------------------------------------------------
  // Operator Command
  if ( tbl_erps[i].operator_cmd ) {

    if ( (tbl_erps[i].operator_cmd & PROT_ERPS_OPCMD_OC) ) {
      //10.1.9. Local Priority Logic
      //The Clear command is only valid if:
      //a) a local Forced Switch or Manual Switch command is in effect (Clear operation a) described in clause 8), or
      //b) a local Ethernet Ring Node is an RPL Owner Node and top priority request is neither R-APS (FS) nor R-APS (MS) (Clear operation  b) or  c) described in clause 8).

      // *** Verification of a) is done when command is received ***

      /// TO BE DONE


      // *** Verification of b) ***

      /// TO BE DONE


      localRequest = LReq_CLEAR;
      req_port = tbl_erps[i].operator_cmd_port;
    }

    else if ( (tbl_erps[i].operator_cmd & PROT_ERPS_OPCMD_FS) ) {
      localRequest = LReq_FS;
      req_port = tbl_erps[i].operator_cmd_port;
    }

    else if ( (tbl_erps[i].operator_cmd & PROT_ERPS_OPCMD_MS) ) {
      localRequest = LReq_MS;
      req_port = tbl_erps[i].operator_cmd_port;
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

    if ( (SF[PROT_ERPS_PORT0] != tbl_erps[i].status_SF[PROT_ERPS_PORT0]) ) {
      if ( SF[PROT_ERPS_PORT0] ) {
        localRequest = LReq_SF;
        req_port = PROT_ERPS_PORT0;
        LOG_TRACE(LOG_CTX_OAM, "prot id=%d, SF[PROT_ERPS_PORT0]", i);
      } else {
        localRequest = LReq_SFc;
        req_port = PROT_ERPS_PORT0;
        LOG_TRACE(LOG_CTX_OAM, "prot id=%d, SF[PROT_ERPS_PORT0] Clear", i);
      }
    }

    if ( (SF[PROT_ERPS_PORT1] != tbl_erps[i].status_SF[PROT_ERPS_PORT1]) && (localRequest != LReq_SF) ) {
      if ( SF[PROT_ERPS_PORT1] ) {
        localRequest = LReq_SF;
        req_port = PROT_ERPS_PORT1;
        LOG_TRACE(LOG_CTX_OAM, "prot id=%d, SF[PROT_ERPS_PORT1]", i);
      } else {
        localRequest = LReq_SFc;
        req_port = PROT_ERPS_PORT1;
        LOG_TRACE(LOG_CTX_OAM, "prot id=%d, SF[PROT_ERPS_PORT1] Clear", i);
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
    else if ( (tbl_erps[i].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION) &&
              (tbl_erps[i].wtr_CMD == TIMER_CMD_START)                                ) {    

      // WaitToRestore em multiplos de 1min
      // wait_restore_timer em ms! Passam 10ms cada vez que se entra neste ciclo

      // Wait_restore_timer/60000 converter para ms!

      tbl_erps[i].wtr_timer+=PROT_ERPS_CALL_PROC_MS;
      if ( (tbl_erps[i].wtr_timer/60000) >= tbl_erps[i].protParam.waitToRestoreTimer ) {

        localRequest = LReq_WTRExp;
        LOG_TRACE(LOG_CTX_OAM, "prot id=%d, WTRExp", i);

        tbl_erps[i].wtr_timer = 0;          // Reset timer value
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP; // Stop timer

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
    else if ( (remoteRequest == RReq_NR) && ( apsStatusRx & RReq_STAT_RB) ) {
      /// TO BE DONE
      topPriorityRequest = RReq_NR;
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

#if 0 /// TO BE DONE
  if (tbl_erps[i].cx_active == PROT_ERPS_PORT0) {

    if ( SF[PROT_ERPS_PORT0] ) {

      // Processamento do holdoff timer
      // holdoff_timer em ms! Passam PROT_ERPS_CALL_PROC_MS ms cada vez que se entra neste ciclo
      // holdoffTimer esta em multiplos de 100ms
      tbl_erps[i].holdoff_timer_previous = tbl_erps[i].holdoff_timer;
      tbl_erps[i].holdoff_timer+=PROT_ERPS_CALL_PROC_MS;
      if (tbl_erps[i].holdoff_timer < tbl_erps[i].holdoff_timer_previous) {
        memset(&tbl_erps[i].holdoff_timer, 0xFF, sizeof(tbl_erps[i].holdoff_timer));
      }

      if (tbl_erps[i].holdoff_timer < (tbl_erps[i].protParam.holdoffTimer*100)) {

        LOG_TRACE(LOG_CTX_OAM, "prot id=%d, SF[PROT_ERPS_PORT0], hold off timer %d", i, tbl_erps[i].holdoff_timer);

        SF[PROT_ERPS_PORT0] = 0;
      } else {
        tbl_erps[i].holdoff_timer = 0;
      }

    } else {
      tbl_erps[i].holdoff_timer = 0;
    }
  }
#endif


  // 10.1.1.  Priority logic
  //The evaluation of the top priority request is repeated every time a local request changes or an RAPS message is received.
  //Ring protection requests, commands and R-APS signals have the priorities as specified in Table 10-1.

  if ( (localRequest != LReq_NONE) && (tbl_erps[i].localRequest != localRequest) ) {
    haveChanges = L7_TRUE;
    LOG_TRACE(LOG_CTX_OAM, "prot id=%d, localRequest: change from %d to %d", i, tbl_erps[i].localRequest, localRequest);
  }
  if ( (remoteRequest != RReq_NONE) && (tbl_erps[i].remoteRequest != remoteRequest) ) {
    haveChanges = L7_TRUE;
    LOG_TRACE(LOG_CTX_OAM, "prot id=%d, remoteRequest: change from %d to %d", i, tbl_erps[i].remoteRequest, remoteRequest);
  }
  if (!haveChanges) {
    return(PROT_ERPS_EXIT_OK);
  }


  //-------------------------------------------------------------------------
  //  Node state - The current state of the Ethernet Ring Node
  //-------------------------------------------------------------------------

  tbl_erps[i].state_machine_h = tbl_erps[i].state_machine;

  switch (ERP_STATE_GetState(tbl_erps[i].state_machine)) {
  
  case ERP_STATE_Freeze:
    break;

  case ERP_STATE_0_Init:
    break;

    //Stop guard timer
    tbl_erps[i].guard_timer                      = 0;
    tbl_erps[i].guard_CMD                        = TIMER_CMD_STOP;

    //Stop WTR timer
    tbl_erps[i].wtr_timer                        = 0;
    tbl_erps[i].wtr_CMD                          = TIMER_CMD_STOP;

    //Stop WTB timer
    tbl_erps[i].wtb_timer                        = 0;
    tbl_erps[i].wtb_CMD                          = TIMER_CMD_STOP;

    //If RPL Owner Node:
    if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
      //Block RPL port
      //Unblock non-RPL port
      if ( tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL ) {                       // Port0 is RPL
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
      } else {                                                                            // Port1 is RPL
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      }

      //Tx R-APS (NR)
      ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_ZEROS, __LINE__);

      //If revertive:
      if ( (tbl_erps[i].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION) ) {
        // Start WTR timer
        tbl_erps[i].wtr_CMD = TIMER_CMD_START;
      }
    }
    // Else if RPL Neighbour Node:
    else if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
      // Block RPL Port
      // Unblock non-RPL port
      if ( tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR ) {              // Port0 is RPL
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
      } else {                                                                            // Port1 is RPL
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      }

      //Tx R-APS (NR)
      ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_ZEROS, __LINE__);
    }
    //Else:
    else {
      // Block one ring port   
      // Unblock other ring port      
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      // Tx R-APS (NR)
      ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_ZEROS, __LINE__);
    }

    // Next node state: E
    ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending), __LINE__);

    break;

  case ERP_STATE_A_Idle:
    //Clear 2
    if ( topPriorityRequest == LReq_CLEAR ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }

    //FS 3 
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(i, RReq_FS, RReq_STAT_DNF, __LINE__);
        
        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port
        ptin_erps_blockOrUnblockPort(i, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(i, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }

    //R-APS (FS)  4
    if ( topPriorityRequest == RReq_FS ) {

      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }

    //local SF  5
    if ( topPriorityRequest == LReq_SF ) {

      //If failed ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {
        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(i, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block failed ring port
        ptin_erps_blockOrUnblockPort(i, req_port, ERP_PORT_FLUSHING, __LINE__);

        // Tx R-APS (SF)
        ptin_erps_aps_tx(i, RReq_SF, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-failed ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }

    //local clear SF  6
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }

    //R-APS (SF)  7
    if ( topPriorityRequest == RReq_SF ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }

    //R-APS (MS)  8
    if ( topPriorityRequest == RReq_MS ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }

    //MS 9
    if ( topPriorityRequest == LReq_MS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (MS,DNF)
        ptin_erps_aps_tx(i, RReq_MS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port
        ptin_erps_blockOrUnblockPort(i, req_port , ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (MS)
        ptin_erps_aps_tx(i, RReq_MS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }

    //WTR Expires  10
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }

    //WTR Running  11
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }

    //WTB Expires  12
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }

    //WTB Running  13
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }

    //R-APS (NR, RB)  14
    if ( (topPriorityRequest == RReq_NR) && (apsStatusRx & RReq_STAT_RB) ) {
      //Unblock non-RPL port
      if ( tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_NONRPL ) {                   // Port0 is non-RPL
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      }
      if ( tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_NONRPL ) {                   // Port1 is non-RPL
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      }

      //If Not RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role != ERPS_PORTROLE_RPL) && (tbl_erps[i].protParam.port1Role != ERPS_PORTROLE_RPL) ) {
        // Stop Tx R-APS  
        ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }

    //R-APS (NR)  15
    if ( topPriorityRequest == RReq_NR ) {
      //If neither RPL Owner Node nor RPL Neighbour Node, and remote Node ID is higher than own Node ID:
      if ( ((tbl_erps[i].protParam.port0Role != ERPS_PORTROLE_RPL) && (tbl_erps[i].protParam.port1Role != ERPS_PORTROLE_RPL))                   &&
           ((tbl_erps[i].protParam.port0Role != ERPS_PORTROLE_RPLNEIGHBOUR) && (tbl_erps[i].protParam.port1Role != ERPS_PORTROLE_RPLNEIGHBOUR)) && 
           (0 /* remote Node ID is higher than own Node ID */)                                                                                      ) {

        //Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        //Stop Tx R-APS
        ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }

    break;

  case ERP_STATE_B_Protection:
    
    //Clear 16
    if ( topPriorityRequest == LReq_CLEAR ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }

    //FS 17
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(i, RReq_FS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(i, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(i, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }

    //R-APS (FS)  18
    if ( topPriorityRequest == RReq_FS ) {
      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //local SF  19
    if ( topPriorityRequest == LReq_SF ) {
      //If failed ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(i, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block failed ring port
        ptin_erps_blockOrUnblockPort(i, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (SF)
        ptin_erps_aps_tx(i, RReq_SF, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-failed ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //local clear SF  20
    if ( topPriorityRequest == LReq_SFc ) {
      //Start guard timer
      tbl_erps[i].guard_CMD = TIMER_CMD_START;

      //Tx R-APS (NR)
      ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[i].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
        //Start WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_START;
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }


    //R-APS (SF)  21
    if ( topPriorityRequest == RReq_SF ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //R-APS (MS)  22
    if ( topPriorityRequest == RReq_MS ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //MS 23
    if ( topPriorityRequest == LReq_MS ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //WTR Expires  24
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //WTR Running  25
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //WTB Expires  26
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //WTB Running  27
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }

    //R-APS (NR, RB)  28
    if ( (topPriorityRequest == RReq_NR) && (apsStatusRx & RReq_STAT_RB) ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }

    //R-APS (NR)  29
    if ( topPriorityRequest == RReq_NR ) {

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[i].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {

        //Start WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_START;
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }


    break;

  case ERP_STATE_C_ManualSwitch:

    //Clear 30
    if ( topPriorityRequest == LReq_CLEAR ) {
      //If any ring port blocked:
      if ( (tbl_erps[i].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) || (tbl_erps[i].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {

        // Start guard timer  
        tbl_erps[i].guard_CMD = TIMER_CMD_START;

        // Tx R-APS (NR)
        ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_ZEROS, __LINE__);
      }

      // If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[i].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
        // Start WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_START;
      }

      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }


    //FS 31
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(i, RReq_FS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(i, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }

    //R-APS (FS)  32
    if ( topPriorityRequest == RReq_FS ) {
      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);      

      //Stop Tx R-APS
      ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //local SF  33
    if ( topPriorityRequest == LReq_SF ) {
      //If failed ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {
        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(i, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block failed ring port       
        ptin_erps_blockOrUnblockPort(i, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (SF)
        ptin_erps_aps_tx(i, RReq_SF, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //local clear SF  34
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }


    //R-APS (SF)  35
    if ( topPriorityRequest == RReq_SF ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }

    //R-APS (MS)  36
    if ( topPriorityRequest == RReq_MS ) {
      //If any ring port blocked:
      if ( (tbl_erps[i].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) || (tbl_erps[i].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {

        // Start guard timer  
        tbl_erps[i].guard_CMD = TIMER_CMD_START;

        // Tx R-APS (NR)
        ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_ZEROS, __LINE__);

        // If RPL Owner Node and revertive mode:
        if ( ((tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
              (tbl_erps[i].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
          // Start WTB
          tbl_erps[i].wtb_CMD = TIMER_CMD_START;
        }
      }

      // Next node state: E (*)
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }

    //MS 37
    if ( topPriorityRequest == LReq_MS ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }


    //WTR Expires  38
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }

    //WTR Running  39
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }

    //WTB Expires  40
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }

    //WTB Running  41
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }

    //R-APS (NR, RB)  42
    if ( (topPriorityRequest == RReq_NR) && (apsStatusRx & RReq_STAT_RB) ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }


    //R-APS (NR)  43
    if ( topPriorityRequest == RReq_NR ) {

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[i].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
        //Start WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_START;
      }

      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }


    break;

  case ERP_STATE_D_ForcedSwitch:

    //Clear 44
    if ( topPriorityRequest == LReq_CLEAR ) {
      //If any ring port blocked:
      if ( (tbl_erps[i].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) || (tbl_erps[i].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {
        // Start guard timer  
        tbl_erps[i].guard_CMD = TIMER_CMD_START;

        // Tx R-APS (NR)
        ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_ZEROS, __LINE__);

        // If RPL Owner Node and revertive mode:
        if ( ((tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[i].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
          // Start WTB
          tbl_erps[i].wtb_CMD = TIMER_CMD_START;
        }
      }

      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }


    //FS 45
    if ( topPriorityRequest == LReq_FS ) {
      //Block requested ring port
      ptin_erps_blockOrUnblockPort(i, req_port, ERP_PORT_BLOCKING, __LINE__);

      //Tx R-APS (FS)
      ptin_erps_aps_tx(i, RReq_FS, RReq_STAT_ZEROS, __LINE__);

      //Flush FDB
      ptin_erps_FlushFDB(i, __LINE__);

      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //R-APS (FS)  46
    if ( topPriorityRequest == RReq_FS ) {
      //No action     
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }

    //local SF  47
    if ( topPriorityRequest == LReq_SF ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //local clear SF  48
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //R-APS (SF)  49
    if ( topPriorityRequest == RReq_SF ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //R-APS (MS)  50
    if ( topPriorityRequest == RReq_MS ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //MS 51
    if ( topPriorityRequest == LReq_MS ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //WTR Expires  52
    if ( topPriorityRequest == LReq_WTRExp ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //WTR Running  53
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //WTB Expires  54
    if ( topPriorityRequest == LReq_WTBExp ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }

    //WTB Running  55
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    } 

    //R-APS (NR, RB)  56
    if ( (topPriorityRequest == RReq_NR) && (apsStatusRx & RReq_STAT_RB) ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }

    //R-APS (NR)  57      
    if ( topPriorityRequest == RReq_NR ) {

      //If RPL Owner Node and revertive mode:
      if ( ((tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL)) &&
            (tbl_erps[i].protParam.revertive == PROT_ERPS_REVERTIVE_OPERATION)                                                                ) {
        //Start WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_START;
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }

    break;

  case ERP_STATE_E_Pending:

    //Clear 58
    if ( topPriorityRequest == LReq_CLEAR ) {
      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_STOP;
      }
      //If RPL port is blocked:
      //-->RPL Port is port0 or port1?
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) && (tbl_erps[i].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) ) {

        // Tx R-APS (NR, RB,DNF)
        ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

        // Unblock non-RPL port
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      } else if ( (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) && (tbl_erps[i].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {

        // Tx R-APS (NR, RB,DNF)
        ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

        // Unblock non-RPL port
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);

      }
      //Else:
      else {
        //Block RPL port
        if (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) {
          ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
        } else if (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) {
          ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
        }
        
        // Tx R-APS (NR, RB)  
        ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_RB, __LINE__);

        // Unblock non-RPL port
        if (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_NONRPL) {
          ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
        } else if (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_NONRPL) {
          ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
        }

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }

    //FS 59
    if ( topPriorityRequest == LReq_FS ) {
      //If requested ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (FS,DNF)
        ptin_erps_aps_tx(i, RReq_FS, RReq_STAT_DNF, __LINE__);
        
        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(i, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (FS)
        ptin_erps_aps_tx(i, RReq_FS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_STOP;
      }

      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //R-APS (FS)  60
    if ( topPriorityRequest == RReq_FS ) {
      //Unblock ring ports
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
      ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_STOP;
      }
           
      // Next node state: D
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_D_ForcedSwitch),__LINE__);
    }


    //local SF  61
    if ( topPriorityRequest == LReq_SF ) {
      //If failed ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (SF,DNF)
        ptin_erps_aps_tx(i, RReq_SF, RReq_STAT_DNF, __LINE__);

        // Unblock non-failed ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
          // Block failed ring port  
          ptin_erps_blockOrUnblockPort(i, req_port, ERP_PORT_BLOCKING, __LINE__);

          // Tx R-APS (SF)
          ptin_erps_aps_tx(i, RReq_SF, RReq_STAT_ZEROS, __LINE__);

          // Unblock non-failed ring port
          ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

          // Flush FDB
          ptin_erps_FlushFDB(i, __LINE__);
      }

      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_STOP;
      }

      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //local clear SF  62
    if ( topPriorityRequest == LReq_SFc ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }


    //R-APS (SF)  63
    if ( topPriorityRequest == RReq_SF ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_STOP;
      }

      // Next node state: B
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_B_Protection),__LINE__);
    }


    //R-APS (MS)  64
    if ( topPriorityRequest == RReq_MS ) {
      //Unblock non-failed ring port
      ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

      //Stop Tx R-APS
      ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);

      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_STOP;
      }

      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }

    //MS 65
    if ( topPriorityRequest == LReq_MS ) {

      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_STOP;
      }

      //If requested ring port is already blocked:
      if (tbl_erps[i].portState[req_port] == ERP_PORT_BLOCKING) {

        //Tx R-APS (MS,DNF)
        ptin_erps_aps_tx(i, RReq_MS, RReq_STAT_DNF, __LINE__);

        // Unblock non-requested ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);
      }
      // Else:
      else {
        // Block requested ring port  
        ptin_erps_blockOrUnblockPort(i, req_port, ERP_PORT_BLOCKING, __LINE__);

        // Tx R-APS (MS)
        ptin_erps_aps_tx(i, RReq_MS, RReq_STAT_ZEROS, __LINE__);

        // Unblock non-requested ring port  
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Flush FDB
        ptin_erps_FlushFDB(i, __LINE__);
      }

      // Next node state: C
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_C_ManualSwitch),__LINE__);
    }

    //WTR Expires  66
    if ( topPriorityRequest == LReq_WTRExp ) {
      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        
        //Stop WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_STOP;
        
        //If RPL port is blocked:
        if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) && (tbl_erps[i].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) ) {

          //Tx R-APS (NR, RB,DNF)   
          ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          //Unblock non-RPL port  
          if ( tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_NONRPL ) {                   // Port0 is non-RPL
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          }
          if ( tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_NONRPL ) {                   // Port1 is non-RPL
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          }
        }
        //Else :
        else {
          // Block RPL port
          if ( tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL ) {                       // Port0 is RPL
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
          } else if ( tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL ) {                // Port1 is RPL
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
          }

          // Tx R-APS (NR, RB)   
          ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_RB, __LINE__);

          // Unblock non-RPL port   
          if ( tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_NONRPL ) {                   // Port0 is non-RPL
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          }
          if ( tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_NONRPL ) {                   // Port1 is non-RPL
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          }

          // Flush FDB
          ptin_erps_FlushFDB(i, __LINE__);
        }
      }

      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }


    //WTR Running  67
    if ( topPriorityRequest == LReq_WTRRun ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }

    //WTB Expires  68
    if ( topPriorityRequest == LReq_WTBExp ) {
      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP;

        // If RPL port is blocked:   
        if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) && (tbl_erps[i].portState[PROT_ERPS_PORT0] == ERP_PORT_BLOCKING) ) {

          // Tx R-APS (NR, RB,DNF)
          ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          // Unblock non-RPL port
          ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

        } else if ( (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) && (tbl_erps[i].portState[PROT_ERPS_PORT1] == ERP_PORT_BLOCKING) ) {

          // Tx R-APS (NR, RB,DNF)
          ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_RB | RReq_STAT_DNF, __LINE__);

          // Unblock non-RPL port
          ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);

        }
        //Else:
        else {
          //Block RPL port
          if (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) {
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
          } else if (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) {
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
          }
          
          // Tx R-APS (NR, RB)  
          ptin_erps_aps_tx(i, RReq_NR, RReq_STAT_RB, __LINE__);

          // Unblock non-RPL port
          if (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_NONRPL) {
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
          } else if (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_NONRPL) {
            ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
          }

          // Flush FDB
          ptin_erps_FlushFDB(i, __LINE__);
        }
      }

      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }


    //WTB Running  69
    if ( topPriorityRequest == LReq_WTBRun ) {
      //No action
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    } 

    //R-APS (NR, RB)  70
    if ( (topPriorityRequest == RReq_NR) && (apsStatusRx & RReq_STAT_RB) ) {
      //If RPL Owner Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPL) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPL) ) {
        //Stop WTR
        tbl_erps[i].wtr_CMD = TIMER_CMD_STOP;
        //Stop WTB
        tbl_erps[i].wtb_CMD = TIMER_CMD_STOP;
      }

      //If neither RPL Owner Node nor RPL Neighbour Node:
      if ( ((tbl_erps[i].protParam.port0Role != ERPS_PORTROLE_RPL) && (tbl_erps[i].protParam.port1Role != ERPS_PORTROLE_RPL))                 &&
           ((tbl_erps[i].protParam.port0Role != ERPS_PORTROLE_RPLNEIGHBOUR) && (tbl_erps[i].protParam.port1Role != ERPS_PORTROLE_RPLNEIGHBOUR))     ) {

        //Unblock ring ports
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
        ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);

        //Stop Tx R-APS
        ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      //If RPL Neighbour Node:
      if ( (tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR) || (tbl_erps[i].protParam.port1Role == ERPS_PORTROLE_RPLNEIGHBOUR) ) {
        //Block RPL port
        //Unblock non-RPL port
        if ( tbl_erps[i].protParam.port0Role == ERPS_PORTROLE_RPLNEIGHBOUR ) {              // Port0 is RPL
        	ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_BLOCKING, __LINE__);
        	ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_FLUSHING, __LINE__);
        } else {                                                                            // Port1 is RPL
        	ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT1, ERP_PORT_BLOCKING, __LINE__);
        	ptin_erps_blockOrUnblockPort(i, PROT_ERPS_PORT0, ERP_PORT_FLUSHING, __LINE__);
        }

        //Stop Tx R-APS      
        ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }

      // Next node state: A
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_A_Idle),__LINE__);
    }


    //R-APS (NR)  71
    if ( topPriorityRequest == RReq_NR ) {
      //If remote Node ID is higher than own Node ID:
      if (0 /*** TO BE DONE ***/) {
        //Unblock non-failed ring port
        ptin_erps_blockOrUnblockPort(i, (req_port == PROT_ERPS_PORT0)? PROT_ERPS_PORT1 : PROT_ERPS_PORT0 , ERP_PORT_FLUSHING, __LINE__);

        // Stop Tx R-APS
        ptin_erps_aps_tx(i, RReq_NONE, RReq_STAT_ZEROS, __LINE__);
      }
      
      // Next node state: E
      ptin_erps_FSM_transition(i,ERP_STATE_SetLocal(ERP_STATE_E_Pending),__LINE__);
    }

    break;

  }//switch



#if 0
  if ( (tbl_erps[i].state_machine_h != tbl_erps[i].state_machine)   || 
       ( (localRequest != LReq_NONE) && (tbl_erps[i].localRequest != localRequest) )         ||
       ( (remoteRequest != RReq_NONE) && (tbl_erps[i].remoteRequest != remoteRequest) )

     ) {

    if (ERP_STATE_IsLocal(tbl_erps[i].state_machine)) {

//    LOG_TRACE(LOG_CTX_OAM, "\n"
//              "|    State    | REQ(FP,P) |\n"
//              "   %3s:%3s:L    %3s(%d,%d) \n",
//              stateToString[ERP_STATE_STATE(tbl_erps[i].state_machine)], ReqToString[localRequest],
//              ReqToString[PSC_REQ(tbl_erps[i].apsRequestTx)], PSC_FPATH(tbl_erps[i].apsRequestTx), PSC_PATH(tbl_erps[i].apsRequestTx));
    } else {
//    LOG_TRACE(LOG_CTX_OAM, "\n"
//              "|    State    | REQ(FP,P) |\n"
//              "   %3s:%3s:R    %3s(%d,%d) \n",
//              stateToString[ERP_STATE_STATE(tbl_erps[i].state_machine)], ReqToString[remoteRequest],
//              ReqToString[PSC_REQ(tbl_erps[i].apsRequestTx)], PSC_FPATH(tbl_erps[i].apsRequestTx), PSC_PATH(tbl_erps[i].apsRequestTx));
    }
  }

  // Switching PATH
  if ( tbl_erps[i].state_machine_h != tbl_erps[i].state_machine ) {

    if ( (ERP_STATE_STATE(tbl_erps[i].state_machine) == PSC_STATE_Normal)      ||
         (ERP_STATE_STATE(tbl_erps[i].state_machine) == PSC_STATE_Unavailable)                 
       ) {

      if ( tbl_erps[i].cx_active == PROT_ERPS_PORT1 ) {
        // Comuta para Working
        switching_path = PROT_ERPS_PORT0;
        tbl_erps[i].cx_active    = PROT_ERPS_PORT0;
        tbl_erps[i].cx_active_h  = PROT_ERPS_PORT1;

        ptin_erps_switch(i, switching_path);
      }
    }

    else if ( (ERP_STATE_STATE(tbl_erps[i].state_machine) == PSC_STATE_DNR)            ||
              (ERP_STATE_STATE(tbl_erps[i].state_machine) == PSC_STATE_ProtFailure)    ||
              (ERP_STATE_STATE(tbl_erps[i].state_machine) == PSC_STATE_ProtAdmin)
            ) {

      if ( tbl_erps[i].cx_active == PROT_ERPS_PORT0 ) {
        // Comuta para Protection
        switching_path = PROT_ERPS_PORT1;
        tbl_erps[i].cx_active    = PROT_ERPS_PORT1;
        tbl_erps[i].cx_active_h  = PROT_ERPS_PORT0;

        ptin_erps_switch(i, switching_path);
      }
    }


  }


  if ( (ERP_STATE_STATE(tbl_erps[i].state_machine) == PSC_STATE_WTR) && (tbl_erps[i].localRequest != localRequest) && (localRequest == LReq_WTRExp) ) {
    if ( tbl_erps[i].cx_active == PROT_ERPS_PORT1 ) {
      // Comuta para Working
      switching_path    = PROT_ERPS_PORT0;
      tbl_erps[i].cx_active    = PROT_ERPS_PORT0;
      tbl_erps[i].cx_active_h  = PROT_ERPS_PORT1;

      ptin_erps_switch(i, switching_path);
    }
  }

  if (localRequest != LReq_NONE) {
    tbl_erps[i].localRequest = localRequest;
  }
  if (remoteRequest != RReq_NONE) {
    tbl_erps[i].remoteRequest = remoteRequest;
  }

  if (apsRequestRx != 0) {
    tbl_erps[i].apsRequestRx = apsRequestRx;
  }

  tbl_erps[i].status_SF[PROT_ERPS_PORT0] = SF[PROT_ERPS_PORT0];
  tbl_erps[i].status_SF[PROT_ERPS_PORT1] = SF[PROT_ERPS_PORT1];

  if ( (tbl_erps[i].operator_cmd & PROT_ERPS_OPCMD_OC) ) {
    tbl_erps[i].operator_cmd = PROT_ERPS_OPCMD_NR;
    tbl_erps[i].status_SF[PROT_ERPS_PORT0] = 0;
    tbl_erps[i].status_SF[PROT_ERPS_PORT1] = 0;
  }
#endif

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
  L7_uint32 i;

  for (i=0; i<MAX_PROT_PROT_ERPS; i++) {

    //LOG_TRACE(LOG_CTX_OAM,"ERPS#%d; admin %d", i, tbl_erps[i].admin);

    if (tbl_erps[i].admin == PROT_ERPS_ENTRY_FREE) {
      continue;
    }

    tbl_erps[i].hal.prot_proc(i);

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
  LOG_TRACE(LOG_CTX_OAM,"ERPS Task started");

  if (osapiTaskInitDone(L7_PTIN_ERPS_TASK_SYNC)!=L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_OAM, "Error syncing task");
    PTIN_CRASH();
  }

  LOG_TRACE(LOG_CTX_OAM,"ERPS task ready");


  /*
   * TESTE
  */
//erpsProtParam_t new_group;
//memset(&new_group, 0, sizeof(erpsProtParam_t));
//ptin_erps_add_entry( /*erps_idx*/ 1, &new_group);


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
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Task ptin_prot_erps_task created");

  if (osapiWaitForTaskInit (L7_PTIN_ERPS_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to initialize ptin_prot_erps_task()\n");
    return(L7_FAILURE);
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Task ptin_prot_erps_task initialized");

  return L7_SUCCESS;
}

