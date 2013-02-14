xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/AssocClientStatusStatsTabsList.js");
function AssocClientVAP_SubmitStart()
{
}


function AssocClientVAP_SubmitEnd()
{
}

function AssocClientVAP_LoadStart()
{
  xeData.nTabInfo = tabsList;

  /*var inputDIV = "wlanSummaryTable";
  var outputDIV = "plotwlanSummaryTable";

  var clientVAPTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 6);
  if(clientVAPTableRepeatInfo.rows !=0 ){
     xui_JSPagination(clientVAPTableRepeatInfo,outputDIV, '2_2');*/
}

function AssocClientVAP_LoadEnd()
{
  loadme();
}

/*function reDrawPage(data)
{
  xui_JSPagination(data,"plotwlanpagination_summary", '2_2');
}

function sortTable(link)
{
    switch (link.firstChild.nodeValue) {
    case "BSSID" :
        globalData.cells.sort(sortByBSSID);
        break;
    case "AP MAC Address" :
        globalData.cells.sort(sortByApMAC);
        break;
    case "Location" :
        globalData.cells.sort(sortByLocation);
        break;
    case "Radio" :
        globalData.cells.sort(sortByRadio);
        break;
    case "Client MAC Address" :
        globalData.cells.sort(sortByClientMAC);
        break;
    default :
        globalData.cells.sort(sortByBSSID);
        break;
    }
    reDrawPage(globalData);
    return;
}

function sortByBSSID(a, b)
{
    var sa = a[1].value.toLowerCase();
    var sb = b[1].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByApMAC(a, b)
{
    var sa = a[2].value.toLowerCase();
    var sb = b[2].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByLocation(a, b)
{
    var sa = a[3].value.toLowerCase();
    var sb = b[3].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByRadio(a, b)
{
    var sa = a[4].value.toLowerCase();
    var sb = b[4].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByClientMAC(a, b)
{
    var sa = a[5].value.toLowerCase();
    var sb = b[5].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}*/
