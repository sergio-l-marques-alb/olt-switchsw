function verifyMAC(val)
{
  if (val.length != 17)
  {
    if (val.length < 1)
      alert(val + "Enter MAC address.");
    else
      alert(val + " is not a valid MAC address.");
    return false;
  }
  
  var macPattern = /^(([0-9a-fA-F]){2}\:){5}([0-9a-fA-F]){2}$/;
  var macArray = val.match(macPattern);

  if (macArray == null)
  {
    alert(val + " is not a valid MAC address.");
	return false;
  }
  return true;
}

function verifyIP(val)
{
  errorString = "";

  var ipPattern = /^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$/;
  var ipArray = val.match(ipPattern);

  if (val == "0.0.0.0")
    errorString = val + ' is a special IP address and cannot be used here.';
  else if (val == "255.255.255.255")
    errorString = val + ' is a special IP address and cannot be used here.';
  if (ipArray == null)
    errorString = val + ' is not a valid IP address.';
  else
  {
    for (i=1;i<5;i++)
    {
      thisSegment = ipArray[i];
      if (thisSegment > 255)
      {
        errorString = val + ' is not a valid IP address.';
        i = 5;
      }
    }
  }

  if (errorString != "")
  {
    alert(errorString);
	return false;
  }
  return true;
}

// disable all elements for the first/base form
function disableAll()
{
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    document.forms[0].elements[i].disabled = true;
  }
}

// disable all elements for the first/base form
function enableAll()
{
  for (var i=0; i<document.forms[0].elements.length; i++)
  {
    document.forms[0].elements[i].disabled = false;
  }
}
