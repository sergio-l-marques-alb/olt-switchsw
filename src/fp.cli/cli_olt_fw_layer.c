//**************************************************************************************
//
//ABSTRACT:
//Client from "cli_unicom.c". Defines get/set functions for struct T_CLI_SET_GET_FUNCTIONS
//
//HISTORY:
//
//
//COPYRIGHT:
//PT Inova��o (a.k.a. PTIn, a.k.a. CET)
//
//
//CONTACT:
//Rui Costa
//
//**************************************************************************************

#include <cli_olt.h>
#include <ipc_lib.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sirerrors.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include "trmi.h"
#include "utils.h"

#define PORTO_RX_MSG_BUGA            6006  //claudia
#define PORTO_TX_MSG_BUGA            6906  //claudia
#define IP_LOCALHOST           0x7f000001  //127.000.000.001

static int handler;
static pc_type comando,resposta;
#define TX_PRT  6103
#define RX_PRT  0
#define CLI_IPC_TIMEOUT 5

#define TX_IP   0x7F000001

#define PARAM_DEFINED(str) (str[0]!='\0')

#define CHECK_TIMEOUT(instruction)  \
    if ((instruction) < 0)          \
    {                               \
      printf("ERROR: TIMEOUT\r\n"); \
      return 0;                     \
    }

/**
 * Configure physical port 
 */
int phy_port_config(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_HWEthPhyConf_t *ptr;
  uint32 valued;

  if (!PARAM_DEFINED(values[0])) {
    printf("ERROR: Port parameter is mandatory!\r\n");
    return -1;
  }
  
  // Pointer to data array
  ptr = (msg_HWEthPhyConf_t *) &(comando.info[0]);

  // Slot id
  ptr->SlotId = (uint8)-1;

  // port
  if (StrToLong(values[0], &valued)<0) {
    printf("ERROR: Invalid port value!\r\n");
    return -1;
  }
  ptr->Port = (uint8) valued;

  // admin
  if (StrToLong(values[1], &valued)==0) {
    ptr->PortEnable = (uint8) valued;
    ptr->Mask |= 0x0020;
  }

  // speed
  if (StrToLong(values[2], &valued)==0) {
    ptr->Speed = (uint8) valued;
    ptr->Mask |= 0x0001;
  }
  
  // full_duplex
  if (StrToLong(values[3], &valued)==0) {
    ptr->Duplex = (uint8) valued;
    ptr->Mask |= 0x0004;
  }
  
  // frame_max length
  if (StrToLong(values[4], &valued)==0) {
    ptr->MaxFrame = (uint16) valued;
    ptr->Mask |= 0x0040;
  }
  
  comando.msgId = CCMSG_ETH_PHY_CONFIG_SET;
  comando.infoDim = sizeof(msg_HWEthPhyConf_t);

  /* Send message */
  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  /* Analyse answer */
  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
    printf(" Switch port configuration executed successfully\n\r");
  else
    printf(" Switch port configuration not executed - error %08x\n\r", *((unsigned int *)resposta.info));

  return 0;
}

/**
 * Get physical port configuration
 */
int phy_port_config_get(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_HwGenReq_t *ptr;
  msg_HWEthPhyConf_t *po;
  L7_uint index, n_index;
  uint32 valued;

  if (!PARAM_DEFINED(values[0])) {
    printf("ERROR: Port parameter is mandatory!\r\n");
    return -1;
  }

  // Pointer to data array
  ptr = (msg_HwGenReq_t *) &(comando.info[0]);
  memset(ptr,0x00,sizeof(msg_HwGenReq_t));

  // Slot id
  ptr->slot_id = (uint8)-1;

  // port
  if (StrToLong(values[0], &valued)<0) {
    printf("ERROR: Invalid port value!\r\n");
    return -1;
  }
  ptr->generic_id = (uint8) valued;

  comando.msgId = CCMSG_ETH_PHY_CONFIG_GET;
  comando.infoDim = sizeof(msg_HwGenReq_t);

  /* Send message */
  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  /* Get answer */
  po=(msg_HWEthPhyConf_t *) &resposta.info[0];

  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
  {
    printf("Switch: port(s) read successfully\n\r");

    n_index = resposta.infoDim / sizeof(msg_HWEthPhyConf_t);
    if ( n_index>PTIN_SYSTEM_MAX_N_PORTS )
      n_index=PTIN_SYSTEM_MAX_N_PORTS;

    for ( index=0; index<n_index; index++ ) {
      printf("Slot %u, Port %u configuration:\n\r",po[index].SlotId, po[index].Port);
      printf("\tSpeed         = %u\n\r"
             "\tMedia         = %u\n\r"
             "\tDuplex        = %u\n\r"
             "\tLoopback      = %u\n\r"
             "\tFlowControl   = %u\n\r"
             "\tPortEnable    = %u\n\r"
             "\tMaxFrame      = %u\n\r"
             "\tVlanAwareness = %u\n\r"
             "\tMacLearning   = %u\n\r"
             "\tAutoMDI       = %u\n\r",
             po[index].Speed,
             po[index].Media,
             po[index].Duplex,
             po[index].LoopBack,
             po[index].FlowControl,
             po[index].PortEnable,
             po[index].MaxFrame,
             po[index].VlanAwareness,
             po[index].MacLearning,
             po[index].AutoMDI );
    }
  }
  else
  {
    printf(" Switch: Error reading phy configurations\n\r");
  }

  return 0;
}


/**
 * Get physical port configuration
 */
int phy_port_state_get(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_HwGenReq_t *ptr;
  msg_HWEthPhyState_t *po;
  L7_uint index, n_index;
  uint32 valued;

  if (!PARAM_DEFINED(values[0])) {
    printf("ERROR: Port parameter is mandatory!\r\n");
    return -1;
  }

  // Pointer to data array
  ptr = (msg_HwGenReq_t *) &(comando.info[0]);
  memset(ptr,0x00,sizeof(msg_HwGenReq_t));

  // Slot id
  ptr->slot_id = (uint8)-1;

  // port
  if (StrToLong(values[0], &valued)<0) {
    printf("ERROR: Invalid port value!\r\n");
    return -1;
  }
  ptr->generic_id = (uint8) valued;

  comando.msgId = CCMSG_ETH_PHY_STATE_GET;
  comando.infoDim = sizeof(msg_HwGenReq_t);

  /* Send message */
  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  /* Get answer */
  po=(msg_HWEthPhyState_t *) &resposta.info[0];

  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
  {
    printf("Switch: port(s) read successfully\n\r");

    n_index = resposta.infoDim / sizeof(msg_HWEthPhyState_t);
    if (n_index>PTIN_SYSTEM_N_PORTS)
      n_index=PTIN_SYSTEM_N_PORTS;

    for ( index=0; index<n_index; index++ )
    {
      printf("Slot %u, Port %u eth state:\n\r",po[index].SlotId,po[index].Port);
      printf("\tSpeed             = %u\n\r"
             "\tDuplex            = %u\n\r"
             "\tCollisions        = %u\n\r"
             "\tRxActivity        = %u\n\r"
             "\tTxActivity        = %u\n\r"
             "\tLinkUp            = %u\n\r"
             "\tAutoNegComplete   = %u\n\r"
             "\tTxFault           = %u\n\r"
             "\tRemoteFault       = %u\n\r"
             "\tLOS               = %u\n\r"
             "\tMedia             = %u\n\r"
             "\tMTU_mismatch      = %u\n\r"
             "\tSupported_MaxFrame= %u\n\r",
             po[index].Speed,
             po[index].Duplex,
             po[index].Collisions,
             po[index].RxActivity,
             po[index].TxActivity,
             po[index].LinkUp,
             po[index].AutoNegComplete,
             po[index].TxFault,
             po[index].RemoteFault,
             po[index].LOS,
             po[index].Media,
             po[index].MTU_mismatch,
             po[index].Supported_MaxFrame );
    }
  }
  else
  {
    printf(" Switch: Error reading phy states\n\r");
  }

  return 0;
}

/**
 * Get physical port stats
 */
int phy_port_stat_get(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_HwGenReq_t *ptr;
  msg_HWEthRFC2819_PortStatistics_t *po;
  uint32 valued;
  L7_uint index, n_index;

  if (!PARAM_DEFINED(values[0])) {
    printf("ERROR: Port parameter is mandatory!\r\n");
    return -1;
  }

  // Pointer to data array
  ptr = (msg_HwGenReq_t *) &(comando.info[0]);
  memset(ptr,0x00,sizeof(msg_HwGenReq_t));

  // Slot id
  ptr->slot_id = (uint8)-1;

  // port
  if (StrToLong(values[0],&valued)<0) {
    printf("ERROR: Invalid port value!\r\n");
    return -1;
  }
  ptr->generic_id = (uint8) valued;

  comando.msgId = CCMSG_ETH_PHY_COUNTERS_GET;
  comando.infoDim = sizeof(msg_HwGenReq_t);

  /* Send message */
  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  /* Answer */
  po = (msg_HWEthRFC2819_PortStatistics_t *) &resposta.info[0];

  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
  {
    printf("Switch: port(s) stats read successfully\n\r");

    n_index = resposta.infoDim / sizeof(msg_HWEthRFC2819_PortStatistics_t);
    if ( n_index>PTIN_SYSTEM_MAX_N_PORTS )
      n_index=PTIN_SYSTEM_MAX_N_PORTS;

    for ( index=0; index<n_index; index++ )
    {
      printf( "Switch statistics of slot %u, port %u:\n\r"
              "\tRX.DropEvents          = %20llu\t\tTX.DropEvents          = %20llu\n\r"
              "\tRX.Octets              = %20llu\t\tTX.Octets              = %20llu\n\r"
              "\tRX.Pkts                = %20llu\t\tTX.Pkts                = %20llu\n\r"
              "\tRX.BroadcastPkts       = %20llu\t\tTX.BroadcastPkts       = %20llu\n\r"
              "\tRX.MulticastPkts       = %20llu\t\tTX.MulticastPkts       = %20llu\n\r"
              "\tRX.CRCAlignErrors      = %20llu\t\tTX.CRCAlignErrors      = %20llu\n\r"
              "\tRX.UndersizePkts       = %20llu\t\tTX.Collisions          = %20llu\n\r"
              "\tRX.OversizePkts        = %20llu\t\tTX.OversizePkts        = %20llu\n\r"
              "\tRX.Fragments           = %20llu\t\tTX.Fragments           = %20llu\n\r"
              "\tRX.Jabbers             = %20llu\t\tTX.Jabbers             = %20llu\n\r"
              "\tRX.Pkts64octets        = %20llu\t\tTX.Pkts64octets        = %20llu\n\r"
              "\tRX.Pkts65to127octets   = %20llu\t\tTX.Pkts65to127octets   = %20llu\n\r"
              "\tRX.Pkts128to255octets  = %20llu\t\tTX.Pkts128to255octets  = %20llu\n\r"
              "\tRX.Pkts256to511octets  = %20llu\t\tTX.Pkts256to511octets  = %20llu\n\r"
              "\tRX.Pkts512to1023octets = %20llu\t\tTX.Pkts512to1023octets = %20llu\n\r"
              "\tRX.Pkts1024to1518octets= %20llu\t\tTX.Pkts1024to1518octets= %20llu\n\r"
              "\tRX.Pkts1519toMaxOctets = %20llu\t\tTX.Pkts1519toMaxOctets = %20llu\n\r"
              "\tRX.Throughput          = %20llu\t\tTX.Throughput          = %20llu\n\r",
              po[index].SlotId,po->Port,
              po[index].Rx.etherStatsDropEvents          , po[index].Tx.etherStatsDropEvents          ,
              po[index].Rx.etherStatsOctets              , po[index].Tx.etherStatsOctets              ,
              po[index].Rx.etherStatsPkts                , po[index].Tx.etherStatsPkts                ,
              po[index].Rx.etherStatsBroadcastPkts       , po[index].Tx.etherStatsBroadcastPkts       ,
              po[index].Rx.etherStatsMulticastPkts       , po[index].Tx.etherStatsMulticastPkts       ,
              po[index].Rx.etherStatsCRCAlignErrors      , po[index].Tx.etherStatsCRCAlignErrors      ,
              po[index].Rx.etherStatsUndersizePkts       , po[index].Tx.etherStatsCollisions          ,
              po[index].Rx.etherStatsOversizePkts        , po[index].Tx.etherStatsOversizePkts        ,
              po[index].Rx.etherStatsFragments           , po[index].Tx.etherStatsFragments           ,
              po[index].Rx.etherStatsJabbers             , po[index].Tx.etherStatsJabbers             ,
              po[index].Rx.etherStatsPkts64Octets        , po[index].Tx.etherStatsPkts64Octets        ,
              po[index].Rx.etherStatsPkts65to127Octets   , po[index].Tx.etherStatsPkts65to127Octets   ,
              po[index].Rx.etherStatsPkts128to255Octets  , po[index].Tx.etherStatsPkts128to255Octets  ,
              po[index].Rx.etherStatsPkts256to511Octets  , po[index].Tx.etherStatsPkts256to511Octets  ,
              po[index].Rx.etherStatsPkts512to1023Octets , po[index].Tx.etherStatsPkts512to1023Octets ,
              po[index].Rx.etherStatsPkts1024to1518Octets, po[index].Tx.etherStatsPkts1024to1518Octets,
              po[index].Rx.etherStatsPkts1519toMaxOctets , po[index].Tx.etherStatsPkts1519toMaxOctets ,
              po[index].Rx.Throughput                    , po[index].Tx.Throughput                    );
    }
  }
  else
  {
    printf(" Switch: Error reading port statistics\n\r");
  }

  return 0;
}

/**
 * Clear physical port stats
 */
int phy_port_stat_clear(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_HWEthRFC2819_PortStatistics_t *ptr;
  uint32 valued;

  if (!PARAM_DEFINED(values[0])) {
    printf("ERROR: Port parameter is mandatory!\r\n");
    return -1;
  }

  // Pointer to data array
  ptr = (msg_HWEthRFC2819_PortStatistics_t *) &(comando.info[0]);

  // Slot id
  ptr->SlotId = (uint8)-1;

  // port
  if (StrToLong(values[0],&valued)<0) {
    printf("ERROR: Invalid port value!\r\n");
    return -1;
  }
  ptr->Port = (uint8) valued;

  comando.msgId = CCMSG_ETH_PHY_COUNTERS_CLEAR;
  comando.infoDim = sizeof(msg_HWEthRFC2819_PortStatistics_t);

  /* Send message */
  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
    printf(" Switch: Port statistics cleared\n\r");
  else
    printf(" Switch: Error clearing port statistics - error %08x\n\r", *(unsigned int*)resposta.info);

  return 0;
}

/**
 * Redirect stdout (dbg)
 * 
 * @return int 
 */
int redirect_stdout(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  char *tty_name;
  uint16 len;

  tty_name = (PARAM_DEFINED(values[0])) ? values[0] : ttyname(1);

  len = strlen((char *) tty_name);
  if (len>100) {
    printf("ERROR: tty device name too long!\n");
    return -1;
  }

  strcpy((char *) &comando.info[0], tty_name );

  comando.msgId = CCMSG_APP_CHANGE_STDOUT;
  comando.infoDim = strlen(tty_name)+1;

  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
    printf(" Stdout redirected successfully\n\r");
  else
    printf(" Error redirecting Stdout - error %08x\n\r", *(unsigned int*)resposta.info);

  return 0;
}

/**
 * Redirect logger to file
 * 
 * @return int 
 */
int redirect_logger(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  char *filename = NULL;
  uint8  output = 0;
  uint16 len = 0;
  uint32 valued;

  // Output index
  if (StrToLong(values[0], &valued) == 0) {
    output = valued;
  }
  /* Filename */
  if (PARAM_DEFINED(values[1]))
  {
    filename = values[1];
    len = strlen(filename);
  }
  
  /* Initial size is zero chars */
  comando.infoDim = 0;

  comando.info[0] = output;
  comando.infoDim += 1;

  /* File */
  if (filename!=NULL && len>0)
  {
    /* Validate length */
    if (len>100) {
      printf("File name too long\n");
    }

    strncpy((char *) &comando.info[1], filename, 101 );
    comando.info[1+100] = '\0';
    printf("Going to use \"%s\" filename...\n",&comando.info[1]);

    /* Consider also the null character */
    comando.infoDim += len+1;
  }
  else
  {
    /* Stdout direction */
    comando.info[1] = '\0';
    comando.infoDim += 1;
  }

  comando.msgId = CCMSG_APP_LOGGER_OUTPUT;

  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
    printf(" Logger redirected successfully\n\r");
  else
    printf(" Error redirecting logger - error %08x\n\r", *(unsigned int*)resposta.info);

  return 0;
}

/**
 * Redirect logger to file
 * 
 * @return int 
 */
int board_show(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_FWFastpathInfo *fpInfo;

  comando.infoDim = 0;
  comando.msgId = CCMSG_BOARD_SHOW;

  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  /* Answer */
  fpInfo = (msg_FWFastpathInfo *) &resposta.info[0];

  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))  {
    printf("SlotIndex = %u\r\n"
           "\tBoardPresent     = %lu\r\n"
           "\tBoardNInterfaces = %lu\r\n"
           "\tBoardVersion     = %lu\r\n"
           "\tBoardMode        = %lu\r\n"
           "\tBoardSerialNumber= \"%.19s\"\r\n"
           "\tFpgaFixoID       = 0x%04lx (version %lu)\r\n"
           "\tFpgaFlexID       = 0x%04lx (version %lu)\r\n"
           "\tPCBVersion       = %lu\r\n",
           fpInfo->SlotIndex,
           fpInfo->BoardPresent,
           fpInfo->BoardNInterfaces,
           fpInfo->BoardVersion,
           fpInfo->BoardMode,
           fpInfo->BoardSerialNumber,
           fpInfo->FpgaFixoID, fpInfo->FpgaFixoVersion,
           fpInfo->FpgaFlexID, fpInfo->FpgaFlexVersion,
           fpInfo->PCBVersion);
  }
  else {
    printf(" Switch: Error reading board information\n\r");
  }

  return 0;
}

/**
 * Ping switchdrvr
 * 
 * @return int 
 */
int fp_ping(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  uint32 valued;
  uint32 period = 20,
         nretries = 6,
         ret = 255;

  // Period
  if (StrToLong(values[0], &valued) == 0) {
    period = valued;
  }
  // #Retries
  if (StrToLong(values[1], &valued) == 0) {
    nretries = valued;
  }

  printf("period=%lu #retries=%lu\r\n", period, nretries);

  close_ipc(handler);
  handler=open_ipc(PORTO_TX_MSG_BUGA,IP_LOCALHOST, NULL, period);
  if ( handler < 0 )
  {
    printf("Erro no open IPC do BUGA...\n\r");
    return -1;
  }

  comando.counter      = rand ();
  comando.msgId        = CCMSG_APPLICATION_IS_ALIVE;
  comando.infoDim      = sizeof(unsigned int);
  *(int*)comando.info  = 0;

  printf("Checking if OLTSWITCH is alive...\n\r");
  do {
    // Send command
    valued = send_data (handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta);
    ret = *((int *)resposta.info);
    if ( valued ) {
      printf("Without answer from OLTSWITCH... probably it is still starting\n\r");
    }
    else if (resposta.flags != (FLAG_RESPOSTA | FLAG_ACK)) {
      printf("Invalid answer... Request not Aknowledged\n\r");
      sleep(period);
    }
    else if (resposta.infoDim == sizeof(int) && ret == 1) {
      printf("OLTSWITCH replied... Application is still loading!\n\r");
      sleep(period);
    }
    else
      break;
  } while ((--nretries) > 0);

  if (nretries > 0) {
    if (ret == 0) {
      printf("OLTSWITCH replied... Application is up!\n\r");
      exit(ret);
    }
    else if (ret == 2) {
      printf("OLTSWITCH replied... Application has CRASHED!\n\r");
      exit(ret);
    }
    else {
      printf("OLTSWITCH replied... return code is unknown: %d\n\r", (int)ret);
      exit(ret);
    }
  }
  else if (resposta.infoDim == sizeof(int) && ret == 1)
    exit(ret);

  printf("OLTSWITCH did not reply... Timeout!\n\r");

  exit(-2);
}


/**
 * Create EVC 
 */
int evc_create(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_HwEthMef10Evc_t *ptr;
  uint32 valued;
  int intf_type, intf_id, mef_type, ovid, ivid;

  /* EVC id parameter is mandatory */
  if (!PARAM_DEFINED(values[0])) {
    printf("ERROR: EVC id parameter is mandatory!\r\n");
    return -1;
  }

  // Pointer to data array
  ptr = (msg_HwEthMef10Evc_t *) &(comando.info[0]);
  memset(ptr, 0x00, sizeof(msg_HwEthMef10Evc_t));

  // Slot id
  ptr->SlotId = (uint8)-1;

  // EVC id (mandatory)
  if (StrToLong(values[0], &valued) < 0) {
    printf("ERROR: Invalid EVC id!\r\n");
    return -1;
  }
  ptr->id = valued;

  /* EVC type (optional) */
  if (StrToLong(values[1], &valued) == 0)
  {
    ptr->flags |= (valued != 0) ? ((valued << 16) & 0x30000) : 0;
  }
  else
  {
    printf("Assuming EVC as P2MP type.\r\n");
  }

  /* Stacked (optional) */
  if (StrToLong(values[2], &valued) == 0)
  {
    ptr->flags |= valued != 0 ? 0x0004 : 0;
  }
  {
    printf("Assuming EVC as unstacked.\r\n");
  }
  
  // MAC Learning (optional)
  if (StrToLong(values[3], &valued) == 0)
  {
    ptr->flags |= (valued != 0) ? 0x0008 : 0;
  }
  else
  {
    ptr->flags |= 0x0008;
    printf("Assuming EVC with MAC learning enabled.\r\n");
  }
  
  // MC Flood type (optional)
  if (StrToLong(values[4], &valued) == 0)
  {
    ptr->mc_flood = valued;
  }
  else
  {
    printf("Assuming MC flood disabled.\r\n");
  }

  // Other masks (optional)
  if (StrToLong(values[5], &valued) == 0)
  {
    ptr->flags |= valued;
    printf("Adding mask 0x%lx to flags: 0x%lx.\r\n", valued, ptr->flags);
  }

  /* Zero interfaces */
  ptr->n_intf = 0;

  // Interfaces (optional)
  if (sscanf(values[6], "%d/%d/%d/%d/%d", &intf_type, &intf_id, &mef_type, &ovid, &ivid) == 5)
  {
    ptr->intf[ptr->n_intf].intf_type = intf_type;
    ptr->intf[ptr->n_intf].intf_id   = intf_id;
    ptr->intf[ptr->n_intf].mef_type  = mef_type;
    ptr->intf[ptr->n_intf].vid       = ovid;
    ptr->intf[ptr->n_intf].inner_vid = ivid;
    ptr->n_intf++;
  }
  // Interfaces (optional)
  if (sscanf(values[7], "%d/%d/%d/%d/%d", &intf_type, &intf_id, &mef_type, &ovid, &ivid) == 5)
  {
    ptr->intf[ptr->n_intf].intf_type = intf_type;
    ptr->intf[ptr->n_intf].intf_id   = intf_id;
    ptr->intf[ptr->n_intf].mef_type  = mef_type;
    ptr->intf[ptr->n_intf].vid       = ovid;
    ptr->intf[ptr->n_intf].inner_vid = ivid;
    ptr->n_intf++;
  }

  comando.msgId = CCMSG_ETH_EVC_ADD;
  comando.infoDim = sizeof(msg_HwEthMef10Evc_t);

  /* Send message */
  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  /* Analyse answer */
  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
    printf(" EVC created successfully\n\r");
  else
    printf(" Failed creating EVC - error %08x\n\r", *(unsigned int*)resposta.info);

  return 0;
}

/**
 * Remove EVC 
 */
int evc_remove(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_HwEthMef10EvcRemove_t *ptr;
  uint32 valued;

  /* EVC id parameter is mandatory */
  if (!PARAM_DEFINED(values[0])) {
    printf("ERROR: Port parameter is mandatory!\r\n");
    return -1;
  }

  // Pointer to data array
  ptr = (msg_HwEthMef10EvcRemove_t *) &(comando.info[0]);
  memset(ptr, 0x00, sizeof(msg_HwEthMef10EvcRemove_t));

  // Slot id
  ptr->SlotId = (uint8)-1;

  // EVC id (mandatory)
  if (StrToLong(values[0], &valued)<0) {
    printf("ERROR: Invalid EVC id!\r\n");
    return -1;
  }
  ptr->id = valued;

  comando.msgId = CCMSG_ETH_EVC_REMOVE;
  comando.infoDim = sizeof(msg_HwEthMef10EvcRemove_t);

  /* Send message */
  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  /* Analyse answer */
  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
    printf(" EVC removed successfully\n\r");
  else
    printf(" Failed removing EVC - error %08x\n\r", *(unsigned int*)resposta.info);

  return 0;
}

/**
 * Add interface to EVC
 */
int evc_intf_add(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_HWevcPort_t *ptr;
  int type, intf;
  uint32 valued;

  /* EVC id is mandatory */
  if (!PARAM_DEFINED(values[0])) {
    printf("ERROR: EVC id param is mandatory!\r\n");
    return -1;
  }
  /* Port is mandatory */
  if (!PARAM_DEFINED(values[1])) {
    printf("ERROR: Interface param is mandatory!\r\n");
    return -1;
  }

  // Pointer to data array
  ptr = (msg_HWevcPort_t *) &(comando.info[0]);
  memset(ptr, 0x00, sizeof(msg_HWevcPort_t));

  // Slot id
  ptr->slotId = (uint8)-1;

  // EVC id (mandatory)
  if (StrToLong(values[0], &valued)<0) {
    printf("ERROR: Invalid EVC id value!\r\n");
    return -1;
  }
  ptr->evcId = valued;

  // Interfaces (mandatory)
  if (sscanf(values[1], "%d/%d", &type, &intf) != 2)
  {
    printf("ERROR: Invalid interface (type/id)!\r\n");
    return -1;
  }
  ptr->intf.intf_type = type;
  ptr->intf.intf_id   = intf;

  /* MEF type (optional) */
  if (StrToLong(values[2], &valued) == 0)
  {
    ptr->intf.mef_type = (uint8) valued;
  }
  else
  {
    ptr->intf.mef_type = 0;
    printf("Assuming mef type as root interface!\r\n");
  }

  /* Outer VLAN */
  if (StrToLong(values[3], &valued) == 0)
  {
    ptr->intf.vid = (uint16) valued;
  }
  else
  {
    ptr->intf.vid = 0;
  }

  /* Inner VLAN (optional) */
  if (StrToLong(values[4], &valued) == 0)
  {
    ptr->intf.inner_vid = (uint16) valued;
  }
  else
  {
    ptr->intf.inner_vid = 0;
  }

  /* PCP (optional) */
  if (StrToLong(values[5], &valued) == 0)
  {
    ptr->intf.pcp = ((uint8) valued & 0x7) | 0x10;
  }
  else
  {
    ptr->intf.pcp = 0;
  }

  /* EtherType (optional) */
  if (StrToLong(values[6], &valued) == 0)
  {
    ptr->intf.ethertype = (int16) valued;
  }
  else
  {
    ptr->intf.ethertype = -1;
  }

  comando.msgId = CCMSG_ETH_EVC_PORT_ADD;
  comando.infoDim = sizeof(msg_HWevcPort_t);

  /* Send message */
  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  /* Analyse answer */
  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
    printf(" Port added successfully to EVC\n\r");
  else
    printf(" Failed adding port to EVC - error %08x\n\r", *(unsigned int*)resposta.info);

  return 0;
}

/**
 * Removed interface from EVC
 */
int evc_intf_remove(int nargs, char (*values)[MAX_PARAM_VALUE_SIZE])
{
  msg_HWevcPort_t *ptr;
  int type, intf;
  uint32 valued;

  /* EVC id is mandatory */
  if (!PARAM_DEFINED(values[0])) {
    printf("ERROR: EVC id param is mandatory!\r\n");
    return -1;
  }
  /* Port is mandatory */
  if (!PARAM_DEFINED(values[1])) {
    printf("ERROR: Interface param is mandatory!\r\n");
    return -1;
  }

  // Pointer to data array
  ptr = (msg_HWevcPort_t *) &(comando.info[0]);
  memset(ptr, 0x00, sizeof(msg_HWevcPort_t));

  // Slot id
  ptr->slotId = (uint8)-1;

  // EVC id (mandatory)
  if (StrToLong(values[0], &valued)<0) {
    printf("ERROR: Invalid EVC id value!\r\n");
    return -1;
  }
  ptr->evcId = valued;

  // Interfaces (mandatory)
  if (sscanf(values[1], "%d/%d", &type, &intf) != 2)
  {
    printf("ERROR: Invalid interface (type/id)!\r\n");
    return -1;
  }
  ptr->intf.intf_type = type;
  ptr->intf.intf_id   = intf;

  comando.msgId = CCMSG_ETH_EVC_PORT_REMOVE;
  comando.infoDim = sizeof(msg_HWevcPort_t);

  /* Outer VLAN */
  if (StrToLong(values[2], &valued) == 0)
  {
    ptr->intf.vid = (uint16) valued;
  }
  else
  {
    ptr->intf.vid = 0;
  }

  /* Inner VLAN (optional) */
  if (StrToLong(values[3], &valued) == 0)
  {
    ptr->intf.inner_vid = (uint16) valued;
  }
  else
  {
    ptr->intf.inner_vid = 0;
  }

  /* PCP (optional) */
  if (StrToLong(values[4], &valued) == 0)
  {
    ptr->intf.pcp = ((uint8) valued & 0x7) | 0x10;
  }
  else
  {
    ptr->intf.pcp = 0;
  }

  /* EtherType (optional) */
  if (StrToLong(values[5], &valued) == 0)
  {
    ptr->intf.ethertype = (int16) valued;
  }
  else
  {
    ptr->intf.ethertype = 0;
  }

  /* Send message */
  CHECK_TIMEOUT(send_data(handler, PORTO_RX_MSG_BUGA, IP_LOCALHOST, &comando, &resposta));

  /* Analyse answer */
  if (resposta.flags == (FLAG_RESPOSTA | FLAG_ACK))
    printf(" Port removed successfully from EVC\n\r");
  else
    printf(" Failed removing port from EVC - error %08x\n\r", *(unsigned int*)resposta.info);

  return 0;
}


int main(int argc, const char **argv) {

  init_ipc_lib();
  handler=open_ipc(PORTO_TX_MSG_BUGA, IP_LOCALHOST, NULL,CLI_IPC_TIMEOUT);

  comando.protocolId   = 1;
  comando.srcId        = PORTO_TX_MSG_BUGA;
  comando.dstId        = PORTO_RX_MSG_BUGA;
  comando.flags        = (FLAG_COMANDO); //(IPCLIB_FLAGS_CMD | (IPC_UID<<4));
  comando.counter      = rand ();
  comando.msgId        = 0;
  comando.infoDim      = sizeof(unsigned int);
  *(int*)comando.info  = 0;

  cli_unicom(argc, argv);

  close_ipc(handler);
  return 0;
}//main

