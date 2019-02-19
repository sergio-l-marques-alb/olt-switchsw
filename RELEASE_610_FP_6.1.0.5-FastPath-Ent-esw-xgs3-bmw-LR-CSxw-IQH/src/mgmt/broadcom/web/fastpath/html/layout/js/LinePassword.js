xuiIncludeJSFile("/scripts/globalStyleJS.js");

function LinePassword_SubmitStart(){
	globalStyleJS_SubmitStart();
}
function LinePassword_SubmitEnd(){
	globalStyleJS_SubmitEnd();

	var pass = MygetElementByName('v_1_2_1');
	var confirmpass = MygetElementByName('v_1_3_1');

	if (pass.value != confirmpass.value)
   { 
		alert("Password and confirm password must be the same");
		return false;
   }
   else if((pass.value).length < 8 || (confirmpass.value).length > 64)
   {
		alert("Minmimum length of password is 8 and Max is 64");
		return false;
   }
	
	var consolePass = MygetElementByName('v_2_1_1');
	var telnetPass = MygetElementByName('v_2_1_2');
	var sshPass = MygetElementByName('v_2_1_3');
	consolePass.value = pass.value;
	telnetPass.value = pass.value;
	sshPass.value = pass.value;
}
function LinePassword_LoadStart(){
	globalStyleJS_LoadStart();
}
function LinePassword_LoadEnd()
{
	globalStyleJS_LoadEnd();
	
	var pass = MygetElementByName('v_1_2_1');
	var confirmpass = MygetElementByName('v_1_3_1');

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
