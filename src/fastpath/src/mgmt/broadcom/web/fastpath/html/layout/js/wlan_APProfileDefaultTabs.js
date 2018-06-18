xuiIncludeJSFile("/scripts/global.js");
xuiIncludeJSFile("/scripts/wlan_APProfileTabsList.js");
var glbCheckVariable="";
function wlan_APProfileDefaultTabs_SubmitStart(submitFlag)
{
}
function wlan_APProfileDefaultTabs_SubmitEnd(submitFlag)
{
}

function wlan_APProfileDefaultTabs_LoadStart(arg)
{
/*tabsList= [ ["Summary","profile_summ.html"], ["Default",["Global","profile_management_cfg_rw.html?advanced=1&profile=1"],["Radio","profile_radio_cfg_rw.html?advanced=1&profile=1"],["VAP","profile_vap_summ.html?advanced=1&profile=1"], ["QoS","APProfileQoSConfiguration.html"],["TSPEC","APProfileRadioTspecConfiguration.html"]] ];*/
  xeData.nTabInfo = tabsList;
}

function wlan_APProfileDefaultTabs_LoadEnd(arg)
{
 /* var additionalTab = [["Global","APProfileGlobalConfiguration.html"],["Radio","APProfileRadioConfiguration.html"],["VAP","APProfileVAPConfiguration.html"], ["QoS","APProfileQoSConfiguration.html"],["TSPEC","APProfileRadioTspecConfiguration.html"]];*/

  var additionalTab = [["Global","profile_management_cfg_rw.html"],["Radio","profile_radio_cfg_rw.html"],["VAP","profile_vap_summ.html"], ["QoS","APProfileQoSConfiguration.html"],["TSPEC","APProfileRadioTspecConfiguration.html"]];
  var range_of_values = xui_oem_getAllWebEnums("1_1_1");
  glbCheckVariable = getInputElementByXid("1_1_1").value;
 if ( (glbCheckVariable == "1-Default") || (glbCheckVariable == "1"))
     glbCheckVariable="";

  for ( var i=1; i < range_of_values.length; i++ )
  {
     var test = new Array();
     test[test.length] = truncate(range_of_values[i],9);
     var tempArray=new Array();
     for ( var j=0; j < additionalTab.length; j++)
     {
       tempArray[j]=additionalTab[j].slice();
       var tempString = tempArray[j][1];
       if((tempString == "APProfileQoSConfiguration.html") || (tempString == "APProfileRadioTspecConfiguration.html")) 
       {
         tempArray[j][1]= tempString+"?v_1_1_1="+range_of_values[i]+"&submit_target="+tempString+"&submit_flag=8";
       }
       else
       {
         var myArray = range_of_values[i].split("-");
         tempArray[j][1]= tempString+"?advanced=1&profile="+myArray[0]; 
       }
     }
     for (var j=0;j< tempArray.length; j++)
     {
         test[test.length]=tempArray[j].slice();
     }
     xeData.nTabInfo[xeData.nTabInfo.length] = test.slice();
  }
  loadme();
}

function xui_oem_getAllWebEnums(xid)
{
  var values = getAllWebEnums(xid) ;
  if(values != null) return values;
  xeData["xe_" + xid] = "v_" + xid;
  var xe = xeData["xe_" + xid];
  if(xe == null) return null;
  var xe_xid = xe + "_" + xid;
  allWebEnums[xe_xid]= new Array();
  return  allWebEnums[xe_xid];
}

function getInputElementByXid(xid)
{
    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++)
    {
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
           element.getAttribute('xid') != xid) continue;

        return element;
}
}

