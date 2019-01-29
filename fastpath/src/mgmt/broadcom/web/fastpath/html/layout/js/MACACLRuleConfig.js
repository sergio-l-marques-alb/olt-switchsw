xuiIncludeJSFile("/scripts/globalStyleJS.js");
xuiIncludeJSFile("/scripts/xui_oem.js");

function MACACLRuleConfig_LoadEnd()
{
  
  globalStyleJS_LoadEnd();


  //
  // SrcMac and Mask
  //
  var tr_1_17 = getElemByIdAndTagName('TR',"1_17"); 
  
  var input_1_16_1 = getElemByIdAndTagName('INPUT','1_16_1');
  var input_1_17_1 = getElemByIdAndTagName('INPUT','1_17_1');
  
  if(input_1_16_1 != null && input_1_17_1 != null)
  {
     var td_left_1_16_1 =  getElemByIdAndTagName('TD','1_16_1');
     td_left_1_16_1.innerHTML += "<br>" + "Destination MAC Mask "

     //hide and disable the row 1_17
      tr_1_17.style.display="none";
      input_1_17_1.disabled=true;
     

  }


  //
  // DstMac and Mask
  //


  var tr_1_21 = getElemByIdAndTagName('TR',"1_21");

  var input_1_20_1 = getElemByIdAndTagName('INPUT','1_20_1');
  var input_1_21_1 = getElemByIdAndTagName('INPUT','1_21_1');

  if(input_1_20_1 != null && input_1_21_1 != null)
  {
     var td_left_1_20_1 =  getElemByIdAndTagName('TD','1_20_1');
     td_left_1_20_1.innerHTML += "<br>" + "Source MAC Mask"

     //hide and disable the row 1_21
      tr_1_21.style.display="none";
      input_1_21_1.disabled=true;


  }



  
}  
  
  
function MACACLRuleConfig_LoadStart()
{
  globalStyleJS_LoadStart(); 

  //
  //SrcIP and Mask
  //
  var input_1_16_1 = getElemByIdAndTagName('INPUT','1_16_1');
  var input_1_17_1 = getElemByIdAndTagName('INPUT','1_17_1');

  if(input_1_16_1 != null && input_1_17_1 != null)
  {

     var ipAddr = input_1_16_1.value;
     var mask   = input_1_17_1.value;
     input_1_16_1.value=ipAddr + "<br>" + mask;
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

function MACACLRuleConfig_SubmitStart()
{
  globalStyleJS_SubmitStart();
}

function MACACLRuleConfig_SubmitEnd()
{
  globalStyleJS_SubmitEnd();
}
   
  

