xuiIncludeJSFile("/scripts/globalStyleJS.js");

function http_file_download_SubmitStart()
{
        return globalStyleJS_SubmitStart();
}
function http_file_download_SubmitEnd()
{
        return globalStyleJS_SubmitEnd();
}

function http_file_download_LoadStart()
{
        globalStyleJS_LoadStart();
}

function http_file_download_LoadEnd()
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

    if (v_2_1_1.value == "1") {
      window.setTimeout(upload_refresh, 5000);
    }
}

function upload_refresh()
{
  var v_2_1_1 = MygetElementByName('v_2_1_1');
  if (v_2_1_1.value == "1") {
     document.forms[0].submit();
  }
}

