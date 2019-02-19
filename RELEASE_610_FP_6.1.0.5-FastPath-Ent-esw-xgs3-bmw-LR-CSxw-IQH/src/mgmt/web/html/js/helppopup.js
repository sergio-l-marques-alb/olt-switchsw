
function displayHelp()
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