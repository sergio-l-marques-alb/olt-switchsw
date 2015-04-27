#ifndef CTRL_IPC_PROTOCOL_H
#define CTRL_IPC_PROTOCOL_H
#include <stdio.h>

#define protocoloID          1


//definicao das variaveis a partir do modulo de controlo
#define srcIdCtr1Adsl    10000 // Processos de Controlo 10000 e 19999 (layer 2 -> layer 1)
#define srcIdCtr1Atm     10001
#define dstIdHwAdsl      20000 // Processos de Hardware 20000 e 29999 (layer 1 -> layer 2)
#define dstIdHwAtm       20001
#define srcIdCtr2Ag      30000 // Processos de Controlo 30000 e 39999 (layer 2 -> layer 3)
#define srcIdCtr2Ti      30001
#define srcIdCtr2Cli     30002
#define dstIdSwAg        40000 // Processos de Software 40000 e 49999 (layer 3 -> layer 2)
#define dstIdSwTi        40001
#define dstIdSwCli       40002
 
//  ======================================
// | estrutura da mensagem do Get/GetNext |
//  ======================================
//
// Comando
// -------
//
// cabecalho
//    flags=fgCmd 
//    msgId=codigo de comando_estrutura (ver na lista dos msgID)
//    infoDim=tamanho do numero de indices da estrutura, caso nao tenha indice colocar a zero
// info
//    info[0]= indice1 (ou zero caso nao seja indexada  ou se esteja a pedir getnext 
//                     para se obter o 1o. indice da tabela)
//    ... ate ao numero de indices
//
// Resposta
// --------
//
// cabecalho
//    flags=fgRpOk (ou fgRpNOk)
//    msgId=codigo da estrutura (ver na lista dos msgID)
//    infoDim=tamanho da estrutura ou um no caso de erro
// info
//    info[0]= primeiro elemento da estrutura ou cod de erro
//    ... ate info[x]=ultimo elemento da estrutura

//  ==============================
// | estrutura da mensagem do Set |
//  ==============================
//
// Comando
// -------
//
// cabecalho
//    flags=fgCmd
//    msgId=codigo de comando_estrutura (ver na lista dos msgID)
//    infoDim=tamanho da estrutura ou tamanho de um unsigned int no caso de erro
// info
//    info[0]= primeiro elemento da estrutura
//    ... ate info[x]=ultimo elemento da estrutura
//
// Resposta
// --------
//
// cabecalho
//    flags=fgRpOk (ou fgRpNOk)
//    msgId=codigo da estrutura (ver na lista dos msgID)
//    infoDim=um
// info
//    info[0]=zero se foi efectuado com sucesso
//           ou cod de erro




// Lista das flags
// Flags  de Comandos
#define fgCmd               0x00000000

// Flags de Resposta
#define fgRpOk              0x00000001
#define fgRpNOk             0x00000003

// Lista de versoes
#define VER_COD_NULL      0
#define VER_COD_LINUX     1
#define VER_COD_FS        2
#define VER_COD_THTTPD    3
#define VER_COD_TIWEB     4
#define VER_COD_AGENTE    5
#define VER_COD_AGTOOL    6
#define VER_COD_SNMPD     7
#define VER_COD_CONTROLO  8
#define VER_COD_ATM       9
#define VER_COD_ATM_MK    10
#define VER_COD_ATM_MOD   11
#define VER_COD_XDSL      12
#define VER_COD_XDSL_GS   13
#define VER_COD_ADSL_A    14
#define VER_COD_ADSL_B    15
#define VER_COD_SHDSL     16

// Lista de tipos
#define VER_TIP_HW        1
#define VER_TIP_FW        2
#define VER_TIP_SW        3

//Lista dos msgId
#define reset            1
//#define init_defeito     2
#define ini_down         3 //inicio de download total
#define ini_up           4 //inicio de upload total (interfaces)
#define ini_down_int     5 //inicio de download interfaces
#define ini_down_cru     6 //inicio de download cruzamentos
#define fim_down         7 //fim de download
#define fim_up           8 //fim de upload 
#define shutdown         9 //termina o processo
#define send_trap       10 //envio_controlo/recepcao_agente do trap para enviar a gestao
#define send_trap_adsl  11 //envio_controlo/recepcao_agente do trap para enviar a gestao
#define novo_debug      12 //envio_controlo/recepcao_agente do trap para enviar a gestao
#define get_desc_modulo 13 //estrutura DESC_MODULO
#define send_trap_shdsl 14 //envio_controlo/recepcao_agente do trap para enviar a gestao
#define ctrl_app        15 //Start/stop da aplicacao
#define send_qual_shdsl 16 //qualidade

//Estados da aplicacao
#define AppStatusStopped   0x0000
#define AppStatusFailled   0x0001
#define AppStatusBuilding  0x0010
#define AppStatusRunning   0x0020

// reservados de 12 a 29 para uso futuro
//estruturas baseadas nas mibs da ptin
#define ptin_getnohw_AG         30 //estrutura alarmgroup
#define ptin_getgohw_AG         31 //estrutura alarmgroup
#define ptin_gntnohw_AG         32 //estrutura alarmgroup
#define ptin_gntgohw_AG         33 //estrutura alarmgroup
#define ptin_setnohw_AG         34 //estrutura alarmgroup
#define ptin_setgohw_AG         35 //estrutura alarmgroup
#define ptin_setgohw_AG_Reset5  36 //estrutura alarmgroup
#define ptin_setgohw_AG_Reset6  37 //estrutura alarmgroup
#define ptin_getnohw_AT         40 //estrutura alarmtable
#define ptin_getgohw_AT         41 //estrutura alarmtable
#define ptin_gntnohw_AT         42 //estrutura alarmtable
#define ptin_gntgohw_AT         43 //estrutura alarmtable
#define ptin_setnohw_AT         44 //estrutura alarmtable
#define ptin_setgohw_AT         45 //estrutura alarmtable
#define ptin_getnohw_BTP        50 //estrutura boardtableptin
#define ptin_getgohw_BTP        51 //estrutura boardtableptin
#define ptin_gntnohw_BTP        52 //estrutura boardtableptin
#define ptin_gntgohw_BTP        53 //estrutura boardtableptin
#define ptin_setnohw_BTP        54 //estrutura boardtableptin
#define ptin_setgohw_BTP        55 //estrutura boardtableptin
#define ptin_getnohw_BPN        60 //estrutura boardptinnumber
#define ptin_getgohw_BPN        61 //estrutura boardptinnumber
#define ptin_gntnohw_BPN        62 //estrutura boardptinnumber
#define ptin_gntgohw_BPN        63 //estrutura boardptinnumber
#define ptin_setnohw_BPN        64 //estrutura boardptinnumber
#define ptin_setgohw_BPN        65 //estrutura boardptinnumber
#define ptin_getnohw_GCCT       70 //estrutura genericcruzaconfigtable
#define ptin_getgohw_GCCT       71 //estrutura genericcruzaconfigtable
#define ptin_gntnohw_GCCT       72 //estrutura genericcruzaconfigtable
#define ptin_gntgohw_GCCT       73 //estrutura genericcruzaconfigtable
#define ptin_setnohw_GCCT       74 //estrutura genericcruzaconfigtable
#define ptin_setgohw_GCCT       75 //estrutura genericcruzaconfigtable
#define ptin_getnohw_GCIT       80 //estrutura genericcruzaindextable
#define ptin_getgohw_GCIT       81 //estrutura genericcruzaindextable
#define ptin_gntnohw_GCIT       82 //estrutura genericcruzaindextable
#define ptin_gntgohw_GCIT       83 //estrutura genericcruzaindextable
#define ptin_setnohw_GCIT       84 //estrutura genericcruzaindextable
#define ptin_setgohw_GCIT       85 //estrutura genericcruzaindextable
#define ptin_getnohw_GCLCT      90 //estrutura genericcruzalogconfigtable
#define ptin_getgohw_GCLCT      91 //estrutura genericcruzalogconfigtable
#define ptin_gntnohw_GCLCT      92 //estrutura genericcruzalogconfigtable
#define ptin_gntgohw_GCLCT      93 //estrutura genericcruzalogconfigtable
#define ptin_setnohw_GCLCT      94 //estrutura genericcruzalogconfigtable
#define ptin_setgohw_GCLCT      95 //estrutura genericcruzalogconfigtable
#define ptin_getnohw_GCLIT     100 //estrutura genericcruzalogicoindextable
#define ptin_getgohw_GCLIT     101 //estrutura genericcruzalogicoindextable
#define ptin_gntnohw_GCLIT     102 //estrutura genericcruzalogicoindextable
#define ptin_gntgohw_GCLIT     103 //estrutura genericcruzalogicoindextable
#define ptin_setnohw_GCLIT     104 //estrutura genericcruzalogicoindextable
#define ptin_setgohw_GCLIT     105 //estrutura genericcruzalogicoindextable
#define ptin_getnohw_GECT      110 //estrutura genericequipconfigtable
#define ptin_getgohw_GECT      111 //estrutura genericequipconfigtable
#define ptin_gntnohw_GECT      112 //estrutura genericequipconfigtable
#define ptin_gntgohw_GECT      113 //estrutura genericequipconfigtable
#define ptin_setnohw_GECT      114 //estrutura genericequipconfigtable
#define ptin_setgohw_GECT      115 //estrutura genericequipconfigtable
#define ptin_setgohw_GECT_data 116 //estrutura genericequipconfigtable
#define ptin_setgohw_GECT_hora 117 //estrutura genericequipconfigtable
#define ptin_getnohw_GES       120 //estrutura genericequipsinc
#define ptin_getgohw_GES       121 //estrutura genericequipsinc
#define ptin_gntnohw_GES       122 //estrutura genericequipsinc
#define ptin_gntgohw_GES       123 //estrutura genericequipsinc
#define ptin_setnohw_GES       124 //estrutura genericequipsinc
#define ptin_setgohw_GES       125 //estrutura genericequipsinc
#define ptin_getnohw_GEST      130 //estrutura genericequipsinctable
#define ptin_getgohw_GEST      131 //estrutura genericequipsinctable
#define ptin_gntnohw_GEST      132 //estrutura genericequipsinctable
#define ptin_gntgohw_GEST      133 //estrutura genericequipsinctable
#define ptin_setnohw_GEST      134 //estrutura genericequipsinctable
#define ptin_setgohw_GEST      135 //estrutura genericequipsinctable
#define ptin_getnohw_GS        140 //estrutura genericsystem
#define ptin_getgohw_GS        141 //estrutura genericsystem
#define ptin_gntnohw_GS        142 //estrutura genericsystem
#define ptin_gntgohw_GS        143 //estrutura genericsystem
#define ptin_setnohw_GS        144 //estrutura genericsystem
#define ptin_setgohw_GS        145 //estrutura genericsystem
#define ptin_getnohw_GINT      150 //estrutura genericifnumbertable
#define ptin_getgohw_GINT      151 //estrutura genericifnumbertable
#define ptin_gntnohw_GINT      152 //estrutura genericifnumbertable
#define ptin_gntgohw_GINT      153 //estrutura genericifnumbertable
#define ptin_setnohw_GINT      154 //estrutura genericifnumbertable
#define ptin_setgohw_GINT      155 //estrutura genericifnumbertable
#define ptin_getnohw_GIT       160 //estrutura genericinterfacetable
#define ptin_getgohw_GIT       161 //estrutura genericinterfacetable
#define ptin_gntnohw_GIT       162 //estrutura genericinterfacetable
#define ptin_gntgohw_GIT       163 //estrutura genericinterfacetable
#define ptin_setnohw_GIT       164 //estrutura genericinterfacetable
#define ptin_setgohw_GIT       165 //estrutura genericinterfacetable
#define ptin_getnohw_GILIN     170 //estrutura geninterfacelogicaindexnext
#define ptin_getgohw_GILIN     171 //estrutura geninterfacelogicaindexnext
#define ptin_gntnohw_GILIN     172 //estrutura geninterfacelogicaindexnext
#define ptin_gntgohw_GILIN     173 //estrutura geninterfacelogicaindexnext
#define ptin_setnohw_GILIN     174 //estrutura geninterfacelogicaindexnext
#define ptin_setgohw_GILIN     175 //estrutura geninterfacelogicaindexnext
#define ptin_getnohw_GILT      180 //estrutura geninterfacelogtable
#define ptin_getgohw_GILT      181 //estrutura geninterfacelogtable
#define ptin_gntnohw_GILT      182 //estrutura geninterfacelogtable
#define ptin_gntgohw_GILT      183 //estrutura geninterfacelogtable
#define ptin_setnohw_GILT      184 //estrutura geninterfacelogtable
#define ptin_setgohw_GILT      185 //estrutura geninterfacelogtable
#define ptin_getnohw_GPLIN     190 //estrutura genportologindexnext 
#define ptin_getgohw_GPLIN     191 //estrutura genportologindexnext 
#define ptin_gntnohw_GPLIN     192 //estrutura genportologindexnext 
#define ptin_gntgohw_GPLIN     193 //estrutura genportologindexnext 
#define ptin_setnohw_GPLIN     194 //estrutura genportologindexnext 
#define ptin_setgohw_GPLIN     195 //estrutura genportologindexnext 
#define ptin_getnohw_GPLT      200 //estrutura genportologtable
#define ptin_getgohw_GPLT      201 //estrutura genportologtable
#define ptin_gntnohw_GPLT      202 //estrutura genportologtable
#define ptin_gntgohw_GPLT      203 //estrutura genportologtable
#define ptin_setnohw_GPLT      204 //estrutura genportologtable
#define ptin_setgohw_GPLT      205 //estrutura genportologtable
#define ptin_getnohw_GBN       210 //estrutura genericboardnumber
#define ptin_getgohw_GBN       211 //estrutura genericboardnumber
#define ptin_gntnohw_GBN       212 //estrutura genericboardnumber
#define ptin_gntgohw_GBN       213 //estrutura genericboardnumber
#define ptin_setnohw_GBN       214 //estrutura genericboardnumber
#define ptin_setgohw_GBN       215 //estrutura genericboardnumber
#define ptin_getnohw_GBT       220 //estrutura genericboardtable
#define ptin_getgohw_GBT       221 //estrutura genericboardtable
#define ptin_gntnohw_GBT       222 //estrutura genericboardtable
#define ptin_gntgohw_GBT       223 //estrutura genericboardtable
#define ptin_setnohw_GBT       224 //estrutura genericboardtable
#define ptin_setgohw_GBT       225 //estrutura genericboardtable
#define ptin_getnohw_HPT       230 //estrutura hwporttable
#define ptin_getgohw_HPT       231 //estrutura hwporttable
#define ptin_gntnohw_HPT       232 //estrutura hwporttable
#define ptin_gntgohw_HPT       233 //estrutura hwporttable
#define ptin_setnohw_HPT       234 //estrutura hwporttable
#define ptin_setgohw_HPT       235 //estrutura hwporttable
#define ptin_getnohw_ITP       240 //estrutura interfacetableptin
#define ptin_getgohw_ITP       241 //estrutura interfacetableptin
#define ptin_gntnohw_ITP       242 //estrutura interfacetableptin
#define ptin_gntgohw_ITP       243 //estrutura interfacetableptin
#define ptin_setnohw_ITP       244 //estrutura interfacetableptin
#define ptin_setgohw_ITP       245 //estrutura interfacetableptin
#define ptin_getnohw_INTP      250 //estrutura ifnumbertableptin
#define ptin_getgohw_INTP      251 //estrutura ifnumbertableptin
#define ptin_gntnohw_INTP      252 //estrutura ifnumbertableptin
#define ptin_gntgohw_INTP      253 //estrutura ifnumbertableptin
#define ptin_setnohw_INTP      254 //estrutura ifnumbertableptin
#define ptin_setgohw_INTP      255 //estrutura ifnumbertableptin
#define ptin_getnohw_ILPT      260 //estrutura interfacelogptintable
#define ptin_getgohw_ILPT      261 //estrutura interfacelogptintable
#define ptin_gntnohw_ILPT      262 //estrutura interfacelogptintable
#define ptin_gntgohw_ILPT      263 //estrutura interfacelogptintable
#define ptin_setnohw_ILPT      264 //estrutura interfacelogptintable
#define ptin_setgohw_ILPT      265 //estrutura interfacelogptintable
#define ptin_getnohw_ILPIN     270 //estrutura iflogptinindexnext
#define ptin_getgohw_ILPIN     271 //estrutura iflogptinindexnext
#define ptin_gntnohw_ILPIN     272 //estrutura iflogptinindexnext
#define ptin_gntgohw_ILPIN     273 //estrutura iflogptinindexnext
#define ptin_setnohw_ILPIN     274 //estrutura iflogptinindexnext
#define ptin_setgohw_ILPIN     275 //estrutura iflogptinindexnext
#define ptin_getnohw_IPLPT     280 //estrutura ifportologptintable
#define ptin_getgohw_IPLPT     281 //estrutura ifportologptintable
#define ptin_gntnohw_IPLPT     282 //estrutura ifportologptintable
#define ptin_gntgohw_IPLPT     283 //estrutura ifportologptintable
#define ptin_setnohw_IPLPT     284 //estrutura ifportologptintable
#define ptin_setgohw_IPLPT     285 //estrutura ifportologptintable
#define ptin_getnohw_IPLPIN    290 //estrutura ifportologptinindexnext
#define ptin_getgohw_IPLPIN    291 //estrutura ifportologptinindexnext
#define ptin_gntnohw_IPLPIN    292 //estrutura ifportologptinindexnext
#define ptin_gntgohw_IPLPIN    293 //estrutura ifportologptinindexnext
#define ptin_setnohw_IPLPIN    294 //estrutura ifportologptinindexnext
#define ptin_setgohw_IPLPIN    295 //estrutura ifportologptinindexnext
#define ptin_getnohw_SCCT      300 //estrutura sistemacruzaconfigtable
#define ptin_getgohw_SCCT      301 //estrutura sistemacruzaconfigtable
#define ptin_gntnohw_SCCT      302 //estrutura sistemacruzaconfigtable
#define ptin_gntgohw_SCCT      303 //estrutura sistemacruzaconfigtable
#define ptin_setnohw_SCCT      304 //estrutura sistemacruzaconfigtable
#define ptin_setgohw_SCCT      305 //estrutura sistemacruzaconfigtable
#define ptin_getnohw_SCIT      310 //estrutura sistemacruzaindextable 
#define ptin_getgohw_SCIT      311 //estrutura sistemacruzaindextable 
#define ptin_gntnohw_SCIT      312 //estrutura sistemacruzaindextable 
#define ptin_gntgohw_SCIT      313 //estrutura sistemacruzaindextable 
#define ptin_setnohw_SCIT      314 //estrutura sistemacruzaindextable 
#define ptin_setgohw_SCIT      315 //estrutura sistemacruzaindextable 
#define ptin_getnohw_SCLCT     320 //estrutura sistemacruzalogconfigtable
#define ptin_getgohw_SCLCT     321 //estrutura sistemacruzalogconfigtable
#define ptin_gntnohw_SCLCT     322 //estrutura sistemacruzalogconfigtable
#define ptin_gntgohw_SCLCT     323 //estrutura sistemacruzalogconfigtable
#define ptin_setnohw_SCLCT     324 //estrutura sistemacruzalogconfigtable
#define ptin_setgohw_SCLCT     325 //estrutura sistemacruzalogconfigtable
#define ptin_getnohw_SCLIT     330 //estrutura sistemacruzalogicoindextable 
#define ptin_getgohw_SCLIT     331 //estrutura sistemacruzalogicoindextable 
#define ptin_gntnohw_SCLIT     332 //estrutura sistemacruzalogicoindextable 
#define ptin_gntgohw_SCLIT     333 //estrutura sistemacruzalogicoindextable 
#define ptin_setnohw_SCLIT     334 //estrutura sistemacruzalogicoindextable 
#define ptin_setgohw_SCLIT     335 //estrutura sistemacruzalogicoindextable 
#define ptin_getnohw_SECT      340 //estrutura sistemaequipconfigtable
#define ptin_getgohw_SECT      341 //estrutura sistemaequipconfigtable
#define ptin_gntnohw_SECT      342 //estrutura sistemaequipconfigtable
#define ptin_gntgohw_SECT      343 //estrutura sistemaequipconfigtable
#define ptin_setnohw_SECT      344 //estrutura sistemaequipconfigtable
#define ptin_setgohw_SECT      345 //estrutura sistemaequipconfigtable
#define ptin_setgohw_SECT_data 346 //estrutura sistemaequipconfigtable
#define ptin_setgohw_SECT_hora 347 //estrutura sistemaequipconfigtable
#define ptin_getnohw_SES       350 //estrutura sistemaequipsinc
#define ptin_getgohw_SES       351 //estrutura sistemaequipsinc
#define ptin_gntnohw_SES       352 //estrutura sistemaequipsinc
#define ptin_gntgohw_SES       353 //estrutura sistemaequipsinc
#define ptin_setnohw_SES       354 //estrutura sistemaequipsinc
#define ptin_setgohw_SES       355 //estrutura sistemaequipsinc
#define ptin_getnohw_SEST      360 //estrutura sistemaequipsinctable
#define ptin_getgohw_SEST      361 //estrutura sistemaequipsinctable
#define ptin_gntnohw_SEST      362 //estrutura sistemaequipsinctable
#define ptin_gntgohw_SEST      363 //estrutura sistemaequipsinctable
#define ptin_setnohw_SEST      364 //estrutura sistemaequipsinctable
#define ptin_setgohw_SEST      365 //estrutura sistemaequipsinctable
#define ptin_getnohw_SGC       370 //estrutura sistemagestaoconfig
#define ptin_getgohw_SGC       371 //estrutura sistemagestaoconfig
#define ptin_gntnohw_SGC       372 //estrutura sistemagestaoconfig
#define ptin_gntgohw_SGC       373 //estrutura sistemagestaoconfig
#define ptin_setnohw_SGC       374 //estrutura sistemagestaoconfig
#define ptin_setgohw_SGC       375 //estrutura sistemagestaoconfig
#define ptin_getnohw_SGMA      380 //estrutura sistemagestaomodoacesso
#define ptin_getgohw_SGMA      381 //estrutura sistemagestaomodoacesso
#define ptin_gntnohw_SGMA      382 //estrutura sistemagestaomodoacesso
#define ptin_gntgohw_SGMA      383 //estrutura sistemagestaomodoacesso
#define ptin_setnohw_SGMA      384 //estrutura sistemagestaomodoacesso
#define ptin_setgohw_SGMA      385 //estrutura sistemagestaomodoacesso
#define ptin_getnohw_EIT       390 //estrutura equipindextable
#define ptin_getgohw_EIT       391 //estrutura equipindextable
#define ptin_gntnohw_EIT       392 //estrutura equipindextable
#define ptin_gntgohw_EIT       393 //estrutura equipindextable
#define ptin_setnohw_EIT       394 //estrutura equipindextable
#define ptin_setgohw_EIT       395 //estrutura equipindextable
#define ptin_getnohw_SPAT      400 //estrutura sistemapontosacessotable 
#define ptin_getgohw_SPAT      401 //estrutura sistemapontosacessotable 
#define ptin_gntnohw_SPAT      402 //estrutura sistemapontosacessotable 
#define ptin_gntgohw_SPAT      403 //estrutura sistemapontosacessotable 
#define ptin_setnohw_SPAT      404 //estrutura sistemapontosacessotable 
#define ptin_setgohw_SPAT      405 //estrutura sistemapontosacessotable 
#define ptin_getnohw_SP        410 //estrutura sistemaptin
#define ptin_getgohw_SP        411 //estrutura sistemaptin
#define ptin_gntnohw_SP        412 //estrutura sistemaptin
#define ptin_gntgohw_SP        413 //estrutura sistemaptin
#define ptin_setnohw_SP        414 //estrutura sistemaptin
#define ptin_setgohw_SP        415 //estrutura sistemaptin
#define ptin_setgohw_SP_data   416 //estrutura sistemaptin
#define ptin_setgohw_SP_hora   417 //estrutura sistemaptin
#define ptin_setgohw_SP_ip     418 //estrutura sistemaptin systemip
#define ptin_getnohw_SST       420 //estrutura sistemasecurizacaotable
#define ptin_getgohw_SST       421 //estrutura sistemasecurizacaotable
#define ptin_gntnohw_SST       422 //estrutura sistemasecurizacaotable
#define ptin_gntgohw_SST       423 //estrutura sistemasecurizacaotable
#define ptin_setnohw_SST       424 //estrutura sistemasecurizacaotable
#define ptin_setgohw_SST       425 //estrutura sistemasecurizacaotable
#define ptin_getnohw_SSCT      430 //estrutura sistemastm1configtable
#define ptin_getgohw_SSCT      431 //estrutura sistemastm1configtable
#define ptin_gntnohw_SSCT      432 //estrutura sistemastm1configtable
#define ptin_gntgohw_SSCT      433 //estrutura sistemastm1configtable
#define ptin_setnohw_SSCT      434 //estrutura sistemastm1configtable
#define ptin_setgohw_SSCT      435 //estrutura sistemastm1configtable
#define ptin_getnohw_GSPT      440 //estrutura genericstm1posicaotable
#define ptin_getgohw_GSPT      441 //estrutura genericstm1posicaotable
#define ptin_gntnohw_GSPT      442 //estrutura genericstm1posicaotable
#define ptin_gntgohw_GSPT      443 //estrutura genericstm1posicaotable
#define ptin_setnohw_GSPT      444 //estrutura genericstm1posicaotable
#define ptin_setgohw_GSPT      445 //estrutura genericstm1posicaotable
#define ptin_getnohw_GTINT     450 //estrutura generictrailindexnexttable
#define ptin_getgohw_GTINT     451 //estrutura generictrailindexnexttable
#define ptin_gntnohw_GTINT     452 //estrutura generictrailindexnexttable
#define ptin_gntgohw_GTINT     453 //estrutura generictrailindexnexttable
#define ptin_setnohw_GTINT     454 //estrutura generictrailindexnexttable
#define ptin_setgohw_GTINT     455 //estrutura generictrailindexnexttable
#define ptin_getnohw_GTCT      460 //estrutura generictrailconfigtable
#define ptin_getgohw_GTCT      461 //estrutura generictrailconfigtable
#define ptin_gntnohw_GTCT      462 //estrutura generictrailconfigtable
#define ptin_gntgohw_GTCT      463 //estrutura generictrailconfigtable
#define ptin_setnohw_GTCT      464 //estrutura generictrailconfigtable
#define ptin_setgohw_GTCT      465 //estrutura generictrailconfigtable
#define ptin_getnohw_GNFCDT    470 //estrutura gennearendconfigdesemptable
#define ptin_getgohw_GNFCDT    471 //estrutura gennearendconfigdesemptable
#define ptin_gntnohw_GNFCDT    472 //estrutura gennearendconfigdesemptable
#define ptin_gntgohw_GNFCDT    473 //estrutura gennearendconfigdesemptable
#define ptin_setnohw_GNFCDT    474 //estrutura gennearendconfigdesemptable
#define ptin_setgohw_GNFCDT    475 //estrutura gennearendconfigdesemptable
#define ptin_getnohw_GFNCDT    480 //estrutura genfarrendconfigdesemptable
#define ptin_getgohw_GFNCDT    481 //estrutura genfarrendconfigdesemptable
#define ptin_gntnohw_GFNCDT    482 //estrutura genfarrendconfigdesemptable
#define ptin_gntgohw_GFNCDT    483 //estrutura genfarrendconfigdesemptable
#define ptin_setnohw_GFNCDT    484 //estrutura genfarrendconfigdesemptable
#define ptin_setgohw_GFNCDT    485 //estrutura genfarrendconfigdesemptable
#define ptin_getnohw_GPAT      490 //estrutura genericpontosacessotable
#define ptin_getgohw_GPAT      491 //estrutura genericpontosacessotable
#define ptin_gntnohw_GPAT      492 //estrutura genericpontosacessotable
#define ptin_gntgohw_GPAT      493 //estrutura genericpontosacessotable
#define ptin_setnohw_GPAT      494 //estrutura genericpontosacessotable
#define ptin_setgohw_GPAT      495 //estrutura genericpontosacessotable
#define ptin_getnohw_AIST      500 //estrutura atminterfacestatstable
#define ptin_getgohw_AIST      501 //estrutura atminterfacestatstable
#define ptin_gntnohw_AIST      502 //estrutura atminterfacestatstable
#define ptin_gntgohw_AIST      503 //estrutura atminterfacestatstable
#define ptin_setnohw_AIST      504 //estrutura atminterfacestatstable
#define ptin_setgohw_AIST      505 //estrutura atminterfacestatstable
#define ptin_getnohw_AVPST     510 //estrutura atmvpstatstable
#define ptin_getgohw_AVPST     511 //estrutura atmvpstatstable
#define ptin_gntnohw_AVPST     512 //estrutura atmvpstatstable
#define ptin_gntgohw_AVPST     513 //estrutura atmvpstatstable
#define ptin_setnohw_AVPST     514 //estrutura atmvpstatstable
#define ptin_setgohw_AVPST     515 //estrutura atmvpstatstable
#define ptin_getnohw_AVCST     520 //estrutura atmvcstatstable
#define ptin_getgohw_AVCST     521 //estrutura atmvcstatstable
#define ptin_gntnohw_AVCST     522 //estrutura atmvcstatstable
#define ptin_gntgohw_AVCST     523 //estrutura atmvcstatstable
#define ptin_setnohw_AVCST     524 //estrutura atmvcstatstable
#define ptin_setgohw_AVCST     525 //estrutura atmvcstatstable
#define ptin_getnohw_AF4OT     530 //estrutura atmf4oamtable
#define ptin_getgohw_AF4OT     531 //estrutura atmf4oamtable
#define ptin_gntnohw_AF4OT     532 //estrutura atmf4oamtable
#define ptin_gntgohw_AF4OT     533 //estrutura atmf4oamtable
#define ptin_setnohw_AF4OT     534 //estrutura atmf4oamtable
#define ptin_setgohw_AF4OT     535 //estrutura atmf4oamtable
#define ptin_getnohw_PACUVpT   540 //estrutura ptinAtmConfigUpcVplTable
#define ptin_getgohw_PACUVpT   541 //estrutura ptinAtmConfigUpcVplTable 
#define ptin_gntnohw_PACUVpT   542 //estrutura ptinAtmConfigUpcVplTable
#define ptin_gntgohw_PACUVpT   543 //estrutura ptinAtmConfigUpcVplTable
#define ptin_setnohw_PACUVpT   544 //estrutura ptinAtmConfigUpcVplTable
#define ptin_setgohw_PACUVpT   545 //estrutura ptinAtmConfigUpcVplTable
#define ptin_getnohw_PACUVcT   550 //estrutura ptinAtmConfigUpcVclTable
#define ptin_getgohw_PACUVcT   551 //estrutura ptinAtmConfigUpcVclTable
#define ptin_gntnohw_PACUVcT   552 //estrutura ptinAtmConfigUpcVclTable
#define ptin_gntgohw_PACUVcT   553 //estrutura ptinAtmConfigUpcVclTable
#define ptin_setnohw_PACUVcT   554 //estrutura ptinAtmConfigUpcVclTable
#define ptin_setgohw_PACUVcT   555 //estrutura ptinAtmConfigUpcVclTable
#define ptin_getnohw_PACUCT    560 //estrutura ptinAtmConfigUpcCbrTable
#define ptin_getgohw_PACUCT    561 //estrutura ptinAtmConfigUpcCbrTable
#define ptin_gntnohw_PACUCT    562 //estrutura ptinAtmConfigUpcCbrTable
#define ptin_gntgohw_PACUCT    563 //estrutura ptinAtmConfigUpcCbrTable
#define ptin_setnohw_PACUCT    564 //estrutura ptinAtmConfigUpcCbrTable
#define ptin_setgohw_PACUCT    565 //estrutura ptinAtmConfigUpcCbrTable
#define ptin_getnohw_PACURT    570 //estrutura ptinAtmConfigUpcRvbrTable
#define ptin_getgohw_PACURT    571 //estrutura ptinAtmConfigUpcRvbrTable
#define ptin_gntnohw_PACURT    572 //estrutura ptinAtmConfigUpcRvbrTable
#define ptin_gntgohw_PACURT    573 //estrutura ptinAtmConfigUpcRvbrTable
#define ptin_setnohw_PACURT    574 //estrutura ptinAtmConfigUpcRvbrTable
#define ptin_setgohw_PACURT    575 //estrutura ptinAtmConfigUpcRvbrTable
#define ptin_getnohw_PACUNT    580 //estrutura ptinAtmConfigUpcNvbrTable
#define ptin_getgohw_PACUNT    581 //estrutura ptinAtmConfigUpcNvbrTable
#define ptin_gntnohw_PACUNT    582 //estrutura ptinAtmConfigUpcNvbrTable
#define ptin_gntgohw_PACUNT    583 //estrutura ptinAtmConfigUpcNvbrTable
#define ptin_setnohw_PACUNT    584 //estrutura ptinAtmConfigUpcNvbrTable
#define ptin_setgohw_PACUNT    585 //estrutura ptinAtmConfigUpcNvbrTable
#define ptin_getnohw_PACUUT    590 //estrutura ptinAtmConfigUpcUbrTable
#define ptin_getgohw_PACUUT    591 //estrutura ptinAtmConfigUpcUbrTable
#define ptin_gntnohw_PACUUT    592 //estrutura ptinAtmConfigUpcUbrTable
#define ptin_gntgohw_PACUUT    593 //estrutura ptinAtmConfigUpcUbrTable
#define ptin_setnohw_PACUUT    594 //estrutura ptinAtmConfigUpcUbrTable
#define ptin_setgohw_PACUUT    595 //estrutura ptinAtmConfigUpcUbrTable
#define ptin_getnohw_PACUEBT   600 //estrutura ptinAtmConfigUpcEquipBuffersTable
#define ptin_getgohw_PACUEBT   601 //estrutura ptinAtmConfigUpcEquipBuffersTable
#define ptin_gntnohw_PACUEBT   602 //estrutura ptinAtmConfigUpcEquipBuffersTable
#define ptin_gntgohw_PACUEBT   603 //estrutura ptinAtmConfigUpcEquipBuffersTable
#define ptin_setnohw_PACUEBT   604 //estrutura ptinAtmConfigUpcEquipBuffersTable
#define ptin_setgohw_PACUEBT   605 //estrutura ptinAtmConfigUpcEquipBuffersTable

#define ptin_getnohw_PACUDT    610 //estrutura ptinAtmConfigUpcDescTable
#define ptin_getgohw_PACUDT    611 //estrutura ptinAtmConfigUpcDescTable
#define ptin_gntnohw_PACUDT    612 //estrutura ptinAtmConfigUpcDescTable
#define ptin_gntgohw_PACUDT    613 //estrutura ptinAtmConfigUpcDescTable
#define ptin_setnohw_PACUDT    614 //estrutura ptinAtmConfigUpcDescTable
#define ptin_setgohw_PACUDT    615 //estrutura ptinAtmConfigUpcDescTable
#define ptin_getnohw_PACUECBT  620 //estrutura ptinAtmConfigUpcEquipCbrBuffersTable
#define ptin_getgohw_PACUECBT  621 //estrutura ptinAtmConfigUpcEquipCbrBuffersTable
#define ptin_gntnohw_PACUECBT  622 //estrutura ptinAtmConfigUpcEquipCbrBuffersTable
#define ptin_gntgohw_PACUECBT  623 //estrutura ptinAtmConfigUpcEquipCbrBuffersTable
#define ptin_setnohw_PACUECBT  624 //estrutura ptinAtmConfigUpcEquipCbrBuffersTable
#define ptin_setgohw_PACUECBT  625 //estrutura ptinAtmConfigUpcEquipCbrBuffersTable
#define ptin_getnohw_PACUERBT  630 //estrutura ptinAtmConfigUpcEquipRvbrBuffersTable
#define ptin_getgohw_PACUERBT  631 //estrutura ptinAtmConfigUpcEquipRvbrBuffersTable
#define ptin_gntnohw_PACUERBT  632 //estrutura ptinAtmConfigUpcEquipRvbrBuffersTable
#define ptin_gntgohw_PACUERBT  633 //estrutura ptinAtmConfigUpcEquipRvbrBuffersTable
#define ptin_setnohw_PACUERBT  634 //estrutura ptinAtmConfigUpcEquipRvbrBuffersTable
#define ptin_setgohw_PACUERBT  635 //estrutura ptinAtmConfigUpcEquipRvbrBuffersTable
#define ptin_getnohw_PACUENBT  640 //estrutura ptinAtmConfigUpcEquipNvbrBuffersTable
#define ptin_getgohw_PACUENBT  641 //estrutura ptinAtmConfigUpcEquipNvbrBuffersTable
#define ptin_gntnohw_PACUENBT  642 //estrutura ptinAtmConfigUpcEquipNvbrBuffersTable
#define ptin_gntgohw_PACUENBT  643 //estrutura ptinAtmConfigUpcEquipNvbrBuffersTable
#define ptin_setnohw_PACUENBT  644 //estrutura ptinAtmConfigUpcEquipNvbrBuffersTable
#define ptin_setgohw_PACUENBT  645 //estrutura ptinAtmConfigUpcEquipNvbrBuffersTable
#define ptin_getnohw_PACUEUBT  650 //estrutura ptinAtmConfigUpcEquipUbrBuffersTable
#define ptin_getgohw_PACUEUBT  651 //estrutura ptinAtmConfigUpcEquipUbrBuffersTable
#define ptin_gntnohw_PACUEUBT  652 //estrutura ptinAtmConfigUpcEquipUbrBuffersTable
#define ptin_gntgohw_PACUEUBT  653 //estrutura ptinAtmConfigUpcEquipUbrBuffersTable
#define ptin_setnohw_PACUEUBT  654 //estrutura ptinAtmConfigUpcEquipUbrBuffersTable
#define ptin_setgohw_PACUEUBT  655 //estrutura ptinAtmConfigUpcEquipUbrBuffersTable

// reservados de 616 a 999 para uso futuro
//estruturas baseadas nas mibs da standard  
#define stand_getnohw_SP       1000 //estrutura systemptin --> system da mibII
#define stand_getgohw_SP       1001 //estrutura systemptin --> system da mibII
#define stand_gntnohw_SP       1002 //estrutura systemptin --> system da mibII
#define stand_gntgohw_SP       1003 //estrutura systemptin --> system da mibII
#define stand_setnohw_SP       1004 //estrutura systemptin --> system da mibII
#define stand_setgohw_SP       1005 //estrutura systemptin --> system da mibII
#define stand_getnohw_IE       1010 //estrutura ifentry --> mibII
#define stand_getgohw_IE       1011 //estrutura ifentry --> mibII
#define stand_gntnohw_IE       1012 //estrutura ifentry --> mibII
#define stand_gntgohw_IE       1013 //estrutura ifentry --> mibII
#define stand_setnohw_IE       1014 //estrutura ifentry --> mibII
#define stand_setgohw_IE       1015 //estrutura ifentry --> mibII
//alloc_atm_ifindex
#define stand_getnohw_atm_IE   1016 //estrutura ifentry --> mibII
#define stand_getgohw_oper_IE  1017 //estrutura ifentry --> mibII
#define stand_getnohw_IST      1020 //estrutura ifstacktable
#define stand_getgohw_IST      1021 //estrutura ifstacktable
#define stand_gntnohw_IST      1022 //estrutura ifstacktable
#define stand_gntgohw_IST      1023 //estrutura ifstacktable
#define stand_setnohw_IST      1024 //estrutura ifstacktable
#define stand_setgohw_IST      1025 //estrutura ifstacktable
//getifstackhigherlayer
#define stand_getnohwhigh_IST      1026 //estrutura ifstacktable
//getifstacklowerlayer
#define stand_getnohwlow_IST   1027 //estrutura ifstacktable
#define stand_getnohw_AFCCT    1030 //estrutura atmfcesconftable
#define stand_getgohw_AFCCT    1031 //estrutura atmfcesconftable
#define stand_gntnohw_AFCCT    1032 //estrutura atmfcesconftable
#define stand_gntgohw_AFCCT    1033 //estrutura atmfcesconftable
#define stand_setnohw_AFCCT    1034 //estrutura atmfcesconftable
#define stand_setgohw_AFCCT    1035 //estrutura atmfcesconftable
#define stand_getnohw_AFCST    1040 //estrutura atmfcesstatstable
#define stand_getgohw_AFCST    1041 //estrutura atmfcesstatstable
#define stand_gntnohw_AFCST    1042 //estrutura atmfcesstatstable
#define stand_gntgohw_AFCST    1043 //estrutura atmfcesstatstable
#define stand_setnohw_AFCST    1044 //estrutura atmfcesstatstable
#define stand_setgohw_AFCST    1045 //estrutura atmfcesstatstable
#define stand_getnohw_AVPT     1050 //estrutura atmvpltable
#define stand_getgohw_AVPT     1051 //estrutura atmvpltable
#define stand_gntnohw_AVPT     1052 //estrutura atmvpltable
#define stand_gntgohw_AVPT     1053 //estrutura atmvpltable
#define stand_setnohw_AVPT     1054 //estrutura atmvpltable
#define stand_setgohw_AVPT     1055 //estrutura atmvpltable
#define stand_getnohw_AVCT     1060 //estrutura atmvcltable
#define stand_getgohw_AVCT     1061 //estrutura atmvcltable
#define stand_gntnohw_AVCT     1062 //estrutura atmvcltable
#define stand_gntgohw_AVCT     1063 //estrutura atmvcltable
#define stand_setnohw_AVCT     1064 //estrutura atmvcltable
#define stand_setgohw_AVCT     1065 //estrutura atmvcltable
#define stand_getnohw_AICT     1070 //estrutura atminterfaceconftable
#define stand_getgohw_AICT     1071 //estrutura atminterfaceconftable
#define stand_gntnohw_AICT     1072 //estrutura atminterfaceconftable
#define stand_gntgohw_AICT     1073 //estrutura atminterfaceconftable
#define stand_setnohw_AICT     1074 //estrutura atminterfaceconftable
#define stand_setgohw_AICT     1075 //estrutura atminterfaceconftable
#define stand_getnohw_AVPCCT   1080 //estrutura atmvpcrossconnecttable
#define stand_getgohw_AVPCCT   1081 //estrutura atmvpcrossconnecttable
#define stand_gntnohw_AVPCCT   1082 //estrutura atmvpcrossconnecttable
#define stand_gntgohw_AVPCCT   1083 //estrutura atmvpcrossconnecttable
#define stand_setnohw_AVPCCT   1084 //estrutura atmvpcrossconnecttable
#define stand_setgohw_AVPCCT   1085 //estrutura atmvpcrossconnecttable
#define stand_getnohw_AVCCCT   1090 //estrutura atmvccrossconnecttable
#define stand_getgohw_AVCCCT   1091 //estrutura atmvccrossconnecttable
#define stand_gntnohw_AVCCCT   1092 //estrutura atmvccrossconnecttable
#define stand_gntgohw_AVCCCT   1093 //estrutura atmvccrossconnecttable
#define stand_setnohw_AVCCCT   1094 //estrutura atmvccrossconnecttable
#define stand_setgohw_AVCCCT   1095 //estrutura atmvccrossconnecttable
//getnextvalidVCLCC
#define stand_gntnohw_valid_AVCCCT   1096 //estrutura atmvccrossconnecttable
//getindiceVCLCC
#define stand_getnohw_indice_AVCCCT   1097 //estrutura atmvccrossconnecttable

#define stand_getnohw_ATDPT    1100 //estrutura atmtrafficdescrparamtable
#define stand_getgohw_ATDPT    1101 //estrutura atmtrafficdescrparamtable
#define stand_gntnohw_ATDPT    1102 //estrutura atmtrafficdescrparamtable
#define stand_gntgohw_ATDPT    1103 //estrutura atmtrafficdescrparamtable
#define stand_setnohw_ATDPT    1104 //estrutura atmtrafficdescrparamtable
#define stand_setgohw_ATDPT    1105 //estrutura atmtrafficdescrparamtable
//FreeIndiceTrafego
#define stand_setnohw_indice_ATDPT    1106 //estrutura atmtrafficdescrparamtable
//GetIndiceTrafego
#define stand_getnohw_indice_ATDPT    1107 //estrutura atmtrafficdescrparamtable

#define stand_getnohw_IGT      1110 //estrutura imagrouptable
#define stand_getgohw_IGT      1111 //estrutura imagrouptable
#define stand_gntnohw_IGT      1112 //estrutura imagrouptable
#define stand_gntgohw_IGT      1113 //estrutura imagrouptable
#define stand_setnohw_IGT      1114 //estrutura imagrouptable
#define stand_setgohw_IGT      1115 //estrutura imagrouptable
#define stand_getnohwfree_IGT  1116 //estrutura imagrouptable
#define stand_getnohw_ILT      1120 //estrutura imalinktable
#define stand_getgohw_ILT      1121 //estrutura imalinktable
#define stand_gntnohw_ILT      1122 //estrutura imalinktable
#define stand_gntgohw_ILT      1123 //estrutura imalinktable
#define stand_setnohw_ILT      1124 //estrutura imalinktable
#define stand_setgohw_ILT      1125 //estrutura imalinktable
#define stand_getnohw_D1CT     1130 //estrutura dsx1configtable
#define stand_getgohw_D1CT     1131 //estrutura dsx1configtable
#define stand_gntnohw_D1CT     1132 //estrutura dsx1configtable
#define stand_gntgohw_D1CT     1133 //estrutura dsx1configtable
#define stand_setnohw_D1CT     1134 //estrutura dsx1configtable
#define stand_setgohw_D1CT     1135 //estrutura dsx1configtable
#define stand_getnohw_D1CuT    1140 //estrutura dsx1currenttable
#define stand_getgohw_D1CuT    1141 //estrutura dsx1currenttable
#define stand_gntnohw_D1CuT    1142 //estrutura dsx1currenttable
#define stand_gntgohw_D1CuT    1143 //estrutura dsx1currenttable
#define stand_setnohw_D1CuT    1144 //estrutura dsx1currenttable
#define stand_setgohw_D1CuT    1145 //estrutura dsx1currenttable
#define stand_getnohw_D1FECuT  1150 //estrutura dsx1farendcurrenttable
#define stand_getgohw_D1FECuT  1151 //estrutura dsx1farendcurrenttable
#define stand_gntnohw_D1FECuT  1152 //estrutura dsx1farendcurrenttable
#define stand_gntgohw_D1FECuT  1153 //estrutura dsx1farendcurrenttable
#define stand_setnohw_D1FECuT  1154 //estrutura dsx1farendcurrenttable
#define stand_setgohw_D1FECuT  1155 //estrutura dsx1farendcurrenttable
#define stand_getnohw_ALT      1160 //estrutura adsllinetable
#define stand_getgohw_ALT      1161 //estrutura adsllinetable
#define stand_gntnohw_ALT      1162 //estrutura adsllinetable
#define stand_gntgohw_ALT      1163 //estrutura adsllinetable
#define stand_setnohw_ALT      1164 //estrutura adsllinetable
#define stand_setgohw_ALT      1165 //estrutura adsllinetable
#define stand_getnohw_ALCPT    1170 //estrutura adsllineconfprofiletable
#define stand_getgohw_ALCPT    1171 //estrutura adsllineconfprofiletable
#define stand_gntnohw_ALCPT    1172 //estrutura adsllineconfprofiletable
#define stand_gntgohw_ALCPT    1173 //estrutura adsllineconfprofiletable
#define stand_setnohw_ALCPT    1174 //estrutura adsllineconfprofiletable
#define stand_setgohw_ALCPT    1175 //estrutura adsllineconfprofiletable
#define stand_getnohw_ALACPT   1180 //estrutura adsllinealarmconfprofiletable
#define stand_getgohw_ALACPT   1181 //estrutura adsllinealarmconfprofiletable
#define stand_gntnohw_ALACPT   1182 //estrutura adsllinealarmconfprofiletable
#define stand_gntgohw_ALACPT   1183 //estrutura adsllinealarmconfprofiletable
#define stand_setnohw_ALACPT   1184 //estrutura adsllinealarmconfprofiletable
#define stand_setgohw_ALACPT   1185 //estrutura adsllinealarmconfprofiletable
#define stand_getnohw_ALET     1190 //estrutura adsllineexttable 
#define stand_getgohw_ALET     1191 //estrutura adsllineexttable 
#define stand_gntnohw_ALET     1192 //estrutura adsllineexttable 
#define stand_gntgohw_ALET     1193 //estrutura adsllineexttable 
#define stand_setnohw_ALET     1194 //estrutura adsllineexttable 
#define stand_setgohw_ALET     1195 //estrutura adsllineexttable 
#define stand_getnohw_ACPET    1200 //estrutura adslconfprofileexttable
#define stand_getgohw_ACPET    1201 //estrutura adslconfprofileexttable
#define stand_gntnohw_ACPET    1202 //estrutura adslconfprofileexttable
#define stand_gntgohw_ACPET    1203 //estrutura adslconfprofileexttable
#define stand_setnohw_ACPET    1204 //estrutura adslconfprofileexttable
#define stand_setgohw_ACPET    1205 //estrutura adslconfprofileexttable
#define stand_getnohw_AACPET   1210 //estrutura adslalarmconfprofileexttable
#define stand_getgohw_AACPET   1211 //estrutura adslalarmconfprofileexttable
#define stand_gntnohw_AACPET   1212 //estrutura adslalarmconfprofileexttable
#define stand_gntgohw_AACPET   1213 //estrutura adslalarmconfprofileexttable
#define stand_setnohw_AACPET   1214 //estrutura adslalarmconfprofileexttable
#define stand_setgohw_AACPET   1215 //estrutura adslalarmconfprofileexttable
#define stand_getnohw_AAcPT    1220 //estrutura adslatucphystable
#define stand_getgohw_AAcPT    1221 //estrutura adslatucphystable
#define stand_gntnohw_AAcPT    1222 //estrutura adslatucphystable
#define stand_gntgohw_AAcPT    1223 //estrutura adslatucphystable
#define stand_setnohw_AAcPT    1224 //estrutura adslatucphystable
#define stand_setgohw_AAcPT    1225 //estrutura adslatucphystable
#define stand_getgohw_AAcPTGS  1226 //estrutura adslatucphystable
#define stand_getnohw_AArPT    1230 //estrutura adslaturphystable
#define stand_getgohw_AArPT    1231 //estrutura adslaturphystable
#define stand_gntnohw_AArPT    1232 //estrutura adslaturphystable
#define stand_gntgohw_AArPT    1233 //estrutura adslaturphystable
#define stand_setnohw_AArPT    1234 //estrutura adslaturphystable
#define stand_setgohw_AArPT    1235 //estrutura adslaturphystable
#define stand_getnohw_AAcCT    1240 //estrutura adslatucchantable
#define stand_getgohw_AAcCT    1241 //estrutura adslatucchantable
#define stand_gntnohw_AAcCT    1242 //estrutura adslatucchantable
#define stand_gntgohw_AAcCT    1243 //estrutura adslatucchantable
#define stand_setnohw_AAcCT    1244 //estrutura adslatucchantable
#define stand_setgohw_AAcCT    1245 //estrutura adslatucchantable
#define stand_getnohw_AArCT    1250 //estrutura adslaturchantable
#define stand_getgohw_AArCT    1251 //estrutura adslaturchantable
#define stand_gntnohw_AArCT    1252 //estrutura adslaturchantable
#define stand_gntgohw_AArCT    1253 //estrutura adslaturchantable
#define stand_setnohw_AArCT    1254 //estrutura adslaturchantable
#define stand_setgohw_AArCT    1255 //estrutura adslaturchantable
#define stand_getnohw_AVpCCIT  1260 //estrutura atmvpcrossconnectindextable
#define stand_getgohw_AVpCCIT  1261 //estrutura atmvpcrossconnectindextable
#define stand_gntnohw_AVpCCIT  1262 //estrutura atmvpcrossconnectindextable
#define stand_gntgohw_AVpCCIT  1263 //estrutura atmvpcrossconnectindextable
#define stand_setnohw_AVpCCIT  1264 //estrutura atmvpcrossconnectindextable
#define stand_setgohw_AVpCCIT  1265 //estrutura atmvpcrossconnectindextable
#define stand_getnohw_AVcCCIT  1270 //estrutura atmvccrossconnectindextable
#define stand_getgohw_AVcCCIT  1271 //estrutura atmvccrossconnectindextable
#define stand_gntnohw_AVcCCIT  1272 //estrutura atmvccrossconnectindextable
#define stand_gntgohw_AVcCCIT  1273 //estrutura atmvccrossconnectindextable
#define stand_setnohw_AVcCCIT  1274 //estrutura atmvccrossconnectindextable
#define stand_setgohw_AVcCCIT  1275 //estrutura atmvccrossconnectindextable
#define stand_getnohw_AAcPGSM  1280 //estrutura adslatucphysgsmib
#define stand_getgohw_AAcPGSM  1281 //estrutura adslatucphysgsmib
#define stand_gntnohw_AAcPGSM  1282 //estrutura adslatucphysgsmib
#define stand_gntgohw_AAcPGSM  1283 //estrutura adslatucphysgsmib
#define stand_getnohw_AArPGSM  1290 //estrutura adslaturphysgsmib
#define stand_getgohw_AArPGSM  1291 //estrutura adslaturphysgsmib
#define stand_gntnohw_AArPGSM  1292 //estrutura adslaturphysgsmib
#define stand_gntgohw_AArPGSM  1293 //estrutura adslaturphysgsmib
#define stand_getnohw_AAcPDT   1300 //estrutura adslatucperfdatatable
#define stand_getgohw_AAcPDT   1301 //estrutura adslatucperfdatatable
#define stand_gntnohw_AAcPDT   1302 //estrutura adslatucperfdatatable
#define stand_gntgohw_AAcPDT   1303 //estrutura adslatucperfdatatable
#define stand_getnohw_AArPDT   1310 //estrutura adslaturperfdatatable
#define stand_getgohw_AArPDT   1311 //estrutura adslaturperfdatatable
#define stand_gntnohw_AArPDT   1312 //estrutura adslaturperfdatatable
#define stand_gntgohw_AArPDT   1313 //estrutura adslaturperfdatatable

#define stand_getnohw_AAcPDET  1320 //estrutura adslatucperfdataexttable
#define stand_getgohw_AAcPDET  1321 //estrutura adslatucperfdataexttable
#define stand_gntnohw_AAcPDET  1322 //estrutura adslatucperfdataexttable
#define stand_gntgohw_AAcPDET  1323 //estrutura adslatucperfdataexttable
#define stand_getnohw_AArPDET  1330 //estrutura adslaturperfdataexttable
#define stand_getgohw_AArPDET  1331 //estrutura adslaturperfdataexttable
#define stand_gntnohw_AArPDET  1332 //estrutura adslaturperfdataexttable
#define stand_gntgohw_AArPDET  1333 //estrutura adslaturperfdataexttable

#define stand_getnohw_ATDPIT   1340 //estrutura atmtrafficdescrparamindextable
#define stand_getgohw_ATDPIT   1341 //estrutura atmtrafficdescrparamindextable
#define stand_gntnohw_ATDPIT   1342 //estrutura atmtrafficdescrparamindextable
#define stand_gntgohw_ATDPIT   1343 //estrutura atmtrafficdescrparamindextable

#define stand_getnohw_ALCPGT   1350 //estrutura adsllineconfprofilegstable
#define stand_getgohw_ALCPGT   1351 //estrutura adsllineconfprofilegstable
#define stand_gntnohw_ALCPGT   1352 //estrutura adsllineconfprofilegstable
#define stand_gntgohw_ALCPGT   1353 //estrutura adsllineconfprofilegstable
#define stand_setnohw_ALCPGT   1354 //estrutura adsllineconfprofilegstable
#define stand_setgohw_ALCPGT   1355 //estrutura adsllineconfprofilegstable

// versao 3 controlo estruturas shdsl 

#define stand_getnohw_H2SSCT   1360 //estrutura hdsl2ShdslSpanConfTable
#define stand_getgohw_H2SSCT   1361 //estrutura hdsl2ShdslSpanConfTable
#define stand_gntnohw_H2SSCT   1362 //estrutura hdsl2ShdslSpanConfTable 
#define stand_gntgohw_H2SSCT   1363 //estrutura hdsl2ShdslSpanConfTable
#define stand_setnohw_H2SSCT   1364 //estrutura hdsl2ShdslSpanConfTable
#define stand_setgohw_H2SSCT   1365 //estrutura hdsl2ShdslSpanConfTable

#define stand_getnohw_H2SSST   1370 //estrutura hdsl2ShdslSpanStatusTable
#define stand_getgohw_H2SSST   1371 //estrutura hdsl2ShdslSpanStatusTable
#define stand_gntnohw_H2SSST   1372 //estrutura hdsl2ShdslSpanStatusTable
#define stand_gntgohw_H2SSST   1373 //estrutura hdsl2ShdslSpanStatusTable

#define stand_getnohw_H2SIT    1380 //estrutura hdsl2ShdslInventoryTable
#define stand_getgohw_H2SIT    1381 //estrutura hdsl2ShdslInventoryTable
#define stand_gntnohw_H2SIT    1382 //estrutura hdsl2ShdslInventoryTable
#define stand_gntgohw_H2SIT    1383 //estrutura hdsl2ShdslInventoryTable

#define stand_getnohw_H2SECoT  1390 //estrutura hdsl2ShdslEndpointConfTable
#define stand_getgohw_H2SECoT  1391 //estrutura hdsl2ShdslEndpointConfTable
#define stand_gntnohw_H2SECoT  1392 //estrutura hdsl2ShdslEndpointConfTable
#define stand_gntgohw_H2SECoT  1393 //estrutura hdsl2ShdslEndpointConfTable
#define stand_setnohw_H2SECoT  1394 //estrutura hdsl2ShdslEndpointConfTable
#define stand_setgohw_H2SECoT  1395 //estrutura hdsl2ShdslEndpointConfTable

#define stand_getnohw_H2SECuT  1400 //estrutura hdsl2ShdslEndpointCurrTable
#define stand_getgohw_H2SECuT  1401 //estrutura hdsl2ShdslEndpointCurrTable
#define stand_gntnohw_H2SECuT  1402 //estrutura hdsl2ShdslEndpointCurrTable
#define stand_gntgohw_H2SECuT  1403 //estrutura hdsl2ShdslEndpointCurrTable
                          
#define stand_getnohw_H2S15MIT 1410 //estrutura hdsl2Shdsl15MinIntervalTable
#define stand_getgohw_H2S15MIT 1411 //estrutura hdsl2Shdsl15MinIntervalTable
#define stand_gntnohw_H2S15MIT 1412 //estrutura hdsl2Shdsl15MinIntervalTable
#define stand_gntgohw_H2S15MIT 1413 //estrutura hdsl2Shdsl15MinIntervalTable

#define stand_getnohw_H2S1DIT  1420 //estrutura hdsl2Shdsl1DayIntervalTable
#define stand_getgohw_H2S1DIT  1421 //estrutura hdsl2Shdsl1DayIntervalTable
#define stand_gntnohw_H2S1DIT  1422 //estrutura hdsl2Shdsl1DayIntervalTable
#define stand_gntgohw_H2S1DIT  1423 //estrutura hdsl2Shdsl1DayIntervalTable

#define stand_getnohw_H2SEMT   1430 //estrutura hdsl2ShdslEndpointMaintTable
#define stand_getgohw_H2SEMT   1431 //estrutura hdsl2ShdslEndpointMaintTable
#define stand_gntnohw_H2SEMT   1432 //estrutura hdsl2ShdslEndpointMaintTable 
#define stand_gntgohw_H2SEMT   1433 //estrutura hdsl2ShdslEndpointMaintTable
#define stand_setnohw_H2SEMT   1434 //estrutura hdsl2ShdslEndpointMaintTable
#define stand_setgohw_H2SEMT   1435 //estrutura hdsl2ShdslEndpointMaintTable

#define stand_getnohw_H2SUMT   1440 //estrutura hdsl2ShdslUnitMaintTable
#define stand_getgohw_H2SUMT   1441 //estrutura hdsl2ShdslUnitMaintTable
#define stand_gntnohw_H2SUMT   1442 //estrutura hdsl2ShdslUnitMaintTable
#define stand_gntgohw_H2SUMT   1443 //estrutura hdsl2ShdslUnitMaintTable
#define stand_setnohw_H2SUMT   1444 //estrutura hdsl2ShdslUnitMaintTable
#define stand_setgohw_H2SUMT   1445 //estrutura hdsl2ShdslUnitMaintTable

#define stand_getnohw_H2SSCPT  1450 //estrutura hdsl2ShdslSpanConfProfileTable
#define stand_getgohw_H2SSCPT  1451 //estrutura hdsl2ShdslSpanConfProfileTable
#define stand_gntnohw_H2SSCPT  1452 //estrutura hdsl2ShdslSpanConfProfileTable
#define stand_gntgohw_H2SSCPT  1453 //estrutura hdsl2ShdslSpanConfProfileTable
#define stand_setnohw_H2SSCPT  1454 //estrutura hdsl2ShdslSpanConfProfileTable
#define stand_setgohw_H2SSCPT  1455 //estrutura hdsl2ShdslSpanConfProfileTable

#define stand_getnohw_H2SEACPT 1460 //estrutura hdsl2ShdslEndpointAlarmConfProfileTable
#define stand_getgohw_H2SEACPT 1461 //estrutura hdsl2ShdslEndpointAlarmConfProfileTable
#define stand_gntnohw_H2SEACPT 1462 //estrutura hdsl2ShdslEndpointAlarmConfProfileTable
#define stand_gntgohw_H2SEACPT 1463 //estrutura hdsl2ShdslEndpointAlarmConfProfileTable
#define stand_setnohw_H2SEACPT 1464 //estrutura hdsl2ShdslEndpointAlarmConfProfileTable
#define stand_setgohw_H2SEACPT 1465 //estrutura hdsl2ShdslEndpointAlarmConfProfileTable

#define stand_getnohw_H2SGsLSOST 1470 //estrutura hdsl2ShdslGsLineStatusOpStateTable
#define stand_getgohw_H2SGsLSOST 1471 //estrutura hdsl2ShdslGsLineStatusOpStateTable
#define stand_gntnohw_H2SGsLSOST 1472 //estrutura hdsl2ShdslGsLineStatusOpStateTable
#define stand_gntgohw_H2SGsLSOST 1473 //estrutura hdsl2ShdslGsLineStatusOpStateTable

#define stand_getnohw_H2SGsLST 1480 //estrutura hdsl2ShdslGsLineStatusTable
#define stand_getgohw_H2SGsLST 1481 //estrutura hdsl2ShdslGsLineStatusTable
#define stand_gntnohw_H2SGsLST 1482 //estrutura hdsl2ShdslGsLineStatusTable
#define stand_gntgohw_H2SGsLST 1483 //estrutura hdsl2ShdslGsLineStatusTable
                            
// reservados do 1496 ao 1799 para uso futuro
// reservados de 2086 a 2399 para uso futuro
//controlo dos perisfericos
#define ctr_getnohw_TTYDev     2400 //estrutura ttydevices
#define ctr_getgohw_TTYDev     2401 //estrutura ttydevices
#define ctr_gntnohw_TTYDev     2402 //estrutura ttydevices
#define ctr_gntgohw_TTYDev     2403 //estrutura ttydevices
#define ctr_setnohw_TTYDev     2404 //estrutura ttydevices
#define ctr_setgohw_TTYDev     2405 //estrutura ttydevices

//#define ctr_        //estrutura
// reservados de 201 a 209 para uso futuro
//#define         //estrutura
//#define         //estrutura


// Lista Codigo de Erros
#define  Ok                0
#define  TimeOut           1       
#define  ValorErrado       2    
#define  ValorNaoExiste    3
#define  ErroNoHw          4
#define  ErroNaBD          5
#define  ErroMsgInfo       6
#define  ErroNotAllowed    7
#define  ErroConverteBD   10
#define  ErroOverflowBD   11
#define  ErroOpenMsgQ     12
#define  ErroEmptyBD      16
#define  ErroUserNotAllwed 20
#define  ErroImpossivel  255


/****************************/
#endif //CTRL_IPC_PROTOCOL_H
