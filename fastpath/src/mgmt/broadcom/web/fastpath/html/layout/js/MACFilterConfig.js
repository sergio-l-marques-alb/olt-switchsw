function getInputElementByXid(xid){
    var all_elements = document.getElementsByTagName('input') ;
    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;
        return element;
    }
}


function MACFilterConfig_SubmitStart(submitFlag){
   var vidMAC = getInputElementByXid('1_2_1');
   var mac = getInputElementByXid('1_3_1');
   var vid = getInputElementByXid('1_4_1');
   var vidMACValue = null;
   var numb = null;

    if(((mac.nextSibling.value.length == 0) || (mac.nextSibling.value == null) || (mac.nextSibling.value == " ")) &&
        ((vid.nextSibling.value.length == 0) || (vid.nextSibling.value == null) || (vid.nextSibling.value == " ")))
  {
      vidMACValue = vidMAC.nextSibling.value;
      numb = vidMACValue.substring(0,2) + vidMACValue.substring(3,5);
      mac.nextSibling.value = vidMACValue.substring(6,23);  
      vid.nextSibling.value = parseInt(numb,16); 
  }  
}
function MACFilterConfig_SubmitEnd(submitFlag){

  

}



function MACFilterConfig_LoadStart(arg){

   

}



function MACFilterConfig_LoadEnd(arg){

 

}

