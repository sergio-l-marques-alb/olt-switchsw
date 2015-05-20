xuiIncludeJSFile("/scripts/prototype.js");
xuiIncludeJSFile("/scripts/utils.js");
//Write JS global functions here

function displayHelp2()
{
  presentHTML = parent.basefrm.location.pathname
  helpFile = "";
  if(navigator.family != "ie4")
  {
    var URL = presentHTML.match(/(.*)[\/\\]([^\/\\]+\.\w+)$/);
    helpFile = "/help_"+URL[2];
  }
  else
  {
    helpFile = "/help_"+presentHTML;
  }
  //create HELP window now
  window.open(helpFile, 'helpwindow', 'resizable=yes,scrollbars=yes,height=480,width=640', false);
}

function displayHelp()
{
  helpFile = "/" + xeData.thisPageHelp;
  //create HELP window now
  window.open(helpFile, '_blank');
}

/* Used for WIRELESS*/
/*Radha : Used to throw error when no check-boxes are selected for operation*/
function delete_check()
{
//alert("Function invoked...");
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
      count++;
  }

  if (count == 0)
  {
    alert("Error: Please make a selection by clicking on check box(es).\0");
  }
}

/*AP Management : Managed AP Reset*/
function resetManagedAP_check()
{
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
      count++;
  }

  if (count == 0)
  {
    alert("Error: Please make a selection by clicking on check box(es).\0");
  }
  else
  {
    if (count == 1)
      var prompt = window.confirm("Are you sure you want to delete the connection failed managed AP entry?");
    else
      var prompt = window.confirm("Are you sure you want to delete the connection failed managed AP entries?");
  }
}

/*Associated Client : SSID Status*/
function disassociateSSID_check()
{
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
      count++;
  }

  if (count == 0)
  {
    alert("Error: Please make a selection by clicking on check box(es).\0");
  }
  else
  {
    if (count == 1)
      var prompt = window.confirm("Are you sure you want to disassociate all the clients associated with this SSID?");
    else
      var prompt = window.confirm("Are you sure you want to disassociate all the clients associated with these SSIDs?");
  }
}

/*Associated Client : Switch Status*/
function disassociateSwitch_check()
{
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
      count++;
  }

  if (count == 0)
  {
    alert("Error: Please make a selection by clicking on check box(es).\0");
  }
  else
  {
    if (count == 1)
      var prompt = window.confirm("Are you sure you want to disassociate all the clients associated with this Switch?");
    else
      var prompt = window.confirm("Are you sure you want to disassociate all the clients associated with these Switches?");
  }
}

/*Associated Client : VAP Status*/
function disassociateVAP_check()
{
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
      count++;
  }

  if (count == 0)
  {
    alert("Error: Please make a selection by clicking on check box(es).\0");
  }
  else
  {
    if (count == 1)
      var prompt = window.confirm("Are you sure you want to disassociate all the clients associated with this VAP?");
    else
      var prompt = window.confirm("Are you sure you want to disassociate all the clients associated with these VAPs?");
  }
}

/*Advanced Config: Peer switch Config Request*/
function cp_check()
{
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
      count++;
  }

  if (count == 0)
  {
    alert("Error: Please make a selection by clicking on check box(es).\0");
  }
  else
  {
    var prompt = window.confirm("Are you sure you want to push configuration to the peer switch(es)?");
  }
}

/*Detected Client : summary*/
function delete_detectedClientCheck()
{
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
      count++;
  }

  if (count == 0)
  {
    alert("Error: Please make a selection by clicking on check box(es).\0");
  }
  else
  {
    if (count == 1)
      var prompt = window.confirm("Are you sure you want to delete the detected client entry?");
    else
      var prompt = window.confirm("Are you sure you want to delete the detected client entries?");
  }
}

/*Basic Setup : Discovery config*/
var BLANK = "<empty list>";
function addValToList(type)
{
//alert("function invoked ..");
  if ("IP"==type)
  {
    //get the ip address and add to list
    var element_1_3_1  =  getInputElementByXid('1_3_1');
    val = element_1_3_1.nextSibling.value;

    var element_1_2_1 = getInputElementByXid('1_2_1');
    list = element_1_2_1.nextSibling;
    
    if (verifyIP(val) == false)
      return;
  }
  else
  {
    //get the vlan id and add to list
    var element_1_3_2 = getInputElementByXid('1_3_2');
    val = element_1_3_2.nextSibling.value;

    var element_1_2_2 = getInputElementByXid('1_2_2');
    list = element_1_2_2.nextSibling;

        if (isNaN(val))
        {
          alert("Error: Please enter a valid VLAN ID: (1-4094)");
          return;
        }
        intVal = parseInt(val);
        if ((intVal<1) || (intVal>4094))
        {
          alert("Error: Please enter a valid VLAN ID: (1-4094)");
          return;
        }
  }

  if (""!=val)
  {
    removeBlankFromList(type);
    for (var i=0;i<list.length;i++)
    {
       myArray = list.options[i].text.split(" -");
      if (myArray[0] == val)
      {
        alert("Error: " + val + " is already in the list!");
        addBlankToList(type);
        return;
      }
    }
    list.options[i] = new Option(val);
    if ("IP"==type)
    { 
      var element_1_12_2  =  getInputElementByXid('1_12_2');
      /*Add list options to the object*/
      element_1_12_2.nextSibling.value += val; 
      element_1_12_2.nextSibling.value += ",";
    }
    else
    {
      var element_1_13_2  =  getInputElementByXid('1_13_2');
      /*Add list options to the object*/
      element_1_13_2.nextSibling.value += val;
      element_1_13_2.nextSibling.value += ",";
    }
  }
  return;
}

function delSelFromList(type)
{
/*Get the list : IP or VLAN*/ 
  if ("IP"==type)
  {
    var element_1_2_1 = getInputElementByXid('1_2_1');
    var element_2_2_1 = getInputElementByXid('2_2_1');
    if(element_1_2_1 != null)
    {
      list = element_1_2_1.nextSibling;
    }
    if(element_2_2_1 != null)
    {
      list = element_2_2_1.nextSibling;
    } 
  }
  else 
  {
    var element_1_2_2 = getInputElementByXid('1_2_2');
    var element_2_2_2 = getInputElementByXid('2_2_1');
    if(element_1_2_2 != null)
    {
      list = element_1_2_2.nextSibling;
    }
    if(element_2_2_2 != null)
    {
      list = element_2_2_2.nextSibling;
    }
  }
/*Perform the deletion*/
  for (i=list.length-1;i>=0;i--)
  {
    if (list.options[i].selected == true)
    {
      list.options[i] = null;
    }
  }
/*Then add the list options to the object*/
  if ("IP"==type)
  {
    var element_1_12_2  =  getInputElementByXid('1_12_2');
    var element_2_12_2  =  getInputElementByXid('2_12_2');
    if(element_1_12_2 != null)
    { 
      element_1_12_2.nextSibling.value = "";
    }
    if(element_2_12_2 != null)
    {
      element_2_12_2.nextSibling.value = "";
    }
    for (i=0;i<list.length;i++)
    {
      if(element_1_12_2 != null)
      {
        element_1_12_2.nextSibling.value += list.options[i].text;
        element_1_12_2.nextSibling.value += ",";
      }
      if(element_2_12_2 != null)
      {
        element_2_12_2.nextSibling.value += list.options[i].text;
        element_2_12_2.nextSibling.value += ",";
      } 
    }
  }
  else
  {
    var element_1_13_2  =  getInputElementByXid('1_13_2');
    var element_2_13_2  =  getInputElementByXid('2_13_2');
    if(element_1_13_2 != null)
    {
      element_1_13_2.nextSibling.value = "";
    }
    if(element_2_13_2 != null)
    {
      element_2_13_2.nextSibling.value = "";
    } 
    for (i=0;i<list.length;i++)
    {
      myArray = list.options[i].text.split(" -");
      if(element_1_13_2 != null)
      {
        element_1_13_2.nextSibling.value += myArray[0];
        element_1_13_2.nextSibling.value += ",";
      }
      if(element_2_13_2 != null)
      {
        element_2_13_2.nextSibling.value += myArray[0];
        element_2_13_2.nextSibling.value += ",";
      } 
    }
  }
  addBlankToList(type);
}

function addBlankToList(type)
{
  if ("IP"==type)
  {
    var element_1_2_1 = getInputElementByXid('1_2_1');
    var element_2_2_1 = getInputElementByXid('2_2_1');
    if(element_1_2_1 != null)
    {
      list = element_1_2_1.nextSibling;
    }
    if(element_2_2_1 != null)
    {
      list = element_2_2_1.nextSibling;
    }
  }
  else
  {
    var element_1_2_2 = getInputElementByXid('1_2_2');
    var element_2_2_2 = getInputElementByXid('2_2_2');
    if(element_1_2_2 != null)
    { 
      list = element_1_2_2.nextSibling;
    }
    if(element_2_2_2 != null)
    {
      list = element_2_2_2.nextSibling;
    }
  }

  if (list.length==0)
    list.options[0] = new Option(BLANK);
}

function removeBlankFromList(type)
{
  if ("IP"==type)
  {
    var element_1_2_1 = getInputElementByXid('1_2_1');
    var element_2_2_1 = getInputElementByXid('2_2_1');
    if(element_1_2_1 != null)
    {
      list = element_1_2_1.nextSibling;
    }
    if(element_2_2_1 != null)
    {
      list = element_2_2_1.nextSibling;
    }
  }
  else
  {
    var element_1_2_2 = getInputElementByXid('1_2_2');
    var element_2_2_2 = getInputElementByXid('2_2_2'); 
    if(element_1_2_2 != null)
    {
      list = element_1_2_2.nextSibling;
    }
    if(element_2_2_2 != null)
    {
      list = element_2_2_2.nextSibling;
    }
  }

  if (list.options[0].text==BLANK)
    list.options[0] = null;
}

function selectListItems()
{
  var element_1_1_1 = getInputElementByXid('1_1_1');
  var element_2_1_1 = getInputElementByXid('2_1_1');
  if(element_1_1_1 != null)
  {
    ipMode = element_1_1_1.nextSibling;
  }
  if(element_2_1_1 != null)
  {
    ipMode = element_2_1_1.nextSibling;
  }
  var element_1_2_1 = getInputElementByXid('1_2_1');
  var element_2_2_1 = getInputElementByXid('2_2_1');

  var element_1_1_2 = getInputElementByXid('1_1_2');
  var element_2_1_2 = getInputElementByXid('2_1_2');
  if(element_1_1_2 != null)
  {
    l2Mode = element_1_1_2.nextSibling;
  } 
  if(element_2_1_2 != null)
  {
    l2Mode = element_2_1_2.nextSibling;
  }
  var element_1_2_2 = getInputElementByXid('1_2_2');
  var element_2_2_2 = getInputElementByXid('2_2_2');

  if(ipMode.checked)
  {
    if(element_1_2_1 != null)
    {
      list = element_1_2_1.nextSibling; 
    }
    if(element_2_2_1 != null)
    {
      list = element_2_2_1.nextSibling;
    }
    for (i=0;i<list.length;i++)
    {
      list.options[i].selected = true;
    }
  }
  if (l2Mode.checked)
  {
    if(element_1_2_2 != null)
    {
      list = element_1_2_2.nextSibling;
    }
    if(element_2_2_2 != null)
    {
      list = element_2_2_2.nextSibling;
    }
    for (i=0;i<list.length;i++)
    {
      list.options[i].selected = true;
    }
  }
}

function getInputElementByXid(xid)
{
    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++)
    {
        var element = all_elements[index];
        if(element.getAttribute('xid') == null ||
           element.getAttribute('xid') != xid) continue;

        return element;
    }


}

function getElemByIdAndTagName(tagName,id)
{
  var elem_list = document.getElementsByTagName(tagName);
  for(var index=0;index<elem_list.length;index++)
  {
      if(elem_list[index].getAttribute('id') == id ||
         elem_list[index].getAttribute('xid') == id)
      {
        return elem_list[index];
      }
  }

  return null;
}

function check_deleteOui()
{
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
    {
      count++
    }
  }
  var element_1_5_1 = getInputElementByXid('4_5_1');
  element_1_5_1.nextSibling.disabled = (count>0)?false:true
}

function check_deleteManagedAP()
{
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
      count++;
  }

  if (count == 0)
  {
    alert("Error: Please make a selection by clicking on check box(es).\0");
  }
  else
  {
    if (count == 1)
    {
      var prompt = window.confirm("Are you sure you want to delete the connection failed managed AP entry?");
    }
    else
    {
      var prompt = window.confirm("Are you sure you want to delete the connection failed managed AP entries?");
    }
    if (!prompt)
    {
      document.forms[0].submit();
      /*
      var element_1_5_1 = getInputElementByXid('2_1_2');
      element_1_5_1.nextSibling.disabled = false; */
    }
  }
}

function check_deleteProfile()
{
  count = 0;
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    if (document.forms[0].elements[i].checked == true)
    {
      count++
    }
  }
  var element_2_3_3 = getInputElementByXid('2_3_3');
  if(element_2_3_3 == null) return;
  var element_2_4_1 = getInputElementByXid('2_4_1');
  if(element_2_4_1 == null) return;
  var element_2_4_2 = getInputElementByXid('2_4_2');
  if(element_2_4_2 == null) return;

  element_2_3_3.nextSibling.disabled = (count>0)?false:true;
  element_2_4_1.nextSibling.disabled = (count>0)?false:true;
  element_2_4_2.nextSibling.disabled = (count>0)?false:true;
}


function check_displayMessage()
{
  var element_1_22_1;
  var chkBox;
  element_1_22_1 = getInputElementByXid('1_22_1');
  chkBox = element_1_22_1.nextSibling;
  var element_1_23_1;
  element_1_23_1 = getInputElementByXid('1_23_1');
  var element_1_20 = getElemByIdAndTagName('TR',"1_20");
  var element_1_24 = getElemByIdAndTagName('TR',"1_24");
  if (element_1_22_1 == null) return;
  if (chkBox == null) return;
  if (element_1_23_1 == null) return;
  if (element_1_20 == null) return;
  if (element_1_24 == null) return;

  if(chkBox.checked)
  {
    if(element_1_23_1.value == 'StackMember')
    {
     element_1_24.style.display = "none";
     element_1_24.firstChild.nextSibling.style.display="none";
     element_1_20.style.display = "";
     element_1_20.firstChild.nextSibling.style.display="";
     return;
    }
    if(element_1_23_1.value == 'Management')
    {
     element_1_24.style.display="";
     element_1_24.firstChild.nextSibling.style.display="";
     element_1_20.style.display = "none";
     element_1_20.firstChild.nextSibling.style.display="none";
     return;
    }
    if(element_1_23_1.value == 'StandBy')
    {
     element_1_24.style.display = "none";
     element_1_24.firstChild.nextSibling.style.display="none";
     element_1_20.style.display = "";
     element_1_20.firstChild.nextSibling.style.display="";
     return;
    }
  }

}

function check_addSubscription()
{
   var button = getInputElementByXid('1_10_1');
   if (button == null) return;

   button.nextSibling.click();
}

function check_mainPage()
{
 var form = document.forms[0];
 form.reset();
} 

function check_removeThHeading()
{
  var thID = getElemByIdAndTagName('TH',"1_2_2");
  thID.style.display= "none";
}

/*Page Refresh when extended action Global is selected*/
function pageRefresh()
{
document.forms[0].submit();
}

function ChangeRowStateBasedOnCheckboxState(args) {
  var xids = tokenize(args,true);
  var ele_obj_list= new Object();
  var elements_array = MapElements.GetElementsByXid(xids[0],"INPUT");
  table_length = elements_array.length/2;
  for (var ele=0; ele < xids.length; ele++) {
    var temp_array = MapElements.GetElementsByXid(xids[ele],"INPUT");
    ele_obj_list[ele] = temp_array;
  }

  for (var row=0; row < table_length; row++) {
    for (var ele=0; ele < xids.length; ele++) {
      var temp_array = ele_obj_list[ele];
      if (ele == 0) {
        var eleobj = temp_array[row * 2];
      } else {
        var eleobj = temp_array[row];
      }
      if (elements_array[(row * 2) +1].checked) {
        eleobj.disabled = false;
        eleobj.removeAttribute("disabled");
      } else {
        eleobj.disabled = true;
        eleobj.setAttribute("disabled", "true");
      }
    }
  }
}

