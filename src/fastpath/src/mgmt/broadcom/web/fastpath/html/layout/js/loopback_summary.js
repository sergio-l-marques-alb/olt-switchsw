function loopback_summary_SubmitStart(submitFlag)
{
}
function loopback_summary_SubmitEnd(submitFlag)
{
}


function loopback_summary_LoadStart(arg)
{
  var tblObj = document.getElementsByTagName('table');
 
  for (var i=0; i<tblObj.length; i++)
  {
    if (tblObj[i].id == '1_1')
    {
      /* Ignore first row as it contains table headers */
      var prev_mode="";
      for (var j=1; j<tblObj[i].rows.length; j++)
      {
        var mode = tblObj[i].rows[j].childNodes[1].childNodes[0].value;
        if (mode == prev_mode)
          tblObj[i].rows[j].childNodes[1].childNodes[0].value = '';
        else
         prev_mode = mode;
      }
    }
  }
}



function loopback_summary_LoadEnd(arg) {
}
