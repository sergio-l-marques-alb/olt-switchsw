xuiIncludeJSFile("/scripts/globalStyleJS.js");
xuiIncludeJSFile("/scripts/xui_oem.js");

function IPACLRuleConfig_LoadEnd()
{
  
  globalStyleJS_LoadEnd();


  //
  // SrcIP and Mask
  //
  var tr_1_16 = getElemByIdAndTagName('TR',"1_16"); 
  
  var input_1_15_1 = getElemByIdAndTagName('INPUT','1_15_1');
  var input_1_16_1 = getElemByIdAndTagName('INPUT','1_16_1');
  
  if(input_1_15_1 != null && input_1_16_1 != null)
  {
     var td_left_1_15_1 =  getElemByIdAndTagName('TD','1_15_1');
     td_left_1_15_1.innerHTML += "<br>" + "Source IP wildcard mask"

     //hide and disable the row 1_16
      tr_1_16.style.display="none";
      input_1_16_1.disabled=true;
     

  }


  //
  // DstIP and Mask
  //


  var tr_1_21 = getElemByIdAndTagName('TR',"1_21");

  var input_1_20_1 = getElemByIdAndTagName('INPUT','1_20_1');
  var input_1_21_1 = getElemByIdAndTagName('INPUT','1_21_1');

  if(input_1_20_1 != null && input_1_21_1 != null)
  {
     var td_left_1_20_1 =  getElemByIdAndTagName('TD','1_20_1');
     td_left_1_20_1.innerHTML += "<br>" + "Destination IP wildcard mask"

     //hide and disable the row 1_21
      tr_1_21.style.display="none";
      input_1_21_1.disabled=true;


  }



  
}  
  
  
function IPACLRuleConfig_LoadStart()
{
  globalStyleJS_LoadStart(); 

  //
  //SrcIP and Mask
  //
  var input_1_15_1 = getElemByIdAndTagName('INPUT','1_15_1');
  var input_1_16_1 = getElemByIdAndTagName('INPUT','1_16_1');

  if(input_1_15_1 != null && input_1_16_1 != null)
  {

     var ipAddr = input_1_15_1.value;
     var mask   = input_1_16_1.value;
     input_1_15_1.value=ipAddr + "<br>" + mask;
  }

  //
  //DstIP and Mask
  //
  var input_1_20_1 = getElemByIdAndTagName('INPUT','1_20_1');
  var input_1_21_1 = getElemByIdAndTagName('INPUT','1_21_1');

  if(input_1_20_1 != null && input_1_21_1 != null)
  {

     var ipAddr = input_1_20_1.value;
     var mask   = input_1_21_1.value;
     input_1_20_1.value=ipAddr + "<br>" + mask;
  }


}

function IPACLRuleConfig_SubmitStart()
{
  globalStyleJS_SubmitStart();
}

function IPACLRuleConfig_SubmitEnd()
{
  globalStyleJS_SubmitEnd();
}
   
  

