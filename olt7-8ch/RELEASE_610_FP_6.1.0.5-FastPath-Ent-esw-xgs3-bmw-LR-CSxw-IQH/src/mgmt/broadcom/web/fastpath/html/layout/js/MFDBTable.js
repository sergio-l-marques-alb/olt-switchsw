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
 var tblObj = document.getElementsByTagName('table');
 var inputs = tblObj[1].getElementsByTagName('input');
 var mac = inputs[1].value;
 var found = 0;
 
 var valid = validateMac(mac); 

 if (valid == 1)
 {
   alert("Enter 2 bytes of VLAN Id and 6 bytes of MAC Address separated by colons" 
          + '\n' + "in the format XX:XX:XX:XX:XX:XX:XX:XX");
   return;
 }

 var allRowsOfMFDBtable =  tblObj[2].getElementsByTagName('tr');
 for (var i = 1; i < allRowsOfMFDBtable.length; i++)
  {
  
    var tmpInputs = allRowsOfMFDBtable[i].getElementsByTagName('td');
    if (tmpInputs == null)
    {
      continue;
    }
    var tmpMAC = tmpInputs[0].innerHTML;
 
    if( mac != tmpMAC )
    {
       allRowsOfMFDBtable[i].style.display = 'none';
    } 
    else
    {
       found = 1;
       allRowsOfMFDBtable[i].style.display = '';
    }
  }

 if (found == 0)
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
