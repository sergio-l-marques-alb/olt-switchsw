function AuthenticationListConfig_LoadEnd(arg){
}

function AuthenticationListConfig_LoadStart(arg){    
}

function getInputElementsByXid(xid)
{
  var all_elements = document.getElementsByTagName('input');
  var result_array = new Array();
  
  for (var index = 0; index < all_elements.length; index++)
  {
    var element = all_elements[index];
    if (element.getAttribute('xid') == null ||
        element.getAttribute('xid') != xid) continue;
        result_array.push(element);
  }
  return result_array;
}

function AuthenticationListConfig_SubmitEnd(submitFlag){
}
      
function AuthenticationListConfig_SubmitStart(submitFlag){
}
      
function authentication_page_refresh(args)
{
  var xid = args.split("|");
    
  for (var k = 0; k < (xid.length-1); k++)
  {
    var j = k+1;
      
    for (var i = 0; i < j; i++)
    {
      var method1 = getInputElementByXid(xid[i]);
      var method2 = getInputElementByXid(xid[j]);
        
      method1 = method1.nextSibling;
      method2 = method2.nextSibling;
        
      var sel_val1 = method1.options[method1.selectedIndex].innerHTML;
      var sel_val2 = method2.options[method2.selectedIndex].innerHTML;
      if ((sel_val1 == sel_val2) && (( sel_val1 != "Undefined") || ( sel_val2 != "Undefined")))
      {
        method2.selectedIndex = 0;
        alert ("This method is already in use.");
        return false;
      } 
      if ((sel_val1 == "LOCAL" || sel_val1 == "NONE" || sel_val1 == "ENABLE" || sel_val1 == "LINE") && ( sel_val2 != "Undefined"))
      {
        method2.selectedIndex = 0;
        alert ("Not a valid value after " + sel_val1);
        return false;
      }
    }
  } 
} 

