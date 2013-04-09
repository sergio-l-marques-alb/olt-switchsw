
function dissable_empty_SubmitStart(submitFlag){
   
var form = document.forms[0];
  inputs = form.getElementsByTagName("input");
  if(!inputs) return;

  for( var k=0; k < inputs.length; k++)
  {
  //  if (inputs[k].type != 'hidden')
  {
      if ((inputs[k].value.length == 0) || (inputs[k].value == null) || inputs[k].value == " ")
      inputs[k].disabled = true;
  }
  }
}





function dissable_empty_SubmitEnd(submitFlag){

  

}



function dissable_empty_LoadStart(arg){

   

}



function dissable_empty_LoadEnd(arg){

 

}

