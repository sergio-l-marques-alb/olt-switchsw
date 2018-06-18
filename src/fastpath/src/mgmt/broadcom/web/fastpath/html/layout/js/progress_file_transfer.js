xuiIncludeJSFile("/scripts/globalStyleJS.js");

function progress_file_transfer_SubmitStart()
{
       globalStyleJS_SubmitStart();
}
function progress_file_transfer_SubmitEnd()
{
	return globalStyleJS_SubmitEnd();
}

function progress_file_transfer_LoadStart()
{
	globalStyleJS_LoadStart();

       var transferDirection = MygetElementByName('v_3_1_3');
       var TransferDownloadFileType = MygetElementByName('v_3_1_4');
       var TransferUploadFileType = MygetElementByName('v_3_1_5');
       var DataType = MygetElementByName('v_2_5_1');

       var transferStatus = MygetElementByName('v_1_1_1');

        if (transferStatus.value == "")
        {
            transferStatus.value = "File transfer operation completed successfully";
        }

	DataType.value = "";   
	if( transferDirection.value == "Download")
	{
            DataType.value = TransferDownloadFileType.value;
	}
	if( transferDirection.value == "Upload")
	{
            DataType.value = TransferUploadFileType.value;
	}
}

function progress_file_transfer_LoadEnd()
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
  var v_3_1_10 = MygetElementByName('v_3_1_10');

    if (v_3_1_10.value == 1) {
      window.setTimeout(upload_refresh, 5000);
    }
}

function upload_refresh()
{
  var v_3_1_10 = MygetElementByName('v_3_1_10');
  if (v_3_1_10.value == 1) {
     formSubmit(1); 
  }
}


