function DownloadFile_SubmitStart()
{
}
function DownloadFile_SubmitEnd()
{
}

function DownloadFile_LoadStart()
{
}

function DownloadFile_LoadEnd()
{
  check_status();
}

function check_status()
{
  var e_1_19_1 = getInputElementByXid('1_19_1');

    if (e_1_19_1.value == 1) {
      window.setTimeout(download_refresh, 3000);
    }
}

function download_refresh()
{
  var e_1_19_1 = getInputElementByXid('1_19_1');
  var e_1_20_1 = getInputElementByXid('1_20_1');

  if (e_1_19_1.value == 1) {
    e_1_20_1.nextSibling.click();
  }
}

function getInputElementByXid(xid){

    var all_elements = document.getElementsByTagName('input') ;

    for(var index=0;index<all_elements.length;index++){
        var element = all_elements[index];
        if( element.getAttribute('xid') == null ||
            element.getAttribute('xid') != xid) continue;

        return element;
    }
}


