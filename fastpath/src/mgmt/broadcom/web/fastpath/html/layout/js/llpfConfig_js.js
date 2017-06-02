xuiIncludeJSFile("/scripts/globalStyleJS.js");
var protocol_g = new Array();
var status_g = new Array();

function llpfConfig_js_SubmitStart(){
   globalStyleJS_SubmitStart();
}
function llpfConfig_js_LoadStart(){
   globalStyleJS_LoadStart();
}
function llpfConfig_js_LoadEnd()
{
   globalStyleJS_LoadEnd();
}

function myGetElements(protocol,status)
{
  var form = document.forms[0];
  inputs = form.getElementsByTagName("input");
  if(!inputs) return;

  for( var z=0; z < inputs.length; z++)
  {
    var name = inputs.item(z).name;
    if(name.indexOf(protocol) != -1) protocol_g[protocol_g.length] = inputs.item(z);
    else if(name.indexOf(status) != -1) status_g[status_g.length] = inputs.item(z);
  }
}

function llpfConfig_js_SubmitEnd()
{
  globalStyleJS_SubmitEnd();

  myGetElements('v_4_1_1','v_4_1_2'); 
  var message = "";

  if (document.forms[0].submit_flag.value != 1)
  {
    for(var i=0; i<protocol_g.length;i++)
    {
      if(status_g[i].value != 'FALSE')
      {
        if (protocol_g[i].value != 'Blockall')
        {
          var value = protocol_g[i].value.toLowerCase();
          message = message + "Warning:Configuring block" + value + " will block the " + protocol_g[i].value + " PDUs" + " from being forwarded\n";
        }
        else
        {
          message = message + "Warning:Configuring Blockall will block the " + "PDUs" + " (with a DMAC of 01:00:00:0C:CC:CX) from being forwarded\n";
        }
     }
    }
    if( message != "")
    {
      alert(message);
    }
    return true;
  }
}
