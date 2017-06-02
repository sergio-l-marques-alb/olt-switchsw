#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <errno.h> //alterado
#include "memchk.h"

//#include "semlib.h"
#include "sirerrors.h"
//#include "traces.h"
#include "logger.h"
#include "ipc_lib.h"
#include "ptin_structs.h"
#include "osapi.h"

#define IPC_LIB_PTHREAD 1


T_IPC ipc_canais[IPCLIB_MAX_CANAIS];		// variavel que suporta os canais de comunicacao

#ifdef IPC_LIB_PTHREAD
static int clone_proc_msg (L7_uint32 argc, void *argv);
#else
static int clone_proc_msg (void* canal_id);
#endif

//void *g_AppIpcSemaphore = L7_NULLPTR;

//void printmemstatus(void)
//{
//  printf ("\n-------------------------------\n");
//  printf ("| tabela memcheck             |\n");
//  printf ("-------------------------------\n");
//  printf ("| canal |   usada  |  total   |\n");
//  printf ("-------------------------------\n");
//  printf ("|   0   | %8d | %8d |\n", IPCLIB_STACK_SIZE  - memchk(ipc_canais[0].stack_clone_ipc, IPCLIB_STACK_SIZE, "STACK_"), IPCLIB_STACK_SIZE);
//  printf ("|   0   | %8d | %8d |\n", IPCLIB_STACK_SIZE  - memchk(ipc_canais[1].stack_clone_ipc, IPCLIB_STACK_SIZE, "STACK_"), IPCLIB_STACK_SIZE);
//  printf ("-------------------------------\n");
//} // printmemstatus (V1.0.5.040820)

// ********************************************************************************
// Função:         init_ipc_lib                                                   *
// Descrição:      Este metodo deve se chamado antes de qualquer um dos outros.   *
//                 Este metodo inicializa a estrutura que gere os handlers        *
// ********************************************************************************
void init_ipc_lib(void)
{
   int i;

//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
   PT_LOG_INFO(LOG_CTX_IPC,
            "Vai iniciar a biblioteca ipc_lib."); 
   for(i=0;i<IPCLIB_MAX_CANAIS;i++)
   {
      ipc_canais[i].estado	      = IPCLIB_CANAL_LIVRE;
      ipc_canais[i].keep_running = FALSE;
      ipc_canais[i].canal_id	   = i;
      ipc_canais[i].socket_descriptor_cliente  = 0;
      ipc_canais[i].socket_descriptor_servidor = 0;
      memset (&ipc_canais[i].socketaddr_cliente,  0, sizeof(SOCKADDR));
      memset (&ipc_canais[i].socketaddr_servidor, 0, sizeof(SOCKADDR));
      ipc_canais[i].processa_msg_handler = NULL;
      ipc_canais[i].clone_ipc_id = 0;
   }
//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
   PT_LOG_INFO(LOG_CTX_IPC,
            "Biblioteca ipc_lib iniciada."); 
} // init_ipc_lib (V1.0.5.040820)

// ********************************************************************************
// Função:         open_ipc                                                       *
// Descrição:      Metodo que permite enviar uma mensagem para um determinado     *
//                 processo. Dependendo dos parametros de entrada, este metodo    *
//                 pode ou nao esperar pela resposta a mensagem enviada           *                                                                                                *
// Parametros entrada:                                                            *
//                 portoRx:       id do porto de recepcao que se pretende colocar *
//                                a escuta.                                       *
//                                                                                *
//                 MessageHandler:ponteiro para a rotina que ira processar os     *
//                                pacotes que chegam do portoRx                   *
//                                Esta rotina tem que ter o seguinte prototype:   *
//                                       int func(pctype *, pctype *)             *
//                                                                                *
//                 timeout:       valor do tempo de espera maximo pela chegada    *
//                                de um pacote (em segundos)                      *
// retorno:                                                                       *
//               S_OK					: OK					                            *
//               ERROR_CODE_CREATESOCKET: Erro no socket Rx                       *
//               ERROR_CODE_BINDSOCKET	: Erro no bind (Rx)                        *
//               ERROR_CODE_CREATECLONE	: Erro a criar clone                    *
//               ERROR_CODE_NOFREECHAN	: Handlers indisponiveis                   *
// ********************************************************************************
int open_ipc (int porto_rx, unsigned int ipaddr, int  (*MessageHandler)(ipc_msg *inbuffer, ipc_msg *outbuffer), int timeout, int *handlerid)
{
   struct timeval tv;
   struct hostent *hostbyname; 
   int canal_id,i;
   int optVal;
   socklen_t optLen;
   //int buffersize, size_of_buffersize;

   tv.tv_sec = 1;
   tv.tv_usec = 0;
   *handlerid = -1;
//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
   PT_LOG_INFO(LOG_CTX_IPC,
            "Abertura de canal de comunicacao (%d, %08X, %d).", porto_rx, MessageHandler, timeout); 
   // Deteccao do primeiro canal livre
   for(i=0;i<IPCLIB_MAX_CANAIS;i++)
   {
      if(ipc_canais[i].estado==IPCLIB_CANAL_LIVRE)
         break;
   }
   if(i==IPCLIB_MAX_CANAIS)
   {
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_EMERGENCY,
      PT_LOG_CRITIC(LOG_CTX_IPC,
                  "Nao existem canais disponiveis."); 
      return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_EMERGENCY, ERROR_CODE_NOFREECHAN);
   }
    
   ipc_canais[i].estado = IPCLIB_CANAL_OCUPADO;				  //canal livre encontrado, marcar como ocupado
   canal_id = i;
   ipc_canais[canal_id].processa_msg_handler = MessageHandler;

   //Inicializacao do socket Cliente - Nao se vai atribuir um porto a este socket (por S.O.)
   if((ipc_canais[canal_id].socket_descriptor_cliente = socket(AF_INET, SOCK_DGRAM, 0)) ==-1)
   {
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_EMERGENCY,
      PT_LOG_CRITIC(LOG_CTX_IPC,
                  "Falhou ao criar o socket cliente (%08X).", errno); 
      return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_EMERGENCY, ERROR_CODE_CREATESOCKET);
   }
   bzero (&ipc_canais[canal_id].socketaddr_cliente, sizeof(ipc_canais[canal_id].socketaddr_cliente));
   hostbyname = gethostbyname("127.0.0.1");  //envio com endereco local
   memcpy (&ipc_canais[canal_id].socketaddr_cliente.sin_addr.s_addr, hostbyname->h_addr, 
           sizeof(ipc_canais[canal_id].socketaddr_cliente.sin_addr.s_addr));

   //Configuracao do timeout
   setsockopt (ipc_canais[canal_id].socket_descriptor_cliente, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
   // Propriedades do socket
   optLen = sizeof(int);
   //if (getsockopt (ipc_canais[canal_id].socket_descriptor_cliente, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&optVal, &optLen)==0)
   //   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Socket cliente do canal %d, SO_MAX_MSG_SIZE=%d.", canal_id, optVal); 
   //else
   //   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR, "Nao obteve o valor de SO_MAX_MSG_SIZE no Socket cliente do canal %d (errno=%d).", canal_id, errno);
   if (getsockopt (ipc_canais[canal_id].socket_descriptor_cliente, SOL_SOCKET, SO_SNDBUF, (char*)&optVal, &optLen)==0)
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
      PT_LOG_INFO(LOG_CTX_IPC,
               "Socket cliente do canal %d, SO_SNDBUF=%d.", canal_id, optVal); 
   else
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR,
      PT_LOG_ERR(LOG_CTX_IPC,
                "Nao obteve o valor de SO_SNDBUF no Socket cliente do canal %d (errno=%d).", canal_id, errno); 
   ipc_canais[canal_id].timeout = timeout;
   // Inicializacao do socket Servidor
   if(MessageHandler!=NULL && porto_rx>0 && porto_rx<IPCLIB_MAX_PORTNUMBER)	    //Se existir handler para a funcao de escuta inicializa
   {

      if((ipc_canais[canal_id].socket_descriptor_servidor = socket(AF_INET, SOCK_DGRAM, 0)) ==-1)
      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_EMERGENCY,
         PT_LOG_CRITIC(LOG_CTX_IPC,
                      "Falhou ao criar o socket servidor (%08X).", errno); 
         close(ipc_canais[canal_id].socket_descriptor_cliente);    //liberta o descritor de cliente
         return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_EMERGENCY, ERROR_CODE_CREATESOCKET);
      }
      bzero(&ipc_canais[canal_id].socketaddr_servidor, sizeof(ipc_canais[canal_id].socketaddr_servidor));
      ipc_canais[canal_id].socketaddr_servidor.sin_family = AF_INET;
      ipc_canais[canal_id].socketaddr_servidor.sin_port	= htons(porto_rx);
      ipc_canais[canal_id].socketaddr_servidor.sin_addr.s_addr = htonl(ipaddr);

      setsockopt (ipc_canais[canal_id].socket_descriptor_servidor, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
      if (bind(ipc_canais[canal_id].socket_descriptor_servidor, 
               (struct sockaddr*)&ipc_canais[canal_id].socketaddr_servidor, 
               sizeof(ipc_canais[canal_id].socketaddr_servidor)) != 0 )
      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_EMERGENCY,
         PT_LOG_CRITIC(LOG_CTX_IPC,
                      "Falhou ao ligar o socket servidor ao porto de escuta (%08X).", errno); 
         close(ipc_canais[canal_id].socket_descriptor_cliente);    //liberta o descritor de cliente
         close(ipc_canais[canal_id].socket_descriptor_servidor);   //liberta o descritor de servidor
         return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_EMERGENCY, ERROR_CODE_BINDSOCKET);
      }
#if 0
      /* RX Buffer size redefinition */
      getsockopt (ipc_canais[canal_id].socket_descriptor_servidor, SOL_SOCKET, 
                  SO_RCVBUF, &buffersize, &size_of_buffersize);
      PT_LOG_NOTICE(LOG_CTX_IPC, "Canal %u: Default rx buffer size is %u bytes!", canal_id, buffersize);

      buffersize = 55296;    //Configuracao do timeout
      setsockopt (ipc_canais[canal_id].socket_descriptor_servidor, SOL_SOCKET, 
                  SO_RCVBUF, &buffersize, sizeof(buffersize));

      getsockopt (ipc_canais[canal_id].socket_descriptor_servidor, SOL_SOCKET, 
                  SO_RCVBUF, &buffersize, &size_of_buffersize);
      PT_LOG_NOTICE(LOG_CTX_IPC, "Canal %u: Redefined rx buffer size to %u bytes!", canal_id, buffersize);
#endif

//      // Definicao do Clone de escutas
      ipc_canais[canal_id].keep_running = TRUE;

#ifdef IPC_LIB_PTHREAD
      if (L7_ERROR == (ipc_canais[canal_id].clone_ipc_id = (L7_uint32)osapiTaskCreate("ipc_task", (void *)clone_proc_msg, 1, (void*)&ipc_canais[canal_id].canal_id,
                                            IPCLIB_STACK_SIZE,
                                            L7_DEFAULT_TASK_PRIORITY,
                                            L7_DEFAULT_TASK_SLICE))
      )
      {
         close(ipc_canais[canal_id].socket_descriptor_cliente);    //liberta o descritor de cliente
         close(ipc_canais[canal_id].socket_descriptor_servidor);   //liberta o descritor de servidor
         return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_EMERGENCY, ERROR_CODE_CREATECLONE);
      }
#else
      memfill(ipc_canais[canal_id].stack_clone_ipc,IPCLIB_STACK_SIZE , "STACK_");
      if ((ipc_canais[canal_id].clone_ipc_id = clone(clone_proc_msg,
               (void *)&ipc_canais[canal_id].stack_clone_ipc[IPCLIB_STACK_SIZE-1],
               (CLONE_VM | /*CLONE_SIGHAND |*/ CLONE_FS | CLONE_FILES),
               (void*)&ipc_canais[canal_id].canal_id)) == -1 )
      {
//         DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_EMERGENCY,
         PT_LOG_CRITIC(LOG_CTX_IPC,
                      "Falhou na criacao do thread de atendimento de mensagens.");
         close(ipc_canais[canal_id].socket_descriptor_cliente);    //liberta o descritor de cliente
         close(ipc_canais[canal_id].socket_descriptor_servidor);   //liberta o descritor de servidor
         return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_EMERGENCY, ERROR_CODE_CREATECLONE);
      }
#endif

   } // fim do if(Message...
   *handlerid = canal_id;  //Retorna o handler correspondente ao canal de comunicacao alocado
//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
   PT_LOG_INFO(LOG_CTX_IPC,
            "Canal aberto com sucesso (id=%d).", canal_id);
   return S_OK;
} // open_ipc (V1.0.5.040820)

// ********************************************************************************
// Função:         close_ipc                                                      *
// Descrição:      Metodo que permite destruir um canal de comunicacao alocado    *
//                 com o metodo <open_ipc>                                        *
// Parametros entrada:                                                            *
//                 handler:       Id do canal que se pretende desalocar.          *
// retorno:                                                                       *
//               S_OK					: canal desalocado                            *
//               ERROR_CODE_NOTOPENED	: canal nao estava ocupado                 *
// ********************************************************************************
int close_ipc(int canal_id)
{
   int res;

//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
   PT_LOG_INFO(LOG_CTX_IPC,
            "Fecho de canal de comunicacoes (id=%d).", canal_id); 
   if (canal_id<0 || canal_id>=IPCLIB_MAX_CANAIS)
   {
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR,
      PT_LOG_ERR(LOG_CTX_IPC,
                "Identificador de canal invalido."); 
      return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDCH);
   }
   if (ipc_canais[canal_id].estado==IPCLIB_CANAL_LIVRE)
   {
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR,
      PT_LOG_ERR(LOG_CTX_IPC,
                "O canal indicado nao esta atribuido."); 
      return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_NOTOPENED);
   }
   if (ipc_canais[canal_id].processa_msg_handler!=NULL)
   {
      ipc_canais[canal_id].keep_running = FALSE;

      if (ipc_canais[canal_id].clone_ipc_id != L7_ERROR)
      {
#ifdef IPC_LIB_PTHREAD
         osapiTaskDelete(ipc_canais[canal_id].clone_ipc_id);
#else
      //waitpid (ipc_canais[canal_id].clone_ipc_id, NULL, __WCLONE);
      kill (ipc_canais[canal_id].clone_ipc_id, SIGKILL);      //mata o clone
#endif
      }
   }
   //destroi o descritor para o socket cliente
   if ((res=close (ipc_canais[canal_id].socket_descriptor_cliente))!=0)
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR,
      PT_LOG_ERR(LOG_CTX_IPC,
                "O socket cliente NAO foi fechado com sucesso (%d).", res);
   //destroi o descritor para o socket servidor
   if ((res=close (ipc_canais[canal_id].socket_descriptor_servidor))!=0)
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR,
      PT_LOG_ERR(LOG_CTX_IPC,
                "O socket servidor NAO foi fechado com sucesso (%d).", res);
   ipc_canais[canal_id].processa_msg_handler = NULL;        //coloca o ponteiro para o metodo de 
                                                            //processamento de mensagens a NULL
   ipc_canais[canal_id].clone_ipc_id = 0;
   ipc_canais[canal_id].socket_descriptor_cliente  = 0;
   ipc_canais[canal_id].socket_descriptor_servidor = 0;

   ipc_canais[canal_id].estado = IPCLIB_CANAL_LIVRE;        //marca o canal como livre
//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
   PT_LOG_INFO(LOG_CTX_IPC,
            "O canal %d foi fechado com sucesso.", canal_id);
   return S_OK;
} // close_ipc (V1.0.5.040820)

// ********************************************************************************
// Função:         send_data                                                      *
// Descrição:      Metodo que permite enviar uma mensagem para um determinado     *
//                 processo. Dependendo dos parametros de entrada, este metodo    *
//                 pode ou nao esperar pela resposta a mensagem enviada           *                                                                                                *
// Parametros entrada:                                                            *
//                 handler:       id do canal de comunicacao para onde sera       *
//                                enviada a mensagem                              *
//                                                                                *
//                 porto_destino: porto para onde sera enviada a mensagem         *
//                                                                                *
//                 sendbuffer:    ponteiro para o buffer de transmissao. Este ptr *
//                                aponta para uma estrutura pc_type               *
// Parametros saida:                                                              *
//                 receivebuffer: ponteiro para o buffer de recepcao. Este ptr    *
//                                aponta para uma estrutura pc_type               *                                                            *
// retorno:                                                                       *
//                S_OK                  : Operação decorrida com sucesso              *
//                ERROR_CODE_SENDFAILED : nao consegue enviar o pacote          *
//                -2: nao recebe uma resposta valida, ou foi atingido o timeout   *
// ********************************************************************************
// 0param canal_id
// 0param porto_destino
// 0param unsigned
// 0param sendbuffer
// 0param receivebuffer
// 0return
int send_data (int canal_id, int porto_destino, unsigned int ipdest, ipc_msg *sendbuffer, ipc_msg *receivebuffer)
{
   int bytes, timeout, res;
   struct sockaddr_in socketaddr_aux;
   socklen_t addr_len;
   unsigned int infoDim;

// DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
//          "Pedido de envio de dados pelo canal %d para o porto %d (keep_running=%d)", canal_id, porto_destino, ipc_canais[canal_id].keep_running);
   if (canal_id<0 || canal_id>=IPCLIB_MAX_CANAIS)
   {
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR,
      PT_LOG_ERR(LOG_CTX_IPC,
                "Identificador de canal invalido."); 
      return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDCH);
   }
//   if ( IPC_HW_CANAL == canal_id )
//   {
//      osapiSemaTake(g_AppIpcSemaphore, L7_WAIT_FOREVER);
//   }
	// 1 - Envia comando
   bzero(&ipc_canais[canal_id].socketaddr_cliente, sizeof(ipc_canais[canal_id].socketaddr_cliente));
   ipc_canais[canal_id].socketaddr_cliente.sin_addr.s_addr=htonl(ipdest); //localhost (temporario)
   ipc_canais[canal_id].socketaddr_cliente.sin_port = htons(porto_destino); //faz o set do endereco destino

   /* Extract infodim */
   infoDim = sendbuffer->infoDim;

   /* Convert outbuffer to correct endianess */
   sendbuffer->protocolId  = ENDIAN_SWAP32(sendbuffer->protocolId);
   sendbuffer->srcId       = ENDIAN_SWAP32(sendbuffer->srcId);
   sendbuffer->dstId       = ENDIAN_SWAP32(sendbuffer->dstId);
   sendbuffer->flags       = ENDIAN_SWAP32(sendbuffer->flags);
   sendbuffer->counter     = ENDIAN_SWAP32(sendbuffer->counter);
   sendbuffer->msgId       = ENDIAN_SWAP32(sendbuffer->msgId);
   sendbuffer->infoDim     = ENDIAN_SWAP32(sendbuffer->infoDim);

   //if(sendto(ipc_canais[canal_id].socket_descriptor_cliente, sendbuffer, sizeof(*sendbuffer), 0, 

   res = sendto(ipc_canais[canal_id].socket_descriptor_cliente, sendbuffer, (infoDim)+(7*sizeof(UINT)), 0, 
                (struct sockaddr*)&ipc_canais[canal_id].socketaddr_cliente, 
                sizeof(ipc_canais[canal_id].socketaddr_cliente));

   /* Revert outbuffer byte-inversions */
   sendbuffer->protocolId  = ENDIAN_SWAP32(sendbuffer->protocolId);
   sendbuffer->srcId       = ENDIAN_SWAP32(sendbuffer->srcId);
   sendbuffer->dstId       = ENDIAN_SWAP32(sendbuffer->dstId);
   sendbuffer->flags       = ENDIAN_SWAP32(sendbuffer->flags);
   sendbuffer->counter     = ENDIAN_SWAP32(sendbuffer->counter);
   sendbuffer->msgId       = ENDIAN_SWAP32(sendbuffer->msgId);
   sendbuffer->infoDim     = ENDIAN_SWAP32(sendbuffer->infoDim);

   if(res == -1)
   {
//      if ( IPC_HW_CANAL == canal_id )
//      {
//         osapiSemaGive(g_AppIpcSemaphore);
//      }
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR, 
      PT_LOG_ERR(LOG_CTX_IPC,
                "Nao foi possivel enviar a mensagem (%08X).", errno); 
      return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_SENDFAILED);
   }
//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, 
   PT_LOG_TRACE(LOG_CTX_IPC,
                "Mensagem enviada pelo canal %d.", canal_id); 

   if(receivebuffer==NULL)						//se receivebuffer apontar para null, nao espera resposta
   {
//      if ( IPC_HW_CANAL == canal_id )
//      {
//         osapiSemaGive(g_AppIpcSemaphore);
//      }
      return S_OK;
   }
//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, 
   PT_LOG_TRACE(LOG_CTX_IPC,
            "Aguarda resposta pelo canal %d.", canal_id); 
    
   // 2 - Espera pela resposta do outro lado
   addr_len = sizeof(socketaddr_aux);
   timeout  = ipc_canais[canal_id].timeout ? ipc_canais[canal_id].timeout : IPCLIB_DEFAULT_TIMEOUT;
   do
   {
      bytes = recvfrom(ipc_canais[canal_id].socket_descriptor_cliente, receivebuffer, 
                       sizeof(*receivebuffer), 0, (struct sockaddr*)&socketaddr_aux,
                       &addr_len);

      /* Convert receivebuffer to correct endianess */
      receivebuffer->protocolId  = ENDIAN_SWAP32(receivebuffer->protocolId);
      receivebuffer->srcId       = ENDIAN_SWAP32(receivebuffer->srcId);
      receivebuffer->dstId       = ENDIAN_SWAP32(receivebuffer->dstId);
      receivebuffer->flags       = ENDIAN_SWAP32(receivebuffer->flags);
      receivebuffer->counter     = ENDIAN_SWAP32(receivebuffer->counter);
      receivebuffer->msgId       = ENDIAN_SWAP32(receivebuffer->msgId);
      receivebuffer->infoDim     = ENDIAN_SWAP32(receivebuffer->infoDim);

      if (bytes<0)
      {
         switch (errno)
         {
            case EAGAIN:   timeout--;
                           break;
            default:       //DEBUGTRACE(TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, 
                           PT_LOG_WARN(LOG_CTX_IPC,
                                       "Saída anormal do recvfrom de respostas no canal %d (%d).", canal_id, errno);
                           break;
         }
      }
   } while (timeout>0 && ValidAnswer(sendbuffer, receivebuffer)==E_INVALID);
//   if ( IPC_HW_CANAL == canal_id )
//   {
//      osapiSemaGive(g_AppIpcSemaphore);
//   }
   if(timeout<=0)
   {                         
//      DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, 
      PT_LOG_WARN(LOG_CTX_IPC,
                  "Timeout na recepcao da resposta no canal %d.", canal_id); 
      return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_WARNING, ERROR_CODE_TIMEOUT);
   }

//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL,
   PT_LOG_TRACE(LOG_CTX_IPC,
            "Devolve resposta recebida no canal %d.", canal_id); 
   return S_OK;
} // send_data (V1.5.0.040823)

// ********************************************************************************
// Função:         clone_proc_msg                                                 *
// Descrição:      Metodo que suporta o clone de escuta no porto de recepcao      *
//                 programado. Este metodo e chamado pelo metodo <open_ipc>       *
// Parametros entrada:                                                            *
//                 handler:       ponteiro para uma variavel que possui o canal   *
//                                ao qual o clone esta associado                  *
// retorno:                                                                       *
//                 S_OK		: Operação decorrida com sucesso                       *
// ********************************************************************************
#ifdef IPC_LIB_PTHREAD
static int clone_proc_msg (L7_uint32 argc, void *argv)
#else
static int clone_proc_msg (void* canal_id)
#endif
{
   ipc_msg	            inbuffer = {0}, outbuffer = {0};
   int                  bytes;
   struct sockaddr_in   socketaddr_aux;
   int                  canal, timeout;
   socklen_t            socketaddr_len;
   unsigned int         infoDim;
#ifdef IPC_LIB_PTHREAD
   canal = *(int *) argv;
#else
   canal = *(int*)canal_id;
#endif

//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, 
   PT_LOG_TRACE(LOG_CTX_IPC, "Abertura do thread de atendimento de mensagens do canal %d.", canal);
   while (ipc_canais[canal].keep_running)
   {		
      timeout  = ipc_canais[canal].timeout ? ipc_canais[canal].timeout : IPCLIB_DEFAULT_TIMEOUT;
      // Espera pelo pacote no porto Rx de escuta
      socketaddr_len = sizeof(socketaddr_aux);
      do
      {
         bytes = recvfrom(ipc_canais[canal].socket_descriptor_servidor, &inbuffer, sizeof(inbuffer), 0,
                          (struct sockaddr*)&socketaddr_aux, &socketaddr_len);

         /* Convert endianess of IPC header (info data is not touched) */
         inbuffer.protocolId  = ENDIAN_SWAP32(inbuffer.protocolId);
         inbuffer.srcId       = ENDIAN_SWAP32(inbuffer.srcId);
         inbuffer.dstId       = ENDIAN_SWAP32(inbuffer.dstId);
         inbuffer.flags       = ENDIAN_SWAP32(inbuffer.flags);
         inbuffer.counter     = ENDIAN_SWAP32(inbuffer.counter);
         inbuffer.msgId       = ENDIAN_SWAP32(inbuffer.msgId);
         inbuffer.infoDim     = ENDIAN_SWAP32(inbuffer.infoDim);

         if (bytes<0)
         {
            switch (errno)
            {
               case EAGAIN:   timeout--;
                              break;
               default:       //DEBUGTRACE(TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_WARNING, 
                              PT_LOG_WARN(LOG_CTX_IPC, "Saída anormal do recvfrom de comandos no canal %d (%d).", canal, errno);
                              break;
            }
         }
      } while (timeout>0 && ValidCommand(&inbuffer)==E_INVALID && ipc_canais[canal].keep_running);
      if (bytes>0)	//Se pacote valido, processa
      {                              
         //DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, 
         PT_LOG_TRACE(LOG_CTX_IPC, "Recebeu comando no canal %d (msgid=%08X, Dim=%d, size=%d).", canal, inbuffer.msgId, inbuffer.infoDim, bytes);
         //envia pacote para a rotina de processamento de pacotes (definida no metodo <open_ipc>,
         //e que devera ser definida pelo utilizador
         if(ipc_canais[canal].processa_msg_handler(&inbuffer, &outbuffer) == IPC_OK)
         {
           infoDim = outbuffer.infoDim;

           /* Convert outbuffer to correct endianess */
           outbuffer.protocolId  = ENDIAN_SWAP32(outbuffer.protocolId);
           outbuffer.srcId       = ENDIAN_SWAP32(outbuffer.srcId);
           outbuffer.dstId       = ENDIAN_SWAP32(outbuffer.dstId);
           outbuffer.flags       = ENDIAN_SWAP32(outbuffer.flags);
           outbuffer.counter     = ENDIAN_SWAP32(outbuffer.counter);
           outbuffer.msgId       = ENDIAN_SWAP32(outbuffer.msgId);
           outbuffer.infoDim     = ENDIAN_SWAP32(outbuffer.infoDim);

            // se o valor de retorno for ==0 entao e enviada um pacote outbuffer para 
            // a origem do pacote que veio no inbuffer
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, 
           PT_LOG_TRACE(LOG_CTX_IPC, "Vai enviar resposta pelo canal %d (size=%d).", canal, infoDim);
            //if(sendto(ipc_canais[canal].socket_descriptor_servidor, &outbuffer, sizeof(outbuffer), 0,
           if (sendto(ipc_canais[canal].socket_descriptor_servidor, &outbuffer, (infoDim)+(7*sizeof(UINT)), 0,
                   (struct sockaddr*)&socketaddr_aux, 
                   sizeof(socketaddr_aux))==-1)
            {
//               DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_ERROR, 
              PT_LOG_ERR(LOG_CTX_IPC, "Nao foi possivel enviar a resposta pelo canal %d - %s ", canal, strerror(errno));
            }
            else
            {
//               DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, 
               PT_LOG_TRACE(LOG_CTX_IPC, "Enviada resposta pelo canal %d.", canal);
            }
         }
         else
         {
//            DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, 
            PT_LOG_TRACE(LOG_CTX_IPC, "Nao existe resposta 'a mensagem %08X no canal %d.", inbuffer.msgId, canal);
         }
      }
   } //while  
//   DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_APP, TRACE_SEVERITY_INFORMATIONAL, 
   PT_LOG_TRACE(LOG_CTX_IPC, "Fecho do thread de atendimento de mensagens do canal %d.",  canal);
   return S_OK;
} // clone_proc_msg (V1.5.0.040823)


// -------------------------------------------------------------------------------------------------
// Métodos Privados
// -------------------------------------------------------------------------------------------------

int  ValidAnswer  (ipc_msg *input, ipc_msg *output)
{
   if (input->protocolId != output->protocolId) return E_INVALID;
   if (input->srcId      != output->dstId)      return E_INVALID;
   if (input->dstId      != output->srcId)      return E_INVALID;
   if (input->counter    != output->counter)    return E_INVALID;
   if (input->msgId      != output->msgId)      return E_INVALID;
   return S_OK;
} // ValidAnswer (V1.5.0.040823)

int  ValidCommand  (ipc_msg *input)
{
   if (input->protocolId != SIR_IPCPROTOCOL_ID)
   {
      //DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "ProtocolId invalid");
      return E_INVALID;
   }
   if ((input->flags & IPCLIB_MASK_MSGTYPE)!=IPCLIB_FLAGS_CMD)
   {
      //DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_IPC, TRACE_SEVERITY_INFORMATIONAL, "Invalido nao sei bem porque");
      return E_INVALID;
   }
   
   return S_OK;
} // ValidAnswer (V1.5.0.040823)
