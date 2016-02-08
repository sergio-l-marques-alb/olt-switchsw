//////////////////////////////////////////////////////////////////////////
//                                                                      //
//              ||||||\   ||||||||   ||||||   ||    ||                  //
//              ||   ||      ||        ||     ||||  ||                  //
//              |||||||      ||        ||     || || ||                  //
//              ||           ||        ||     ||  ||||                  //
//              ||           ||        ||     ||   |||                  //
//              ||           ||      ||||||   ||    ||                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FileName:                                                            //
//            SFF_8472.c                                                //
//                                                                      //
//----------------------------------------------------------------------//
// Projecto:                                                            //
//            PR864_SDH_SOFT                                            //
//                                                                      //
//----------------------------------------------------------------------//
// Descrição: Este módulo possui as funções relativas a especificacao   //
//            SFF-8472  Rev. 9.5                                        //
//                                                                      //
//----------------------------------------------------------------------//
//                                                                      //
// Histórico:                                                           //
//            11 Maio 2005 - Primeira Versao                            //
//                                                                      //
//----------------------------------------------------------------------//
// Autor: Celso Lemos                                                   //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>             
#include <string.h> 

#include "sff_8472.h" 
#include "math.h" 


///////////////////////////////////////////////////////////////////////////////////
// Função:     SFF8472_BaseIDFields_to_strings                                    /
//                                                                                /
//--------------------------------------------------------------------------------/
// Descrição:  Esta funcao permite converter os campos dos "SpecificID Fields"    /
//             para strings.                                                      /                 
//                                                                                /
//--------------------------------------------------------------------------------/
// Parametros:                                                                    /
//             i2c_reg: Registo que contem o controlo do I2C                      /
//             BaseIdFields: Estrutura onde serao retornados os dados             /
//                                                                                /
//--------------------------------------------------------------------------------/
// retorno:                                                                       /
//             0:  Operação decorreu com sucesso                                  /
//             -1: Operação não foi efectuada com sucesso                         /
//--------------------------------------------------------------------------------/
// historico:                                                                     /
//         11 Maio 2005 - Primeira versao                                         /
//                                                                                /
//--------------------------------------------------------------------------------/
// Autor: Celso Lemos                                                             /
//                                                                                /
///////////////////////////////////////////////////////////////////////////////////
void SFF8472_BaseIdFields_to_strings(T_SFF_8472_BaseIdFields *BaseIdFields)
{
    int i;

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
    printf("+                                   BASE ID FIELDS                              +\n\r");
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");

    //Identifier
    switch (BaseIdFields->identifier) {
        case 00: printf("Identifier: Desconhecido nao especificado\n\r"); break;
        case 01: printf("Identifier: GBIC\n\r"); break;
        case 02: printf("Identifier: Modulo/Conector soldado na board\n\r"); break;
        case 03: printf("Identifier: SFP\n\r"); break;
        default:
            if ((BaseIdFields->identifier>=4) && (BaseIdFields->identifier<=0x7F))
                printf("Identifier: Codigo reservado\n\r");
            else
            if ((BaseIdFields->identifier>=0x80)/* && (BaseIdFields->identifier<=0xFF)*/)
                    printf("Identifier: Codigo especifico do fabricante: 0x%.2x\n\r",BaseIdFields->identifier);
            else
                printf("Identifier: ??????: 0x%.2x\n\r",BaseIdFields->identifier);
    }

    //Extended Identifier
    if (BaseIdFields->identifier==0x01) {
        switch (BaseIdFields->ext_identifier) {
            case 01: printf("Extended Identifier: GBIC is compliant with MOD_DEF 1\n\r"); break;
            case 02: printf("Extended Identifier: GBIC is compliant with MOD_DEF 2\n\r"); break;
            case 03: printf("Extended Identifier: GBIC is compliant with MOD_DEF 3\n\r"); break;
            case 04: printf("Extended Identifier: GBIC/SFP function is defined by serial ID only\n\r"); break;
            case 05: printf("Extended Identifier: GBIC is compliant with MOD_DEF 5\n\r"); break;
            case 06: printf("Extended Identifier: GBIC is compliant with MOD_DEF 6\n\r"); break;
            case 07: printf("Extended Identifier: GBIC is compliant with MOD_DEF 7\n\r"); break;

            default:
                if ((BaseIdFields->ext_identifier>=8)/* && (BaseIdFields->identifier<=0xFF)*/)
                    printf("Extended Identifier: Codigo reservado: 0x%.2x\n\r",BaseIdFields->ext_identifier);
                else
                    printf("Extended Identifier: ?????? 0x%.2x\n\r",BaseIdFields->ext_identifier);
        }
    }


    //Connector
    switch (BaseIdFields->connector) {
        case 0x00: printf("Connector: Unknown or unspecified\n\r"); break;
        case 0x01: printf("Connector: SC\n\r"); break;
        case 0x02: printf("Connector: Fiber Channel Style 1 copper connector\n\r"); break;
        case 0x03: printf("Connector: Fiber Channel Style 2 copper connector\n\r"); break;
        case 0x04: printf("Connector: BNC/TNC\n\r"); break;
        case 0x05: printf("Connector: Fiber Channel coaxial headers\n\r"); break;
        case 0x06: printf("Connector: Fiber Jack\n\r"); break;
        case 0x07: printf("Connector: LC\n\r"); break;
        case 0x08: printf("Connector: MT-RJ\n\r"); break;
        case 0x09: printf("Connector: MU\n\r"); break;
        case 0x0A: printf("Connector: SG\n\r"); break;
        case 0x0B: printf("Connector: Optical Pigtail\n\r"); break;
        case 0x20: printf("Connector: HSSDC II\n\r"); break;
        case 0x21: printf("Connector: Copper Pigtail\n\r"); break;

        default:
            printf("Connector: Reservado ou especifico: 0x%.2x\n\r",BaseIdFields->connector);
    }

    //Transceiver
    printf("Transceiver: 0x%02X%02X%02X%02X%02X%02X%02X%02X\n\r" ,
           BaseIdFields->transceiver[0],
           BaseIdFields->transceiver[1],
           BaseIdFields->transceiver[2],
           BaseIdFields->transceiver[3],
           BaseIdFields->transceiver[4],
           BaseIdFields->transceiver[5],
           BaseIdFields->transceiver[6],
           BaseIdFields->transceiver[7]
           );

    //Encodig
    switch (BaseIdFields->encoding) {
        case 0x00: printf("Encoding: Unknown or unspecified\n\r"); break;
        case 0x01: printf("Encoding: 8B10B\n\r"); break;
        case 0x02: printf("Encoding: 4B5B\n\r"); break;
        case 0x03: printf("Encoding: NRZ\n\r"); break;
        case 0x04: printf("Encoding: Manchester\n\r"); break;
        case 0x05: printf("Encoding: SONET Scrambled\n\r"); break;
        default:
            printf("Encoding: Reservado ou especifico: 0x%.2x\n\r",BaseIdFields->encoding);    
    }

    printf("Nominal Bitrate: %d MBits/sec.\n\r", BaseIdFields->nominal_bitrate*100);
    printf("Length (9im,km): %d km.\n\r", BaseIdFields->length_9im_km);
    printf("Length (9im)   : %d m.\n\r",  BaseIdFields->length_9im*100);
    printf("Length (50im)  : %d m.\n\r",  BaseIdFields->length_50im*10);
    printf("Length (62.5im): %d m.\n\r",  BaseIdFields->length_62_5im*10);
    printf("Length (Copper): %d m.\n\r",  BaseIdFields->length_copper);

    printf("Vendor Name: ");
    for (i=0;i<16;i++) {
        printf("%c",BaseIdFields->vendor_name[i]);
    }
    printf("\n\r");

    printf("Vendor OUI : 0x%.2x%.2x\n\r",BaseIdFields->vendor_OUI[0],BaseIdFields->vendor_OUI[2]);

    printf("Vendor PN  : ");
    for (i=0;i<16;i++) {
        printf("%c",BaseIdFields->vendor_PN[i]);
    }
    printf("\n\r");

    printf("Vendor rev : ");
    for (i=0;i<4;i++) {
        printf("%c",BaseIdFields->vendor_rev[i]);
    }
    printf("\n\r");

    printf("Laser Wavelength : %dnm\n\r",BaseIdFields->wavelength[0]*256+BaseIdFields->wavelength[1]);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");

}






///////////////////////////////////////////////////////////////////////////////////
// Função:     SFF8472_BaseIDFields_to_strings                                    /
//                                                                                /
//--------------------------------------------------------------------------------/
// Descrição:  Esta funcao permite converter os campos dos "SpecificID Fields"    /
//             para strings.                                                      /                 
//                                                                                /
//--------------------------------------------------------------------------------/
// Parametros:                                                                    /
//             i2c_reg: Registo que contem o controlo do I2C                      /
//             BaseIdFields: Estrutura onde serao retornados os dados             /
//                                                                                /
//--------------------------------------------------------------------------------/
// retorno:                                                                       /
//             0:  Operação decorreu com sucesso                                  /
//             -1: Operação não foi efectuada com sucesso                         /
//--------------------------------------------------------------------------------/
// historico:                                                                     /
//         11 Maio 2005 - Primeira versao                                         /
//                                                                                /
//--------------------------------------------------------------------------------/
// Autor: Celso Lemos                                                             /
//                                                                                /
///////////////////////////////////////////////////////////////////////////////////
void SFF8472_ExtendedIdFields_to_strings(T_SFF_8472_ExtendedIdFields *ExtendedIdFields)
{
    int i;

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
    printf("+                                EXTENDED ID FIELDS                             +\n\r");
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
    //Options
    printf("Options implemented:\n\r");
    if ((ExtendedIdFields->options[1] & 0x02)!=0) 
        printf("     -> Loss of Signal Implemented, signal as defined in SFP MSA\n\r");

    if ((ExtendedIdFields->options[1] & 0x04)!=0) 
        printf("     -> Loss of Signal Implemented, signal inverted from standard definition in SFP MSA\n\r");
    
    if ((ExtendedIdFields->options[1] & 0x08)!=0) 
        printf("     -> TX_FAULT signal implemented\n\r");

    if ((ExtendedIdFields->options[1] & 0x10)!=0) 
        printf("     -> TX_DISABLE signal implemented\n\r");
                        
    if ((ExtendedIdFields->options[1] & 0x20)!=0) 
        printf("     -> RATE_SELECT is implemented\n\r");

    printf("Max Baudrate: %d%%\n\r",ExtendedIdFields->max_br);
    printf("Min Baudrate: %d%%\n\r",ExtendedIdFields->min_br);

    printf("Vendor SN  : ");
    for (i=0;i<16;i++) {
        printf("%c",ExtendedIdFields->vendor_sn[i]);
    }
    printf("\n\r");

    //Date Code
    printf("Date Code : %c%c-%c%c-%c%c, lot code: %c%c\n\r",
           ExtendedIdFields->date_code[4],ExtendedIdFields->date_code[5],
           ExtendedIdFields->date_code[2],ExtendedIdFields->date_code[3],
           ExtendedIdFields->date_code[0],ExtendedIdFields->date_code[1],
           ExtendedIdFields->date_code[6],ExtendedIdFields->date_code[7]);

    printf("Diagonostic Monitoring Type:\n\r");
    
    if ((ExtendedIdFields->diagnostic & 0x04)!=0) 
        printf("     -> Address change required\n\r");

    if ((ExtendedIdFields->diagnostic & 0x08)!=0) 
        printf("     -> Received power measurement type = OMA\n\r");
    else
        printf("     -> Received power measurement type = Average Power\n\r");

    if ((ExtendedIdFields->diagnostic & 0x10)!=0) 
        printf("     -> Externally Calibrated\n\r");

    if ((ExtendedIdFields->diagnostic & 0x20)!=0) 
        printf("     -> Internally Calibrated\n\r");

    if ((ExtendedIdFields->diagnostic & 0x40)!=0) 
        printf("     -> Digital diagnostic monitoring implemented\n\r");

    if ((ExtendedIdFields->diagnostic & 0x80)!=0) 
        printf("     -> Reserved for legacy doagnostico (must be 0)\n\r");


    //Enhanced Options
    if ((ExtendedIdFields->enhanced_options & 0xFC)==0) 
        printf("Enhanced Options: none\n\r");
    else
        printf("Enhanced Options:\n\r");
    
    if ((ExtendedIdFields->enhanced_options & 0x04)!=0) 
        printf("     -> Optional Application Select control implemented per SFF-8079\n\r");

    if ((ExtendedIdFields->enhanced_options & 0x08)!=0) 
        printf("     -> Optional Soft RATE_SELECT control and monitoring implemented\n\r");

    if ((ExtendedIdFields->enhanced_options & 0x10)!=0) 
        printf("     -> Optional Soft RX_LOS monitoring implemented\n\r");

    if ((ExtendedIdFields->enhanced_options & 0x20)!=0) 
        printf("     -> Optional Soft TX_FAULT monitoring implemented\n\r");
    
    if ((ExtendedIdFields->enhanced_options & 0x40)!=0) 
        printf("     -> Optional Soft TX_DISABLE control and monitoring implemented\n\r");

    if ((ExtendedIdFields->enhanced_options & 0x80)!=0) 
        printf("     -> Optional Alarm//warning flasg implemented for all monitored quantities\n\r");



    //SFF-8472 Compliance
    printf("SFF-8472 Compliance: ");
    if (ExtendedIdFields->SFF8472_compliance==0x00) 
        printf("Digital Diagnostic functionality not included or undefined\n\r");
    else
    if (ExtendedIdFields->SFF8472_compliance==0x01) 
        printf("SFF-8472 Rev 9.3\n\r");
    else
    if (ExtendedIdFields->SFF8472_compliance==0x02) 
        printf("SFF-8472 Rev 9.4\n\r");
    else
        printf("-\n\r");


    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
}



//T_SFF_8472_ADStatusBits


///////////////////////////////////////////////////////////////////////////////////
// Função:     SFF8472_ADStatusBits_to_strings                                    /
//                                                                                /
//--------------------------------------------------------------------------------/
// Descrição:  Esta funcao permite converter os campos dos "SpecificID Fields"    /
//             para strings.                                                      /                 
//                                                                                /
//--------------------------------------------------------------------------------/
// Parametros:                                                                    /
//             i2c_reg: Registo que contem o controlo do I2C                      /
//             ADStatusBits: Estrutura onde serao retornados os dados             /
//                                                                                /
//--------------------------------------------------------------------------------/
// retorno:                                                                       /
//             0:  Operação decorreu com sucesso                                  /
//             -1: Operação não foi efectuada com sucesso                         /
//--------------------------------------------------------------------------------/
// historico:                                                                     /
//         31 Maio 2005 - Primeira versao                                         /
//                                                                                /
//--------------------------------------------------------------------------------/
// Autor: Celso Lemos                                                             /
//                                                                                /
///////////////////////////////////////////////////////////////////////////////////
void SFF8472_ADStatusBits_to_strings(T_SFF_8472_ADStatusBits *ADStatusBits)
{
    int tempreal;
    float v_real, tx_bias_real,tx_power_real,rx_power_real;

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
    printf("+                                AD Status Bits                                 +\n\r");
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
    //Options
    
    tempreal = (int)(((float)(ADStatusBits->TempMSB*256+ADStatusBits->TempLSB))/256.0);
    v_real   = (float)(((float)(ADStatusBits->VccMSB*256+ADStatusBits->VccLSB))/10000);
    tx_bias_real = (float)((((float)(ADStatusBits->TxBiasMSB*256+ADStatusBits->TxBiasLSB))*2)/1000); //mA
    tx_power_real = (float)(((float)(ADStatusBits->TxPowerMSB*256+ADStatusBits->TxPowerLSB))/10000); //mW
    rx_power_real = (float)(((float)(ADStatusBits->RxPowerMSB*256+ADStatusBits->RxPowerLSB))/10000); //mW

    printf("Status:\n \r");
    printf("  Temp     = 0x%.2x%.2x, %d C\n\r",ADStatusBits->TempMSB,ADStatusBits->TempLSB,tempreal);
    printf("  Voltage  = 0x%.2x%.2x, %f V\n\r",ADStatusBits->TxBiasMSB,ADStatusBits->TxBiasLSB,v_real);
    printf("  TxBias   = 0x%.2x%.2x, %f mA\n\r",ADStatusBits->TxBiasMSB,ADStatusBits->TxBiasLSB,tx_bias_real);
    printf("  TxPOWER  = 0x%.2x%.2x, %f mW, %f dBm\n\r",ADStatusBits->TxPowerMSB,ADStatusBits->TxPowerLSB,tx_power_real,10*log10(tx_power_real));
    printf("  RxPOWER  = 0x%.2x%.2x, %f mW, %f dBm\n\r",ADStatusBits->RxPowerMSB,ADStatusBits->RxPowerLSB,rx_power_real,10*log10(rx_power_real));
    printf("  OpStatus = 0x%.2x\n\r",ADStatusBits->Status_control_bits);
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
}




///////////////////////////////////////////////////////////////////////////////////
// Função:     SFF8472_ADStatusBits_to_strings                                    /
//                                                                                /
//--------------------------------------------------------------------------------/
// Descrição:  Esta funcao permite converter os campos dos "SpecificID Fields"    /
//             para strings.                                                      /                 
//                                                                                /
//--------------------------------------------------------------------------------/
// Parametros:                                                                    /
//             i2c_reg: Registo que contem o controlo do I2C                      /
//             ADStatusBits: Estrutura onde serao retornados os dados             /
//                                                                                /
//--------------------------------------------------------------------------------/
// retorno:                                                                       /
//             0:  Operação decorreu com sucesso                                  /
//             -1: Operação não foi efectuada com sucesso                         /
//--------------------------------------------------------------------------------/
// historico:                                                                     /
//         31 Maio 2005 - Primeira versao                                         /
//                                                                                /
//--------------------------------------------------------------------------------/
// Autor: Celso Lemos                                                             /
//                                                                                /
///////////////////////////////////////////////////////////////////////////////////
void SFF8472_AlarmThresholdcalibration_to_strings(T_SFF_8472_AlarmThreshold_calibration *AlarmThreshold_calibration)
{

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
    printf("+                        Alarm Threshold and calibration                        +\n\r");
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
    //Options
    
    printf("Alarm Threshold:\n \r");
    printf("    TempHighAlarm      = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->TempHighAlarmMSB,AlarmThreshold_calibration->TempHighAlarmLSB);
    printf("    TempLowAlarm       = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->TempLowAlarmMSB,AlarmThreshold_calibration->TempLowAlarmLSB);
    printf("    TempHighWarning    = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->TempHighWarningMSB,AlarmThreshold_calibration->TempHighWarningLSB);
    printf("    TempLowWarning     = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->TempLowWarningMSB,AlarmThreshold_calibration->TempLowWarningLSB);
    printf("    VoltageHighAlarm   = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->VoltageHighAlarmMSB,AlarmThreshold_calibration->VoltageHighAlarmLSB);
    printf("    VoltageLowAlarm    = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->VoltageLowAlarmMSB,AlarmThreshold_calibration->VoltageLowAlarmLSB);
    printf("    VoltageHighWarning = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->VoltageHighWarningMSB,AlarmThreshold_calibration->VoltageHighWarningLSB);
    printf("    VoltageLowWarning  = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->VoltageLowWarningMSB,AlarmThreshold_calibration->VoltageLowWarningLSB);
    printf("    BiasHighAlarm      = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->BiasHighAlarmMSB,AlarmThreshold_calibration->BiasHighAlarmLSB);
    printf("    BiasLowAlarm       = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->BiasLowAlarmMSB,AlarmThreshold_calibration->BiasLowAlarmLSB);
    printf("    BiasHighWarning    = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->BiasHighWarningMSB,AlarmThreshold_calibration->BiasHighWarningLSB);
    printf("    BiasLowWarning     = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->BiasLowWarningMSB,AlarmThreshold_calibration->BiasLowWarningLSB);
    printf("    TxPowerHighAlarm   = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->TxPowerHighAlarmMSB,AlarmThreshold_calibration->TxPowerHighAlarmLSB);
    printf("    TxPowerLowAlarm    = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->TxPowerLowAlarmMSB,AlarmThreshold_calibration->TxPowerLowAlarmLSB);
    printf("    TxPowerHighWarning = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->TxPowerHighWarningMSB,AlarmThreshold_calibration->TxPowerHighWarningLSB);
    printf("    TxPowerLowWarning  = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->TxPowerLowWarningMSB,AlarmThreshold_calibration->TxPowerLowWarningLSB);
    printf("    RxPowerHighAlarm   = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->RxPowerHighAlarmMSB,AlarmThreshold_calibration->RxPowerHighAlarmLSB);
    printf("    RxPowerLowAlarm    = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->RxPowerLowAlarmMSB,AlarmThreshold_calibration->RxPowerLowAlarmLSB);
    printf("    RxPowerHighWarning = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->RxPowerHighWarningMSB,AlarmThreshold_calibration->RxPowerHighWarningLSB);
    printf("    RxPowerLowWarning  = 0x%.2x%.2x\n\r",AlarmThreshold_calibration->RxPowerLowWarningMSB,AlarmThreshold_calibration->RxPowerLowWarningLSB);
    
    printf("calibration:\n \r");
    printf("    Calibration_Rx_PWR_4         = 0x%.2x%.2x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Rx_PWR_4[0],
           AlarmThreshold_calibration->Calibration_Rx_PWR_4[1],
           AlarmThreshold_calibration->Calibration_Rx_PWR_4[2],
           AlarmThreshold_calibration->Calibration_Rx_PWR_4[3]); 
    printf("    Calibration_Rx_PWR_3         = 0x%.2x%.2x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Rx_PWR_3[0],
           AlarmThreshold_calibration->Calibration_Rx_PWR_3[1],
           AlarmThreshold_calibration->Calibration_Rx_PWR_3[2],
           AlarmThreshold_calibration->Calibration_Rx_PWR_3[3]); 
    printf("    Calibration_Rx_PWR_2         = 0x%.2x%.2x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Rx_PWR_2[0],
           AlarmThreshold_calibration->Calibration_Rx_PWR_2[1],
           AlarmThreshold_calibration->Calibration_Rx_PWR_2[2],
           AlarmThreshold_calibration->Calibration_Rx_PWR_2[3]); 
    printf("    Calibration_Rx_PWR_1         = 0x%.2x%.2x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Rx_PWR_1[0],
           AlarmThreshold_calibration->Calibration_Rx_PWR_1[1],
           AlarmThreshold_calibration->Calibration_Rx_PWR_1[2],
           AlarmThreshold_calibration->Calibration_Rx_PWR_1[3]); 
    printf("    Calibration_Rx_PWR_0         = 0x%.2x%.2x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Rx_PWR_0[0],
           AlarmThreshold_calibration->Calibration_Rx_PWR_0[1],
           AlarmThreshold_calibration->Calibration_Rx_PWR_0[2],
           AlarmThreshold_calibration->Calibration_Rx_PWR_0[3]); 
    printf("    Calibration_Tx_I_Slope       = 0x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Tx_I_Slope[0],
           AlarmThreshold_calibration->Calibration_Tx_I_Slope[1]);
    printf("    Calibration_Tx_I_Offset      = 0x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Tx_I_Offset[0],
           AlarmThreshold_calibration->Calibration_Tx_I_Offset[1]);
    printf("    Calibration_Tx_Power_Slope   = 0x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Tx_Power_Slope[0],
           AlarmThreshold_calibration->Calibration_Tx_Power_Slope[1]);
    printf("    Calibration_Tx_Power_Offset  = 0x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Tx_Power_Offset[0],
           AlarmThreshold_calibration->Calibration_Tx_Power_Offset[1]);
    printf("    Calibration_Temp_Slope       = 0x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Temp_Slope[0],
           AlarmThreshold_calibration->Calibration_Temp_Slope[1]);
    printf("    Calibration_Temp_Offset      = 0x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_Temp_Offset[0],
           AlarmThreshold_calibration->Calibration_Temp_Offset[1]);
    printf("    Calibration_V_Slope           = 0x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_V_Slope[0],
           AlarmThreshold_calibration->Calibration_V_Slope[1]);
    printf("    Calibration_V_Offset         = 0x%.2x%.2x\n\r",
           AlarmThreshold_calibration->Calibration_V_Offset[0],
           AlarmThreshold_calibration->Calibration_V_Offset[1]);

    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");
}

