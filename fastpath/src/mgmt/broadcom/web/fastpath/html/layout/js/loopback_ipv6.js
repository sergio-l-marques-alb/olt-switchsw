function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(!element.hasAttribute('xid') ||
            element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function loopback_ipv6_SubmitStart(submitFlag) {

  var e_1_10_1  =  getInputElementByXid('1_10_1');  
  var e_1_13_1  =  getInputElementByXid('1_13_1');
  var e_1_14_1  =  getInputElementByXid('1_14_1');
  var e_1_16_5  =  getInputElementByXid('1_16_5');


  if((e_1_10_1 != null) && (e_1_13_1 != null) 
               && (e_1_14_1 != null) && (e_1_16_5 != null)) 
  {
    if(((e_1_10_1.nextSibling.options[0].selected == true &&
         e_1_10_1.nextSibling.options[0].value == "Enable") ||
        (e_1_10_1.nextSibling.options[1].selected == true &&
         e_1_10_1.nextSibling.options[1].value == "Enable")) ||
       ((e_1_10_1.nextSibling.options[0].selected == true &&
         e_1_10_1.nextSibling.options[0].value == "Disable") ||
        (e_1_10_1.nextSibling.options[1].selected == true &&
         e_1_10_1.nextSibling.options[1].value == "Disable")))
	
    {       
      if(e_1_13_1.nextSibling.value == null || e_1_13_1.nextSibling.value == "" || e_1_13_1.nextSibling.value == 0)
	    { 
        e_1_13_1.disabled = e_1_13_1.nextSibling.disabled = true;
		    e_1_16_5.disabled = e_1_16_5.nextSibling.disabled = true;
	    }	 
      if(e_1_14_1.nextSibling.value == null || e_1_14_1.nextSibling.value == "" || e_1_14_1.nextSibling.value == 0)
	    {
        e_1_14_1.disabled = e_1_14_1.nextSibling.disabled = true;
		    e_1_16_5.disabled = e_1_16_5.nextSibling.disabled = true;
	    }	 
    }
  }
}


function loopback_ipv6_SubmitEnd(submitFlag){

}

function loopback_ipv6_LoadStart(arg){


}

function loopback_ipv6_LoadEnd(arg){

}

