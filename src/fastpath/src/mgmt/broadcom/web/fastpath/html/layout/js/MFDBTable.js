xuiIncludeJSFile("/scripts/xui_oem.js");
function registerOnClickFunction(obj, func)
{
  if(obj == undefined) {
    alert("registerOnClickFunction : failed");
    return;
  }
  obj.setAttribute('onclick',func);
}

// return 0 for valid and 1 for invalid
function validateMac(mac)
{
  var result = 0;

  var tokens=mac.split(":",8); 

  if((mac.length != 23) || (tokens.length != 8))
  {
    result = 1;
    return result;
  } 

  var j = 2;
  for (var i = 1; i < 8; i++)
  {
    if (mac.charAt(j) != ":") 
    {  
      result = 1;
      break;
    }
    j = j + 3;
  } 
  
 return result; 
}
 
function searchMac()
{
 var tblObj = getElemByIdAndTagName('table','1_2');
 var searchField = getElemByIdAndTagName('input','1_1_1');
 
 if(tblObj == null || 
    searchField  == null)
 {
    return;
 }  
 var mac = searchField.nextSibling.value;
 var found = 0;
 var tmpArray = new Array();
 var valid =1;
 
 if(mac == "")
 {
    valid=2;
 }
 else
 { 
   valid = validateMac(mac); 
 }   

  if (valid == 1)
  {
    alert("Enter 2 bytes of VLAN Id and 6 bytes of MAC Address separated by colons" 
          + '\n' + "in the format XX:XX:XX:XX:XX:XX:XX:XX");
    return;
  }

  var allRowsOfMFDBtable =  tblObj.rows;
  for (var i = 1; i < allRowsOfMFDBtable.length; i++)
  {
  
    if(valid == 2)
	{
	   allRowsOfMFDBtable[i].style.display = '';
	   continue;
	} 
	
    var macField = allRowsOfMFDBtable[i].cells[1];
    if (macField == null)
    {
      continue;
    }
    var tmpMAC = macField.innerHTML;

    //case insensitive regular expression
    var regex = new RegExp(mac,"i");

    if(regex != null)     
    {

       var result = regex.exec(tmpMAC) ;

       //value matched with the regular expression pattern
       if(result != null && RegExp.leftContext.length ==0 && RegExp.rightContext.length ==0)
       {
           found = 1;
           allRowsOfMFDBtable[i].style.display = '';
       }
       else
       {
           allRowsOfMFDBtable[i].style.display = 'none';
       } 
    }
  }

  if (found == 0 && valid != 2)
  {
    alert(mac + " Not found");
  } 
}


function MFDBTable_SubmitStart()
{
  
}
 
function MFDBTable_SubmitEnd()
{
  
}
 
function MFDBTable_LoadStart()
{}
 
function MFDBTable_LoadEnd1()
{
  registerOnClickFunction(document.forms[0].v_1_1_2[1], 'searchMac()');
}

function MFDBTable_LoadEnd()
{
  document.forms[0].v_1_1_2[1].onclick = function(){
    searchMac();
  }
}
