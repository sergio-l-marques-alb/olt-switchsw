function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function acl_summ_SubmitStart(submitFlag)
{
}

function acl_summ_SubmitEnd(submitFlag)
{
}

function acl_summ_LoadStart(arg)
{
  transformValues("1_1_5");
}

function acl_summ_LoadEnd(arg)
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
        if( val == 'Out Bound')
        {
	  if (z >= 10)
	  {
            if (inputs.item(z-10).value == inputs.item(z-3).value)
            {
              inputs.item(z-1).value = '';
              inputs.item(z-3).value = '';
            }
	  }
        }
      }
    }
}
