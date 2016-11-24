if (/msie/i.test (navigator.userAgent)) //only override IE
{
  document.nativeGetElementById = document.getElementById;
  document.getElementById = function(id)
  {
    var elem = document.nativeGetElementById(id);
    if(elem)
    {
      //make sure that it is a valid match on id
      if(elem.attributes['id'].value == id)
      {
        return elem;
      }
      else
      {
        //otherwise find the correct element
        for(var i=1;i<document.all[id].length;i++)
        {
          if(document.all[id][i].attributes['id'].value == id)
          {
            return document.all[id][i];
          }
        }
      }
    }
    return null;
  };
}

function alternateColors()
{
   var bgColor = '#C0C0C0';
   var divs = document.getElementsByTagName('div');
   for (i=0; i< divs.length; i++)
   {
     if (divs[i].id == 'summary')
     {
        var tbl = divs[i].getElementsByTagName('table');
        for (j=0;j<tbl[0].rows.length;j++)
        {
           if(j%2!=0) 
           {
              tbl[0].rows[j].style.background= bgColor;
           }
        }
     }
   }
} 

function globalStyleJS_SubmitStart()
{
 
}
function globalStyleJS_SubmitEnd()
{
 
}
 
function customHelp()
{
 
}
 
function globalStyleJS_LoadStart()
{
 
}
 
function globalStyleJS_LoadEnd()
{
}



