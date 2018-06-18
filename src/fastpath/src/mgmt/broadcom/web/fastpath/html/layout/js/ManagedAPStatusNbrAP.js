xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/ManagedAPStatusStatsTabsList.js");
function ManagedAPStatusNbrAP_SubmitStart()
{
}


function ManagedAPStatusNbrAP_SubmitEnd()
{
}

function ManagedAPStatusNbrAP_LoadStart()
{
  xeData.nTabInfo = tabsList;

  /*var inputDIV = "wlanSummaryTable";
  var outputDIV = "plotwlanSummaryTable";

  var apNbrTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 5);
  if(apNbrTableRepeatInfo.rows !=0 ){
     xui_JSPagination(apNbrTableRepeatInfo,outputDIV, '2_1');
  } */ 
}

function ManagedAPStatusNbrAP_LoadEnd()
{
  loadme();
}

/*
function reDrawPage(data)
{
  xui_JSPagination(data,"plotwlanSummaryTable", '2_1');
}

function sortTable(link)
{
    switch (link.firstChild.nodeValue) {
    case "Neighbor AP MAC" :
        globalData.cells.sort(sortByMAC);
        break;
    case "SSID":
        globalData.cells.sort(sortBySSID);
        break;
    case "RSSI":
        globalData.cells.sort(sortByRSSI);
        break;
    case "Status":
        globalData.cells.sort(sortByStatus);
        break;
    case "Age":
        globalData.cells.sort(sortByAge);
        break;
    default :
        globalData.cells.sort(sortByMAC);
        break;
    }
  reDrawPage(globalData);
    return;
}

function sortByMAC(a, b)
{
    var sa = a[1].value.toLowerCase();
    var sb = b[1].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortBySSID(a, b)
{
    var sa = a[2].value.toLowerCase();
    var sb = b[2].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : sortByMAC(a, b)));
}

function sortByRSSI(a, b)
{
    return a[3].value - b[3].value;
}

function sortByStatus(a, b)
{
    var sa = a[4].value.toLowerCase();
    var sb = b[4].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : sortByMAC(a, b)));
}

function sortByAge(a, b)
{
    var sa = a[5].value.toLowerCase();
    var sb = b[5].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : sortByMAC(a, b)));
}*/
