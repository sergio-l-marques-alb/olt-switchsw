#include <stdio.h>
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

/* Slot id */
uint8 ptin_board_slotId = 0;

/* Default server ip */
uint32 server_ipaddr = IPC_SERVER_IPADDR;
uint32 mx_ipaddr = IPC_MX_IPADDR;

static int g_iInterfaceSW  = -1;      // Canal de dados com as aplicacoes cliente
static int g_iInterfaceHW  = -1;      // Canal de dados com as aplicacoes firmware
static int g_iInterfaceMan = -1;      // Canal de dados com as aplicacoes firmware

static  int g_iCCounter   = 0;      //

typedef struct _st_alarmGeral {
  unsigned char  SlotId      ;  // Slot id
  unsigned char  trapSrc;    ;  // Trap source 
  unsigned char  oltiftype   ;  // Interface type: Physical:0, LAG:1
  unsigned int   oltifindex  ;  // Pon [0..7], Eth [8..15], Eth(10G)
  unsigned int   equipid     ;  // 0-OLT, 1..128-ONT (onu_Id)
  unsigned int   equipifindex;  // [1..65535] (apenas usado nos ONTs (equipid!=0), índice do porto do ONT.
  unsigned int   alarmtype   ;  // Interfaces (TRAP_TYPE_INTERFACE = 3)
  unsigned int   alarmcode   ;  // TRAP_CODE_XXX
  unsigned int   alarmstatus ;  // 0=Fim ; 1=Inicio ; 2=Evento
  unsigned int   param1      ;
  unsigned int   param2      ;
} __attribute__((packed)) st_alarmGeral;

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

//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
      LOG_INFO(LOG_CTX_IPC,
                  "Vai iniciar o modulo de comunicacoes IPC."); 
      init_ipc_lib ();

//      if ((res=open_ipc (IPC_CCMSG_PORT, &CCMessageHandler, IPC_CC_TIMEOUT, &g_iInterfaceSW))!=S_OK)
//      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_EMERGENCY, "Nao foi possivel abrir o canal de comunicacoes com as aplicacoes cliente (%08X).", res);
//         return res;
//      }

      if ((res=open_ipc (IPC_HW_FASTPATH_PORT, INADDR_ANY, &CHMessageHandler, IPC_CH_TIMEOUT, &g_iInterfaceHW))!=S_OK)
      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_EMERGENCY,
        LOG_CRITICAL(LOG_CTX_IPC,
                     "Nao foi possivel abrir o canal de comunicacoes para atendimento de mensagens (%08X).", res);
//         close_ipc (g_iInterfaceSW);
         return res;
      }
     
      if ((res=open_ipc (0, 0, NULL, IPC_CM_TIMEOUT, &g_iInterfaceSW))!=S_OK)
      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_EMERGENCY,
        LOG_INFO(LOG_CTX_IPC,
                 "Nao foi possivel abrir o canal para o envio de mensagens (%08X).", res);
//         close_ipc (g_iInterfaceSW);
         close_ipc (g_iInterfaceHW);
         return res;
      }

      pid = (int)getpid();
      g_iCCounter = (pid&0x0000FFFF)<<16;
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
      LOG_INFO(LOG_CTX_IPC,
                  "Modulo de comunicacoes IPC iniciado com sucesso."); 
      return S_OK;
   } // OpenIPC (V1.0.0.060622)

   void CloseIPC	(void)
   {
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
     LOG_INFO(LOG_CTX_IPC,
              "Vai fechar o modulo de comunicacoes IPC."); 
      if (g_iInterfaceSW>=0)
         close_ipc (g_iInterfaceSW);
      if (g_iInterfaceHW>=0)
         close_ipc (g_iInterfaceHW);
      if (g_iInterfaceMan>=0)
         close_ipc (g_iInterfaceMan);
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
      LOG_INFO(LOG_CTX_IPC,
               "Modulo de comunicacoes IPC fechado com sucesso."); 
   } // CloseIPC (V1.0.0.060622)

   void EnableHandling (BOOLEAN enable)
   {
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
     LOG_INFO(LOG_CTX_IPC,
              "(Des)activacao do processamento de mensagens do modulo de comunicacoes IPC (%d).", enable); 
//      g_CCHandlingEnabled = enable;
//      g_CHHandlingEnabled = enable;
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
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR, "Erro no atendimento da mensagem, retorno de NACK (%08X).", res);
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

  ret=send_data(g_iInterfaceSW, IPC_CHMSG_TRAP_PORT, server_ipaddr, (ipc_msg *)&comando, (ipc_msg *)NULL);
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
  alarm->SlotId      = ptin_board_slotId;
  alarm->trapSrc     = ERROR_FAMILY_HARDWARE;
  alarm->oltiftype   = intfType;
  alarm->oltifindex  = porto;
  alarm->alarmtype   = TRAP_ALARM_TYPE_INTERFACE;
  alarm->alarmcode   = code;
  alarm->alarmstatus = status;
  alarm->param1      = param;
  alarm->param2      = 0;

  ret=send_data(g_iInterfaceSW, IPC_CHMSG_TRAP_PORT, server_ipaddr, (ipc_msg *)&comando, (ipc_msg *)NULL);
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

  ret=send_data(g_iInterfaceSW, IPC_CHMSG_TRAP_PORT, server_ipaddr, (ipc_msg *)&comando, (ipc_msg *)NULL);
  if(ret<0)
      LOG_ERR(LOG_CTX_IPC,"SENDTRAP to PORT %d: interface=%d, Code = 0x%.4x, status = %d: ERROR = %d", IPC_CHMSG_TRAP_PORT, porto, code, status, ret);
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
  ipaddr = (server_ipaddr & (~((L7_uint32) 0xff))) | ((slot_to_send+2) & 0xff);

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




/**
 * Send IPC message
 * 
 * @param porto 
 * @param ipaddr 
 * @param msg_id 
 * @param request 
 * @param answer 
 * @param infoDim 
 * 
 * @return int : negative if error
 */
int send_ipc_message(int porto, uint32 ipaddr, int msg_id, char *request, char *answer, uint32 infoDim)
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
  comando.infoDim     = infoDim;
  memcpy(comando.info, request, infoDim);

  ret=send_data(g_iInterfaceSW, porto, ipaddr, (ipc_msg *)&comando, (ipc_msg *)&resposta);
  if(ret<0)
  {
    LOG_ERR(LOG_CTX_IPC,"send_message to PORT %d (Canal =%d), Code = 0x%.4x: ERROR = %d", porto, g_iInterfaceSW, msg_id, ret);
    return ret;
  }

  if (resposta.infoDim != infoDim)
  {
    LOG_ERR(LOG_CTX_IPC,"Wrong infodim (received %u bytes VS expected %u bytes)", infoDim, resposta.infoDim);
    return ret;
  }

  /* Return answer */
  memcpy(answer, resposta.info, infoDim);

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
   p->SlotId=   ptin_board_slotId;  //this field's structure agnostic
  }

  ret=send_data(g_iInterfaceSW, IPC_CHMSG_TRAP_PORT, server_ipaddr, (ipc_msg *)&comando, (ipc_msg *)NULL);
  if(ret<0)
      LOG_ERR(LOG_CTX_IPC,"SENDTRAP to PORT %d: ERROR = %d", IPC_CHMSG_TRAP_PORT, ret);
  return(ret);
}



//// --------------------------------------------------------------
//// Função:		int SendSetMessage ([in]int msg,
////				[in]void *data, [in/out]int* num)
////
//// Parametros:	msg	  - identificador da mensagem a enviar
////				data  - buffer com os dados a enviar na mensagem.
////				num   - à entrada indica o número bytes que se
////						vai enviar; à saída indica o código
////						da resposta (para erro ou sucesso).
////
//// Retorno:		S_OK  - o pedido/resposta correu normalmente
////				outro - indica o tipo de erro ocorrido
////
//// Descrição:	Executa um pedido de dados à aplicação remota.
//// --------------------------------------------------------------
//int SendSetMessage (UINT msg, void *data, int* num)
//{
// int     res;
// ipc_msg comando,
//          resposta;
//
// // 1 - Preparar mensagem a enviar ao modulo de controlo
// DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,  "Vai enviar mensagem %08X (SET) para o modulo de FW.", msg);
// comando.protocolId  = SIR_IPCPROTOCOL_ID;
// comando.srcId          = IPC_CCMSG_PORT;
// //comando.pageSize     = 0;
// comando.dstId          = IPC_AGENT_PORT;
// comando.flags       = IPCLIB_FLAGS_CMD;
// comando.counter     = GetMsgCounter ();
// comando.msgId       = msg;
// comando.infoDim     = *num;
// memcpy (comando.info, data, *num);
// // 2 - enviar mensagem e receber resposta
// res = send_data (g_iInterfaceSW, IPC_AGENT_PORT, &comando, &resposta);
// *num = 0;
// if (res==S_OK)
// {
//    // 3 - validar resposta (em termos protocolares
//      *num = *((int*)resposta.info);
//      if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//      {
//       res = *((int*)resposta.info);
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com NACK (%08X).", res);
//      }
// }
//   else
//   {
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//   }
// return res;
//} // SendSetMessage (V1.0.0.060622)
//


//
//   int SendGetStructMessage   (UINT msg, void *data, int  num)
//   {
//      int 		res;
//	   ipc_msg	comando,
//				   resposta;
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Vai enviar mensagem %08X (GET) para o modulo de FW (idx=%d).", msg);
//	   // 1 - Preparar mensagem a enviar ao modulo de controlo
//	   comando.protocolId	= SIR_IPCPROTOCOL_ID;
//	   comando.srcId 		   = IPC_CCMSG_PORT;
//	   //comando.pageSize     = 0;
//	   comando.dstId 		   = IPC_AGENT_PORT;
//	   comando.flags		   = IPCLIB_FLAGS_CMD;
//	   comando.counter		= GetMsgCounter ();
//	   comando.msgId		   = msg;
//	   comando.infoDim		= num;
//      memcpy (comando.info, data, num);
//
//	   // 2 - enviar mensagem e receber resposta
//	   res = send_data (g_iInterfaceSW, IPC_AGENT_PORT, &comando, &resposta);
//	   if (res==S_OK)
//	   {
//		   // 3 - validar resposta (em termos protocolares)
//         if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//         {
//			   res = *((int*)resposta.info);
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com NACK (%08X).", res);
//         }
//         else
//         {
//            if (resposta.infoDim!=num)
//            {
//               DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com tamanho diferente (%d!=%d).", resposta.infoDim, num);
//               res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
//            }
//            else
//            {
//			      memcpy (data, resposta.info, num);
//               DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Mensagem enviada, resposta com ACK");
//            }
//         }
//	   }
//      else
//      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//      }
//	   return res;
//   } // SendGetStructMessage (V1.0.0.060622)
//
//   int SendNotifyMessage (UINT msg, void *data, int* num)
//   {
//	   int 		res;
//	   ipc_msg	comando;
//
//	   // 1 - Preparar mensagem a enviar ao modulo de controlo
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,  "Vai enviar mensagem %08X (SET) para o modulo de Agente.", msg);
//	   comando.protocolId	= SIR_IPCPROTOCOL_ID;
//	   comando.srcId 		   = IPC_CCMSG_PORT;
//	   //comando.pageSize     = 0;
//	   comando.dstId 		   = IPC_AGENT_PORT;
//	   comando.flags		   = IPCLIB_FLAGS_CMD;
//	   comando.counter		= GetMsgCounter ();
//	   comando.msgId		   = msg;
//	   comando.infoDim		= *num;
//	   memcpy (comando.info, data, *num);
//	   // 2 - enviar mensagem e receber resposta
//	   res = send_data (g_iInterfaceSW, IPC_AGENT_PORT, &comando, NULL);
//	   if (res!=S_OK)
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//	   return res;
//   } // SendNotifyMessage (V1.0.0.060622)
//
//   // --------------------------------------------------------------
//   // Função:	int SendHWGetMessage ([in]int index, [in]int msg,
//   //				                    [out]void *data, [in/out]int* num)
//   //
//   // Parametros:	index - valor inteiro enviado no corpo da mensagem
//   //				   msg   - identificador da mensagem a enviar
//   //				   data  - buffer com os dados recebidos na resposta
//   //				   num   - à entrada indica o número máximo de bytes
//   //					   	que se espera receber; à saída indica o
//   //					   	número de bytes válidos no campo data
//   //					   	ou o código detalhado do erro da resposta.
//   //
//   // Retorno:		S_OK  - o pedido/resposta correu normalmente
//   //				outro - indica o tipo de erro ocorrido
//   //
//   // Descrição:	Executa um pedido de dados à aplicação remota.
//   // --------------------------------------------------------------
//   int SendHWGetMessage (int index, UINT msg, void *data, size_t* num)
//   {
//	   int 		res, menor;
//	   ipc_msg	comando, resposta;
//
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "{%s(inbuffer[%08X], msg[%04X], num[%d])", __FUNCTION__, index, msg, *num);
//	   // 1 - Preparar mensagem a enviar ao modulo de controlo
//	   comando.protocolId	= SIR_IPCPROTOCOL_ID;
//	   comando.srcId 		   = IPC_CHMSG_PORT;
//	   //comando.pageSize     = 0;
//	   comando.dstId 		   = IPC_HARDWARE_PORT;
//	   comando.flags		   = IPCLIB_FLAGS_CMD;
//	   comando.counter		= GetMsgCounter();
//	   comando.msgId		   = msg;
//	   comando.infoDim		= sizeof(UINT);
//	   *(int*)comando.info	= index;
//
//	   // 2 - enviar mensagem e receber resposta
//	   res = send_data (g_iInterfaceHW, IPC_HARDWARE_PORT, &comando, &resposta);
//	   if (res==S_OK)
//	   {
//		   // 3 - validar resposta (em termos protocolares)
//         if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//         {
//            *num = 0;
//			   res = *((int*)resposta.info);
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "}%s(%08X - Receive NACK)", __FUNCTION__, res);
//         }
//         else
//         {
//            menor = (*num>resposta.infoDim) ? resposta.infoDim : *num;
//			   memcpy (data, resposta.info, menor);
//			   *num = menor;
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "}%s(%08X - Receive ACK, return %d bytes of %d read)", __FUNCTION__, res, *num, resposta.infoDim);
//         }
//	   }
//      else
//      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "}%s(%08X - Fail sending message[%04X])", __FUNCTION__, res, msg);
//      }
//	   return res;
//   }// SendHWGetMessage
//
//   int SendHWGetStructMessage (UINT msg, void *data, int num)
//   {
//      int 		res;
//	   ipc_msg	comando,
//				   resposta;
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Vai enviar mensagem %08X (GET) para o modulo de FW.", msg);
//	   // 1 - Preparar mensagem a enviar ao modulo de controlo
//	   comando.protocolId	= SIR_IPCPROTOCOL_ID;
//	   comando.srcId 		   = IPC_CHMSG_PORT;
//	   //comando.pageSize     = 0;
//	   comando.dstId 		   = IPC_HARDWARE_PORT;
//	   comando.flags		   = IPCLIB_FLAGS_CMD;
//	   comando.counter		= GetMsgCounter ();
//	   comando.msgId		   = msg;
//	   comando.infoDim		= num;
//      memcpy (comando.info, data, num);
//
//	   // 2 - enviar mensagem e receber resposta
//	   res = send_data (g_iInterfaceHW, IPC_HARDWARE_PORT, &comando, &resposta);
//	   if (res==S_OK)
//	   {
//		   // 3 - validar resposta (em termos protocolares)
//         if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//         {
//			   res = *((int*)resposta.info);
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com NACK (%08X).", res);
//         }
//         else
//         {
//            if (resposta.infoDim!=num)
//            {
//               DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com tamanho diferente (%d!=%d).", resposta.infoDim, num);
//               res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
//            }
//            else
//            {
//			      memcpy (data, resposta.info, num);
//               DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Mensagem enviada, resposta com ACK");
//            }
//         }
//	   }
//      else
//      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//      }
//	   return res;
//   } // SendHWGetStructMessage (V1.0.0.060622)
//
//   int SendHWGetStructMessageEx (UINT msg, void *data, int *num)
//   {
//      int 		res;
//      int      menor;
//      ipc_msg	comando = {0},
//               resposta;
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Vai enviar mensagem %08X (GET) para o modulo de FW.", msg);
//      // 1 - Preparar mensagem a enviar ao modulo de controlo
//      comando.protocolId	= SIR_IPCPROTOCOL_ID;
//      comando.srcId 		   = IPC_CHMSG_PORT;
//	   //comando.pageSize     = 0;
//      comando.dstId 		   = IPC_HARDWARE_PORT;
//      comando.flags		   = IPCLIB_FLAGS_CMD;
//      comando.counter		= GetMsgCounter ();
//      comando.msgId		   = msg;
//      comando.infoDim		= (*num);
//      memcpy (comando.info, data, (*num));
//
//      // 2 - enviar mensagem e receber resposta
//      res = send_data (g_iInterfaceHW, IPC_HARDWARE_PORT, &comando, &resposta);
//      if (res==S_OK)
//      {
//         // 3 - validar resposta (em termos protocolares)
//         if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//         {
//            res = *((int*)resposta.info);
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com NACK (%08X).", res);
//         }
//         else
//         {
//            menor = (*num>resposta.infoDim) ? resposta.infoDim : *num;
//            memcpy (data, resposta.info, menor);
//            *num = menor;
//         }
//      }
//      else
//      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//      }
//      return res;
//   } // SendHWGetStructMessageEx (V1.0.0.060622)
//
//   int SendHwPortGetMessage (UINT dest, UINT index, UINT msg, void *data, int* num)
//   {
//   	int 		res, menor;
//   	ipc_msg	comando = {0},
//   				resposta;
//
//   	// 1 - Preparar mensagem a enviar ao modulo de controlo
//   	comando.protocolId	= SIR_IPCPROTOCOL_ID;
//   	comando.srcId 		   = IPC_CHMSG_PORT;
//	   //comando.pageSize     = 0;
//   	comando.dstId 		   = dest;
//   	comando.flags		   = IPCLIB_FLAGS_CMD;
//   	comando.counter		= GetMsgCounter ();
//   	comando.msgId		   = msg;
//   	comando.infoDim		= sizeof(UINT);
//   	*(int*)comando.info	= index;
//
//   	// 2 - enviar mensagem e receber resposta
//   	res = send_data (g_iInterfaceHW, dest, &comando, &resposta);
//   	if (res==S_OK)
//   	{
//         //LOGTRACE (LOG_MASKBIT_IPC, ERROR_SEVERITY_INFORMATIONAL, "Enviou o comando %08X para HW.", msg);
//   		// 3 - validar resposta (em termos protocolares)
//         if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//         {
//            //LOGTRACE (LOG_MASKBIT_IPC, ERROR_SEVERITY_ERROR, "IPCLIB_FLAGS_NACK (%08X)", resposta.flags);
//            *num = 0;
//   			res = *((int*)resposta.info);
//         }
//         else
//         {
//            menor = (*num>resposta.infoDim) ? resposta.infoDim : *num;
//   			memcpy (data, resposta.info, menor);
//   			*num = menor;
//         }
//   	}
//      else
//      {
//         res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, res);
//         //LOGTRACE (LOG_MASKBIT_IPC, ERROR_SEVERITY_ERROR, "Falhou o envio de comando %08X para o Porto (%08X).", res);
//      }
//   	return res;
//   } // SendHwPortGetMessage
//
//   // --------------------------------------------------------------
//   // Função:		int SendHWSetMessage ([in]int msg,
//   //				[in]void *data, [in/out]int* num)
//   //
//   // Parametros:	msg	  - identificador da mensagem a enviar
//   //				data  - buffer com os dados a enviar na mensagem.
//   //				num   - à entrada indica o número bytes que se
//   //						vai enviar; à saída indica o código
//   //						da resposta (para erro ou sucesso).
//   //
//   // Retorno:		S_OK  - o pedido/resposta correu normalmente
//   //				outro - indica o tipo de erro ocorrido
//   //
//   // Descrição:	Executa um pedido de dados à aplicação remota.
//   // --------------------------------------------------------------
//   int SendHWSetMessage (UINT msg, void *data, size_t *num)
//   {
//	   int 		res;
//	   ipc_msg	comando = {0},
//				   resposta;
//
//	   // 1 - Preparar mensagem a enviar ao modulo de controlo
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "{%s(message[%08X], size[%u]", __FUNCTION__, msg, *num);
//	   comando.protocolId	= SIR_IPCPROTOCOL_ID;
//	   comando.srcId 		   = IPC_CHMSG_PORT;
//	   //comando.pageSize     = 0;
//	   comando.dstId 		   = IPC_HARDWARE_PORT;
//	   comando.flags		   = IPCLIB_FLAGS_CMD;
//	   comando.counter		= GetMsgCounter ();
//	   comando.msgId		   = msg;
//	   comando.infoDim		= *num;
//	   memcpy (comando.info, data, *num);
//	   // 2 - enviar mensagem e receber resposta
//	   res = send_data (g_iInterfaceHW, IPC_HARDWARE_PORT, &comando, &resposta);
//	   *num = 0;
//	   if (res==S_OK)
//	   {
//		   // 3 - validar resposta (em termos protocolares
//         *num = *((int*)resposta.info);
//         if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//         {
//			   res = *((int*)resposta.info);
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "}%s(%08X - Message send, but received NACK)", __FUNCTION__, res);
//         }
//	   }
//      else
//      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "}%s(%08X - Error sending message)", __FUNCTION__, res);
//         return res;
//      }
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "}%s(%08X - Success sending message)", __FUNCTION__, res);
//	   return res;
//   } // SendHWSetMessage (V1.0.0.060622)
//
//   int SendHWNotifyMessage (UINT msg, void *data, int* num)
//   {
//	   int 		res;
//	   ipc_msg	comando;
//
//	   // 1 - Preparar mensagem a enviar ao modulo de controlo
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Vai enviar mensagem %08X (SET) para o modulo de FW.", msg);
//	   comando.protocolId	= SIR_IPCPROTOCOL_ID;
//	   comando.srcId 		   = IPC_CHMSG_PORT;
//	   //comando.pageSize     = 0;
//	   comando.dstId 		   = IPC_HARDWARE_PORT;
//	   comando.flags		   = IPCLIB_FLAGS_CMD;
//	   comando.counter		= GetMsgCounter ();
//	   comando.msgId		   = msg;
//	   comando.infoDim		= *num;
//	   memcpy (comando.info, data, *num);
//	   // 2 - enviar mensagem e receber resposta
//	   res = send_data (g_iInterfaceSW, IPC_HARDWARE_PORT, &comando, NULL);
//	   if (res!=S_OK)
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//	   return res;
//   } // SendHWNotifyMessage (V1.0.0.060622)
//
//   int SendHWSetExMessage (UINT msg, void *input, int num, int *output, int* outnum)
//   {
//	   int 		res;
//      int      menor;
//	   ipc_msg	comando,
//				   resposta;
//
//	   // 1 - Preparar mensagem a enviar ao modulo de controlo
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Vai enviar mensagem %08X (SET) para o modulo de FW.", msg);
//	   comando.protocolId	= SIR_IPCPROTOCOL_ID;
//	   comando.srcId 		   = IPC_CHMSG_PORT;
//	   //comando.pageSize     = 0;
//	   comando.dstId 		   = IPC_HARDWARE_PORT;
//	   comando.flags		   = IPCLIB_FLAGS_CMD;
//	   comando.counter		= GetMsgCounter ();
//	   comando.msgId		   = msg;
//	   comando.infoDim		= num;
//	   memcpy (comando.info, input, num);
//	   // 2 - enviar mensagem e receber resposta
//	   res = send_data (g_iInterfaceHW, IPC_HARDWARE_PORT, &comando, &resposta);
//	   if (res==S_OK)
//	   {
//		   // 3 - validar resposta (em termos protocolares
//         if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//         {
//			   res = *((int*)resposta.info);
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com NACK (%08X).", res);
//         }
//         else
//         {
//            menor = (*outnum>resposta.infoDim) ? resposta.infoDim : *outnum;
//			   memcpy (output, resposta.info, menor);
//			   *outnum = menor;
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Mensagem enviada, resposta com ACK, recebidos %d bytes, devolvidos %d.", resposta.infoDim, *outnum);
//         }
//	   }
//      else
//      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//      }
//	   return res;
//   } // SendHWSetExMessage (V1.0.0.060622)
//
//   int SendPortGetStructMessage (UINT dest, UINT msg, void *data, int num, int uid)
//   {
//      int 		res;
//      ipc_msg	comando, resposta;
//
//      uid = uid & 0x0000000F;
//      // 1 - Preparar mensagem a enviar ao modulo de controlo
//      comando.protocolId	= SIR_IPCPROTOCOL_ID;
//   	comando.srcId 		   = IPC_CCMSG_PORT;
//	   //comando.pageSize     = 0;
//   	comando.dstId 		   = dest;
//   	comando.flags		   = 0;
//   	comando.counter		= GetMsgCounter ();
//   	comando.msgId		   = msg;
//      comando.infoDim		= num;
//      memcpy (comando.info, data, num);
//
//   	// 2 - enviar mensagem e receber resposta
//      res = send_data (g_iInterfaceMan, dest, &comando, &resposta);
//      if (res==S_OK)
//      {
//   		// 3 - validar resposta (em termos protocolares)
//         if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//         {
//   			res = *((int*)resposta.info);
//            DEBUGTRACE (ERROR_FAMILY_IPC | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com NACK (%08X).", res);
//         }
//         else
//         {
//            if (resposta.infoDim!=num)
//            {
//               DEBUGTRACE (ERROR_FAMILY_IPC | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com tamanho diferente (%d!=%d).", resposta.infoDim, num);
//               res = SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
//            }
//            else
//            {
//   			   memcpy (data, resposta.info, num);
//               DEBUGTRACE (ERROR_FAMILY_IPC | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Mensagem enviada, resposta com ACK");
//            }
//         }
//   	}
//      else
//      {
//         DEBUGTRACE (ERROR_FAMILY_IPC | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//      }
//   	return res;
//   } // SendPortGetStructMessage
//
//   int SendHwPortNotifyMessage (UINT dest, UINT msg, void *data, int* num)
//   {
//      int 		res;
//      ipc_msg	comando;
//
//      // 1 - Preparar mensagem a enviar ao modulo de controlo
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Vai enviar mensagem %08X (SET) para o modulo de FW.", msg);
//      comando.protocolId	= SIR_IPCPROTOCOL_ID;
//      comando.srcId 		   = IPC_CHMSG_PORT;
//	   //comando.pageSize     = 0;
//      comando.dstId 		   = dest;
//      comando.flags		   = IPCLIB_FLAGS_CMD;
//      comando.counter		= GetMsgCounter ();
//      comando.msgId		   = msg;
//      comando.infoDim		= *num;
//      memcpy (comando.info, data, *num);
//      // 2 - enviar mensagem e receber resposta
//      res = send_data (g_iInterfaceSW, IPC_HARDWARE_PORT, &comando, NULL);
//      if (res!=S_OK)
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//      return res;
//   } // SendHWNotifyMessage (V1.0.0.060622)
//
//   HRES SendPortNotifyMessage (UINT dest, UINT msg, void *data, int* num)
//   {
//      int 		res;
//      ipc_msg	comando;
//
//      // 1 - Preparar mensagem a enviar ao modulo de controlo
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Vai enviar mensagem %08X (SET) para o modulo de FW.", msg);
//      comando.protocolId	= SIR_IPCPROTOCOL_ID;
//      comando.srcId 		   = IPC_CHMSG_PORT;
//      //comando.pageSize     = 0;
//      comando.dstId 		   = dest;
//      comando.flags		   = IPCLIB_FLAGS_CMD;
//      comando.counter		= GetMsgCounter ();
//      comando.msgId		   = msg;
//      comando.infoDim		= *num;
//      memcpy (comando.info, data, *num);
//      // 2 - enviar mensagem e receber resposta
//      res = send_data (g_iInterfaceSW, IPC_HARDWARE_PORT, &comando, NULL);
//      if (res!=S_OK)
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "Nao foi possivel enviar a mensagem (%08X).", res);
//      return res;
//   } // SendHWNotifyMessage (V1.0.0.060622)
//
//HRES SendHwStructMessage (void *input, size_t inSize, UINT msg, void *output, size_t *outSize)
//{
//   ipc_msg	comando, resposta;
//   BYTE     *p = (BYTE*)input;
//   HRES 		res = S_OK;
//   int      menor;
//
//   // 1 - Preparar mensagem a enviar ao modulo de controlo
//   comando.protocolId	= SIR_IPCPROTOCOL_ID;
//	comando.srcId 		   = IPC_CHMSG_PORT;
//	comando.dstId 		   = IPC_CCMSG_PORT;
//	comando.flags		   = IPCLIB_FLAGS_CMD;
//	comando.counter		= GetMsgCounter();
//	comando.msgId		   = msg;
//   comando.infoDim		= inSize;
//   memcpy(comando.info, input, inSize);
//
//	// 2 - enviar mensagem e receber resposta
//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "{%s(message[%08X], input[%02X%02X%02X%02X], inSize[%u])", __FUNCTION__, msg, p[0], p[1], p[2], p[3], inSize);
//   if ( S_OK == (res = send_data(g_iInterfaceSW, IPC_HARDWARE_PORT, &comando, &resposta)) )
//   {
//      // 3 - validar resposta (em termos protocolares)
//      if ((resposta.flags & IPCLIB_FLAGS_NACK)==IPCLIB_FLAGS_NACK)
//      {
//         *outSize = 0;
//         res = *((HRES*)resposta.info);
//         DEBUGTRACE (ERROR_FAMILY_IPC | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "}%s(%08X - Message send, but received NACK)", __FUNCTION__, res);
//      }
//      else
//      {
//         menor = MIN(resposta.infoDim, *outSize);
//         memcpy (output, resposta.info, menor);
//         *outSize = menor;
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "}%s(%08X - Receive ACK, return %d bytes of %d read)", __FUNCTION__, res, *outSize, resposta.infoDim);
//      }
//	}
//   else
//   {
//      DEBUGTRACE (ERROR_FAMILY_IPC | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, "}%s(%08X - Fail sending message[%04X])", __FUNCTION__, res, msg);
//   }
//	return res;
//}// SendHwStructMessage


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
 * Set a new server ipaddr
 * 
 * @author mruas (12/8/2012)
 * 
 * @param ipaddr 
 */
void ipc_server_ipaddr_set(uint32 ipaddr)
{
  if (ipaddr==0 || ipaddr==(uint32)-1)
    ipaddr = IPC_SERVER_IPADDR;

  server_ipaddr = ipaddr;
}

/**
 * Init server ipaddr
 */
static void ipc_server_ipaddr_init(void)
{
  server_ipaddr = IPC_SERVER_IPADDR;

  #if (PTIN_BOARD_IS_MATRIX)
  if (cpld_map->reg.slot_id==0)
  {
    server_ipaddr = IPC_SERVER_IPADDR_WORKING;
    mx_ipaddr = IPC_MX_IPADDR_WORKING;
    /* Slot id */
    ptin_board_slotId = 1;
  }
  else
  {
    server_ipaddr = IPC_SERVER_IPADDR_PROTECTION;
    mx_ipaddr = IPC_MX_IPADDR_PROTECTION;
    /* Slot id */
    ptin_board_slotId = 20;
  }
  #endif
}

