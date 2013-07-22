function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function IPV6ACLRuleDestIPConfig_SubmitStart(submitFlag){
  var dummy_element = getInputElementByXid('1_4_1');
  var prefix_element = getInputElementByXid('1_4_2');
  var prefixLength_element = getInputElementByXid('1_4_3');
  var pre;

  if (dummy_element == null) return;
  if (prefix_element == null) return;
  if (prefixLength_element == null) return;

  pre = dummy_element.nextSibling.value.split("/");

  prefix_element.value = prefix_element.value.replace(prefix_element.value,pre[0]);
  prefixLength_element.value = prefixLength_element.value.replace(prefixLength_element.value,pre[1]);

  return;
}


function IPV6ACLRuleDestIPConfig_SubmitEnd(submitFlag){

}

function IPV6ACLRuleDestIPConfig_LoadStart(arg){

}

function IPV6ACLRuleDestIPConfig_LoadEnd(arg){
  var dummy_element = getInputElementByXid('1_4_1');
  var prefix_element = getInputElementByXid('1_4_2');
  var prefixLength_element = getInputElementByXid('1_4_3');
  var str = "/";

  if (dummy_element == null) return;
  if (prefix_element == null) return;
  if (prefixLength_element == null) return;
  
  if ((prefix_element.value == "" ) || (prefixLength_element.value == ""))
  {
    return;
  }
  else
  {
   dummy_element.nextSibling.value = dummy_element.nextSibling.value.concat(prefix_element.value.concat(str.concat(prefixLength_element.value)));
  }
  return;

}



