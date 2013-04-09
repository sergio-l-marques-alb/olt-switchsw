String.prototype.tokenize = tokenize;

function tokenize(inputString)
  {
     var input       = inputString;
     var separator         = "|";
     var trim              = " ";
     var ignoreEmptyTokens = true;

     var array = input.split(separator);

     if(trim)
       for(var i=0; i<array.length; i++)
         {
           while(array[i].slice(0, trim.length) == trim)
             array[i] = array[i].slice(trim.length);
           while(array[i].slice(array[i].length-trim.length) == trim)
             array[i] = array[i].slice(0, array[i].length-trim.length);
         }

     var tmpArray = new Array();
     if(ignoreEmptyTokens)
       {
          for(var i=0; i<array.length; i++)
            if(array[i] != "")
              tmpArray.push(array[i]);
            else
              tmpArray.push(0);
       }
     else
       {
          tmpArray = array;
       }
     return tmpArray;
  }
