function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function VRRPTrackIntfAdd_copy_SubmitStart(submitFlag) {

	var elementCombo = getInputElementByXid('1_3_1');
	var elementIntf = getInputElementByXid('1_6_1');

	var selIndex1 = elementCombo.nextSibling.selectedIndex;

	elementIntf.value = elementCombo.nextSibling.options[selIndex1].text;
	
}


function VRRPTrackIntfAdd_copy_SubmitEnd(submitFlag){
  
}

function VRRPTrackIntfAdd_copy_LoadStart(arg){

	var elementSourceIntf = getInputElementByXid('1_7_1');   
	var elementDestIntf = getInputElementByXid('1_1_1');

	var elementSourcevrID = getInputElementByXid('1_8_1');   
	var elementDestvrID = getInputElementByXid('1_2_1');

	elementDestIntf.value = elementSourceIntf.value;

	elementDestvrID.value = elementSourcevrID.value;
}

function VRRPTrackIntfAdd_copy_LoadEnd(arg){
 
}
