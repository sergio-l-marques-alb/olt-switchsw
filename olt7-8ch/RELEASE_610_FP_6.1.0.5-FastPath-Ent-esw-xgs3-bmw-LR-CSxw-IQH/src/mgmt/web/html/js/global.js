var timeoutCount=0;
var clean=true;

function showInfo( info, period ) {

  var subMenu = document.getElementById('help');
	var style2 = document.getElementById('rightbar').style;
	style2.display = "block";
	
  if( !( ( info==null || info.length==0 ) && clean ) && subMenu.innerHTML!=null ) {
    if( ( info==null || info.length==0 ) && !clean ) clearInfo( true );
    else {
	clean=false;

  subMenu.innerHTML = info;

	timeoutCount++;

	// default: clear info after 15 seconds
	var clearAfter=15000;
	if( period!=null ) clearAfter=period;
	// 0 or -ve clearAfter and null info takes forever
	if( clearAfter>0 ) setTimeout( "clearInfo( false );", clearAfter );
	else timeoutCount=0;
    }
  }
}

function expand()
{
    var tmp = document.getElementsByTagName("input");
    for(j=0; j<=tmp.length-1; j++)
    {
        if(tmp[j].readOnly == true)
        {
            tmp[j].size = tmp[j].value.length + 20
        }
    } 
}


function clearInfo( now ) {

  var subMenu = document.getElementById('help');

  if( now ) {
      subMenu.innerHTML =""
      clean=true;
  }
  else if( timeoutCount>0 ) {
    timeoutCount--;
    if( timeoutCount==0 && !clean ) {
      subMenu.innerHTML ="Place the cursor on ? to see help"
      clean=true;
    }
  }
		var style2 = document.getElementById('rightbar').style;
	style2.display = "none";
}

var bustcachevar=1 //bust potential caching of external pages after initial request? (1=yes, 0=no)
var loadedobjects=""
var rootdomain="http://"+window.location.hostname
var bustcacheparameter=""

function ajaxpage(url, containerid){
  var page_request = false
  if (window.XMLHttpRequest) // if Mozilla, Safari etc
    page_request = new XMLHttpRequest()
  else if (window.ActiveXObject){ // if IE
        try {
              //page_request = new ActiveXObject("Msxml2.XMLHTTP")
              page_request = new ActiveXObject("Msxml2.DOMDocument.6.0")
        }
        catch (e){
                    try{
                          page_request = new ActiveXObject("Microsoft.XMLHTTP")
                        }
                    catch (e){}
        }
  }
  else
    return false
  page_request.onreadystatechange=function(){
      loadpage(page_request, containerid)
  }
  if (bustcachevar) //if bust caching of external page
    bustcacheparameter=(url.indexOf("?")!=-1)? "&"+new Date().getTime() : "?"+new Date().getTime()
  //page_request.open('GET', url+bustcacheparameter, true)
  page_request.open('GET', url, true)
  page_request.send(null)
}

function loadpage(page_request, containerid){
if (page_request.readyState == 4 && (page_request.status==200 || window.location.href.indexOf("http")==-1))
document.getElementById(containerid).innerHTML=page_request.responseText
}

function loadobjs(){
if (!parent.document.getElementById)
return
for (i=0; i<arguments.length; i++){
var file=arguments[i]
var fileref=""
if (loadedobjects.indexOf(file)==-1){ //Check to see if this object has not already been added to page before proceeding
if (file.indexOf(".js")!=-1){ //If object is a js file
fileref=parent.document.createElement('script')
fileref.setAttribute("type","text/javascript");
fileref.setAttribute("src", file);
}
else if (file.indexOf(".css")!=-1){ //If object is a css file
fileref=parent.document.createElement("link")
fileref.setAttribute("rel", "stylesheet");
fileref.setAttribute("type", "text/css");
fileref.setAttribute("href", file);
}
}
if (fileref!=""){
document.getElementsByTagName("head").item(0).appendChild(fileref)
loadedobjects+=file+" " //Remember this object as being already added to page
}
}
}

function toggleImage(obj)
{
	if(obj.firstChild.src.substring(obj.firstChild.src.length-5,obj.firstChild.src.length)=="b.gif")
	obj.firstChild.src=obj.firstChild.src.substring(0,obj.firstChild.src.length-5)+"t.gif";
	else if(obj.firstChild.src.substring(obj.firstChild.src.length-5,obj.firstChild.src.length)=="t.gif")
	obj.firstChild.src=obj.firstChild.src.substring(0,obj.firstChild.src.length-5)+"u.gif";
	else if(obj.firstChild.src.substring(obj.firstChild.src.length-5,obj.firstChild.src.length)=="u.gif")
	obj.firstChild.src=obj.firstChild.src.substring(0,obj.firstChild.src.length-5)+"b.gif";

}

function tagImage(obj,lyrid)
{
	pntelment=obj.childNodes[0];
	ln=pntelment.src.length-5
	temp=pntelment.src.substring(ln,pntelment.src.length)
	if(temp=="b.gif"){
		tabimgsrc="t.gif";
		}
	else if(temp=="t.gif"){
		tabimgsrc="u.gif";
		}
	else {
		tabimgsrc="b.gif";
	}
	prntTblid = document.getElementById(lyrid)
    
	for (i=0; i<prntTblid.rows.length; i++)
	{
		colelemnt = prntTblid.rows[i].childNodes
        
		for (j=0; j<colelemnt.length; j++)
		{
			//if(colelemnt[j].firstChild.tagName=='A')
            if(colelemnt[j].getElementsByTagName("A").length > 0)
			{
                elment=colelemnt[j].getElementsByTagName("IMG");
				elment[0].src=elment[0].src.substring(0,elment[0].src.length-5)+tabimgsrc;
                
			}
		}
	}
}
function alternateColors()
{
   var bgColor = '#C0C0C0';
   var tbl = document.getElementsByTagName('table')[0];
   for (j=0;j<tbl.rows.length;j++)
   {
       if(j%2!=0) 
       {
          tbl.rows[j].style.background= bgColor;
       }
   }
}

Array.prototype.compare = function(testArr) {
    if (this.length != testArr.length) return false;
    for (var i = 0; i < testArr.length; i++) {
        if (this[i].compare) { 
            if (!this[i].compare(testArr[i])) return false;
        }
        if (this[i] !== testArr[i]) return false;
    }
    return true;
}

function getMeURL()
{
    linkUrl = this.location.href;
    end     = linkUrl.indexOf(".htm");
    start   = linkUrl.lastIndexOf("/", end);
    currentURL = linkUrl.substring(start+1, end);
    currentURL = currentURL.concat(".html");
    return currentURL;
}
function isSelected ( tabInfo )
{
   var pathInfo = xeData.nPath;
   var cmpVar;
   var isNN = navigator.appName == "Netscape";
   var check = false; 
   if (isNN)
   {
     check = ((tabInfo[1][0] != undefined) && (tabInfo[1][0][1] != undefined));
   }
   else
   {
     check = ((tabInfo[0] != undefined) && (tabInfo[1][0] != undefined));
   }

   /*if ( (tabInfo[1][0] != undefined) && (tabInfo[1][0][1] != undefined))*/
   if(check)
   {
     cmpVar = [ tabInfo[0], tabInfo[1][1] ];
   } 
   else
   {
     cmpVar = [ tabInfo[0], tabInfo[1] ];
   }
   
   if ( typeof(glbCheckVariable) != "undefined" )
   {
      if ( glbCheckVariable != "" )
      {
        if( cmpVar[0] == glbCheckVariable)
        {
          return true;
        }
        else
        {
          var curURL = getMeURL();
          var cmpString = curURL+"?v_1_1_1="+glbCheckVariable+"&submit_target="+curURL+"&submit_flag=8";
          if ( cmpVar[1] == cmpString)
            return true;
          else
          return false;
        }
      }
   }

   for ( i=0; i < pathInfo.length; i++ )
   {
     if ( pathInfo[i].compare(cmpVar) )
     {
       return true;
     }
   }
  
   return false;
}
function loadme()
{
  var myDIV = document.getElementById('topdiv');
  //var firstChild = myDIV.getFirstChild();
  var first = myDIV.firstChild;
  var nPath = xeData.nTabInfo;
  var isNN = navigator.appName == "Netscape";
  var subExists = false;
  var check = false;
  var newTable = document.createElement("table");
  newTable.setAttribute('border',0);
  newTable.setAttribute('cellSpacing',0);
  newTable.setAttribute('cellPadding',0);
  newTable.border = 0;
  var newTBody = document.createElement("TBODY");
  var newTR = document.createElement("tr");
  for(var i=0; i < nPath.length; i++)
  {
     var tmpStr = "";
     var blankTD1 = document.createElement("td");
     if( isNN )
     {
       check = ((nPath[i][1][0] != undefined) && (nPath[i][1][0][1] != undefined));
     }
     else
     {
       check = ((nPath[i][1] != undefined) && (nPath[i][1][0] != undefined));
     }
     if ( (isSelected(nPath[i])) && (check) ) 
     {
       subExists = true;
       var innerNewTable = document.createElement("table");
       innerNewTable.setAttribute('border',0);
       innerNewTable.setAttribute('cellSpacing',0);
       innerNewTable.setAttribute('cellPadding',0);
       innerNewTable.setAttribute('width',"100%");  
       var innerNewTBody = document.createElement("TBODY");
       var innerNewTR = document.createElement("tr");
       for(var j=1; j < nPath[i].length; j++)
       {
         var tmpInnerStr = "";
         var innerblankTD1 = document.createElement("td");
         innerblankTD1.setAttribute('id',"ltab");
         innerblankTD1.innerHTML ="&nbsp";
         var innerblankTD2 = document.createElement("td");
         innerblankTD2.setAttribute('nowrap',"");
         innerblankTD2.setAttribute('id',"mtab");
         innerblankTD2.setAttribute('align',"center");
         if(isSelected(nPath[i][j]))
         { 
           tmpInnerStr = "<a style='color:#ffffff' id='tab2link' href='"+nPath[i][j][1]+"'>"+nPath[i][j][0]+"</a>";
         }
         else
         {
           tmpInnerStr = "<a style='color:#000000' id='tab2link' href='"+nPath[i][j][1]+"'>"+nPath[i][j][0]+"</a>";
         }
         innerblankTD2.innerHTML = tmpInnerStr;
         var innerblankTD3 = document.createElement("td");
         innerblankTD3.setAttribute('id',"rtab");
         innerblankTD3.setAttribute('align',"right");
         innerblankTD3.innerHTML = "<img src='/images/tab_spacer_row2.gif' width=1 height=18 border=0>";

         if(isSelected(nPath[i][j]))
         {
           innerblankTD1.setAttribute('bgColor',"#b70024");
           innerblankTD2.setAttribute('bgColor',"#b70024");
           innerblankTD3.setAttribute('bgColor',"#b70024");
         }
         else
         {
           innerblankTD1.setAttribute('bgColor',"#c0c0c0");
           innerblankTD2.setAttribute('bgColor',"#c0c0c0");
           innerblankTD3.setAttribute('bgColor',"#c0c0c0");
         }

         innerNewTR.appendChild(innerblankTD1);
         innerNewTR.appendChild(innerblankTD2);
         innerNewTR.appendChild(innerblankTD3);
       }
       innerNewTBody.appendChild(innerNewTR);
       innerNewTable.appendChild(innerNewTBody);
     }
   blankTD1.setAttribute('id',"ltab");
   blankTD1.setAttribute('background',"/images/tableft.gif");
   blankTD1.innerHTML = "<img src='/images/dot.gif' width=8 height=24 border=0>";
   var blankTD2 = document.createElement("td");
   blankTD2.setAttribute('nowrap',"");
   blankTD2.setAttribute('id',"mtab");
   
   /*if ((nPath[i][1][0] != undefined) && (nPath[i][1][0][1] != undefined))*/
   if(check)
   {
     if(isSelected(nPath[i]))
     {
       tmpStr = "<a style='color:#ffffff' id='tablink' href='"+nPath[i][1][1]+"'>"+nPath[i][0]+"</a>";
     }
     else
     {
       tmpStr = "<a style='color:#000000' id='tablink' href='"+nPath[i][1][1]+"'>"+nPath[i][0]+"</a>";
     }
   } 
   else
   {
     if(isSelected(nPath[i]))
     {
       tmpStr = "<a style='color:#ffffff' id='tablink' href='"+nPath[i][1]+"'>"+nPath[i][0]+"</a>";
     }
     else
     {
       tmpStr = "<a style='color:#000000' id='tablink' href='"+nPath[i][1]+"'>"+nPath[i][0]+"</a>";
     }
   }
    
   blankTD2.innerHTML = tmpStr;
   var blankTD3 = document.createElement("td");
   blankTD3.setAttribute('id',"rtab");
   blankTD3.setAttribute('background',"/images/tabright.gif");
   blankTD3.innerHTML = "<img src='/images/dot.gif' width=7 height=24 border=0>";

   if(isSelected(nPath[i]))
   {
     blankTD1.setAttribute('bgColor',"#b70024");
     blankTD2.setAttribute('bgColor',"#b70024");
     blankTD3.setAttribute('bgColor',"#b70024");
   }
   else
   {
     blankTD1.setAttribute('bgColor',"#c0c0c0");
     blankTD2.setAttribute('bgColor',"#c0c0c0");
     blankTD3.setAttribute('bgColor',"#c0c0c0");     
   }
   newTR.appendChild(blankTD1);
   newTR.appendChild(blankTD2);
   newTR.appendChild(blankTD3);
  }
  newTBody.appendChild(newTR);
  newTable.appendChild(newTBody);
  myDIV.insertBefore(newTable,first);
  // document.body.insertBefore(newTable,myDIV);
  if ( subExists )
  {
    // document.body.insertBefore(innerNewTable, myDIV);
    myDIV.insertBefore(innerNewTable,first);
  }
}

