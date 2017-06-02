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
 var element_1_9_1 = getInputElementByXid('1_9_1');
 var element_3_1_1 = getInputElementByXid('3_1_1');
 var element_3_1_3 = getInputElementByXid('3_1_3');

 element_1_3_1.disabled = true;
 element_1_9_1.disabled = true;
 if(element_3_1_1)
   element_3_1_1.disabled = true;
 if(element_3_1_3)
 element_3_1_3.disabled = true;

 element_1_3_1.nextSibling.disabled = true;
 element_1_9_1.nextSibling.disabled = true;
 if(element_3_1_1 && element_3_1_1.nextSibling)
   element_3_1_1.nextSibling.disabled = true;
 if(element_3_1_3 && element_3_1_3.nextSibling)
 {
   element_3_1_3.nextSibling.disabled = true;
   element_3_1_3.nextSibling.value = "Active";
   
 }

}

function UserMgrIntAuthServUserDBConfig_SubmitStart(submitFlag)
{
 var element_1_3_1 = getInputElementByXid('1_3_1');
 var element_1_9_1 = getInputElementByXid('1_9_1');
 var element_1_11_1 = getInputElementByXid('1_11_1');

 var min_len = 0;

 if(element_1_11_1 != null)
 {
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
   
   var len1 = element_1_9_1.nextSibling.value.length;
   
   if(len1 > 64)
   {
        alert("Error! Password must be between "+min_len+" and 64 characters.");
        disable_all_elements(submitFlag);
        return false;
   }
 }
 else
 {
   return;
 }
}

function UserMgrIntAuthServUserDBConfig_SubmitEnd(submitFlag)
{
}

function UserMgrIntAuthServUserDBConfig_LoadStart(arg)
{
}

function UserMgrIntAuthServUserDBConfig_LoadEnd(arg)
{ 
}
