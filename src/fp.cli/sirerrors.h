/* ======================================================================
Projecto:   ********
Modulo:     ********
Copyright:  SIR@PT Inovacao
Descricao:  Biblioteca de funções de uso geral.

Autor:      Vitor Mirones (VM), Álvaro Corga (AC)

Historico:  2003.10.01  - Criacao do modulo V1.0.0.0

======================================================================= */
#ifndef SIR_SIRERROR_H
#define SIR_SIRERROR_H

// Famílias de erros
#define ERROR_FAMILY_PDH            0x01
#define ERROR_FAMILY_SDH            0x02
#define ERROR_FAMILY_XDSL           0x03
#define ERROR_FAMILY_ATM            0x04
#define ERROR_FAMILY_ETH            0x05
#define ERROR_FAMILY_BD             0x06
#define ERROR_FAMILY_IPC            0x07
#define ERROR_FAMILY_HARDWARE       0x08
#define ERROR_FAMILY_APPLICATION    0x09
#define ERROR_FAMILY_SYSTEM         0x0A

// Níveis de severidade
#define ERROR_SEVERITY_EMERGENCY       0x00	//	system is unusable
#define ERROR_SEVERITY_ALERT           0x01	//	action must be taken immediately
#define ERROR_SEVERITY_CRITICAL        0x02	//	critical conditions
#define ERROR_SEVERITY_ERROR           0x03	//	error conditions
#define ERROR_SEVERITY_WARNING         0x04	//	warning conditions
#define ERROR_SEVERITY_NOTICE          0x05	//	normal but significant condition
#define ERROR_SEVERITY_INFORMATIONAL   0x06	//	informational messages
#define ERROR_SEVERITY_DEBUG	       0x07	//	debug-level messages

// Códigos de erro
//#define ERROR_CODE_XXXX				0xXXXX
   //Erros para a familia ATM
   #define ERROR_IDX_INVALIDO             0x0001 //indice(s) invalido


   // Erros para a familia IPC
   #define ERROR_CODE_NOFREECHAN          0x0001
   #define ERROR_CODE_CREATESOCKET        0x0002
   #define ERROR_CODE_BINDSOCKET          0x0003
   #define ERROR_CODE_CREATECLONE         0x0004
   #define ERROR_CODE_NOTOPENED           0x0005
   #define ERROR_CODE_SENDFAILED          0x0006
   #define ERROR_CODE_TIMEOUT             0x0007
   #define ERROR_CODE_INVALIDCH           0x0008
   #define ERROR_CODE_NOSUCHMSG           0x0010
   
   // Erros para a familia DB
   #define ERROR_CODE_OPENFAILED          0x0001
   #define ERROR_CODE_SELECTFAILED        0x0002

   // Erros para a familia System

// Macros de utilização
#define SIR_ERROR(fam,sev,tip)      (((fam&0x000000FF)<<24) | ((sev&0x000000FF)<<16) | (tip & 0x0000FFFF))
#define FAMILY_ERROR(sir_error)     (((sir_error & 0xFF000000)>>24))
#define SEVERITY_ERROR(sir_error)   (((sir_error & 0x00FF0000)>>16))
#define TYPE_ERROR(sir_error)       ((sir_error & 0x0000FFFF))

#endif // SIR_SIRERROR_H

