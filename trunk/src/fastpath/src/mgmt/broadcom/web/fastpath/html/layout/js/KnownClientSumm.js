function KnownClientSumm_SubmitStart()
{
}


function KnownClientSumm_SubmitEnd()
{
}

function KnownClientSumm_LoadStart()
{
  var inputDIV = "wlanSummaryTable";
  var outputDIV = "plotwlanSummaryTable";

  var kcSummaryTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 3);
  if(kcSummaryTableRepeatInfo.rows !=0 ){
     xui_JSPagination(kcSummaryTableRepeatInfo,outputDIV, '2_1');
  }
}

function KnownClientSumm_LoadEnd()
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
    case "Name":
        globalData.cells.sort(sortByName);
        break;
    case "Authentication Action":
        globalData.cells.sort(sortByAction);
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

function sortByName(a, b)
{
    var sa = a[2].value.toLowerCase();
    var sb = b[2].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByAction(a, b)
{
    var sa = a[3].value.toLowerCase();
    var sb = b[3].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}
