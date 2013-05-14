// Project olt7_8ch 
// This software will run on the stand-alone olt boxes of PT Inovacao
// Development started on 20/Aug/2008 
// nuno-f-monteiro@ptinovacao.pt

// This software is provided "AS IS," without a warranty of any kind. 
// This software is not designed or intended for use in on-line control of
// aircraft, air traffic, aircraft navigation or aircraft communications; or in
// the design, construction, operation or maintenance of any nuclear
// facility. Licensee represents and warrants that it will not use or
// redistribute the Software for such purposes
// 
// Copyright (c) 2008 PT Inova��o, SA, All Rights Reserved.

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DRIV_SHELL  "driv"
#define DEV_SHELL   "dev"

#include "main.h"
#include "utils.h"
#include "ipc_lib.h"
#include "globals.h"

int canal_cli;

char default_shell[] = DRIV_SHELL;

#define     CCMSG_ETH_DRIVSHELL_CMD       0x91F1

//******************************************************************************
//
//******************************************************************************

int main (int argc, char *argv[])
{  
  //int id;
  char         request[102], *shell=default_shell;
  pc_type      comando, resposta;
  int          valued, i, cmdIndex=0;

  // 1 - Preparar mensagem a enviar ao modulo de controlo
  comando.protocolId   = 1;
  comando.srcId        = PORTO_TX_MSG_CLI;
  comando.dstId        = PORTO_RX_MSG_CLI;
  comando.flags        = (FLAG_COMANDO); //(IPCLIB_FLAGS_CMD | (IPC_UID<<4));
  comando.counter      = rand ();
  comando.msgId        = CCMSG_ETH_DRIVSHELL_CMD;
  comando.infoDim      = sizeof(int);
  *(int*)comando.info  = 0;
    
  //printf("FastPath Shell Command Executer v1.0\n\n");
  
  // Define shell and command index, through arguments
  if ( argc<=1 )
  {
    shell = default_shell;
    cmdIndex = 0;
  }
  else if ( argc>=2 )
  {
    if (strcmp(argv[1],DRIV_SHELL)==0 || strcmp(argv[1],DEV_SHELL)==0 )
    {
      shell = argv[1];
      cmdIndex = (argc==2) ? 0 : 2;
    }
    else
    {
      shell = default_shell;
      cmdIndex = 1;
    }
  }

  // Open channel
  canal_cli=open_ipc(PORTO_TX_MSG_CLI,IP_LOCALHOST,NULL,10);
  if ( canal_cli<0 )
  {
    printf("Erro no open IPC do CLI...\n\r");
    exit(0);
  }

  // If no command is given, run in loop
  do
  {
    printf("Broadcom shell > ");
    request[0]='\0';

    // If no command is given, request it
    if ( cmdIndex==0 )
    {
      scanf("%100[^\n]",request);
      scanf("%*[^\n]");
      scanf("%*c");
      request[100]='\0';

      if ( strcmp(request,"quit")==0 || strcmp(request,"QUIT")==0 || 
           strcmp(request,"exit")==0 || strcmp(request,"EXIT")==0 ||
           strcmp(request,"leave")==0 || strcmp(request,"LEAVE")==0 ||
           strcmp(request,"bye")==0 || strcmp(request,"BYE")==0 ||
           strcmp(request,"q")==0 || strcmp(request,"Q")==0 ||
           strcmp(request,"x")==0 || strcmp(request,"X")==0 )           break;
    }
    // Otherwise, use argv arguments
    else  {
      for ( i=cmdIndex; i<argc; i++ )
      {
        if ( strlen(request)+strlen(argv[i])+1>100 )  break;
        if ( i>cmdIndex )  strcat(request," ");
        strcat( request , argv[i] );
      }
      printf("%s\n",request);
    }

    // Prepare message to send
    comando.info[0]='\0';
    strcat( (char*)comando.info , ttyname(1) );
    strcat( (char*)comando.info , " " );
    strcat( (char*)comando.info , shell );
    strcat( (char*)comando.info , " " );
    strcat( (char*)comando.info , request );
    comando.infoDim = strlen((char *) comando.info)+1;

    // Send message
    valued = send_data (canal_cli, PORTO_RX_MSG_CLI, IP_LOCALHOST, &comando, &resposta);
    if ( valued )
    {
      printf("Erro %d  no send_data IPC do CLI...\n\r", valued);
    }
    else {
      resposta.info[resposta.infoDim]='\0';
      printf("%s\n",resposta.info);
    }
  } while (cmdIndex==0);
  // Only run in loop, if command was not given
    
  // Close ipc channel
  close_ipc(canal_cli);

  return 0;
}

