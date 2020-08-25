xuiIncludeJSFile("/scripts/global.js");
function APProvisioningSummary_SubmitStart()
{
}


function APProvisioningSummary_SubmitEnd()
{
}

function APProvisioningSummary_LoadStart()
{
  xeData.nTabInfo = [ ["Summary","ap_provisioning_summ.html"], ["Detail","APProvisioningDetail.html"] ]

/*  var inputDIV = "pagination_summary";
  var outputDIV = "plotpagination_summary";

  var apSummaryTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 7);
  if(apSummaryTableRepeatInfo.rows !=0 ){
     xui_JSPagination(apSummaryTableRepeatInfo,outputDIV, '2_1');
  }*/

}

function APProvisioningSummary_LoadEnd()
{
  loadme();
}


/*function reDrawPage(data)
{
     xui_JSPagination(data,"plotpagination_summary", '2_1');
}

function sortTable(link)
{
    switch (link.firstChild.nodeValue) {
    case "MAC Address" :
        globalData.cells.sort(sortByMAC);
        break;
    case "IP Address":
        globalData.cells.sort(sortByIP);
        break;
    case "Primary IP Address":
        globalData.cells.sort(sortByPriIP);
        break;
    case "Backup IP Address":
        globalData.cells.sort(sortByBkpIP);
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

function sortByIP(a, b)
{
    var sa = a[2].value.toLowerCase();
    var sb = b[2].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByPriIP(a, b)
{
    var sa = a[3].value.toLowerCase();
    var sb = b[3].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByBkpIP(a, b)
{
    var sa = a[4].value.toLowerCase();
    var sb = b[4].value.toLowerCase();
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
}*/

