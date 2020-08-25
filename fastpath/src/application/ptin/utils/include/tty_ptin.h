#ifndef _MIBGROUP_TTY_H
#define _MIBGROUP_TTY_H

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

extern void ptin_PitHandler (char *target);


#endif /* _MIBGROUP_TTY_H */
