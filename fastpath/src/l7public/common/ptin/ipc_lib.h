/* ======================================================================
Projecto:   ********
Modulo:     ********
Copyright:  SIR@PT Inovacao
Descricao:  Biblioteca de comunicação entre processos baseada em sockets.

Autor:      Celso Lemos (CL), Ana Cláudia (AC), Paulo Gonçalves (PG),
			Fernando Romão (FR), Vitor Mirones (VM)

Historico:  CL 2003.02.19 - Criacao do modulo V1.0.0.0
			   VM 2003.05.20 - correcção saída precoce por signal no recvfrom
						          (incluir EAGAIN no controlo de loop)
		      PG 2003.08.30 - Verificação do counter no recvfrom (ack), manter 
					 	          à escuta de não coincidir						
			   VM 2003.10.07 - Alteração dos nomes de algumas constantes
						        - Valores de retorno
                          - Validação no recvfrom pelo cabeçalho do prot.
            VM 2004.08.20 - Revisao V1.5.0.040820

======================================================================= */
#ifndef SIR_IPC_LIB_H
#define SIR_IPC_LIB_H

#include <arpa/inet.h>
#include "globaldefs.h"

#ifndef DEBUGTRACE
   #define DEBUGTRACE(...)	;
#endif

/* Return values */
#define IPC_OK        0
#define IPC_NO_REPLY  1
#define IPC_FAIL     -1


#define SIR_IPCPROTOCOL_ID       1

#define IPCLIB_MAX_PORTNUMBER	   65500    // 
#define IPCLIB_MAX_CANAIS		   5        // Numero maximo de canais de comunicacao
#define IPCLIB_CANAL_LIVRE		   0        // Estado do handler: livre
#define IPCLIB_CANAL_OCUPADO	   1        // Estado do handler: ocupado

#define IPCLIB_STACK_SIZE		   80000    // tamanho do stack para o clone
#define IPCLIB_MAX_MSGSIZE		   6144     // tamanho para o buffer info do protocolo

#define IPCLIB_DEFAULT_TIMEOUT   5

// mascaras para as flags
#define IPCLIB_MASK_MSGTYPE	   0x03		  // comando/resposta
#define IPCLIB_MASK_PROFILE	   0xFFF0	  // 

// tipos de mensagem
#define IPCLIB_FLAGS_CMD		   0x00     // pedido
#define IPCLIB_FLAGS_ACK		   0x01     // resposta
#define IPCLIB_FLAGS_NACK		   0x03     // resposta com erro

#define IPC_HW_CANAL          1

// *****************************************************************************
//      definicao da estrutura que suporta o protocolo de comunicacao          *
// *****************************************************************************
typedef struct
{
   UINT   protocolId;               //identificador do tipo de protocolo
   UINT   srcId;                    //endereco fonte   (nao obrigatorio)
   //UWORD  pageSize;                 //indica o tamanho da pagina de leitura
   UINT   dstId;                    //endereco destino (nao obrigatorio)
   UINT   flags;                    //flags (bit0-0:comando, 1:resposta)
   UINT   counter;                  //contador usado para identificar as msg
   UINT   msgId;                    //codigo da mensagem a enviar
   UINT   infoDim;                  //dimensao em bytes validos  do buffer info
   BYTE	 info[IPCLIB_MAX_MSGSIZE]; //buffer de dados
} ipc_msg;

// ****************************************************************************
//      definicao da estrutura que suporta cada canal de comunicacao          *
// ****************************************************************************
typedef  struct
{
   int		estado;                                //Estado do canal: 0-livre, 1-ocupado
   BOOLEAN  keep_running;                          //Controlo o loop de execucao do thread de atendimento
   int		canal_id;                              //Id do canal

   int		socket_descriptor_cliente;             //descritor do socket de transmissao
   int		socket_descriptor_servidor;            //descritor do socket de recepcao
   SOCKADDR	socketaddr_cliente;                    //endereco do socket de transmissao
   SOCKADDR	socketaddr_servidor;                   //endereco do socket de recepcao
   int      timeout;                               //define o intervalo de espera por uma resposta

   int      (*processa_msg_handler) (ipc_msg *inbuffer, ipc_msg *outbuffer); //handler para a funcao de escuta no porto de recepcao

   char     stack_clone_ipc[IPCLIB_STACK_SIZE];    //declaração do stack para o clone
   int      clone_ipc_id;                          //id para o clone de escuta
} T_IPC;


// ****************************************************************************
//     DECLARACAO DAS FUNCOES FORNECIDAS PELA BIBLIOTECA DE COMUNICACAOES     *
// ****************************************************************************

EXTERN_C void printmemstatus  (void);

EXTERN_C void init_ipc_lib    (void);

EXTERN_C int  open_ipc        (int porto_rx,
                               unsigned int ipaddr,
                               int  (*MessageHandler)(ipc_msg *inbuffer, ipc_msg *outbuffer),
                               int timeout,
                               int *handlerid);

EXTERN_C int  close_ipc       (int handler);

EXTERN_C int  send_data       (int canal_id,
                               int porto_destino,
                               unsigned int ipdest,
                               ipc_msg *sendbuffer,
                               ipc_msg *receivebuffer);

//EXTERN_C int            g_AppIpcSemaphore;
EXTERN_C void *g_AppIpcSemaphore;


// ****************************************************************************
//     DECLARACAO DAS FUNCOES PRIVADAS DA BIBLIOTECA		                     *
// ****************************************************************************

int  ValidAnswer  (ipc_msg *input, ipc_msg *output);
int  ValidCommand (ipc_msg *input);

#endif  // SIR_IPC_LIB_H
