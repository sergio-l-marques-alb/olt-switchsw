function service_summ_SubmitStart(submitFlag)
{
}

function service_summ_SubmitEnd(submitFlag)
{
}

function service_summ_LoadStart(arg)
{
 transformValues("1_1_4");
 transformValues("1_1_5");
}

function service_summ_LoadEnd(arg)
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

