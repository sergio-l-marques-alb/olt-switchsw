xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/AssocClientStatusStatsTabsList.js");
function AssocClientStatsSessionSummary_SubmitStart()
{
}


function AssocClientStatsSessionSummary_SubmitEnd()
{
}

function AssocClientStatsSessionSummary_LoadStart()
{
  xeData.nTabInfo = tabsList;

  /*var inputDIV = "wlanSummaryTable";
  var outputDIV = "plotwlanSummaryTable";

  var clientSummaryTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 5);
  if(clientSummaryTableRepeatInfo.rows !=0 ){
     xui_JSPagination(clientSummaryTableRepeatInfo,outputDIV, '2_2');*/

}

function AssocClientStatsSessionSummary_LoadEnd()
{
  loadme();
}
/*
function reDrawPage(data)
{
  xui_JSPagination(data,"plotwlanpagination_summary", '2_2');
}

function sortTable(link)
{
    switch (link.firstChild.nodeValue) {
    case "MAC Address" :
        globalData.cells.sort(sortByMAC);
        break;
    case "Packets Received" :
        globalData.cells.sort(sortByPktsRx);
        break;
    case "Bytes Received" :
        globalData.cells.sort(sortByBytesRx);
        break;
    case "Packets Transmitted" :
        globalData.cells.sort(sortByPktsTx);
        break;
    case "Bytes Transmitted" :
        globalData.cells.sort(sortByBytesTx);
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
    var sa = a[0].value.toLowerCase();
    var sb = b[0].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByPktsRx(a, b)
{
    return a[1].value - b[1].value;
}

function sortByBytesRx(a, b)
{
    return a[2].value - b[2].value;
}

function sortByPktsTx(a, b)
{
    return a[3].value - b[3].value;
}

function sortByBytesTx(a, b)
{
    return a[4].value - b[4].value;
}*/
