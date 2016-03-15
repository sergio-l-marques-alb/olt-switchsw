#ifndef __IPC_LIB_H__
#define __IPC_LIB_H__

#include <arpa/inet.h>

//#define _PTHREAD_

#ifdef _PTHREAD_
#include <pthread.h>
#endif

//*****************************************************************************
//#define _IPC_DEBUG_             //Descomentar para debug

#define MAX_CANAIS    5          //Numero maximo de canais de comunicacao
#define CANAL_LIVRE   0          //Estado do handler: livre
#define CANAL_OCUPADO 1          //Estado do handler: ocupado

#define STACK_CLONE_IPC  32768*2  //tamanho do stack para o clone
//#define PC_MsgDimMax     4096   //tamanho para o buffer info do protocolo
#define PC_MsgDimMax    5120   //tamanho para o buffer info do protocolo
#define PC_PckDimMax     sizeof(pc_type)  // Tamanho do pacote completo
#define PC_HeaderDim     (PC_PckDimMax-PC_MsgDimMax)
                    
/* mascaras para as flags */
#define FLAG_RESPOSTA 0x01   //1-resposta
#define FLAG_ACK      0x00   //1-ACK
#define FLAG_COMANDO  0x00   
#define FLAG_NACK     0x02   

//*****************************************************************************
/*      definicao da estrutura que suporta o protocolo de comunicacao        */
/*****************************************************************************/
typedef struct {
  unsigned int   protocolId;         //identificador do tipo de protocolo
  unsigned int   srcId;              //endereco fonte   (nao obrigatorio)
  unsigned int   dstId;              //endereco destino (nao obrigatorio)
  unsigned int   flags;              //flags (bit0-0:comando, 1:resposta)
  unsigned int   counter;            //contador usado para identificar as msg
  unsigned int   msgId;              //codigo da mensagem a enviar
  unsigned int   infoDim;            //dimensao em bytes validos  do buffer info
  unsigned char  info[PC_MsgDimMax]; //buffer de dados
} pc_type;
/*****************************************************************************/


/*      definicao da estrutura que suporta cada canal de comunicacao         */
/*****************************************************************************/
typedef  struct {
  int estado;                             //Estado do canal: 0-livre, 1-ocupado
  int canal_id;                           //Id do canal

  int socket_descriptor_cliente;          //descritor do socket de transmissao
  int socket_descriptor_servidor;         //descritor do socket de recepcao
  struct sockaddr_in socketaddr_cliente;  //endereco do socket de transmissao
  struct sockaddr_in socketaddr_servidor; //endereco do socket de recepcao

  //handler para a funcao de escuta no porto de recepcao */
  int (*processa_msg_handler)(pc_type *inbuffer, pc_type *outbuffer); 


  char stack_clone_ipc[STACK_CLONE_IPC]; //declara��o do stack para o clone
  int clone_ipc_id;                      //id para o clone de escuta
#ifdef _PTHREAD_
  pthread_t	*ti;
  long thread_id;
  pthread_attr_t thread_attr;
#endif                                          
} T_IPC;
/*****************************************************************************/


/*****************************************************************************/
/*     DECLARACAO DAS FUNCOES FORNECIDAS PELA BIBLIOTECA DE COMUNICACAOES    */
/*****************************************************************************/

/*****************************************************************************/
#ifdef __cplusplus
extern "C"
#else
extern
#endif
void init_ipc_lib(void);

/*****************************************************************************/
#ifdef __cplusplus
extern "C"
#else
extern
#endif
int open_ipc(int porto_rx, unsigned int ipaddr,
             int (*MessageHandler)(pc_type *inbuffer,pc_type *outbuffer),
             int timeout, int prio);

/*****************************************************************************/
#ifdef __cplusplus
extern "C"
#else
extern
#endif
int close_ipc(int handler);

/*****************************************************************************/
#ifdef __cplusplus
extern "C"
#else
extern
#endif
int send_data(int canal_id, 
              int porto_destino, 
              unsigned int ipdest,
              pc_type *sendbuffer, 
              pc_type *receivebuffer);

/*****************************************************************************/

#endif  //__IPC_LIB_H__

