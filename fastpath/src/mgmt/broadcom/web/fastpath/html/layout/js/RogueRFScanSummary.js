function RogueRFScanSummary_SubmitStart()
{
}


function RogueRFScanSummary_SubmitEnd()
{
}

function RogueRFScanSummary_LoadStart()
{
  var inputDIV = "wlanSummaryTable";
  var outputDIV = "plotwlanSummaryTable";

  var rfScanSummaryTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 7);
  if(rfScanSummaryTableRepeatInfo.rows !=0 ){
     xui_JSPagination(rfScanSummaryTableRepeatInfo,outputDIV, '2_1');
  }

}

function RogueRFScanSummary_LoadEnd()
{
}

function reDrawPage(data)
{
  xui_JSPagination(data,"plotwlanSummaryTable", '2_1');
}

function sortTable(link)
{
    switch (link.firstChild.nodeValue) 
  {
    case "MAC Address" :
        globalData.cells.sort(sortByMAC);
        break;
    case "SSID":
        globalData.cells.sort(sortBySSID);
        break;
    case "Physical Mode":
        globalData.cells.sort(sortByMode);
        break;
    case "Channel":
        globalData.cells.sort(sortByChannel);
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
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByMode(a, b)
{
    var sa = a[3].value.toLowerCase();
    var sb = b[3].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByChannel(a, b)
{
    var sa = parseInt(a[4].value);
    var sb = parseInt(b[4].value);
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByStatus(a, b)
{
    var sa = a[5].value.toLowerCase();
    var sb = b[5].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByAge(a, b)
{
    var sa = a[6].value.toLowerCase();
    var sb = b[6].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}
