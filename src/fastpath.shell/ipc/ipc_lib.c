/************************************************************************
*                                                                       *
* FileName:                                                             *
*            ipc_lib.c                                                  *
*                                                                       *
* Projecto:                                                             *
*            PR710_SDH_RA                                               *
*                                                                       *
* Descrição: Este módulo possui as funções necessárias para controlo de *
*            canais de comunicacao entre processos                      *
*                                                                       *
*                                                                       *
* Histórico:                                                            *
*            12 Março 2003 - as mensagens de debuf foram colocadas      *
*                            sob a condiçao de definiçao do #define     *
*                            _IPC_DEBUG_.                               *
*                                                                       *
*            19 Fevereiro 2003 - primeira versão                        *
*                                                                       *
*                                                                       *
* Autor: Celso Lemos                                                    *
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "ipc_lib.h"
#include "memchk.h"
#include <sys/socket.h>
//variavel que suporta os canais de comunicacao
static T_IPC ipc_canais[MAX_CANAIS];

//#define _IPC_DEBUG_

/*********************************************************************************
* Função:         send_data                                                      *
*                                                                                *
* Descrição:      Metodo que permite enviar uma mensagem para um determinado     *
*                 processo. Dependendo dos parametros de entrada, este metodo    *
*                 pode ou nao esperar pela resposta a mensagem enviada           *                                                                                                
*                                                                                *
* Parametros entrada:                                                            *
*                 handler:       id do canal de comunicacao para onde sera       *
*                                enviada a mensagem                              *
*                                                                                *
*                 porto_destino: porto para onde sera enviada a mensagem         *
*                                                                                *
*                 sendbuffer:    ponteiro para o buffer de transmissao. Este ptr *
*                                aponta para uma estrutura pc_type               *
* Parametros saida:                                                              *
*                 receivebuffer: ponteiro para o buffer de recepcao. Este ptr    *
*                                aponta para uma estrutura pc_type               *                                                            
*                                                                                *
* retorno:                                                                       *
*                 0: Operação decorrida com sucesso                              *
*                -1: nao consegue enviar o pacote                                *
*                -2: nao recebe uma resposta valida, ou foi atingido o timeout   *
*                                                                                *
* historico:                                                                     *
*         8 Feveveiro de 2003                                                    *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
int send_data(int canal_id, int porto_destino, unsigned int ipdest, pc_type *sendbuffer, pc_type *receivebuffer)
{
   int bytes, addr_len;
   struct sockaddr_in socketaddr_aux;   

   if(canal_id>=MAX_CANAIS)
       return(-3);

   //Fazer o set do endereco destino
   bzero(&ipc_canais[canal_id].socketaddr_cliente, sizeof(ipc_canais[canal_id].socketaddr_cliente));
   ipc_canais[canal_id].socketaddr_cliente.sin_addr.s_addr=htonl(ipdest); //localhost (temporario)
   ipc_canais[canal_id].socketaddr_cliente.sin_port = htons(porto_destino); 
   
   if ( sendto(ipc_canais[canal_id].socket_descriptor_cliente, sendbuffer, (sendbuffer->infoDim)+(7*sizeof(int)), 0,
               (struct sockaddr*)&ipc_canais[canal_id].socketaddr_cliente,
               sizeof(ipc_canais[canal_id].socketaddr_cliente))==-1 )
       return(-1);

   if ( receivebuffer==NULL ) return(0);  //se receivebuffer apontar para null, nao espera resposta

   //sleep(1);
   // Espera pela resposta do outro lado
   addr_len = sizeof(socketaddr_aux);
   do {
     bytes = recvfrom(ipc_canais[canal_id].socket_descriptor_cliente, receivebuffer,
                      sizeof(*receivebuffer), 0,(struct sockaddr*)&socketaddr_aux,
                      (socklen_t*)&addr_len);
   }
   while ( ((bytes<0) && (errno!=EAGAIN)) || ((bytes>0) && (sendbuffer->counter != receivebuffer->counter)));

   if ( bytes==-1 ) {  //Resposta invalida, ou timeout
#ifdef _IPC_DEBUG_
     printf("Timeout a receber\n\r");
#endif
     return(-2);
   }

#ifdef _IPC_DEBUG_
   printf("msg from %s:%d (%d bytes)\n", inet_ntoa(socketaddr_aux.sin_addr),
          ntohs(socketaddr_aux.sin_port), bytes);
#endif

   return(0);
}



/*********************************************************************************
* Função:         clone_proc_msg                                                 *
*                                                                                *
* Descrição:      Metodo que suporta o clone de escuta no porto de recepcao      *
*                 programado. Este metodo e chamado pelo metodo <open_ipc>       *
*                                                                                *
* Parametros entrada:                                                            *
*                 handler:       ponteiro para uma variavel que possui o canal ao*
*                                qual o clone esta associado                     *
*                                                                                *
* retorno:                                                                       *
*                 0: Operação decorrida com sucesso                              *
*                                                                                *
* historico:                                                                     *
*         8 Feveveiro de 2003                                                    *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
static int clone_proc_msg(void* canal_id)
{
	pc_type	inbuffer;
	pc_type	outbuffer;
    int bytes, socketaddr_len;
    struct sockaddr_in socketaddr_aux;
    int canal;

    canal = *(int*)canal_id;

    #ifdef _IPC_DEBUG_                   
    fprintf(stderr,"clone handler=%d\n\r",canal);
    #endif
       
    
    while (ipc_canais[canal].estado==CANAL_OCUPADO)     //Enquanto o canal estiver alocado
	{		
        
        /* Espera pelo pacote no porto Rx de escuta */
        socketaddr_len = sizeof(socketaddr_aux);
		bytes = recvfrom(ipc_canais[canal].socket_descriptor_servidor, &inbuffer, sizeof(inbuffer), 0,
                         (struct sockaddr*)&socketaddr_aux, (socklen_t*)&socketaddr_len);

		if (bytes>0)
		{                              //Se pacote valido, processa
            #ifdef _IPC_DEBUG_
			fprintf(stderr,"msg from %s:%d (%d bytes)\n", inet_ntoa(socketaddr_aux.sin_addr), 
                                              ntohs(socketaddr_aux.sin_port), bytes);
            fprintf(stderr,"id=%d,dim=%d\n\r",inbuffer.msgId,inbuffer.infoDim);
            #endif
            
            //envia pacote para a rotina de processamento de pacotes (definida no metodo <open_ipc>,
            //e que devera ser definida pelo utilizador 
            if(ipc_canais[canal].processa_msg_handler(&inbuffer,&outbuffer)>=0)
            {
              // se o valor de retorno for >=0 entao e enviada um pacote outbuffer para 
              // a origem do pacote que vei no inbuffer
              sendto(ipc_canais[canal].socket_descriptor_servidor, &outbuffer,  (outbuffer.infoDim)+(7*sizeof(int)), 0,
                   (struct sockaddr*)&socketaddr_aux, 
                   sizeof(socketaddr_aux));
            }
		}
        //#ifdef _IPC_DEBUG_
		//else
        //    fprintf(stderr,"\n loop control\n");
        //#endif
        
	} //while   
    
    #ifdef _IPC_DEBUG_
    fprintf(stderr,"VOU SAIR\n\r");
    #endif
    _exit(0);
}


/*********************************************************************************
* Função:         open_ipc                                                       *
*                                                                                *
* Descrição:      Metodo que permite enviar uma mensagem para um determinado     *
*                 processo. Dependendo dos parametros de entrada, este metodo    *
*                 pode ou nao esperar pela resposta a mensagem enviada           *                                                                                                *
*                                                                                *
* Parametros entrada:                                                            *
*                 portoRx:       id do porto de recepcao que se pretende colocar *
*                                a escuta.                                       *
*                                                                                *
*                 MessageHandler:ponteiro para a rotina que ira processar os     *
*                                pacotes que chegam do portoRx                   *
*                                Esta rotina tem que ter o seguinte prototype:   *
*                                       int func(pctype *, pctype *)             *
*                                                                                *
*                 timeout:       valor do tempo de espera maximo pela chegada    *
*                                de um pacote (em segundos)                      *
*                                                                                *
*                                                                                *
* retorno:                                                                       *
*               >=0: handler para o canal                                        *
*               -1: Erro no socket Rx                                            *
*               -2: Erro no bind (Rx)                                            *
*               -3: Erro a criar clone                                           *
*               -4: Handlers indisponiveis                                       *
*                                                                                *
* historico:                                                                     *
*         8 Feveveiro de 2003                                                    *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
int open_ipc(int porto_rx, unsigned int ipaddr, int  (*MessageHandler)(pc_type *inbuffer, pc_type *outbuffer), int timeout)
{
    struct timeval tv;
    //struct hostent *hostbyname; 
    int canal_id,i;
    
    /* Deteccao do primeiro canal livre */
    for(i=0;i<MAX_CANAIS;i++)
    {
        if(ipc_canais[i].estado==CANAL_LIVRE)
            break;
    }
    /*************************************/

    if(i==MAX_CANAIS) return(-4);          //canal livre nao encontrado
    
    ipc_canais[i].estado=CANAL_OCUPADO;    //canal livre encontrado, marcar como ocupado
    canal_id=i;
    /**************************************/
    
    ipc_canais[canal_id].processa_msg_handler = MessageHandler;

    #ifdef _IPC_DEBUG_
    fprintf(stderr,"canal alocado=%d\n\r",canal_id);
    #endif


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //Inicializacao do socket Cliente - Nao se vai atribuir um porto a este socket (por S.O.)
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if((ipc_canais[canal_id].socket_descriptor_cliente = socket(AF_INET, SOCK_DGRAM, 0)) ==-1)
    {
        #ifdef _IPC_DEBUG_
        fprintf(stderr,"<ipc_lib.c> Erro no socket cliente\n\r");
        #endif
        return(-1);
    }
	bzero(&ipc_canais[canal_id].socketaddr_cliente, sizeof(ipc_canais[canal_id].socketaddr_cliente));
    ipc_canais[canal_id].socketaddr_cliente.sin_addr.s_addr=htonl(0x7F000001); //localhost (temporario)
  
    //Configuracao do timeout
    tv.tv_sec  = timeout;    
    tv.tv_usec = 0;
    setsockopt (ipc_canais[canal_id].socket_descriptor_cliente, 
                SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Inicializacao do socket Servidor
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if(MessageHandler!=NULL) {    //Se existir handler para a funcao de escuta inicializa
        if((ipc_canais[canal_id].socket_descriptor_servidor = socket(AF_INET, SOCK_DGRAM, 0)) ==-1)
        {
            #ifdef _IPC_DEBUG_
            fprintf(stderr,"<ipc_lib.c> Erro no socket servidor\n\r");
            #endif
            close(ipc_canais[canal_id].socket_descriptor_cliente);
            return(-1);
        }
        bzero(&ipc_canais[canal_id].socketaddr_servidor, sizeof(ipc_canais[canal_id].socketaddr_servidor));
        ipc_canais[canal_id].socketaddr_servidor.sin_family = AF_INET;
        ipc_canais[canal_id].socketaddr_servidor.sin_port	= htons(porto_rx);
        ipc_canais[canal_id].socketaddr_servidor.sin_addr.s_addr = htonl(ipaddr);
   

        #ifdef _IPC_DEBUG_
        printf("porto_rx = %d, ipaddr = 0x%.8x\n\r",porto_rx,ipaddr);
        #endif

        if (bind(ipc_canais[canal_id].socket_descriptor_servidor, 
                 (struct sockaddr*)&ipc_canais[canal_id].socketaddr_servidor, 
                  sizeof(ipc_canais[canal_id].socketaddr_servidor)) != 0 )
        {
             #ifdef _IPC_DEBUG_
             fprintf(stderr,"<ipc_lib.c> Erro no bind do servidor\n\r");
             #endif
             close(ipc_canais[canal_id].socket_descriptor_cliente);    //liberta o descritor de cliente         
             close(ipc_canais[canal_id].socket_descriptor_servidor);   //liberta o descritor de servidor
             return(-2);
        }

        /* Definicao do Clone de escuta */
        memfill(&ipc_canais[canal_id].stack_clone_ipc[0],STACK_CLONE_IPC,"IPC");
        if ((ipc_canais[canal_id].clone_ipc_id=clone(clone_proc_msg,
                      (void *)&ipc_canais[canal_id].stack_clone_ipc[STACK_CLONE_IPC-1],
                      (CLONE_VM | CLONE_FS | CLONE_FILES), 
                      (void*)&ipc_canais[canal_id].canal_id)) == -1 )
        {
            printf("\rIPC_LIB : Arranque CLONE <clone_proc_msg> [ERROR]\n\r");
            #ifdef _IPC_DEBUG_
            fprintf(stderr,"<clone_processar_msg>: Erro a criar o clone\n");
            #endif
            close(ipc_canais[canal_id].socket_descriptor_cliente);    //liberta o descritor de cliente         
            close(ipc_canais[canal_id].socket_descriptor_servidor);   //liberta o descritor de servidor
            return(-3);
        }     

        printf("\rIPC_LIB : Arranque CLONE <clone_proc_msg> [OK] [ID=%d]\n\r",ipc_canais[canal_id].clone_ipc_id);
        #ifdef _IPC_DEBUG_
        fprintf(stderr,"<clone_processar_msg> criado com id=%d.\n\r",ipc_canais[canal_id].clone_ipc_id);
        #endif



    } //fim do if(Message...

    tv.tv_sec = timeout;    //Configuracao do timeout
    tv.tv_usec = 0;
    setsockopt (ipc_canais[canal_id].socket_descriptor_servidor, SOL_SOCKET, 
                SO_RCVTIMEO, &tv, sizeof(tv));
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    return(canal_id);  //Retorna o handler correspondente ao canal de comunicacao alocado
}



/*********************************************************************************
* Função:         init_ipc_lib                                                   *
*                                                                                *
* Descrição:      Este metodo deve se chamado antes de qualquer um dos outros.   *
*                 Este metodo inicializa a estrutura que gere os handlers        *
*                                                                                *
* historico:                                                                     *
*         8 Feveveiro de 2003                                                    *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
void init_ipc_lib(void)
{
    int i;

    for(i=0;i<MAX_CANAIS;i++)
    {
        ipc_canais[i].estado=CANAL_LIVRE;
        ipc_canais[i].canal_id=i;
    }
    
}


/*********************************************************************************
* Função:         close_ipc                                                      *
*                                                                                *
* Descrição:      Metodo que permite destruir um canal de comunicacao alocado    *
*                 com o metodo <open_ipc>                                        *
*                                                                                *
* Parametros entrada:                                                            *
*                 handler:       Id do canal que se pretende desalocar.          *
*                                                                                *
* retorno:                                                                       *
*               =0: canal desalocado                                             *
*               -1: canal nao estava ocupado                                     *
*                                                                                *
* historico:                                                                     *
*         8 Feveveiro de 2003                                                    *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
int close_ipc(int canal_id)
{
    #ifdef _IPC_DEBUG_
    printf("Close canal, canali_id=%d\n\r",canal_id);
    #endif

    if(ipc_canais[canal_id].estado==CANAL_LIVRE) {
        //#ifdef _IPC_DEBUG_
        printf("Erro: Canal %d nao foi alocado\n\r",canal_id);
        //#endif
        return(-1);
    }
        
    if(ipc_canais[canal_id].processa_msg_handler!=NULL) {
        printf("Stack(ipc): used %d of %d\n\r", STACK_CLONE_IPC-memchk(&ipc_canais[canal_id].stack_clone_ipc[0], STACK_CLONE_IPC, 
                                                "IPC"), STACK_CLONE_IPC);
        kill(ipc_canais[canal_id].clone_ipc_id,SIGKILL);      //mata o clone
    }

    close(ipc_canais[canal_id].socket_descriptor_cliente);   //destroi o descritor para o socket cliente
    close(ipc_canais[canal_id].socket_descriptor_servidor);  //destroi o descritor para o socket servidor
    ipc_canais[canal_id].processa_msg_handler = NULL;        //coloca o ponteiro para o metodo de 
                                                             //processamento de mensagens a NULL
    ipc_canais[canal_id].estado = CANAL_LIVRE;               //marca o canal como livre
    return(0);
}


