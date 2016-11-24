function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function HelperIPIntfCfg_copy_SubmitStart(submitFlag) {
   
}


function HelperIPIntfCfg_copy_SubmitEnd(submitFlag){
  
}

function HelperIPIntfCfg_copy_LoadStart(arg){
   
}

function HelperIPIntfCfg_copy_LoadEnd(arg){

   var elementCombo = getInputElementByXid('1_2_2');   
   var elementIntf = getInputElementByXid('1_3_1');
   
	var selIndex1 = elementCombo.nextSibling.selectedIndex;
	elementIntf.value = elementCombo.nextSibling.options[selIndex1].text;
	elementIntf.nextSibling.value = elementCombo.nextSibling.options[selIndex1].text;

}
