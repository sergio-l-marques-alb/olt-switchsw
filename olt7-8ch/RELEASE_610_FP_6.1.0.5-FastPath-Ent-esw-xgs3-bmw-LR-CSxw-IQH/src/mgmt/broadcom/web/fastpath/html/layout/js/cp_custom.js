xuiIncludeJSFile("/scripts/globalStyleJS.js");

function clearForm()
{
  var clear = window.confirm("NOTE: If you clear this locale configuration,\n"
                          +"all the fields on this page will be reset and\n"
                          +"set to their default values!\n\n"
                          +"Are you sure you want to clear?");
  if (clear)
  {
    return true;
  }
  else
  {
    return false;
  }
}

function cp_custom_SubmitStart(){
	globalStyleJS_SubmitStart();
  var cpId = MygetElementByName('v_2_3_1');
  var webId = MygetElementByName('v_2_4_1');
  var preview_button = MygetElementByName('v_6_1_2');
  var dummy =  MygetElementByName('v_6_2_1');
  var url = "captive_portal.html?p1="+10;
  url += "&p2="+cpId.value;
  url += "&p3="+webId.value;
  if(dummy.value == "Enable")
  {
    window.open(url);
  }
  return true; 
}

function cp_custom_SubmitEnd(){
  var clearItem = MygetElementByName('v_6_2_2');
  globalStyleJS_SubmitEnd();
  if(clearItem != null)
  {
	if(clearItem.disabled == false)
	{
	  return clearForm();
	}
  }
  return true;
}

function cp_custom_LoadStart()
{
 return globalStyleJS_LoadStart();
}

function cp_custom_LoadEnd()
{
 return  globalStyleJS_LoadEnd();
}

function MygetElementByName(id)
{
	var all_elements = document.getElementsByTagName('input') ;
   for(var index=0;index<all_elements.length;index++){
      var element = all_elements[index];
      if( element.name != id) continue;
      return element;
   }
	return null;
}
