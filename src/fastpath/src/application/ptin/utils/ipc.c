//#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "logger.h"
#include "sirerrors.h"
#include "traces.h"
#include "ipc.h"
#include "ptin_msghandler.h"
#include "sirerrors.h"
#include "ptin_intf.h"
#include "ptin_fpga_api.h"

/* Slot id */
uint8 ptin_board_slotId = 0;

static int g_iInterfaceSW  = -1;      // Canal de dados com as aplicacoes cliente
static int g_iInterfaceHW  = -1;      // Canal de dados com as aplicacoes firmware
static int g_iInterfaceCX  = -1;      // Canal de dados com as aplicacoes firmware
static int g_iInterfaceMan = -1;      // Canal de dados com as aplicacoes firmware

static  int g_iCCounter   = 0;      //


#define EQUIPID_OLT 0x10000

typedef struct _st_alarmGeral {
  unsigned char  SlotId      ;  // Slot id
  unsigned char  trapSrc;    ;  // Trap source 
  unsigned char  oltiftype   ;  // Interface type: Physical:0, LAG:1
  unsigned int   oltifindex  ;  // Pon [0..7], Eth [8..15], Eth(10G)
  unsigned int   equipid     ;  // 0x10000-OLT, 0..128-ONT (onu_Id)
  unsigned int   equipifindex;  // [1..65535] (apenas usado nos ONTs (equipid!=0), índice do porto do ONT.
  unsigned int   alarmtype   ;  // Interfaces (TRAP_TYPE_INTERFACE = 3)
  unsigned int   alarmcode   ;  // TRAP_CODE_XXX
  unsigned int   alarmstatus ;  // 0=Fim ; 1=Inicio ; 2=Evento
  unsigned int   param1      ;
  unsigned int   param2      ;
} __attribute__((packed)) st_alarmGeral;


typedef struct
{
   BYTE  slotId;
   UINT  code;          // CHTRAP_CODE_MAC_LIMIT
   UINT  alarmstatus;   // 0=Fim ; 1=Inicio ; 2=Evento
   UINT  evcId;         // unused
   BYTE  intf_type;     // PTIN_EVC_INTF_PHYSICAL
   BYTE  intf_id;       // interface
   UWORD nni_cvlan;     // unused
   UWORD outer_vid;     // GEM ID
   UWORD inner_vid;     // unused
   BYTE  mac[6];        // unused
   UINT  flags;         // unused
} __attribute__((packed)) st_EthSwitchEvent;


typedef struct{
      union {
          unsigned char mac[6];             // MAC Address
          unsigned long ipv4Addr;           // IPv4 Address
          unsigned long ipv6Addr[4];        // IPv6 Address
      } __attribute__((packed)) inetAddr;

      unsigned long long seconds;           // Seconds field of timestamp
      unsigned long nanoseconds;            // nanoseconds field of timestamp

      unsigned char encap;                  // Type of encapsulation
      unsigned char messageType;            // message type
      unsigned short sequenceId;            // sequence ID
      unsigned char domainNumber;           // Domain Number
} __attribute__((packed)) st_PtpTsInfo;     // struct used to pass Tx Timestamps info to API



/* Initialize server ip */
static void ipc_server_ipaddr_init(void);

// ----------------------------------------------------
// Interface Publica
// ----------------------------------------------------

   int  OpenIPC	(void)
   {
      int res = S_OK;
      int pid;

      /* Initialize server ip */
      ipc_server_ipaddr_init();

      LOG_INFO(LOG_CTX_IPC,
                  "Vai iniciar o modulo de comunicacoes IPC."); 
      init_ipc_lib ();

      if ((res=open_ipc (IPC_HW_FASTPATH_PORT, INADDR_ANY, &CHMessageHandler, IPC_CH_TIMEOUT, &g_iInterfaceHW))!=S_OK)
      {
        LOG_CRITICAL(LOG_CTX_IPC,
                     "Nao foi possivel abrir o canal de comunicacoes para atendimento de mensagens (%08X).", res);
        return res;
      }
     
      if ((res=open_ipc (IPC_HW_FP_CTRL_PORT, INADDR_ANY, &CHMessageHandler, IPC_CH_TIMEOUT, &g_iInterfaceCX))!=S_OK)
      {
        LOG_CRITICAL(LOG_CTX_IPC,
                     "Nao foi possivel abrir o canal de comunicacoes para atendimento de mensagens (%08X).", res);
        close_ipc(g_iInterfaceHW);
        return res;
      }

      if ((res=open_ipc (0, 0, NULL, IPC_CM_TIMEOUT, &g_iInterfaceSW))!=S_OK)
      {
        LOG_INFO(LOG_CTX_IPC,
                 "Nao foi possivel abrir o canal para o envio de mensagens (%08X).", res);
        close_ipc (g_iInterfaceHW);
        close_ipc (g_iInterfaceCX);
        return res;
      }

      pid = (int)getpid();
      g_iCCounter = (pid&0x0000FFFF)<<16;
      LOG_INFO(LOG_CTX_IPC,
                  "Modulo de comunicacoes IPC iniciado com sucesso."); 
      return S_OK;
   } // OpenIPC (V1.0.0.060622)

   void CloseIPC	(void)
   {
     LOG_INFO(LOG_CTX_IPC,
              "Vai fechar o modulo de comunicacoes IPC."); 
      if (g_iInterfaceSW>=0)
         close_ipc (g_iInterfaceSW);
      if (g_iInterfaceHW>=0)
         close_ipc (g_iInterfaceHW);
      if (g_iInterfaceCX>=0)
         close_ipc (g_iInterfaceCX);
      if (g_iInterfaceMan>=0)
         close_ipc (g_iInterfaceMan);
      LOG_INFO(LOG_CTX_IPC,
               "Modulo de comunicacoes IPC fechado com sucesso."); 
   } // CloseIPC (V1.0.0.060622)

   void EnableHandling (BOOLEAN enable)
   {
     LOG_INFO(LOG_CTX_IPC,
              "(Des)activacao do processamento de mensagens do modulo de comunicacoes IPC (%d).", enable); 
   } // EnableHandling (V1.0.0.060622)

   // --------------------------------------------------------------
   // Função:     void SwapIPCHeader (pc_type *in, pc_type *out)
   // Parametros: in  - buffer que contém uma mensagem recebida
   //             out - buffer que contém uma mensagem a enviar
   // Retorno:
   //
   // Descrição:  Preenche o cabeçalho da mensagem de saída por
   //             forma a corresponder a resposta da mensagem de
   //             entrada.
   // --------------------------------------------------------------
   void SwapIPCHeader (ipc_msg *inbuffer, ipc_msg *outbuffer)
   {
      outbuffer->protocolId  = inbuffer->protocolId;
      outbuffer->msgId       = inbuffer->msgId;
      outbuffer->dstId       = inbuffer->srcId;
      outbuffer->srcId       = inbuffer->dstId;
      //outbuffer->pageSize    = 0;
      outbuffer->counter     = inbuffer->counter;
      outbuffer->flags       = IPCLIB_FLAGS_ACK;   // por omissao corre bem, caso contrario é trocada depois
   } // SwapIPCHeader (V1.0.0.060622)

   void SetIPCNACK (ipc_msg *outbuffer, int res)
   {
      outbuffer->flags = IPCLIB_FLAGS_NACK;
      outbuffer->infoDim = sizeof (UINT);
      *(unsigned int*)outbuffer->info = res;
   } // SetIPCNACK (V1.0.0.060622)


/* Envia trap */
int send_trap(int porto, int trap_type, int arg)
{
  ipc_msg	comando;
  int ret;

//if (!global_var_system_ready)  return S_OK;

  if(g_iInterfaceSW==-1) 
      return(-1);

  comando.protocolId  = SIR_IPCPROTOCOL_ID;
  comando.flags		  = IPCLIB_FLAGS_CMD;
  comando.counter     = GetMsgCounter ();
  comando.msgId		  = trap_type;

  if (arg==-1)
     comando.infoDim = 0;
  else {
     comando.infoDim = sizeof(int);
      *((int *)comando.info) = arg;
  }

  ret=send_data(g_iInterfaceSW, IPC_CHMSG_TRAP_PORT, IPC_SERVER_IPADDR, (ipc_msg *)&comando, (ipc_msg *)NULL);
  if(ret<0)
      LOG_ERR(LOG_CTX_IPC,"SENDTRAP to PORT %d (Canal =%d), Code = 0x%.4x, arg = 0x%08x (%d): ERROR = %d", IPC_CHMSG_TRAP_PORT, g_iInterfaceSW, trap_type,arg,arg, ret);
  else
      LOG_TRACE(LOG_CTX_IPC,"SENDTRAP to PORT %d (Canal =%d), Code = 0x%.4x: arg = 0x%08x (%d)", IPC_CHMSG_TRAP_PORT, g_iInterfaceSW, trap_type,arg,arg);  
  return(ret);
}


/* Envia trap */
int send_trap_intf_alarm(unsigned char intfType, int porto, int code, int status, int param)
{
  ipc_msg	comando;
  int ret;
  st_alarmGeral *alarm;

//if (!global_var_system_ready)  return S_OK;

  if(g_iInterfaceSW==-1) 
      return(-1);

  comando.protocolId= SIR_IPCPROTOCOL_ID;
  comando.flags		= IPCLIB_FLAGS_CMD;
  comando.counter   = GetMsgCounter ();
  comando.msgId		= TRAP_ALARME_INTF;
  comando.infoDim   = sizeof(st_alarmGeral);
  alarm             = (st_alarmGeral *) &comando.info[0];

  memset(alarm,0x00,sizeof(st_alarmGeral));
  alarm->SlotId      = ptin_fpga_board_slot_get();
  alarm->trapSrc     = ERROR_FAMILY_HARDWARE;
  alarm->oltiftype   = intfType;
  alarm->oltifindex  = porto;
  alarm->equipid     = EQUIPID_OLT;
  alarm->alarmtype   = TRAP_ALARM_TYPE_INTERFACE;
  alarm->alarmcode   = code;
  alarm->alarmstatus = status;
  alarm->param1      = param;
  alarm->param2      = 0;

  ret=send_data(g_iInterfaceSW, IPC_CHMSG_TRAP_PORT, IPC_SERVER_IPADDR, (ipc_msg *)&comando, (ipc_msg *)NULL);
  if(ret<0)
      LOG_ERR(LOG_CTX_IPC,"SENDTRAP to PORT %d: interface=%d, Code = 0x%.4x, status = %d: ERROR = %d", IPC_CHMSG_TRAP_PORT, porto, code, status, ret);
  return(ret);
}

int send_trap_gen_alarm(unsigned char intfType, int porto, int code, int status, int param1, int param2)
{
  ipc_msg	comando;
  int ret;
  st_alarmGeral *alarm;

//if (!global_var_system_ready)  return S_OK;

  if(g_iInterfaceSW==-1) 
      return(-1);

  comando.protocolId= SIR_IPCPROTOCOL_ID;
  comando.flags		= IPCLIB_FLAGS_CMD;
  comando.counter   = GetMsgCounter ();
  comando.msgId		= TRAP_ALARME_SWITCH;
  comando.infoDim   = sizeof(st_alarmGeral);
  alarm             = (st_alarmGeral *) &comando.info[0];

  memset(alarm,0x00,sizeof(st_alarmGeral));
  alarm->trapSrc     = ERROR_FAMILY_HARDWARE;
  alarm->oltiftype   = intfType;
  alarm->oltifindex  = porto;
  alarm->alarmtype   = TRAP_ALARM_TYPE_INTERFACE;
  alarm->alarmcode   = code;
  alarm->alarmstatus = status;
  alarm->param1      = param1;
  alarm->param2      = param2;

  ret=send_data(g_iInterfaceSW, IPC_CHMSG_TRAP_PORT, IPC_SERVER_IPADDR, (ipc_msg *)&comando, (ipc_msg *)NULL);
  if(ret<0)
      LOG_ERR(LOG_CTX_IPC,"SENDTRAP to PORT %d: interface=%d, Code = 0x%.4x, status = %d: ERROR = %d", IPC_CHMSG_TRAP_PORT, porto, code, status, ret);
  return(ret);
}

/* Trap related to Switch Events */
int send_trap_switch_event(unsigned char intfType, int interface, int code, int status, int param)
{
  ipc_msg	comando;
  int ret = 0;
  st_EthSwitchEvent *alarm;
  L7_uint16 slot_to_send;
  ptin_intf_t ptin_intf;

//if (!global_var_system_ready)  return S_OK;

  if(g_iInterfaceSW==-1) 
      return(-1);

  /* Which slot to send? */
  ptin_intf.intf_type = intfType;
  ptin_intf.intf_id   = interface;

  if (ptin_intf_ptintf2SlotPort(&ptin_intf, &slot_to_send, L7_NULLPTR, L7_NULLPTR)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_IPC,"Unable to determine slot to send trap (port=%u)", interface);
    return -1;
  }

  LOG_TRACE(LOG_CTX_IPC,"%d",slot_to_send);

  comando.protocolId= SIR_IPCPROTOCOL_ID;
  comando.flags		= IPCLIB_FLAGS_CMD;
  comando.counter   = GetMsgCounter ();
  comando.msgId		= TRAP_ALARME_SWITCH;
  comando.infoDim   = sizeof(st_EthSwitchEvent);
  alarm             = (st_EthSwitchEvent *) &comando.info[0];

  memset(alarm,0x00,sizeof(st_EthSwitchEvent));

  if(PTIN_BOARD == PTIN_BOARD_CXO640G)
  {
    alarm->slotId      = slot_to_send;
  }
  else
  {
    alarm->slotId      = ptin_fpga_board_slot_get();
  }

  alarm->code        = code;
  alarm->alarmstatus = status;
  alarm->intf_type   = intfType;
  alarm->intf_id     = interface;
  alarm->outer_vid   = param;

  LOG_TRACE(LOG_CTX_IPC,"SENDTRAP to PORT %d: interface=%d, Code = 0x%.4x, status = %d, param = %d: ERROR = %d %d", IPC_CHMSG_TRAP_PORT, interface, code, status, param, ret, alarm->slotId);

  ret=send_data(g_iInterfaceSW, IPC_CHMSG_TRAP_PORT, IPC_SERVER_IPADDR, (ipc_msg *)&comando, (ipc_msg *)NULL);
  if(ret<0)
      LOG_ERR(LOG_CTX_IPC,"SENDTRAP to PORT %d: interface=%d, Code = 0x%.4x, status = %d: ERROR = %d", IPC_CHMSG_TRAP_PORT, interface, code, status, ret);
  return(ret);
}


/* Envia trap to linecard */
int send_trap_to_linecard(unsigned char intfType, int porto, int code, int status, int param)
{
  ipc_msg	comando;
  int ret;
  st_alarmGeral *alarm;
  L7_uint16 slot_to_send;
  ptin_intf_t ptin_intf;
  L7_uint32 ipaddr;

//if (!global_var_system_ready)  return S_OK;

  if(g_iInterfaceSW==-1) 
      return(-1);

  /* Which slot to send? */
  ptin_intf.intf_type = intfType;
  ptin_intf.intf_id   = porto;
  if (ptin_intf_ptintf2SlotPort(&ptin_intf, &slot_to_send, L7_NULLPTR, L7_NULLPTR)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_IPC,"Unable to determine slot to send trap (port=%u)", porto);
    return -1;
  }
  /* Which ipaddr? */
  ipaddr = (IPC_SERVER_IPADDR & (~((L7_uint32) 0xff))) | ((slot_to_send+2) & 0xff);

  comando.protocolId= SIR_IPCPROTOCOL_ID;
  comando.flags		= IPCLIB_FLAGS_CMD;
  comando.counter   = GetMsgCounter ();
  comando.msgId		= TRAP_LINECARD_INTF;
  comando.infoDim   = sizeof(st_alarmGeral);
  alarm             = (st_alarmGeral *) &comando.info[0];

  memset(alarm,0x00,sizeof(st_alarmGeral));
  alarm->SlotId      = slot_to_send;
  alarm->trapSrc     = ERROR_FAMILY_HARDWARE;
  alarm->oltiftype   = intfType;
  alarm->oltifindex  = porto;
  alarm->alarmtype   = TRAP_LC_TYPE_INTERFACE;
  alarm->alarmcode   = code;
  alarm->alarmstatus = status;
  alarm->param1      = param;
  alarm->param2      = 0;

  ret=send_data(g_iInterfaceSW, IPC_TRAP_LC_PORT, ipaddr, (ipc_msg *)&comando, (ipc_msg *)NULL);
  if(ret<0)
      LOG_ERR(LOG_CTX_IPC,"SENDTRAP to PORT %d: interface=%d, Code = 0x%.4x, status = %d: ERROR = %d", IPC_TRAP_LC_PORT, porto, code, status, ret);
  return(ret);
}


/* Trap related to TS Events */
int send_TxTsRecord(ptin_PptTsRecord_t *ptpTs)
{
  ipc_msg	    comando;
  st_PtpTsInfo  *tsInfo;
  int ret = 0;
  
  if(g_iInterfaceSW==-1) 
      return(-1);

  comando.protocolId= SIR_IPCPROTOCOL_ID;
  comando.flags		= IPCLIB_FLAGS_CMD;
  comando.counter   = GetMsgCounter ();
  comando.msgId		= TRAP_PTP_TS_RECORD;
  comando.infoDim   = sizeof(st_PtpTsInfo);
  tsInfo            = (st_PtpTsInfo *) &comando.info[0];

  memset(tsInfo,0x00,sizeof(st_PtpTsInfo));

  memcpy(&tsInfo->inetAddr, &ptpTs->inetAddr, sizeof(ptpTs->inetAddr));

  tsInfo->seconds = ptpTs->seconds;
  tsInfo->nanoseconds = ptpTs->nanoseconds;
  tsInfo->encap = ptpTs->encap;
  tsInfo->messageType = ptpTs->messageType;
  tsInfo->sequenceId = ptpTs->sequenceId;
  tsInfo->domainNumber = ptpTs->domainNumber;

  ret=send_data(g_iInterfaceSW, IPC_TRAP_TO_PTP, IPC_LOCALHOST_IPADDR, (ipc_msg *)&comando, (ipc_msg *)NULL);
  if(ret<0)
      LOG_ERR(LOG_CTX_IPC,"SENDTRAP ERROR = %d", ret);

  return(ret);
}



/**
 * Send IPC message
 * 
 * @param porto 
 * @param ipaddr 
 * @param msg_id 
 * @param request 
 * @param answer 
 * @param infoDimRequest 
 * @param infoDimRequestAnswer  
 * 
 * @return int : negative if error
 */
int send_ipc_message(int porto, uint32 ipaddr, int msg_id, char *request, char *answer, uint32 infoDimRequest, uint32 *infoDimAnswer)
{
  ipc_msg	comando, resposta;
  int ret;

//if (!global_var_system_ready)  return S_OK;

  if(g_iInterfaceSW==-1) 
      return(-1);

  comando.protocolId  = SIR_IPCPROTOCOL_ID;
  comando.flags		  = IPCLIB_FLAGS_CMD;
  comando.counter     = GetMsgCounter ();
  comando.msgId		  = msg_id;
  comando.infoDim     = infoDimRequest;
  memcpy(comando.info, request, infoDimRequest);

  ret=send_data(g_iInterfaceSW, porto, ipaddr, (ipc_msg *)&comando, NULL==answer? NULL:(ipc_msg *)&resposta);
  if(ret<0)
  {
    LOG_ERR(LOG_CTX_IPC,"send_message to PORT %d (Canal =%d), Code = 0x%.4x: ERROR = %d", porto, g_iInterfaceSW, msg_id, ret);
    return ret;
  }
  if (NULL==answer) return ret;

  /* Check if ACK */
  if (resposta.flags != IPCLIB_FLAGS_ACK)
  {
    LOG_ERR(LOG_CTX_IPC,"NACK (%u)", resposta.flags);
    return -1;
  }


  /* Check info */
  if (infoDimAnswer == NULL)
  {
    if  (resposta.infoDim != infoDimRequest)
    {
      LOG_ERR(LOG_CTX_IPC,"Wrong infodim (expected %u bytes VS received %u bytes)", infoDimRequest, resposta.infoDim);    
      return -1;
    }
     /* Return answer */
    memcpy(answer, resposta.info, infoDimRequest); 
  }
  else
  {
    if( resposta.infoDim>IPCLIB_MAX_MSGSIZE || 0 != (resposta.infoDim % (*infoDimAnswer)))
    {
      LOG_ERR(LOG_CTX_IPC,"Infodim (%u) higher than the maximum allowed value (IPCLIB_MAX_MSGSIZE=%u)",resposta.infoDim, IPCLIB_MAX_MSGSIZE);    
      return -1;
    }
    
     /* Return answer */
    memcpy(answer, resposta.info, resposta.infoDim); 
    *infoDimAnswer=resposta.infoDim;
  }

  return(ret);
}





int send_trap_ETH_OAM(void *param, int param_size)
{
  ipc_msg	comando;
  int ret;

//if (!global_var_system_ready)  return S_OK;

  if(g_iInterfaceSW==-1) 
      return(-1);

  comando.protocolId= SIR_IPCPROTOCOL_ID;
  comando.flags		= IPCLIB_FLAGS_CMD;
  comando.counter   = GetMsgCounter ();
  comando.msgId		= TRAP_ALARME_ETH_OAM;
  comando.infoDim   = param_size;
  memcpy(comando.info, param, param_size);

  {
   st_alarmGeral *p;
   p=           (st_alarmGeral*)comando.info;
   p->SlotId=   ptin_fpga_board_slot_get();  //this field's structure agnostic
  }

  ret=send_data(g_iInterfaceSW, IPC_CHMSG_TRAP_PORT, IPC_SERVER_IPADDR, (ipc_msg *)&comando, (ipc_msg *)NULL);
  if(ret<0)
      LOG_ERR(LOG_CTX_IPC,"SENDTRAP to PORT %d: ERROR = %d", IPC_CHMSG_TRAP_PORT, ret);
  return(ret);
}


// ----------------------------------------------------
// Interface Privada
// ----------------------------------------------------

   // --------------------------------------------------------------
   // Função:		int GetMsgCounter (void)
   // Parametros:
   // Retorno:		valor de contagem para usar na próxima mensagem
   //
   // Descrição:	Mantém um contador de mensagens evitando que duas
   //				   mensagem transportem o mesmo counter no cabeçalho.
   // --------------------------------------------------------------
   int GetMsgCounter (void)
   {
      int res = g_iCCounter;

      g_iCCounter = (g_iCCounter&0xFFFF0000) + (((g_iCCounter&0x0000FFFF)+1) % MAX_COUNTERVALUE);
      return res;
   } // GetMsgCounter (V1.0.0.060622)


/**
 * Init server ipaddr
 */
static void ipc_server_ipaddr_init(void)
{
#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
#ifdef MAP_CPLD
  if (ptin_intf_slot_get(&ptin_board_slotId) != L7_SUCCESS)
  {
    LOG_CRITICAL(LOG_CTX_IPC, "Could not acquire slot position");
  }
#endif
#endif
}

