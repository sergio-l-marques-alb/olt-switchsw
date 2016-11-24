#include "strlib_metro_cli.h"

L7_char8 *pStrInfo_metro_dot1ad = "dot1ad";
L7_char8 *pStrInfo_metro_dot1ad_svid = "svid";
L7_char8 *pStrInfo_metro_dot1ad_svid_help = "service vlan id";
L7_char8 *pStrInfo_metro_isolate_vid = "<isolate-vid>";
L7_char8 *pStrInfo_metro_isolate_vid_help = "Isolate VLAN ID";
L7_char8 *pStrInfo_metro_dot1ad_help = "configure dot1ad";
L7_char8 *pStrInfo_metro_dot1ad_help_1 = "dot1ad service instance parameters";
L7_char8 *pStrInfo_metro_service  = "service";
L7_char8 *pStrInfo_metro_service_help =   "create a service instance";
L7_char8 *pStrInfo_metro_serviceName = "<service-name>";
L7_char8 *pStrInfo_metro_serviceName_help = "specify the name of the service";
L7_char8 *pStrInfo_metro_dot1ad_upQos = "upstream-qos";
L7_char8 *pStrInfo_metro_dot1ad_upQos_help = "configure QoS parameters for upstream";
L7_char8 *pStrInfo_metro_dot1ad_downQos = "downstream-qos";
L7_char8 *pStrInfo_metro_dot1ad_downQos_help = "configure QoS parameters for downstream";
L7_char8 *pStrInfo_metro_subscrName = "<subscription-name>";
L7_char8 *pStrInfo_metro_subscrName_help = "specify the name of the subscription";
L7_char8 *pStrInfo_metro_servicevid_help = "service vlan ID";
L7_char8 *pStrInfo_metro_elan = "e-lan";
L7_char8 *pStrInfo_metro_elan_help = "e-lan service";
L7_char8 *pStrInfo_metro_etree = "e-tree";
L7_char8 *pStrInfo_metro_etree_help = "e-tree service";
L7_char8 *pStrInfo_metro_eline = "e-line";
L7_char8 *pStrInfo_metro_eline_help = "e-line service";
L7_char8 *pStrInfo_metro_tls = "tls";
L7_char8 *pStrInfo_metro_tls_help = "transparent LAN service";
L7_char8 *pStrInfo_metro_nni = "nni";
L7_char8 *pStrInfo_metro_nni_help = "NNI Port List";
L7_char8 *pStrInfo_metro_port_list = "<port_list>";
L7_char8 *pStrInfo_metro_port_list_help = "Specify the NNI Port List - separate non-consecutive ports with ',' and no\r\nspaces between the range; Use '-' for range.";
L7_char8 *pStrInfo_metro_subscribe = "subscribe";
L7_char8 *pStrInfo_metro_subscribe_help = "subscribe to a configured service";
L7_char8 *pStrInfo_metro_match = "match" ;
L7_char8 *pStrInfo_metro_match_help = "Match field";
L7_char8 *pStrInfo_metro_untaggedPkt = "untagged-pkt";
L7_char8 *pStrInfo_metro_untaggedPkt_help = "all untagged packets";
L7_char8 *pStrInfo_metro_assignCvid = "assign-cvid";
L7_char8 *pStrInfo_metro_assignCvid_help = "assign Customer Tag VLAN ID";
L7_char8 *pStrInfo_metro_cvid = "cvid ";
L7_char8 *pStrInfo_metro_svid = "svid ";
L7_char8 *pStrInfo_metro_cvid_help = "customer VLAN ID";
L7_char8 *pStrInfo_metro_cvid_value_help = "Value of customer VLAN ID";
L7_char8 *pStrInfo_metro_priority = "priority";
L7_char8 *pStrInfo_metro_priority_help = "priority of the priority tagged packet";
L7_char8 *pStrInfo_metro_priorityVal_help = "priority value of the priority tagged packet";
L7_char8 *pStrInfo_metro_cvid_help1 = "customer VLAN ID of a tagged packet";
L7_char8 *pStrInfo_metro_svid_help1 = "service VLAN ID of a tagged packet";
L7_char8 *pStrInfo_metro_svlan_range = "<%d-%d>";
L7_char8 *pStrInfo_metro_priority_range_0_7 = "<0-7>" ;
L7_char8 *pStrInfo_metro_remark_cvid = "remark-cvid";
L7_char8 *pStrInfo_metro_remove_ctag = "remove-ctag";
L7_char8 *pStrInfo_metro_remark_cvid_help = "new customer VLAN ID " ;
L7_char8 *pStrInfo_metro_remove_ctag_help = "remove the customer vlan tag" ;
L7_char8 *pStrInfo_metro_assignQueue = "assign-queue";
L7_char8 *pStrInfo_metro_assignQueue_help = "assign the packet to specified queue";
L7_char8 *pStrInfo_metro_queue_0_3 = "<0-3>";
L7_char8 *pStrInfo_metro_queue_help = "queue to be assigned";
L7_char8 *pStrInfo_metro_PoliceSimple = "police-simple";
L7_char8 *pStrInfo_metro_PoliceSimpleHelp =  "Configure Simple traffic policing action.";
L7_char8 *pStrInfo_metro_Range1to2147483647 = "<1-4294967295>";
L7_char8 *pStrInfo_metro_Range64to2147483647_1 = "<64-4294967295>";
L7_char8 *pStrInfo_metro_DataRateKbps = "Enter an integer in the range of 1 to 4294967295 specifying conforming data\r\nrate in Kbps.";
L7_char8 *pStrInfo_metro_DiffservDataRateKbpsRobo = "Enter an integer in the range of 64 to 4294967295 specifying conforming data\r\nrate in Kbps with step size 64. Otherwise entered value will be rounded to\r\nnearest 64";

L7_char8 *pStrInfo_metro_Range1to128 = "<1-128>";
L7_char8 *pStrInfo_metro_BurstSize = "Enter burst size in the range of 1 to 128 in Kbytes.";
L7_char8 *pStrInfo_metro_ConformAction = "conform-action";
L7_char8 *pStrInfo_metro_CfgPoliceConform = "Specify action taken on Conforming traffic.";
L7_char8 *pStrInfo_metro_Drop = "drop";
L7_char8 *pStrInfo_metro_ConformDrop = "Enter action of drop on conforming traffic.";
L7_char8 *pStrInfo_metro_ConformSend = "Enter action of send on conforming traffic.";
L7_char8 *pStrInfo_metro_ViolateAction = "violate-action";
L7_char8 *pStrInfo_metro_PoliceNonConform =  "Specify action taken on Nonconforming traffic.";
L7_char8 *pStrInfo_metro_NonConformDrop = "Enter action of drop on nonconforming traffic.";
L7_char8 *pStrInfo_metro_NonConformSend ="Enter action of send on nonconforming traffic.";
L7_char8 *pStrInfo_metro_serviceAll_help = "All service instances";
L7_char8 *pStrInfo_metro_Interface= "interface";
L7_char8 *pStrInfo_metro_Interface_help = "interface number";
L7_char8 *pStrInfo_metro_service_subscr = "service-subscription";
L7_char8 *pStrInfo_metro_dot1ad_service_subcr_help = "subscribed services";
L7_char8 *pStrErr_metro_CfgDot1adServiceCreate = "Use dot1ad service <service-name> <svid> {elan | e-line | tls} [isolate]";
L7_char8 *pStrErr_metro_CfgDot1adInvalidServiceName = "Invalid service Name";
L7_char8 *pStrErr_metro_CfgDot1adNonAlphaNumericServiceName = "Invalid service name.Service name may only contain alphanumeric characters.";
L7_char8 *pStrErr_metro_CfgDot1adNonAlphaNumericSubscrName = "Invalid subscription name.Subscription name may only contain alphanumeric\r\ncharacters.";
L7_char8 *pStrErr_metro_CfgDot1adServiceExists = "Service with this name is already configured";
L7_char8 *pStrErr_metro_CfgDot1adServiceNameDonotExists = "Specified service name does not exist";
L7_char8 *pStrErr_metro_CfgDot1adSvidExists = "Service with this service VLAN ID is already configured";
L7_char8 *pStrErr_metro_CfgDot1adIsolateVidExists = "Service with this Isolate VLAN ID is already configured";
L7_char8 *pStrErr_metro_CfgDot1adIsolateVidEqualsToSvid = "Isolate VLAN ID is equal to service VLAN ID of already configured service";
L7_char8 *pStrErr_metro_CfgDot1adSvidEqualsToIsolateVid = "Service VLAN ID is equal to Isolate VLAN ID of already configured service";
L7_char8 *pStrErr_metro_CfgDot1adSvidEqualsToIsolateVid_1 = "Service VLAN ID is equal to Isolate VLAN ID";
L7_char8 *pStrErr_metro_CfgDot1adSvidNotInRange = "Service VLAN ID entered is not in Range";
L7_char8 *pStrErr_metro_CfgDot1adConfigFail = "Creating Service Failed";
L7_char8 *pStrErr_metro_CfgDot1adServiceDeleteFail = "Failed to delete the specified service";
L7_char8 *pStrErr_metro_CfgDot1adConfigFailTblFull = "Failed Creating Service !!! Max services reached.";
L7_char8 *pStrErr_metro_CfgDot1adConfigFailSubFull = "Failed to Apply Service on this port !!! Max subscriptions reached.";
L7_char8 *pStrErr_metro_CfgDot1adSvcNotConfigured ="Service specified is not available";
L7_char8 *pStrErr_metro_dot1adServiceSyntax  = "dot1ad service <service-name> svid <1-4094> {e-lan[isolate] | e-line | tls } [nni <port_list>]";
L7_char8 *pStrErr_metro_dot1adServiceQosSyntax = "{downstream-qos | upstream-qos} assign-queue <0-3>\r\n[police-simple <64-4294967295> <1-128> conform-action [drop | transmit]\r\nviolate-action [transmit | drop]]";
L7_char8 *pStrErr_metro_dot1adSubTls = "subscribe <service-name> <subscription-name> [assign-queue <0-3>\r\n[police-simple <64-4294967295> <1-128> conform-action [drop | transmit]\r\nviolate-action [transmit | drop]]]";
L7_char8 *pStrErr_metro_dot1adSubCvid = "subscribe <service-name> <subscr-name> match cvid <1-4094> [remark-cvid <1-4094> ]\r\n[remove-ctag] [priority <0-7>] [assign-queue <0-3>\r\n[police-simple <64-4294967295> <1-128> conform-action [drop | transmit]\r\nviolate-action [transmit | drop]]]";
L7_char8 *pStrErr_metro_dot1adSubPriority = "subscribe <service-name> <subscr-name> match priority <0-7>\r\n[assign-cvid <1-4094>] [assign-queue <0-3> [police-simple <64-4294967295>\r\n<1-128> conform-action [drop | transmit] violate-action [transmit | drop]]]";
L7_char8 *pStrErr_metro_dot1adSubUntag = "subscribe <service-name> <subscr-name> match untagged-pkt\r\n[assign-cvid <1-4094>] [assign-queue <0-3> [police-simple <64-4294967295>\r\n<1-128> conform-action [drop | transmit] violate-action [transmit | drop]]]";
L7_char8 *pstrErr_metro_dot1adSVlanDefaultVlanId = "Dot1ad service cannot be associated with management VLAN ID";








