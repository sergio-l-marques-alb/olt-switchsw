/* 
  Esta funcao permite redireccionar o stdin, stdout e stderr de um deamon ou programa em
  background para a consola desejada. Para isso, a aplicacao devera ter um loop infinito que fique
  permanentemente a procurar a informacao para comutar de consola.
 
  Deve ser incluida na Makefile
*/

/* 
  Para usar, em qualquer linha de comando, evocar tty e redireccionar para o
  directorio correcto. O ficheiro deve chamar-se sempre tty.txt
  Exemplo:
    tty > /var/lib/ptin/agente_tty/tty.txt
    
  Para anular, indica-se como consola o /dev/null. 
  Exemplo:
    echo '/dev/null' > /var/lib/ptin/agente_tty/tty.txt
*/

/*
  No programa, chamar a funcao com valor 0 dentro de um ciclo infinito

  while (1) 
  {
    PitHandler (0);  // verifica se deve comutar a informacao na consola
  ...
  }
*/


#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <stdlib.h> 
#include <stdio.h>

#include "tty.h"

int d_anterior=-1;      


void PitHandler (char *target)
{
  int d;
  
  d = open(target, O_RDWR|O_APPEND);
  if (d != -1)
  {
    close(0);   
    close(1);   
    close(2);

    if (d_anterior!=-1) {
        close(d_anterior);   
    }

    dup2(d, 0); 
    dup2(d, 1); 
    dup2(d, 2);
    
    d_anterior = d;
  }
}


/*
void PitHandler (char *target)
{
  int d;
  
  d = open(target, O_RDWR|O_APPEND);
  if (d != -1)
  {
    //printf("Consola anterior\n\r");
    close(0);   
    close(1);   
    close(2);

    dup2(d, 0); 
    dup2(d, 1); 
    dup2(d, 2);
    //printf("Consola actual\n\r");

    //close(d);
    d = -1;
  }
}

*/
