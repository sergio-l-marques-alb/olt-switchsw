function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function VRRPTrackIntf_copy_SubmitStart(submitFlag) {

	var elementSourceIntf = getInputElementByXid('1_1_1');   
	var elementDestIntf = getInputElementByXid('1_3_1');

	var elementSourcevrID = getInputElementByXid('1_2_1');   
	var elementDestvrID = getInputElementByXid('1_4_1');

	elementDestIntf.value = elementSourceIntf.value;

	elementDestvrID.value = elementSourcevrID.value;

}


function VRRPTrackIntf_copy_SubmitEnd(submitFlag){
  
}

function VRRPTrackIntf_copy_LoadStart(arg){
	
	var elementSourceIntf = getInputElementByXid('1_1_1');   
	var elementDestIntf = getInputElementByXid('1_3_1');

	var elementSourcevrID = getInputElementByXid('1_2_1');   
	var elementDestvrID = getInputElementByXid('1_4_1');

	elementDestIntf.value = elementSourceIntf.value;

	elementDestvrID.value = elementSourcevrID.value;

}

function VRRPTrackIntf_copy_LoadEnd(arg){
 
}
