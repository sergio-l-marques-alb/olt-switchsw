
/* 
 * a very simple email validation checking. 
 * you can add more complex email checking if it helps  
 */
function validateEmail(email)
{
  var splitted = email.match("^(.+)@(.+)$");
  if(splitted == null) return false;
  if(splitted[1] != null )
  {
    var regexp_user=/^\"?[\w-_\.]*\"?$/;
    if(splitted[1].match(regexp_user) == null) return false;
  }
  if(splitted[2] != null)
  {
    var regexp_domain=/^[\w-\.]*\.[A-Za-z]{2,4}$/;
    if(splitted[2].match(regexp_domain) == null) 
    {
      var regexp_ip =/^\[\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\]$/;
      if(splitted[2].match(regexp_ip) == null) return false;
    }// if
    return true;
  }
  return false;
}

/* function validateData 
 *  Checks each field in a form 
 *  Called from validateForm function 
 */ 
function validateData(strValidateStr,objValue,strError) 
{ 
  var label = strError;
  var epos = strValidateStr.search("="); 
  var  command  = ""; 
  var  cmdvalue = ""; 
  if(epos >= 0) 
  { 
    command  = strValidateStr.substring(0,epos); 
    cmdvalue = strValidateStr.substr(epos+1); 
  } 
  else 
  { 
    command = strValidateStr; 
  } 

  switch(command) 
  { 
    case "req": 
      case "required": 
      { 
        if(eval(objValue.value.length) == 0) 
        { 
          strError1 = label + " : Required Field"; 
          alert(strError1); 
          return false; 
        }//if 
        break;             
      }//case required 
    case "maxlength": 
      case "maxlen": 
      { 
        if(eval(objValue.value.length) >  eval(cmdvalue)) 
        { 
          strError1 = label + " : "+cmdvalue+" characters maximum "; 
          alert(strError1); 
          return false; 
        }//if 
        break; 
      }//case maxlen 
    case "minlength": 
      case "minlen": 
      { 
        if(eval(objValue.value.length) <  eval(cmdvalue)) 
        { 
          strError1 = label + " : " + cmdvalue + " characters minimum  "; 
          alert(strError1); 
          return false;                 
        }//if 
        break; 
      }//case minlen 
    case "alnum": 
      case "alphanumeric": 
      { 
        var charpos = objValue.value.search("[^A-Za-z0-9-.]"); 
        if(objValue.value.length > 0 &&  charpos >= 0) 
        { 
          strError1 = label+" : Only alpha-numeric characters allowed ";     
          alert(strError1);                 
          return false; 
        }//if 
        break; 
      }//case alphanumeric 
    case "num": 
      case "numeric": 
      { 
        var charpos = objValue.value.search("[^0-9]"); 
        if(objValue.value.length > 0 &&  charpos >= 0) 
        { 
          strError1 = label+" : Not a valid input, value must be integer ";
          alert(strError1); 
          return false; 
        }//if 
        break;               
      }//numeric 
    case "alphabetic": 
      case "alpha": 
      { 
        var charpos = objValue.value.search("[^A-Za-z]"); 
        if(objValue.value.length > 0 &&  charpos >= 0) 
        { 
          strError1 = label+": Only alphabetic characters allowed ";     
          alert(strError1);                 
          return false; 
        }//if 
        break; 
      }//alpha 
    case "email": 
    { 
      if(!validateEmail(objValue.value)) 
      { 
        strError1 = label+" : Enter a valid Email address ";
        alert(strError1); 
        return false; 
      }//if 
      break; 
    }//case email 
    case "lt": 
      case "lessthan": 
      { 
        if(!isNaN(objValue.value))  
        {
          if(eval(objValue.value) >  eval(cmdvalue)) 
          { 
            strError1 = label + " : value should be less than "+ cmdvalue;     
            alert(strError1); 
            return false;                 
          }//if             
        }
        break; 
      }//case lessthan 
    case "gt": 
      case "greaterthan": 
      { 
        if(!isNaN(objValue.value))  
        {            
          if(eval(objValue.value) <  eval(cmdvalue)) 
          { 
            strError1 = label + " : value should be greater than "+ cmdvalue;     
            alert(strError1); 
            return false;                 
          }//if             
        }     
        break; 
      }//case greaterthan 
    case "regexp": 
    { 
      if(!objValue.value.match(cmdvalue)) 
      { 
        strError1 = label+" : Invalid characters found ";     
        alert(strError1); 
        return false;                   
      }//if 
      break; 
    }//case regexp 
    case "dontselect": 
    { 
      if(objValue.selectedIndex == null) 
      { 
        alert("BUG: dontselect command for non-select Item"); 
        return false; 
      } 
      if(objValue.selectedIndex == eval(cmdvalue)) 
      { 
        strError1 = label+" : Please Select one option "; 
        alert(strError1); 
        return false;                                   
      } 
      break; 
    }//case dontselect 

    case "ipv4": 
    { 
      if(!isIPV4(objValue.value)) 
      { 
        strError1 = label+" : Enter a valid IPv4 address ";     
        alert(strError1); 
        return false; 
      }//if 
      break; 
    }//case ipv4          
    case "ipv6": 
    { 
      if(!isIPV6(objValue.value)) 
      { 
        strError1 = label+" : Enter a valid IPV6 address ";     
        alert(strError1); 
        return false; 
      }//if 
      break; 
    }//case ipv6 
    case "mac": 
    { 
      if(!isMAC(objValue.value)) 
      { 
        strError1 = label+" : Enter a valid MAC address ";     
        alert(strError1); 
        return false; 
      }//if 
      break; 
    }//case mac         

  }//switch 
  return true; 
} 

function isMAC(strValue)
{
  var macPattern = /^([0-9a-fA-F][0-9a-fA-F]:){5}([0-9a-fA-F][0-9a-fA-F])$/;
  var macArray = strValue.match(macPattern);

  if (macArray == null)
  {
    return false;
  }
  return true;
}

function isIPV6(strValue) 
{ 
  errorString = "";

  var ipPattern = /^([0-9a-fA-F]{4}|0)(\:([0-9a-fA-F]{4}|0)){7}$/;
  var ipArray = strValue.match(ipPattern);

  if (strValue == "0000:0000:0000:0000:0000:000")
  {
    errorString = strValue + ' is a special IP address and cannot be used here.';
  }
  else if (ipArray == null)
  {
    errorString = strValue + ' is not a valid IP address.';
  }

  if (errorString != "")
  {
    return false;
  }
  return true;

}

function isIPV4(strValue) 
{ 
  errorString = "";

  var ipPattern = /^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$/;
  var ipArray = strValue.match(ipPattern);

  if (strValue == "0.0.0.0")
  {
    errorString = strValue + ' is a special IP address and cannot be used here.';
  }
  else if (strValue == "255.255.255.255")
  {
    errorString = strValue + ' is a special IP address and cannot be used here.';
  }
  if (ipArray == null)
  {
    errorString = strValue + ' is not a valid IP address.';
  }
  else
  {
    for (i=1;i<5;i++)
    {
      thisSegment = ipArray[i];
      if (thisSegment > 255)
      {
        errorString = strValue + ' is not a valid IP address.';
        i = 5;
      }
    }
  }

  if (errorString != "")
  {
    return false;
  }
  return true;

}

function validateForm(objForm,formFields,formAccess,arrObjDesc) 
{ 
  var retval = true;
  for(var itrobj=0; itrobj < arrObjDesc.length; itrobj++) 
  {
    var access =  formAccess[itrobj];
    for(var itrdesc=0; itrdesc < arrObjDesc[itrobj].length ;itrdesc++) 
    {
      var indxs = new Array();
      var tmp = 0;
      for(i = 0; i < objForm.elements.length; i++) 
      {
        presentName = objForm.elements[i].name;
        if(presentName.indexOf(formFields[itrobj]) != -1) 
        { 
          indxs[tmp]=i;
          tmp++;
        }
      }
      for(j=0; j < indxs.length; j++) 
      {
        if(objForm.elements[indxs[j]].disabled == false)
        {
          if( objForm.elements[indxs[j]].defaultValue != objForm.elements[indxs[j]].value || access != "read-write")
          {
            if(validateData(arrObjDesc[itrobj][itrdesc][0], objForm.elements[indxs[j]],arrObjDesc[itrobj][0][1]) == false) 
            { 
              objForm.elements[indxs[j]].focus();
              retval = false;
            }
          }
        }
      }
    }
  }

  return retval;
} 
