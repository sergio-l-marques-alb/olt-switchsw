#ifndef _MYTYPES_H_
#define _MYTYPES_H_

#include "config.h"
#include <endian.h>


#ifdef __cplusplus
    #define EXTERN_C extern "C"
#else
    #define EXTERN_C extern
#endif

#define PTIN_BOARD_CXP360G  1
#define PTIN_BOARD_CXO640G  2
#define PTIN_BOARD_CXO160G  3
#define PTIN_BOARD_TG4G     4
#define PTIN_BOARD_TG16G    5
#define PTIN_BOARD_TG16GF   6
#define PTIN_BOARD_TA48GE   7
#define PTIN_BOARD_OLT1T0   8
#define PTIN_BOARD_OLT1T0F  9
#define PTIN_BOARD_TT04SXG  10
#define PTIN_BOARD_TT08SXG  11

/* Fort systems Little Endian and if not pizza box, invert bytes */
#if (__BYTE_ORDER == __LITTLE_ENDIAN) && \
    (PTIN_BOARD != PTIN_BOARD_OLT1T0 && PTIN_BOARD != PTIN_BOARD_OLT1T0F)

#define ENDIAN_SWAP8(val) \
  ((unsigned char)(val))
#define ENDIAN_SWAP16(val) \
  ((unsigned short) (((unsigned short)(val)<<8) | ((unsigned short)(val)>>8)))
#define ENDIAN_SWAP32(val) \
  ((unsigned long) (((unsigned long)(val) >> 24) | (((unsigned long)(val) >> 8) & 0x0000ff00UL) | \
                    ((unsigned long)(val) << 24) | (((unsigned long)(val) << 8) & 0x00ff0000UL)))
#define ENDIAN_SWAP64(val) \
  ((unsigned long long) (((unsigned long long)(val) >> 56) | (((unsigned long long)(val) >> 40) & 0x000000000000ff00ULL) | (((unsigned long long)(val) >> 24) & 0x0000000000ff0000ULL) | (((unsigned long long)(val) >> 8) & 0x00000000ff000000ULL) | \
                         ((unsigned long long)(val) << 56) | (((unsigned long long)(val) << 40) & 0x00ff000000000000ULL) | (((unsigned long long)(val) << 24) & 0x0000ff0000000000ULL) | (((unsigned long long)(val) << 8) & 0x000000ff00000000ULL)))

#else

#define ENDIAN_SWAP8(val)  (val)
#define ENDIAN_SWAP16(val) (val)
#define ENDIAN_SWAP32(val) (val)
#define ENDIAN_SWAP64(val) (val)

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

//#define CS_UNIPROC         0x1000000

//#define PORTO_TX_MSG_CX              6000
//#define PORTO_RX_MSG_GESTAO          6000
#define PORTO_RX_TRAP_CONTROLO       6001
//#define PORTO_TX_MSG_UNICOM          7000
#define PORTO_TX_TRAP_CONTROLO       5001
//#define PORTO_TX_TRAP_UNI2CX         6002
//#define PORTO_COM_CX_2_UNICOM_SE     6003
//#define PORTO_COM_TRAP_UNI2CONTROLO  6004
#define PORTO_RX_MSG_BUGA            6006  //claudia
#define PORTO_TX_MSG_BUGA            6906  //claudia

//#define IP_CX_MASTER           0xC0A8C80A  //192.168.200.010
//#define IP_CX_SLAVE            0xC0A8C80B  //192.168.200.011
//#define IP_UNICOM              0xC0A8C801  //192.168.200.001
//#define IP_UNICOM_1            0xC0A8C814  //192.168.200.020
#define IP_LOCALHOST           0x7f000001  //127.000.000.001

//#define SECCAO_DCC_MSOH 0
//#define SECCAO_DCC_RSOH 0x40

//#define MODULO_UNICOM     0x41
//#define MODULO_UNICOM_SE  0x44
#define MODULO_OLT7_8CH   0x69


/* Nomenclatura para os agregados Este/Oeste */
/*
#define ESTE  0     //posicao nos fpgas
#define OESTE 1
*/

#define MAX_ID_STRING_SIZE    15   //tamanho maximo para a identificacao de circuitos
                  
typedef signed char         int8;    //  8 bits
typedef unsigned char       uint8;   //  8 bits
typedef signed short        int16;   // 16 bits
typedef unsigned short      uint16;  // 16 bits
typedef signed long         int32;   // 32 bits
typedef unsigned long       uint32;  // 32 bits
typedef signed long long    int64;   // 64 bits
typedef unsigned long long  uint64;  // 64 bits


/* Definiçao de um tipo para as cores dos leds */
typedef enum { 
    COR_OFF,
    COR_VERDE, 
    COR_VERMELHO,                           
} T_COR_LED;

#endif //_MYTYPES_H_
