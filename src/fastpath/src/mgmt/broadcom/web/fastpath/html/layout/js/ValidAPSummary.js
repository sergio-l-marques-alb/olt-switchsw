function ValidAPSummary_SubmitStart()
{
}


function ValidAPSummary_SubmitEnd()
{
}

function ValidAPSummary_LoadStart()
{
  var inputDIV = "pagination_summary";
  var outputDIV = "plotpagination_summary";

  var apSummaryTableRepeatInfo = xuiRepeatInfoReadByCount(inputDIV, 5);
  if(apSummaryTableRepeatInfo.rows !=0 ){
     xui_JSPagination(apSummaryTableRepeatInfo,outputDIV, '2_1');
  }

}

function ValidAPSummary_LoadEnd()
{
}


function reDrawPage(data)
{
     xui_JSPagination(data,"plotpagination_summary", '2_1');
}

function sortTable(link)
{
    switch (link.firstChild.nodeValue) 
    {
    case "MAC Address" :
        globalData.cells.sort(sortByMAC);
        break;
    case "Location" :
        globalData.cells.sort(sortByLoc);
        break;
    case "Profile" :
        globalData.cells.sort(sortByProfile);
        break;
    case "AP Mode" :
        globalData.cells.sort(sortByStatus);
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

function sortByProfile(a, b)
{
    var sa = a[4].value.toLowerCase();
    var sb = b[4].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

function sortByStatus(a, b)
{
    var sa = a[3].value.toLowerCase();
    var sb = b[3].value.toLowerCase();
    return ((sa < sb) ? -1 : ((sa > sb) ? 1 : 0));
}

