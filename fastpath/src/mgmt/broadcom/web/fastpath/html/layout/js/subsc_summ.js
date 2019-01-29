function subsc_summ_SubmitStart(submitFlag)
{
}

function subsc_summ_SubmitEnd(submitFlag)
{
}

function subsc_summ_LoadStart(arg)
{
 transformValues("1_1_6");
}

function subsc_summ_LoadEnd(arg)
{
}

function transformValues(xid) {

    var form = document.forms[0];
    inputs = form.getElementsByTagName("input");
    if(!inputs) return;

    for( var z=0; z < inputs.length; z++)
    {
      var name = inputs.item(z).name;
      if(name.indexOf(xid) != -1)
      {
        var val = inputs.item(z).value;
        if (val)
          inputs.item(z).value = val.replace(/\n/g,"<BR>");
      }
    }
}

