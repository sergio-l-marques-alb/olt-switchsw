function ospf_route_summ_SubmitStart(submitFlag)
{
}
function ospf_route_summ_SubmitEnd(submitFlag)
{
}

function ospf_route_summ_LoadStart(arg)
{
  var tblObj = document.getElementsByTagName('table');

  for (var i=0; i<tblObj.length; i++)
  {
    if (tblObj[i].id == '1_1')
    {
      /* Ignore first row as it contains table headers */
      for (var j=1; j<tblObj[i].rows.length; j++)
      {
        var mode = tblObj[i].rows[j].childNodes[13].childNodes[0].value;
//alert("Mode = "+mode);
        if (mode == "Disable")
          tblObj[i].rows[j].style.display = 'none';
        else
         tblObj[i].rows[j].style.display = '';
      }
    }
  }
}

function ospf_route_summ_LoadEnd(arg)
{
}


function getTableElementByXid(xid)
{
    var all_elements = document.getElementsByTagName('table') ;

    for(var index=0;index<all_elements.length;index++)
    {
        var element = all_elements[index];
        if(!element.hasAttribute('xid') ||
            element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }


}

