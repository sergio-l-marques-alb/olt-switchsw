#ifndef _SFF_8472_
#define _SFF_8472_


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                 
//Serial ID: Data Fields - Adress 0xA0
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                 

typedef struct { // Base Id Fields

    unsigned char identifier;         // 00 - Type of serial transceiver (see Table 3.2)
    unsigned char ext_identifier;     // 01 - Extended type of serial transceiver (see Table 3.3)
    unsigned char connector;          // 02 - Code for connector type (see Table 3.4)
    unsigned char transceiver[8];     // 03 - Code for electronic compatibility or optical compatibility
    unsigned char encoding;           // 11 - Code for serial encoding algorith (see table 3.6)
    unsigned char nominal_bitrate;    // 12 - Nominal bitrate, units of 100Mbits/sec
    unsigned char reserved0;          // 13
    unsigned char length_9im_km;      // 14 - Link Length suported for 9/125 im fiber, units of Km
    unsigned char length_9im;         // 15 - Link Length suported for 9/125 im fiber, units of 100m
    unsigned char length_50im;        // 16 - Link Length suported for 50/125 im fiber, units of 10m
    unsigned char length_62_5im;      // 17 - Link Length suported for 62.5/125 im fiber, units of 10m    
    unsigned char length_copper;      // 18 - Link Length suported for copper, units of meters
    unsigned char reserved1;          // 19
    unsigned char vendor_name[16];    // 20 - SFP vendor name (ASCII)      
    unsigned char reserved2;          // 36
    unsigned char vendor_OUI[3];      // 37 - SFP vendor IEEE company ID
    unsigned char vendor_PN[16];      // 40 - Part number provided by vendor (ASCII)
    unsigned char vendor_rev[4];      // 56 - Revision level for part number provided by vendor (ASCII)
    unsigned char wavelength[2];      // 60 - Laser wavelength
    unsigned char reserved3;          // 62
    unsigned char cc_base;            // 63 - Check code for Base Fields (adresses 0 to 62)

} T_SFF_8472_BaseIdFields;



typedef struct { //Extended Id Fields

    unsigned char options[2];         // 64 - Indicates which optional transceiver signals are implemented (3.7) 
    unsigned char max_br;             // 66 - Upper bit rate margin, units of %
    unsigned char min_br;             // 67 - Lower bit rate margin, units of %
    unsigned char vendor_sn[16];      // 68 - Serial number provided by vendor (ASCII)
    unsigned char date_code[8];       // 84 - Vendor's manufacturing data code (table 3.8)
    unsigned char diagnostic;         // 92 - Indicates which type of diagnostic monitoring is implemented (3.9)
    unsigned char enhanced_options;   // 93 - Indicates which optional enhanced features are implemented (3.10)
    unsigned char SFF8472_compliance; // 94 - Indicates which revision of SFF8472 the transceiver complies with (3.12)
    unsigned char cc_ext;             // 95 - Check code for Extended Fields (adresses 64 to 94)

} T_SFF_8472_ExtendedIdFields;


typedef struct { //Specific Id Fields

    unsigned char VendorSpecific[32]; // 96 - Vendor specific EEPROM

} T_SFF_8472_SpecificIdFields;



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                 
//Alarm and warning thresholds - Adress 0xA2
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                 
typedef struct { //AD Status Bits

    unsigned char TempMSB;              //  96 - Internally measured module temperature MSB
    unsigned char TempLSB;              //  97 - Internally measured module temperature LSB
    unsigned char VccMSB;               //  98 - Internally measured supply voltage in transceiver MSB
    unsigned char VccLSB;               //  99 - Internally measured supply voltage in transceiver LSB
    unsigned char TxBiasMSB;            // 100 - Measured Tx Bias Current (MSB)
    unsigned char TxBiasLSB;            // 101 - Measured Tx Bias Current (LSB)
    unsigned char TxPowerMSB;           // 102 - Measured Tx Output power (MSB)
    unsigned char TxPowerLSB;           // 103 - Measured Tx Output power (LSB)
    unsigned char RxPowerMSB;           // 104 - Measured Rx Output power (MSB)
    unsigned char RxPowerLSB;           // 105 - Measured Rx Output power (LSB)
    unsigned char Reserved1MSB;         // 106 -
    unsigned char Reserved1LSB;         // 107 -
    unsigned char Reserved2MSB;         // 108 -
    unsigned char Reserved2LSB;         // 109 -
    unsigned char Status_control_bits;  // 110 - Optional status/control bits

} T_SFF_8472_ADStatusBits;


typedef struct {

    unsigned char TempHighAlarmMSB;        // - 00
    unsigned char TempHighAlarmLSB;        // - 01

    unsigned char TempLowAlarmMSB;         // - 02
    unsigned char TempLowAlarmLSB;         // - 03

    unsigned char TempHighWarningMSB;      // - 04
    unsigned char TempHighWarningLSB;      // - 05

    unsigned char TempLowWarningMSB;       // - 06
    unsigned char TempLowWarningLSB;       // - 07

    unsigned char VoltageHighAlarmMSB;     // - 08
    unsigned char VoltageHighAlarmLSB;     // - 09

    unsigned char VoltageLowAlarmMSB;      // - 10
    unsigned char VoltageLowAlarmLSB;      // - 11

    unsigned char VoltageHighWarningMSB;   // - 12
    unsigned char VoltageHighWarningLSB;   // - 13

    unsigned char VoltageLowWarningMSB;    // - 14
    unsigned char VoltageLowWarningLSB;    // - 15
    
    unsigned char BiasHighAlarmMSB;        // - 16
    unsigned char BiasHighAlarmLSB;        // - 17

    unsigned char BiasLowAlarmMSB;         // - 18
    unsigned char BiasLowAlarmLSB;         // - 19

    unsigned char BiasHighWarningMSB;      // - 20
    unsigned char BiasHighWarningLSB;      // - 21
    
    unsigned char BiasLowWarningMSB;       // - 22
    unsigned char BiasLowWarningLSB;       // - 23
    
    unsigned char TxPowerHighAlarmMSB;     // - 24
    unsigned char TxPowerHighAlarmLSB;     // - 25

    unsigned char TxPowerLowAlarmMSB;      // - 26
    unsigned char TxPowerLowAlarmLSB;      // - 27

    unsigned char TxPowerHighWarningMSB;   // - 28
    unsigned char TxPowerHighWarningLSB;   // - 29

    unsigned char TxPowerLowWarningMSB;    // - 30
    unsigned char TxPowerLowWarningLSB;    // - 31

    unsigned char RxPowerHighAlarmMSB;     // - 32
    unsigned char RxPowerHighAlarmLSB;     // - 33

    unsigned char RxPowerLowAlarmMSB;      // - 34
    unsigned char RxPowerLowAlarmLSB;      // - 35

    unsigned char RxPowerHighWarningMSB;   // - 36
    unsigned char RxPowerHighWarningLSB;   // - 37
    
    unsigned char RxPowerLowWarningMSB;    // - 38
    unsigned char RxPowerLowWarningLSB;    // - 39

    unsigned char reserved[16];            // - 40 a 55

    unsigned char Calibration_Rx_PWR_4[4];        // - 56 a 59
    unsigned char Calibration_Rx_PWR_3[4];        // - 60 a 63
    unsigned char Calibration_Rx_PWR_2[4];        // - 64 a 67
    unsigned char Calibration_Rx_PWR_1[4];        // - 68 a 71
    unsigned char Calibration_Rx_PWR_0[4];        // - 72 a 75
    
    unsigned char Calibration_Tx_I_Slope[2];      // - 76 a 77
    unsigned char Calibration_Tx_I_Offset[2];     // - 78 a 79

    unsigned char Calibration_Tx_Power_Slope[2];  // - 80 a 81
    unsigned char Calibration_Tx_Power_Offset[2]; // - 82 a 83

    unsigned char Calibration_Temp_Slope[2];      // - 84 a 85
    unsigned char Calibration_Temp_Offset[2];     // - 86 a 87

    unsigned char Calibration_V_Slope[2];          // - 88 a 89
    unsigned char Calibration_V_Offset[2];        // - 90 a 91

    unsigned char Reserved[3];

    unsigned char Checksum;                       // - 95
    
} T_SFF_8472_AlarmThreshold_calibration;




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                 
// DECLARACAO DAS FUNCOES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                 

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void SFF8472_BaseIdFields_to_strings(T_SFF_8472_BaseIdFields *BaseIdFields);

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void SFF8472_ExtendedIdFields_to_strings(T_SFF_8472_ExtendedIdFields *ExtendedIdFields);

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void SFF8472_ADStatusBits_to_strings(T_SFF_8472_ADStatusBits *ADStatusBits);

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void SFF8472_AlarmThresholdcalibration_to_strings(T_SFF_8472_AlarmThreshold_calibration *AlarmThreshold_calibration);


#endif //__SFF_8472__
