function l2pt_summ_SubmitStart(submitFlag)
{
}

function l2pt_summ_SubmitEnd(submitFlag)
{
}

function l2pt_summ_LoadStart(arg)
{
 transformValues("1_1_3");
}

function l2pt_summ_LoadEnd(arg)
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
        if( val == "0x0")
        {
          inputs.item(z).value  = "Match All";
        }
      }
    }
}

