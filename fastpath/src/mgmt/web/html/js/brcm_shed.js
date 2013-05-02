
function myGetElementsByName(name) 
{
  var arr = new Array();
  var elem = document.getElementsByTagName("TR");

  for(i = 0; i < elem.length; i++) 
  {
    att = elem[i].getAttribute("NAME");
    if(att == name) 
    {
      arr[arr.length] = elem[i];
    }
  }

  elem = document.getElementsByTagName("TD");
  for(i = 0; i < elem.length; i++) 
  {
    att = elem[i].getAttribute("NAME");
    if(att == name) 
    {
      arr[arr.length] = elem[i];
    }
  }
  return arr;
}

function s_h_d(show,hide,disable,enable)
{
  var showTRs = new Array();
  var hideTRs = new Array();
  var disableFields = new Array();
  var enableFields = new Array();

  if (show.toString()  != "BLANK")
  {
    showTRs = tokenize(show);
    for ( var i=0; i<=showTRs.length-1; i++ )
    {
      tmp = myGetElementsByName(showTRs[i]);
      for(j=0; j<=tmp.length-1; j++)
      {
        tmp[j].style.display = "";
      }
    }
  }

  if (hide.toString()  != "BLANK")
  {
    hideTRs = tokenize(hide);
    for ( var i=0; i<=hideTRs.length-1; i++ )
    {
      tmp = myGetElementsByName(hideTRs[i]);
      for(j=0; j<=tmp.length-1; j++)
      {
        tmp[j].style.display = "none";
      }
    }
  }    

  if (disable.toString() != "BLANK")
  {
    disableFields = tokenize(disable);
    for ( var i=0; i<=disableFields.length-1; i++ )
    {
      objectID = myGetElementsByName(disableFields[i]);
      DisableOrEnable(objectID,true);
    }
  }

  if (enable.toString() != "BLANK")
  {
    enableFields = tokenize(enable);
    for ( var i=0; i<=enableFields.length-1; i++ )
    {
      objectID = myGetElementsByName(enableFields[i]);
      DisableOrEnable(objectID,false);
    }
  } 
}

function brcm_show_hide_disable_enable(show,hide,disable,enable)
{
  s_h_d(show,hide,disable,enable);
}

function DisableOrEnable(objectID,action)
{
  for ( var i=0; i<=objectID.length-1; i++ )
  {
    formInputs = objectID[i].getElementsByTagName("input");
    if(formInputs)
    {
      for (var j = 0; j < formInputs.length; j++)
      {
        formInputs.item(j).disabled = action;
      }
    }
    formInputs = objectID[i].getElementsByTagName("select");
    if(formInputs)
    {
      for (var j = 0; j < formInputs.length; j++)
      {
        formInputs.item(j).disabled = action;
      }
    }  
    formInputs = objectID[i].getElementsByTagName("textarea");
    if(formInputs)
    {
      for (var j = 0; j < formInputs.length; j++)
      {
        formInputs.item(j).disabled = action;
      }
    }
    formInputs = objectID[i].getElementsByTagName("button");
    if(formInputs)
    {
      for (var j = 0; j < formInputs.length; j++)
      {
        formInputs.item(j).disabled = action;
      }
    }
  }    
}

function brcm_hide_setdisable(hide)
{
  tmpArray = new Array();
  hideTRs = tokenize(hide);

  for ( var i=0; i<=hideTRs.length-1; i++ )
  {
    tmp = myGetElementsByName(hideTRs[i]);
    for(j=0; j<=tmp.length-1; j++)
    {
      if(tmp[j].style.display == "none")
      {
        tmpArray[0] = tmp[j];
        DisableOrEnable(tmpArray,true);
      }
    }
  }
}
