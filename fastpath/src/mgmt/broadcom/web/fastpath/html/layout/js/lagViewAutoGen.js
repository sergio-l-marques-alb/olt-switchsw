
var maxLagsPossible = 6;
var lagTable;

function toggleLacp(obj,unit,port)
{
  if(obj.firstChild.src.substring(obj.firstChild.src.length-5,obj.firstChild.src.length)=="b.gif")
  {
    portLacpMode[unit][port] = 1;
    obj.firstChild.src=obj.firstChild.src.substring(0,obj.firstChild.src.length-5)+"l.gif";
  }
  else if(obj.firstChild.src.substring(obj.firstChild.src.length-5,obj.firstChild.src.length)=="l.gif")
  {
    portLacpMode[unit][port] = 0;
    obj.firstChild.src=obj.firstChild.src.substring(0,obj.firstChild.src.length-5)+"b.gif";
  }
}

function toggleLag(obj,unit,port)
{
  var currLagId = obj.firstChild.src.substring(obj.firstChild.src.length-5,obj.firstChild.src.length-4);
  lagTable[0] = 1; /* Assume that lag0 which means not a member of any lag is always empty */

  for (i=0;i<totalLags;i++)
  {
    if (availableLags[i] == currLagId)
     break;
  }

  for (j=i+1;;j++)
  {
    if (j >= totalLags)
    {
      j = 0;
      slot = 0;
      break;
    }

    if (lagTable[availableLags[j]] < maxLagMemberCount)
    {
      slot =  availableLags[j];
      break;
    }
  }

  lagTable[currLagId]--;
  lagTable[slot]++;
  portLagMembership[unit][port] = slot;
  obj.firstChild.src=obj.firstChild.src.substring(0,obj.firstChild.src.length-5)+slot+".gif";
  return;
}

function drawlagview(ports, memberships, lacpModes)
{
  var drawview = document.getElementById('lagViewDiv').innerHTML;

  var toggleHref = "<a href=\"#\" onclick=\"toggleImage(this);\">";

  for (i=0; i <=maxLagsPossible; i++)
  {
    lagTable[i]=0;
  }

  drawview = drawview + "<table border=0>";
  drawview = drawview + "<tr>";
  drawview = drawview + "<table border=0 class='lagview' id='lagtable"+i+"'>";

  /* LAG Membership */
  drawview = drawview + "<tr><td align=left valign=Top>Port<BR>LAG<BR><BR>LACP</td>";
  for(j=0; j<ports.length; j++)
  {
    toggleHref =  "<a href=\"#\" onclick=\"toggleLag(this,"+0+","+j+");\">";
    drawview = drawview + "<td>"+ (j+1) + "<BR>" + toggleHref;
    drawview = drawview + "<img src=\"/images/"+memberships[j].value+".gif\" name=\"imx\" width=\"22\" height=\"22\" border=\"0\" id=\"imx\" />";
    lagTable[memberships[j].value]++;

    drawview = drawview + "</a>";

    toggleHref =  "<a href=\"#\" onclick=\"toggleLacp(this,"+0+","+j+");\">";
    drawview = drawview + "<BR>" + toggleHref;
    if (lacpModes[j].value == "Enable")
    {
      drawview = drawview + "<img src=\"/images/l.gif\" name=\"imx\" width=\"22\" height=\"22\" border=\"0\" id=\"imx\" />";
    }
    else
    {
      drawview = drawview + "<img src=\"/images/b.gif\" name=\"imx\" width=\"22\" height=\"22\" border=\"0\" id=\"imx\" />";
    }


    drawview = drawview + "</a></td>";
    if( j % 24 == 23 && j != ports.length-1)
    {
      drawview = drawview + "</tr><BR><tr><td>Port<BR>LAG<BR><BR>LACP</td>";
    }
  }


  drawview = drawview + "</tr>";

  drawview = drawview + "</table>";
  drawview = drawview + "</tr>";
  drawview = drawview + "</table>";

  document.getElementById('lagViewDiv').innerHTML = drawview;
}

function showvalues(ports, memberships, lacpModes)
{
  var tableObjArray = new Array();

  if( document.getElementById("lagtable"+0))
  {
    tableObjArray[0] = document.getElementById("lagtable"+0);
  }

  var allImages = tableObjArray[0].getElementsByTagName("IMG");
  for (i=0; i<allImages.length; i++)
  {
    if( allImages[i].name == "imx")
    {
      var source = allImages[i].src;
      if( source.search("/images/t.gif") != -1  )
      {    
        memberships[i].value = "include";
        lacpModes[i].value = "tagged";
      }
      else if( source.search("/images/u.gif") != -1)
      {    
        memberships[i].value = "include";
        lacpModes[i].value = "untagged";
      }
      else if( source.search("/images/b.gif") != -1)
      {    
        memberships[i].value = "forbidden";
        lacpModes[i].value = "untagged";
      }
    }
  }
}

function readHiddenObjects(ports, memberships, lacpModes)
{
  var form = document.forms[0];
  inputs = form.getElementsByTagName("input");
  if(!inputs) return;

  for( var k=0; k < inputs.length; k++)
  {
    var name = inputs.item(k).name;
    if(name.indexOf("LagViewConfig_Interface") != -1) ports[ports.length] = inputs.item(k);
    else if(name.indexOf("LagViewConfig_Membership") != -1) memberships[memberships.length] = inputs.item(k);
    else if(name.indexOf("LagViewConfig_LacpMode") != -1) lacpModes[lacpModes.length] = inputs.item(k);
  }
}

function preLoad_lagViewAutoGen()
{
  var ports = new Array();
  var memberships = new Array();
  var lacpModes = new Array();
  lagTable = new Array();

  readHiddenObjects(ports, memberships, lacpModes);
  drawlagview(ports, memberships, lacpModes);
}

function formSubmit_lagViewAutoGen(submitFlag)
{
  if(submitFlag == 1)
  {
    return;
  }

  var ports = new Array();
  var memberships = new Array();
  var lacpModes = new Array();
  lagTable = new Array();

  readHiddenObjects(ports, memberships, lacpModes);
  showvalues(ports, memberships, lacpModes);
}

