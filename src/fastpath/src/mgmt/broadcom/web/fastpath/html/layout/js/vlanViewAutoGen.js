
function drawvlanview(ports, includes, tags)
{
  var drawview = document.getElementById('vlanViewDiv').innerHTML;

  var toggleHref = "<a href=\"#\" onclick=\"toggleImage(this);\">";
  var untag = "<img src=\"/images/u.gif\" name=\"imx\" width=\"22\" height=\"22\" border=\"0\" id=\"imx\" />";
  var tag = "<img src=\"/images/t.gif\" name=\"imx\" width=\"22\" height=\"22\" border=\"0\" id=\"imx\" />";
  var blank = "<img src=\"/images/b.gif\" name=\"imx\" width=\"22\" height=\"22\" border=\"0\" id=\"imx\" />";

  drawview = drawview + "<table border=0>";
  drawview = drawview + "<tr>";
  drawview = drawview + "<td>";
  drawview = drawview + "<a href=\"#\" onclick=\"tagImage(this,'vlantable"+0+"');toggleImage(this)\" >" + untag + "</a>";
  drawview = drawview + "Tag/Untag/Exclude All";
  drawview = drawview + "</td>";
  drawview = drawview + "</tr>";
  drawview = drawview + "<tr>";
  drawview = drawview + "<table border=0 class='vlanview' id='vlantable"+0+"'>";

  drawview = drawview + "<tr><td>Port<BR>&nbsp;&nbsp;</td>";
  for(j=0; j<ports.length; j++)
  {
    drawview = drawview + "<td>"+ ports[j].value + "<BR>" + toggleHref;
    if(includes[j].value == "exclude")
    {
      drawview = drawview + blank;
    }
    else if(tags[j].value == "tagged")
    {
      drawview = drawview + tag;
    }
    else if(tags[j].value == "untagged")
    {
      drawview = drawview + untag;
    }
    else
    {
      alert("UNKNOWN: " + "INC " + includes[j].value + " TAG " + tags[j].value + "\n");
    }
    drawview = drawview + "</a></td>";
    if( j % 24 == 23) 
    { 
      drawview = drawview + "</tr><tr><td>&nbsp;</td>";
    }
  }

  drawview = drawview + "</tr>";
  drawview = drawview + "</table>";
  drawview = drawview + "</tr>";
  drawview = drawview + "</table>";

  document.getElementById('vlanViewDiv').innerHTML = drawview;
}

function showvalues(ports, includes, tags)
{
  var tableObjArray = new Array();

  if( document.getElementById("vlantable"+0))
  {
    tableObjArray[0] = document.getElementById("vlantable"+0);
  }

  var allImages = tableObjArray[0].getElementsByTagName("IMG");
  for (i=0; i<allImages.length; i++)
  {
    if( allImages[i].name == "imx")
    {
      var source = allImages[i].src;
      if( source.search("/images/t.gif") != -1  )
      {    
        includes[i].value = "include";
        tags[i].value = "tagged";
      }
      else if( source.search("/images/u.gif") != -1)
      {    
        includes[i].value = "include";
        tags[i].value = "untagged";
      }
      else if( source.search("/images/b.gif") != -1)
      {    
        includes[i].value = "forbidden";
        tags[i].value = "untagged";
      }
    }
  }
}

function readHiddenObjects(ports, includes, tags)
{
  var form = document.forms[0];
  inputs = form.getElementsByTagName("input");
  if(!inputs) return;

  for( var k=0; k < inputs.length; k++)
  {
    var name = inputs.item(k).name;
    if(name.indexOf("VlanViewConfig_Interface") != -1) ports[ports.length] = inputs.item(k);
    else if(name.indexOf("VlanViewConfig_Participation") != -1) includes[includes.length] = inputs.item(k);
    else if(name.indexOf("VlanViewConfig_Tagging") != -1) tags[tags.length] = inputs.item(k);
  }
}

function preLoad_vlanViewAutoGen()
{
  var ports = new Array();
  var includes = new Array();
  var tags = new Array();

  readHiddenObjects(ports, includes, tags);
  drawvlanview(ports, includes, tags);
}

function formSubmit_vlanViewAutoGen(submitFlag)
{
  if(submitFlag == 1)
  {
    return;
  }

  var ports = new Array();
  var includes = new Array();
  var tags = new Array();

  readHiddenObjects(ports, includes, tags);
  showvalues(ports, includes, tags);
}

