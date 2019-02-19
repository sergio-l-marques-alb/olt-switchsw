
var allListAction;
var dirListAction;
function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}

function DiffservConfigButtons_SubmitStart(submitFlag) {

	var elementIfIndex = getInputElementByXid('1_1_1');
        var elementInPolicyDirectionList = getInputElementByXid('1_3_1');
	var elementInPolicyNameList = getInputElementByXid('1_4_1');
    var elementInPolicyNameAdd = getInputElementByXid('1_6_1');
    var elementInPolicyNameDel = getInputElementByXid('1_7_1');
	var elementOutPolicyNameList = getInputElementByXid('1_5_1');
	var elementOutPolicyNameListAll = getInputElementByXid('1_14_1');
    var elementOutPolicyNameAdd = getInputElementByXid('1_8_1');
    var elementOutPolicyNameDel = getInputElementByXid('1_9_1');
	var elementGlobalPolicyNameAdd = getInputElementByXid('1_10_1');
	var elementGlobalPolicyNameDel = getInputElementByXid('1_11_1');

    var IfIndexOptions=elementIfIndex.nextSibling.options;
    var InPolicyOptions=elementInPolicyNameList.nextSibling.options;
    var OutPolicyOptions;
    if (elementOutPolicyNameList)
     OutPolicyOptions = elementOutPolicyNameList.nextSibling.options;
    else
     OutPolicyOptions = null;

    var OutAllPolicyOptions;
    if (elementOutPolicyNameListAll)
     OutAllPolicyOptions = elementOutPolicyNameListAll.nextSibling.options;
    else
     OutAllPolicyOptions = null;

    elementInPolicyNameDel.disabled=true;
    if (elementOutPolicyNameDel)
    elementOutPolicyNameDel.disabled=true;
	elementGlobalPolicyNameDel.disabled=true;


  if (submitFlag == 1)
  {
    elementInPolicyNameList.disabled = true; 
    if (elementOutPolicyNameList)
      elementOutPolicyNameList.disabled = true;
    if (elementOutPolicyNameListAll)
      elementOutPolicyNameListAll.disabled = true;
  }

    for (var i=0; i<InPolicyOptions.length; i++)
    {
      if(InPolicyOptions[i].selected == true)
      {
        break;
      }
    }

    for (var j=0; j<IfIndexOptions.length; j++)
    {
      if(IfIndexOptions[j].selected == true)
      {
        break;
      }              
    }

    for (var k=0; OutPolicyOptions && k<OutPolicyOptions.length; k++)
    {
      if(OutPolicyOptions[k].selected == true)
      {
        break;
      }
    }

    for (var l=0; OutAllPolicyOptions && l<OutAllPolicyOptions.length; l++)
    {
      if(OutAllPolicyOptions[l].selected == true)
      {
        break;
      }
    }

    if (IfIndexOptions[j].text == 'All')
    {


      if (elementInPolicyDirectionList.nextSibling.options[elementInPolicyDirectionList.nextSibling.selectedIndex].text == 'In')
      {
        if (InPolicyOptions[elementInPolicyNameList.nextSibling.selectedIndex].text == 'None') 
        {
            elementGlobalPolicyNameDel.disabled=false;  
            elementGlobalPolicyNameDel.parentNode.disabled=false;  
            elementGlobalPolicyNameAdd.disabled=true;  
            elementGlobalPolicyNameAdd.parentNode.disabled=true;  
        }
        else
        {
            elementGlobalPolicyNameAdd.disabled=false; 
            elementGlobalPolicyNameAdd.parentNode.disabled=false; 
        }
      }
      else
      {
        if (OutAllPolicyOptions && OutAllPolicyOptions[l].text == 'None')
         {
            elementGlobalPolicyNameDel.disabled=false; 
            elementGlobalPolicyNameDel.parentNode.disabled=false; 
            elementGlobalPolicyNameAdd.disabled=true; 
            elementGlobalPolicyNameAdd.parentNode.disabled=true; 
         }
         else
         {
            elementGlobalPolicyNameAdd.disabled=false; 
            elementGlobalPolicyNameAdd.parentNode.disabled=false; 
         }
      }

        elementInPolicyNameAdd.disabled = true;
        if (elementOutPolicyNameAdd)
          elementOutPolicyNameAdd.disabled = true;
    }
    else
    {
        elementGlobalPolicyNameAdd.disabled = true;
        if (InPolicyOptions[i].text == 'None')
        {
            elementInPolicyNameAdd.disabled=true;
            elementInPolicyNameAdd.parentNode.disabled=true;
            elementInPolicyNameDel.disabled=false;
            elementInPolicyNameDel.parentNode.disabled=false;
        }
        if (OutPolicyOptions && OutPolicyOptions[k].text == 'None')
        {
            elementOutPolicyNameAdd.disabled=true;
            elementOutPolicyNameAdd.parentNode.disabled=true;
            elementOutPolicyNameDel.disabled=false;
            elementOutPolicyNameDel.parentNode.disabled=false;
        }
    }
}

function DiffservConfigButtons_SubmitEnd(submitFlag){
}

function transformValues(xid,direction) {
 
    var form = document.forms[0];
    inputs = form.getElementsByTagName("input"); 
    if(!inputs) return;
 
    for( var z=0; z < inputs.length; z++)
    {
      var name = inputs.item(z).name;
 
      if(name.indexOf(xid) != -1)
      {
        var val = inputs.item(z).value;
        if( val != direction)
        {
          inputs.item(z).parentNode.parentNode.style.display  = "none";
        }
        else
        {
          inputs.item(z).parentNode.parentNode.style.display  = "";
        }
      }
    }
}

function DiffservConfigButtons_LoadStart(arg){
	var elementInPolicyNameList = getInputElementByXid('1_4_1');
	var elementOutPolicyNameList = getInputElementByXid('1_5_1');
	var elementOutPolicyNameListAll = getInputElementByXid('1_14_1');

       if (elementInPolicyNameList.value == "")
         elementInPolicyNameList.value = "None";
       if (elementOutPolicyNameList && elementOutPolicyNameList.value == "")
         elementOutPolicyNameList.value = "None";
       if (elementOutPolicyNameListAll && elementOutPolicyNameListAll.value == "")
         elementOutPolicyNameListAll.value = "None";
}
function registerOnChangeFunction(obj, func)

{
  if(obj == undefined) {
    return;
  }
  obj.onchange = func;
}

function filterTable() {
  var elementInPolicyDirectionList = getInputElementByXid('1_3_1');
  var obj = elementInPolicyDirectionList.nextSibling;

//  xuiProcessOnChangeRefresh(false, '0' , '2' ,obj.selectedIndex, '1_3_1' , obj.name, obj);
  eval(dirListAction);

 transformValues("1_13_2",obj.options[obj.selectedIndex].text);
}

function intIfFilterTable() {
  var elementIntfList = getInputElementByXid('1_1_1');
  var obj = elementIntfList.nextSibling;

 // xuiProcessOnChangeRefresh(true, '1' , '53' ,obj.selectedIndex, '1_1_1' , obj.name, obj);
  eval(allListAction);
  if (obj.options[obj.selectedIndex].text == "All")
    transformValues("1_13_2","In");
}


function DiffservConfigButtons_LoadEnd(arg){
   var elementInPolicyDirectionList = getInputElementByXid('1_3_1');
   var elementIntfList = getInputElementByXid('1_1_1');
  allListAction = "" + elementIntfList.nextSibling.getAttribute("onChange");
  dirListAction = "" + elementInPolicyDirectionList.nextSibling.getAttribute("onChange");
  var idx;

 idx = allListAction.indexOf("anonymous");
 if (idx != -1)
 {
   allListAction = allListAction.substr(idx+11);
 }

 idx = dirListAction.indexOf("anonymous");
 if (idx != -1)
 {
   dirListAction = dirListAction.substr(idx+11);
 }

  registerOnChangeFunction(elementInPolicyDirectionList.nextSibling, filterTable);
  registerOnChangeFunction(elementIntfList.nextSibling, intIfFilterTable);
//  transformValues("1_13_2", "In");
}

