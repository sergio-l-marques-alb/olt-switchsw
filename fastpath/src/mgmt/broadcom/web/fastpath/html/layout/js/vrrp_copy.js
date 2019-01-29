function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function vrrp_copy_SubmitStart(submitFlag) {
   
   var elementCombo = getInputElementByXid('1_4_1');   
   var elementIntf = getInputElementByXid('1_15_1');
   
    if(elementCombo.disabled == false) {
	var selIndex1 = elementCombo.nextSibling.selectedIndex;
	elementIntf.value = elementCombo.nextSibling.options[selIndex1].text;
    } 
}


function vrrp_copy_SubmitEnd(submitFlag){
  
}

function vrrp_copy_LoadStart(arg){
   
}

function vrrp_copy_LoadEnd(arg){
 
}
