xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/AssocClientStatusStatsTabsList.js");
function AssocClientStatus_SubmitStart()
{
}


function AssocClientStatus_SubmitEnd()
{
}

function AssocClientStatus_LoadStart()
{
  xeData.nTabInfo = tabsList;

  /*var inputDIV = "wlanSummaryTable";
  var outputDIV = "plotwlanSummaryTable";

  var clientSummaryTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 9);
  if(clientSummaryTableRepeatInfo.rows !=0 ){
     xui_JSPagination(clientSummaryTableRepeatInfo,outputDIV, '2_2');
  }*/
}

function AssocClientStatus_LoadEnd()
{
  loadme();
}

function sortTable(link)
{
    switch (link.firstChild.nodeValue) 
    {
    case "MAC Address<br>(*)-Peer Associated" :
        globalData.cells.sort(sortByMAC);
        break;
    case "Detected IP<br>Address":
        globalData.cells.sort(sortByIP);
        break;
    case "NetBIOS Name":
        globalData.cells.sort(sortByNetBios);
        break;
    case "SSID":
        globalData.cells.sort(sortBySSID);
        break;
    case "BSSID":
        globalData.cells.sort(sortByBSSID);
        break;
    case "Channel":
        globalData.cells.sort(sortByChannel);
        break;
    case "Status":
        globalData.cells.sort(sortByStatus);
        break;
    case "Network Time":
        globalData.cells.sort(sortByNwTime);
        break;
    default :
        globalData.cells.sort(sortByMAC);
        break;
    }
    reDrawPage(globalData);
    return;
}

function reDrawPage(data)
{
  xui_JSPagination(data,"plotwlanpagination_summary", '2_2');
}

function sortByMAC(a, b)
{
    var sa = a[1].value.toLowerCase();
    var sb = b[1].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByIP(a, b)
{
    var sa = a[2].value.toLowerCase();
    var sb = b[2].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByNetBios(a, b)
{
    var sa = a[3].value.toLowerCase();
    var sb = b[3].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortBySSID(a, b)
{
    var sa = a[4].value.toLowerCase();
    var sb = b[4].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByBSSID(a, b)
{
    var sa = a[5].value.toLowerCase();
    var sb = b[5].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByChannel(a, b)
{
    var sa = a[6].value.toLowerCase();
    var sb = b[6].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByStatus(a, b)
{
    var sa = a[7].value.toLowerCase();
    var sb = b[7].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByNwTime(a, b)
{
    var sa = a[8].value.toLowerCase();
    var sb = b[8].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}
