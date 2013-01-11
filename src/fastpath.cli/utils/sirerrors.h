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

// Famï¿½lias de erros
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
#define ERROR_FAMILY_HWGPON         0x50
#define ERROR_FAMILY_HWSWITCH       0x51     // New error family dedicated to the switch

// Níveis de severidade
#define ERROR_SEVERITY_EMERGENCY       0x00	//	system is unusable
#define ERROR_SEVERITY_ALERT           0x01	//	action must be taken immediately
#define ERROR_SEVERITY_CRITICAL        0x02	//	critical conditions
#define ERROR_SEVERITY_ERROR           0x03	//	error conditions
#define ERROR_SEVERITY_WARNING         0x04	//	warning conditions
#define ERROR_SEVERITY_NOTICE          0x05	//	normal but significant condition
#define ERROR_SEVERITY_INFORMATIONAL   0x06	//	informational messages
#define ERROR_SEVERITY_DEBUG	       0x07	//	debug-level messages


//#define _ACTUALIZACAO_
//----------------------------------------------------------------------//
//                     Codigos de erros (retornos)                      //
//----------------------------------------------------------------------//
// --------------------------------
// Erros comuns a varias famílias (0x00xx)
// --------------------------------
#define ERROR_CODE_OK                  0x0000   // Ok
#define ERROR_CODE_INVALIDPARAM        0x0001   // Parametro(s) de entrada invalido(s)
#define ERROR_CODE_NOTIMPLEMENTED      0x0002   // Funcionalidade nao implementada
#define ERROR_CODE_NOSUCHNAME          0x0003   // Parametro (ou nome) desconhecido
#define ERROR_CODE_READONLY            0x0004   // Valor de leitura (nao permite cnfigurar)
#define ERROR_CODE_WRONGNUMBER         0x0005   // nº de elementos invalido
#define ERROR_CODE_FULLTABLE           0x0006   // atingiu nº maximo de elementos
#define ERROR_CODE_EMPTYTABLE          0x0007   // Tabela vazia
#define ERROR_CODE_NOTALLOWED          0x0008   // Sem previlegios para realizar a operacao
#define ERROR_CODE_USED                0x0009   // O recurso esta ocupado
#define ERROR_CODE_NOTUSED             0x000A   // O recurso nao esta ocupado
#define ERROR_CODE_NOTPRESENT          0x000B   // O recurso nao esta disponivel
#define ERROR_CODE_WRONGADMINSTATE     0x000C   // O estado administrativo nao permite a operacao
#define ERROR_CODE_DUPLICATENAME       0x000D
#define ERROR_CODE_WRONGSTATE          0x000E
#define ERROR_CODE_NOTSUPPORTED        0x000F
#define ERROR_CODE_ABNORMALCONTEXT     0x0010   // Contexto anormal (normalmente resultande de validacoes insuficientes)
#define ERROR_CODE_WRONGSIZE           0x0011


// --------------------------------
//  ERROR_FAMILY_HARDWARE
// --------------------------------
#define ERROR_CODE_TIMEOUT                 0x0100 
#define ERROR_CODE_INVALIDSIZE             0x0101 
#define ERROR_CODE_INVALIDPACKET           0x0102 
#define ERROR_CODE_INVALIDCRC              0x0103 
#define ERROR_CODE_OMCINOK                 0x0104 
#define ERROR_CODE_OMCINOK_RESPONSE        0x0105
 
#define ERROR_CODE_APP_NOTREADY            0x010B 

//ERROS PARA OMCI
#define ERROR_CODE_OMCI_INVALIDO           0x0110
#define ERROR_CODE_OMCI_INVALIDSTATUS      0x0111
#define ERROR_CODE_OMCI_INVALCOMMSTATUS    0x0112
//ERROS PARA PERFIS
#define ERROR_CODE_PERFIL_INVALIDO         0x0120
#define ERROR_CODE_PERFIL_ID_OCUPADO       0x0121
#define ERROR_CODE_PERFIL_ID_INVALIDO      0x0122
#define ERROR_CODE_PERFIL_JAEXISTE         0x0123
#define ERROR_CODE_PERFIL_ID_INDISPONIVEIS 0x0124
#define ERROR_CODE_PERFIL_HARDWARE_ERROR   0x0125

// --------------------------------
//  ERROR_FAMILY_HWSWITCH
// --------------------------------
#define ERROR_CODE_HWSWITCH_NOK                  0x0020


// Macros de utilização
#define SIR_ERROR(fam,sev,tip)      (((fam&0x000000FF)<<24) | ((sev&0x000000FF)<<16) | (tip & 0x0000FFFF))
#define FAMILY_ERROR(sir_error)     (((sir_error & 0xFF000000)>>24))
#define SEVERITY_ERROR(sir_error)   (((sir_error & 0x00FF0000)>>16))
#define TYPE_ERROR(sir_error)       ((sir_error & 0x0000FFFF))

#endif // SIR_SIRERROR_H
