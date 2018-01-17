xuiIncludeJSFile("/scripts/globalStyleJS.js");

function transferProgress_SubmitStart()
{
        return globalStyleJS_SubmitStart();
}
function transferProgress_SubmitEnd()
{
        return globalStyleJS_SubmitEnd();
}

function transferProgress_LoadStart()
{
        globalStyleJS_LoadStart();
}

function transferProgress_LoadEnd()
{
        globalStyleJS_LoadEnd();
        check_status();
}

function MygetElementByName(id)
{
  var all_elements = document.getElementsByTagName('input') ;
   for(var index=0;index<all_elements.length;index++){
      var element = all_elements[index];
      if( element.name != id) continue;
      return element;
   }
  return null;
}

function check_status()
{
  var v_2_1_1 = MygetElementByName('v_2_1_1');

    if (v_2_1_1.value == 1) {
      window.setTimeout(upload_refresh, 5000);
    }
}

function upload_refresh()
{
  var v_2_1_1 = MygetElementByName('v_2_1_1');
  if (v_2_1_1.value == 1) {
     formSubmit(1);
  }
}

