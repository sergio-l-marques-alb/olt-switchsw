#ifndef _MYTYPES_H_
#define _MYTYPES_H_

#include "config.h"



#ifdef __cplusplus
    #define EXTERN_C extern "C"
#else
    #define EXTERN_C extern
#endif

              
#define NSERIE_SIZE 19              

/* bytes ssm/s1 */
#define QUALITY_UNKNOWN  0x0
#define G_811            0x2
#define SSU_A            0x4
#define SSU_B            0x8
#define SETS             0xB
#define DUFS             0xF
 

#define CIRCUITO_FREE    0   //posicao livre
#define CIRCUITO_BUSY    1   //posicao ocupada


/* Tipo de contentor */                    
#define TIPO_VC4         0
#define TIPO_VC3         1
#define TIPO_VC12        2

/* Tipo de circuito */
#define UDIR_AtoB              0
#define UDIR_BtoA              1
#define BDIR                   2

/* accao nos circuitos */
#define ACCAO_CRUZAR           0
#define ACCAO_REMOVER          1


/* retorno de funçoes */
#define EXIT_NO_FUNC  -4
#define EXIT_LIB_INI  -3
#define EXIT_MODULE   -2
#define EXIT_NOK      -1
#define EXIT_OK        0

/* tipos de loops */
#define CONF_NO_LOOP     0
#define CONF_LOOP_LOCAL  1
#define CONF_LOOP_REMOTO 2

/* tipos de mapeamentos */
#define VC4_TRANSPARENTE 0
#define VC4_TERMINADO    1
#define TUG3_VC12        0 //TIPO_VC12
#define TUG3_VC3         1 //TIPO_VC3 
                  
/* Mapeamentos */
#undef  BEGIN_PACKED
#undef  END_PACKED
#define BEGIN_PACKED
#define END_PACKED  __attribute__  ((packed))

#define PORTO_RX_MSG_CLI            6006
#define PORTO_TX_MSG_CLI            6906

#define IP_LOCALHOST           0x7f000001  //127.000.000.001

#define MODULO_OLT7_8CH   0x69

#define MAX_ID_STRING_SIZE    15   //tamanho maximo para a identificacao de circuitos
                  
typedef char           int8;       //  8 bits
typedef unsigned char  uint8;      //  8 bits
typedef unsigned short uint16;     // 16 bits
typedef unsigned long  uint32;     // 32 bits
typedef unsigned long long  uint64;     // 64 bits

#endif //_MYTYPES_H_
