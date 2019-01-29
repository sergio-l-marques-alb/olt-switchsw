
function formSubmit_MstPage(submitFlag, thisControl)
{
}

function preLoad_MstPage()
{
  var form = document.forms[0];
  formInputs = form.getElementsByTagName("input");
  if(!formInputs) return;

  var mstObj = undefined;

  for(var i = 0; i < formInputs.length; i++)
  {
    if(formInputs[i].name == "SpanningTreeMSTIntfConfig_MstInstance_G1N1E1")
    {
      mstObj = formInputs[i];
      break;
    }
  }

  if(mstObj.value != "0")
    return;

  for(var i = 0; i < formInputs.length; i++)
  {
    if(brcm_strip_iter_prefix(formInputs[i].name) == "SpanningTreeMSTIntfConfig_VlanParticipation_G2N1E2")
    {
      var obj = formInputs[i].nextSibling;
      obj.disabled = true;
    }
  }
}
