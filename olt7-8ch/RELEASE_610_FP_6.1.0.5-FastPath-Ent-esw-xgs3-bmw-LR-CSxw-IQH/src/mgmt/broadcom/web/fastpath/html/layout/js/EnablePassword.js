xuiIncludeJSFile("/scripts/globalStyleJS.js");

function EnablePassword_SubmitStart(){
	globalStyleJS_SubmitStart();
}
function EnablePassword_SubmitEnd(){
	globalStyleJS_SubmitEnd();
	
	var pass = MygetElementByName('v_1_1_1');
	var confirmpass = MygetElementByName('v_1_2_1');
	
	if((pass.value == "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx") && 
	   (confirmpass.value == "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"))
	{
		alert("Password is not edited");
		return false;
	}	
	else if (pass.value != confirmpass.value)
   { 
		alert("Password and confirm password must be the same");
		return false;
   }
   else if((pass.value).length < 8 || (confirmpass.value).length > 64)
   {
		alert("Minmimum length of password is 8 and Max is 64");
		return false;
   }
}
function EnablePassword_LoadStart(){
	globalStyleJS_LoadStart();

}
function EnablePassword_LoadEnd()
{
	globalStyleJS_LoadEnd();

	var pass = MygetElementByName('v_1_1_1');
	var confirmpass = MygetElementByName('v_1_2_1');

	pass.nextSibling.value = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	confirmpass.nextSibling.value = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	
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
