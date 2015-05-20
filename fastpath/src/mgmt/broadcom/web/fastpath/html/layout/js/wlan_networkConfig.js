
function wep_auth_open_select()
{
  var element_1_240_1 = getInputElementByXid('1_240_1');
  if(element_1_240_1 == null) return;

  var element_1_240_2 = getInputElementByXid('1_240_2');
  if(element_1_240_2 == null) return;

  if ((element_1_240_1.nextSibling.checked == false) &&
      (element_1_240_2.nextSibling.checked == false))
  {
    if (element_1_240_1.nextSibling.checked == false)
      element_1_240_2.nextSibling.checked = true;
  }

  var element_1_24_1 = getInputElementByXid('1_24_1');
  if(element_1_24_1 == null) return;

  if ((element_1_240_1.nextSibling.checked == true) &&
      (element_1_240_2.nextSibling.checked == false))
  {
    element_1_24_1.nextSibling.value = "Open";
  }
  if ((element_1_240_1.nextSibling.checked == false) &&
      (element_1_240_2.nextSibling.checked == true))
  {
    element_1_24_1.nextSibling.value = "Shared Key";
  }
  if ((element_1_240_1.nextSibling.checked == true) &&
      (element_1_240_2.nextSibling.checked == true))
  {
    element_1_24_1.nextSibling.value = "Open and Shared";
  }
}

function wep_auth_shared_select()
{
  var element_1_240_1 = getInputElementByXid('1_240_1');
  if(element_1_240_1 == null) return;

  var element_1_240_2 = getInputElementByXid('1_240_2');
  if(element_1_240_2 == null) return;

  if ((element_1_240_1.nextSibling.checked == false) &&
      (element_1_240_2.nextSibling.checked == false))
  {
    if (element_1_240_2.nextSibling.checked == false)
      element_1_240_1.nextSibling.checked = true;
  }

  var element_1_24_1 = getInputElementByXid('1_24_1');
  if(element_1_24_1 == null) return;

  if ((element_1_240_1.nextSibling.checked == true) &&
      (element_1_240_2.nextSibling.checked == false))
  {
    element_1_24_1.nextSibling.value = "Open";
  }
  if ((element_1_240_1.nextSibling.checked == false) &&
      (element_1_240_2.nextSibling.checked == true))
  {
    element_1_24_1.nextSibling.value = "Shared Key";
  }
  if ((element_1_240_1.nextSibling.checked == true) &&
      (element_1_240_2.nextSibling.checked == true))
  {
    element_1_24_1.nextSibling.value = "Open and Shared";
  }

}

function wpa_type_wpa_select()
{
  var element_1_320_1 = getInputElementByXid('1_320_1');
  if(element_1_320_1 == null) return;

  var element_1_320_2 = getInputElementByXid('1_320_2');
  if(element_1_320_2 == null) return;

  if ((element_1_320_1.nextSibling.checked == false) &&
      (element_1_320_2.nextSibling.checked == false))
  {
    if (element_1_320_1.nextSibling.checked == false)
      element_1_320_2.nextSibling.checked = true;
  }

  var element = getInputElementByXid('1_32_1');
  if(element == null) return;

  if ((element_1_320_1.nextSibling.checked == true) &&
      (element_1_320_2.nextSibling.checked == false))
  {
    element.nextSibling.value = "WPA";
  }
  if ((element_1_320_1.nextSibling.checked == false) &&
      (element_1_320_2.nextSibling.checked == true))
  {
    element.nextSibling.value = "WPA2";
  }
  if ((element_1_320_1.nextSibling.checked == true) &&
      (element_1_320_2.nextSibling.checked == true))
  {
    element.nextSibling.value = "WPA_WPA2";
  }

}

function wpa_type_wpa2_select()
{
 var element_1_320_1 = getInputElementByXid('1_320_1');
  if(element_1_320_1 == null) return;

  var element_1_320_2 = getInputElementByXid('1_320_2');
  if(element_1_320_2 == null) return;

  if ((element_1_320_1.nextSibling.checked == false) &&
      (element_1_320_2.nextSibling.checked == false))
  {
    if (element_1_320_2.nextSibling.checked == false)
      element_1_320_1.nextSibling.checked = true;
  }

  var wpa_preauth = getInputElementByXid('1_36_1');
  if(wpa_preauth == null) return;

  var wpa_preauth_limit = getInputElementByXid('1_37_1');
  if(wpa_preauth_limit == null) return;

  if (element_1_320_2.nextSibling.checked == true)
  {
    wpa_preauth.nextSibling.disabled = false;
    if (wpa_preauth.nextSibling.checked == true)
    {
      wpa_preauth_limit.nextSibling.disabled = false;
    }
    else
    {
      wpa_preauth_limit.nextSibling.disabled = true;
    }
  }
  else
  {
    wpa_preauth.nextSibling.disabled = true;
    wpa_preauth_limit.nextSibling.disabled = true;
  }

  var element = getInputElementByXid('1_32_1');
  if(element == null) return;

  if ((element_1_320_1.nextSibling.checked == true) &&
      (element_1_320_2.nextSibling.checked == false))
  {
    element.nextSibling.value = "WPA";
    wpa_preauth_limit.nextSibling.disabled = true;
  }
  if ((element_1_320_1.nextSibling.checked == false) &&
      (element_1_320_2.nextSibling.checked == true))
  {
    element.nextSibling.value = "WPA2";
    wpa_preauth.nextSibling.disabled = false;
  }
  if ((element_1_320_1.nextSibling.checked == true) &&
      (element_1_320_2.nextSibling.checked == true))
  {
    element.nextSibling.value = "WPA_WPA2";
    wpa_preauth.nextSibling.disabled = false;
  }

}

function wpa_algo_ccmp_select()
{
  var element_1_330_1 = getInputElementByXid('1_330_1');
  if(element_1_330_1 == null) return;

  var element_1_330_2 = getInputElementByXid('1_330_2');
  if(element_1_330_2 == null) return;

  if ((element_1_330_1.nextSibling.checked == false) &&
      (element_1_330_2.nextSibling.checked == false))
  {
    if (element_1_330_2.nextSibling.checked == false)
      element_1_330_1.nextSibling.checked = true;
  }

  var element_1_33_1 = getInputElementByXid('1_33_1');
  if(element_1_33_1 == null) return;

  if ((element_1_330_1.nextSibling.checked == true) &&
      (element_1_330_2.nextSibling.checked == false))
  {
    element_1_33_1.nextSibling.value = "TKIP";
  }
  if ((element_1_330_1.nextSibling.checked == false) &&
      (element_1_330_2.nextSibling.checked == true))
  {
    element_1_33_1.nextSibling.value = "CCMP";
  }
  if ((element_1_330_1.nextSibling.checked == true) &&
      (element_1_330_2.nextSibling.checked == true))
  {
    element_1_33_1.nextSibling.value = "TKIP_CCMP";
  }

}

function wpa_algo_tkip_select()
{
  var element_1_330_1 = getInputElementByXid('1_330_1');
  if(element_1_330_1 == null) return;

  var element_1_330_2 = getInputElementByXid('1_330_2');
  if(element_1_330_2 == null) return;

  if ((element_1_330_1.nextSibling.checked == false) &&
      (element_1_330_2.nextSibling.checked == false))
  {
    if (element_1_330_1.nextSibling.checked == false)
      element_1_330_2.nextSibling.checked = true;
  }

  var element_1_33_1 = getInputElementByXid('1_33_1');
  if(element_1_33_1 == null) return;

  if ((element_1_330_1.nextSibling.checked == true) &&
      (element_1_330_2.nextSibling.checked == false))
  {
    element_1_33_1.nextSibling.value = "TKIP";
  }
  if ((element_1_330_1.nextSibling.checked == false) &&
      (element_1_330_2.nextSibling.checked == true))
  {
    element_1_33_1.nextSibling.value = "CCMP";
  }
  if ((element_1_330_1.nextSibling.checked == true) &&
      (element_1_330_2.nextSibling.checked == true))
  {
    element_1_33_1.nextSibling.value = "TKIP_CCMP";
  }

}

function calculate_chars_required()
{
  var element_1_25_1 = getInputElementByXid('1_25_1');
  if(element_1_25_1 != null)
  {
    var element_1_26_1 = getInputElementByXid('1_26_1');
    if(element_1_26_1 != null)
    { 
      if(element_1_25_1.checked == true)
      {
        key_type = element_1_25_1.value;
      }
      if(element_1_25_1.checked == false)
      {
        key_type = element_1_25_1.nextSibling.value;
      }
      if(element_1_25_1.nextSibling.checked == true)
      {
        key_type = element_1_25_1.nextSibling.value;
      }
      if(element_1_25_1.nextSibling.checked == false)
      {
        key_type = element_1_25_1.value;
      }

      if(element_1_26_1.checked == true)
      {
        key_len = element_1_26_1.value;
      }
      if(element_1_26_1.checked == false)
      {
        key_len = element_1_26_1.nextSibling.value;
      }
      if(element_1_26_1.nextSibling.checked == true)
      {
        key_len = element_1_26_1.nextSibling.value;
      }
      if(element_1_26_1.nextSibling.checked == false)
      {
        key_len = element_1_26_1.value;
      }
    }
  }
  if(element_1_25_1 == null)
  {
    var element_2_25_1 = getInputElementByXid('2_25_1');
    var element_2_26_1 = getInputElementByXid('2_26_1');
    if(element_2_26_1 != null)
    {
      if(element_2_25_1.checked == true)
      {
        key_type = element_2_25_1.value;
      }
      if(element_2_25_1.checked == false)
      {
        key_type = element_2_25_1.nextSibling.value;
      }
      if(element_2_25_1.nextSibling.checked == true)
      {
        key_type = element_2_25_1.nextSibling.value;
      }
      if(element_2_25_1.nextSibling.checked == false)
      {
        key_type = element_2_25_1.value;
      }

      if(element_2_26_1.checked == true)
      {
        key_len = element_2_26_1.value;
      }
      if(element_2_26_1.checked == false)
      {
        key_len = element_2_26_1.nextSibling.value;
      }
      if(element_2_26_1.nextSibling.checked == true)
      {
        key_len = element_2_26_1.nextSibling.value;
      }
      if(element_2_26_1.nextSibling.checked == false)
      {
        key_len = element_2_26_1.value;
      }
    }
  }
  var val;
  if(key_type == "ASCII" && key_len == "64")
  {
    val = 5;
  }
  if(key_type == "ASCII" && key_len == "128")
  {
    val = 13;
  }
  if(key_type == "HEX" && key_len == "64")
  {
    val = 10;
  }
  if(key_type == "HEX" && key_len == "128")
  {
    val = 26;
  }
  return val;
}

function set_chars_required()
{
  var val = calculate_chars_required();
  var element_1_27_2 = getInputElementByXid('1_27_2');
  if(element_1_27_2 != null) 
  {
    element_1_27_2.nextSibling.nodeValue = "Tx (Characters required: " + val + ")";
  }
  if(element_1_27_2 == null)
  {
    var element_2_27_2 = getInputElementByXid('2_27_2');
    element_2_27_2.nextSibling.nodeValue = "Tx (Characters required: " + val + ")";
  }

  var element_1_28_2 = getInputElementByXid('1_28_2');
  if(element_1_28_2 == null) return;
  element_1_28_2.nextSibling.maxLength = val;

  var element_1_29_2 = getInputElementByXid('1_29_2');
  if(element_1_29_2 == null) return;
  element_1_29_2.nextSibling.maxLength = val;

  var element_1_30_2 = getInputElementByXid('1_30_2');
  if(element_1_30_2 == null) return;
  element_1_30_2.nextSibling.maxLength = val;

  var element_1_31_2 = getInputElementByXid('1_31_2');
  if(element_1_31_2 == null) return;
  element_1_31_2.nextSibling.maxLength = val;
}

function set_wep_keys()
{
  var wep_key1_select = getInputElementByXid('1_28_1');
  if (wep_key1_select == null) return;
  var wep_key1_txt = getInputElementByXid('1_28_2');
  if (wep_key1_txt == null) return;

  var wep_key2_select = getInputElementByXid('1_29_1');
  if (wep_key2_select == null) return;
  var wep_key2_txt = getInputElementByXid('1_29_2');
  if (wep_key2_txt == null) return;

  var wep_key3_select = getInputElementByXid('1_30_1');
  if (wep_key3_select == null) return;
  var wep_key3_txt = getInputElementByXid('1_30_2');
  if (wep_key3_txt == null) return;

  var wep_key4_select = getInputElementByXid('1_31_1');
  if (wep_key4_select == null) return;
  var wep_key4_txt = getInputElementByXid('1_31_2');
  if (wep_key4_txt == null) return;

  if(wep_key1_select.nextSibling.disabled == false)
  {
  if(wep_key1_select.nextSibling.checked == true)
  {
    if(wep_key1_txt.nextSibling.value == "")
    {
      alert("ERROR: The WEP Key value is incorrect. Please enter a valid WEP Key and re-submit. ");
    } 
  }
  }
  if(wep_key2_select.nextSibling.disabled == false)
  {
  if(wep_key2_select.nextSibling.checked == true)
  {
    if(wep_key2_txt.nextSibling.value == "")
    {
      alert("ERROR: The WEP Key value is incorrect. Please enter a valid WEP Key and re-submit. ");
    }
  }
  }
  if(wep_key3_select.nextSibling.disabled == false)
  {
  if(wep_key3_select.nextSibling.checked == true)
  {
    if(wep_key3_txt.nextSibling.value == "")
    {
      alert("ERROR: The WEP Key value is incorrect. Please enter a valid WEP Key and re-submit. ");
    }
  }
  }
  if(wep_key4_select.nextSibling.disabled == false)
  {
  if(wep_key4_select.nextSibling.checked == true)
  {
    if(wep_key4_txt.nextSibling.value == "")
    {
      alert("ERROR: The WEP Key value is incorrect. Please enter a valid WEP Key and re-submit. ");
    }
  }
  }
}

function wlan_networkConfig_SubmitStart(submitFlag)
{
 set_wep_keys();
}
function wlan_networkConfig_SubmitEnd(submitFlag)
{
}

function wlan_networkConfig_LoadStart(arg)
{
}

function wlan_networkConfig_LoadEnd(arg)
{
  intIfSplit();
}

function my_xuiGetElementsByClass( searchClass, domNode, tagName)
{
  if (domNode == null) domNode = document;
  if (tagName == null) tagName = '*';
  var el = new Array();
  var tags = domNode.getElementsByTagName(tagName);
  var tcl = " "+searchClass+" ";
  for(i=0,j=0; i<tags.length; i++) {
    var test = " " + tags[i].className + " ";
    if (test.indexOf(tcl) != -1)
      el[j++] = tags[i];
  }
  return el;
}

function intIfSplit()
{
    var allTRs = my_xuiGetElementsByClass('intfSplitType1',null,null);
    if( allTRs.length <= 0 ) { return;}
    for ( var i=0; i<allTRs.length; i++)
    {
       var allTDs = allTRs[i].getElementsByTagName('TD');
       var globalNewTD = document.createElement("td");
       globalNewTD.setAttribute('width',"1%");
       globalNewTD.setAttribute('class',"intfSplitType1right");
       globalNewTD.setAttribute('className',"intfSplitType1right"); 
/*globalNewTD.style.cssText = 'background-color:#F0F0F0;';*/
       var newTable = document.createElement("table");
       newTable.setAttribute('width',"1%");
       newTable.setAttribute('cellSpacing',"0");
       newTable.setAttribute('cellPadding',"0");
       newTable.border = 0;
       var newTBody = document.createElement("TBODY")
       var newTR = document.createElement("tr");

       k = allTDs.length-1;
       var colSpanNum = k -1;
       for(var z=1; z<=k; z++)
       {
          newTR.appendChild(allTDs[1],null);
       }
       newTable.appendChild(newTBody);
       newTBody.appendChild(newTR);
       globalNewTD.appendChild(newTable);
       allTRs[i].appendChild(globalNewTD);
       var blankTD = document.createElement("td");
       blankTD.setAttribute('colSpan',colSpanNum);
       blankTD.innerHTML = "&nbsp;"
       allTRs[i].appendChild(blankTD);
    }
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
