xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/ManagedAPStatusStatsTabsList.js");
function ManagedAPSummary_SubmitStart()
{
}


function ManagedAPSummary_SubmitEnd()
{
}

function ManagedAPSummary_LoadStart()
{
  xeData.nTabInfo = tabsList;
  
/*  var inputDIV = "wlanSummaryTable";
  var outputDIV = "plotwlanSummaryTable";

  var apSummaryTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 9);
  if(apSummaryTableRepeatInfo.rows !=0 ){
     xui_JSPagination(apSummaryTableRepeatInfo,outputDIV, '2_1');
  }*/
}

function ManagedAPSummary_LoadEnd()
{
  loadme();
}


/*function reDrawPage(data)
{
     xui_JSPagination(data,"plotwlanSummaryTable", '2_1');
}

function sortTable(link)
{
    switch (link.firstChild.nodeValue) {
    case "MAC Address<br>(*)-Peer Managed" :
        globalData.cells.sort(sortByMAC);
        break;
    case "Location":
        globalData.cells.sort(sortByLoc);
        break;
    case "IP Address":
        globalData.cells.sort(sortByIP);
        break;
    case "Profile":
        globalData.cells.sort(sortByProfile);
        break;
    case "Software Version":
        globalData.cells.sort(sortByswVersion);
        break;
    case "Status":
        globalData.cells.sort(sortByStatus);
        break;
    case "Configuration Status":
 		globalData.cells.sort(sortByCfgStatus);
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

function sortByLoc(a, b)
{
    var sa = a[2].value.toLowerCase();
    var sb = b[2].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByIP(a, b)
{
    var sa = a[3].value.toLowerCase();
    var sb = b[3].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByProfile(a, b)
{
    var sa = a[4].value.toLowerCase();
    var sb = b[4].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByswVersion(a, b)
{
    var sa = a[5].value.toLowerCase();
    var sb = b[5].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByStatus(a, b)
{
    var sa = a[6].value.toLowerCase();
    var sb = b[6].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByCfgStatus(a, b)
{
    var sa = a[7].value.toLowerCase();
    var sb = b[7].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByAge(a, b)
{
    var sa = a[8].value.toLowerCase();
    var sb = b[8].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}
*/
