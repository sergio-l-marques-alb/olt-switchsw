xuiIncludeJSFile("/scripts/global.js");
function wlan_BasicSetupTab7_SubmitStart(submitFlag)
{
}
function wlan_BasicSetupTab7_SubmitEnd(submitFlag)
{
}

function wlan_BasicSetupTab7_LoadStart(arg)
{

  xeData.nTabInfo = [ ["Global","WirelessGlobalConfig.html"], ["Discovery","WirelessDiscoveryConfig.html"], ["Profile","WirelessDefaultProfileConfig.html"], ["Radio","profile_radio_cfg_rw.html"], ["VAP","profile_vap_summ.html"], ["Valid AP","local_ap_database_summ.html"] ];

  /*var inputDIV = "wlanSummaryTable";
  var outputDIV = "plotwlanSummaryTable";

  var ouiTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 3);
  if(ouiTableRepeatInfo.rows !=0 ){
     xui_JSPagination(ouiTableRepeatInfo,outputDIV, '2_2');
}*/
}

function wlan_BasicSetupTab7_LoadEnd(arg)
{
  loadme();
}

/*
function reDrawPage(data)
{
  xui_JSPagination(data,"plotwlanSummaryTable", '2_2');
}

function sortTable(link)
{
    switch (link.firstChild.nodeValue) 
    {
    case "OUI Value" :
        globalData.cells.sort(sortByOuiVal);
        break;
    case "OUI Description" :
        globalData.cells.sort(sortByOui);
        break;
    default :
        globalData.cells.sort(sortByOuival);
        break;
    }
    reDrawPage(globalData);
    return;
}

function sortByOuiVal(a, b)
{
    var sa = a[1].value.toLowerCase();
    var sb = b[1].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByOui(a, b)
{
    var sa = a[2].value.toLowerCase();
    var sb = b[2].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}*/
