
//
// COMMON FUNCTIONS
//

function brcm_strip_iter_prefix(name)
{
  var parts = name.split(".");
  return parts[parts.length - 1];
}

function brcm_get_iter_prefix(name)
{
  var retString = "";
  var parts = name.split(".");

  for(var i = 0; i < parts.length - 1; i++)
  {
    if(i != 0) retString = retString + ".";
    retString = retString + parts[i];
  }
  return retString;
}

function brcm_getby_tag_class(tag, className)
{
  var retArray = new Array();
  var inputs = document.getElementsByTagName(tag);
  if(!inputs) return retArray;

  for(var i = 0; i < inputs.length; i++)
  {
    if(inputs[i].className == className)
    {
      retArray[retArray.length] = inputs[i];
    }
  }

  return retArray;
}

function brcm_getby_tag_and_name(tag, name, strip)
{
  var retArray = new Array();
  var inputs = document.getElementsByTagName(tag);
  if(!inputs) return retArray;

  for(var i = 0; i < inputs.length; i++)
  {
    var lname = inputs[i].name;
    if(strip == true) 
      lname = brcm_strip_iter_prefix(lname);

    if(lname == name)
    {
      retArray[retArray.length] = inputs[i];
    }
  }
  return retArray;
}


