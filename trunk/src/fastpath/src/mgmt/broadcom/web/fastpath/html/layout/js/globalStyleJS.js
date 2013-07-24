
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
  var images = document.getElementsByTagName('img') ;

  for(var index=0;index<images.length;index++)
  {
      var imgSrc = images[index].getAttribute('src');
      if(imgSrc.indexOf("images/help.gif") != -1)
      {
         imgSrc = "/images/help.gif";
         images[index].src = imgSrc;  
      } 
  }
}
 
function globalStyleJS_LoadEnd()
{
}



