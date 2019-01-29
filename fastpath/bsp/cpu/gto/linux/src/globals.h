#ifndef _MYTYPES_H_
#define _MYTYPES_H_

//#include "config.h"



#ifdef __cplusplus
    #define EXTERN_C extern "C"
#else
    #define EXTERN_C extern
#endif

              

/* retorno de fun�oes */
#define EXIT_NO_FUNC  -4
#define EXIT_LIB_INI  -3
#define EXIT_MODULE   -2
#define EXIT_NOK      -1
#define EXIT_OK        0

/* tipos de loops */
#define CONF_NO_LOOP     0
#define CONF_LOOP_LOCAL  1
#define CONF_LOOP_REMOTO 2

/* Mapeamentos */
#undef  BEGIN_PACKED
#undef  END_PACKED
#define BEGIN_PACKED
#define END_PACKED  __attribute__  ((packed))

#define CS_UNIPROC         0x1000000

//#define PORTO_TX_MSG_CX              6000
#define PORTO_RX_MSG_GESTAO          6000
#define PORTO_RX_TRAP_CONTROLO       6001
//#define PORTO_TX_MSG_UNICOM          7000
#define PORTO_TX_TRAP_CONTROLO       5001
//#define PORTO_TX_TRAP_UNI2CX         6002
//#define PORTO_COM_CX_2_UNICOM_SE     6003
//#define PORTO_COM_TRAP_UNI2CONTROLO  6004
#define PORTO_RX_MSG_BUGA            6005  //claudia
#define PORTO_TX_MSG_BUGA            6905  //claudia

//#define IP_CX_MASTER           0xC0A8C80A  //192.168.200.010
//#define IP_CX_SLAVE            0xC0A8C80B  //192.168.200.011
//#define IP_UNICOM              0xC0A8C801  //192.168.200.001
//#define IP_UNICOM_1            0xC0A8C814  //192.168.200.020
#define IP_LOCALHOST           0x7f000001  //127.000.000.001

//#define SECCAO_DCC_MSOH 0
//#define SECCAO_DCC_RSOH 0x40

//#define MODULO_UNICOM        0x41
//#define MODULO_UNICOM_SE     0x44
#define MODULO_OLT7_8CH      0x69


/* Nomenclatura para os agregados Este/Oeste */
/*
#define ESTE  0     //posicao nos fpgas
#define OESTE 1
*/

#define MAX_ID_STRING_SIZE    15   //tamanho maximo para a identificacao de circuitos
        
#ifndef UINT_TYPEDEFS
#define UINT_TYPEDEFS
typedef unsigned char  UINT8;      //  8 bits
typedef unsigned short UINT16;     // 16 bits
typedef unsigned long  UINT32;     // 32 bits
typedef unsigned long long  UINT64;     // 64 bits
#endif

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

#ifndef _SAL_TYPES_H               // This condition only applies when Broadcom drivers are used
typedef char           int8;       //  8 bits
typedef unsigned char  uint8;      //  8 bits
typedef unsigned short uint16;     // 16 bits
typedef unsigned long  uint32;     // 32 bits
typedef unsigned long long uint64;     // 64 bits
#endif

/* Defini�ao de um tipo para as cores dos leds */
typedef enum { 
    COR_OFF,
    COR_VERDE, 
    COR_VERMELHO,                           
} T_COR_LED;


#include "debug.h"

#endif //_MYTYPES_H_
