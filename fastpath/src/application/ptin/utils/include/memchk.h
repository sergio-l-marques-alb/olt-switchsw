/* ======================================================================
Projecto:   ********
Modulo:     ********
Copyright:  SIR1@PT Inovacao
Descricao:  Biblioteca de funções de uso geral.

Autor:      Paulo Gonçalves (PG)

Historico:  VM 2003.10.01 - Adaptacao do modulo (VM)
            VM 2004.05.27 - Revisao para versao V1.2.0.0
            VM 2004.08.23 - Revisao V1.5.0.040823

======================================================================= */
#ifndef SIR_MEMCHK_H
#define SIR_MEMCHK_H

#include "globaldefs.h"

   EXTERN_C void  memfill  (char *start, unsigned int dim, char *with);
   EXTERN_C int   memchk   (char *start, unsigned int dim, char *with);
   EXTERN_C void  printmem (char *start, unsigned int dim);

#endif // SIR_MEMCHK_H
