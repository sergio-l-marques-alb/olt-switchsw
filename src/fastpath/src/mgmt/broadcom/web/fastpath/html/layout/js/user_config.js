function getInputElementByXid(xid)
{

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}
function disable_all_elements(submitFlag)
{
 var element_1_3_1 = getInputElementByXid('1_3_1');
 var element_2_6_1 = getInputElementByXid('2_6_1');
 var element_1_9_1 = getInputElementByXid('1_9_1');
 var element_2_2_1 = getInputElementByXid('2_2_1');
 var element_2_3_1 = getInputElementByXid('2_3_1');
 var element_2_4_1 = getInputElementByXid('2_4_1');
 var element_3_1_1 = getInputElementByXid('3_1_1');
 var element_3_1_3 = getInputElementByXid('3_1_3');

 element_1_3_1.disabled = true;
 element_2_6_1.disabled = true;
 element_1_9_1.disabled = true;
 element_2_2_1.disabled = true;
 element_2_3_1.disabled = true;
 element_2_4_1.disabled = true;
 if(element_3_1_1)
   element_3_1_1.disabled = true;
 if(element_3_1_3)
 element_3_1_3.disabled = true;

 element_1_3_1.nextSibling.disabled = true;
 element_2_6_1.nextSibling.disabled = true;
 element_1_9_1.nextSibling.disabled = true;
 element_2_2_1.nextSibling.disabled = true;
 element_2_3_1.nextSibling.disabled = true;
 element_2_4_1.nextSibling.disabled = true;
 if(element_3_1_1 && element_3_1_1.nextSibling)
   element_3_1_1.nextSibling.disabled = true;
 if(element_3_1_3 && element_3_1_3.nextSibling)
 {
   element_3_1_3.nextSibling.disabled = true;
   element_3_1_3.nextSibling.value = "Active";
   
 }

}

function user_config_SubmitStart(submitFlag)
{
 var element_1_3_1 = getInputElementByXid('1_3_1');
 var element_1_8_1 = getInputElementByXid('1_8_1');
 var element_1_4_1 = getInputElementByXid('1_4_1');
 var element_2_6_1 = getInputElementByXid('2_6_1');
 var element_1_9_1 = getInputElementByXid('1_9_1');
 var element_2_2_1 = getInputElementByXid('2_2_1');
 var element_2_3_1 = getInputElementByXid('2_3_1');
 var element_2_4_1 = getInputElementByXid('2_4_1');
 var element_1_10_1 = getInputElementByXid('1_10_1');
 var element_1_11_1 = getInputElementByXid('1_11_1');

 var min_len = element_1_10_1.value;

  if(element_1_11_1.disabled == true)
  {
    return false;
  }

  if(element_1_3_1.nextSibling.value.length == 0)
  {
    alert("Error! Username must be entered.");
    disable_all_elements(submitFlag);
    return false;
  }
  if(element_1_3_1.nextSibling.value.length > 64)
  {
    alert("Error! Username can not exceed more than 64 characters");
    disable_all_elements(submitFlag);
    return false;
  }
  if( element_1_8_1.nextSibling.value != element_1_4_1.nextSibling.value)
  {
    alert("Error! Password and Confirm Password must be identical!");
    disable_all_elements(submitFlag);
    return false;
  }

  var len1 = element_1_8_1.nextSibling.value.length;
  var len2 = element_2_4_1.nextSibling.value.length;
  if(element_2_2_1.nextSibling.options &&
     element_2_2_1.nextSibling.options[element_2_2_1.nextSibling.selectedIndex].innerHTML != "None")
  {
  if(len1 == 0)
  {
    alert("Error! Password can not be empty if Authentication is Enabled");
    disable_all_elements(submitFlag);
    return false;
  }
  }
  if(element_2_3_1.nextSibling.disabled == false &&
     element_2_3_1.nextSibling.options &&
     element_2_3_1.nextSibling.options[element_2_3_1.nextSibling.selectedIndex].innerHTML != "None")
  {
    if(element_2_2_1.nextSibling.options &&
       element_2_2_1.nextSibling.options[element_2_2_1.nextSibling.selectedIndex].innerHTML == "None")
    {
      alert("Error!  Authentication is required for this user since encryption is enabled.");
      disable_all_elements(submitFlag);
      return false;
    }
    if(len2 < 8)
    {
       alert("Error! Encryption Key Should be atleast 8 Characters in length");
       disable_all_elements(submitFlag);
       return false;
    }
  }
  if((len1 >= 0) && ((len1 < min_len)||(len1 > 64)))
  {
       alert("Error! Password must be between "+min_len+" and 64 characters.");
       disable_all_elements(submitFlag);
       return false;
  }
  element_2_6_1.nextSibling.value = element_1_8_1.nextSibling.value;
  element_1_9_1.nextSibling.value = element_1_8_1.nextSibling.value;
}

function user_config_SubmitEnd(submitFlag)
{
}

function user_config_LoadStart(arg)
{
 var element_2_5_1 = getInputElementByXid('2_5_1');
 element_2_5_1.value="False";
}

function user_config_LoadEnd(arg)
{ 
 var element_1_4_1 = getInputElementByXid('1_4_1');
 var element_1_8_1 = getInputElementByXid('1_8_1');
 var element_1_10_1 = getInputElementByXid('1_10_1');
 var min_len = element_1_10_1.value;
 var help_passwd = "   ("+min_len+" to 64 Characters)";
 element_1_8_1.nextSibling.nextSibling.data = help_passwd;
 element_1_4_1.nextSibling.nextSibling.data = help_passwd;
 element_1_4_1.nextSibling.value = "";
 element_1_8_1.nextSibling.value = "";
}
