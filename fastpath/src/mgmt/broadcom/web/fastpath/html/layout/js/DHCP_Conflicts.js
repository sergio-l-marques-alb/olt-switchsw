function registerOnClickFunction(obj)

{

  if(obj == undefined) {

    alert("registerOnClickFunction : failed");

    return;

  }

  obj.onclick = searchDHCPIP;

}


function validateDHCPIP(IP)

{

  var result = 0;

  var exp = "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$" ;
  
  var regex = new RegExp(exp);
      
  result = regex.exec(IP) ;
        
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

 

function searchDHCPIP()

{

 var tblObj = document.getElementsByTagName('table');

 var inputs = tblObj[1].getElementsByTagName('input');

 var IP = inputs[2].value;

 


 

 var valid = validateDHCPIP(IP);

 

 if (valid == 1)

 {

   alert("Enter IP Address separated by colons"

          + '\n' + "in the format X.X.X.X");

   return;

 }

 

 var allRowsOfDHCPIPtable =  tblObj[2].getElementsByTagName('tr');
 
 var count = 0;

 for (var i = 1; i < allRowsOfDHCPIPtable.length; i++)

  {

    var tmpInputs = allRowsOfDHCPIPtable[i].getElementsByTagName('input');

    var tmpIP = tmpInputs[0].value;
    var exp =   tmpIP;
    var regex = new RegExp(exp,"i");

    result = regex.exec(IP) ; 

    if(result != null && RegExp.leftContext.length ==0 && RegExp.rightContext.length ==0)

    {

       allRowsOfDHCPIPtable[i].style.display = '';
	   
    }

    else

    {

       allRowsOfDHCPIPtable[i].style.display = 'none';
       count++;
    }

  }
  
  count++ ;

  if(count == allRowsOfDHCPIPtable.length)
  {
     alert("Search for the specified IP address: Failed to find any entries") ;
	 
	 return ;
  }

}

function DHCP_Conflicts_SubmitStart()

{
}


function DHCP_Conflicts_SubmitEnd()

{

}

function DHCP_Conflicts_LoadStart()
{
}

 

function DHCP_Conflicts_LoadEnd()

{

  registerOnClickFunction(document.forms[0].v_1_3_1[1]);

}

 

 


