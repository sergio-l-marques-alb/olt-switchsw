/* ======================================================================
Projecto:   OMG_SIRAC
Modulo:     ********
Copyright:  SIR@PT Inovacao
Descricao:  Codificacao dos erros dos sistemas de gestao.

Autor:      Ana Cláudia Dias (CD), Alvaro Corga (AC),
            Augusta Manuela (AM), Vitor Mirones (VM)

Historico:  VM 2003.10.01 - Criacao do modulo V1.0.0.0
            ...
            VM 2004.08.19 - Revisao e documentacao

======================================================================= */
#ifndef GLNETBAND_MANAGER_SIRERROR_H
#define GLNETBAND_MANAGER_SIRERROR_H

// ========================================
// Famílias de erros       (0xAA------)
// ========================================
   #define ERROR_FAMILY_PDH               0x01     // PDH (Ex: E1, E3,...)
   #define ERROR_FAMILY_SDH               0x02     // SDH (Ex: STM1,STM4,...)
   #define ERROR_FAMILY_ETH               0x05     // ETH
   #define ERROR_FAMILY_ETHERNET          0x05     // ETH
   #define ERROR_FAMILY_DATA              0x06     // Acesso a Bases de Dados
   #define ERROR_FAMILY_IPC               0x07     // Protocolo de comunicação
   #define ERROR_FAMILY_HARDWARE          0x08     // Hardware
   #define ERROR_FAMILY_APPLICATION       0x09     // Software de Aplicacao
   #define ERROR_FAMILY_SYSTEM            0x0A     // Sistema
   #define ERROR_FAMILY_SYNC              0x0B     // Sincronismo
   #define ERROR_FAMILY_SYNCHRONISM       0x0B     // Sincronismo
   #define ERROR_FAMILY_ATM               0x0C     // ATM
   #define ERROR_FAMILY_MPLS              0x22     // MPLS
   #define ERROR_FAMILY_CORE              0x0A     // Core (equipment, card, interface)
   #define ERROR_FAMILY_NETWORK           0x0D     // Network
   #define ERROR_FAMILY_DOWNLOAD          0x10     // Reservado ate' 0x1F
   #define ERROR_FAMILY_OTN               0x40     // Reservado ate' 0x1F


// ========================================
// Níveis de severidade    (0x--AA----)
// ========================================
   #define ERROR_SEVERITY_EMERGENCY       0x00     // system is unusable
   #define ERROR_SEVERITY_ALERT           0x01     // action must be taken immediately
   #define ERROR_SEVERITY_CRITICAL        0x02     // critical conditions
   #define ERROR_SEVERITY_ERROR           0x03     // error conditions
   #define ERROR_SEVERITY_WARNING         0x04     // warning conditions
   #define ERROR_SEVERITY_NOTICE          0x05     // normal but significant condition
   #define ERROR_SEVERITY_INFORMATIONAL   0x06     // informational messages
   #define ERROR_SEVERITY_DEBUG           0x07     // debug-level messages

// ========================================
// Códigos de erro         (0x----AAAA)
// ========================================

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

   // --------------------------------
   // Erros para a familia Fastpath (0x01)
   // --------------------------------
      // sub-familia interfaces (0x02)
         #define ERROR_CODE_FP_OK            0x0100   // OK
         #define ERROR_CODE_FP_EVC_UNKNOWN   0x0101   // Unknown EVC

   // --------------------------------
   //Erros para a familia PDH (0x01)
   // --------------------------------
      // sub-familia interfaces (0x02)
         #define ERROR_CODE_IFExLOOP            0x0201   // Loop de interface invalido
         #define ERROR_CODE_IFCRC               0x0202   // CRC de interface invalido
         #define ERROR_CODE_IFJITTER            0x0203   // Atenuacao de Jitter invalida
         #define ERROR_CODE_IFNOCASSUPPORTED    0x0216


   // --------------------------------
   //Erros para a familia SDH (0x02)
   // --------------------------------
      // sub-familia Cruzamentos (0x01)
         #define ERROR_CODE_LINKINDEX           0x0100   // Indice de cruzamento invalido
         #define ERROR_CODE_LINKCOMPLETE        0x0101   // Cruzamento incompleto
         #define ERROR_CODE_LINKOCUPIED         0x0102   // Cruzamento ocupado para o cruzamento indicado
         #define ERROR_CODE_LINKEMPTY           0x0103   // Cruzamento vazio para o cruzamento indicado
         #define ERROR_CODE_LINKADMIN           0x0106   // Estado administrativo do cruzamento invalido
         #define ERROR_CODE_LINKTYPE            0x0107   // Tipo de cruzamento invalido (Unidireccional, Bidireccional)
         #define ERROR_CODE_LINKVCxTYPE         0x0108   // Tipo de contentor invalido
         #define ERROR_CODE_LINKVC4INVALID      0x0109   // Tipo de contentor VC4 invalido
         #define ERROR_CODE_LINKVCxINVALID      0x010A   // Tipo de contentor VCx invalido
         #define ERROR_CODE_LINKPROTBOARD       0x010F   // O porto e de uma board de proteccao
         #define ERROR_CODE_LINKSLEX            0x010C   //
         #define ERROR_CODE_LINKSLTX            0x010D   //
         #define ERROR_CODE_LINKSLEX2MBPS       0x0110
         #define ERROR_CODE_LINKSLEX34MBPS      0x0111
         #define ERROR_CODE_LINKSLTX2MBPS       0x0112
         #define ERROR_CODE_LINKSLTX34MBPS      0x0113

      // sub-familia interfaces (0x02)
         #define ERROR_CODE_IFSTMLOOP           0x0202   // Loop numa interface do tipo SDH
         #define ERROR_CODE_IFLOOPTIMEOUT       0x0203   // Tempo de espera do loop
         #define ERROR_CODE_IFSTMTYPE           0x0204   // Tipo de interface STM
         #define ERROR_CODE_IFPHYSTYPE          0x0205   // Tipo de interface fisica
         #define ERROR_CODE_IFALS               0x0206   // Automatic laser shutdown
         #define ERROR_CODE_IFALSDELAY          0x0207   // Configuracao do tempo de ALS
         #define ERROR_CODE_IFLASER             0x0208   // Configurar o estado do laser
         #define ERROR_CODE_IFSIGDEG            0x0209   // Configurar SD
         #define ERROR_CODE_IFDEXC              0x020B   // Configurar params de SD
         #define ERROR_CODE_IFDDEG              0x020C   // Configurar params de SD
         #define ERROR_CODE_IFDEGM              0x020D   // Configurar params de SD
         #define ERROR_CODE_IFDEGTHR            0x020E   // Configurar params de SD

   // sub-familia VC4 (0x03)
         #define ERROR_CODE_VC4INDEX            0x0300   // Indice de VC4 invalido
         #define ERROR_CODE_VC4PAYLOADIDEX      0x0301   // Indice de VC4 invalido
         #define ERROR_CODE_VC4PAYLOADIDTX      0x0302   // Indice de VC4 invalido
         #define ERROR_CODE_VC4STRUCTMISMATCH   0x0303   // Indice de VC4 invalido

      // sub-familia CC  (0x04)
         #define ERROR_CODE_CCINDEX             0x0400   // Indice de grupo de proteccao SNCP invalido
         #define ERROR_CODE_CCCOMPLETE          0x0401   // Indices de grupo de proteccao SNCP ocupados
         #define ERROR_CODE_CCOCUPIED           0x0402   // Grupo de proteccao SNCP ocupado
         #define ERROR_CODE_CCEMPTY             0x0403   // Grupo de proteccao SNCP vazio
         #define ERROR_CODE_CCADMIN             0x0405   // Estado administrativo do grupo de proteccao SNCP invalido
         #define ERROR_CODE_CCSWITCHMODE        0x0406   // Parametro de configuracao Switch Mode invalido
         #define ERROR_CODE_CCMODO              0x0407   // Parametro de configuracao Modo invalido
         #define ERROR_CODE_CCREVERSE           0x0409   // Parametro de configuracao Reverse invalido
         #define ERROR_CODE_CCMODOAPS           0x040A   // Parametro de configuracao Modo APS invalido
         #define ERROR_CODE_CCHOLDOFF           0x0411   // Parametro de configuracao Hold Off Time invalido
         #define ERROR_CODE_CCWAIT2RESTORE      0x0412   // Parametro de configuracao Wait To Restore invalido
         #define ERROR_CODE_CCDIRECTION         0x0417   //
         #define ERROR_CODE_CCVCXTYPE           0x0418   //
         #define ERROR_CODE_CCTOPOLOGY          0x0419   //
         #define ERROR_CODE_CCINVALIDVC4        0x041A   //
         #define ERROR_CODE_CCINVALIDVCx        0x041B   //
         #define ERROR_CODE_CCSIGDEG            0x041C   // Configurar SD
         #define ERROR_CODE_CCDEXC              0x041D   // Configurar params de SD
         #define ERROR_CODE_CCDDEG              0x041E   // Configurar params de SD
         #define ERROR_CODE_CCDEGM              0x041F   // Configurar params de SD
         #define ERROR_CODE_CCDEGTHR            0x0420   // Configurar params de SD
         #define ERROR_CODE_CCSLEX              0x0421   //
         #define ERROR_CODE_CCSLTX              0x0422   //
         #define ERROR_CODE_CCPROTBOARD         0x0423   //
         #define ERROR_CODE_CCSTMTYPE           0x0424   // Os portos A e B tem que ser STM
         #define ERROR_CODE_CCSLEX2MBPS         0x0425   // A terminacao e 2Mbps nao suporta tipo de label
         #define ERROR_CODE_CCSLEX34MBPS        0x0426   // A terminacao e 34Mbps nao suporta tipo de label
         #define ERROR_CODE_CCSLTX2MBPS         0x0427   // A terminacao e 2Mbps nao suporta tipo de label
         #define ERROR_CODE_CCSLTX34MBPS        0x0428   // A terminacao e 34Mbps nao suporta tipo de label

      // sub-familia MSP (0x05)
         #define ERROR_CODE_MSPINDEX            0x0500   // Indice de proteccao MSP invalido (out of range)
         #define ERROR_CODE_MSPNOADMIN          0x0501   // Estado admin do grupo MSP invalido (status inadequado)
         #define ERROR_CODE_MSPNOINDEX          0x0502   // Nao foi definido nenhum dos indices do grupo
         #define ERROR_CODE_MSPNOTCREATED       0x0503   // O grupo nao foi criado (rowstatus==free)
         #define ERROR_CODE_MSPOCUPIED          0x0504   // Grupo de proteccao MSP ocupado (nao pode criar)
         #define ERROR_CODE_MSPNOBOARD          0x0505   // Porto invalido (slot vazia ou com carta nova)
         #define ERROR_CODE_MSPNOPORT           0x0506   // Porto invalido (out of range)
         #define ERROR_CODE_MSPPORTTYPE         0x0507   // O porto indicado nao e' do tipo STM
         #define ERROR_CODE_MSPPORTUSED         0x0508   // O porto indicado existe noutro grupo
         #define ERROR_CODE_MSPPROTBOARD        0x0509   // Porto invalido (a board e' de proteccao)
         #define ERROR_CODE_MSPADMIN            0x050A   // Campo out of range
         #define ERROR_CODE_MSPWAIT2RESTORE     0x050B   // Campo out of range
         #define ERROR_CODE_MSPMODO             0x050C   // Campo out of range
         #define ERROR_CODE_MSPREVERSE          0x050D   // Campo out of range
         #define ERROR_CODE_MSPDIRECTION        0x050E   // Campo out of range
         #define ERROR_CODE_MSPTRAFEXTRA        0x050F   // Campo out of range
         #define ERROR_CODE_MSPPRIORITYA        0x0510   // Campo out of range
         #define ERROR_CODE_MSPPRIORITYB        0x0511   // Campo out of range
         #define ERROR_CODE_MSPLOCKOUTA         0x0512   // Campo out of range
         #define ERROR_CODE_MSPLOCKOUTB         0x0513   // Campo out of range
         #define ERROR_CODE_MSPSWITCHCMDA       0x0514   // Comando invalido
         #define ERROR_CODE_MSPSWITCHCMDB       0x0515   // Comando invalido
         #define ERROR_CODE_MSPSAMEPORT         0x0516   // O mesmo porto foi repetido nos canais do grupo
         #define ERROR_CODE_MSPSTMTYPE          0x0517   // Os portos do grupo sao de diferentes tipos de STM
         #define ERROR_CODE_MSPNAMEREPEATED     0x0518   // Existe outro grupo com igual Name

      // sub-familia MSSPring (0x06)
         #define ERROR_CODE_HASSQUELCHENTRIES   0x0600   // O grupo possui entradas na squelch table
         #define ERROR_CODE_MSSPRINGADMIN       0x0601   // paramentros admin invalido
         #define ERROR_CODE_MSSPRINGNODEID      0x0602   // paramentros nodeId invalido
         #define ERROR_CODE_MSSPRINGSTMTYPE     0x0603   // paramentros stmType invalido
         #define ERROR_CODE_MSSPRINGFIBERS      0x0604   // paramentros fibers invalido
         #define ERROR_CODE_MSSPRINGTRAFEXTRA   0x0605   // paramentros trafExtra invalido
         #define ERROR_CODE_MSSPRINGSTMINDEX    0x0606   // paramentros stmIndex invalido
         #define ERROR_CODE_MSSPRINGRINGMAP     0x0607   // paramentros nodes invalido
         #define ERROR_CODE_MSSPRINGCOMMAND     0x0608   // paramentros command invalido
         #define ERROR_CODE_MSSPRINGNODEIDUSED  0x0609   // paramentros nodeid invalido (usado no ringmap)
         #define ERROR_CODE_MSSPRINGHASPORTS    0x060A   //
         #define ERROR_CODE_MSSPRINGSTMIXUSED   0x060B   //
         #define ERROR_CODE_MSSPRINGINSUFPORTS  0x060C   //

   // --------------------------------
   //Erros para a familia ETH (0x05)
   // --------------------------------
      // sub-familia interface fisica (0x01)
      #define ERROR_CODE_IFETHLOOP           0x0100   //
      #define ERROR_CODE_IFETHAUTONEG        0x0101   //
      #define ERROR_CODE_IFETHMEDIATYPE      0x0102   //
      #define ERROR_CODE_IFETHFLOWCTRL       0x0103   //
      #define ERROR_CODE_IFETHNOOPTICAL      0x0104   //
      #define ERROR_CODE_IFETHALS            0x0105   // Automatic laser shutdown
      #define ERROR_CODE_IFETHALSDELAY       0x0106   // Configuracao do tempo de ALS
      #define ERROR_CODE_IFETHLASER          0x0107   // Configurar o estado do laser

      // sub-familia VCG (0x02)
      #define ERROR_CODE_VCGINDEX            0x0200   //
      #define ERROR_CODE_VCGNOINDEX          0x0201
		#define ERROR_CODE_VCGEMPTY            0x0203   // Grupo VCG nao existe
      #define ERROR_CODE_VCGADMINUP          0x0204   // Grupo a up nao deixa mexer
      #define ERROR_CODE_VCGMAPMODEINVALID   0x0205   //
      #define ERROR_CODE_VCGHASASSOCIATIONS  0x0206   //
      #define ERROR_CODE_VCGADMIN            0x0207   // Parametros invalido
      #define ERROR_CODE_VCGVCXTYPE          0x0208   // Parametros invalido
      #define ERROR_CODE_VCGMAPTYPE          0x0209   // Parametros invalido
      #define ERROR_CODE_VCGFRAMETYPE        0x020A   // Parametros invalido
      #define ERROR_CODE_VCGGFPFMODE         0x020B   // Parametros invalido
      #define ERROR_CODE_VCGLCAS             0x020C   // Parametros invalido
      #define ERROR_CODE_VCGHASVCATS         0x020D   //
      #define ERROR_CODE_VCGNOASSOCIATIONS   0x020E   //
      #define ERROR_CODE_VCGMULTIPLEPORTS    0x020F   //
      #define ERROR_CODE_VCGINVALIDSEQUENCE  0x0210   //
      #define ERROR_CODE_VCGEXISTS           0x0211   //
      #define ERROR_CODE_VCGINVALIDCIDSEQ    0x0212   //
//      #define ERROR_CODE_         0x0212   //
      #define ERROR_CODE_VCGHOLDOFFTIME      0x0213   //
      #define ERROR_CODE_VCGIFBOARDMODE      0x0214   // Interface interdita por estar outra activa
      // #define ERROR_CODE_IFETHMAPTYPE        0x0224   //
      // #define ERROR_CODE_IFETHGFPTYPE        0x0225   //
      // #define ERROR_CODE_IFETHGFPFMODE       0x0226   //
      // #define ERROR_CODE_IFETHLCAS           0x0227   //

      // sub-familia VCAT (0x03)
      #define ERROR_CODE_VCATEMPTY           0x0300
      #define ERROR_CODE_VCATFULL            0x0301
      #define ERROR_CODE_VCATVCxINVALID      0x0302
      #define ERROR_CODE_VCATDUPLICATE       0x0303
      #define ERROR_CODE_VCATNOTFOUND        0x0304
      #define ERROR_CODE_VCATSEQUENCE        0x0305
      #define ERROR_CODE_VCATADMIN           0x0306
      #define ERROR_CODE_VCATTYPE            0x0307
      #define ERROR_CODE_VCATINDEX           0x0308
      #define ERROR_CODE_VCATTUGVCx          0x0309
      #define ERROR_CODE_VCATVC4MODE         0x030A
      #define ERROR_CODE_VCATADMININVALID    0x030B

      // sub-familia EthMap (0x04)
      #define ERROR_CODE_ETHMAPEMPTY         0x0400
      #define ERROR_CODE_ETHMAPFULL          0x0401
      #define ERROR_CODE_ETHMAPETHADDR       0x0402
      #define ERROR_CODE_ETHMAPDUPLICATE     0x0403
      #define ERROR_CODE_ETHMAPMANYMAPS      0x0404
      #define ERROR_CODE_ETHMAPVCxTYPE       0x0405
      #define ERROR_CODE_ETHMAPCID           0x0406
      #define ERROR_CODE_ETHMAPSCHEDULER     0x0407
      #define ERROR_CODE_ETHMAPADMIN         0x0408
      #define ERROR_CODE_ETHMAPMAPTYPE       0x0409
      #define ERROR_CODE_ETHMAPMAPPARAM      0x040A
      #define ERROR_CODE_ETHMAPMINETHMAPS    0x040B

      // sub-familia EvcXc (0x05)
      #define ERROR_CODE_EVCINVALIDTYPE      0x0501   // Invalid TYPE value
      #define ERROR_CODE_EVCINVALIDVLANIDPR  0x0502   // Invalid CE-VLAN ID PRESERVATION value
      #define ERROR_CODE_EVCINVALIDONCTP     0x0503   // EVC de um CTP repetido na mesma interface.
      #define ERROR_CODE_EVCINVALIDCTPREF    0x0504   // Este EVC tem pelo menos um CTP a referencia-lo
      #define ERROR_CODE_EVCINVALIDSVLANID   0x0505   // Invalid S-VLAN ID value
      #define ERROR_CODE_EVCINVALIDMAXUNIS   0x0506   // Invalid MAX UNIS value
      #define ERROR_CODE_EVCINVALIDMTU       0x0507   // Invalid MTU value
      #define ERROR_CODE_EVCNOTENOUGHCTPS    0x0508   // Not enough active CTP's in EVC
      #define ERROR_CODE_EVCVLANSNOTRIGHT    0x0509   // Not coerent number of VLAN's in all CTP's in EVC
      #define ERROR_CODE_EVCVLANNOTWILDCARD  0x050A   // VLAN's are not wildcardable
      #define ERROR_CODE_EVCSARECONFIGURED   0x050B   // Existem EVC's configurados

      // sub-familia EthTD (0x06)
      #define ERROR_CODE_ETDINVALIDADMIN     0x0600
      #define ERROR_CODE_ETDINVALIDTYPE      0x0601
      #define ERROR_CODE_ETDINVALIDCMODE     0x0602
      #define ERROR_CODE_ETDINVALIDCFLAG     0x0603
      #define ERROR_CODE_ETDINVALIDCIR       0x0604
      #define ERROR_CODE_ETDINVALIDCBS       0x0605
      #define ERROR_CODE_ETDINVALIDEIR       0x0606
      #define ERROR_CODE_ETDINVALIDEBS       0x0607

      // sub-familia EthMefExt (0x07)
      #define ERROR_CODE_ETHMEFEXT_VLANAWARE          0x0700
      #define ERROR_CODE_ETHMEFEXT_SERVICEMUX         0x0701
      #define ERROR_CODE_ETHMEFEXT_CEVLANID           0x0702
      #define ERROR_CODE_ETHMEFEXT_BUNDLING           0x0703
      #define ERROR_CODE_ETHMEFEXT_ALL2ONEBUNDLING    0x0704
      #define ERROR_CODE_ETHMEFEXT_INGRESSPROFILE     0x0705
      #define ERROR_CODE_ETHMEFEXT_EGRESSPROFILE      0x0706
      #define ERROR_CODE_ETHMEFEXT_NUMMAXECVS         0x0707
      #define ERROR_CODE_ETHMEFEXT_TYPE               0x0708
      #define ERROR_CODE_ETHMEFEXT_NAME               0x0709
      #define ERROR_CODE_ETHMEFEXT_HASACTIVESERVICES  0x0710


      // sub-familia Ctp   (0x08)
      #define ERROR_CODE_CTPINVALIDADMIN     0x0800
      #define ERROR_CODE_CTPHASVLAN          0x0801   // CTP tem VLANS a apontar para ele
      #define ERROR_CODE_CTPNUMMAX           0x0802
      #define ERROR_CODE_CTPHASiCOS          0x0803   // CTP tem iCoS a apontar para ele
      #define ERROR_CODE_CTPHASNULLPROFILE   0x0804   // CTP has null profile

      // sub-familia CFM   (0x0A)
      #define ERROR_CODE_CFMPROBEEXIST       0x0A00   // entidade ja existente

   // --------------------------------
   //Erros para a familia DATA (0x06)
   // --------------------------------
      #define ERROR_CODE_DBINVALIDNAME       0x0101   // Erro na definicao da base de dados a utilizar
      #define ERROR_CODE_OPENFAILED          0x0102   // Erro na abertura da base de dados indicada
      #define ERROR_CODE_NOCONNECTION        0x0103   // Erro na ligacao a Base de Dados
      #define ERROR_CODE_DBSELECTFAILED      0x0104   // Erro na leitura de uma tabela da base de dados
      #define ERROR_CODE_DBUPDATEFAILED      0x0105   // Erro na actualizacao de uma tabela na base de dados
      #define ERROR_CODE_TRANSACTION         0x0106   // Erro na criacao da transaccao
      #define ERROR_CODE_DBDELETEFAILED      0x0107   // A operacao DELETE nao foi em sucedida
      #define ERROR_CODE_DBINSERTFAILED      0x0108   // A operacao INSERT nao foi em sucedida
      #define ERROR_CODE_DBPRAGMAFAILED      0x0109   //
      #define ERROR_CODE_DBCREATEFAILED      0x010A   // Nao consegui criar uma tabela na BD
      #define ERROR_CODE_DBSCRIPTFAILED      0x010C   //
      #define ERROR_CODE_DBCOLUMNNUMBER      0x010D   // Wrong column number

   // --------------------------------
   //Erros para a familia IPC (0x07)
   // --------------------------------
      #define ERROR_CODE_NOFREECHAN          0x0101   // Nao existem canais de comunicacao disponivais
      #define ERROR_CODE_CREATESOCKET        0x0102   // Erro na criacao do socket cliente
      #define ERROR_CODE_BINDSOCKET          0x0103   // Erro ao ligar o socket servidor ao porto de escuta
      #define ERROR_CODE_CREATECLONE         0x0104   // Erro na criacao do thread de atendimento de mensagens
      #define ERROR_CODE_NOTOPENED           0x0105   // Canal de comunicacao indicado nao esta atribuido
      #define ERROR_CODE_SENDFAILED          0x0106   // Erro no envio de mensagem
      #define ERROR_CODE_TIMEOUT             0x0107   // Atingiu tempo limite de espera na recepcao da resposta no canal
      #define ERROR_CODE_INVALIDCH           0x0108   // Identificador de canal invalido
      #define ERROR_CODE_NOSUCHMSG           0x0109   // Mensagem recebida nao e suportada pelo modulo de controlo
      #define ERROR_CODE_EMPTYMSG            0x010A   // Recebeu mensagem NULA das aplicacoes cliente
      #define ERROR_CODE_NOTREADY            0x010B   // Erro no processo de mensagem por ter o atendimento desabilitado
      #define ERROR_CODE_WRONGSIZE           0x010C   // Dimensao do campo de dados invalida

   // -----------------------------------
   //Erros para a familia HARDWARE (0x08)
   // -----------------------------------

   // --------------------------------------
   //Erros para a familia APPLICATION (0x09)
   // --------------------------------------
         #define ERROR_CODE_CXDLOPENFILE        0x0101   // Nao abriu ficheiro com downloads
         #define ERROR_CODE_WDAGENT_SCRIPT      0x0102   // Script de WDAgent nao executou
         #define ERROR_CODE_WDAGENT_RETURN      0x0103   // Script de WDAgent nao devolve nada
         #define ERROR_CODE_WDAGENT_ALARM       0x0104   // Script de WDAgent detectou problemas
         #define ERROR_CODE_FILECOPY            0x0105
         #define ERROR_CODE_SPAWN               0x0106

   // --------------------------------
   //Erros para a familia SYSTEM (0x0A)
   // --------------------------------
#define __TAG_SIRERRORS_H_SYSTEM
      // Sub-familia system (0x01)
         #define ERROR_CODE_INVALIDDATE         0x0100   // Erro no acerto de data
         #define ERROR_CODE_INVALIDTIME         0x0101   // Erro no acerto da hora
         #define ERROR_CODE_INVALIDIP           0x0102   // Nao foi possivel configurar o IP
         #define ERROR_CODE_SYSADMIN            0x0103   // Status do sistema invalido
         #define ERROR_CODE_INVALIDSYSCTRL      0x0104   // Modulo de controlo invalido
         #define ERROR_CODE_SNMPDCONF           0x0105   // Nao configurou o ficheiro snmpd.conf
         #define ERROR_CODE_HOSTS               0x0106   // Nao configurou o ficheiro do hostname
         #define ERROR_CODE_ZEBRACONF           0x0107   // Nao configurou o ficheiro do zebra.conf
         #define ERROR_CODE_RIPDCONF            0x0108   // Nao configurou o ficheiro do ripd.conf
         #define ERROR_CODE_INITDAT             0x0109   // Nao configurou o ficheiro do init.dat
         #define ERROR_CODE_STARTNET            0x010A   // Nao configurou o ficheiro do startNet.sh
         #define ERROR_CODE_LAPDCONF            0x010B   // Nao configurou o ficheiro do lapd.conf
         #define ERROR_CODE_OPCEXECONF          0x010C   // Nao configurou o ficheiro do opcexe.conf

      // Sub-familia boards (0x02)
         #define ERROR_CODE_INVALIDSTATE        0x0200   // Estado operacional invalido
         #define ERROR_CODE_BOARDINDEX          0x0201   // Indice de board invalido
         #define ERROR_CODE_BOARDADMIN          0x0202   // Estado administrativo invalido
         #define ERROR_CODE_BOARDTYPE           0x0203   // Tipo de board invalida
         #define ERROR_CODE_BOARDOPERMODE       0x0204   // Nao pode configurar parametro
         #define ERROR_CODE_BOARDPROTADMIN      0x0205   // Campo protection admin invalido
         #define ERROR_CODE_CHANGINGADMIN       0x0206   // Admin a ser configurado
         #define ERROR_CODE_PROTECTIONID        0x0207   //
         #define ERROR_CODE_PROTECSTATUS        0x0208   //
         #define ERROR_CODE_PROTECTYPE          0x0209   // A board e' de proteccao
         #define ERROR_CODE_PROTECBOARD         0x020A   // Board de proteccao (nao deixa R/W)
         #define ERROR_CODE_BOARDISSRCSYNC      0x020B   //
         #define ERROR_CODE_BOARDHASCIRCUITS    0x020C   //
         #define ERROR_CODE_BOARDNOTWRITABLE    0x020D   //
         #define ERROR_CODE_BOARDCONTACTFAILED  0x020E   //
         #define ERROR_CODE_BOARDCANNOTUPGRADE  0x020F   //
         #define ERROR_CODE_BOARDPROTCOMMAND    0x0210   // comando invalido
         #define ERROR_CODE_PROTSWNOTELEGIBLE   0x0211   // A carta protectora deixou de ser valida devido a erros em configuracoes difundidas
         #define ERROR_CODE_BOARDBUSY           0x0212   // A carta encontra-se em processamento
         #define ERROR_CODE_BOARDTESTINGSTATE   0x0213   // A carta encontra-se em teste de estado por outro processo

         #define ERROR_CODE_BOARDCONV_SRCSYNC   0x0230   // Conversao de carta nao permitida, o porto a ser destruido e fonte de sincronismo
         #define ERROR_CODE_BOARDCONV_HASMSP    0x0231   // Conversao de carta nao permitida, o porto a ser destruido e membro de proteccoes MSP

      // Sub-familia interfaces (0x03)
         #define ERROR_CODE_IFINDEX             0x0300   // Indice de interface inválido
         #define ERROR_CODE_IFADMIN             0x0301   // Estado admnistrativo da interface invalido
         #define ERROR_CODE_NETADDRESS          0x0302   // endereco de net em vez de host.
         #define ERROR_CODE_BROADCAST           0x0303   // endereco de broacast.
         #define ERROR_CODE_MULTICAST           0x0304   // limite de endereco 224.0.0.0
         #define ERROR_CODE_INVALIDIPMASK       0x0305   // mascara entre 0 e 32
         #define ERROR_CODE_DUPLICATEIP         0x0306   // o ip e' repetido
         #define ERROR_CODE_DUPLICATENETIP      0x0307   // o ip e' duma mesma rede
         #define ERROR_CODE_ROUTEINDEX          0x0308   // indice da rota
         #define ERROR_CODE_ROUTEINDEXOCUPIED   0x0309   // Indice ocupado
         #define ERROR_CODE_IFBOARDMODE         0x030A   // Interface interdita por estar outra activa
         #define ERROR_CODE_IFTYPE              0x030B   //
         #define ERROR_CODE_RECENTRESET         0x030C   // Foi feito um arranque recentemente, deve esperar tempo minimo para proximo reset

      // Sub-familia manutencao  (0x04)
         #define ERROR_CODE_NOSUCHMODULE        0x0400   // o indice do modulo nao existe
         #define ERROR_CODE_NOSUCHOPERATION     0x0401   // o tipo de operacao nao e' conhecido
         #define ERROR_CODE_INPROGRESS          0x0402   // o modulo esta ocupado
         #define ERROR_CODE_OPMODEINVALID       0x0403   //
         #define ERROR_CODE_INMAINTENANCE       0x0404   //
         #define ERROR_CODE_OPNOTSUPPORTED      0x0405   //
         #define ERROR_CODE_OPINVALIDMODULE     0x0406   //
         #define ERROR_CODE_NOTINMAINTENANCE    0x0408   //
         #define ERROR_CODE_NOTINDOWNLOAD       0x0409   //
         #define ERROR_CODE_MSPDOWNLOADFAILED   0x040A   //

      // sub-familia traps (0x05)
         #define ERROR_CODE_TRAPSOVERFLOW       0x0500   // atingiu o numero maximo de traps
         #define ERROR_CODE_TRAPDONTEXIST       0x0501   //
         #define ERROR_CODE_TRAPSKIP            0x0502   //

      // sub-familia BoardMode (0x06)
         #define ERROR_CODE_BOARDMODE_INVALID         0x0600
         #define ERROR_CODE_BOARDMODE_LOCKED          0x0601

   // sub-familia EthBoardMode (0x07)
         #define ERROR_CODE_ETHBOARDOFFSET_INVALID    0x0701
         #define ERROR_CODE_ETHBOARDOFFSET_VLANOFF    0x0702

   // --------------------------------
   //Erros para a familia SYNC (0x0B)
   // --------------------------------
      // Sub-familia SyncGlobal (0x01)
         #define ERROR_CODE_SYNCQLEVEL          0x0100   // Parametro de configuracao syncQualityLevel invalido
         #define ERROR_CODE_SYNCREVERS          0x0101   // Parametro de configuracao Reversibility invalido
         #define ERROR_CODE_SYNCREVTIME         0x0102   // Parametro de configuracao ReversibilityTime invalido
         #define ERROR_CODE_SYNCHMSGDELAY       0x0103   // Parametro de configuracao HoldMsgDelay invalido
         #define ERROR_CODE_SYNCSWMSGDELAY      0x0104   // Parametro de configuracao SwitchMsgDelay invalido
         #define ERROR_CODE_SYNCMINB4SQ         0x0105   // Parametro de configuracao MinQualB4Squelch invalido
         #define ERROR_CODE_SYNCAFTERQU         0x0106   // Parametro de configuracao SendValueAfterQU invalido
         #define ERROR_CODE_SYNCNJUNC           0x0107   // Parametro de configuracao do numero de juncoes de sincronismo invalido

      // Sub-familia SrcSync (0x02)
         #define ERROR_CODE_SRCSYNCINDEX        0x0200   // Indice de fonte de sincronismo invalido
         #define ERROR_CODE_SRCSYNCIFINDEX      0x0201   // Parametro de configuracao do indice do interface invalido
         #define ERROR_CODE_SRCSYNCPRIORITY     0x0202   // Parametro de configuracao da prioridade da fonte de sincronismo invalido
         #define ERROR_CODE_SRCSYNCQLSSM        0x0203   // Parametro de configuracao QLSSM invalido
         #define ERROR_CODE_SRCSYNCSARX         0x0204   // Parametro de configuracao SABitTx invalido
         #define ERROR_CODE_SRCSYNCSATX         0x0205   // Parametro de configuracao SABitRx invalido
         #define ERROR_CODE_SRCSYNCHOLDOFF      0x0206   // Parametro de configuracao HoldoffTime invalido
         #define ERROR_CODE_SRCSYNCWAIT2REST    0x0207   // Parametro de configuracao Wait2RestoreTime invalido
         #define ERROR_CODE_SRCSYNCINVALIDSRC   0x0208   // Fonte de sincronismo nao disponivel

   // --------------------------------
   //Erros para a familia ATM (0x0C)
   // --------------------------------

      // Sub-familia IMA (0x01)
         #define ERROR_CODE_IMANAMEEXISTS       0x0100   // O nome indicado ja foi atribuido
         #define ERROR_CODE_IMAIDINVALID        0x0101   // Fora dos limites
         #define ERROR_CODE_IMAMINNUMTXLINKS    0x0102   // Fora dos limites
         #define ERROR_CODE_IMAMINNUMRXLINKS    0x0103   // Fora dos limites
         #define ERROR_CODE_IMASYMMETRY         0x0104   // Fora dos limites
         #define ERROR_CODE_IMATXCLKMODE        0x0105   // Fora dos limites
         #define ERROR_CODE_IMATXFRAMELEN       0x0106   // Fora dos limites
         #define ERROR_CODE_IMAIMAVER           0x0107   // Fora dos limites
         #define ERROR_CODE_IMAAUTOENABLE       0x0108   // Fora dos limites
         #define ERROR_CODE_IMATIMINGREFLID     0x0109   // Fora dos limites
         #define ERROR_CODE_IMADIFFDELAY        0x010A   // Fora dos limites
         #define ERROR_CODE_IMAHASLINKS         0x010B
         #define ERROR_CODE_IMAHASLINKSDOWN     0x010C
         #define ERROR_CODE_IMADIFFMINNUMLINKS  0x010D

      // Sub-familia IMALINK (0x02)
         #define ERROR_CODE_E1NOTATTACHED       0x0200   //
         #define ERROR_CODE_IMALINKMINIMUM      0x0201   //
         #define ERROR_CODE_IMALINKSLOTMISMATCH 0x0202   //
         #define ERROR_CODE_IMALINKFULLTABLE    0x0203   //
         #define ERROR_CODE_IMALINKSETMISMATCH  0x0204   //
         #define ERROR_CODE_IMALINKNOIMAGROUP   0x0205   //
         #define ERROR_CODE_IMALINKHIIFUP       0x0206   //

      // Sub-familia ATMINTERFACE (0x03)
         #define ERROR_CODE_VPIBITSINVALID      0x0301
         #define ERROR_CODE_VCIBITSINVALID      0x0302
         #define ERROR_CODE_ATMAUTOENABLE       0x0303
         #define ERROR_CODE_ATMLOIFINVALID      0x0304
         #define ERROR_CODE_ATMLOIFTYPEINVALID  0x0305
         #define ERROR_CODE_ATMLOIFNOTCREATED   0x0306
         #define ERROR_CODE_ATMLOIFUSED         0x0307
         #define ERROR_CODE_ATMLOIFDOWN         0x0308
         #define ERROR_CODE_ATMIFINVCLUSE       0x0309
         #define ERROR_CODE_ATMIFLOREADONLY     0x030A
         #define ERROR_CODE_ATMHIIFUP           0x030B
         #define ERROR_CODE_ATMOVSBSCBRPCR      0x030C
         #define ERROR_CODE_ATMOVSBSRVBRSCR     0x030D
         #define ERROR_CODE_ATMOVSBSRVBRPCRd    0x030E
         #define ERROR_CODE_ATMOVSBSNVBRSCR     0x030F
         #define ERROR_CODE_ATMOVSBSUBRMDCR     0x0310
         #define ERROR_CODE_ATMOVSBSUBRPCRd     0x0311

      // Sub-familia TRAFFICDESCRIPTOR (0x04)
         #define ERROR_CODE_TRAFINDEX           0x0400   // Indice de descritor de trafego invalido
         #define ERROR_CODE_TRAFCOMPLETE        0x0401   // Descritores de trafego completos
         #define ERROR_CODE_TRAFOCUPIED         0x0402   // Descritor de trafego ocupado para o descritor de trafego indicado
         #define ERROR_CODE_TRAFEMPTY           0x0403   // Descritor de trafego vazio para o descritor de trafego indicado
         #define ERROR_CODE_TRAFADMIN           0x0406   // Estado administrativo do descritor de trafego invalido
         #define ERROR_CODE_TRAFTYPE            0x0407   // Tipo de Descritor de trafego invalido ( 1..15  |  9- CBR.1, 10-VBR.1, 14-VBR.2, 15-VBR.3, 12-UBR.1, 11-UBR.2 )
         #define ERROR_CODE_TRAFCATEG           0x0408   // Tipo de categoria invalido ( 1-other, 2-cbr, 3-rtVbr, 4-nrtVbr, 5-abr, 6-ubr )
         #define ERROR_CODE_TRAFFRAMEDISC       0x0409   // Configuracao invalida para este parametro ( 1-true, 2-false )
         #define ERROR_CODE_TRAFPARAM1          0x040A   // Configuracao invalida para este parametro (CBR-pcr,  rVBR-pcr,   nVBR-pcr,   UBR+-pcr,  UBR-pcr  )
         #define ERROR_CODE_TRAFPARAM2          0x040B   // Configuracao invalida para este parametro (CBR-cdvt, rVBR-scr,   nVBR-scr,   UBR+-cdvt, UBR-cdvt )
         #define ERROR_CODE_TRAFPARAM3          0x040C   // Configuracao invalida para este parametro (CBR-NA,   rVBR-mbs,   nVBR-mbs,   UBR+-MDCR, UBR-0    )
         #define ERROR_CODE_TRAFPARAM4          0x040D   // Configuracao invalida para este parametro (CBR-NA,   rVBR-cdvt,  nVBR-cdvt,  UBR+-NA,   UBR-NA   )
         #define ERROR_CODE_TRAFADMININVALID    0x040E   // parametro admin invalido
         #define ERROR_CODE_TDDUPLICATENAME     0x040F
         #define ERROR_CODE_TRAFWRONGTYPE       0x0410
         #define ERROR_CODE_TRAFWRONGPCR        0x0411
         #define ERROR_CODE_TRAFWRONGCDVT       0x0412
         #define ERROR_CODE_TRAFWRONGSCR        0x0413
         #define ERROR_CODE_TRAFWRONGMBS        0x0414
         #define ERROR_CODE_TRAFWRONGMDCR       0x0415

      // Sub-familia VCL (0x05)
         #define ERROR_CODE_VCLNOTATMIF         0x0500
         #define ERROR_CODE_VCLATMIFDOWN        0x0501
         #define ERROR_CODE_VCLVPOUTOFRANGE     0x0502
         #define ERROR_CODE_VCLVCOUTOFRANGE     0x0503
         #define ERROR_CODE_VCLRXUPCMODENVALID  0x0504
         #define ERROR_CODE_VCLTXUPCMODENVALID  0x0505
         #define ERROR_CODE_VCLRXTDINVALID      0x0506
         #define ERROR_CODE_VCLTXTDINVALID      0x0507
         #define ERROR_CODE_VCLAALTYPE          0x0508
         #define ERROR_CODE_VCLINUSE            0x0509
         #define ERROR_CODE_VCLNOTD             0x050A   //
         #define ERROR_CODE_VCLDUPLICATENAME    0x050B
         #define ERROR_CODE_VCLENCAPINVALID     0x050C
         #define ERROR_CODE_VCLSDUSIZEINVALID   0x050D

      // Sub-familia VCC (0x06)
         #define ERROR_CODE_ATMVCCINDEX         0x0600
         #define ERROR_CODE_ATMVCCCOMPLETE      0x0601   //
         #define ERROR_CODE_ATMVCCADMININVALID  0x0602   //
         #define ERROR_CODE_ATMVCCOCUPIED       0x0603   //
         #define ERROR_CODE_ATMVCCDIFFSLOTS     0x0604   //
         #define ERROR_CODE_VCCTDNOMATCH        0x0605

      // Sub-familia ATMIPInterfaces (0x07)
         #define ERROR_CODE_ATMIPNETMASKINVALID 0x0700
         #define ERROR_CODE_ATMIPBCASTINVALID   0x0701
         #define ERROR_CODE_ATMIPADMINISUP      0x0702
         #define ERROR_CODE_ATMIPHASIPNOTNULL   0x0703
         #define ERROR_CODE_ATMIPNOIPADDR       0x0704

      // Sub-familia ATMRoute (0x08)

// Macros de utilização
#define SIR_ERROR(fam,sev,tip)      (((fam&0x000000FF)<<24) | ((sev&0x000000FF)<<16) | (tip & 0x0000FFFF))
#define FAMILY_ERROR(sir_error)     (((sir_error & 0xFF000000)>>24))
#define SEVERITY_ERROR(sir_error)   (((sir_error & 0x00FF0000)>>16))
#define TYPE_ERROR(sir_error)       ((sir_error & 0x0000FFFF))

#endif // GLNETBAND_MANAGER_SIRERROR_H
