function registerOnClickFunction(obj)

{

  if(obj == undefined) {

    alert("registerOnClickFunction : failed");

    return;

  }

  obj.onclick = searchMac;

}


function validateMac(mac)

{

  var result = 0;

  var exp = "^([0-9a-fA-F][0-9a-fA-F]:){7}([0-9a-fA-F][0-9a-fA-F])$" ;
  
  var regex = new RegExp(exp);
      
  result = regex.exec(mac) ;
        
  //value matched with the regular expression pattern
  if(result != null && RegExp.leftContext.length ==0 && RegExp.rightContext.length ==0)
  {
	
	result = 0 ;
		 
  }
  
  else
  {

    result = 1;
	
  }

 return result;

}

 

function searchMac()

{

 var tblObj = document.getElementsByTagName('table');

 var inputs = tblObj[1].getElementsByTagName('input');

 var mac = inputs[3].value;

 


 

 var valid = validateMac(mac);

 

 if (valid == 1)

 {

   alert("Enter 2 bytes of VLAN Id and 6 bytes MAC Address separated by colons"

          + '\n' + "in the format XX:XX:XX:XX:XX:XX:XX:XX");

   return;

 }

 

 var allRowsOfMFDBtable =  tblObj[2].getElementsByTagName('tr');
 
 var count = 0;

 for (var i = 1; i < allRowsOfMFDBtable.length; i++)

  {

    var tmpInputs = allRowsOfMFDBtable[i].getElementsByTagName('td');

    var tmpMAC = tmpInputs[0].innerHTML;
    var exp =   tmpMAC;
    var regex = new RegExp(exp,"i");

    result = regex.exec(mac) ; 

    if(result != null && RegExp.leftContext.length ==0 && RegExp.rightContext.length ==0)

    {

       allRowsOfMFDBtable[i].style.display = '';
	   
    }

    else

    {

       allRowsOfMFDBtable[i].style.display = 'none';
       count++;
    }

  }
  
  count++ ;

  if(count == allRowsOfMFDBtable.length)
  {
     alert("Search for the specified MAC address: Failed to find any entries") ;
	 
	 return ;
  }

}

function fdb_search_SubmitStart()

{
}


function fdb_search_SubmitEnd()

{

}

function fdb_search_LoadStart()
{
}

 

function fdb_search_LoadEnd()

{

  registerOnClickFunction(document.forms[0].v_1_3_2[1]);

}

 

 


