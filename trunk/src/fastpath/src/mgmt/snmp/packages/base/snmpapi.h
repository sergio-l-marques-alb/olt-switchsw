/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\snmp\snmp_sr\src\snmpd\unix\snmpapi.h
*
* @purpose Provide SNMP API constants
*
* @component unitmgr - snmp
*
* @comments tba
*
* @create 12/20/2000
*
* @author cpverne
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

#ifndef SNMPAPI_H
#define SNMPAPI_H

#define SNMP_ONE_MBPS               1000000
#define SNMP_TEN_MBPS               10000000
#define SNMP_HUNDRED_MBPS           100000000
#define SNMP_THOUSAND_MBPS          1000000000
#define SNMP_2P5THOUSAND_MBPS       2500000000UL  /* PTin added (2.5G) */
#define SNMP_TENTHOUSAND_MBPS       10000000000
#define SNMP_IFSPEEDMAX_MBPS        4294967295UL  /* max allowed ifSpeed value */

#define SNMP_TEN        10
#define SNMP_HUNDRED    100
#define SNMP_THOUSAND   1000
#define SNMP_2P5THOUSAND 2500   /* PTin added (2.5G) */
#define SNMP_TENTHOUSAND 10000

#define SNMP_MINUTES    (60)
#define SNMP_HOURS      ((60) * (60))
#define SNMP_DAYS       (((60) * (60)) * (24))

#define SNMP_TIMETICKS_PER_SECOND SNMP_HUNDRED
#endif /* SNMPAPI_H */
