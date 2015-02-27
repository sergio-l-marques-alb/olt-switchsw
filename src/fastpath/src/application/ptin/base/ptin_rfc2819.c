

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//                                                                      //
//              ||||||\   ||||||||   ||||||   ||    ||                  //
//              ||   ||      ||        ||     ||||  ||                  //
//              |||||||      ||        ||     || || ||                  //
//              ||           ||        ||     ||  ||||                  //
//              ||           ||        ||     ||   |||                  //
//              ||           ||      ||||||   ||    ||                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FileName:                                                            //
//            ptin_rfc2819.c                                            //
//                                                                      //
//----------------------------------------------------------------------//
// Projecto:                                                            //
//            GPON                                                      //
//                                                                      //
//----------------------------------------------------------------------//
// Descricao:                                                           //
//            Este implementa as funcoes para processamento de          //
//         performnce segundo a norma rfc2819                           //
//                                                                      //
//----------------------------------------------------------------------//
//                                                                      //
// Historico:                                                           //
//            23 Fevereiro de 2015                                      //
//                                                                      //
//----------------------------------------------------------------------//
// Autor: Celso Lemos                                                   //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <unistd.h>

#include "osapi.h"
#include "logger.h"
#include "ptin_rfc2819.h"
#include "ptin_rfc2819_buffer.h"
#include "ptin_structs.h"
#include "ptin_intf.h"

#define PERIODO_1MIN     1
#define PERIODO_15MIN    2
#define PERIODO_24HORAS  4
#define PERIODO_60MIN    8

TBufferRegQualRFC2819 aux_qual_RFC2819;

T_QUALIDADE_RFC2819 RFC2819_probes_Rx[PTIN_SYSTEM_N_PORTS];
T_QUALIDADE_RFC2819 RFC2819_probes_Tx[PTIN_SYSTEM_N_PORTS];

//Task id
L7_uint32 ptin_rfc2819_TaskId = 0;

/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_config_probe
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param Port: 
 * @param Admin: 
 * ----------------------------------------------------------------------
 * 
 * @return int: 
 * ----------------------------------------------------------------------
 * @author clemos (2/24/2015)
 * ----------------------------------------------------------------------
 */
L7_RC_t ptin_rfc2819_config_probe(L7_int Port, L7_uint8 Admin)
{
  //Check Port
  if (Port>=ptin_sys_number_of_ports)
  {
    return L7_FAILURE;
  }

  //Check Admin  
  if ((Admin!=RFC2819_PROBE_ENABLE) && (Admin!=RFC2819_PROBE_DISABLE))
  {
    return L7_FAILURE;
  }

  //Configure
  if (Admin==RFC2819_PROBE_ENABLE)
  { 
    if (ptin_rfc2819_get_admin(&RFC2819_probes_Rx[Port]) || ptin_rfc2819_get_admin(&RFC2819_probes_Tx[Port])) {
      LOG_INFO(LOG_CTX_RFC2819, "RFC2819 Probe (Port: %d) already enabled",Port);
    }
    else {
      LOG_INFO(LOG_CTX_RFC2819, "Enable RFC2819 Probe (Port: %d)",Port);
      ptin_rfc2819_refresh_counters(Port);
      ptin_rfc2819_enable(&RFC2819_probes_Rx[Port]);
      ptin_rfc2819_enable(&RFC2819_probes_Tx[Port]);
    }
  }
  else
  {
      LOG_INFO(LOG_CTX_RFC2819, "Disable RFC2819 Probe (Port: %d)",Port);
      ptin_rfc2819_disable(&RFC2819_probes_Rx[Port]);
      ptin_rfc2819_disable(&RFC2819_probes_Tx[Port]);
  }
  
  return L7_SUCCESS;

}

/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_get_config_probe
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param Port: 
 * @param Admin: 
 * ----------------------------------------------------------------------
 * 
 * @return L7_RC_t: 
 * ----------------------------------------------------------------------
 * @author clemos (2/27/2015)
 * ----------------------------------------------------------------------
 */
L7_RC_t ptin_rfc2819_get_config_probe(L7_int Port, L7_uint8 *Admin)
{
  //Check Port
  if (Port>=ptin_sys_number_of_ports)
  {
    return L7_FAILURE;
  }

  //we only test Rx,because condRx=ConfTx (allways)
  if (ptin_rfc2819_get_admin(&RFC2819_probes_Rx[Port])==0) {
    *Admin = RFC2819_PROBE_DISABLE;
  }
  else 
  if (ptin_rfc2819_get_admin(&RFC2819_probes_Rx[Port])==1) {
    *Admin = RFC2819_PROBE_ENABLE;
  }
  else 
    return L7_FAILURE;

  return L7_SUCCESS;

}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_init_buffers
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_init_buffers(void)
{
  L7_int aux;

  LOG_INFO(LOG_CTX_RFC2819, "Initializing RFC2819 buffers");
  aux=ptin_rfc2819_buffer_init(-1);
}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_clear_buffers
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_clear_buffers(void)
{
  L7_int i;

  for (i=0;i<MAX_QUAL_RFC2819_BUFFERS;i++) {
    LOG_INFO(LOG_CTX_RFC2819, "Clearing RFC2819 buffers");
    ptin_rfc2819_buffer_clear(i);
  }
}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_regista_15min
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param slot: 
 * @param qual: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_regista_15min(L7_uint8 slot, T_QUALIDADE_RFC2819 *qual)
{

  if (/*(slot==0) || */(slot>=MAX_QUAL_RFC2819_BUFFERS))
    return;

  aux_qual_RFC2819.arg                  =   qual->conf.slot;
  aux_qual_RFC2819.path                 =   qual->conf.path;
  aux_qual_RFC2819.time                 =   time(NULL);
  aux_qual_RFC2819.cTempo               =   qual->status[RFC2819_ACTUAL].cTempo;

  aux_qual_RFC2819.Octets               =   qual->status[RFC2819_ACTUAL].Octets              ;
  aux_qual_RFC2819.Pkts                 =   qual->status[RFC2819_ACTUAL].Pkts                ;
  aux_qual_RFC2819.Broadcast            =   qual->status[RFC2819_ACTUAL].Broadcast           ;
  aux_qual_RFC2819.Multicast            =   qual->status[RFC2819_ACTUAL].Multicast           ;
  aux_qual_RFC2819.CRCAlignErrors       =   qual->status[RFC2819_ACTUAL].CRCAlignErrors      ;
  aux_qual_RFC2819.UndersizePkts        =   qual->status[RFC2819_ACTUAL].UndersizePkts       ;
  aux_qual_RFC2819.OversizePkts         =   qual->status[RFC2819_ACTUAL].OversizePkts        ;
  aux_qual_RFC2819.Fragments            =   qual->status[RFC2819_ACTUAL].Fragments           ;
  aux_qual_RFC2819.Jabbers              =   qual->status[RFC2819_ACTUAL].Jabbers             ;
  aux_qual_RFC2819.Collisions           =   qual->status[RFC2819_ACTUAL].Collisions          ;
  aux_qual_RFC2819.Utilization          =   (L7_uint64)((aux_qual_RFC2819.Octets / aux_qual_RFC2819.cTempo)*8);
  aux_qual_RFC2819.Pkts64Octets         =   qual->status[RFC2819_ACTUAL].Pkts64Octets        ;
  aux_qual_RFC2819.Pkts65to127Octets    =   qual->status[RFC2819_ACTUAL].Pkts65to127Octets   ;
  aux_qual_RFC2819.Pkts128to255Octets   =   qual->status[RFC2819_ACTUAL].Pkts128to255Octets  ;
  aux_qual_RFC2819.Pkts256to511Octets   =   qual->status[RFC2819_ACTUAL].Pkts256to511Octets  ;
  aux_qual_RFC2819.Pkts512to1023Octets  =   qual->status[RFC2819_ACTUAL].Pkts512to1023Octets ;
  aux_qual_RFC2819.Pkts1024to1518Octets =   qual->status[RFC2819_ACTUAL].Pkts1024to1518Octets;   

  LOG_TRACE(LOG_CTX_RFC2819, "15MIN  : %.08lx  |  %.02d-%.02d-%.04d  |  %d:%.02d:00  | %5d | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld",
            qual->conf.path,
            qual->reg_data.dia,
            qual->reg_data.mes,
            qual->reg_data.ano,
            qual->reg_data.hora,
            qual->reg_data.min,

            aux_qual_RFC2819.cTempo               ,
            aux_qual_RFC2819.Octets               ,
            aux_qual_RFC2819.Pkts                 ,
            aux_qual_RFC2819.Broadcast            ,
            aux_qual_RFC2819.Multicast            ,
            aux_qual_RFC2819.CRCAlignErrors       ,
            aux_qual_RFC2819.UndersizePkts        ,
            aux_qual_RFC2819.OversizePkts         ,
            aux_qual_RFC2819.Fragments            ,
            aux_qual_RFC2819.Jabbers              ,
            aux_qual_RFC2819.Collisions           ,
            aux_qual_RFC2819.Utilization          ,
            aux_qual_RFC2819.Pkts64Octets         ,
            aux_qual_RFC2819.Pkts65to127Octets    ,
            aux_qual_RFC2819.Pkts128to255Octets   ,
            aux_qual_RFC2819.Pkts256to511Octets   ,
            aux_qual_RFC2819.Pkts512to1023Octets  ,
            aux_qual_RFC2819.Pkts1024to1518Octets);

  ptin_rfc2819_buffer_write(slot, (void *)(&aux_qual_RFC2819), 0);
}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_regista_24horas
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param slot: 
 * @param qual: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_regista_24horas(L7_uint8 slot, T_QUALIDADE_RFC2819 *qual)
{    
  if (/*(slot==0) || */(slot>=MAX_QUAL_RFC2819_BUFFERS))
    return;

  aux_qual_RFC2819.arg                  = qual->conf.slot;
  aux_qual_RFC2819.path                = qual->conf.path;
  aux_qual_RFC2819.time                = time(NULL);
  aux_qual_RFC2819.cTempo              = qual->status[RFC2819_PER24HORAS].cTempo;

  aux_qual_RFC2819.Octets              =   qual->status[RFC2819_PER24HORAS].Octets              ;
  aux_qual_RFC2819.Pkts                =   qual->status[RFC2819_PER24HORAS].Pkts                ;
  aux_qual_RFC2819.Broadcast           =   qual->status[RFC2819_PER24HORAS].Broadcast           ;
  aux_qual_RFC2819.Multicast           =   qual->status[RFC2819_PER24HORAS].Multicast           ;
  aux_qual_RFC2819.CRCAlignErrors      =   qual->status[RFC2819_PER24HORAS].CRCAlignErrors      ;
  aux_qual_RFC2819.UndersizePkts       =   qual->status[RFC2819_PER24HORAS].UndersizePkts       ;
  aux_qual_RFC2819.OversizePkts        =   qual->status[RFC2819_PER24HORAS].OversizePkts        ;
  aux_qual_RFC2819.Fragments           =   qual->status[RFC2819_PER24HORAS].Fragments           ;
  aux_qual_RFC2819.Jabbers             =   qual->status[RFC2819_PER24HORAS].Jabbers             ;
  aux_qual_RFC2819.Collisions          =   qual->status[RFC2819_PER24HORAS].Collisions          ;
  aux_qual_RFC2819.Utilization         =   (L7_uint64)((aux_qual_RFC2819.Octets / aux_qual_RFC2819.cTempo)*8) ;
  aux_qual_RFC2819.Pkts64Octets        =   qual->status[RFC2819_PER24HORAS].Pkts64Octets        ;
  aux_qual_RFC2819.Pkts65to127Octets   =   qual->status[RFC2819_PER24HORAS].Pkts65to127Octets   ;
  aux_qual_RFC2819.Pkts128to255Octets  =   qual->status[RFC2819_PER24HORAS].Pkts128to255Octets  ;
  aux_qual_RFC2819.Pkts256to511Octets  =   qual->status[RFC2819_PER24HORAS].Pkts256to511Octets  ;
  aux_qual_RFC2819.Pkts512to1023Octets =   qual->status[RFC2819_PER24HORAS].Pkts512to1023Octets ;
  aux_qual_RFC2819.Pkts1024to1518Octets=   qual->status[RFC2819_PER24HORAS].Pkts1024to1518Octets;

  LOG_TRACE(LOG_CTX_RFC2819, "24HOURS: %.08lx  |  %.02d-%.02d-%.04d  |  %d:%.02d:00  | %5d | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld | %13lld",
            qual->conf.path,
            qual->reg_data.dia,
            qual->reg_data.mes,
            qual->reg_data.ano,
            qual->reg_data.hora,
            qual->reg_data.min,

            aux_qual_RFC2819.cTempo            ,  
            aux_qual_RFC2819.Octets            ,  
            aux_qual_RFC2819.Pkts              ,  
            aux_qual_RFC2819.Broadcast         ,  
            aux_qual_RFC2819.Multicast         ,  
            aux_qual_RFC2819.CRCAlignErrors    ,  
            aux_qual_RFC2819.UndersizePkts     ,  
            aux_qual_RFC2819.OversizePkts      ,  
            aux_qual_RFC2819.Fragments         ,  
            aux_qual_RFC2819.Jabbers           ,  
            aux_qual_RFC2819.Collisions        ,  
            aux_qual_RFC2819.Utilization       ,  
            aux_qual_RFC2819.Pkts64Octets      ,  
            aux_qual_RFC2819.Pkts65to127Octets ,  
            aux_qual_RFC2819.Pkts128to255Octets,  
            aux_qual_RFC2819.Pkts256to511Octets,  
            aux_qual_RFC2819.Pkts512to1023Octets, 
            aux_qual_RFC2819.Pkts1024to1518Octets);

  ptin_rfc2819_buffer_write(slot, (void *)(&aux_qual_RFC2819), 0);
}



/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_init_probe
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param param: 
 * @param slot: 
 * @param path: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/25/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_init_probe(T_QUALIDADE_RFC2819 *param, L7_uint8 slot, L7_ulong32 path)
{
  L7_int i;

  param->conf.estado   = 0;

  param->conf.slot     = slot;
  param->conf.path     = path;

  for (i=0;i<2;i++) {

    param->status[i].Octets               = 0;
    param->status[i].Pkts                 = 0;
    param->status[i].Broadcast            = 0;
    param->status[i].Multicast            = 0;
    param->status[i].CRCAlignErrors       = 0;
    param->status[i].UndersizePkts        = 0;
    param->status[i].OversizePkts         = 0;
    param->status[i].Fragments            = 0;
    param->status[i].Jabbers              = 0;
    param->status[i].Collisions           = 0;
    param->status[i].Utilization          = 0;
    param->status[i].Pkts64Octets         = 0;
    param->status[i].Pkts65to127Octets    = 0;
    param->status[i].Pkts128to255Octets   = 0;
    param->status[i].Pkts256to511Octets   = 0;
    param->status[i].Pkts512to1023Octets  = 0;
    param->status[i].Pkts1024to1518Octets = 0;
    param->status[i].cTempo               = 0;

  }

  param->Octets               = 0;
  param->Pkts                 = 0;
  param->Broadcast            = 0;
  param->Multicast            = 0;
  param->CRCAlignErrors       = 0;
  param->UndersizePkts        = 0;
  param->OversizePkts         = 0;
  param->Fragments            = 0;
  param->Jabbers              = 0;
  param->Collisions           = 0;
  param->Utilization          = 0;
  param->Pkts64Octets         = 0;
  param->Pkts65to127Octets    = 0;
  param->Pkts128to255Octets   = 0;
  param->Pkts256to511Octets   = 0;
  param->Pkts512to1023Octets  = 0;
  param->Pkts1024to1518Octets = 0;
}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_init_probes
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/24/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_init_all_probes(void)
{
  L7_int Port;

  LOG_INFO(LOG_CTX_STARTUP, "Initializing RFC2819 all Probes");

  for (Port=0; Port<PTIN_SYSTEM_N_PORTS; Port++)
  {
    ptin_rfc2819_init_probe(&RFC2819_probes_Rx[Port], 0xFF, CONCAT_QUAL_PATH(0,0,Port+1)); //RFC2819 RX
    ptin_rfc2819_init_probe(&RFC2819_probes_Tx[Port], 0xFF, CONCAT_QUAL_PATH(1,0,Port+1)); //RFC2819 TX
  }

}



/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_refresh_counters
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param Port: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/25/2015)
 * ----------------------------------------------------------------------
 */
L7_int ptin_rfc2819_refresh_counters(L7_int Port)
{
  ptin_HWEthRFC2819_PortStatistics_t portStats;

  /* Read statistics */
  portStats.Port = Port;
  portStats.Mask = 0xFF;
  portStats.RxMask = 0xFFFFFFFF;
  portStats.TxMask = 0xFFFFFFFF;
  if (ptin_intf_counters_read(&portStats) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_RFC2819, "Error getting statistics of port# %u", portStats.Port);
    return L7_FAILURE;
  }
  else
    LOG_TRACE(LOG_CTX_RFC2819, "Getting statistics of port# %u: SUCCESS", portStats.Port);

  //RX
  RFC2819_probes_Rx[Port].Octets               = portStats.Rx.etherStatsOctets;
  RFC2819_probes_Rx[Port].Pkts                 = portStats.Rx.etherStatsPkts;
  RFC2819_probes_Rx[Port].Broadcast            = portStats.Rx.etherStatsBroadcastPkts;
  RFC2819_probes_Rx[Port].Multicast            = portStats.Rx.etherStatsMulticastPkts;
  RFC2819_probes_Rx[Port].CRCAlignErrors       = portStats.Rx.etherStatsCRCAlignErrors;
  RFC2819_probes_Rx[Port].UndersizePkts        = portStats.Rx.etherStatsUndersizePkts;
  RFC2819_probes_Rx[Port].OversizePkts         = portStats.Rx.etherStatsOversizePkts;
  RFC2819_probes_Rx[Port].Fragments            = portStats.Rx.etherStatsFragments;
  RFC2819_probes_Rx[Port].Jabbers              = portStats.Rx.etherStatsJabbers;
  RFC2819_probes_Rx[Port].Collisions           = portStats.Rx.etherStatsCollisions;
  RFC2819_probes_Rx[Port].Utilization          = portStats.Rx.Throughput;
  RFC2819_probes_Rx[Port].Pkts64Octets         = portStats.Rx.etherStatsPkts64Octets;
  RFC2819_probes_Rx[Port].Pkts65to127Octets    = portStats.Rx.etherStatsPkts65to127Octets;
  RFC2819_probes_Rx[Port].Pkts128to255Octets   = portStats.Rx.etherStatsPkts128to255Octets;
  RFC2819_probes_Rx[Port].Pkts256to511Octets   = portStats.Rx.etherStatsPkts256to511Octets;
  RFC2819_probes_Rx[Port].Pkts512to1023Octets  = portStats.Rx.etherStatsPkts512to1023Octets;
  RFC2819_probes_Rx[Port].Pkts1024to1518Octets = portStats.Rx.etherStatsPkts1024to1518Octets;

  //TX
  RFC2819_probes_Tx[Port].Octets               = portStats.Tx.etherStatsOctets;
  RFC2819_probes_Tx[Port].Pkts                 = portStats.Tx.etherStatsPkts;
  RFC2819_probes_Tx[Port].Broadcast            = portStats.Tx.etherStatsBroadcastPkts;
  RFC2819_probes_Tx[Port].Multicast            = portStats.Tx.etherStatsMulticastPkts;
  RFC2819_probes_Tx[Port].CRCAlignErrors       = portStats.Tx.etherStatsCRCAlignErrors;
  RFC2819_probes_Tx[Port].UndersizePkts        = portStats.Tx.etherStatsUndersizePkts;
  RFC2819_probes_Tx[Port].OversizePkts         = portStats.Tx.etherStatsOversizePkts;
  RFC2819_probes_Tx[Port].Fragments            = portStats.Tx.etherStatsFragments;
  RFC2819_probes_Tx[Port].Jabbers              = portStats.Tx.etherStatsJabbers;
  RFC2819_probes_Tx[Port].Collisions           = portStats.Tx.etherStatsCollisions;
  RFC2819_probes_Tx[Port].Utilization          = portStats.Tx.Throughput;
  RFC2819_probes_Tx[Port].Pkts64Octets         = portStats.Tx.etherStatsPkts64Octets;
  RFC2819_probes_Tx[Port].Pkts65to127Octets    = portStats.Tx.etherStatsPkts65to127Octets;
  RFC2819_probes_Tx[Port].Pkts128to255Octets   = portStats.Tx.etherStatsPkts128to255Octets;
  RFC2819_probes_Tx[Port].Pkts256to511Octets   = portStats.Tx.etherStatsPkts256to511Octets;
  RFC2819_probes_Tx[Port].Pkts512to1023Octets  = portStats.Tx.etherStatsPkts512to1023Octets;
  RFC2819_probes_Tx[Port].Pkts1024to1518Octets = portStats.Tx.etherStatsPkts1024to1518Octets;

  return L7_SUCCESS;
}




/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_load_counter
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param port: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/25/2015)
 * ----------------------------------------------------------------------
 */
L7_int ptin_rfc2819_load_counters(L7_int Port)
{
  ptin_HWEthRFC2819_PortStatistics_t portStats;

  //if ((RFC2819_probes_Rx[Port].conf.estado==1) || (RFC2819_probes_Tx[Port].conf.estado==1)) {
      /* Read statistics */
      portStats.Port = Port;
      portStats.Mask = 0xFF;
      portStats.RxMask = 0xFFFFFFFF;
      portStats.TxMask = 0xFFFFFFFF;
      if (ptin_intf_counters_read(&portStats) != L7_SUCCESS) {
        LOG_ERR(LOG_CTX_RFC2819, "Error getting statistics of port# %u", portStats.Port);
        return L7_FAILURE;
      }
      else
        LOG_TRACE(LOG_CTX_RFC2819, "Getting statistics of port# %u: SUCCESS", portStats.Port);

  //}

    
  //Process receive counters
  //------------------------------------------------------------------------
  if (RFC2819_probes_Rx[Port].conf.estado==0 ) {
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Octets               = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts                 = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Broadcast            = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Multicast            = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].CRCAlignErrors       = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].UndersizePkts        = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].OversizePkts         = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Fragments            = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Jabbers              = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Collisions           = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Utilization          = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts64Octets         = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts65to127Octets    = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts128to255Octets   = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts256to511Octets   = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts512to1023Octets  = 0;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts1024to1518Octets = 0;
  } 
  else {
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Octets               = portStats.Rx.etherStatsOctets              -RFC2819_probes_Rx[Port].Octets;
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts                 = portStats.Rx.etherStatsPkts                -RFC2819_probes_Rx[Port].Pkts;                
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Broadcast            = portStats.Rx.etherStatsBroadcastPkts       -RFC2819_probes_Rx[Port].Broadcast;           
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Multicast            = portStats.Rx.etherStatsMulticastPkts       -RFC2819_probes_Rx[Port].Multicast;           
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].CRCAlignErrors       = portStats.Rx.etherStatsCRCAlignErrors      -RFC2819_probes_Rx[Port].CRCAlignErrors;      
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].UndersizePkts        = portStats.Rx.etherStatsUndersizePkts       -RFC2819_probes_Rx[Port].UndersizePkts;       
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].OversizePkts         = portStats.Rx.etherStatsOversizePkts        -RFC2819_probes_Rx[Port].OversizePkts;        
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Fragments            = portStats.Rx.etherStatsFragments           -RFC2819_probes_Rx[Port].Fragments;        
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Jabbers              = portStats.Rx.etherStatsJabbers             -RFC2819_probes_Rx[Port].Jabbers;         
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Collisions           = portStats.Rx.etherStatsCollisions          -RFC2819_probes_Rx[Port].Collisions;          
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Utilization          = 0; //Sera processado apenas no registo
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts64Octets         = portStats.Rx.etherStatsPkts64Octets        -RFC2819_probes_Rx[Port].Pkts64Octets;        
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts65to127Octets    = portStats.Rx.etherStatsPkts65to127Octets   -RFC2819_probes_Rx[Port].Pkts65to127Octets;   
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts128to255Octets   = portStats.Rx.etherStatsPkts128to255Octets  -RFC2819_probes_Rx[Port].Pkts128to255Octets;  
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts256to511Octets   = portStats.Rx.etherStatsPkts256to511Octets  -RFC2819_probes_Rx[Port].Pkts256to511Octets;  
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts512to1023Octets  = portStats.Rx.etherStatsPkts512to1023Octets -RFC2819_probes_Rx[Port].Pkts512to1023Octets; 
    RFC2819_probes_Rx[Port].status[RFC2819_ACTUAL].Pkts1024to1518Octets = portStats.Rx.etherStatsPkts1024to1518Octets-RFC2819_probes_Rx[Port].Pkts1024to1518Octets;


    //Refresh backup counters
    RFC2819_probes_Rx[Port].Octets               = portStats.Rx.etherStatsOctets              ;
    RFC2819_probes_Rx[Port].Pkts                 = portStats.Rx.etherStatsPkts                ;
    RFC2819_probes_Rx[Port].Broadcast            = portStats.Rx.etherStatsBroadcastPkts       ;
    RFC2819_probes_Rx[Port].Multicast            = portStats.Rx.etherStatsMulticastPkts       ;
    RFC2819_probes_Rx[Port].CRCAlignErrors       = portStats.Rx.etherStatsCRCAlignErrors      ;
    RFC2819_probes_Rx[Port].UndersizePkts        = portStats.Rx.etherStatsUndersizePkts       ;
    RFC2819_probes_Rx[Port].OversizePkts         = portStats.Rx.etherStatsOversizePkts        ;
    RFC2819_probes_Rx[Port].Fragments            = portStats.Rx.etherStatsFragments           ;
    RFC2819_probes_Rx[Port].Jabbers              = portStats.Rx.etherStatsJabbers             ;
    RFC2819_probes_Rx[Port].Collisions           = portStats.Rx.etherStatsCollisions          ;
    RFC2819_probes_Rx[Port].Utilization          = 0; //Sera processado apenas no registo
    RFC2819_probes_Rx[Port].Pkts64Octets         = portStats.Rx.etherStatsPkts64Octets        ;
    RFC2819_probes_Rx[Port].Pkts65to127Octets    = portStats.Rx.etherStatsPkts65to127Octets   ;
    RFC2819_probes_Rx[Port].Pkts128to255Octets   = portStats.Rx.etherStatsPkts128to255Octets  ;
    RFC2819_probes_Rx[Port].Pkts256to511Octets   = portStats.Rx.etherStatsPkts256to511Octets  ;
    RFC2819_probes_Rx[Port].Pkts512to1023Octets  = portStats.Rx.etherStatsPkts512to1023Octets ;
    RFC2819_probes_Rx[Port].Pkts1024to1518Octets = portStats.Rx.etherStatsPkts1024to1518Octets;
  }

  //Process Transmit counters
  //------------------------------------------------------------------------
  if (RFC2819_probes_Tx[Port].conf.estado==0 ) {
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Octets               = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts                 = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Broadcast            = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Multicast            = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].CRCAlignErrors       = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].UndersizePkts        = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].OversizePkts         = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Fragments            = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Jabbers              = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Collisions           = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Utilization          = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts64Octets         = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts65to127Octets    = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts128to255Octets   = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts256to511Octets   = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts512to1023Octets  = 0;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts1024to1518Octets = 0;
  } 
  else {
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Octets               = portStats.Tx.etherStatsOctets              -RFC2819_probes_Tx[Port].Octets;
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts                 = portStats.Tx.etherStatsPkts                -RFC2819_probes_Tx[Port].Pkts;                
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Broadcast            = portStats.Tx.etherStatsBroadcastPkts       -RFC2819_probes_Tx[Port].Broadcast;           
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Multicast            = portStats.Tx.etherStatsMulticastPkts       -RFC2819_probes_Tx[Port].Multicast;           
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].CRCAlignErrors       = portStats.Tx.etherStatsCRCAlignErrors      -RFC2819_probes_Tx[Port].CRCAlignErrors;      
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].UndersizePkts        = portStats.Tx.etherStatsUndersizePkts       -RFC2819_probes_Tx[Port].UndersizePkts;       
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].OversizePkts         = portStats.Tx.etherStatsOversizePkts        -RFC2819_probes_Tx[Port].OversizePkts;        
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Fragments            = portStats.Tx.etherStatsFragments           -RFC2819_probes_Tx[Port].Fragments;        
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Jabbers              = portStats.Tx.etherStatsJabbers             -RFC2819_probes_Tx[Port].Jabbers;         
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Collisions           = portStats.Tx.etherStatsCollisions          -RFC2819_probes_Tx[Port].Collisions;          
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Utilization          = 0; //Sera processado apenas no registo
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts64Octets         = portStats.Tx.etherStatsPkts64Octets        -RFC2819_probes_Tx[Port].Pkts64Octets;        
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts65to127Octets    = portStats.Tx.etherStatsPkts65to127Octets   -RFC2819_probes_Tx[Port].Pkts65to127Octets;   
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts128to255Octets   = portStats.Tx.etherStatsPkts128to255Octets  -RFC2819_probes_Tx[Port].Pkts128to255Octets;  
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts256to511Octets   = portStats.Tx.etherStatsPkts256to511Octets  -RFC2819_probes_Tx[Port].Pkts256to511Octets;  
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts512to1023Octets  = portStats.Tx.etherStatsPkts512to1023Octets -RFC2819_probes_Tx[Port].Pkts512to1023Octets; 
    RFC2819_probes_Tx[Port].status[RFC2819_ACTUAL].Pkts1024to1518Octets = portStats.Tx.etherStatsPkts1024to1518Octets-RFC2819_probes_Tx[Port].Pkts1024to1518Octets;

    //Refresh backup counters
    RFC2819_probes_Tx[Port].Octets               = portStats.Tx.etherStatsOctets              ;
    RFC2819_probes_Tx[Port].Pkts                 = portStats.Tx.etherStatsPkts                ;
    RFC2819_probes_Tx[Port].Broadcast            = portStats.Tx.etherStatsBroadcastPkts       ;
    RFC2819_probes_Tx[Port].Multicast            = portStats.Tx.etherStatsMulticastPkts       ;
    RFC2819_probes_Tx[Port].CRCAlignErrors       = portStats.Tx.etherStatsCRCAlignErrors      ;
    RFC2819_probes_Tx[Port].UndersizePkts        = portStats.Tx.etherStatsUndersizePkts       ;
    RFC2819_probes_Tx[Port].OversizePkts         = portStats.Tx.etherStatsOversizePkts        ;
    RFC2819_probes_Tx[Port].Fragments            = portStats.Tx.etherStatsFragments           ;
    RFC2819_probes_Tx[Port].Jabbers              = portStats.Tx.etherStatsJabbers             ;
    RFC2819_probes_Tx[Port].Collisions           = portStats.Tx.etherStatsCollisions          ;
    RFC2819_probes_Tx[Port].Utilization          = 0; //Sera processado apenas no registo
    RFC2819_probes_Tx[Port].Pkts64Octets         = portStats.Tx.etherStatsPkts64Octets        ;
    RFC2819_probes_Tx[Port].Pkts65to127Octets    = portStats.Tx.etherStatsPkts65to127Octets   ;
    RFC2819_probes_Tx[Port].Pkts128to255Octets   = portStats.Tx.etherStatsPkts128to255Octets  ;
    RFC2819_probes_Tx[Port].Pkts256to511Octets   = portStats.Tx.etherStatsPkts256to511Octets  ;
    RFC2819_probes_Tx[Port].Pkts512to1023Octets  = portStats.Tx.etherStatsPkts512to1023Octets ;
    RFC2819_probes_Tx[Port].Pkts1024to1518Octets = portStats.Tx.etherStatsPkts1024to1518Octets;
  }

  return L7_SUCCESS;
}




/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_task
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/25/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_task( void )
{
  time_t tm;
  struct tm tm_time;
  L7_int tgl_clock;
  int old_sec, old_day;
  L7_int Port;

  if (osapiTaskInitDone(L7_PTIN_RFC2819_TASK_SYNC) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_RFC2819, "Error syncing task");
    PTIN_CRASH();
  }

  time(&tm);
  memcpy(&tm_time, localtime(&tm), sizeof(tm_time));
  old_sec = tm_time.tm_sec;
  old_day = tm_time.tm_mday;

  /* Loop */
  while (1) {
    //LOG_INFO(LOG_CTX_RFC2819, "ptin_rfc2819_task running...");

    time(&tm);
    memcpy(&tm_time, localtime(&tm), sizeof(tm_time));        

    tgl_clock   = 0;

    if(tm_time.tm_sec==0) {
        tgl_clock=PERIODO_1MIN;
        //LOG_INFO(LOG_CTX_RFC2819, "ptin_rfc2819_task running...1 min");
        if ((tm_time.tm_min%15)==0) {
            //LOG_INFO(LOG_CTX_RFC2819, "ptin_rfc2819_task running...15 min");
            tgl_clock |= PERIODO_15MIN;
        }
        if (tm_time.tm_min==0) {
            //LOG_INFO(LOG_CTX_RFC2819, "ptin_rfc2819_task running...60 min");
            tgl_clock |= PERIODO_60MIN;
        }
        if(tm_time.tm_mday!=old_day) {
            //LOG_INFO(LOG_CTX_RFC2819, "ptin_rfc2819_task running...24 hours");
            old_day=tm_time.tm_mday;
            tgl_clock |= PERIODO_24HORAS;
        }
    }      

    for (Port=0; Port<PTIN_SYSTEM_N_PORTS; Port++)
    {
      //load counters every 15min period. 
      // We don't need to load every second because we only need to proccess counters in periods of 15min/24hours.
      if ((tgl_clock & PERIODO_15MIN)!=0) {  
        ptin_rfc2819_load_counters(Port);
      }

      ptin_rfc2819_proc(RFC2819_BUFFER_15MIN, RFC2819_BUFFER_24HOURS, &RFC2819_probes_Rx[Port], &tm_time, tgl_clock);
      ptin_rfc2819_proc(RFC2819_BUFFER_15MIN, RFC2819_BUFFER_24HOURS, &RFC2819_probes_Tx[Port], &tm_time, tgl_clock);   
    }
                                    
    osapiSleep(1);
  }
}



/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_init
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/25/2015)
 * ----------------------------------------------------------------------
 */
L7_RC_t ptin_rfc2819_init(void)
{
  ptin_rfc2819_init_buffers();
  ptin_rfc2819_init_all_probes();

  // Create task for rfc2819 processing
  ptin_rfc2819_TaskId = osapiTaskCreate("ptin_rfc2819_task", ptin_rfc2819_task, 0, 0,
                                                L7_DEFAULT_STACK_SIZE,
                                                L7_DEFAULT_TASK_PRIORITY,
                                                L7_DEFAULT_TASK_SLICE);

  if (ptin_rfc2819_TaskId == L7_ERROR) {
    LOG_FATAL(LOG_CTX_RFC2819, "Could not create task ptin_rfc2819_task");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_RFC2819,"Task ptin_rfc2819_task created");

  if (osapiWaitForTaskInit (L7_PTIN_RFC2819_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_RFC2819,"Unable to initialize ptin_rfc2819_task()\n");
    return(L7_FAILURE);
  }
  LOG_TRACE(LOG_CTX_RFC2819,"Task ptin_rfc2819_task initialized");

  return L7_SUCCESS;

}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_clear
 * ----------------------------------------------------------------------
 * Description:     
 * ----------------------------------------------------------------------
 * 
 * @param param: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_clear(T_QUALIDADE_RFC2819 *param)
{
  param->status[RFC2819_ACTUAL].Octets               = param->Octets              ; 
  param->status[RFC2819_ACTUAL].Pkts                 = param->Pkts                ; 
  param->status[RFC2819_ACTUAL].Broadcast            = param->Broadcast           ; 
  param->status[RFC2819_ACTUAL].Multicast            = param->Multicast           ; 
  param->status[RFC2819_ACTUAL].CRCAlignErrors       = param->CRCAlignErrors      ; 
  param->status[RFC2819_ACTUAL].UndersizePkts        = param->UndersizePkts       ; 
  param->status[RFC2819_ACTUAL].OversizePkts         = param->OversizePkts        ; 
  param->status[RFC2819_ACTUAL].Fragments            = param->Fragments           ; 
  param->status[RFC2819_ACTUAL].Jabbers              = param->Jabbers             ; 
  param->status[RFC2819_ACTUAL].Collisions           = param->Collisions          ; 
  param->status[RFC2819_ACTUAL].Utilization          = param->Utilization         ; 
  param->status[RFC2819_ACTUAL].Pkts64Octets         = param->Pkts64Octets        ; 
  param->status[RFC2819_ACTUAL].Pkts65to127Octets    = param->Pkts65to127Octets   ; 
  param->status[RFC2819_ACTUAL].Pkts128to255Octets   = param->Pkts128to255Octets  ; 
  param->status[RFC2819_ACTUAL].Pkts256to511Octets   = param->Pkts256to511Octets  ; 
  param->status[RFC2819_ACTUAL].Pkts512to1023Octets  = param->Pkts512to1023Octets ; 
  param->status[RFC2819_ACTUAL].Pkts1024to1518Octets = param->Pkts1024to1518Octets; 
  param->status[RFC2819_ACTUAL].cTempo               = 0;
}



/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_enable
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param param: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_enable(T_QUALIDADE_RFC2819 *param)
{
  param->conf.estado = 1;
}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_get_admin
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param param: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/26/2015)
 * ----------------------------------------------------------------------
 */
L7_int ptin_rfc2819_get_admin(T_QUALIDADE_RFC2819 *param)
{
  return(param->conf.estado);
}

/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_disable
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param param: 
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_disable(T_QUALIDADE_RFC2819 *param)
{
  param->conf.estado = 0;
}



/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_proc
 * ----------------------------------------------------------------------
 * Description:  Função que deve ser chamada com um intervalo de 15min, para       
 *           processamento de qualidade de um determinado feixe.               
 *           Esta rotina so' deve ser chamada depois de ter sido executada a   
 *           rotina de inicializacao.                                          
 *   
 * ----------------------------------------------------------------------
 * 
 * @param buffer_15min: 
 * @param buffer_24horas: 
 * @param param: estrutura que possui oas variaveis de estado e configuracao 
 *               da fonte que se pretende analisar
 * @param data_hora: 
 * @param tgl_time: 
 *           Esta variavel e' uma mascara, em que cada bit indica se  
 *         foi uma mudança de periodo de tempo:                     
 *           Bit0: quando vem a 1 significa que temos uma mudança de  
 *                 de minuto                                          
 *           Bit1: quando vem a 1 significa que temos uma mudança de  
 *                 de 15 minutos                                      
 *           Bit2: quando vem a 1 significa que temos uma mudança de  
 *                 de 24 horas                                        
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_proc(L7_uint8 buffer_15min, L7_uint8 buffer_24horas, T_QUALIDADE_RFC2819 *param, struct tm *data_hora, L7_int tgl_time)
{
  if (param->conf.estado==0) {
    return;    
  }

  param->status[RFC2819_ACTUAL].cTempo ++;

  if ((tgl_time & PERIODO_15MIN)!=0) 
  {  
    param->reg_data.dia  = data_hora->tm_mday;
    param->reg_data.mes  = data_hora->tm_mon+1;
    param->reg_data.ano  = data_hora->tm_year+1900;
    param->reg_data.hora = data_hora->tm_hour;
    param->reg_data.min  = data_hora->tm_min;


    //Regista os 15 minutos
    ptin_rfc2819_regista_15min(buffer_15min,(void *)param);        

    param->status[RFC2819_PER24HORAS].cTempo              +=   param->status[RFC2819_ACTUAL].cTempo              ;
    param->status[RFC2819_PER24HORAS].Octets              +=   param->status[RFC2819_ACTUAL].Octets              ;
    param->status[RFC2819_PER24HORAS].Pkts                +=   param->status[RFC2819_ACTUAL].Pkts                ;
    param->status[RFC2819_PER24HORAS].Broadcast           +=   param->status[RFC2819_ACTUAL].Broadcast           ;
    param->status[RFC2819_PER24HORAS].Multicast           +=   param->status[RFC2819_ACTUAL].Multicast           ;
    param->status[RFC2819_PER24HORAS].CRCAlignErrors      +=   param->status[RFC2819_ACTUAL].CRCAlignErrors      ;
    param->status[RFC2819_PER24HORAS].UndersizePkts       +=   param->status[RFC2819_ACTUAL].UndersizePkts       ;
    param->status[RFC2819_PER24HORAS].OversizePkts        +=   param->status[RFC2819_ACTUAL].OversizePkts        ;
    param->status[RFC2819_PER24HORAS].Fragments           +=   param->status[RFC2819_ACTUAL].Fragments           ;
    param->status[RFC2819_PER24HORAS].Jabbers             +=   param->status[RFC2819_ACTUAL].Jabbers             ;
    param->status[RFC2819_PER24HORAS].Collisions          +=   param->status[RFC2819_ACTUAL].Collisions          ;
    param->status[RFC2819_PER24HORAS].Utilization         +=   param->status[RFC2819_ACTUAL].Utilization         ;
    param->status[RFC2819_PER24HORAS].Pkts64Octets        +=   param->status[RFC2819_ACTUAL].Pkts64Octets        ;
    param->status[RFC2819_PER24HORAS].Pkts65to127Octets   +=   param->status[RFC2819_ACTUAL].Pkts65to127Octets   ;
    param->status[RFC2819_PER24HORAS].Pkts128to255Octets  +=   param->status[RFC2819_ACTUAL].Pkts128to255Octets  ;
    param->status[RFC2819_PER24HORAS].Pkts256to511Octets  +=   param->status[RFC2819_ACTUAL].Pkts256to511Octets  ;
    param->status[RFC2819_PER24HORAS].Pkts512to1023Octets +=   param->status[RFC2819_ACTUAL].Pkts512to1023Octets ;
    param->status[RFC2819_PER24HORAS].Pkts1024to1518Octets+=   param->status[RFC2819_ACTUAL].Pkts1024to1518Octets;


    ptin_rfc2819_clear(param);  //Clear das variaveis contadoras

    if ((param->reg_data.hora==0) && (param->reg_data.min)==0) 
    {
      //Regista as 24 horas
      ptin_rfc2819_regista_24horas(buffer_24horas,(void *)param); 

      param->status[RFC2819_PER24HORAS].cTempo            = 0;  
      param->status[RFC2819_PER24HORAS].Octets            = 0;  
      param->status[RFC2819_PER24HORAS].Pkts              = 0;  
      param->status[RFC2819_PER24HORAS].Broadcast         = 0;  
      param->status[RFC2819_PER24HORAS].Multicast         = 0;  
      param->status[RFC2819_PER24HORAS].CRCAlignErrors    = 0;  
      param->status[RFC2819_PER24HORAS].UndersizePkts     = 0;  
      param->status[RFC2819_PER24HORAS].OversizePkts      = 0;  
      param->status[RFC2819_PER24HORAS].Fragments         = 0;  
      param->status[RFC2819_PER24HORAS].Jabbers           = 0;  
      param->status[RFC2819_PER24HORAS].Collisions        = 0;  
      param->status[RFC2819_PER24HORAS].Utilization       = 0;  
      param->status[RFC2819_PER24HORAS].Pkts64Octets      = 0;  
      param->status[RFC2819_PER24HORAS].Pkts65to127Octets = 0;  
      param->status[RFC2819_PER24HORAS].Pkts128to255Octets= 0;  
      param->status[RFC2819_PER24HORAS].Pkts256to511Octets= 0;  
      param->status[RFC2819_PER24HORAS].Pkts512to1023Octets= 0;  
      param->status[RFC2819_PER24HORAS].Pkts1024to1518Octets= 0;  
    }

  }
}




